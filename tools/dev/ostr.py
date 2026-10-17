"""ostr.py OVL addr...: the C strings at these addresses in an overlay (from your orig/ files)."""
import re, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
ovl = sys.argv[1]
y = (ROOT / f"config/SLPM_654.28/{ovl.lower()}.yaml").read_text()
vram = int(re.search(r"vram:\s*0x([0-9A-Fa-f]+)", y).group(1), 16)
d = (ROOT / f"orig/SLPM_654.28/PROG/{ovl.upper()}.BIN").read_bytes()
for a in sys.argv[2:]:
    v = int(a, 16); o = v - vram + 0x40; e = d.index(b"\0", o); print(hex(v), repr(d[o:e].decode("latin1")))
