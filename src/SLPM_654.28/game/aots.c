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

/* Takes zone a out of the list (prev at 0x28 towards D_0023BD90's end) into the free ones;
   0 if those are full. */
s32 func_00590CC0(Aot *a) {
    if (D_0023BD88 == 0x80) {
        return 0;
    }
    D_00386410[D_0023BD88] = a;
    D_0023BD88++;
    if (D_0023BD90 == a) {
        if (a->prev == NULL) {
            D_0023BD8C = NULL;
            D_0023BD90 = NULL;
        } else {
            a->prev->next = NULL;
            D_0023BD90 = a->prev;
        }
        return 1;
    }
    if (D_0023BD8C == a) {
        if (a->next == NULL) {
            D_0023BD8C = NULL;
            D_0023BD90 = NULL;
        } else {
            a->next->prev = NULL;
            D_0023BD8C = a->next;
        }
        return 1;
    }
    a->next->prev = a->prev;
    a->prev->next = a->next;
    return 1;
}

extern s8 D_0048BF88[];
extern f32 D_00306338[3];       /* the camera */
extern f32 D_006D6CF0[];        /* draw distances */
f32 func_001916E0(const f32 *a, const f32 *b);

/* Whether pos (100 up) is within draw distance range of the camera; never while D_0048BF88. */
s8 func_005902F0(f32 *pos, s8 range) {
    Vec3 p;

    if (D_0048BF88[0] != 0) {
        return 0;
    }
    p = *(Vec3 *)pos;
    p.y += 100.0f;
    return func_001916E0(&p.x, D_00306338) < D_006D6CF0[range];
}

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

extern void (*D_006D6D00[])(Aot *a);   /* per type */
extern void (*D_006D6D40[])(Aot *a);   /* per type & 0x7F, for types with 0x80 */

/* Runs the zone's per-type function. */
void func_005910F0(Aot *a) {
    if (a->type & 0x80) {
        D_006D6D40[a->type & 0x7F](a);
        return;
    }
    D_006D6D00[a->type](a);
}

#include "object.h"
#include "game.h"
#include "job.h"

/* A kind-5 zone (a message where a player steps in) as func_00593FE0 sees it. */
typedef struct {
    u8 unk0[0x34];
    DrawObjFull *who;   /* 0x34 */
    u8 unk38[4];
    u8 on;              /* 0x3C */
    u8 unk3D[0x50 - 0x3D];
    u16 msg;            /* 0x50 */
    u16 flag;           /* 0x52: 0xFFFF none; 0x8000 | flag, 0x4000 | item, or an event flag */
    u16 msg2;           /* 0x54: when the flag is set */
    u16 arg;            /* 0x56 */
    u8 snd;             /* 0x58: 0xFF none */
    u8 mask;            /* 0x59: characters with their own message */
    u16 msgBy[8];       /* 0x5A */
    s16 max;            /* 0x6A: messages that follow on each time (0: one) */
    s16 count;          /* 0x6C */
    u8 once;            /* 0x6E */
    u8 unk6F;
    s32 val;            /* 0x70 */
    s32 mode;           /* 0x74: -1 none, 0 func_005403E0, 1 func_005405F0 */
} ZoneMsg;

extern DrawObjFull D_00476DD0[];
extern u8 D_0048AEA9[];
extern void (*D_006D67BC[])(Job *j);
extern void (*D_006D67C0[])(Job *j);
s32 func_001AF540(DrawObjFull *o);
s32 func_001AF4D0(void);
s32 func_00540A10(s32 n);
void func_005409E0(s32 n);
s32 func_005409B0(u16 id);
void func_00540970(u16 id);
void func_00588D30(u8 slot, u8 kind, void (*fn)(Job *j), u8 player);
void func_005403E0(s32 id, s32 arg);
void func_005405F0(s32 id, s32 arg, s32 arg2);
void func_00540120(u16 msg, s32 a, s32 b);
void func_0058EB70(u8 a, u16 b);

/* A kind-3 zone (a sound where a player steps in) as func_005939C0 sees it. */
typedef struct {
    u8 unk0[0x34];
    DrawObjFull *who;   /* 0x34 */
    u8 unk38[0x40 - 0x38];
    s32 snd;            /* 0x40: (low 9 bits) */
    s32 mode;           /* 0x44: 0 func_005403E0, 1 func_005405F0 (ours, or for all), 2 and a message, 3 both */
    s32 unk48;
    s32 test;           /* 0x4C: 1 the player number is (or isn't) val, 2 the character, 3 online and not
                           ours, 4 ours */
    s32 val;            /* 0x50 */
    s32 not;            /* 0x54 */
    u16 msg;            /* 0x58: 0xFFFF none */
} ZoneSnd;

