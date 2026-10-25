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
    f32 range[2];       /* 0x28: (flag 0x20) its fade by distance (func_005906C0) */
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
s32 func_001D04D0(s32 id);

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
                *(s32 *)(D_004BEEF0[tex] + 0x44) = func_001D04D0(0xFF0);
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

/* A light effect as func_00546B70 sets it up (an Obj250L). */
typedef struct {
    s16 a;
    s16 b;
    s32 c;
} LightPart;

typedef struct {
    s32 first;
    s32 n;
} LightGroup;

typedef struct {
    s16 a;
    s16 b;              /* 0x2: (the last of a run) its frames */
} LightKey;

typedef struct {
    u8 unk0[8];
    LightGroup *groups; /* 0x08 */
    LightPart *parts;   /* 0x0C */
    LightKey *keys;     /* 0x10 */
    u8 unk14[0x18 - 0x14];
    s32 unk18;          /* 0x18 */
} LightDef;

typedef struct {
    s8 active;          /* 0x000 */
    s8 visible;         /* 0x001 */
    u8 unk2[6];
    u8 state;           /* 0x008 */
    u8 step;            /* 0x009 */
    u8 unkA[2];
    s16 timer;          /* 0x00C: the blink's */
    s16 period;         /* 0x00E */
    s16 blinkA;         /* 0x010 */
    s16 blinkB;         /* 0x012 */
    u8 unk14[0x24 - 0x14];
    RoomLight *light;   /* 0x024 */
    u8 unk28[0x30 - 0x28];
    u8 mtxId;           /* 0x030 */
    u8 unk31;
    u8 group;           /* 0x032: its group in def + 1 (0: none) */
    u8 unk33[5];
    f32 pos[3];         /* 0x038 */
    f32 ofs[3];         /* 0x044 */
    Mat44 own;          /* 0x050 */
    s16 rot[3];         /* 0x090 */
    u8 unk96[0xB4 - 0x96];
    f32 scale[3];       /* 0x0B4 */
    f32 unkC0[3];       /* 0x0C0 */
    u8 unkCC[0xF0 - 0xCC];
    s16 frames[0x12];   /* 0x0F0: per channel (func_0055FBF0) */
    u8 unk114[0x130 - 0x114];
    f32 stepA;          /* 0x130 */
    f32 stepB;          /* 0x134 */
    u8 countA;          /* 0x138 */
    u8 countB;          /* 0x139 */
    s8 blink;           /* 0x13A */
    u8 unk13B[0x18C - 0x13B];
    s32 delay;          /* 0x18C */
    u8 unk190[0x198 - 0x190];
    f32 unk198;         /* 0x198 */
    f32 unk19C;         /* 0x19C */
    u8 unk1A0[0x1F0 - 0x1A0];
    Mat44 *mtx;         /* 0x1F0 */
    LightDef *def;      /* 0x1F4 */
    s32 unk1F8;         /* 0x1F8 */
    u8 unk1FC[4];
    u32 flags;          /* 0x200 */
    u8 unk204[4];
    f32 col[4];         /* 0x208 */
    f32 unk218;         /* 0x218 */
    f32 unk21C;         /* 0x21C */
    f32 unk220;         /* 0x220 */
    f32 unk224;         /* 0x224 */
    f32 unk228;         /* 0x228 */
    f32 unk22C;         /* 0x22C */
    f32 unk230;         /* 0x230 */
    s32 unk234;         /* 0x234 */
    u8 unk238[2];
    s16 unk23A;         /* 0x23A: 1 ..., 0x10 its own matrix at the camera */
    s16 unk23C;         /* 0x23C */
    s16 unk23E;         /* 0x23E */
} LightFx;

typedef struct {
    f32 v[3];
} LightVec;

