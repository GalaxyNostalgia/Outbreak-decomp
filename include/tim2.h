#ifndef TIM2_H
#define TIM2_H

#include "common.h"

/* Sony's TIM2 picture files. */
typedef struct {
    u8 magic[4];            /* "TIM2" */
    u8 version;             /* 3 or 4 */
    u8 format;              /* 0: parts aligned to 16 bytes, 1: to 128 */
    u16 pictures;
    u8 pad[8];
} Tim2File;

typedef struct {
    u32 totalSize;
    u32 clutSize;
    u32 imageSize;
    u16 headerSize;
    u16 clutColors;
    u8 pictFormat;
    u8 mipmaps;             /* 0x11 */
    u8 clutType;            /* 0x12: 1 16-bit, 2 24-bit, 3 32-bit */
    u8 imageType;           /* 0x13: 1-3 as clutType, 4 4-bit, 5 8-bit indexed */
    u16 width;              /* 0x14 */
    u16 height;             /* 0x16 */
    u64 gsTex0;
    u64 gsTex1;
    u32 gsTexaFbaPabe;
    u32 gsTexClut;
} Tim2Picture;

/* A pixel format as the texture code wants it. */
typedef struct {
    s32 bits;
    s32 shift;
    s32 mask;
} PixChannel;

typedef struct {
    u32 flags;              /* 0x04 indexed, 0x10 4-bit */
    s32 width;              /* 0x04 (for a palette: colours) */
    s32 height;             /* 0x08 */
    s32 pitch;              /* 0x0C: bytes per row */
    s32 unk10;
    s32 bpp;                /* 0x14: bytes per pixel, 0 for 4-bit */
    PixChannel r, g, b, a;  /* 0x18 */
} PixFormat;

s32 func_001A4CD0(Tim2File *tim);

Tim2Picture *func_001A4DE0(Tim2File *tim, s32 index);
u8 *func_001A4E60(Tim2File *tim, s32 index, s32 mip);
u8 *func_001A4F40(Tim2File *tim, s32 index);
s32 func_001A47A0(Tim2File *tim, s32 unused);
s32 func_001A47F0(PixFormat *fmt, Tim2File *tim);
s32 func_001A4A90(PixFormat *fmt, Tim2File *tim, s32 unused);
u8 *func_001A4CB0(Tim2File *tim, s32 mip);
u8 *func_001A4CC0(Tim2File *tim);

#endif
