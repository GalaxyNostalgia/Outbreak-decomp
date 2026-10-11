# English translation

The English patch for the Japanese games is by **REfan988 & Silent Hell** (map names: Fothsid),
[SilentHeII/Outbreak-Translation](https://github.com/SilentHeII/Outbreak-Translation), GPL-3.0. The current
release (v2.1) ships `Patches.7z` with `File 1.xdelta` and `File 2.xdelta` and DeltaPatcher. Each patch
translates the text and includes a DNAS skip. Obsrv's English-speaking players use it and play with unpatched
Japanese players.

## Applying it

Apply the patch to your own unmodified image with DeltaPatcher (Windows) or
`xdelta3 -d -s "<original>.iso" "File 2.xdelta" "<english>.iso"`. Both patches apply cleanly to the images this
project's hashes describe (`config/<ELF>/build.sha1`); xdelta verifies every 8 MiB window's checksum.

## What it changes (measured 2026-10-11)

Same-size, in-place edits. The disc layout doesn't move.

| | File 1 | File 2 |
|---|---|---|
| Main ELF | 6,179 bytes, all data (text) | 9,844 bytes, all data |
| demo overlay | 6,576 bytes, data | 339 bytes, data |
| game overlay | 553 bytes data + **1 instruction** | **1 instruction** |
| netwk overlay | 9,368 bytes, data | 9,567 bytes, data |
| other overlays | unchanged | unchanged |
| other files | `IOP/MODULES/DNAS280.IMG` (DNAS skip), `NETBIO00.DAT` | `NETBIO00.DAT` |

The one code change is the same in both games (F1 `0x6a7924`, F2 `0x6b70f8`): an `addiu a0, s2, 8` becomes
`addiu a0, s2, 40`, so both branches use the field at offset 40 of a structure (most likely an English name/text
field instead of the Japanese one at offset 8). Everything else is text in the data sections.

## How the project handles it

- The patched binaries are their own build targets: `f1en.main`, `f1en.demo`, `f1en.game`, `f1en.netwk` and the
  same for `f2en`. Put your patched image in `orig/<ELF>/english/`, run `python configure.py f1 f2`, and
  `tools/build.py` rebuilds them byte-identical too (`config/<ELF>/english.sha1`, `<target>.en.yaml`).
- In C, the English build is the same source with different string data and one `#if` for the offset-40 field.
- The repo never contains the patch or patched data. Users download the official release and patch their own
  images. Ask Silent Hell before publishing anything that depends on the patch; bundling translated text later
  would have to follow GPL-3.0 and the authors' wishes.
- PC port: read text from the user's patched image (or the patched files next to the original image).
