# Biohazard Outbreak decomp

A matching decompilation of **Biohazard Outbreak** and **Biohazard Outbreak File 2** (PlayStation 2,
Japanese releases), aiming for a native PC version built from the same source that plays online on the
[Obsrv](https://www.obsrv.org/) servers alongside PS2 and PCSX2 players, with the English fan translation.

> [!IMPORTANT]
> This repository contains no game code, data or disassembly. You need your own discs; the build
> disassembles them locally. See [docs/LEGAL.md](docs/LEGAL.md).

## Versions

| Key | Game | Disc | Executable | Status |
|---|---|---|---|---|
| `f1` | Biohazard Outbreak | SLPM-65428 (VER 1.04) | `SLPM_654.28` + 8 overlays (`PROG/*.BIN`) | 9/9 targets rebuild byte-identical from asm |
| `f2` | Biohazard Outbreak File 2 | SLPM-65692 (VER 1.03) | `SLPM_656.92` + 9 overlays (`BIN/*.DAT`) | 10/10 targets rebuild byte-identical from asm |
| `f1en` / `f2en` | same, with the English fan patch v2.1 | your patched image in `orig/<ELF>/english/` | the 4 binaries per game the patch changes | 8/8 targets rebuild byte-identical from asm |

Decompiled so far: 10 of ~14,500 File 1 functions (`src/`), byte-identical in the PS2 build; File 2 not started.

**PC port** (`pc/`, [docs/PCPORT.md](docs/PCPORT.md)): a 32-bit native build that maps PS2 memory, reads your disc
image and runs the decompiled code natively. It can't start the game yet; `--test` runs the decompiled disc functions. About half of File 2's code is
shared with File 1 and only needs decompiling once. What's in the binaries: [docs/LAYOUT.md](docs/LAYOUT.md).

Japanese only, on purpose: Obsrv emulates the Japanese online service, and nobody emulates the US/EU one.
English comes from the fan translation applied to the Japanese games ([docs/TRANSLATION.md](docs/TRANSLATION.md)).

## Setup

Linux or **WSL2 on Windows** (Ubuntu 24.04):

```sh
sudo apt install python3-venv binutils-mips-linux-gnu git
# (64-bit Linux runs the 32-bit Windows compilers through wibo; no Wine needed)
python3 -m venv .venv && . .venv/bin/activate
pip install -r requirements.txt
python3 -I tools/firewall.py --install        # pre-commit hook that keeps game data out of git
```

Put your disc images (verified against Redump) in place:

```
orig/SLPM_654.28/<anything>.iso     # Biohazard Outbreak
orig/SLPM_656.92/<anything>.iso     # Biohazard Outbreak File 2

(or the extracted files instead of the image: the ELF, SYSTEM.CNF and PROG/ or BIN/)
```

Then:

```sh
python configure.py f1 f2            # extract, verify sha1, survey, disassemble
python configure.py --compilers      # Metrowerks compilers (decomp.me builds) + wibo, to build the C
python -I tools/build.py f1 f2       # rebuild all 19 binaries and check they're byte-identical
```

`build.py` must print `OK` for every target (`f1.main`, `f1.game`, …). That's the rule for every commit.

## Tools

| | |
|---|---|
| `configure.py` | per game: extract the executable and overlays from your image, check sha1s, write splat configs, run splat |
| `tools/build.py` | assemble + link at the original addresses, compare byte-for-byte, report the first difference |
| `tools/survey.py` | ELF layout, `$gp`, compiler fingerprints, SDK/CRI versions, IOP modules; writes the splat configs for the ELF and its `MWo3` overlays |
| `tools/xmatch.py` | finds functions File 1 and File 2 share, so each is decompiled once |
| `tools/try.py` | compiles a C file with the game's compiler and compares each function with the original ([docs/DECOMPILING.md](docs/DECOMPILING.md)) |
| `tools/iso_list.py` | list a disc's files; `--diff` shows what a patch (e.g. the English one) changed |
| `tools/firewall.py` | refuses executables, images, disassembly and big binaries in git |
| `tools/compilers.json` | candidate compilers (decomp.me builds) for `configure.py --compilers` |

## Plan

[docs/ROADMAP.md](docs/ROADMAP.md) and [docs/PCPORT.md](docs/PCPORT.md): baseline → compiler → decompilation → IOP → PC platform layer → Obsrv →
English. Online details: [docs/NETWORK.md](docs/NETWORK.md). Compiler proof: [docs/COMPILER.md](docs/COMPILER.md).

## Thanks

Tooling and approach follow [splat](https://github.com/ethteck/splat), [spimdisasm](https://github.com/Decompollaborate/spimdisasm),
[decomp.me](https://decomp.me), and the Capcom PS2 decomps [dmc3-decomp](https://github.com/WadeWinningWilson/dmc3-decomp)
and [recvx-decomp](https://github.com/AshfordFamily/recvx-decomp). Online research builds on ghostline's BioServer
and the Obsrv team; the English patch is by REfan988 & Silent Hell.
