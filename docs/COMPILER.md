# Pinning the compiler

A matching decomp needs the exact compiler and flags. Nobody has published which ones Outbreak used, so we
prove it ourselves.

## What we know (2026-10-11)

- Both main ELFs carry the `.comment` **`MW MIPS C Compiler (2.4.1.01)`**.
- Every overlay is in Metrowerks' `MWo3` format, so the Metrowerks linker built them.
- Capcom precedent: *Code: Veronica X* (2001) used Metrowerks; *DMC3* (2005) used `mwcps2-3.0b52-030722` with
  `-O3,p -fl divbyzerocheck -inline depth=8`.
- So test **mwcps2 2.4** first. decomp.me's closest build is `mwcps2-2.4-001213` (2.4, Dec 2000); 2.4.1.01
  may be a later patch release, in which case 2.4 might match most code and miss a few codegen details. Keep
  2.3.3 and the 3.0 builds as fallbacks (`tools/compilers.json`). The Sony SDK 2.8 and CRI libraries may have
  been built with ee-gcc by their vendors even though no GCC `.comment` survived the MW linker.

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
| f1 | game code | ? | ? | |
| f1 | libraries | ? | ? | |
| f2 | game code | ? | ? | |
| f2 | libraries | ? | ? | |
