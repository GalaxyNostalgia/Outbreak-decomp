#!/usr/bin/env python3
"""Find functions that File 1 and File 2 share, so each is decompiled once.

    python -I tools/xmatch.py                 # summary per target
    python -I tools/xmatch.py game --list     # f2 function -> f1 function pairs for one target

Two functions "match" when their instruction sequences are identical after
masking addresses and immediates (jal targets, lui/addiu/lw offsets). That is
the same C, built at different addresses. Reads splat output in asm/.
"""
import argparse
import hashlib
import json
import re
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
LINE = re.compile(r"/\* [0-9A-F]+ ([0-9A-F]{8}) ([0-9A-F]{8}) \*/")


def functions(path):
    out, cur, words = {}, None, []
    for line in path.read_text().splitlines():
        if line.startswith("glabel "):
            if cur:
                out[cur] = words
            cur, words = line.split()[1], []
            continue
        m = LINE.search(line)
        if m and cur:
            w = int.from_bytes(bytes.fromhex(m.group(2)), "little")
            op = w >> 26
            if op in (2, 3):                 # j / jal: drop the target
                w &= 0xFC000000
            elif op not in (0, 0x1C, 0x10, 0x11, 0x12, 0x13):  # I-type: drop the immediate
                w &= 0xFFFF0000
            words.append(w)
    if cur:
        out[cur] = words
    return out


def shape(words):
    return hashlib.sha1(b"".join(w.to_bytes(4, "little") for w in words)).hexdigest()


def main():
    versions = json.loads((ROOT / "config/versions.json").read_text())
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("targets", nargs="*")
    ap.add_argument("--list", action="store_true")
    a = ap.parse_args()
    e1, e2 = versions["f1"]["elf"], versions["f2"]["elf"]
    names = ["main", *versions["f2"]["overlays"]]
    total = shared = 0
    for t in a.targets or names:
        p1, p2 = ROOT / "asm" / e1 / t / "text.s", ROOT / "asm" / e2 / t / "text.s"
        if not p2.exists():
            continue
        f2 = functions(p2)
        f1 = functions(p1) if p1.exists() else {}
        by_shape = defaultdict(list)
        for n, w in f1.items():
            by_shape[shape(w)].append(n)
        pairs = [(n, by_shape[shape(w)]) for n, w in f2.items() if shape(w) in by_shape]
        b_all = sum(4 * len(w) for w in f2.values())
        b_sh = sum(4 * len(f2[n]) for n, _ in pairs)
        total += b_all
        shared += b_sh
        print(f"{t:9} f2 {len(f2):5} functions; {len(pairs):5} also in f1 "
              f"({100 * len(pairs) / max(len(f2), 1):5.1f}% of functions, {100 * b_sh / max(b_all, 1):5.1f}% of bytes)")
        if a.list:
            for n, m in pairs:
                print(f"    {n} = {', '.join(m)}")
    if total:
        print(f"all      {100 * shared / total:5.1f}% of File 2's code bytes have the same function in File 1")


if __name__ == "__main__":
    main()
