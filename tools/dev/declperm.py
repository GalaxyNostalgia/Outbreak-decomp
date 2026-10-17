"""declperm.py <target> <file.c> <func>: try every order of the function's leading declarations; apply the first match."""
import itertools, re, subprocess, sys
from pathlib import Path
target, cfile, func = sys.argv[1:4]
src = Path(cfile).read_text()
m = re.search(r"^[^\n;{}]*\b" + func + r"\s*\([^;{]*\)\s*\{\n", src, re.M)
start = m.end()
lines = src[start:].split("\n")
decls = []
for l in lines:
    if re.match(r"^    [A-Za-z_][\w ]*[\w\*]+ \**\w+(\[[^\]]*\])?( = [^;]*)?;$", l) and "(" not in l.split("=")[0]:
        decls.append(l)
    else:
        break
n = len(decls)
rest = "\n".join(lines[n:])
best = None
for perm in itertools.permutations(decls):
    if any("=" in d for d in perm) and list(perm) != decls:
        pass
    s = src[:start] + "\n".join(perm) + "\n" + rest
    Path(cfile + ".perm.c").write_text(s)
    r = subprocess.run(["python3", "-I", "tools/try.py", target, cfile + ".perm.c"], capture_output=True, text=True)
    line = [x for x in r.stdout.splitlines() if func in x]
    if line and " OK " in line[0]:
        Path(cfile).write_text(s)
        print("matched:", perm)
        break
    k = int(re.search(r"(\d+) differ", line[0]).group(1)) if line and "differ" in line[0] else 999
    if best is None or k < best[0]:
        best = (k, perm)
else:
    print("no match; best", best)
    if best and best[0] < 999:
        Path(cfile).write_text(src[:start] + "\n".join(best[1]) + "\n" + rest)
        print("applied best")
Path(cfile + ".perm.c").unlink(missing_ok=True)
