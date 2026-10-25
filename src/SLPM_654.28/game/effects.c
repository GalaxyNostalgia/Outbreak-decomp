#include "common.h"
#include "options.h"
#include "object.h"
#include "effect.h"
#include "thing.h"
#include "item.h"

/* Making effects. */

void *memset(void *p, s32 c, u32 n);
extern Obj250L *D_0023BDC8;     /* the free list */
extern Obj250L *D_0023BDC4;
extern s16 D_0023BDC0;          /* free */
extern s16 D_0023BDCC[2];       /* objects in each list */
void func_006203B0(Obj250L *p, u8 list);
void func_00620420(Obj250L *p, u8 list);
void func_006204E0(Obj250L *at, Obj250L *p, u8 list);
void func_00620490(Obj250L *at, Obj250L *p, u8 list);

/* A free effect in list (where: 0 first, 1 last, 2 after at, 3 before at). */
Obj250L *func_00620280(Obj250L *at, u8 list, s8 where);
#ifdef NON_MATCHING   /* first draft */
Obj250L *func_00620280(Obj250L *at, u8 list, s8 where) {
    Obj250L *p = NULL;

    if (D_0023BDC0 != 0) {
        p = D_0023BDC8;
        D_0023BDC8 = p->prev;
        D_0023BDC0--;
        if (D_0023BDC0 == 0) {
            D_0023BDC4 = NULL;
        }
        memset(p, 0, sizeof(Obj250L));
        p->unk6 = D_0023BDCC[list];
        p->unk2 = list + 6;
        p->list = list;
        D_0023BDCC[list]++;
        switch (where) {
        case 0:
            func_006203B0(p, list);
            break;
        case 1:
            func_00620420(p, list);
            break;
        case 2:
            func_006204E0(at, p, list);
            break;
        case 3:
            func_00620490(at, p, list);
            break;
        }
    }
    return p;
}
#endif

extern Obj250L *D_0023BDD0[2];  /* each list's last */
extern Obj250L *D_0023BDD8[2];  /* each list's first */

/* Puts p first in list. */
#ifdef NON_MATCHING   /* the list offset is computed once in the original */
void func_006203B0(Obj250L *p, u8 list) {
    if (D_0023BDD8[list] == NULL && D_0023BDD0[list] == NULL) {
        D_0023BDD0[list] = p;
        D_0023BDD8[list] = p;
        return;
    }
    p->prev = NULL;
    p->next = D_0023BDD8[list];
    D_0023BDD8[list]->prev = p;
    D_0023BDD8[list] = p;
}
#endif

/* Puts p last in list. */
#ifdef NON_MATCHING   /* the list offset is computed once in the original */
void func_00620420(Obj250L *p, u8 list) {
    if (D_0023BDD8[list] == NULL && D_0023BDD0[list] == NULL) {
        D_0023BDD0[list] = p;
        D_0023BDD8[list] = p;
        return;
    }
    p->next = NULL;
    p->prev = D_0023BDD0[list];
    D_0023BDD0[list]->next = p;
    D_0023BDD0[list] = p;
}
#endif

/* Puts p before at in list. */
void func_00620490(Obj250L *at, Obj250L *p, u8 list) {
    if (at->prev == NULL) {
        p->prev = NULL;
        p->next = at;
        at->prev = p;
        D_0023BDD8[list] = p;
    } else {
        p->prev = at->prev;
        p->next = at;
        at->prev->next = p;
        at->prev = p;
    }
}

/* Puts p after at in list. */
void func_006204E0(Obj250L *at, Obj250L *p, u8 list) {
    if (at->next == NULL) {
        p->next = NULL;
        p->prev = at;
        at->next = p;
        D_0023BDD0[list] = p;
    } else {
        p->prev = at;
        p->next = at->next;
        at->next->prev = p;
        at->next = p;
    }
}

s16 func_003426E0(s16 id);
void func_006216E0(Obj250L *p, s16 kind);

