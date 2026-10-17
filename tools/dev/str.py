"""str.py addr...: the C strings at these addresses in File 1's main executable (from your orig/)."""
import struct, sys
from pathlib import Path
ROOT = Path(__file__).resolve().parents[2]
d = (ROOT / "orig/SLPM_654.28/SLPM_654.28").read_bytes()
phoff, = struct.unpack_from("<I", d, 0x1C); phnum, = struct.unpack_from("<H", d, 0x2C)
segs = []
for i in range(phnum):
    t, off, va, pa, fs, ms = struct.unpack_from("<6I", d, phoff + i * 32)
    if t == 1: segs.append((va, off, fs))
for a in sys.argv[1:]:
    v = int(a, 16)
    for va, off, fs in segs:
        if va <= v < va + fs:
            o = off + v - va; e = d.index(b"\0", o); print(hex(v), repr(d[o:e].decode("latin1")))
