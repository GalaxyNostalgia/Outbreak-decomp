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
    u8 unkF;
    u8 unk10[5];
    u8 unk15;           /* 0x15: 1 runs SUBMAIN's func_349EC0 each frame */
    u8 unk16;           /* 0x16 */
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u16 unk1A;
    u16 unk1C;
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
    u8 unk3B;
    u8 unk3C;
    u8 unk3D;
    u8 unk3E;
    u8 adxBusy;         /* 0x3F: an ADX stream is reading */
    u8 unk40[0x54 - 0x40];
    s32 fontTbp;        /* 0x54: GS page for the font's glyphs */
    u8 unk58[0x8C - 0x58];
    u8 *roomBuf;        /* 0x8C */
    u8 unk90[0xA0 - 0x90];
    u8 *fontBuf;        /* 0xA0: where the font file is loaded */
    u8 unkA4[0xB5 - 0xA4];
    u8 unkB5;
} Options;

extern Options D_00306590;

#endif
