#include "system.h"
#include "ps2mem.h"

/* Flat 2D rectangles (fades, the screen clear) built as GS packets. */

s32 func_0018CA60(s32 mode, u32 entry, u32 *tag, DmaQueue *q);
u32 func_0018D850(u32 size, s32 align);

void func_00189618(u128 *src, u128 *dst, s32 n);   /* copy n quadwords (assembly) */

/* Queue a GS packet (DMA tag, VIF codes, GIF data) built in place: copied to the frame's packet buffer
   and sent through VIF1 DIRECT. */
s32 func_001937B0(u32 *pkt) {
    s32 n = (pkt[0] & 0x7FFF) + 1;
    u32 dst = func_0018D850(n * 16, 16);

    pkt[0] |= 0x80000000;
    pkt[2] = 0x13000000;            /* FLUSHA */
    pkt[3] = (n - 1) | 0x51000000;  /* DIRECT n - 1 */
    func_00189618((u128 *)pkt, (u128 *)dst, n);
    func_0018CA60(0, (dst & 0x0FFFFFFF) | 0x40000000, (u32 *)dst, &D_00281AF0);
    return 1;
}

#ifdef NON_MATCHING   /* register allocation: the byte pointers */
/* PS2 alpha (0-128) from 0-255: the alpha only, or (all 0) every channel. */
u32 func_001982C0(u32 c, s32 alphaOnly) {
    u8 b[4];
    s32 i;

    b[0] = c >> 16;
    b[1] = c >> 8;
    b[2] = c;
    b[3] = c >> 24;
    if (alphaOnly == 0) {
        for (i = 0; i < 4; i++) {
            if (b[i] == 0xFF) {
                b[i] = 0x80;
            } else if (b[i] != 0) {
                b[i] >>= 1;
                if (b[i] == 0) {
                    b[i] = 1;
                }
            }
        }
    } else {
        if (b[3] == 0xFF) {
            b[3] = 0x80;
        } else if (b[3] != 0) {
            b[3] >>= 1;
            if (b[3] == 0) {
                b[3] = 1;
            }
        }
    }
    return b[2] | ((b[1] << 8) | ((b[3] << 24) | (b[0] << 16)));
}
#endif

typedef struct {
    s16 x0, y0;
    s16 x1, y1;
    u32 color;      /* 0xAARRGGBB */
} Rect2D;

extern u32 D_00216460[];    /* sprite packet: tags, PRIM, RGBAQ, XYZ2 x2 */

#ifdef NON_MATCHING   /* register allocation, one instruction more */
/* Draw a rectangle given in 640x448 screen coordinates. */
s32 func_001AAE10(Rect2D *r) {
    s32 ox;
    s32 oy;
    s32 x0;
    s32 x1;
    s32 y0;
    s32 y1;
    f32 sx;
    u32 c;

    if (D_00281DB0.unk3F4 != 0) {
        return 1;
    }
    ox = (0x1000 - D_00281DB0.width) / 2 * 16;
    oy = (0x1000 - D_00281DB0.dispHeight) / 2 * 16;
    sx = D_00281DB0.width / 640.0f;
    y0 = r->y0 * 16;
    x0 = 16.0f * (r->x0 * sx);
    x1 = 16.0f * (r->x1 * sx);
    y1 = r->y1 * 16;
    if (D_00281DB0.field == 0) {
        y0 >>= 1;
        y1 >>= 1;
    }
    x0 += ox;
    x1 += ox;
    y0 += oy;
    y1 += oy;
    func_00189618((u128 *)D_00216460, (u128 *)PS2_SPR(0x70000000), 3);
    c = func_001982C0(r->color, 1);
    *(u64 *)PS2_SPR(0x70000028) = ((u64)((c >> 16) & 0xFF) | ((u64)((c >> 8) & 0xFF) << 8) |
                                   ((u64)(c & 0xFF) << 16) | ((u64)((c >> 24) & 0xFF) << 24)) | ((u64)1 << 32);
    *(u64 *)PS2_SPR(0x70000030) = ((u64)(u32)D_00281DB0.zMax << 32) | ((s64)x0 | ((s64)y0 << 16));
    *(u64 *)PS2_SPR(0x70000038) = ((u64)(u32)D_00281DB0.zMax << 32) | ((s64)x1 | ((s64)y1 << 16));
    func_001937B0((u32 *)PS2_SPR(0x70000000));
    return 1;
}
#endif

extern u64 D_00213520[];    /* full-screen packet */
extern s32 D_0023BA9C;
extern s32 D_0023BAA0;

#ifdef NON_MATCHING   /* load order of the offsets */
/* Clear the screen to a colour: mode 0 with Z test, 1 without. */
s32 func_001A0170(u8 mode, u32 c) {
    switch (mode) {
    case 0:
        D_00213520[4] = 0x3200F;
        break;
    case 1:
        D_00213520[4] = 0x30003;
        break;
    default:
        return 0;
    }
    D_00213520[8] = ((u64)((c >> 16) & 0xFF) | ((u64)((c >> 8) & 0xFF) << 8) | ((u64)(c & 0xFF) << 16)) |
                    ((u64)1 << 32);
    D_00213520[10] = (D_00281DB0.offsetX << 4) | ((u64)(D_00281DB0.offsetY << 4) << 16);
    D_00213520[14] = D_00281DB0.test[0];
    D_00213520[12] = ((D_00281DB0.offsetX + D_0023BAA0) << 4) | ((u64)((D_00281DB0.offsetY + D_0023BA9C) << 4) << 16);
    func_001937B0((u32 *)D_00213520);
    return 0;
}
#endif
