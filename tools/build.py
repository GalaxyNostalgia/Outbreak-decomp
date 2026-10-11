#!/usr/bin/env python3
"""Rebuild a game executable from splat's output and check it is byte-identical.

    python -I tools/build.py f2            # build + verify SLPM_656.92
    python -I tools/build.py f1 f2         # both games
    python -I tools/build.py all

Phase 1 (now): every subsegment is assembled from asm/<ELF>/ and linked at its
original address; the ELF header and the non-loaded tail (section headers,
symbol tables) are copied from orig/. A matching result proves the split,
the $gp value and the toolchain are right.

Phase 2 (once the compiler is pinned): src/<ELF>/**.c functions replace their
asm. The splice point is marked `PHASE2` below.

Needs GNU binutils for MIPS on PATH (mips-linux-gnu-as/ld/objcopy, or set
CROSS=mips-ps2-elf- / mipsel-linux-gnu- etc.).
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
    r = subprocess.run(cmd, cwd=ROOT, capture_output=True, text=True)
    if r.returncode:
        sys.stderr.write(r.stdout + r.stderr)
        sys.exit(f"failed: {' '.join(map(str, cmd))}")
    return r.stdout


def subsegment_objects(elf, cfg):
    """[(load_index, vram, [(asm_path, section)], bss_list)] in file order."""
    asm = ROOT / "asm" / elf
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
            off = start - seg["start"]      # byte offset inside the output section
            if kind == "asm":
                objs.append((asm / f"{name}.s", ".text", off))
            elif kind in ("data", "rodata", "sdata"):
                objs.append((asm / "data" / f"{name}.{kind}.s", f".{kind}", off))
            elif kind in ("bss", "sbss"):
                bss.append((asm / "data" / f"{name}.{kind}.s", f".{kind}", sub.get("vram")))
            else:
                sys.exit(f"{elf}: subsegment type {kind} not handled by build.py yet")
        out.append((seg["name"], seg["vram"], seg["start"], objs, bss))
    return out


def assemble(src, b):
    obj = b / "obj" / src.relative_to(ROOT).with_suffix(".o")
    obj.parent.mkdir(parents=True, exist_ok=True)
    if not obj.exists() or obj.stat().st_mtime < src.stat().st_mtime:
        run([f"{CROSS}as", *AS_FLAGS, "-o", obj, src])
    return obj


def build(key, versions, keep_going):
    ver = versions[key]
    elf = ver["elf"]
    cfgp = ROOT / "config" / elf / "splat.yaml"
    orig = ROOT / "orig" / elf / elf
    if not cfgp.exists() or not orig.exists():
        sys.exit(f"[{key}] run `python configure.py {key}` first")
    if not (ROOT / "asm" / elf).exists():
        sys.exit(f"[{key}] asm/{elf} missing: run `python configure.py {key}`")
    cfg = yaml.safe_load(cfgp.read_text())
    b = ROOT / "build" / elf
    b.mkdir(parents=True, exist_ok=True)
    loads = subsegment_objects(elf, cfg)

    srcs = [s for _, _, _, objs, _ in loads for s, _, _ in objs] + [s for *_, bss in loads for s, _, _ in bss]
    missing = [s for s in srcs if not s.exists()]
    if missing:
        sys.exit(f"[{key}] splat did not produce: {', '.join(str(m.relative_to(ROOT)) for m in missing[:5])}")
    with ThreadPoolExecutor() as ex:
        objmap = dict(zip(srcs, ex.map(lambda s: assemble(s, b), srcs)))

    # PHASE2: compile src/<elf>/**.c, drop the matching functions from the asm, add objects here.

    ld = ["SECTIONS", "{"]
    if cfg["options"].get("gp_value") is not None:
        ld.append(f"    _gp = {cfg['options']['gp_value']:#x};")
    for name, vram, _, objs, bss in loads:
        # SUBALIGN(1): every piece is pinned to its original offset below, so the
        # 16-byte section alignment gas gives MIPS objects must not move it
        ld.append(f"    .{name} {vram:#x} : SUBALIGN(1) {{")
        for s, sec, off in objs:
            # pin every piece: gas aligns MIPS sections to 16, and a piece that grew
            # makes ld fail ("cannot move location counter backwards") instead of shifting
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
    for auto in ("undefined_syms_auto.txt", "undefined_funcs_auto.txt"):
        if (b / auto).exists():
            extra += ["-T", str((b / auto).relative_to(ROOT))]
    linked = b / f"{elf}.linked.elf"
    run([f"{CROSS}ld", "-EL", *extra, "-T", script.relative_to(ROOT), "-Map", b / f"{elf}.map",
         "--no-check-sections", "-o", linked])

    data = orig.read_bytes()
    out = bytearray(data)          # header and tail stay as in the original (not code)
    first = min(start for _, _, start, _, _ in loads)
    out[first:] = bytes(len(data) - first)
    for i, (name, vram, start, _, _) in enumerate(loads):
        nxt = loads[i + 1][2] if i + 1 < len(loads) else None
        binf = b / f"{name}.bin"
        run([f"{CROSS}objcopy", "-O", "binary", "-j", f".{name}", linked, binf])
        blob = binf.read_bytes()
        if nxt is not None and start + len(blob) > nxt:
            sys.exit(f"[{key}] {name} is {len(blob):#x} bytes and overruns the next segment")
        out[start:start + len(blob)] = blob
    tail_seg = next((s for s in cfg["segments"] if isinstance(s, dict) and s.get("name") == "elf_tail"), None)
    if tail_seg:
        t = tail_seg["start"]
        out[t:] = data[t:]
    rebuilt = b / elf
    rebuilt.write_bytes(out)

    if out == data:
        print(f"[{key}] {elf}: OK  sha1 {hashlib.sha1(out).hexdigest()}  ({len(out):,} bytes)")
        return True
    diff = next(i for i in range(len(data)) if out[i] != data[i])
    where = ""
    for name, vram, start, _, _ in loads:
        if diff >= start:
            where = f" (in {name}, vram {vram + diff - start:#x})"
    print(f"[{key}] {elf}: MISMATCH at file offset {diff:#x}{where}; see build/{elf}/{elf}.map")
    if not keep_going:
        sys.exit(1)
    return False


def main():
    versions = json.loads((ROOT / "config/versions.json").read_text())
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("versions", nargs="*", default=["all"], help=f"{', '.join(versions)} or all")
    ap.add_argument("--keep-going", action="store_true")
    a = ap.parse_args()
    keys = list(versions) if "all" in a.versions else a.versions
    keys = [k for k in keys if "all" not in a.versions or (ROOT / "config" / versions[k]["elf"] / "splat.yaml").exists()]
    ok = all([build(k, versions, a.keep_going) for k in keys])
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
