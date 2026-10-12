#ifndef FLTEX_H
#define FLTEX_H

#include "common.h"
#include "tim2.h"

/* Capcom's flPS2 texture manager: textures and palettes kept in main memory and given room in
   GS memory (VRAM) when they're used. A texture handle has the texture's slot + 1 in its low
   16 bits and its palette's slot + 1 in the high 16. */

/* GS pixel formats */
#define PSMCT32  0x00
#define PSMCT24  0x01
#define PSMCT16  0x02
#define PSMCT16S 0x0A
#define PSMT8    0x13
#define PSMT4    0x14

typedef struct {
    u32 owner;          /* 0x00: how it's kept: 1/4 stay in main memory until used */
    u32 flags;          /* 0x04 */
    s32 size;           /* 0x08: bytes, every mipmap level */
    s32 bpp;            /* 0x0C: bytes per pixel (0 for 4-bit) */
    s16 blocks;         /* 0x10: VRAM it needs, in 256-byte blocks */
    s16 align;          /* 0x12: VRAM alignment in blocks */
    s16 vram;           /* 0x14: its block address once in VRAM */
    s16 tw;             /* 0x16: log2 width */
    s16 th;             /* 0x18: log2 height */
    s16 w;              /* 0x1A */
    s16 h;              /* 0x1C */
    s16 pad1E;
    s32 psm;            /* 0x20 */
    s32 mem;            /* 0x24: main-memory handle of the pixels */
    s16 sw;             /* 0x28: size when swizzled to PSMCT32 */
    s16 sh;             /* 0x2A */
    s32 unk2C;
    u32 mips;           /* 0x30 */
    s8 used;            /* 0x34 */
    s8 inVram;          /* 0x35 */
    s8 swizzled;        /* 0x36: stored as PSMCT32 */
    s8 unk37;
} FlTexture;

/* VRAM allocations, a list in address order */
typedef struct FlVramNode {
    struct FlVramNode *prev;
    struct FlVramNode *next;
    FlTexture *tex;     /* 0x08 */
    u32 flags;          /* 0x0C */
    u32 owner;          /* 0x10 */
    s16 addr;           /* 0x14: 0 = free node */
    s16 size;           /* 0x16 */
    s16 align;          /* 0x18 */
    s16 pad1A;
} FlVramNode;

extern FlTexture D_0027AAC0[0x100];     /* textures */
extern FlTexture D_002772C0[0x100];     /* palettes */
extern FlVramNode D_0027E2F0[0x200];
extern FlVramNode *D_0023BA6C;          /* first allocation */
extern s32 D_0023BA68;                  /* VRAM nodes in use */
extern s16 D_00281DF8;                  /* first VRAM block for textures */

s32 func_0018D8C0(Tim2File *tim, u32 owner);

#endif
