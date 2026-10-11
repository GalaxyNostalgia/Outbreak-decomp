# Pinning the compiler

A matching decomp needs the exact compiler and flags. Nobody has published which ones Outbreak used, so we
prove it ourselves.

## What we know (2026-10-11)

- **Game code is Metrowerks CodeWarrior for PS2 3.0.x.** Seven File 1 `game` functions match exactly with
  every 3.0 build from `mwcps2-3.0-011126` to `mwcps2-3.0.1b87-031208` (`tools/try.py`). The 2.3/2.4 builds
  can't match: they emit MMI register moves (`padduw`-style) where the games use `daddu`.
- The main ELF's `.comment` `MW MIPS C Compiler (2.4.1.01)` (decomp.me's `mwcps2-2.4-001213` writes exactly
  this) comes from some object linked into the ELF, probably a runtime/library object, not the game code.
- `-O4,p` (DMC3's setting) **rules out 3.0b38, 3.0b50 and 3.0b52** for File 1: they schedule `actor_update.c`
  differently. `-O3` and `-O4,s` match with all of them, so flags aren't pinned yet either.
- **Then the main ELF's disc helper `func_001A8300` ruled out every 3.0.1 build:** only 3.0b38, 3.0b50 and
  3.0b52 compile it correctly, with `-O3`, `-O3,s` or `-O4,s`.
- **Current default: `mwcps2-3.0b52-030722` with `-O3`** (`config/versions.json`), the build Capcom used for
  DMC3. All 10 decompiled functions (main ELF and `game` overlay) match with it. b38/b50 and the `,s`
  variants still need a function that tells them apart.
- Every overlay is in Metrowerks' `MWo3` format, so the Metrowerks linker built them.

## Step 1: read the fingerprints

`configure.py` writes `config/<ELF>/survey.json`. Look at:
- `gcc_comments`: `GCC: (GNU) 2.9x …` strings → which ee-gcc built the libraries.
- `counters.mw_*`: Metrowerks markers, `__ct__`/`__dt__` C++ names, MW-only sections.
- `counters.sn_markers`: SN Systems ProDG.
- `sdk_versions` (`PsII…` strings) and `cri` (CRI ADX/Sofdec versions): match against other decomps for free
  library code.

## Step 2: prove it on small functions

1. Pick 10–20 small leaf functions from `asm/<ELF>/text.s` (getters, setters, short loops); some from the
   library range, most from game code.
2. For each, write a C guess. [m2c](https://github.com/matt-kempster/m2c) gives a first draft
   (`git clone` it, then `python m2c/m2c.py asm/<ELF>/text.s --function func_XXXXXXXX`).
3. Try it on [decomp.me](https://decomp.me) (PS2 preset) with each candidate compiler, or locally with
   `python configure.py --compilers` and the compilers in `tools/compilers/` (Metrowerks runs on Linux/WSL
   through `wibo`).
4. A compiler that matches several unrelated functions with the same flags is the one. Record it below and set
   `compiler:` in `config/<ELF>/splat.yaml` (MWCCPS2 or EEGCC).

## Result

| Game | Range | Compiler | Flags | Evidence |
|---|---|---|---|---|
| f1 | game code (main ELF + `game` overlay) | MW 3.0b38/b50/b52, using `mwcps2-3.0b52-030722` | `-O3` (or `-O3,s`/`-O4,s`) | 10 functions match |
| f1 | libraries | ? | ? | |
| f2 | game code | ? | ? | |
| f2 | libraries | ? | ? | |
