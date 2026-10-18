#include "common.h"
#include "item.h"
#include "options.h"

/* GAME.BIN: the room's trigger areas (0xC4 bytes, 0x80 of them in pool D_003B2290 with free
   stack D_00386410), set up from the room data; each has a type (0-13) and 0x80 bytes of
   parameters whose layout depends on it. */

typedef struct Aot {
    s32 corner[4][2];   /* 0x00: x, z */
    s32 unk20[2];       /* 0x20 */
    struct Aot *prev;   /* 0x28 */
    struct Aot *next;   /* 0x2C */
    u8 type;            /* 0x30 */
    u8 unk31;           /* 0x31 */
    u8 unk32;           /* 0x32: D_006D6D60[type] */
    u8 unk33[0x3C - 0x33];
    u8 on;              /* 0x3C */
    u8 unk3D;           /* 0x3D */
    u8 unk3E;
    u8 num;             /* 0x3F: its number in the room data + 1 */
    union {
        s32 w[0x20];
        u16 h[0x40];
        u8 b[0x80];
    } p;                /* 0x40: parameters (p.w[0]: its index in the scene's tables) */
    u8 unkC0[4];
} Aot;                  /* 0xC4 */

typedef struct {
    s32 type;           /* 0x00 */
    s32 unk4;           /* 0x04 */
    u8 unk8[0x20];      /* 0x08 */
    u8 unk28[8];        /* 0x28 */
    u8 params[0x80];    /* 0x30 */
} AotDef;

typedef struct {
    s32 unk0;
    u32 n;              /* 0x4 */
    s32 offs[1];        /* 0x8 */
} AotFile;

/* (part of D_00470090) */
typedef struct {
    u8 unk0[0xB20];
    f32 mats[0x80][16];         /* 0xB20: per model (Aot p.w[3]) its matrix */
    u8 unk2B20[0x2E80 - 0x2B20];
    u8 flags[0x4730 - 0x2E80];  /* 0x2E80 */
    u8 entries[0x10][0x60];     /* 0x4730 */
} AotWork;

extern Aot *D_00386410[0x80];   /* the free ones */
extern s16 D_0023BD88;          /* how many */
extern Aot *D_0023BD8C;         /* the list's first */
extern Aot *D_0023BD90;         /* its last */
extern u8 D_006D6D60[];         /* per type */
extern AotWork D_00470090;
extern f32 D_0047092C[];
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *d, const void *s, u32 n);
Aot *func_00595710(Aot *a);
Aot *func_00595AE0(Aot *a);
void func_00593FA0(Aot *a);
void func_005960A0(Aot *a);

/* A new (cleared) area, put first in the list (last when at is 0); NULL when none is free. */
Aot *func_00590BF0(s32 at) {
    Aot *a;

    if (D_0023BD88 == 0) {
        return NULL;
    }
    D_0023BD88--;
    a = D_00386410[D_0023BD88];
    memset(a, 0, sizeof(Aot));
    if (D_0023BD90 == NULL) {
        D_0023BD8C = a;
        D_0023BD90 = a;
        a->next = NULL;
        a->prev = NULL;
    } else if (at == 0) {
        a->prev = D_0023BD90;
        a->next = NULL;
        D_0023BD90->next = a;
        D_0023BD90 = a;
    } else {
        a->prev = NULL;
        a->next = D_0023BD8C;
        D_0023BD8C->prev = a;
        D_0023BD8C = a;
    }
    return a;
}