/* A new effect of model id (none when it isn't loaded). */
Obj250L *func_00620B70(s16 id);
#ifdef NON_MATCHING   /* first draft */
Obj250L *func_00620B70(s16 id) {
    s16 k = func_003426E0(id);
    Obj250L *p;

    if (k != -1) {
        p = func_00620280(NULL, 0, 1);
        if (p != NULL) {
            func_006216E0(p, k);
        }
        return p;
    }
    return NULL;
}
#endif

typedef struct {
    u8 unk0[4];
    u8 unk4;            /* 0x4 */
    u8 unk5;            /* 0x5 */
    u8 unk6;            /* 0x6 */
    u8 unk7;
} EffectPartDef;        /* 0x8 */

typedef struct {
    u8 unk0[0xC];
    u16 nparts;         /* 0xC */
    u8 unkE[4];
    s16 unk12;          /* 0x12 */
} EffectDef;

typedef struct {
    u8 unk0;
    u8 unk1;            /* 0x1 */
    s16 unk2;           /* 0x2 */
    s32 unk4;           /* 0x4 */
    void *owner;        /* 0x8 */
} EffectPart;

extern u8 D_00363950[];
EffectPart *func_00620610(u8 a);

/* Starts an effect from its definition: a part for each (made when there is room). */
void func_00620C50(Obj250L *p, EffectDef *def, EffectPartDef *parts, s32 mask);
#ifdef NON_MATCHING   /* first draft */
void func_00620C50(Obj250L *p, EffectDef *def, EffectPartDef *parts, s32 mask) {
    EffectPart *e;
    s32 i;

    p->unk0 = 1;
    p->unkC = 0;
    p->unkE = def->unk12;
    p->unk240 = def->unk12;
    p->unk10 = 0;
    p->unk12 = 0;
    p->nparts = 0;
    p->unk1FF = 1;
    p->unk1F4 = D_00363950;
    p->unk1F8 = *(s32 *)((u8 *)p->unk1F4 + 0x18);
    for (i = 0; i < def->nparts; i++, parts++) {
        e = func_00620610((u8)(u32)p);
        if (e != NULL) {
            e->owner = p;
            e->unk2 = func_003426E0(parts->unk4);
            e->unk1 = parts->unk5;
            e->unk4 = p->flags;
            if (!(mask & (1 << i))) {
                e->unk4 |= 0x800;
            }
            p->partIds[i] = parts->unk6;
            p->nparts++;
        } else {
            p->partIds[i] = -1;
        }
        p->parts[i] = e;
    }
}
#endif

extern s16 D_003639B0[];
extern s16 D_00363BB0[];
u16 func_001ADEF0(s32 n);

/* Sets an effect's kind and its variant (-1: a random one of the kind's). */
void func_00621600(Obj250L *p, u16 id, s16 variant);
#ifdef NON_MATCHING   /* first draft */
void func_00621600(Obj250L *p, u16 id, s16 variant) {
    p->id = id;
    p->unk23A = func_003426E0(id);
    if (variant == -1) {
        variant = D_00363BB0[(u16)id];
        if (variant > 0) {
            variant = (s16)((func_001ADEF0(1) & 0xFFFF) % variant);
        }
    }
    p->unk206 = variant + D_003639B0[(u16)id];
}
#endif

extern u8 *D_0036396C;

/* The definition (and its parts) of effect n. */
void func_00560400(s32 unused, void **out, s32 n);
#ifdef NON_MATCHING   /* first draft */
void func_00560400(s32 unused, void **out, s32 n) {
    s32 *def = (s32 *)(D_0036396C + n * 0x14);

    out[0] = def;
    out[1] = D_0036396C + def[0];
}
#endif

extern u8 D_00343030[];
void func_00556CE0(Obj250L *p);

