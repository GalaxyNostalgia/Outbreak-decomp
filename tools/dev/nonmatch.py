"""nonmatch.py <file.c> <func> "<note>": wrap a function definition in #ifdef NON_MATCHING."""
import re, sys
from pathlib import Path
cfile, func, note = sys.argv[1:4]
src = Path(cfile).read_text()
m = re.search(r"^(?:static )?[A-Za-z_][^\n;{}#=()]*\b" + func + r"\s*\([^;{]*\)\s*\{", src, re.M)
i = src.index("{", m.start()); depth = 0
while True:
    if src[i] == "{": depth += 1
    elif src[i] == "}":
        depth -= 1
        if depth == 0: break
    i += 1
block = src[m.start():i + 1]
if "#ifdef NON_MATCHING" in src[max(0, m.start() - 120):m.start()]:
    sys.exit(f"{func} already NON_MATCHING")
sig = re.search(r"[A-Za-z_][^\n;{}#=()]*\b" + func + r"\s*\([^;{]*\)", block).group(0).strip()
proto = "" if re.search(r"(?m)^[A-Za-z_][^\n;{}=()]*\b" + func + r"\s*\([^;{]*\);", src) else sig + ";\n"
if sig.startswith("static"):
    # an NM function is taken from the asm in the matching build, so it can't be static
    block = block.replace("static ", "", 1)
    sig = sig[len("static "):]
    src = re.sub(r"^static ([^\n]*\b" + func + r"\s*\()", r"\1", src, flags=re.M)
    m = re.search(r"^[A-Za-z_][^\n;{}#=()]*\b" + func + r"\s*\([^;{]*\)\s*\{", src, re.M)
    i = src.index("{", m.start()); depth = 0
    while True:
        if src[i] == "{": depth += 1
        elif src[i] == "}":
            depth -= 1
            if depth == 0: break
        i += 1
    block = src[m.start():i + 1]
    proto = "" if re.search(r"(?m)^[A-Za-z_][^\n;{}=()]*\b" + func + r"\s*\([^;{]*\);", src) else sig + ";\n"
src = src[:m.start()] + proto + f"#ifdef NON_MATCHING   /* {note} */\n" + block + "\n#endif" + src[i + 1:]
Path(cfile).write_text(src)
print("wrapped", func)
