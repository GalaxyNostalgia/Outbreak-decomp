#!/usr/bin/env python3
"""Compile a C file with the game's compiler and compare each function against the original.

    python -I tools/try.py f1.game scratch.c                 # default compiler + flags
    python -I tools/try.py f1.game scratch.c --flags "-O4,p" --cc mwcps2-2.4-001213 -v

Functions are matched by name: `func_005B47E0` in the C file is compared with the function
splat named func_005B47E0 in asm/<ELF>/<target>/text.s. Fields the object file relocates
(jal targets, %hi/%lo, gp offsets) are compared without their address bits, so a function
that differs only by addresses still counts as a match; tools/build.py then checks the real
bytes. Exit code 0 when every function matches.
"""
import argparse
import json
import os
import re
import struct
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
CROSS = os.environ.get("CROSS", "mips-linux-gnu-")
LINE = re.compile(r"/\* [0-9A-F]+ ([0-9A-F]{8}) ([0-9A-F]{8}) \*/")


def original_functions(asm_text):
    out, cur = {}, None
    for line in asm_text.read_text().splitlines():
        if line.startswith("glabel "):
            cur = line.split()[1]
            out[cur] = []
            continue
        m = LINE.search(line)
        if m and cur:
            out[cur].append(int.from_bytes(bytes.fromhex(m.group(2)), "little"))
    for k, w in out.items():                 # drop alignment padding after the last instruction
        while len(w) > 1 and w[-1] == 0 and not is_jump(w[-2]):
            w.pop()
    return out


def is_jump(w):
    """Instructions with a delay slot: the nop after them belongs to the function."""
    op = w >> 26
    if op in (1, 2, 3, 4, 5, 6, 7, 0x14, 0x15, 0x16, 0x17):   # REGIMM, j, jal, beq/bne/blez/bgtz(+l)
        return True
    if op == 0x11 and ((w >> 21) & 31) == 8:                  # bc1x
        return True
    return op == 0 and (w & 0x3F) in (8, 9)                   # jr, jalr


def compile_c(src, cc, flags, out):
    cdir = ROOT / "tools/compilers"
    exe = cdir / cc / "mwccps2.exe"
    if not exe.exists():
        sys.exit(f"{exe.relative_to(ROOT)} missing: python configure.py --compilers")
    env = dict(os.environ, MWCIncludes=".")
    r = subprocess.run([str(cdir / "wibo"), str(exe), "-c", *flags.split(), "-nostdinc", "-Iinclude",
                        "-o", str(out), str(src)], cwd=ROOT, env=env, capture_output=True, text=True)
    msgs = [l for l in (r.stdout + r.stderr).splitlines() if l.strip() and "MWCIncludes" not in l
            and not l.startswith("#   Environment")]
    if r.returncode:
        sys.exit("\n".join(msgs))


def object_functions(obj):
    """{name: (words, {word_index: reloc_type})} from an MW object (one .text section per function)."""
    d = obj.read_bytes()
    shoff, = struct.unpack_from("<I", d, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", d, 0x2E)
    sh = [struct.unpack_from("<10I", d, shoff + i * shentsize) for i in range(shnum)]
    relocs = {}
    for s in sh:
        if s[1] == 9:                        # SHT_REL: info = section it applies to
            for k in range(s[5] // 8):
                r_off, r_info = struct.unpack_from("<II", d, s[4] + k * 8)
                relocs.setdefault(s[7], {})[r_off // 4] = r_info & 0xFF
    out = {}
    for s in sh:
        if s[1] != 2:                        # SHT_SYMTAB
            continue
        strtab = sh[s[6]]
        for k in range(s[5] // 16):
            name_off, value, size, info, _, shndx = struct.unpack_from("<IIIBBH", d, s[4] + k * 16)
            if info & 0xF != 2 or shndx >= shnum:  # STT_FUNC
                continue
            no = strtab[4] + name_off
            name = d[no:d.index(b"\0", no)].decode()
            sec = sh[shndx]
            size = size or sec[5]
            body = d[sec[4] + value:sec[4] + value + size]
            rel = {i - value // 4: t for i, t in relocs.get(shndx, {}).items()}
            out[name] = (list(struct.unpack(f"<{len(body) // 4}I", body)), rel)
    return out


R_MIPS_26, R_MIPS_HI16, R_MIPS_LO16, R_MIPS_GPREL16 = 4, 5, 6, 7


def mask_rel(w, rtype):
    if rtype == R_MIPS_26:
        return w & 0xFC000000
    if rtype in (R_MIPS_HI16, R_MIPS_LO16, R_MIPS_GPREL16):
        return w & 0xFFFF0000
    return w


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("target", help="f1.game, f2.main, ...")
    ap.add_argument("src", type=Path)
    ap.add_argument("--cc", help="compiler id (default: the game's, from config/versions.json)")
    ap.add_argument("--flags", help="compiler flags (default: the game's)")
    ap.add_argument("-v", "--verbose", action="store_true", help="show both sides of non-matching functions")
    a = ap.parse_args()
    versions = json.loads((ROOT / "config/versions.json").read_text())
    key, _, t = a.target.partition(".")
    en = key.endswith("en")
    ver = versions[key[:-2] if en else key]
    elf = ver["elf"]
    a.cc = a.cc or ver["compiler"]["cc"]
    a.flags = a.flags or ver["compiler"]["flags"]
    asm = ROOT / "asm" / elf / ("en/" + t if en else t) / "text.s"
    orig = original_functions(asm)
    obj = ROOT / "build" / "try.o"
    obj.parent.mkdir(exist_ok=True)
    compile_c(a.src.resolve(), a.cc, f"{a.flags} -DVERSION_{key[:-2].upper() if en else key.upper()}"
              + (" -DENGLISH" if en else ""), obj)
    mine = object_functions(obj)
    ok = True
    for name, (words, rel) in mine.items():
        while len(words) > 1 and words[-1] == 0 and not is_jump(words[-2]):
            words = words[:-1]
        if name not in orig:
            print(f"  ?  {name}: no such function in {a.target}")
            ok = False
            continue
        want = orig[name]
        # relocated fields are compared without their address bits; everything else exactly
        mx = [mask_rel(w, rel[i]) if i in rel else w for i, w in enumerate(words)]
        my = [mask_rel(w, rel[i]) if i in rel else w for i, w in enumerate(want)]
        same = len(words) == len(want) and mx == my
        ok &= same
        diff = sum(x != y for x, y in zip(mx, my)) + abs(len(words) - len(want))
        print(f"  {'OK' if same else 'XX'} {name}: {len(words)} vs {len(want)} instructions"
              + ("" if same else f", {diff} differ"))
        if not same and a.verbose:
            for i in range(max(len(words), len(want))):
                x = f"{words[i]:08x}" if i < len(words) else "--------"
                y = f"{want[i]:08x}" if i < len(want) else "--------"
                flag = "" if i < min(len(mx), len(my)) and mx[i] == my[i] else "  <"
                print(f"      {i * 4:4x}  mine {x}  orig {y}{flag}")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