extern s8 D_0048BF88[];
extern f32 D_00306338[3];       /* the camera */
void func_0055FB20(Obj250L *p);
void func_0055FBF0(LightFx *p, LightDef *d, s32 ch, s32 n, s32 k);
void func_00347040(f32 x, f32 y, f32 *v, f32 *a, f32 *b);
f32 func_005906C0(f32 near, f32 far, f32 *pos);
void func_0018FA40(f32 x, f32 y, f32 z, f32 *m);
void func_0018FA30(f32 x, f32 y, f32 z, f32 *m);
void func_0018FAB0(f32 *out, const f32 *m);

void func_005470B0(Obj250L *p0);
#ifdef NON_MATCHING   /* operand order in the group walk; the w copy and delay store through address registers */
void func_005470B0(Obj250L *p0) {
    LightFx *p = (LightFx *)p0;
    s32 i;
    LightDef *d = p->def;
    RoomLight *l;
    LightPart *e;
    f32 f;
    f32 a;
    f32 b;
    f32 v[3];
    f32 w[3];
    Mat44 m;

    if ((D_0048AE9C[0] & (1 << p->mtxId)) || D_0048BF88[0] != 0) {
        p->active = p->visible = 1;
    } else {
        p->active = p->visible = 0;
    }
    if (p->active == 0) {
        return;
    }
    switch (p->state) {
    case 0:
        p->state++;
        p->visible = 0;
        p->flags |= 0x8000;
        p->unk1F8 = p->def->unk18;
    case 1:
        break;
    default:
        goto end;
    }
    switch (p->step) {
    case 0:
        p->visible = 1;
        p->step++;
        p->pos[0] = p->ofs[0];
        p->pos[1] = p->ofs[1];
        p->pos[2] = p->ofs[2];
        p->scale[0] = 1.0f;
        p->scale[1] = 1.0f;
        p->scale[2] = 1.0f;
        p->rot[0] = 0;
        p->rot[1] = 0;
        p->rot[2] = 0;
        p->unk234 = 0;
        p->col[3] = 1.0f;
        p->col[0] = 1.0f;
        p->col[1] = 1.0f;
        p->col[2] = 1.0f;
        p->unk21C = 0.0f;
        p->unk218 = 0.0f;
        p->unk220 = 1.0f;
        p->unk224 = 1.0f;
        p->unkC0[0] = 0.0f;
        p->unkC0[1] = 0.0f;
        p->unkC0[2] = 0.0f;
        p->unk230 = 0.0f;
    case 1:
        break;
    default:
        goto end;
    }
    if (p->group != 0) {
        for (i = 0; i < d->groups[p->group - 1].n; i++) {
            e = &d->parts[d->groups[p->group - 1].first + i];
            func_0055FBF0(p, d, e->c, e->b, e->a);
        }
    }
    p->col[3] *= p->unk198;
    p->col[0] *= p->unk19C;
    p->col[1] *= p->unk19C;
    p->col[2] *= p->unk19C;
    if (p->unk23A & 1) {
        v[2] = 0.0f;
        v[1] = 0.0f;
        v[0] = 0.0f;
        if (p->mtx != NULL) {
            func_0018FAB0(v, (f32 *)p->mtx);
        }
        v[0] += p->pos[0];
        v[1] += p->pos[1];
        v[2] += p->pos[2];
        func_00347040(p->unk23C / 16.0f, p->unk23E / 16.0f, v, &a, &b);
        p->unk228 = a;
        p->unk22C = b;
    }
    if (p->unk23A & 0x10) {
        p->pos[0] = 0.0f;
        p->pos[1] = 0.0f;
        p->pos[2] = 0.0f;
        func_0018FA30(D_00306338[0], D_00306338[1], D_00306338[2], (f32 *)&p->own);
    }
    if (p->blink != 0) {
        if (p->delay != 0) {
            p->delay--;
            goto end;
        }
        if (p->timer <= 0) {
            p->timer = p->period;
            if (++p->blinkA >= p->countA) {
                p->unk228 = 0.0f;
                p->blinkA = 0;
                if (++p->blinkB >= p->countB) {
                    p->unk22C = 0.0f;
                    p->blinkB = 0;
                } else {
                    p->unk22C += p->stepB;
                }
            } else {
                p->unk228 += p->stepA;
            }
        }
        p->timer--;
    }
    l = p->light;
    if (l->flags & 0x20) {
        if (p->mtx != NULL) {
            func_00190ED0((f32 *)&m, (f32 *)p->mtx);
            func_0018FA40(p->pos[0], p->pos[1], p->pos[2], (f32 *)&m);
            *(LightVec *)w = *(LightVec *)m.m[3];
        } else {
            w[0] = p->pos[0];
            w[1] = p->pos[1];
            w[2] = p->pos[2];
        }
        f = func_005906C0(l->range[0], l->range[1], w);
        if (f == 0.0f) {
            p->visible = 0;
            return;
        }
        p->visible = 1;
        p->col[3] *= f;
    }
end:
    func_0055FB20(p0);
}
#endif


