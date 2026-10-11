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
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor
from pathlib import Path

import yaml

ROOT = Path(__file__).resolve().parent.parent
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
    with ThreadPoolExecutor() as ex:
        objmap = dict(zip(srcs, ex.map(lambda s: assemble(s, b), srcs)))

    # PHASE2: compile src/ C for this target, drop the matching functions from the asm, add objects here.

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
    ld += ["    /DISCARD/ : { *(.reginfo) *(.MIPS.abiflags) *(.pdr) *(.gnu.attributes) *(.comment) *(.mdebug*) }",
           "}", ""]
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

    if out == data:
        if not quiet:
            print(f"[{label}] OK   {hashlib.sha1(out).hexdigest()}  {len(out):>9,} bytes")
        return True
    diff = next(i for i in range(len(data)) if out[i] != data[i])
    where = ""
    for name, vram, start, _, _ in loads:
        if diff >= start:
            where = f" ({name}, vram {vram + diff - start:#x})"
    print(f"[{label}] MISMATCH at file offset {diff:#x}{where}; see {(b / 'link.map').relative_to(ROOT)}")
    return False


def targets(versions, wanted):
    out = []
    for key, ver in versions.items():
        names = ["main", *ver.get("overlays", {})]
        for t in names:
            label = f"{key}.{t}"
            if not wanted or key in wanted or label in wanted:
                cfgp = ROOT / "config" / ver["elf"] / f"{t}.yaml"
                if cfgp.exists() or wanted:
                    out.append((label, cfgp))
    unknown = set(wanted) - {l for l, _ in out} - set(versions)
    if unknown:
        sys.exit(f"unknown target(s): {', '.join(sorted(unknown))}")
    return out


def main():
    versions = json.loads((ROOT / "config/versions.json").read_text())
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("targets", nargs="*", help="f1, f2, f1.main, f2.game, ... (default: everything configured)")
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
