#!/usr/bin/env python3
"""List what is on a disc image: boot ELF, IOP modules, archives, movies.

    python -I tools/iso_list.py orig/SLPM_656.92/*.iso

Prints SYSTEM.CNF and every file with its size, grouped by type. Use it to
see which IRX modules the game loads (Capcom's own sound driver vs Sony's),
which AFS archives hold the text the English patch changes, and which files
the translation patch touches (compare a vanilla and a patched image with
--diff OTHER.iso). Nothing is extracted or written.
"""
import argparse
import hashlib
from collections import defaultdict
from pathlib import Path

import pycdlib


def walk(iso):
    out = {}
    for dirname, _, files in iso.walk(iso_path="/"):
        for f in files:
            path = f"{dirname.rstrip('/')}/{f}"
            rec = iso.get_record(iso_path=path)
            out[path.split(";")[0]] = rec
    return out


def digest(iso, path, size):
    if size > 64 << 20:
        return None
    import io
    buf = io.BytesIO()
    iso.get_file_from_iso_fp(buf, iso_path=f"{path};1")
    return hashlib.sha1(buf.getvalue()).hexdigest()


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("iso", type=Path)
    ap.add_argument("--diff", type=Path, help="second image: list files whose content differs")
    a = ap.parse_args()
    iso = pycdlib.PyCdlib()
    iso.open(str(a.iso))
    files = walk(iso)
    import io
    cnf = io.BytesIO()
    iso.get_file_from_iso_fp(cnf, iso_path="/SYSTEM.CNF;1")
    print("SYSTEM.CNF:\n  " + cnf.getvalue().decode(errors="replace").strip().replace("\n", "\n  "))
    groups = defaultdict(list)
    for path, rec in sorted(files.items()):
        groups[Path(path).suffix.upper() or "(none)"].append((path, rec.data_length))
    for ext, items in sorted(groups.items(), key=lambda kv: -sum(s for _, s in kv[1])):
        print(f"\n{ext}  {len(items)} file(s), {sum(s for _, s in items) / 1048576:.1f} MiB")
        for path, size in items[:200]:
            print(f"  {size:>12,}  {path}")
    if a.diff:
        other = pycdlib.PyCdlib()
        other.open(str(a.diff))
        ofiles = walk(other)
        print(f"\nDifferences vs {a.diff.name}:")
        for path in sorted(set(files) | set(ofiles)):
            if path not in ofiles or path not in files:
                print(f"  {'only in ' + (a.iso.name if path in files else a.diff.name)}: {path}")
                continue
            s1, s2 = files[path].data_length, ofiles[path].data_length
            if s1 != s2 or digest(iso, path, s1) != digest(other, path, s2):
                print(f"  changed: {path}  ({s1:,} -> {s2:,} bytes)")
        other.close()
    iso.close()


if __name__ == "__main__":
    main()