/* Steps channel ch of light effect p along its n keys from k (func_005607B0 reads the value at
   the channel's frame): scale, alpha, colour, turn, place and the rest. */
extern f32 D_00239D44;

/* The value at frame of n keys (each a value and the frame it is reached at), in a straight
   line between them. */
f32 func_005607B0(LightKey *keys, s32 frame, s32 n);
#ifdef NON_MATCHING   /* the key loop's compare into $at; lo's two fields in swapped registers */
f32 func_005607B0(LightKey *keys, s32 frame, s32 n) {
    s32 i;
    LightKey *lo;
    LightKey *hi;

    for (i = 0; i < n; i++) {
        if (frame < keys[i].b) {
            break;
        }
    }
    if (i == n) {
        return keys[i - 1].a;
    }
    if (i == 0) {
        return keys[0].a;
    }
    lo = &keys[i - 1];
    hi = &keys[i];
    return lo->a + (f32)(hi->a - lo->a) * ((f32)(frame - lo->b) / (f32)(hi->b - lo->b));
}
#endif


#ifdef NON_MATCHING   /* two addu operands swapped in the last key's address */
void func_0055FBF0(LightFx *p, LightDef *d, s32 ch, s32 n, s32 k) {
    f32 f = func_005607B0(&d->keys[k], p->frames[ch], n);

    switch (ch) {
    case 0:
        p->scale[0] = f / 16.0f;
        break;
    case 1:
        p->scale[1] = f / 16.0f;
        break;
    case 2:
        p->scale[2] = f / 16.0f;
        break;
    case 3:
        p->col[3] = f / 255.0f;
        break;
    case 15:
        p->col[0] = f / 255.0f;
        break;
    case 16:
        p->col[1] = f / 255.0f;
        break;
    case 17:
        p->col[2] = f / 255.0f;
        break;
    case 4:
        if (f < 0.0f) {
            f += 360.0f;
        }
        p->rot[0] = (u32)(0.5f + 65536.0f * f / 360.0f);
        break;
    case 5:
        if (f < 0.0f) {
            f += 360.0f;
        }
        p->rot[1] = (u32)(0.5f + 65536.0f * f / 360.0f);
        break;
    case 6:
        if (f < 0.0f) {
            f += 360.0f;
        }
        p->rot[2] = (u32)(0.5f + 65536.0f * f / 360.0f);
        break;
    case 7:
        p->pos[0] = p->ofs[0] + f / 16.0f;
        break;
    case 8:
        p->pos[1] = p->ofs[1] + f / 16.0f;
        break;
    case 9:
        p->pos[2] = p->ofs[2] + f / 16.0f;
        break;
    case 10:
        p->unk218 = f / 16.0f;
        break;
    case 11:
        p->unk21C = f / 16.0f;
        break;
    case 12:
        p->unk220 = f / 16.0f;
        break;
    case 13:
        p->unk224 = f / 16.0f;
        break;
    case 14:
        p->unk230 = D_00239D44 * (f / 16.0f);
        break;
    default:
        return;
    }
    p->frames[ch]++;
    if (n != 0) {
        n--;
    }
    if ((&d->keys[k])[n].b <= p->frames[ch]) {
        p->frames[ch] = 0;
    }
}
#endif
