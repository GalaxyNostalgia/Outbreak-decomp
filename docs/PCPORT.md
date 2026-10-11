# PC port

The native PC (Windows) version lives in its own repository,
[GalaxyNostalgia/Outbreak-PC](https://github.com/GalaxyNostalgia/Outbreak-PC), which includes this repository
as a git submodule and compiles the C in `src/` natively.

Rules for C written here, so it works in both builds:
- Refer to game data through its symbols (`extern char D_00224268[];`), never through a hard-coded address.
  The PC build turns the user's data into linkable assembly and gives every symbol a native address.
- Keep structs as the game lays them out (the PC build is 32-bit, so pointers stay 4 bytes).
- Code that only makes sense on one side goes under `#ifdef PLATFORM_PC`.
