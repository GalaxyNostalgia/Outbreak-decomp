#!/usr/bin/env python3
"""Rebuild the games' binaries from splat's output and check they're byte-identical.

    python -I tools/build.py f2              # File 2: main executable + every overlay
    python -I tools/build.py f1.game f2.game # just the game overlays
    python -I tools/build.py                 # everything configured

A target is the main ELF (`f1.main`) or one Metrowerks overlay (`f1.game`, ...).

Phase 1 (now): every subsegment is assembled from asm/<ELF>/<target>/ and linked
at its original address. Bytes outside the loaded code (ELF/overlay header, ELF
section-header tail) are copied from orig/. A matching result proves the split,
the $gp value and the toolchain are right.

Phase 2 (once the compiler is pinned): C functions replace their asm; the splice
point is marked PHASE2 below.

Needs GNU binutils for MIPS on PATH (mips-linux-gnu-as/ld/objcopy, or set CROSS).
"""
import argparse
import hashlib
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))
import mwo_sections  # noqa: E402
CROSS = os.environ.get("CROSS", "mips-linux-gnu-")
AS_FLAGS = ["-EL", "-march=r5900", "-mabi=eabi", "-G0", "-no-pad-sections", "-Iinclude"]


def run(cmd):
    r = subprocess.run([str(c) for c in cmd], cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        sys.stderr.write(r.stdout[-4000:] + r.stderr[-4000:])
        sys.exit(f"failed: {' '.join(map(str, cmd))[:300]}")
    return r.stdout


def load_segments(cfg, asm):
    """[(name, vram, file_offset, [(src, section, offset_in_segment)], [(src, section, vram)])]"""
    out = []
    for seg in cfg["segments"]:
        if not isinstance(seg, dict) or seg.get("type") != "code":
            continue
        objs, bss = [], []
        for sub in seg["subsegments"]:
            if isinstance(sub, dict):
                start, kind, name = sub["start"], sub["type"], sub.get("name")
            else:
                start, kind, name = sub[0], sub[1], (sub[2] if len(sub) > 2 else f"{sub[0]:X}")
            off = start - seg["start"]
            if kind == "asm":
                objs.append((asm / f"{name}.s", ".text", off))
            elif kind in ("data", "rodata", "sdata"):
                objs.append((asm / "data" / f"{name}.{kind}.s", f".{kind}", off))
            elif kind in ("bss", "sbss"):
                bss.append((asm / "data" / f"{name}.{kind}.s", f".{kind}", sub.get("vram")))
            else:
                sys.exit(f"subsegment type {kind} is not handled by build.py yet")
        out.append((seg["name"], seg["vram"], seg["start"], objs, bss))
    return out



def compiler_for(src, versions_key):
    """`// cc: <id> <flags>` on the first line overrides the game's default compiler."""
    first = src.read_text(errors="replace").split("\n", 1)[0]
    m = re.match(r"\s*//\s*cc:\s*(\S+)\s*(.*)$", first)
    if m:
        return m.group(1), m.group(2)
    c = VERSIONS[versions_key]["compiler"]
    return c["cc"], c["flags"]


def c_addr_syms(label):
    """PROVIDE an address for every func_XXXXXXXX / D_XXXXXX the C names, so C can use a variable or function
    that no asm of this target touches (e.g. another overlay's) without a hand-written definition."""
    key, _, target = label.partition(".")
    elf = VERSIONS[key[:-2] if key.endswith("en") else key]["elf"]
    names = set()
    words = set()
    for src in sorted((ROOT / "src" / elf / target).rglob("*.c")):
        text = src.read_text()
        names |= set(re.findall(r"\b((?:func|D)_([0-9A-F]{6,8}))\b", text))
        words |= set(re.findall(r"\b([A-Za-z_]\w*)\s*\(", text))
    out = [f"PROVIDE({n} = 0x{a});" for n, a in sorted(names)]
    # named functions of the main executable (memcpy, sprintf...) an overlay's C calls
    if target != "main":
        known = {}
        for line in (ROOT / "config" / elf / "symbol_addrs.txt").read_text().splitlines():
            m = re.match(r"\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", line)
            if m:
                known[m.group(1)] = int(m.group(2), 16)
        for w in sorted(words):
            if w in known and not re.fullmatch(r"(func|D)_[0-9A-F]+", w):
                out.append(f"PROVIDE({w} = {known[w]:#x});")
    return out


# C functions' jump tables: {func: (obj, section, size)}. Each is linked at the address of the original
# function's table (which the data asm still holds, with the same values) so the code refers to it there.
JUMP_TABLES = {}
JT_PLACED = []


def compile_sources(label, o, b):
    """Compile src/<ELF>/<target>/**.c. Returns {name: (obj, section, size)} of the functions."""
    key, _, target = label.partition(".")
    english = key.endswith("en")
    vkey = key[:-2] if english else key
    elf = VERSIONS[vkey]["elf"]
    sdir = ROOT / "src" / elf / target
    out = {}
    if not sdir.exists():
        return out
    cdir = ROOT / "tools/compilers"
    for src in sorted(sdir.rglob("*.c")):
        cc, flags = compiler_for(src, vkey)
        exe = cdir / cc / "mwccps2.exe"
        if not exe.exists():
            sys.exit(f"[{label}] compiler {cc} missing: python configure.py --compilers")
        rel = src.relative_to(ROOT)
        raw = b / "c" / (rel.as_posix().replace("/", "_") + ".raw.o")
        obj = raw.with_name(raw.name.replace(".raw.o", ".o"))
        raw.parent.mkdir(parents=True, exist_ok=True)
        stamp = obj.with_suffix(".cmd")
        cmd = f"{cc} {flags} {'-DENGLISH' if english else ''} -DVERSION_{vkey.upper()}"
        if not obj.exists() or obj.stat().st_mtime < src.stat().st_mtime or \
                not stamp.exists() or stamp.read_text() != cmd:
            env = dict(os.environ, MWCIncludes=".")
            r = subprocess.run([str(cdir / "wibo"), str(exe), "-c", *flags.split(), "-nostdinc", "-Iinclude",
                                f"-DVERSION_{vkey.upper()}", *(["-DENGLISH"] if english else []),
                                "-o", str(raw), str(rel)], cwd=ROOT, env=env, capture_output=True, text=True)
            if r.returncode:
                sys.stderr.write(r.stdout + r.stderr)
                sys.exit(f"[{label}] {rel} failed to compile")
            mwo_sections.rename(raw, obj)
            stamp.write_text(cmd)
        for name, size in mwo_sections.sections(obj):
            if name.startswith(".text."):
                out[name[6:]] = (obj, name, size)
            elif name.startswith(".rodata."):
                JUMP_TABLES[name[8:]] = (obj, name, size)
            elif name in (".data", ".rodata", ".sdata", ".bss", ".sbss") and size:
                sys.exit(f"[{label}] {rel}: C data ({name}) isn't supported yet; keep data in asm")
    return out


def split_text(text_s):
    """(header, [(addr, name, chunk_text)]) cut at splat's `nonmatching` markers."""
    src = text_s.read_text()
    parts = re.split(r"(?m)^(?=nonmatching )", src)
    pieces = []
    for p in parts[1:]:
        name = re.match(r"nonmatching (\w+)", p).group(1)
        a = re.search(r"/\* [0-9A-F]+ ([0-9A-F]{8}) ", p)
        pieces.append((int(a.group(1), 16), name, p))
    return parts[0], pieces


def splice(text_s, vram, cfuncs, b):
    """Replace C-provided functions in one asm text file. Returns ([(file, section, offset)], abs_syms)."""
    header, pieces = split_text(text_s)
    by_name = {n: a for a, n, _ in pieces}
    chosen = {}
    for name, (obj, sec, size) in cfuncs.items():
        if name in by_name:
            chosen[by_name[name]] = (obj, sec, name, size)
    if not chosen:
        return None, []
    cdir = b / "chunks"
    cdir.mkdir(parents=True, exist_ok=True)
    for old in cdir.glob(f"{text_s.stem}_*.s"):
        old.unlink()
    out, abs_syms, cur = [], [], None
    for a, n, t in pieces:
        if a in chosen:
            obj, sec, name, size = chosen[a]
            out.append((obj, sec, a - vram))
            if name in JUMP_TABLES:
                # the function's read-only data: its switch jump tables and string literals (the
                # game was built with -str readonly). It goes where the original's was: at the
                # lowest address the asm takes (%hi) that the C no longer names, i.e. that came
                # from a literal or a jump table.
                jobj, jsec, _ = JUMP_TABLES[name]
                named = mwo_sections.undefined_symbols(jobj)
                refs = sorted(int(a_, 16) for s_, a_ in
                              re.findall(r"%hi\(((?:jtbl|D)_([0-9A-F]{8}))\)", t) if s_ not in named)
                if not refs:
                    sys.exit(f"{name}: its C has read-only data (jump tables or strings) but I can't "
                             f"tell where the original's is")
                JT_PLACED.append((jobj, jsec, refs[0]))
            for lab, la in re.findall(r"(?m)^[jgd]label (\w+)\n(?:.*\n)*?\s*/\* [0-9A-F]+ ([0-9A-F]{8})", t):
                if lab != name:
                    abs_syms.append((lab, int(la, 16)))
            cur = None
            continue
        if cur is None:
            cur = [a, []]
            out.append((cur, ".text", a - vram))
        cur[1].append(t)
    files = []
    for item, sec, off in out:
        if isinstance(item, list):
            f = cdir / f"{text_s.stem}_{item[0]:08X}.s"
            f.write_text(header + "".join(item[1]))
            files.append((f, sec, off, False))
        else:
            files.append((item, sec, off, True))
    return files, abs_syms


def assemble(src, b):
    obj = b / "obj" / src.name.replace(".s", ".o")
    if src.parent.name == "data":
        obj = b / "obj" / "data" / src.name.replace(".s", ".o")
    obj.parent.mkdir(parents=True, exist_ok=True)
    if not obj.exists() or obj.stat().st_mtime < src.stat().st_mtime:
        run([f"{CROSS}as", *AS_FLAGS, "-o", obj, src])
    return obj


def build_target(label, cfgp, quiet=False):
    cfg = yaml.safe_load(cfgp.read_text())
    o = cfg["options"]
    orig, asm, b = ROOT / o["target_path"], ROOT / o["asm_path"], ROOT / o["build_path"]
    if not orig.exists() or not asm.exists():
        sys.exit(f"[{label}] missing {orig.relative_to(ROOT) if not orig.exists() else asm.relative_to(ROOT)}: "
                 "run configure.py first")
    b.mkdir(parents=True, exist_ok=True)
    loads = load_segments(cfg, asm)
    srcs = [s for *_, objs, _ in loads for s, _, _ in objs] + [s for *_, bss in loads for s, _, _ in bss]
    missing = [s for s in srcs if not s.exists()]
    if missing:
        sys.exit(f"[{label}] splat did not produce {missing[0].relative_to(ROOT)}")
    # Phase 2: C functions from src/<ELF>/<target>/ replace their asm, pinned at the same address
    JUMP_TABLES.clear()
    JT_PLACED.clear()
    cfuncs = compile_sources(label, o, b)
    abs_syms, n_c, c_bytes = [], 0, 0
    if cfuncs:
        new_loads = []
        for name, vram, start, objs, bss in loads:
            new_objs = []
            for s_, sec, off in objs:
                files = None
                if sec == ".text":
                    files, syms = splice(s_, vram + off, cfuncs, b)
                    abs_syms += syms
                if files is None:
                    new_objs.append((s_, sec, off))
                    continue
                for f, fsec, foff, is_c in files:
                    new_objs.append((f, fsec, off + foff))
                    if is_c:
                        n_c += 1
                        c_bytes += next(v[2] for v in cfuncs.values() if v[1] == fsec)
            new_loads.append((name, vram, start, new_objs, bss))
        loads = new_loads
        missing = set(cfuncs) - {sec[6:] for *_, objs, _ in loads for _, sec, _ in objs if sec.startswith(".text.")}
        if missing:
            sys.exit(f"[{label}] C functions not found in the asm: {', '.join(sorted(missing))}")
    srcs = [s_ for *_, objs, _ in loads for s_, sec, _ in objs if not sec.startswith(".text.")] + \
           [s_ for *_, bss in loads for s_, _, _ in bss]
    with ThreadPoolExecutor() as ex:
        objmap = dict(zip(srcs, ex.map(lambda s_: assemble(s_, b), srcs)))
    for *_, objs, _ in loads:
        for s_, sec, _ in objs:
            if sec.startswith(".text."):
                objmap[s_] = s_

    ld = ["SECTIONS", "{"]
    if o.get("gp_value") is not None:
        ld.append(f"    _gp = {o['gp_value']:#x};")
    for name, vram, _, objs, bss in loads:
        # SUBALIGN(1): every piece is pinned to its original offset, so the 16-byte
        # section alignment gas gives MIPS objects must not move it
        ld.append(f"    .{name} {vram:#x} : SUBALIGN(1) {{")
        for s, sec, off in objs:
            ld.append(f"        . = {off:#x};")
            ld.append(f"        {objmap[s].relative_to(ROOT)}({sec});")
        ld.append("    }")
        for s, sec, bvram in bss:
            at = f"{bvram:#x} " if bvram is not None else ""
            ld.append(f"    .{name}_bss {at}(NOLOAD) : {{ {objmap[s].relative_to(ROOT)}({sec}); }}")
    for jobj, jsec, ja in JT_PLACED:
        ld.append(f"    .jt{jsec.replace('.', '_')} {ja:#x} (NOLOAD) : {{ {jobj.relative_to(ROOT)}({jsec}); }}")
    ld += ["    /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.gnu.attributes) *(.comment) *(.mdebug*)"
           " *(.mwcats*) *(.rel.mwcats*) *(.rel.pdr) *(.note*) }",
           "}"] + [f"{lab} = {la:#x};" for lab, la in abs_syms] + c_addr_syms(label) + [""]
    script = b / "link.ld"
    script.write_text("\n".join(ld))
    extra = []
    for auto in ("undefined_syms_auto_path", "undefined_funcs_auto_path"):
        if o.get(auto) and (ROOT / o[auto]).exists():
            extra += ["-T", o[auto]]
    linked = b / "linked.elf"
    run([f"{CROSS}ld", "-EL", *extra, "-T", script.relative_to(ROOT), "-Map", b / "link.map",
         "--no-check-sections", "-o", linked])

    data = orig.read_bytes()
    out = bytearray(data)          # header and non-loaded tail stay as in the original
    first = min(start for _, _, start, _, _ in loads)
    tail = next((s["start"] for s in cfg["segments"] if isinstance(s, dict) and s.get("name") == "elf_tail"),
                len(data))
    out[first:tail] = bytes(tail - first)
    for i, (name, vram, start, _, _) in enumerate(loads):
        nxt = loads[i + 1][2] if i + 1 < len(loads) else tail
        binf = b / f"{name}.bin"
        run([f"{CROSS}objcopy", "-O", "binary", "-j", f".{name}", linked, binf])
        blob = binf.read_bytes()
        if start + len(blob) > nxt:
            sys.exit(f"[{label}] {name} is {len(blob):#x} bytes and overruns the next segment")
        out[start:start + len(blob)] = blob
    (b / Path(o["target_path"]).name).write_bytes(out)

    cnote = f"  ({n_c} functions from C, {c_bytes:,} bytes)" if n_c else ""
    if out == data:
        if not quiet:
            print(f"[{label}] OK   {hashlib.sha1(out).hexdigest()}  {len(out):>9,} bytes{cnote}")
        return True
    diff = next(i for i in range(len(data)) if out[i] != data[i])
    where = ""
    for name, vram, start, _, _ in loads:
        if diff >= start:
            where = f" ({name}, vram {vram + diff - start:#x})"
    print(f"[{label}] MISMATCH at file offset {diff:#x}{where}; see {(b / 'link.map').relative_to(ROOT)}{cnote}")
    return False


def targets(versions, wanted):
    """(label, config) for every target: <key>.<target>, plus <key>en.<target> for the English patch."""
    out = []
    for key, ver in versions.items():
        cdir = ROOT / "config" / ver["elf"]
        names = ["main", *ver.get("overlays", {})]
        for variant, suffix in ((key, ""), (f"{key}en", ".en")):
            for t in names:
                label, cfgp = f"{variant}.{t}", cdir / f"{t}{suffix}.yaml"
                if wanted and variant not in wanted and label not in wanted:
                    continue
                if suffix and cfgp.exists() and not wanted:
                    # English targets only when the user has a patched image in orig/<ELF>/english/
                    tp = yaml.safe_load(cfgp.read_text())["options"]["target_path"]
                    if not (ROOT / tp).exists():
                        continue
                if cfgp.exists() or (wanted and not suffix):
                    out.append((label, cfgp))
    known = set(versions) | {f"{k}en" for k in versions} | {l for l, _ in out}
    unknown = set(wanted) - known
    if unknown:
        sys.exit(f"unknown target(s): {', '.join(sorted(unknown))}")
    return out


VERSIONS = json.loads((ROOT / "config/versions.json").read_text())


def main():
    versions = VERSIONS
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("targets", nargs="*",
                    help="f1, f2, f1en, f2en, f1.main, f2en.game, ... (default: everything configured)")
    ap.add_argument("--keep-going", action="store_true", help="build every target even after a mismatch")
    a = ap.parse_args()
    ok = True
    for label, cfgp in targets(versions, a.targets):
        if not cfgp.exists():
            sys.exit(f"[{label}] {cfgp.relative_to(ROOT)} missing: run configure.py first")
        good = build_target(label, cfgp)
        ok &= good
        if not good and not a.keep_going:
            break
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
