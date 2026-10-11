# Online play and Obsrv

What the PC build has to do to play on [Obsrv](https://www.obsrv.org/) with PS2 and PCSX2 players. This is
from reading the open-source server (ghostline's BioServer 1/2, AGPL-3.0, mirrored at
[corbin-zip/bioserver](https://github.com/corbin-zip/bioserver)) and the [Obsrv wiki](https://wiki.obsrv.org).
The live Obsrv server may differ from the published source.

## Versions

Only the Japanese games work online. US/EU used a different service (SNAP) that nobody emulates.

| | f1 Biohazard Outbreak | f2 Biohazard Outbreak File 2 |
|---|---|---|
| Disc | SLPM-65428 (v1.00; v1.01 comes from the server) | SLPM-65692 |
| KDDI title directory | `/00000002/` | `/00000010/` |
| Lobby (TCP) | 8300 | 8200 |
| Game relay (TCP) | 8690 | 8590 |

## Connection sequence

1. **DNS.** The game resolves `gate1.jp.dnas.playstation.org` (DNAS) and `www01.kddi-mmbb.jp` / `kddi-mmbb.jp`
   (login + lobby). Obsrv players point these at Obsrv: DNAS → 153.121.44.150, KDDI → 208.72.237.15
   (current per the wiki; they have changed before, so make them configurable).
2. **DNAS (HTTPS 443).** Very old TLS: the PS2 offers `EDH-RSA-DES-CBC3-SHA` / `EDH-DSS-DES-CBC3-SHA`, sends
   User-Agent `open sesame asdfjkl` and needs an HTTP/1.0 reply. Endpoints under `/gai-gw/`: `v2.5_i-connect`,
   `v2.5_d-connect`, `v2.5_others`. (The English patch re-enables DNAS checks.)
3. **KDDI login pages (HTTPS 443, self-signed OK).** HTML pages whose answers sit in comments as CSV, in EUC-JP:
   `<!--CRS-game-start-->` gives `"OK","<8-digit session>",<next URLs>`;
   `<!--CRS-lbs-info-get-->` gives the lobby `host:port`.
4. **Lobby (plain TCP).** 12-byte big-endian header: `who` (0x18 server, 0x81 client, 0x28 game server,
   0x82 game client), `qsw` (0x01 query, 0x02 answer, 0x10 broadcast), `cmd` (2), payload length (2),
   packet id (2), `err` (1), `FF FF FF`, then the payload. About 90 commands are known (LOGIN 0x6101,
   HEARTBEAT 0x6202 every 30 s, CONNCHECK 0x6001 every 60 s, CHATIN/OUT 0x6701/0x6702, GSINFO 0x6916 …);
   several are still unknown. Strings (names, chat, passwords) are XOR-scrambled with a key derived from the
   packet id, not encrypted. The session id travels as two 5-digit halves offset by the packet id.
5. **Game relay (plain TCP).** First packet GSLOGIN 0x1031 with the session. After that the server copies
   every packet unchanged to everyone else in the same game. It never parses or validates gameplay.

## What this means for the PC build

- **In-game traffic is undocumented and peer-interpreted.** PS2 peers read the PC's packets directly. The only
  safe way to be compatible is to run the game's own message code, built from the decompiled source, with the
  original 30 Hz logic tick. Do not "rewrite the netcode".
- **Replace the transport, not the protocol.** The decompiled game calls Sony's network library (socket API on
  the IOP side via the DEV9 network adapter). On PC, that layer becomes native sockets plus an HTTPS client
  that can still speak the legacy ciphers (a bundled OpenSSL 1.0.2-era build or a small custom TLS1.0 client).
  DNAS could be stubbed locally only if the later login steps don't depend on its result; verify first.
- **f1 v1.01 patch.** BioServer 1 sends a 0x7AA0-byte binary patch (code) in 0x100-byte chunks when the client
  reports an old patch level. The PC build must behave as v1.01: decompile the patch payload too, from a
  capture of your own session, and build it in.
- **Hosts and ports configurable**, defaulting to Obsrv's.
- **No cheats online.** Obsrv bans cheating; PC-only options (60 fps, widescreen, mods) must be off or
  provably non-gameplay when connected.

## Testing plan

1. Run BioServer 2 locally (Docker images exist: `yabab/bioserver-docker`, `TAB64-K/ps2-outbreak-server`).
2. Connect two PCSX2 instances (DEV9 sockets mode, DNS pointed at the local server) and record traffic with
   Wireshark: this is the reference for every PC test.
3. Replay and compare: the PC build's lobby and game packets must match PCSX2's byte-for-byte for the same inputs.
4. Mixed PC + PCSX2 rooms on the local server; then ask the Obsrv admins (Discord / forum) before testing live.
