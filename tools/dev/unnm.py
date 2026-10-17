"""unnm.py file.c func: remove the #ifdef NON_MATCHING / #if 1 wrapper around func's definition."""
import re, sys
p, fn = sys.argv[1:3]
s = open(p).read()
m = re.search(r'^[^\n;]*\b' + fn + r'\([^;{]*\)\s*\{', s, re.M)
d = m.start()
w = s.rfind('\n#', 0, d) + 1
line_end = s.index('\n', w)
assert s[w:line_end].startswith('#if'), s[w:line_end]
e = s.index('\n#endif\n', d)
s = s[:w] + s[line_end + 1:e + 1] + s[e + len('\n#endif\n'):]
open(p, 'w').write(s)
print('unwrapped', fn)