/* Sets up the room's areas from f; their states come from the room script's first table. */
void func_00590D90(AotFile *f);
#ifdef NON_MATCHING   /* first draft */
void func_00590D90(AotFile *f) {
    u32 i = 0;
    u32 n = f->n;
    u8 *st = *(u8 **)(D_00306590.script + 0x8000);
    s32 *off = f->offs;
    AotDef *d;
    Aot *a;

    for (; i < n; i++, off++) {
        d = (AotDef *)((u8 *)f + *off);
        a = func_00590BF0(0);
        a->type = d->type;
        a->unk31 = d->unk4;
        a->unk3D = 0;
        a->num = i + 1;
        a->unk32 = D_006D6D60[d->type];
        memcpy(a->corner, d->unk8, 0x20);
        memcpy(a->unk20, d->unk28, 8);
        memcpy(&a->p, d->params, 0x80);
        switch (d->type) {
        case 0:
            a->p.w[3] = st[a->p.w[0]];                  /* 0x4C */
            D_00470090.flags[a->p.w[3]] = a->p.w[7];    /* 0x5C */
            a->p.h[0x14] = 0xFFFF;                      /* 0x68 */
            a->on = 1;
            a->p.w[0xB] = -1;                           /* 0x6C */
            D_00470090.entries[a->p.w[0]][0x14] = d->type;
            func_00595710(a);
            break;
        case 1:
            a->on = 1;
            a->p.h[0x23] = 0xFFFF;                      /* 0x86 */
            if (a->p.w[0xB] == 8 || a->p.w[0xB] == 7 || a->p.w[0xB] == 0) {
                a->p.w[0xA] = (u16)(a->p.w[6] - 0x8000);  /* 0x68 from 0x58 */
            }
            break;
        case 2:
            a->p.w[1] = st[a->p.w[0]];                  /* 0x44 */
            a->p.h[0x14] = 0xFFFF;
            D_00470090.flags[a->p.w[1]] = a->p.w[5];    /* 0x54 */
            a->p.w[0xB] = -1;
            D_00470090.entries[a->p.w[0]][0x14] = d->type;
            func_00595AE0(a);
            a->on = 1;
            break;
        case 3:
            a->on = 0;
            a->p.h[0xC] = 0xFFFF;                       /* 0x58 */
            break;
        case 4:
            a->on = 0;
            a->p.w[0xA] = -1;                           /* 0x68 */
            break;
        case 5:
            func_00593FA0(a);
            break;
        case 6:
            a->on = 1;
            a->p.w[8] = st[a->p.w[0] + 0x20];           /* 0x60 */
            func_005960A0(a);
            break;
        case 7:
            a->on = 1;
            if (a->p.w[3] == 0x21) {
                a->p.w[0xE] = st[a->p.w[0]];            /* 0x78 */
            } else {
                a->p.w[0xE] = st[a->p.w[0] + 0x20];
            }
            switch (a->p.w[3]) {
            case 2:
            case 3:
            case 4:
            case 0xD:
            case 0xE:
                D_00470090.flags[a->p.w[0xE]] = a->p.w[0xD];  /* 0x74 */
                break;
            }
            break;
        case 8:
            a->on = 0;
            break;
        case 10:
            D_0047092C[0] = a->unk20[1];
            a->on = 0;
            break;
        case 11:
        case 12:
        case 13:
            a->on = 0;
            break;
        }
    }
}
#endif

void func_0018FAB0(f32 *out, const f32 *m);
void func_00190E00(f32 *out, const f32 *m);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
f32 func_00191A70(f32 x);
void func_00340560(f32 *out, const f32 *a, const f32 *b);
void func_003404F0(f32 *out, const f32 *a, const f32 *b);

/* For a door-like area a (type 0, on model p.w[3]): a new type 0x84 area, the same box in the
   model's frame with its two corners nearest the model's plane pushed half its depth out and
   the other two four depths in. */
