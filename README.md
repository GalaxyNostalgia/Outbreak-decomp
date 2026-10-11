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
| `f1` | Biohazard Outbreak | SLPM-65428 | `SLPM_654.28` | waiting for first dump |
| `f2` | Biohazard Outbreak File 2 | SLPM-65692 | `SLPM_656.92` | waiting for first dump |

Japanese only, on purpose: Obsrv emulates the Japanese online service, and nobody emulates the US/EU one.
English comes from the fan translation applied to the Japanese games ([docs/TRANSLATION.md](docs/TRANSLATION.md)).

## Setup

Linux or **WSL2 on Windows** (Ubuntu 24.04):

```sh
sudo apt install python3-venv binutils-mips-linux-gnu git
python3 -m venv .venv && . .venv/bin/activate
pip install -r requirements.txt
python3 -I tools/firewall.py --install        # pre-commit hook that keeps game data out of git
```

Put your disc images (verified against Redump) in place:

```
orig/SLPM_654.28/<anything>.iso     # Biohazard Outbreak
orig/SLPM_656.92/<anything>.iso     # Biohazard Outbreak File 2
```

Then:

```sh
python configure.py f1 f2            # extract, verify sha1, survey, disassemble
python -I tools/build.py f1 f2       # rebuild both executables and check they're byte-identical
```

`build.py` must print `OK` for each game. That's the rule for every commit.

## Tools

| | |
|---|---|
| `configure.py` | per game: extract the executable from your image, check sha1, write the first splat config, run splat |
| `tools/build.py` | assemble + link at the original addresses, compare byte-for-byte, report the first difference |
| `tools/survey.py` | ELF layout, `$gp`, compiler fingerprints, SDK/CRI versions, IOP modules, network strings |
| `tools/iso_list.py` | list a disc's files; `--diff` shows what a patch (e.g. the English one) changed |
| `tools/firewall.py` | refuses executables, images, disassembly and big binaries in git |
| `tools/compilers.json` | candidate compilers (decomp.me builds) for `configure.py --compilers` |

## Plan

[docs/ROADMAP.md](docs/ROADMAP.md): baseline → compiler → decompilation → IOP → PC platform layer → Obsrv →
English. Online details: [docs/NETWORK.md](docs/NETWORK.md). Compiler proof: [docs/COMPILER.md](docs/COMPILER.md).

## Thanks

Tooling and approach follow [splat](https://github.com/ethteck/splat), [spimdisasm](https://github.com/Decompollaborate/spimdisasm),
[decomp.me](https://decomp.me), and the Capcom PS2 decomps [dmc3-decomp](https://github.com/WadeWinningWilson/dmc3-decomp)
and [recvx-decomp](https://github.com/AshfordFamily/recvx-decomp). Online research builds on ghostline's BioServer
and the Obsrv team; the English patch is by REfan988 & Silent Hell.
