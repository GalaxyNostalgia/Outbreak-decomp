#include "common.h"
#include "options.h"
#include "object.h"
#include "effect.h"
#include "thing.h"

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
void func_00621600(Obj250L *p, s16 id, s16 variant);
#ifdef NON_MATCHING   /* first draft */
void func_00621600(Obj250L *p, s16 id, s16 variant) {
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
