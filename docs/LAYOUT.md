# What's in the executables

Facts about the Japanese discs, from `tools/survey.py` and splat (2026-10-11). Function counts are
splat's first pass and will change as boundaries are refined.

## Main executable + Metrowerks overlays

The boot ELF is small: crt0, the engine core, the Sony SDK and CRI middleware. Most of the game sits in
**overlays** in Metrowerks' `MWo3` format, which the main program loads from disc into fixed slots. Each
overlay file is a 0x40-byte header (magic `MWo3`, id, load address, .text/.data/.bss sizes, static-init range,
name), then .text and .data, loaded verbatim at the load address. The ELF lists every slot as an empty
`PT_LOAD` (file size 0).

| Target | File 1 file | id | Load | Functions | File 2 file | id | Load | Functions | F2 bytes also in F1 |
|---|---|---|---|---|---|---|---|---|---|
| main | `SLPM_654.28` | - | 0x100000 | 5,357 | `SLPM_656.92` | - | 0x100000 | 5,447 | 88.8% |
| submain | `PROG/SUBMAIN.BIN` | 1 | 0x340000 | 190 | `BIN/0.DAT` | 1 | 0x343000 | 191 | 54.6% |
| demo | `PROG/DEMO.BIN` | 2 | 0x540000 | 172 | `BIN/1.DAT` | 2 | 0x543000 | 312 | 10.0% |
| game | `PROG/GAME.BIN` | 3 | 0x540000 | 4,412 | `BIN/2.DAT` | 3 | 0x543000 | 5,098 | 31.1% |
| netwk | `PROG/NETWK.BIN` | 4 | 0x540000 | 2,381 | `BIN/3.DAT` | 4 | 0x543000 | 2,391 | 61.5% |
| netdnas | `PROG/NETDNAS.BIN` | 5 | 0x6c0000 | 30 | `BIN/4.DAT` | 5 | 0x6af000 | 34 | 2.3% |
| nethttp | `PROG/NETHTTP.BIN` | 6 | 0x6c0000 | 1,549 | `BIN/5.DAT` | 6 | 0x6af000 | 1,549 | 100% |
| insdnas | - | | | | `BIN/6.DAT` | 7 | 0x6af000 | 9 | - |
| single | `PROG/SINGLE.BIN` | 8 | 0x820000 | 328 | `BIN/8.DAT` | 9 | 0x7d2000 | 347 | 49.9% |
| netaq | `PROG/NETAQ.BIN` | 9 | 0x820000 | 68 | `BIN/9.DAT` | 10 | 0x7d2000 | 72 | 35.5% |
| **total** | | | | **14,487** | | | | **15,450** | **~49%** |

"F2 bytes also in F1" = share of File 2's code whose function has an identical instruction sequence in File 1
(addresses masked), from `tools/xmatch.py`. Those functions are decompiled once and built for both games.
`nethttp` is the same library in both; `netdnas` is Sony's DNAS code (few, very large, obfuscated functions).

Names come from the overlay headers. Guesses to confirm: `submain` (front end/menus), `game` (scenario
gameplay), `netwk` (online lobby + game-session code: the Obsrv-critical one), `nethttp` (HTTP/TLS client for
the KDDI login pages), `netdnas`/`insdnas` (DNAS authentication/installation), `single` and `netaq` (offline
mode and network setup helpers).

## Main ELF layout

| | File 1 | File 2 |
|---|---|---|
| Entry | 0x100008 | 0x100008 |
| `$gp` (set by crt0 via `move $gp, $a0`) | 0x241c70 | 0x2510f0 |
| EE code | 0x100000 - 0x1d9b00 | 0x100000 - 0x1ddb80 |
| VU microcode, EE kernel-patch code, data | 0x1d9b00 - 0x23ba00 | 0x1ddb80 - 0x24ac00 |
| .bss | 0x23ba00 + 0xf8c00 | 0x24ac00 + 0xf7100 |
| Overlay slots cleared by crt0 | up to 0x8b9180 | up to 0x86be00 |

The data region still needs splitting: VU microcode (upper-NOP `0x000002FF` pairs) from ~0x1d9b00, an EE
kernel-mode block (`mfc0`/`mtc0`, addresses 0x8007xxxx) ending at 0x1ed8a8 (F1), then .data/.rodata/.sdata.

## Toolchain and middleware

- `.comment`: **`MW MIPS C Compiler (2.4.1.01)`** (both games); overlays in Metrowerks `MWo3` format.
- Sony SDK 2.8 (`PsIIlibcdvd 2800` etc.; File 2's memory-card library is 3.0.2).
- CRI Sofdec/MPEG (`SFD 1.862`, `MPV 1.862`, `M2V/MPS 1.860`, `SFX 1.30`, `SFH 1.15`, `CFT 1.33`).
- IOP modules on disc: Sony's network stack (INET, PPP/PPPoE, SMAP, DEV9, modem drivers), HDD/PFS,
  USB keyboard, DNAS (`DNAS280.IMG`), plus NTGUI2 (Sony's network-configuration GUI).
