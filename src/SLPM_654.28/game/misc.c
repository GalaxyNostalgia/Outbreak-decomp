#include "common.h"

void func_005B47E0(Vec3 *v, f32 x, f32 y, f32 z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

typedef struct {
    u8 unk0[9];
    u8 unk9;
    u8 unkA[2];
    s16 unkC;
    u8 unkE[0xC0 - 0xE];
    s32 unkC0;
    u8 unkC4[8];
    s32 unkCC;
    u8 unkD0[8];
    s32 unkD8;
    u8 unkDC[0x14];
    s32 unkF0;
} Unk575440;

void func_00575440(Unk575440 *p) {
    p->unk9 = 1;
    p->unkC0 = 0;
    p->unkCC = 0;
    p->unkF0 = 0;
    p->unkD8 = 0;
    p->unkC = 0;
}

typedef struct { s32 unk0; s32 unk4; } Unk5C7F90a;
typedef struct { u8 unk0[0x54F]; u8 unk54F; } Unk5C7F90b;

s32 func_005C7F90(Unk5C7F90a *a, Unk5C7F90b *b) {
    return a->unk4 == b->unk54F;
}

typedef struct { u8 unk0[0x51C]; s32 unk51C; } Unk55CB70;
extern void func_0055CB90(Unk55CB70 *p, s32 a, s32 b, s32 c, s32 d);

void func_0055CB70(Unk55CB70 *p, s32 a, s32 b, s32 c) {
    func_0055CB90(p, p->unk51C, a, b, c);
}
