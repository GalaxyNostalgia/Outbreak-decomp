#include "common.h"
#include "options.h"
#include "effect.h"
#include "vu0.h"

/* The room's lights and light effects (from the room's .rdt parts). */

typedef struct {
    u8 kind;            /* 0x00 */
    u8 flags;           /* 0x01: 1 ..., 2 ..., 4 ..., 8 a blink, 0x10 its own matrix */
    u8 mtx;             /* 0x02: the room matrix it follows */
    u8 unk3;            /* 0x03 */
    u8 unk4;
    u8 blink[2];        /* 0x05 */
    u8 unk7;            /* 0x07 */
    u16 unk8;           /* 0x08 */
    u16 unkA;           /* 0x0A */
    u8 unkC[0x1C - 0xC];
    s32 unk1C;          /* 0x1C */
    f32 unk20[2];       /* 0x20 */
    u8 unk28[0x30 - 0x28];
} RoomLight;            /* 0x30 */

extern RoomLight D_00363310[];
extern s32 D_003639A0[];        /* [0]: lights */
extern u8 D_0036397C[];
extern u8 D_00341EB0[];
void func_005470B0(Obj250L *p);
Obj250L *func_00620280(Obj250L *at, u8 list, s8 where);

void func_00546B70(s32 i);
#ifdef NON_MATCHING   /* first draft */
void func_00546B70(s32 i) {
    RoomLight *l;
    Obj250L *p;
    u8 *q;
    s32 k;

    if (D_003639A0[0] == 0 || D_003639A0[0] < i) {
        return;
    }
    l = &D_00363310[i];
    p = func_00620280(NULL, 1, 0);
    if (p == NULL) {
        return;
    }
    q = (u8 *)p;
    p->unk0 = 1;
    p->update = func_005470B0;
    p->unk4 = 0;
    p->draw = D_00341EB0;
    *(s32 *)(q + 0x44) = 0;
    *(s32 *)(q + 0x48) = 0;
    *(s32 *)(q + 0x4C) = 0;
    p->unkC = 0;
    p->unkE = 0;
    p->unk10 = 0;
    for (k = 0; k < 0x20; k++) {
        ((s16 *)(q + 0xF0))[k] = 0;
    }
    p->flags = 0xC3D3;
    if (l->flags & 1) {
        p->flags |= 0x800;
        if (l->flags & 2) {
            p->flags |= 0x1000;
        }
    }
    p->unk23A = 0;
    *(s32 *)(q + 0x228) = 0;
    *(s32 *)(q + 0x22C) = 0;
    if (l->flags & 4) {
        p->unk23A = 1;
    }
    if (l->flags & 0x10) {
        p->unk23A = 0x10;
    }
    q[0x13A] = 0;
    if (l->flags & 8) {
        *(f32 *)(q + 0x130) = 1.0f / l->blink[0];
        *(f32 *)(q + 0x134) = 1.0f / l->blink[1];
        q[0x138] = l->blink[0];
        q[0x139] = l->blink[1];
        q[0x13A] = 1;
        *(s32 *)(q + 0x18C) = l->unk1C;
        p->unkC = l->unk7;
        p->unkE = l->unk7;
    }
    p->unk10 = 0;
    p->unk12 = 0;
    q[0x32] = l->unk3 + 1;
    p->unk30 = l->mtx;
    *(s16 *)(q + 0x238) = l->kind;
    *(u16 *)(q + 0x23C) = l->unk8;
    *(u16 *)(q + 0x23E) = l->unkA;
    p->unk1F4 = D_0036397C;
    *(f32 *)(q + 0x198) = l->unk20[0];
    *(f32 *)(q + 0x19C) = l->unk20[1];
    p->target = l;
    if (!(p->unk23A & 0x10)) {
        p->mtx = (Mat44 *)(*(u8 **)((u8 *)&D_00306590 + 0x68) + (p->unk30 << 6));
    } else {
        func_0018F7C0((f32 *)(q + 0x50));
        p->mtx = (Mat44 *)(q + 0x50);
        p->flags |= 0x10000;
    }
    p->unk5 = i;
}
#endif

typedef struct {
    u8 kind;            /* 0x0: 0xFF none; 0x80 a light of its own */
    u8 unk1;
    u8 unk2;
    u8 unk3;
    s32 unk4;
} RoomFx;               /* 0x8 */

extern RoomFx D_00363910[];
extern s32 D_0036399C;
extern u8 D_00389750[];
extern u8 D_004BEEF0[][0x4C];
Obj250L *func_00620BE0(Obj250L *at, void *update, u8 list, s8 where);
void func_00547530(Obj250L *p);
void func_003419F0(s32 a, void *update);
s32 func_001D04D0(s32 base, u16 id);

void func_00546EB0(void);
#ifdef NON_MATCHING   /* first draft */
void func_00546EB0(void) {
    RoomFx *f;
    Obj250L *p;
    s32 i;
    s32 done = 0;
    u16 tex;

    for (i = 0, f = D_00363910; i < D_0036399C; i++, f++) {
        if (f->kind == 0xFF) {
            continue;
        }
        if (f->kind & 0x80) {
            p = func_00620BE0(NULL, func_00547530, 0, 1);
            if (p != NULL) {
                u8 *q = (u8 *)p;

                q[0x13B] = f->kind & 0x7F;
                q[0x13D] = f->unk1;
                q[0x13F] = f->unk1;
                q[0x13E] = f->unk2;
                q[0x13C] = f->unk3;
                p->unk10 = f->unk4;
            }
        } else {
            func_003419F0(1, func_00547530);
            if (done == 0) {
                tex = *(u16 *)(D_00389750 + 0x502C + f->kind * 2);
                *(s32 *)(D_004BEEF0[tex] + 0x44) = func_001D04D0(0xFF0, tex);
                done++;
            }
        }
    }
}
#endif

typedef struct {
    u8 unk0[2];
    s16 unk2;           /* 0x2 */
    f32 unk4[2];        /* 0x4 */
} RoomFx2;              /* 0xC */

extern RoomFx2 D_003632F0[];
extern s32 D_003639A4;
void func_00547680(Obj250L *p);

void func_00546FE0(void);
#ifdef NON_MATCHING   /* first draft */
void func_00546FE0(void) {
    RoomFx2 *f;
    Obj250L *p;
    s32 i;
    s32 k;

    for (i = 0, f = D_003632F0; i < D_003639A4; i++, f++) {
        p = func_00620BE0(NULL, func_00547680, 0, 1);
        if (p != NULL) {
            u8 *q = (u8 *)p;

            p->unk0 = 1;
            p->unk1 = 0;
            p->unkC = 0;
            p->unkE = 0;
            p->unk10 = 0;
            p->unk1FF = i;
            for (k = 0; k < 0x20; k++) {
                ((s16 *)(q + 0xF0))[k] = 0;
            }
            q[0x32] = f->unk2 + 1;
            p->unk1F4 = D_0036397C;
            *(f32 *)(q + 0x190) = f->unk4[0];
            *(f32 *)(q + 0x194) = f->unk4[1];
        }
    }
}
#endif

extern s16 D_0023BB98;
extern s16 D_0023BB94;
extern s32 D_0048AE9C[];

void func_00546E30(void) {
    s32 i;

    func_00546EB0();
    func_00546FE0();
    for (i = 0; i < D_003639A0[0]; i++) {
        func_00546B70(i);
    }
    if (D_0023BB98 == 1 || D_0023BB94 != 0) {
        D_0048AE9C[0] = -1;
    }
}
