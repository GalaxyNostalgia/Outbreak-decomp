#!/usr/bin/env python3
"""Set up the Outbreak decomp from your own discs.

    python configure.py f1 f2          # both games (or: all)
    python configure.py f2 --compilers # also fetch the candidate compilers + wibo
    python configure.py --tools-only   # compilers only (CI, no game files)

For each game (each step is skipped once done):
  1. Find the executable in orig/<ELF>/: the ELF itself or a disc image (*.iso)
     it is extracted from (f1 = SLPM_654.28, f2 = SLPM_656.92).
  2. Check its sha1 against config/<ELF>/build.sha1. The first person to run
     this records it; commit that file (a hash is not game data).
  3. Survey the ELF and write config/<ELF>/splat.yaml if there is none.
  4. Disassemble it with splat into asm/<ELF>/.
Then:  python -I tools/build.py <f1|f2>   (must print OK: byte-identical rebuild)

orig/, asm/, assets/, build/ and tools/compilers/ are git-ignored. Nothing from
the games is ever committed; tools/firewall.py enforces that in CI.
"""
import argparse
import hashlib
import json
import shutil
import subprocess
import sys
import tarfile
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parent
COMPILERS_URL = "https://github.com/decompme/compilers/releases/download/compilers/{}.{}"
WIBO_URL = "https://github.com/decompals/wibo/releases/download/1.2.0/wibo-i686"


def step(msg):
    print(f"==> {msg}", flush=True)


def sha1(path):
    h = hashlib.sha1()
    with open(path, "rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def find_executable(elfname):
    d = ROOT / "orig" / elfname
    d.mkdir(parents=True, exist_ok=True)
    elf = d / elfname
    if elf.exists():
        return elf
    isos = sorted(d.glob("*.iso")) + sorted(d.glob("*.ISO"))
    if not isos:
        sys.exit(f"Put {elfname} or the disc image (*.iso) in {d.relative_to(ROOT)}/")
    import pycdlib
    step(f"extracting {elfname} from {isos[0].name}")
    iso = pycdlib.PyCdlib()
    iso.open(str(isos[0]))
    try:
        cnf = d / "SYSTEM.CNF"
        iso.get_file_from_iso(str(cnf), iso_path="/SYSTEM.CNF;1")
        boot = cnf.read_text(errors="replace")
        if elfname not in boot:
            sys.exit(f"{isos[0].name} boots something else:\n{boot}")
        iso.get_file_from_iso(str(elf), iso_path=f"/{elfname};1")
    finally:
        iso.close()
    return elf


def fetch(url, dest):
    step(f"downloading {url}")
    with urllib.request.urlopen(url) as r, open(dest, "wb") as f:
        shutil.copyfileobj(r, f)


def get_compilers(ids):
    cdir = ROOT / "tools/compilers"
    cdir.mkdir(parents=True, exist_ok=True)
    for cc in ids:
        if (cdir / cc).is_dir():
            continue
        for ext in ("tar.gz", "tar.xz"):
            arc = cdir / f"{cc}.{ext}"
            try:
                fetch(COMPILERS_URL.format(cc, ext), arc)
                break
            except Exception:
                arc.unlink(missing_ok=True)
        else:
            print(f"    (no download for {cc}; skipped)")
            continue
        with tarfile.open(arc) as t:
            t.extractall(cdir / cc, filter="data")
        arc.unlink()
    wibo = cdir / "wibo"
    if not wibo.exists():
        fetch(WIBO_URL, wibo)
        wibo.chmod(0o755)


def configure(key, ver):
    elfname = ver["elf"]
    step(f"[{key}] {ver['title']} - {elfname}")
    elf = find_executable(elfname)
    cdir = ROOT / "config" / elfname
    cdir.mkdir(parents=True, exist_ok=True)
    shaf = cdir / "build.sha1"
    got = sha1(elf)
    if shaf.exists():
        want = shaf.read_text().split()[0]
        if got != want:
            sys.exit(f"{elf.name}: sha1 {got} is not the supported {elfname} ({want}).\n"
                     "Different disc revision? Open an issue with the sha1 and SYSTEM.CNF contents.")
        step(f"[{key}] sha1 OK")
    else:
        shaf.write_text(f"{got}  {elfname}\n")
        step(f"[{key}] recorded sha1 {got} in {shaf.relative_to(ROOT)} - commit it")
    if not (cdir / "splat.yaml").exists():
        step(f"[{key}] surveying the ELF")
        subprocess.run([sys.executable, "-I", "tools/survey.py", str(elf), "--write", key], cwd=ROOT, check=True)
    if not (ROOT / "asm" / elfname).exists():
        step(f"[{key}] disassembling with splat")
        subprocess.run([sys.executable, "-m", "splat", "split", f"config/{elfname}/splat.yaml"],
                       cwd=ROOT, check=True)
    step(f"[{key}] ready:  python -I tools/build.py {key}")


def main():
    versions = json.loads((ROOT / "config/versions.json").read_text())
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("versions", nargs="*", default=[], help=f"{', '.join(versions)} or all")
    ap.add_argument("--compilers", action="store_true", help="fetch candidate compilers (tools/compilers.json)")
    ap.add_argument("--tools-only", action="store_true", help="only fetch compilers")
    a = ap.parse_args()
    if a.compilers or a.tools_only:
        cands = json.loads((ROOT / "tools/compilers.json").read_text())
        get_compilers(sorted({c for v in cands["candidates"].values() for c in v}))
    if a.tools_only:
        return
    keys = list(versions) if (not a.versions or "all" in a.versions) else a.versions
    for k in keys:
        if k not in versions:
            sys.exit(f"unknown version {k}; known: {', '.join(versions)}")
        configure(k, versions[k])


if __name__ == "__main__":
    main()
