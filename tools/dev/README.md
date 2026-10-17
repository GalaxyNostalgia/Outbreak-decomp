# tools/dev: small helpers for decompiling one function at a time

Run from the repo root. They read your `orig/` and `asm/` (never committed).

| tool | what it does |
|---|---|
| `show.py asm/.../text.s func...` | a function's instructions (`MAXL=n` lines) |
| `dis.sh file.c func` | your compile of `func` beside the original, normalized (`OVL=game` for an overlay) |
| `declperm.py f1.game file.c func` | tries every order of the function's leading declarations; keeps the first match |
| `nonmatch.py file.c func "note"` | wraps a definition in `#ifdef NON_MATCHING /* note */` |
| `unnm.py file.c func` | removes that wrapper once it matches |
| `str.py addr...` / `ostr.py OVL addr...` | C strings at addresses in the main executable / an overlay |

`tools/draft.py f1.game func` gives the m2c first draft (m2c cloned next to the repo).
