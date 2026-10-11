# Roadmap

The goal: both Japanese games — *Biohazard Outbreak* (SLPM-65428, "f1") and *Biohazard Outbreak File 2*
(SLPM-65692, "f2") — decompiled to C that rebuilds the original executables byte-for-byte, then built as a
native PC game from the same source. The PC build plays online on the Obsrv servers alongside PS2 and PCSX2
players, and supports the English fan translation.

Why Japanese: Obsrv only emulates the Japanese online service (KDDI + DNAS). Nobody captured the US/EU
service (SNAP), so US/EU builds cannot reach Obsrv. See [NETWORK.md](NETWORK.md).

Each phase ends with a check that a machine can run. "Done" means the check passes.

## Phase 0: Project setup ✅

- `configure.py` takes your own disc image, extracts the executable, checks its sha1, surveys it and runs splat.
- `tools/build.py` reassembles everything and verifies a byte-identical executable.
- `tools/firewall.py` keeps game data out of git (CI and pre-commit).
- **Check:** the pipeline rebuilds synthetic PS2-shaped ELFs (with, without and fully stripped section headers)
  byte-for-byte. Done on 2026-10-11.

## Phase 1: Byte-identical baseline from assembly (both games) ✅

- Each game is a main ELF plus Metrowerks `MWo3` overlays: 9 binaries for File 1, 10 for File 2
  ([LAYOUT.md](LAYOUT.md)). `configure.py` extracts and hashes all of them and writes one splat config each.
- `config/<ELF>/main.yaml` splits the ELF's single load segment into EE code and the VU/data region by hand.
- **Check:** `python -I tools/build.py f1 f2` prints OK for all 19 targets. Done on 2026-10-11.
- Left for Phase 2: split the main ELF's data region (VU microcode, kernel-patch code, .data/.rodata/.sdata)
  and each overlay's .data into proper sections.

## Phase 2: Pin the compilers and map the binary (started)

- 2026-10-11: game code is Metrowerks 3.0.x; `build.py` links decompiled C in place of asm, and 7 File 1
  `game` functions are in `src/`, byte-identical ([COMPILER.md](COMPILER.md), [DECOMPILING.md](DECOMPILING.md)).


1. Run the compiler test in [COMPILER.md](COMPILER.md) on 10–20 small functions per game. Both ELFs say
   `MW MIPS C Compiler (2.4.1.01)` and the overlays are Metrowerks `MWo3`, so start with mwcps2 2.4.
   Record the exact ids and flags.
2. Split `.text` into libraries vs game code. Identify Sony SDK, CRI ADX/Sofdec and Capcom library functions
   by signature: compare against symbol-bearing builds of other Capcom PS2 games and PS2 SDK objects of the same version.
3. Cross-match f1 and f2: `tools/xmatch.py` already finds ~49% of File 2's code bytes identical in File 1
   (89% of the main ELF, all of `nethttp`). Each shared function is decompiled once and built for both games.
4. Find translation-unit boundaries (string/rodata grouping, alignment gaps) and split `text` into per-file
   subsegments.
- **Check:** `build.py` links C objects; one matched function per game is in `src/` and both builds stay OK.

## Phase 3: Decompile

- Order: small leaf functions → libraries already matched elsewhere (CRI ADXT exists in recvx-decomp) →
  game systems in the order the PC port needs them: boot/main loop, file I/O, input, then rendering
  front-end, then gameplay, then networking.
- Track progress per game in `build/<ELF>/progress.json` (functions and bytes matched); list on decomp.dev
  once there is a public repo.
- Use m2c for first drafts, decomp.me scratches for hard functions, and decomp-permuter for register allocation.
- **Check:** progress grows every week; both builds stay OK on every commit.

## Phase 4: IOP side

- List the IOP modules on each disc (`tools/iso_list.py`). Sony's modules (CDVD, pad, memory card, network
  stack) get replaced on PC, not decompiled. Capcom's own modules, such as a sound driver, get decompiled.
- **Check:** every IRX on the disc is classified as "replace on PC" or "decompile".

## Phase 5: PC port (from the same source)

A `PLATFORM_PC` build compiles the decompiled C with a modern compiler and links a platform layer in place
of the PS2 libraries:

| PS2 piece | PC replacement |
|---|---|
| libgraph/libdma/GIF/GS packets | native renderer (Vulkan or D3D11/12); start with a slow "packet → GS" reference path for debugging, then native renderers per draw type (OpenGOAL's approach) |
| VU1 microcode | port each microprogram to CPU/GPU code; never emulate VU1 |
| VU0 macro / MMI | C helpers (SSE) |
| CDVD + AFS | read files straight from the user's disc image |
| SPU2 + sound driver | software mixer; ADX via vgmstream-style decoder |
| Sofdec movies | FFmpeg |
| pad / memory card | SDL input; save files on disk |
| network stack | see Phase 6 |

The game logic stays at its original 30 Hz tick, which online play needs; rendering can interpolate on top later.
- **Check:** f2 boots to the title screen, then plays a scenario offline, from a user-supplied image.

## Phase 6: Online on Obsrv

- Keep the game's own network code (lobby protocol and in-game messages) unchanged. The server relays
  in-game packets without reading them, so PS2 peers accept only exactly what a PS2 would send.
- Replace only the transport: Sony's network IOP stack becomes native sockets; DNAS and the KDDI login pages
  go over HTTPS to the same hosts Obsrv answers on (legacy TLS ciphers, HTTP/1.0).
- f1 only: the server pushes the v1.01 code patch; the PC build must ship v1.01 behaviour built from source.
- Test against a local copy of the open-source server (AGPL, `corbin-zip/bioserver`) with PCSX2 clients first;
  go live on Obsrv only with the admins' agreement.
- **Check:** a mixed PC + PCSX2 room on a local server completes a scenario without desync.

## Phase 7: English (baseline done ✅)

- 2026-10-11: the v2.1 patch was measured ([TRANSLATION.md](TRANSLATION.md)): text data plus one instruction per
  game. `f1en`/`f2en` targets rebuild byte-identical.
- Remaining: support Silent Hell & REfan988's English patch (GPL-3.0) by building the patched executable as a third and
  fourth target (`f1en`, `f2en`) and reading the patched data files from the user's patched image. See
  [TRANSLATION.md](TRANSLATION.md).
- **Check:** the PC build shows English text from a user-patched image and still plays with unpatched JP players.

## Realistic size

Comparable matching decomps took 2–6 years with many contributors (SM64 ≈2 years, OoT ≈21 months, Paper Mario
3+ years, DMC3 at ~14% after months of work). Two games share much of their engine, which helps. Phases 1–2
fit in a semester solo; Phase 3 is where contributors matter.
