#ifndef DRAWLIST_H
#define DRAWLIST_H

#include "common.h"

/* A double-buffered list of GS packets the game builds each frame (D_0029CD80 the main one,
   D_0029CCD0 a second one sent straight to the GIF). */
typedef struct {
    s32 unk0;           /* 0x00 */
    s32 mode;           /* 0x04: the font (index into D_00216A80) */
    s32 encoding;       /* 0x08: 0 Shift-JIS, 1 EUC, 2 UTF-8 */
    s32 glyphSize;      /* 0x0C: pixels */
    s32 packed;         /* 0x10: glyphs are 2-bit and get expanded */
    s32 unk14;          /* 0x14 */
    s32 uploadBytes;    /* 0x18: bytes per glyph as uploaded */
    s32 glyphs;         /* 0x1C */
    s32 glyphBytes;     /* 0x20: bytes per glyph in the font file */
    s32 cluts;          /* 0x24 */
    u8 *font;           /* 0x28: the font file */
    u8 *glyphBuf[2];    /* 0x2C: glyphs to upload this frame */
    u32 *clut;          /* 0x34 */
    u32 head[2];        /* 0x38: DMA address of each buffer's first packet */
    u32 *buf[2];        /* 0x40: each buffer's start */
    s32 cur;            /* 0x48: buffer being filled */
    u32 *ptr;           /* 0x4C: where the next packet goes */
    s32 unk50;          /* 0x50 */
    s32 tbp;            /* 0x54: GS texture page for the glyphs */
    s32 cbp;            /* 0x58: and their palettes */
    s32 unk5C;          /* 0x5C */
    u32 maxGlyphs;      /* 0x60 */
    u32 count;          /* 0x64: packets (glyphs) this frame */
    s32 lastCount;      /* 0x68 */
    u32 size;           /* 0x6C: advance */
    s32 x;              /* 0x70 */
    s32 y;              /* 0x74 */
    s32 z;              /* 0x78 */
    s32 clutIndex;      /* 0x7C */
    u32 color;          /* 0x80 */
    s32 unk84;          /* 0x84 */
    s32 unk88;          /* 0x88 */
    u32 glyphCount;     /* 0x8C: glyphs in the font */
    s32 unk90;          /* 0x90 */
    s32 unicode;        /* 0x94: the font maps all of Unicode (else ASCII is drawn as is) */
    u32 height;         /* 0x98 */
    s32 unk9C;          /* 0x9C */
    s32 unkA0;          /* 0xA0 */
    s32 unkA4;          /* 0xA4: no shadow */
    u8 unkA8[0xB0 - 0xA8];
} DrawList;

extern DrawList D_0029CCD0;
extern DrawList D_0029CD80;
extern DrawList *D_00239F80;    /* the list text goes to */

#endif