/* A kind-3 zone stepped in: its sound (and message), for whom it's meant. */
void func_005939C0(ZoneSnd *z);
#ifdef NON_MATCHING   /* registers: z and o swapped (s1/s2) */
void func_005939C0(ZoneSnd *z) {
    DrawObjFull *o = z->who;
    s32 id;

    if (o->type != 0) {
        return;
    }
    id = z->snd & 0x1FF;
    switch (z->test) {
    case 0:
        break;
    case 1:
        if ((o->kind != z->val) != z->not) {
            return;
        }
        break;
    case 2:
        if ((o->unkB98 != z->val) != z->not) {
            return;
        }
        break;
    case 3:
        if (!func_001AF4D0()) {
            return;
        }
        if (func_001AF540(o)) {
            return;
        }
        break;
    case 4:
        if (!func_001AF540(o)) {
            return;
        }
        break;
    }
    switch (z->mode) {
    case 0:
        func_005403E0(id, 1);
        break;
    case 1:
        if (func_001AF4D0() == 1) {
            func_005405F0(id, 1, o->kind);
        } else if (func_001AF540(o)) {
            func_005405F0(id, 1, 0);
        }
        break;
    case 2:
        func_005403E0(id, 1);
        if (z->msg != 0xFFFF && func_001AF540(o)) {
            func_00540120(z->msg, 0x3C, 4);
        }
        break;
    case 3:
        if (func_001AF4D0() == 1) {
            func_005405F0(id, 1, o->kind);
        } else if (func_001AF540(o)) {
            func_005405F0(id, 1, 0);
        }
        if (z->msg != 0xFFFF && func_001AF540(o)) {
            func_00540120(z->msg, 0x3C, 4);
        }
        break;
    }
}
#endif

/* A kind-5 zone stepped in: its message (by flag, item or event; per character; following
   ones), a sound, and its value. */
void func_00593FE0(ZoneMsg *z);
#ifdef NON_MATCHING   /* first draft: the player index's dead else, the count's sign extension, branch layout */
void func_00593FE0(ZoneMsg *z) {
    DrawObjFull *o = z->who;
    u16 msg;
    u8 kind;
    s32 p;

    if (o->type != 0) {
        return;
    }
    if (func_001AF540(o) == 0) {
        return;
    }
    kind = D_00476DD0[D_0048BF60.unk1D].unkB98;
    msg = z->msg;
    p = 0;
    if (func_001AF4D0() == 1) {
        p = o->kind;
    }
    if (z->flag != 0xFFFF) {
        if (z->flag & 0x8000) {
            if (func_00540A10(z->flag & 0x3FFF) != 0) {
                msg = z->msg2;
            } else {
                func_005409E0(z->flag & 0x3FFF);
            }
        } else if (z->flag & 0x4000) {
            if (D_0048AEA9[0] >= 8) {
                return;
            }
            func_00588D30(8, 8, D_006D67C0[0], o->kind);
            D_00387670[p][8].args[0] = z->flag & 0x3FFF;
            D_00387670[p][8].args[1] = (s32)z;
            return;
        } else if (func_005409B0(z->flag) != 0) {
            msg = z->msg2;
        } else {
            func_00588D30(8, 7, D_006D67BC[0], o->kind);
            D_00387670[p][8].args[0] = z->flag;
            D_00387670[p][8].args[1] = msg;
            func_00540970(z->flag);
            return;
        }
    }
    if (z->mask != 0) {
        if (!(z->mask & (1 << kind))) {
            return;
        }
        msg = z->msgBy[kind];
    }
    if (z->mode != -1 && z->count >= z->max) {
        if (z->mode == 0) {
            func_005403E0(z->val, 1);
        } else if (z->mode == 1) {
            if (func_001AF4D0() == 1) {
                func_005405F0(z->val, 1, o->kind);
            } else {
                func_005405F0(z->val, 1, 0);
            }
        }
    }
    if (z->max != 0) {
        msg += z->count;
        if (z->count < z->max) {
            z->count++;
        } else {
            z->count = z->max;
        }
    }
    if (z->flag != 0xFFFF) {
        func_00540120(msg, 0x3C, 2);
    } else {
        func_00540120(msg, 0x3C, 4);
    }
    if (z->snd != 0xFF) {
        func_0058EB70(z->snd, z->arg);
    }
    if (z->once != 0) {
        z->on = 0;
    }
}
#endif

#include "object.h"
#include "game.h"

