# The native PC port

The PC version is built from the same decompiled C as the byte-matching PS2 build. Every function we
decompile for the decomp also becomes native PC code. The PS2 SDK, CRI middleware and hardware get
replacements in `pc/`. This page explains how that works while the decomp is incomplete.

Status (2026-10-11): `pc/` builds a 32-bit `outbreak-f1`. It maps PS2 memory, opens the user's disc
image, loads the executable's data, and calls the game's own `main()` (decompiled,
`src/SLPM_654.28/main/main.c`) natively. It stops at the first function that isn't decompiled yet:

```
[info] starting the game: main(1, {"cdrom0:\SLPM_654.28;1"})
[error] game called func_001A85C0 (0x001a85c0), which isn't decompiled yet
```

`func_001A85C0` is the system init (graphics reset, IOP modules, …): the next thing to decompile.
`--test` runs the decompiled disc helpers (`src/SLPM_654.28/main/cdfile.c`) against the real files on the disc.

## Building

```sh
# Linux (Debian/Ubuntu): sudo apt install gcc-multilib cmake
cmake -S pc -B build/pc -DOUTBREAK_GAME=f1 -DCMAKE_C_FLAGS=-m32
cmake --build build/pc
build/pc/outbreak-f1 --test "Bio Hazard Outbreak - File 1.iso"
```

On Windows use an i686 (32-bit) MinGW-w64 toolchain, e.g. MSYS2's `mingw-w64-i686-gcc` and
`mingw-w64-i686-cmake`, with the same commands from a "MINGW32" shell. MSVC isn't supported, because
the build relies on GNU ld linker-script symbols.

## Design decisions

### 32-bit, little-endian

PS2 pointers are 4 bytes. If the PC build is 32-bit (x86, also little-endian), every game structure keeps
its PS2 layout:
- data loaded from game files and the original executable can be used as-is;
- network packets built from structs stay byte-identical, which Obsrv compatibility needs.

### The PS2 address space inside the PC process (`pc/src/guest.c`)

At startup the port reserves PS2 main RAM (`0x00100000-0x02000000`) and the scratchpad (`0x70000000`,
16 KiB) at the same addresses in the PC process. The program itself is linked elsewhere (Windows:
`--image-base 0x40000000`).

The original executable's initialised data (strings, tables, constants) is copied there from the user's
disc (`pc/src/loader.c`), and its `.bss` is cleared, as the PS2 kernel does. Decompiled C keeps
referring to that data by its PS2 address: `extern char D_00224268[];` links to the absolute address
`0x224268` (`pc/tools/gen_link.py` writes the symbol list). So the data never has to be decompiled or
redistributed; it always comes from the user's own disc.

### Overlays

The game loads code overlays (`PROG/*.BIN` / `BIN/*.DAT`) into shared slots. On PC:
- all overlays' decompiled code is linked into the executable;
- when the game loads an overlay, `loader_load_overlay` still copies the file into guest memory, so the
  overlay's data is correct;
- the port records which overlay occupies each slot. Function pointers stored in game data (PS2 code
  addresses) are translated through a table of (overlay, PS2 address) → native function.

Overlays reuse addresses, so two overlays can each have a `func_00540040`. `gen_link.py` stops the build
when that happens; such functions need real names first.

### Functions that aren't decompiled yet

Every `func_XXXXXXXX` the C calls but nobody has decompiled gets a generated stub
(`build/pc/gen/stubs.c`). Calling one stops the port with "game called func_XXXXXXXX, which isn't
decompiled yet", which tells us what to decompile next on the path being brought up.

### Library replacements (`pc/src/`)

The game calls Sony's libraries by name: C library, `sceOpen`/`sceRead`/…, graphics, pad, memory card,
SIF/IOP, network. Those names come from `config/<ELF>/symbol_addrs.txt`. On PS2 they stay original
object code; on PC they're implemented natively:

| PS2 library | PC replacement | Status |
|---|---|---|
| C library (`strcpy`, `sprintf`, …) | host C library; `strupr` in `libc_extra.c` | done for what the C uses |
| file I/O (`sceOpen/Read/Lseek/Close`) | `sce_io.c`: `cdrom0:`/`pfs0:` → disc image, `host:` → local folder | done |
| CD/DVD streaming (`sceCdRead`, CRI file system) | disc image reads | to do |
| graphics (libgraph, DMA, VIF/VU1, GS) | native renderer, starting with a GS packet interpreter | to do |
| pad, memory card | SDL input; save files on disk | to do |
| sound (IOP driver, SPU2, CRI ADX) | software mixer + ADX decoder | to do |
| movies (CRI Sofdec) | FFmpeg | to do |
| network (DEV9/INET IOP stack, DNAS, KDDI pages) | native sockets + legacy-TLS HTTPS to Obsrv | to do ([NETWORK.md](NETWORK.md)) |

### Floating point

The PS2 FPU isn't IEEE: no infinities, NaNs or denormals; results clamp and round differently. Offline
this rarely matters. Online it might: PS2 and PC peers that simulate the same thing must not drift
apart. If testing shows desyncs, the gameplay code involved will use PS2-accurate float helpers on PC.

## Order of work for File 1

1. **Boot path:** `_start` (crt0: `SetupThread`, `SetupHeap`, `_InitSys` `func_0011D648`, `FlushCache`)
   → `main` `func_001A87F0` ✅ → system init `func_001A85C0` → per-frame loop (`func_001B9070`,
   `func_001C53F0`, `func_001C5430`). Init covers (SIF/IOP modules, graphics, pad, memory card) →
   the overlay loader → `submain`. Each decompiled step lets `outbreak-f1` run further; the stubs show
   where it stops.
2. **Graphics bring-up:** interpret the GS packets the game builds, enough to show the logo and menus.
3. **Input, memory card, sound, movies.**
4. **The `game` overlay** (offline play), then **`netwk`/`nethttp`/`netdnas`** for Obsrv.