/* (Room 0x18) three of effect 0x21 around a player. */
void func_00556BD0(DrawObjFull *player);
#ifdef NON_MATCHING   /* first draft */
void func_00556BD0(DrawObjFull *player) {
    void *def[2];
    Obj250L *p;
    s32 i;

    if (D_00306590.unk1A != 0x18) {
        return;
    }
    if (func_003426E0(0x21) == -1) {
        return;
    }
    for (i = 0; i < 3; i++) {
        p = func_00620280(NULL, 0, 1);
        if (p == NULL) {
            return;
        }
        func_00621600(p, 0x21, 0);
        func_00560400(0, def, p->unk206);
        p->update = func_00556CE0;
        p->kind = 0xC;
        p->draw = D_00343030;
        p->mtx = NULL;
        p->target = player;
        p->flags = 0x1AB93;
        func_00620C50(p, def[0], def[1], -1);
        p->unk5 = i;
        p->unk1FF = 0;
    }
}
#endif

void func_00190ED0(Mat44 *dst, const Mat44 *src);

/* A new effect id (variant) with flags and update, at ofs (from m when given: the effect keeps
   a copy of it); mask says which of its parts show. */
Obj250L *func_00620DB0(Mat44 *m, f32 *ofs, s32 mask, s16 id, s32 variant, s32 flags,
                       void *update) {
    void *def[2];
    EffectDef *d;
    EffectPartDef *parts;
    Obj250L *p;

    if ((p = func_00620B70(id)) != NULL) {
        func_00621600(p, id, variant);
        func_00560400((s32)p, def, (u16)p->unk206);
        d = def[0];
        parts = def[1];
        p->update = update;
        p->unk4 = 0;
        p->draw = D_00343030;
        p->ofs[0] = ofs[0];
        p->ofs[1] = ofs[1];
        p->ofs[2] = ofs[2];
        p->mtx = NULL;
        if (m != NULL) {
            func_00190ED0(&p->unk50, m);
            p->mtx = &p->unk50;
        }
        p->flags = flags | 0xA000;
        func_00620C50(p, d, parts, mask);
    }
    return p;
}

extern u8 D_00341EB0[];
void func_00622DD0(Obj250L *p);

/* (A poisoned player) its effect. */
void func_005AEDB0(DrawObjFull *player);
#ifdef NON_MATCHING   /* first draft */
void func_005AEDB0(DrawObjFull *player) {
    Obj250L *p = func_00620B70(7);

    if (p != NULL) {
        func_00621600(p, 7, -1);
        p->unk0 = 1;
        p->unk1 = 0;
        p->update = func_00622DD0;
        p->draw = D_00341EB0;
        p->kind = 0x15;
        p->unk4 = 0;
        p->target = player;
        p->unkC = func_001ADEF0(1) & 0xF;
    }
}
#endif

void func_0018F7C0(Mat44 *m);
void func_00190F00(Mat44 *dst, Mat44 *src);
void func_00622F00(Obj250L *p);
extern f32 D_00239CE8;
extern f32 D_00239D00;
extern f32 D_00239D04;
extern f32 D_00239D08;
extern f32 D_00239D0C;
extern f32 D_00239D10;