/* A door's trigger zone (an Aot of kind 5) as func_00591160 reads it. */
typedef struct {
    u8 unk0[0x34];
    DrawObjFull *who;   /* 0x34: who stepped in */
    u8 unk38[6];
    u8 kind;            /* 0x3E: the kind of who (5: a player opening it, 2 with a key, 6 ...) */
    u8 unk3F;
    s32 id;             /* 0x40: the door's switch */
    s32 room2;          /* 0x44: the room on the other side */
    s32 id2;            /* 0x48: its switch there */
    s32 model;          /* 0x4C: whose matrix (D_00470090.mats) */
    s32 unk50;
    s32 type;           /* 0x54: the door's type (D_0023A8F0, D_006D7600) */
    s32 dir;            /* 0x58 */
    s32 unk5C;
    s32 state;          /* 0x60: 0-3 */
    s16 lock;           /* 0x64: what's left to open it (kind 6) */
    s16 unk66;
    u16 sound;          /* 0x68 */
    u16 unk6A;
    s32 key;            /* 0x6C: the item that opens it (-2: none) */
    u8 unk70[2];
    u8 linked;          /* 0x72: the other side follows */
    s8 call;            /* 0x73: (1) func_00593920 when gone through */
    u16 callArg;        /* 0x74 */
} DoorZone;

extern u16 D_0048AFB0[][6];     /* per room: [0] open, [1] seen, [2] locked, [3] ..., [5] ... */
extern u8 D_0023A8F0[];         /* per door type: its first message */
extern u8 D_00476DC5[];         /* per player */
extern f32 D_006D7600[][3];     /* per door type: where to stand (this side) */
extern f32 D_006D7630[][3];     /* the same, for 0x1A / 0x29 */
extern f32 D_006D7660[][3];     /* the other side's */
extern f32 D_006D7690[][3];
s32 func_001AF540(DrawObjFull *o);
void func_005406B0(s32 row, s32 bit, s32 col);
void func_00540700(s32 row, s32 bit, s32 col);
void func_00540E10(u8 room, u8 id, s32 v);
void func_0064F3F0(u8 room, u8 id, s32 v);
s32 func_00540ED0(u8 model, s32 a);
s16 func_00540F40(s16 a, u8 item);
void func_005400A0(s32 a, s32 b, s32 c);
void func_00540100(s32 id, s32 a, s32 b, s32 c);
void func_00590B40(s32 a);
void func_00583F10(DrawObjFull *o, u16 item);
void func_005C4260(void);
void func_00593920(s32 a, s32 b);
void func_001B70B0(void *p, s32 a);

#define DOOR_SEEN(z) \
    if (func_001AF540(o) != 0) { \
        func_00590B40(1); \
        D_0048AFB0[D_00306590.unk1C][1] |= (u16)(1 << (z)->id); \
    }

#ifdef NON_MATCHING   /* first draft (1044 instructions) */
/* Someone at a door (zone z): what it says (o->unkAB9, the message), whether it opens
   (o->unkAB8), where to stand to go through (o->unkABC, heading unkAC8), and the room flags. */
