#!/usr/bin/env python3
"""Refuse to let game data into the repository.

    python -I tools/firewall.py            # check every tracked + staged file
    python -I tools/firewall.py --install  # also run it as a git pre-commit hook

Fails on: ELF/IRX executables, disc images, splat output (asm/, assets/),
files whose sha1 matches a supported executable, large binaries, and asm that
still carries spimdisasm's per-instruction ROM comments. Matching decomp C is
fine; disassembly and data dumps are not.
"""
import hashlib
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
MAX_BINARY = 256 * 1024
FORBIDDEN_DIRS = ("orig/", "asm/", "assets/", "build/", "tools/compilers/")
FORBIDDEN_EXT = {".iso", ".bin", ".img", ".cso", ".chd", ".mdf", ".afs", ".irx", ".adx", ".sfd",
                 ".pss", ".xdelta", ".ppf", ".elf", ".o"}
SPIM_LINE = re.compile(rb"/\* [0-9A-F]+ [0-9A-F]{8} [0-9A-F]{8} \*/")


def tracked():
    out = subprocess.run(["git", "ls-files", "-z", "--cached", "--others", "--exclude-standard"],
                         cwd=ROOT, capture_output=True, check=True).stdout
    return [p for p in out.decode().split("\0") if p]


def known_hashes():
    hs = set()
    for f in (ROOT / "config").glob("*/build.sha1"):
        hs.add(f.read_text().split()[0])
    return hs


def check(path, hashes):
    p = ROOT / path
    if not p.is_file():
        return None
    if path.startswith(FORBIDDEN_DIRS):
        return "generated or game-derived directory"
    if p.suffix.lower() in FORBIDDEN_EXT:
        return f"forbidden file type {p.suffix}"
    data = p.read_bytes()
    if data[:4] == b"\x7fELF":
        return "ELF executable"
    if hashlib.sha1(data).hexdigest() in hashes:
        return "this IS a game executable"
    if b"\0" in data[:8192] and len(data) > MAX_BINARY:
        return f"binary file over {MAX_BINARY // 1024} KiB"
    if p.suffix in (".s", ".S", ".inc") and len(SPIM_LINE.findall(data)) > 16:
        return "raw disassembly (spimdisasm listing) - keep it in asm/, which is generated"
    return None


def install_hook():
    hook = ROOT / ".git/hooks/pre-commit"
    hook.write_text("#!/bin/sh\nexec python3 -I tools/firewall.py\n")
    hook.chmod(0o755)
    print(f"installed {hook.relative_to(ROOT)}")


def main():
    if "--install" in sys.argv:
        install_hook()
    hashes = known_hashes()
    bad = [(f, why) for f in tracked() if (why := check(f, hashes))]
    for f, why in bad:
        print(f"firewall: {f}: {why}")
    if bad:
        sys.exit(f"firewall: {len(bad)} file(s) must not be committed")
    print("firewall: clean")


if __name__ == "__main__":
    main()
