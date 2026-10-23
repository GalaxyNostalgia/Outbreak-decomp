#ifndef OPTIONS_H
#define OPTIONS_H

#include "common.h"

/* The game's settings and per-session state (0xE0 bytes, cleared in gameinit.c). Field names
   are what the code does with them so far. */
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3;
    u8 unk4;
    u8 sound;           /* 0x05: 0/1 (stereo/mono?) */
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 bgmVol;          /* 0x09: 0-7 */
    u8 seVol;           /* 0x0A: 0-7 */
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    s8 unkF;            /* 0x0F: narrower screen (0x200 wide) */
    u8 unk10[5];
    u8 unk15;           /* 0x15: 1 runs SUBMAIN's func_349EC0 each frame */
    u8 unk16;           /* 0x16 */
    s8 unk17;           /* 0x17: 1 (always, set in func_001AB900): files come from the AFS archives by name; 0: loose files under data/rom (development) */
    u8 unk18;
    u8 unk19;
    u16 unk1A;          /* 0x1A: the room (r%03d files) */
    u16 unk1C;          /* 0x1C: the scenario's room variant (r%03d%02d%02d) */
    u16 unk1E;
    u16 unk20;
    u16 unk22;
    u16 unk24;
    u16 unk26;
    u8 unk28;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    u8 unk2C;           /* below 6 */
    s8 adjustX;         /* 0x2D: screen position, -10..10 */
    s8 adjustY;         /* 0x2E */
    u8 vibration;       /* 0x2F: 0/1 */
    u8 brightness;      /* 0x30: 0x38..0xA4, default 0x80 */
    u8 unk31;
    u8 online;          /* 0x32: 1 online (loads NETAQ.BIN), 0 offline (SINGLE.BIN) */
    u8 unk33[5];
    u8 ovl54;           /* 0x38: overlay loaded at 0x540000 */
    u8 ovl82;           /* 0x39: overlay loaded at 0x820000 */
    u8 unk3A;
    s8 unk3B;           /* 0x3B: 1 or 2 while a game runs */
    u8 unk3C;
    u8 unk3D;           /* 0x3D: the "u" message files (r%03du.bin, English patch?) */
    u8 unk3E;
    u8 adxBusy;         /* 0x3F: an ADX stream is reading */
    u8 unk40[5];
    u8 unk45;           /* 0x45: 1 draws the screen effects (func_003479E0) */
    u8 unk46[0x4C - 0x46];
    s32 unk4C;          /* 0x4C: a big work area (in game: +0x2CF000 is the frame's scratch) */
    u8 unk50[0x54 - 0x50];
    s32 fontTbp;        /* 0x54: GS page for the font's glyphs */
    u8 unk58[0x5C - 0x58];
    s32 rdtSize;        /* 0x5C: the room's .rdt file */
    u8 *unk60;          /* 0x60: (the same work memory as unk4C; the room's .rdt) */
    u8 *script;         /* 0x64: the room script buffer (r%03d.evb) */
    u8 *unk68;          /* 0x68: work memory (+0x40000: the messages' data, offline) */
    u8 *unk6C;          /* 0x6C: work memory (the messages' data and files, the other mode) */
    u8 unk70[0x78 - 0x70];
    u8 *speech;         /* 0x78: the room's speech data (sp/spm files) */
    s32 *message;       /* 0x7C: the room's messages (r%03d.bin) */
    u8 unk80[0x84 - 0x80];
    u8 *unk84;          /* 0x84: 0xA8 entries of 0x70 bytes */
    u8 unk88[0x8C - 0x88];
    u8 *roomBuf;        /* 0x8C */
    u8 *mapBuf;         /* 0x90: the room map file (func_006A6620) */
    u8 unk94[0x98 - 0x94];
    u8 *door;           /* 0x98: the room's door%03d.dat (0x4000 bytes) */
    u8 unk9C[0xA0 - 0x9C];
    u8 *fontBuf;        /* 0xA0: where the font file is loaded */
    u8 *rtp;            /* 0xA4: the room's r%02d.rtp */
    u8 *voice;          /* 0xA8: the players' voice partitions (0x10C0 bytes each) */
    s8 *emSet;          /* 0xAC: the room's enemy placements (r%02d.epp) */
    u8 *charData;       /* 0xB0: 0x1324 bytes per character (D_0023BCD0 points here) */
    u8 unkB4;
    u8 unkB5;
} Options;

extern Options D_00306590;

#endif