#ifdef NON_MATCHING   /* first draft: same length, registers and stack differ */
Aot *func_00595710(Aot *a) {
    f32 r[16];
    f32 loc[4][3];
    f32 p[4];
    f32 v[4];
    f32 t[4];
    Aot *c;
    u32 i;
    u8 i0;
    u8 i1;
    f32 best;
    f32 depth;
    f32 far;
    f32 near;

    c = func_00590BF0(0);
    if (c == NULL) {
        return c;
    }
    c->type = 0x84;
    c->unk31 = 1;
    c->unk3D = 0;
    c->unk32 = a->unk32;
    func_0018FAB0(t, D_00470090.mats[a->p.w[3]]);
    func_00190E00(r, D_00470090.mats[a->p.w[3]]);
    for (i = 0; i < 4; i++) {
        v[0] = a->corner[i][0];
        v[1] = 0.0f;
        v[2] = a->corner[i][1];
        func_00340560(v, v, t);
        func_001913A0(p, v, r);
        loc[i][0] = p[0];
        loc[i][1] = p[1];
        loc[i][2] = p[2];
    }
    i1 = 0xFF;
    i0 = i1;
    best = 10000;
    for (i = 0; i < 4; i++) {
        if (i0 != i) {
            f32 d = func_00191A70(loc[i][2]);

            if (!(best <= d)) {
                best = d;
                i0 = i;
            }
        }
    }
    best = 10000;
    for (i = 0; i < 4; i++) {
        if (i0 != i && i1 != i) {
            f32 d = func_00191A70(loc[i][2]);

            if (!(best <= d)) {
                best = d;
                i1 = i;
            }
        }
    }
    depth = 40.0f;
    for (i = 0; i < 4; i++) {
        p[0] = loc[i][0];
        p[1] = loc[i][1];
        p[2] = loc[i][2];
        if (i0 != i && i1 != i) {
            depth = p[2] - loc[i0][2];
            break;
        }
    }
    far = -4.0f * depth;
    near = 0.5f * depth;
    for (i = 0; i < 4; i++) {
        p[0] = loc[i][0];
        p[1] = loc[i][1];
        p[2] = loc[i][2];
        if (i0 != i && i1 != i) {
            p[2] = far + loc[i0][2];
        } else {
            p[2] = near + loc[i0][2];
        }
        func_001913A0(v, p, D_00470090.mats[a->p.w[3]]);
        func_003404F0(p, t, v);
        c->corner[i][0] = p[0];
        c->corner[i][1] = p[2];
    }
    c->unk20[0] = a->unk20[0];
    c->unk20[1] = a->unk20[1];
    memset(&c->p, 0, sizeof(c->p));
    c->p.w[0] = (s32)a;
    c->on = 1;
    return c;
}
#endif

/* For a type 2 area a (on model p.w[1], unless p.w[4] is set): a new type 0x85 area (off), the
   box made 120 deep across the model's plane. */
#ifdef NON_MATCHING   /* the original keeps &v[1], &v[2] in s-registers */
Aot *func_00595AE0(Aot *a) {
    f32 r[16];
    f32 p[4];
    f32 v[4];
    f32 t[4];
    Aot *c;
    u32 i;

    if (a->p.w[4] != 0) {
        return NULL;
    }
    c = func_00590BF0(0);
    if (c == NULL) {
        return c;
    }
    c->type = 0x85;
    c->unk31 = 1;
    c->unk3D = 0;
    c->unk32 = a->unk32;
    func_0018FAB0(t, D_00470090.mats[a->p.w[1]]);
    func_00190E00(r, D_00470090.mats[a->p.w[1]]);
    for (i = 0; i < 4; i++) {
        v[0] = a->corner[i][0];
        v[1] = 0.0f;
        v[2] = a->corner[i][1];
        func_00340560(v, v, t);
        func_001913A0(p, v, r);
        if (i == 1 || i == 2) {
            p[2] = 60.0f;
        } else {
            p[2] = -60.0f;
        }
        func_001913A0(v, p, D_00470090.mats[a->p.w[1]]);
        func_003404F0(p, t, v);
        c->corner[i][0] = p[0];
        c->corner[i][1] = p[2];
    }
    c->unk20[0] = a->unk20[0];
    c->unk20[1] = a->unk20[1];
    memset(&c->p, 0, sizeof(c->p));
    c->p.w[0] = (s32)a;
    c->on = 0;
    return c;
}
#endif

/* Type 5: starts off. */
void func_00593FA0(Aot *a) {
    a->on = 0;
    a->p.b[0x2E] = 0;           /* 0x6E */
    a->p.h[8] = 0;              /* 0x50 */
    a->p.h[9] = 0xFFFF;         /* 0x52 */
    a->p.b[0x19] = 0;           /* 0x59 */
    a->p.h[0x15] = 0;           /* 0x6A */
    a->p.h[0x16] = 0;           /* 0x6C */
    a->p.w[0xD] = -1;           /* 0x74 */
    a->p.b[0x18] = 0xFF;        /* 0x58 */
    a->p.h[0xB] = 0;            /* 0x56 */
}

