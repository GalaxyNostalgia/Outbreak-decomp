# Decompiling a function

1. **Pick one.** `asm/<ELF>/<target>/text.s` after `configure.py`. Start small; `tools/xmatch.py --list`
   shows which File 2 functions are the same code as File 1 functions.
2. **First draft.** `python -I tools/draft.py f1.main func_XXXXXXXX` runs
   [m2c](https://github.com/matt-kempster/m2c) (cloned next to this repo) on the function, jump tables
   included, and lists what it calls and which data it touches.
3. **Iterate until it matches.** Put the C in a scratch file and run
   `python -I tools/try.py f1.game scratch.c -v`. It compiles with the game's compiler and compares every
   function in the file with the original, instruction by instruction (addresses masked). decomp.me also
   works: PS2 platform, the compiler named in `config/versions.json`.
4. **Add it to the build.** Move the C into `src/<ELF>/<target>/<file>.c` (e.g. `src/SLPM_654.28/game/`).
   Functions keep their `func_XXXXXXXX` names until we know better. Shared structs go in `include/`.
5. **Check.** `python -I tools/build.py f1` must still print OK for every target, and the target line now
   says how many functions come from C. `build.py` cuts those functions out of the asm and links the compiled
   ones at the same addresses; anything that's even one byte off shows up as a MISMATCH with its address.

Notes

- **Not matching yet?** A function that is correct but not byte-identical goes under `#ifdef NON_MATCHING`
  with a one-line note of what differs. The PS2 build leaves those out (the asm stays), the PC build
  ([Outbreak-PC](https://github.com/GalaxyNostalgia/Outbreak-PC)) compiles them. Matching them later is
  ordinary decomp work.
- Hardware registers go through `HW_REG32`/`HW_REG64` (`include/ps2mem.h`), interrupt-handler epilogues
  through `SYNC_EI()` (`include/system.h`), quadword clears through `U128_ZERO`, so the same C builds on PC.

- A different compiler or flags for one file: put `// cc: mwcps2-3.0b52-030722 -O3` on its first line.
- C data (`.rodata`/`.data`/`.sdata`) isn't linked yet: functions with jump tables, float constants or
  strings stay in asm for now.
- English targets (`f1en.*`) compile the same sources with `-DENGLISH`.