/* The effects some items carry (a glow and the like), started on their thing o. */
void func_005AEA90(ItemObj *o, s8 quiet) {
    Mat44 m;
    f32 ofs[3];
    s8 kind;
    s16 n;
    Obj250L *e;
    Obj250L *e2;

    e = NULL;
    {   /* (an inlined byte-wise clear, as in many of the game's functions) */
        u8 *q = (u8 *)ofs;
        s32 k = sizeof(ofs);

        if (q != NULL) {
            do {
                *q++ = 0;
            } while (--k != 0);
        }
    }
    n = 0;
    if (o == NULL) {
        return;
    }
    func_0018F7C0(&m);
    func_00190F00(&m, (Mat44 *)o->mat);
    switch (o->id) {
    case 0x72:
        ofs[1] = D_00239CE8;
        e = func_00620DB0(&m, ofs, 0, 0x27, 0, 0x1A193, func_00622F00);
        kind = 0;
        n = 1;
        break;
    case 0x3200:
        ofs[1] = -1.0f;
        e = func_00620DB0(&m, ofs, 0xFFFF, 0x7F, 0, 0x1A193, func_00622F00);
        kind = 6;
        n = 1;
        if (quiet == 0) {
            e->unk10 = e->unkE;
            e->unkE = 2;
        }
        break;
    case 0x1C:
        ofs[1] = D_00239D00;
        ofs[2] = D_00239D04;
        e = func_00620DB0(&m, ofs, 0, 0x44, 0, 0x1A193, func_00622F00);
        kind = 3;
        n = 0xE;
        break;
    case 0x1D:
        ofs[1] = D_00239D08;
        ofs[2] = 3.5f;
        e2 = func_00620DB0(&m, ofs, 0, 0x40, 0, 0x1A193, func_00622F00);
        if (e2 != NULL) {
            e2->target = o;
            e2->unk1FF = 4;
            e2->unk234 = o->id;
            e2->unk23C = 0x15;
        }
        ofs[1] = D_00239D00;
        ofs[2] = D_00239D04;
        e = func_00620DB0(&m, ofs, 0x20, 0x3F, 0, 0x1A193, func_00622F00);
        kind = 3;
        n = 0x4E;
        break;
    case 0x135:
        ofs[1] = D_00239D00;
        ofs[2] = D_00239D04;
        e = func_00620DB0(&m, ofs, 0, 0x43, 0, 0x1A193, func_00622F00);
        kind = 3;
        n = 0xE;
        break;
    case 0x75:
        ofs[0] = D_00239D0C;
        ofs[1] = D_00239D10;
        e = func_00620DB0(&m, ofs, 0, 0x91, 0, 0x1A193, func_00622F00);
        kind = 5;
        if (e != NULL) {
            e->unkEC = 10.0f;
        }
        n = 0xA5;
        break;
    }
    if (e != NULL) {
        e->kind = 0x1C;
        e->target = o;
        e->unk1FF = kind;
        e->unk234 = o->id;
        e->unk23C = n;
    }
}

void func_005B99B0(Obj250L *p);

/* Starts t's effect n (a kind 0x32 effect following t), except for type 1's flagged variants. */
void func_005AF010(Thing *t, s8 n) {
    Obj250L *p;

    if (t->kind == 1 && D_00357500[1].defs[t->def].unk5 == 1) {
        return;
    }
    p = func_00620280(NULL, 0, 1);
    if (p != NULL) {
        p->unk0 = 1;
        p->unk1 = 0;
        p->update = func_005B99B0;
        p->draw = D_00341EB0;
        p->kind = 0x32;
        p->unk4 = n;
        p->target = t;
    }
}

/* Runs the update of every effect in list. */
void func_006206F0(u8 list) {
    Obj250L *p;

    for (p = D_0023BDD8[list]; p != NULL; p = p->next) {
        ((void (*)(Obj250L *))p->update)(p);
    }
}

/* Splashes: for each of its owner's bones in D_006D8220 (kind unk4 picks the set in
   D_0023A940), one where the bone is in the water's band and one where it crosses its surface
   (or moves along it), each with its own wait. */
typedef struct {
    s32 bone;
    f32 lo;             /* 0x04: the band, from the water */
    f32 hi;             /* 0x08 */
    f32 vel[3];         /* 0x0C: the splash's */
} SplashBone;           /* 0x18 */

typedef struct {
    f32 prev[11][3];    /* 0x00: each bone's position last frame */
    s8 waitA[11];       /* 0x84 */
    s8 waitB[11];       /* 0x8F */
} SplashState;

typedef struct {
    f32 v[3];
} SplashVec;

extern s8 D_0023A940[4][2];     /* per kind: its first SplashBone, how many */
extern SplashBone D_006D8220[];
extern f32 D_006D8580[][3];
extern u16 D_0048AEAA[];
extern u8 D_006DE4B6[];
extern u8 D_006DE4B3[];
void func_00620530(Obj250L *p);
void func_005BA250(Obj250L *p);
void func_00622660(Obj250L *p);
f32 func_00341860(f32 x);
f32 func_005E5B40(const f32 *p, u8 *kind, s32 want);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);