/* Type 6: corners 1 and 2 taken through model p.w[8]'s matrix (from its frame). */
#ifdef NON_MATCHING   /* the original keeps &v[1], &v[2] in s-registers */
void func_005960A0(Aot *a) {
    f32 r[16];
    f32 p[4];
    f32 v[4];
    f32 t[4];
    u32 i;

    func_0018FAB0(t, D_00470090.mats[a->p.w[8]]);
    func_00190E00(r, D_00470090.mats[a->p.w[8]]);
    for (i = 1; i < 3; i++) {
        v[0] = a->corner[i][0];
        v[1] = 0.0f;
        v[2] = a->corner[i][1];
        func_00340560(v, v, t);
        func_001913A0(p, v, r);
        func_001913A0(v, p, D_00470090.mats[a->p.w[8]]);
        func_003404F0(p, t, v);
        a->corner[i][0] = p[0];
        a->corner[i][1] = p[2];
    }
}
#endif

void func_00107B50(void *dst, const void *src, s32 n);

/* A new item area (type 0x80): footprint c, heights top/bottom, for item slot index. */
Aot *func_00595CD0(s32 index, s32 (*c)[2], s32 top, s32 bottom) {
    Aot *a = func_00590BF0(0);

    if (a == NULL) {
        return a;
    }
    a->type = 0x80;
    a->unk31 = 0;
    a->unk3D = 0;
    a->unk32 = 3;
    func_00107B50(a->corner, c, 0x20);
    a->unk20[0] = top;
    a->unk20[1] = bottom;
    memset(&a->p, 0, sizeof(a->p));
    a->p.w[0] = index;
    a->on = 1;
    a->p.h[5] = 0xFFFF;
    return a;
}

/* A room object's own area (type 0x81). */
Aot *func_00595D90(ItemObj *o, s32 (*c)[2], s32 bottom, s32 top) {
    Aot *a = func_00590BF0(0);

    if (a == NULL) {
        return a;
    }
    a->type = 0x81;
    a->unk31 = 7;
    a->unk3D = 0;
    a->unk32 = 1;
    func_00107B50(a->corner, c, 0x20);
    a->unk20[0] = bottom;
    a->unk20[1] = top;
    memset(&a->p, 0, sizeof(a->p));
    a->p.b[4] = o->index;
    a->p.w[0] = (s32)o;
    a->on = 1;
    return a;
}

/* A room object's side area (type 0x82; side 0 or 1). */
Aot *func_00595E60(ItemObj *o, s32 (*c)[2], s32 bottom, s32 top, s32 side) {
    Aot *a = func_00590BF0(0);

    if (a == NULL) {
        return a;
    }
    a->type = 0x82;
    a->unk31 = 0;
    a->unk3D = 0;
    a->unk32 = 1;
    func_00107B50(a->corner, c, 0x20);
    a->unk20[0] = bottom;
    a->unk20[1] = top;
    memset(&a->p, 0, sizeof(a->p));
    a->p.b[4] = o->index;
    a->p.b[5] = side;
    a->p.w[0] = (s32)o;
    a->on = 1;
    return a;
}

/* The area on top of a room object (type 0x83). */
Aot *func_00595F30(ItemObj *o, s32 (*c)[2], s32 bottom, s32 top) {
    Aot *a = func_00590BF0(0);

    if (a == NULL) {
        return a;
    }
    a->type = 0x83;
    a->unk31 = 0;
    a->unk3D = 1;
    a->unk32 = 1;
    func_00107B50(a->corner, c, 0x20);
    a->unk20[0] = bottom;
    a->unk20[1] = top;
    memset(&a->p, 0, sizeof(a->p));
    a->p.b[4] = o->index;
    a->p.w[0] = (s32)o;
    a->on = 1;
    return a;
}
