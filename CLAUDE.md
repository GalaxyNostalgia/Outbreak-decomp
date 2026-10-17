# Working on Outbreak-decomp (notes for Claude sessions)

This repo is driven from the PC port: see `CLAUDE.md` in Outbreak-PC (setup from a fresh session,
the run-and-decompile loop, the rules, and MWCC habits). In short:

- Decompile the function the PC run reports next; check it with `python3 -I tools/try.py
  f1.<overlay> src/SLPM_654.28/<overlay>/<file>.c`; helpers in `tools/dev/`.
- After one or two honest attempts, keep the C as `NON_MATCHING` (`tools/dev/nonmatch.py`).
- Before committing: `python3 -I tools/build.py` (every target OK). No game data in commits.