void func_005B99B0(Obj250L *e);
#ifdef NON_MATCHING   /* s-register numbering; the original reads p[1] through an address register */
void func_005B99B0(Obj250L *e) {
    DrawObjFull *o = e->target;
    SplashState *st = (SplashState *)&e->partIds;
    SplashBone *d;
    Mat44 *m;
    Obj250L *x;
    s32 i;
    s32 fl;
    s16 r;
    f32 w;
    f32 top;
    f32 lo;
    f32 hi;
    u8 k;
    f32 p[3];
    f32 c[3];
    s32 n;

    if (o->active == 0 || o->visible == 0) {
        func_00620530(e);
        return;
    }
    n = D_0023A940[e->unk4][1];
    if (e->unk8[0] == 0) {
        if (!(D_0048AEAA[0] & 1)) {
            func_00620530(e);
            return;
        }
        for (i = 0; i < n; i++) {
            st->waitB[i] = 1;
        }
        e->unk8[0] = 1;
    }
    *(SplashVec *)p = *(SplashVec *)o->unk38;
    p[1] += 200.0f;
    w = func_005E5B40(p, &k, 0x81);
    if (k != 0x81) {
        return;
    }
    top = 3.0f + w;
    d = &D_006D8220[D_0023A940[e->unk4][0]];
    for (i = 0; i < n; i++, d++) {
        fl = 0;
        m = ((Mat44 **)((u8 *)o->model + 0x820))[d->bone];
        *(SplashVec *)p = *(SplashVec *)m->m[3];
        lo = w + d->lo;
        hi = w + d->hi;
        if (st->waitA[i] >= 0) {
            st->waitA[i]--;
        }
        if (!(p[1] < lo) && p[1] <= hi) {
            fl = 4;
            if (st->waitA[i] < 0) {
                fl |= 1;
            }
        }
        if (st->waitB[i] >= 0) {
            st->waitB[i]--;
        }
        if (st->waitB[i] < 0) {
            if ((st->prev[i][1] <= hi && !(p[1] <= hi)) || (!(st->prev[i][1] < hi) && p[1] < hi)) {
                fl |= 2;
            } else if (fl & 4) {
                if (!(func_00341860((p[0] - st->prev[i][0]) * (p[0] - st->prev[i][0]) +
                                    (p[2] - st->prev[i][2]) * (p[2] - st->prev[i][2])) < 8.0f)) {
                    fl |= 2;
                }
            }
        }
        *(SplashVec *)st->prev[i] = *(SplashVec *)p;
        if (!(fl & 3)) {
            continue;
        }
        c[0] = p[0];
        c[1] = top;
        c[2] = p[2];
        if (e->unk4 == 1) {
            func_001913A0(p, D_006D8580[i], (f32 *)m);
            c[0] += p[0];
            c[2] += p[2];
        }
        r = func_001ADEF0(1);
        if (fl & 1) {
            x = func_00620DB0(NULL, c, D_006DE4B6[0], 0x36, 0, 0x1A193, func_00622660);
            if (x != NULL) {
                *(SplashVec *)x->unkB4 = *(SplashVec *)d->vel;
                x->unk92 = r;
                x->unk23E = 6;
                func_005BA250(x);
                st->waitA[i] = 10;
            }
        }
        if (fl & 2) {
            x = func_00620DB0(NULL, c, D_006DE4B3[0], 0x33, 0, 0x1A193, func_00622660);
            if (x != NULL) {
                *(SplashVec *)x->unkB4 = *(SplashVec *)d->vel;
                x->unk23E = 4;
                func_005BA250(x);
                st->waitB[i] = 15;
            }
        }
    }
}
#endif


/* An effect shows while the room matrix it follows (unk30) is drawn. */
extern s32 D_003AEF6C[];

void func_0055FB20(Obj250L *p) {
    if (D_003AEF6C[0] & (1 << p->unk30)) {
        p->unk1 = 1;
    } else {
        p->unk1 = 0;
    }
}
