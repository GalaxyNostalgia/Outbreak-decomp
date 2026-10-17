"""show.py text.s func...: a function's instructions from the disassembly (MAXL lines, default 90)."""
import re, sys, os
path, names = sys.argv[1], sys.argv[2:]
maxl = int(os.environ.get("MAXL", 90))
txt = open(path).read()
parts = {}
for p in re.split(r"(?m)^(?=nonmatching )", txt)[1:]:
    m = re.search(r"glabel (\w+)", p)
    if m: parts[m.group(1)] = p
for n in names:
    lines = []
    for l in parts[n].splitlines():
        if re.search(r"/\* [0-9A-F]+ [0-9A-F]{8} [0-9A-F]{8} \*/", l): lines.append("   " + l.split("*/")[1].strip())
        elif l.strip().startswith(".L"): lines.append(l.strip())
    print(f"--- {n} ({sum(1 for x in lines if x.startswith('   '))} insns)"); print("\n".join(lines[:maxl]))
