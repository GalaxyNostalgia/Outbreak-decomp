#ifndef DRAWLIST_H
#define DRAWLIST_H

#include "common.h"

/* A double-buffered list of GS packets the game builds each frame (D_0029CD80 the main one,
   D_0029CCD0 a second one sent straight to the GIF). */
typedef struct {
    s32 unk0;           /* 0x00 */
    s32 mode;           /* 0x04 */
    u8 unk8[0x38 - 0x8];
    u32 head[2];        /* 0x38: DMA address of each buffer's first packet */
    u32 *buf[2];        /* 0x40: each buffer's start */
    s32 cur;            /* 0x48: buffer being filled */
    u32 *ptr;           /* 0x4C: where the next packet goes */
    s32 unk50;          /* 0x50 */
    u8 unk54[0x64 - 0x54];
    s32 count;          /* 0x64: packets this frame */
    s32 lastCount;      /* 0x68 */
    u8 unk6C[0x88 - 0x6C];
    s32 unk88;          /* 0x88 */
    u8 unk8C[0x9C - 0x8C];
    s32 unk9C;          /* 0x9C */
    s32 unkA0;          /* 0xA0 */
    u8 unkA4[0xB0 - 0xA4];
} DrawList;

extern DrawList D_0029CCD0;
extern DrawList D_0029CD80;

#endif
