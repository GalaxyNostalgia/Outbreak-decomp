# Ground rules

This is a fan preservation project. These rules keep it on the safe side of what comparable projects
(SM64, Ocarina of Time, DMC3, Code: Veronica X, the Outbreak recomps) do. It's not legal advice.

1. **Never commit or distribute game data:** no disc images, executables, IOP modules, assets, extracted
   files, disassembly or prebuilt binaries of the game or the PC build that contain game code. `tools/firewall.py`
   enforces the obvious cases in CI and as a pre-commit hook (`python -I tools/firewall.py --install`).
2. **Users bring their own discs.** Builds start from the user's dump, verified by sha1.
3. **No money.** No donations tied to the port, no paid builds, no ads.
4. **No leaked material.** Don't use leaked source code, SDKs or debug builds you don't legitimately have.
   Contributors only use Sony SDK headers/libraries they obtained legitimately; the repo doesn't ship them.
5. **Respect the community's work.** The Obsrv server (AGPL-3.0) and the English patch (GPL-3.0) have their own
   licences and authors; ask before building anything that depends on them going public, and never use the PC
   build for cheating on Obsrv.
6. **If Capcom asks, comply.** Take the repo down on a DMCA notice; don't counter-file (Take-Two vs re3 shows
   that can turn into a lawsuit).

Background: the EU Software Directive (2009/24/EC, transposed in Croatia's ZAPSP) allows decompiling only for
interoperability with an independently created program, and Art. 6(2) excludes making a program
"substantially similar in its expression", which a matching decomp is. The project relies on Capcom's
tolerance, which so far has extended to Obsrv (since 2014) and existing Outbreak decomp/recomp projects, but
not to fan remakes that reused Capcom assets.