void func_00591160(DoorZone *z) {
    DrawObjFull *o = z->who;
    f32 v[4];
    f32 m[4];
    u16 room;
    s32 bit;

    switch (o->type) {
    case 0:
        if (z->state == 2) {
            if (D_00470090.entries[z->id][1] == 1) {
                return;
            }
        } else {
            if (D_00470090.entries[z->id][1] != 0) {
                return;
            }
            if (D_0048AFB0[D_00306590.unk1C][2] & (1 << z->id)) {
                return;
            }
        }
        o->unkAD0 = z->id;
        if (z->kind == 5) {
            if (o->kind != D_0048BF60.unk1D || z->state != 0) {
                return;
            }
            if (D_00470090.entries[z->id][1] != 0) {
                return;
            }
            if (D_0048AFB0[D_00306590.unk1C][3] & (1 << z->id)) {
                return;
            }
            o->unkAB8 = 1;
            o->unkAD1 = 0xFF;
            o->unkAB9 = D_0023A8F0[z->type] + 0x18;
            if (!(D_0048AFB0[D_00306590.unk1C][5] & (1 << z->id))) {
                func_005406B0(D_00306590.unk1C, z->id, 3);
                func_005406B0(z->room2, z->id2, 3);
            }
            D_00476DC5[D_0048BF60.unk1D] = 1;
            if (func_001AF540(o) != 0) {
                D_0048AFB0[D_00306590.unk1C][1] |= (u16)(1 << z->id);
            }
            goto stand;
        }
        room = D_00306590.unk1C;
        bit = 1 << z->id;
        if (D_0048AFB0[room][0] & bit) {
            /* open */
            if (z->state != 0) {
                o->unkAB9 = 0;
            } else {
                switch (z->kind) {
                case 2: {
                    u16 it = D_003967D0[o->unkC82].unk6;
                    if (it >= 0x19E && it < 0x1A2) {
                        o->unkAB9 = D_0023A8F0[z->type] + 0x21;
                        o->unkAB8 = 1;
                        z->lock = 200;
                        if (func_001AF540(o) != 0) {
                            D_0048AFB0[D_00306590.unk1C][1] |= (u16)(1 << z->id);
                        }
                        func_005C4260();
                        o->unkABA = 0;
                        goto msg;
                    }
                    if (z->key == it) {
                        if (func_001AF540(o) != 0) {
                            func_00590B40(1);
                            func_005400A0(2, 0x28, 0x3C);
                            D_0048AFB0[D_00306590.unk1C][1] |= (u16)(1 << z->id);
                        }
                        o->unkAB9 = D_0023A8F0[z->type] + 3;
                        o->unkAB8 = 1;
                        func_00540700(D_00306590.unk1C, z->id, 0);
                        if (z->linked != 0) {
                            func_00540700(z->room2, z->id2, 0);
                        }
                        func_00583F10(o, z->key);
                        func_005C4260();
                    } else {
                        o->unkAB9 = 0;
                    }
                    break;
                }
                case 6:
                    if (z->lock == 0) {
                        break;
                    }
                    if (!(D_0048AFB0[room][5] & bit)) {
                        z->lock -= func_00540F40(z->unk66, o->unkC82);
                    }
                    if (z->lock <= 0) {
                        o->unkAB9 = D_0023A8F0[z->type] + 0x25;
                        func_00540700(D_00306590.unk1C, z->id, 0);
                        if (func_001AF540(o) != 0) {
                            func_005400A0(2, 0x28, 0x3C);
                        }
                        if (z->linked != 0) {
                            func_00540700(z->room2, z->id2, 0);
                        }
                        o->unkAB8 = 1;
                    } else {
                        o->unkAB9 = D_0023A8F0[z->type] + 0x23;
                        o->unkAB8 = 0;
                        func_001B70B0((u8 *)*(void **)((u8 *)o->model + 0x85C) + 0x30,
                                      func_00540ED0((u8)z->model, 1));
                    }
                    break;
                default:
                    DOOR_SEEN(z);
                    if (z->key == -2) {
                        o->unkAB9 = D_0023A8F0[z->type] + 9;
                        if (func_001AF540(o) != 0) {
                            func_005400A0(2, 0x28, 0x3C);
                        }
                        func_00540700(D_00306590.unk1C, z->id, 0);
                        if (z->linked != 0) {
                            func_00540700(z->room2, z->id2, 0);
                        }
                    } else {
                        o->unkAB9 = D_0023A8F0[z->type] + 5;
                        if (func_001AF540(o) != 0) {
                            if (z->sound != 0xFFFF) {
                                func_00540100(z->sound, 0x14, 0x28, 3);
                            }
                            func_00590B40(1);
                            D_0048AFB0[D_00306590.unk1C][1] |= (u16)(1 << z->id);
                        }
                    }
                    o->unkAB8 = 1;
                    break;
                }
            }
            o->unkABA = 0;
            goto msg;
        }
        /* closed */
        if (z->kind == 2 || z->kind == 6) {
            return;
        }
        switch (z->state) {
        case 0:
            DOOR_SEEN(z);
            if (D_0048AFB0[D_00306590.unk1C][3] & (1 << z->id)) {
                o->unkAB9 = D_0023A8F0[z->type] + 0x27;
                if (func_001AF540(o) != 0) {
                    func_005400A0(1, 0x28, 0x3C);
                }
            } else {
                o->unkAB9 = D_0023A8F0[z->type] + 1;
                func_00540700(D_00306590.unk1C, z->id, 3);
                func_00540700(z->room2, z->id2, 3);
                if (o->kind == D_0048BF60.unk1D) {
                    o->unkABA = 1;
                }
            }
            break;
        case 1:
            DOOR_SEEN(z);
            o->unkACC = 168.0f;
            o->unkAB9 = 0x1A;
            o->unkABA = 1;
            break;
        case 2:
            if (D_00470090.entries[z->id][1] == 0) {
                func_0064F3F0(room, z->id, 1);
                func_00540E10(D_00306590.unk1C, z->id, 1);
                func_0064F3F0(z->room2, z->id2, 1);
                o->unkAB9 = 0x1D;
            } else {
                func_0064F3F0(room, z->id, 0);
                func_00540E10(D_00306590.unk1C, z->id, 0);
                func_0064F3F0(z->room2, z->id2, 0);
                o->unkAB9 = 0x1E;
            }
            break;
        case 3:
            DOOR_SEEN(z);
            o->unkACC = 168.0f;
            o->unkAB9 = 0x29;
            o->unkABA = 1;
            break;
        default:
            return;
        }
        o->unkAB8 = 1;
        o->unkAD1 = 0xFF;
        if (o->unkABA != 0 && func_001AF540(o) != 0) {
            D_0048BF60.unk35 = z->id2;
            D_0048BF60.unk37 = 1;
            D_00306590.unk20 = D_00306590.unk1A;
            D_00306590.unk22 = z->room2;
        }
    stand:
    msg:
        func_0018FAB0(v, D_00470090.mats[z->model]);
        if (o->unkAB9 == 0x1A || o->unkAB9 == 0x29) {
            func_001913A0(m, D_006D7630[z->type], D_00470090.mats[z->model]);
        } else {
            func_001913A0(m, D_006D7600[z->type], D_00470090.mats[z->model]);
        }
        if (z->call == 1 && o->unkABA == 1) {
            z->call = -1;
            func_00593920((z->callArg & 0xF000) >> 12, z->callArg & 0x1FF);
        }
        break;
    case 1:
        if (D_00470090.entries[z->id][1] != 0) {
            return;
        }
        o->unkAD0 = z->id;
        o->unkAD1 = 0xFF;
        if (D_0048AFB0[D_00306590.unk1C][0] & (1 << z->id)) {
            o->unkAB9 = D_0023A8F0[z->type] + 5;
            o->unkABA = 0;
        } else {
            o->unkAB9 = D_0023A8F0[z->type] + 1;
            ((u8 *)o)[0x102D] = z->id2;
            ((u8 *)o)[0x115C] = z->room2;
            o->unkABA = 1;
        }
        func_0018FAB0(v, D_00470090.mats[z->model]);
        if (o->unkAB9 == 0x1A || o->unkAB9 == 0x29) {
            func_001913A0(m, D_006D7630[z->type], D_00470090.mats[z->model]);
        } else {
            func_001913A0(m, D_006D7600[z->type], D_00470090.mats[z->model]);
        }
        o->unkAB8 = 1;
        break;
    case 2: {
        u8 e = D_00470090.entries[z->id][1];

        if (e == 2) {
            o->unkAB9 = 7;
            o->unkAB8 = 1;
            func_0018FAB0(v, D_00470090.mats[z->model]);
            if (o->kind == 0x29 || o->kind == 0x16 || o->kind == 1) {
                func_001913A0(m, D_006D7660[z->type], D_00470090.mats[z->model]);
            }
            break;
        }
        if (e == 1) {
            return;
        }
        if (e != 0) {
            break;
        }
        bit = 1 << z->id;
        room = D_00306590.unk1C;
        if (D_0048AFB0[room][0] & bit) {
            o->unkAB9 = D_0023A8F0[z->type] + 0x27;
        } else {
            switch (z->state) {
            case 0:
                if (D_0048AFB0[room][2] & bit) {
                    o->unkAB9 = 7;
                } else if (D_0048AFB0[room][3] & bit) {
                    o->unkAB9 = D_0023A8F0[z->type] + 0x27;
                } else {
                    o->unkAB9 = D_0023A8F0[z->type] + 1;
                }
                break;
            case 1:
                o->unkACC = 168.0f;
                o->unkAB9 = 0x1A;
                break;
            case 2:
                break;
            case 3:
                o->unkACC = 168.0f;
                o->unkAB9 = 0x29;
                break;
            default:
                return;
            }
        }
        func_0018FAB0(v, D_00470090.mats[z->model]);
        if (o->kind == 0x29 || o->kind == 0x16 || o->kind == 1) {
            if (o->unkAB9 == 0x27 || o->unkAB9 == 0x28) {
                func_001913A0(m, D_006D7690[z->id2], D_00470090.mats[z->room2]);
            } else {
                func_001913A0(m, D_006D7660[z->id2], D_00470090.mats[z->room2]);
            }
        }
        o->unkAB8 = 1;
        break;
    }
    }
    func_003404F0(o->unkABC, v, m);
    o->unkAC8 = (u16)(z->dir - 0x8000);
    o->unkABB = z->model;
}
#endif
