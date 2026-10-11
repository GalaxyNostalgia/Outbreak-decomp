#!/usr/bin/env python3
"""First-draft C for a function, with m2c.

    python -I tools/draft.py f1.main func_001B9070
    python -I tools/draft.py f1.game func_005F37D0 --m2c path/to/m2c

Cuts the function out of asm/<ELF>/<target>/text.s, adds the macros m2c needs and runs
m2c (https://github.com/matt-kempster/m2c, cloned next to the repo or given with --m2c).
Also lists what the function calls and which data it touches, with the names we already
know, so the draft can be cleaned up quickly.
"""
import argparse
import json
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
PRELUDE = """.set noat
.set noreorder
.set gp=64
.macro glabel label
    .global \\label
    \\label:
.endm
.macro jlabel label
    \\label:
.endm
.macro dlabel label
    \\label:
.endm
.macro alabel label
    \\label:
.endm
.macro nonmatching label, size=1
.endm
.macro endlabel label
.endm
.section .text
"""


def cut(text_s, name):
    src = text_s.read_text()
    for part in re.split(r"(?m)^(?=nonmatching )", src)[1:]:
        if re.search(rf"^glabel {re.escape(name)}\s*$", part, re.M):
            return part
    sys.exit(f"{name} not found in {text_s}")


def jump_tables(asm_dir, body):
    """The function's jump tables from the data asm, with entries pointing at its labels (m2c needs them)."""
    names = sorted(set(re.findall(r"\b(jtbl_[0-9A-F]+)\b", body)))
    if not names:
        return ""
    out = ["", ".section .rodata"]
    data = "".join(p.read_text() for p in sorted(asm_dir.glob("data/*.s")))
    for name in names:
        m = re.search(rf"(?ms)^dlabel {name}\n(.*?)^enddlabel", data)
        if not m:
            continue
        out.append(f"glabel {name}")
        for w in re.findall(r"\.word 0x([0-9A-F]{8})", m.group(1)):
            if int(w, 16) == 0:
                break
            out.append(f"    .word .L{w}")
    return "\n".join(out) + "\n"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("target")
    ap.add_argument("function")
    ap.add_argument("--m2c", type=Path, default=ROOT.parent / "m2c" / "m2c.py")
    a = ap.parse_args()
    versions = json.loads((ROOT / "config/versions.json").read_text())
    key, _, t = a.target.partition(".")
    elf = versions[key]["elf"]
    body = cut(ROOT / "asm" / elf / t / "text.s", a.function)
    tables = jump_tables(ROOT / "asm" / elf / t, body)
    for target in sorted(set(re.findall(r"\.word \.L([0-9A-F]{8})", tables))):
        if not re.search(rf"^\s*\.L{target}:", body, re.M):
            body = re.sub(rf"(?m)^(\s*/\* [0-9A-F]+ {target} )", rf".L{target}:\n\1", body, count=1)
    body = re.sub(r"/\*.*?\*/", "", body)
    calls = sorted(set(re.findall(r"\bjal\s+(\w+)", body)))
    data = sorted(set(re.findall(r"\b(D_[0-9A-F]+)\b", body)))
    print(f"/* {a.function}: calls {', '.join(calls) or 'nothing'}; data {', '.join(data) or 'none'} */")
    if not a.m2c.exists():
        sys.exit(f"m2c not found at {a.m2c}: git clone https://github.com/matt-kempster/m2c next to the repo")
    with tempfile.NamedTemporaryFile("w", suffix=".s", delete=False) as f:
        f.write(PRELUDE + body + tables)
        path = f.name
    r = subprocess.run([sys.executable, str(a.m2c), "--target", "mips-mwcc-c", "--valid-syntax", path],
                       capture_output=True, text=True)
    sys.stdout.write(r.stdout)
    if r.returncode:
        sys.stderr.write(r.stderr[-2000:])
        sys.exit(r.returncode)


if __name__ == "__main__":
    main()
