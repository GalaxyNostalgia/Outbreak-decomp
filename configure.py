#!/usr/bin/env python3
"""Set up the Outbreak decomp from your own discs.

    python configure.py f1 f2          # both games (or: all)
    python configure.py f2 --compilers # also fetch the candidate compilers + wibo
    python configure.py --tools-only   # compilers only (CI, no game files)

For each game (each step is skipped once done):
  1. Find the executable and its Metrowerks overlays (f1: PROG/*.BIN,
     f2: BIN/*.DAT) in orig/<ELF>/, or extract them from a disc image there.
  2. Check their sha1s against config/<ELF>/build.sha1. The first person to
     run this records them; commit that file (hashes are not game data).
  3. Survey the ELF and write config/<ELF>/main.yaml + <overlay>.yaml.
  4. Disassemble every target with splat into asm/<ELF>/<target>/.
English: patch your own image with the fan translation and put it in
orig/<ELF>/english/; the binaries it changes become targets f1en.* / f2en.*.

Then:  python -I tools/build.py <f1|f2>   (must print OK: byte-identical rebuild)

orig/, asm/, assets/, build/ and tools/compilers/ are git-ignored. Nothing from
the games is ever committed; tools/firewall.py enforces that in CI.
"""
import argparse
import hashlib
import json
import re
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


def find_files(ver, sub="", wanted=None):
    """orig/<ELF>/[sub/] must hold the ELF and its overlays, or a disc image to take them from."""
    elfname = ver["elf"]
    d = ROOT / "orig" / elfname / sub
    d.mkdir(parents=True, exist_ok=True)
    wanted = wanted or [elfname] + list(ver.get("overlays", {}).values())
    if all((d / w).exists() for w in wanted):
        return d, wanted
    isos = sorted(d.glob("*.iso")) + sorted(d.glob("*.ISO"))
    if not isos:
        sys.exit(f"Put the disc image (*.iso) in {d.relative_to(ROOT)}/ "
                 f"(or the extracted {', '.join(wanted)})")
    import pycdlib
    step(f"extracting {len(wanted)} files from {isos[0].name}")
    iso = pycdlib.PyCdlib()
    iso.open(str(isos[0]))
    try:
        cnf = d / "SYSTEM.CNF"
        iso.get_file_from_iso(str(cnf), iso_path="/SYSTEM.CNF;1")
        boot = cnf.read_text(errors="replace")
        if elfname not in boot:
            sys.exit(f"{isos[0].name} boots something else:\n{boot}")
        for w in wanted:
            (d / w).parent.mkdir(parents=True, exist_ok=True)
            iso.get_file_from_iso(str(d / w), iso_path=f"/{w};1")
    finally:
        iso.close()
    return d, wanted


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
    step(f"[{key}] {ver['title']} - {elfname} + {len(ver.get('overlays', {}))} overlays")
    d, files = find_files(ver)
    cdir = ROOT / "config" / elfname
    cdir.mkdir(parents=True, exist_ok=True)
    check_hashes(cdir / "build.sha1", d, files, key)
    if not (cdir / "main.yaml").exists():
        step(f"[{key}] surveying the ELF")
        subprocess.run([sys.executable, "-I", "tools/survey.py", str(d / elfname), "--write", key],
                       cwd=ROOT, check=True)
    for target in ["main", *ver.get("overlays", {})]:
        if not (ROOT / "asm" / elfname / target).exists():
            step(f"[{key}] disassembling {target} with splat")
            r = subprocess.run([sys.executable, "-m", "splat", "split", f"config/{elfname}/{target}.yaml"],
                               cwd=ROOT, capture_output=True, text=True)
            if r.returncode:
                sys.stderr.write(r.stdout[-3000:] + r.stderr[-3000:])
                sys.exit(f"splat failed on {target}")
    step(f"[{key}] ready:  python -I tools/build.py {key}")


def target_of(ver, path):
    if path == ver["elf"]:
        return "main"
    return next(t for t, p in ver["overlays"].items() if p == path)


def check_hashes(shaf, d, files, label):
    got = {f: sha1(d / f) for f in files}
    if shaf.exists():
        want = dict(reversed(line.split(None, 1)) for line in shaf.read_text().splitlines() if line.strip())
        bad = [f for f in files if want.get(f) != got[f]]
        if bad:
            sys.exit("\n".join(f"{f}: sha1 {got[f]} != supported {want.get(f)}" for f in bad) +
                     "\nDifferent disc or patch version? Open an issue with these hashes.")
        step(f"[{label}] sha1 OK ({len(files)} files)")
    else:
        shaf.write_text("".join(f"{got[f]}  {f}\n" for f in files))
        step(f"[{label}] recorded {len(files)} hashes in {shaf.relative_to(ROOT)} - commit it")


def configure_english(key, ver):
    """The English fan patch (REfan988 & Silent Hell) as extra targets <key>en.<target>.

    Patch your own image with the official release, put it in orig/<ELF>/english/,
    and only the binaries the patch changes become build targets."""
    en = ver.get("english")
    elfname = ver["elf"]
    d = ROOT / "orig" / elfname / "english"
    if not en or not d.exists():
        return
    label = f"{key}en"
    step(f"[{label}] English patch: {', '.join(en['changed'])}")
    d, files = find_files(ver, "english", en["changed"])
    cdir = ROOT / "config" / elfname
    check_hashes(cdir / "english.sha1", d, files, label)
    for path in files:
        t = target_of(ver, path)
        y = cdir / f"{t}.en.yaml"
        if not y.exists():
            src = (cdir / f"{t}.yaml").read_text()
            src = src.replace(f"target_path: orig/{elfname}/{path}", f"target_path: orig/{elfname}/english/{path}")
            src = re.sub(r"(asm|assets|build)/" + re.escape(elfname) + "/" + t + r"\b",
                         lambda m: f"{m.group(1)}/{elfname}/en/{t}", src)
            src = src.replace(f"basename: {elfname}.{t}", f"basename: {elfname}.en.{t}")
            src = re.sub(r"^sha1: \w+", f"sha1: {sha1(d / path)}", src, flags=re.M)
            src = re.sub(r"^name: (.*)$", r"name: \1 (English patch)", src, count=1, flags=re.M)
            y.write_text(src)
            step(f"[{label}] wrote {y.relative_to(ROOT)}")
        if not (ROOT / "asm" / elfname / "en" / t).exists():
            step(f"[{label}] disassembling {t} with splat")
            r = subprocess.run([sys.executable, "-m", "splat", "split", str(y.relative_to(ROOT))],
                               cwd=ROOT, capture_output=True, text=True)
            if r.returncode:
                sys.stderr.write(r.stdout[-3000:] + r.stderr[-3000:])
                sys.exit(f"splat failed on {t}.en")
    step(f"[{label}] ready:  python -I tools/build.py {label}")


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
        configure_english(k, versions[k])


if __name__ == "__main__":
    main()
