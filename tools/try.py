#!/usr/bin/env python3
"""Compile a C file with the game's compiler and compare each function against the original.

    python -I tools/try.py f1.game scratch.c                 # default compiler + flags
    python -I tools/try.py f1.game scratch.c --flags "-O4,p" --cc mwcps2-2.4-001213 -v

Functions are matched by name: `func_005B47E0` in the C file is compared with the function
splat named func_005B47E0 in asm/<ELF>/<target>/text.s. Relocated fields (jal targets,
%hi/%lo, gp offsets) are masked on both sides, so a function that differs only by
addresses still counts as a match. Exit code 0 when every function matches.
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
DEFAULT_CC = "mwcps2-2.4-001213"       # its output carries .comment "MW MIPS C Compiler (2.4.1.01)", same as the games
DEFAULT_FLAGS = "-O4,p"
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
        while len(w) > 1 and w[-1] == 0 and w[-2] != 0x03E00008 and (w[-2] >> 26) not in (2, 3):
            w.pop()
    return out


def mask(w):
    op = w >> 26
    if op in (2, 3):                         # j / jal
        return w & 0xFC000000
    if op in (0x0F,) or (op not in (0, 0x1C, 0x10, 0x11, 0x12, 0x13) and ((w >> 21) & 31) in (1, 28)):
        return w & 0xFFFF0000                # lui, or a load/store/addiu through $at or $gp
    return w


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
    """{name: [words]} from an MW object (one .text section per function)."""
    d = obj.read_bytes()
    shoff, = struct.unpack_from("<I", d, 0x20)
    shentsize, shnum, shstrndx = struct.unpack_from("<HHH", d, 0x2E)
    sh = [struct.unpack_from("<10I", d, shoff + i * shentsize) for i in range(shnum)]
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
            out[name] = list(struct.unpack(f"<{len(body) // 4}I", body))
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("target", help="f1.game, f2.main, ...")
    ap.add_argument("src", type=Path)
    ap.add_argument("--cc", default=DEFAULT_CC)
    ap.add_argument("--flags", default=DEFAULT_FLAGS)
    ap.add_argument("-v", "--verbose", action="store_true", help="show both sides of non-matching functions")
    a = ap.parse_args()
    versions = json.loads((ROOT / "config/versions.json").read_text())
    key, _, t = a.target.partition(".")
    en = key.endswith("en")
    elf = versions[key[:-2] if en else key]["elf"]
    asm = ROOT / "asm" / elf / ("en/" + t if en else t) / "text.s"
    orig = original_functions(asm)
    obj = ROOT / "build" / "try.o"
    obj.parent.mkdir(exist_ok=True)
    compile_c(a.src.resolve(), a.cc, a.flags, obj)
    mine = object_functions(obj)
    ok = True
    for name, words in mine.items():
        if name not in orig:
            print(f"  ?  {name}: no such function in {a.target}")
            ok = False
            continue
        want = orig[name]
        same = len(words) == len(want) and all(mask(x) == mask(y) for x, y in zip(words, want))
        ok &= same
        diff = sum(mask(x) != mask(y) for x, y in zip(words, want)) + abs(len(words) - len(want))
        print(f"  {'OK' if same else 'XX'} {name}: {len(words)} vs {len(want)} instructions"
              + ("" if same else f", {diff} differ"))
        if not same and a.verbose:
            for i in range(max(len(words), len(want))):
                x = f"{words[i]:08x}" if i < len(words) else "--------"
                y = f"{want[i]:08x}" if i < len(want) else "--------"
                flag = "" if i < min(len(words), len(want)) and mask(words[i]) == mask(want[i]) else "  <"
                print(f"      {i * 4:4x}  mine {x}  orig {y}{flag}")
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
