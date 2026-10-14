#include "common.h"

/* Motion keys (chunk.c loads them): a track's first and last key times. Key layouts by kind:
   0x11 {u16, s16 time}, 0x12 {u16, s16 time, u16, u16}, 0x13 {u32, u16, s16 time, u16, u16},
   0x21 {f32, f32 time}, 0x22 {f32, f32 time, f32, f32}, 0x23 {u32, f32, f32 time, f32, f32}. */

typedef struct {
    u8 kind;
    u8 interp;
    u16 n;
    u32 keys;           /* offset from D_0023BACC */
} MotionTrack;

extern u32 D_0023BACC;          /* the motions' base address */

void func_001A42D0(u32 base) {
    D_0023BACC = base;
}

/* The first key's time. */
f32 func_001A4360(MotionTrack *t) {
    u8 *k = (u8 *)(t->keys + D_0023BACC);
    f32 f;

    switch (t->kind) {
    case 0x20:
        f = 0.0f;
        break;
    case 0x21:
        f = *(f32 *)(k + 4);
        break;
    case 0x22:
        f = *(f32 *)(k + 4);
        break;
    case 0x23:
        f = *(f32 *)(k + 8);
        break;
    case 0x10:
        f = 0.0f;
        break;
    case 0x11:
        f = *(s16 *)(k + 2);
        break;
    case 0x12:
        f = *(s16 *)(k + 2);
        break;
    case 0x13:
        f = *(s16 *)(k + 6);
        break;
    }
    return f;
}

f32 func_001A4440(MotionTrack *t);

f32 func_001A4430(MotionTrack *t) {
    return func_001A4440(t);
}

/* The last key's time. */
f32 func_001A4440(MotionTrack *t) {
    u8 *k = (u8 *)(t->keys + D_0023BACC);
    f32 f;

    switch (t->kind) {
    case 0x20:
        f = 0.0f;
        break;
    case 0x21:
        f = *(f32 *)(k + t->n * 8 - 4);
        break;
    case 0x22:
        f = *(f32 *)(k + t->n * 16 - 0xC);
        break;
    case 0x23:
        f = *(f32 *)(k + t->n * 20 - 0xC);
        break;
    case 0x10:
        f = 0.0f;
        break;
    case 0x11:
        f = *(s16 *)(k + t->n * 4 - 2);
        break;
    case 0x12:
        f = *(s16 *)(k + t->n * 8 - 6);
        break;
    case 0x13:
        f = *(s16 *)(k + t->n * 12 - 6);
        break;
    }
    return f;
}
