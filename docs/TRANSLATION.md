# English translation

The English patch for the Japanese games is by **REfan988 & Silent Hell**
([SilentHeII/Outbreak-Translation](https://github.com/SilentHeII/Outbreak-Translation), GPL-3.0). Release v2.0
covers File 1 & 2. It expects Redump-verified images
([redump.org/disc/476](http://redump.org/disc/476/), [redump.org/disc/9533](http://redump.org/disc/9533/)).
It re-enables DNAS checks and adds an L2 toggle for half-width English characters on the keyboard. v2.1 adds
translated map names. It's the patch Obsrv's English-speaking players use, and they play with unpatched
Japanese players, so it doesn't change the network messages.

## How this project supports it

We never ship the patch's data or a patched image. The user applies the official patch to their own image,
and the build reads that.

1. **Find out what the patch changes.** Patch a copy of each image, then
   `python -I tools/iso_list.py orig/<ELF>/game.iso --diff patched.iso`. Expect changed text/font archives and a
   changed executable.
2. **The patched executable becomes its own build target** (`f1en`, `f2en` in `config/versions.json` with its
   own sha1). Most functions are identical to the Japanese build, so the same C builds both. The handful the
   patch changes (text layout, keyboard, DNAS) get `#if VERSION_EN` variants that reproduce the patched bytes.
3. **PC build:** a setting chooses Japanese or English; English needs the user's patched image (or the patched
   data files next to the original image).
4. **Credit and coordinate** with Silent Hell before publishing anything that depends on the patch. If the
   project later wants to bundle translated text, it must comply with GPL-3.0 and get the authors' blessing;
   the game's own assets still never go in the repo.
