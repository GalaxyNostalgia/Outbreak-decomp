#include "common.h"
#include "game.h"
#include "object.h"
#include "options.h"

/* The per-frame passes over the object lists. */

typedef struct Updatable {
    s8 active;          /* 0x00 */
    u8 unk1[0x14 - 1];
    void (*update)(struct Updatable *o); /* 0x14 */
    u8 unk18[4];
    struct Updatable *next; /* 0x1C */
} Updatable;

extern u8 D_0070F8B0[0x848];   /* (the map marks) */

void func_0064FAD0(void) {
    ((s32 *)D_0070F8B0)[0] = 0;
    ((s32 *)D_0070F8B0)[1] = 0;
}

extern DrawObjFull D_00476DD0[];
extern Npc *D_0023BE04;
extern DrawObjFull *D_0023BDF4;
void func_005ABA30(void *o);
void func_005AB840(void *o);
void func_005AB930(void *o);

void func_005ABAC0(void) {
    DrawObjFull *p;
    u8 i;

    for (i = 0; i < 8; i++) {
        p = &D_00476DD0[i];
        if (p->active != 0) {
            func_005ABA30(p);
            func_005AB840(p);
        }
    }
}

void func_005ABB40(void) {
    Npc *n;

    for (n = D_0023BE04; n != NULL; n = n->next) {
        func_005ABA30(n);
        func_005AB840(n);
    }
}

void func_005ABB80(void) {
    DrawObjFull *t;

    for (t = D_0023BDF4; t != NULL; t = t->next) {
        func_005ABA30(t);
        if (FIELD(t, s32, 0xC94) & 2) {
            func_005AB930(t);
        } else {
            func_005AB840(t);
        }
    }
}

/* Per player, ten timed jobs (offline: players 1 and 2's too, each as "us"). */
#include "job.h"
s32 func_001AF4D0(void);

void func_00588C30(void) {
    Job *j;
    s32 i;
    s32 k;

    for (i = 0; i < 10; i++) {
        j = &D_00387670[0][i];
        if (j->active != 0) {
            j->fn(j);
        }
    }
    if (func_001AF4D0() == 1) {
        for (i = 1; i < 3; i++) {
            D_0048BF60.unk1D = i;
            for (k = 0; k < 10; k++) {
                j = &D_00387670[i][k];
                if (j->active != 0) {
                    j->fn(j);
                }
            }
        }
        D_0048BF60.unk1D = 0;
    }
}

/* Start job `slot` of the given kind for `player` (offline) or for us. */
void func_00588D30(u8 slot, u8 kind, void (*fn)(Job *j), u8 player) {
    u8 me;
    s32 p;

    if (fn != NULL) {
        if (func_001AF4D0() == 1) {
            me = player;
            p = me;
        } else {
            p = 0;
            me = D_0048BF60.unk1D;
        }
        D_00387670[p][slot].active = 1;
        D_00387670[p][slot].slot = slot;
        D_00387670[p][slot].fn = fn;
        D_00387670[p][slot].kind = kind;
        D_00387670[p][slot].player = me;
    }
}

extern Updatable *D_0023BDAC;
extern Updatable *D_0023BDE8;

void func_00551FF0(void) {
    Updatable *o;

    for (o = D_0023BDAC; o != NULL; o = o->next) {
        o->update(o);
    }
}

void func_005437C0(void) {
    Updatable *o;

    for (o = D_0023BDE8; o != NULL; o = o->next) {
        o->update(o);
    }
}

void func_006206F0(u8 list);

void func_006206C0(void) {
    func_006206F0(0);
    func_006206F0(1);
}

typedef struct {
    s8 active;          /* 0x000 */
    u8 unk1[0x14 - 1];
    void (*update)(void *o); /* 0x014 */
    u8 unk18[0x1F0 - 0x18];
} Obj1F0;

extern Obj1F0 D_003939E0[4];

void func_00543CA0(void) {
    Obj1F0 *o;
    s32 i;

    for (o = D_003939E0, i = 0; i < 4; i++, o++) {
        if (o->active != 0) {
            o->update(o);
        }
    }
}

void func_00602160(void) {
}

/* The players' and NPCs' per-frame collision pass (D_0023BF48 set while it runs). */
extern s8 D_0023BF48;
void func_00631CA0(void *o);
void func_00606030(void);

void func_005A9D20(void) {
    Npc *n;
    u32 i;

    D_0023BF48 = 1;
    for (i = 0; i < 8; i++) {
        func_00631CA0(&D_00476DD0[i]);
    }
    for (n = D_0023BE04; n != NULL; n = n->next) {
        func_00631CA0(n);
    }
    D_0023BF48 = 0;
    func_00606030();
}

/* The room's item things, once a frame. */
extern Updatable *D_0023BDB8;
extern s8 D_0023BF44;
void func_0067CA80(void);

void func_00560AF0(void) {
    Updatable *o;

    if (FIELD(&D_0048BF60, s8, 0xCA) != 0) {
        return;
    }
    o = D_0023BDB8;
    D_0023BF44 = 0;
    for (; o != NULL; o = o->next) {
        o->update(o);
    }
    if (D_0023BF44 != 0) {
        func_0067CA80();
    }
}

/* The things held to be hit this frame (pairs {id, thing}). */
typedef struct {
    u16 unk0;
    u8 unk2[2];
    DrawObjFull *obj;   /* 0x4 */
} HitEntry;

extern HitEntry D_00386650[];
extern u8 D_00724230[][0xC];
extern s16 D_0023BD94;          /* entries */
void *memset(void *p, s32 c, u32 n);
void func_006BCCC0(DrawObjFull *o, u8 *work, s16 i);
void func_006BE2F0(DrawObjFull *o, u8 *work);
void func_006BC150(DrawObjFull *o, u8 *work, u16 id, s8 i);
void func_006BE5C0(void);
void func_005A9CA0(void);

void func_006BC030(void);
#ifdef NON_MATCHING   /* first draft */
void func_006BC030(void) {
    u8 work[0x40];
    HitEntry *h;
    DrawObjFull *o;
    s32 i;

    memset(D_00724230, 0, D_0023BD94 * 0xC);
    for (i = 0; i < D_0023BD94; i++) {
        o = D_00386650[i].obj;
        if (o->type == 0) {
            FIELD(o, s8, 0xC53) = 0;
        }
    }
    for (i = 0; i < D_0023BD94; i++) {
        h = &D_00386650[i];
        o = h->obj;
        if (FIELD(o, s16 *, 0x1A8) != NULL && FIELD(o, s16 *, 0x1A8)[1] > 0) {
            func_006BCCC0(o, work, i);
            func_006BE2F0(o, work);
            if (h->unk0 != 0) {
                func_006BC150(o, work, h->unk0, i);
            }
        }
    }
    func_006BE5C0();
    func_005A9CA0();
}
#endif

/* The room's animated textures and such (D_00470090). */
extern u8 D_00470090[0x6D40];
s32 func_001B6980(s32 flag);
void func_00663560(void);
void func_006833D0(u8 i);
void func_005A7660(void);

/* Clears byte 2 of the room's 16 texture animations (0x60 each at 0x4730). */
void func_005A7630(void) {
    s32 i;

    for (i = 0xF; i >= 0; i--) {
        (D_00470090 + 0x4730 + i * 0x60)[2] = 0;
    }
}

void func_005A78C0(void);
#ifdef NON_MATCHING   /* first draft */
void func_005A78C0(void) {
    s32 i;
    u8 *e;

    if (FIELD(D_00470090, s32, 0x894) != -1) {
        for (i = 0; i < 0x80; i++) {
            ((s32 *)(D_00470090 + 0x920))[i] = FIELD(D_00470090, s32, 0x894);
        }
    }
    if (func_001B6980(0x64) != 0 && D_00306590.unk1A == 1 && D_00306590.unk1C == 0x19) {
        func_00663560();
    }
    for (i = 0; i < 0x10; i++) {
        e = D_00470090 + 0x4730 + i * 0x60;
        if (e[0xB] != 0 && e[3] == 0) {
            func_006833D0(i);
            e[0xB] = 0;
        }
    }
    func_005A7660();
    for (i = 0x17; i >= 0; i--) {
        ((s16 *)(D_00470090 + 0x4700))[i] = 0;
    }
}
#endif

extern u32 D_003938E8[];

/* Clears bit n + 28 of D_003938E8. */
void func_005AC1C0(s32 n) {
    D_003938E8[0] &= ~(0x1000 << (n + 0x10));
}

/* Sets bit n + 28 of D_003938E8. */
void func_005AC1F0(s32 n) {
    D_003938E8[0] |= 0x1000 << (n + 0x10);
}

/* (In D_00470090) the room object models' matrices: model k's is copied to m, and m is
   remembered (with a bit) as where k is drawn from. */
extern u32 D_00472D00[];
extern Mat44 *D_00472D10[];
extern Mat44 D_00470BB0[];

void func_005A79E0(u8 k, Mat44 *m) {
    D_00472D00[k >> 5] |= 1 << (k & 0x1F);
    D_00472D10[k] = m;
    *m = D_00470BB0[k];
}

/* The room's switches (16 of 0x60 bytes at D_00470090 + 0x4730), each worked by a zone of
   kind 0 or 2 with its number. */
typedef struct {
    u8 unk0;
    u8 unk1;            /* 0x01: (D_006ED780) */
    u8 unk2;
    u8 state;           /* 0x03: 2 running */
    u8 unk4;            /* 0x04 */
    u8 unk5;
    u8 model;           /* 0x06: its room model */
    u8 anim;            /* 0x07: D_006ED840 entry */
    u8 step;            /* 0x08: in the anim */
    u8 unk9;            /* 0x09 */
    u8 unkA;            /* 0x0A */
    u8 unkB;            /* 0x0B */
    u8 unkC[0x20 - 0xC];
    Mat44 mtx;          /* 0x20 */
} RoomSwitch;

/* The zones' fields for switches (kinds 0 and 2). */
typedef struct {
    u8 unk0[0x30];
    u8 kind;            /* 0x30 */
    u8 unk31[0x40 - 0x31];
    s32 id;             /* 0x40 */
    u8 model2;          /* 0x44: (kind 2) */
    u8 unk45[3];
    s32 base2;          /* 0x48: (kind 2) D_0023A8F0 entry */
    u8 model0;          /* 0x4C: (kind 0) */
    u8 unk4D[0x54 - 0x4D];
    s32 base0;          /* 0x54: (kind 0) */
    u8 type2;           /* 0x58: (kind 2) */
    u8 unk59[0x60 - 0x59];
    u8 type0;           /* 0x60: (kind 0) */
    u8 unk61[0x71 - 0x61];
    u8 dir;             /* 0x71: (script op func_0065E880) */
} SwitchZone;

extern u16 D_0048AFB4[][6];      /* (D_0048AFB0 column 2: the room's switches) */
extern u8 D_0023A8F0[];
extern u8 *D_006ED840[];        /* the switch animations */
extern u8 D_006ED780[][2];
void *func_00551820(u8 id);
void func_00682CC0(RoomSwitch *e, u8 *anim);

/* Starts switch n (when the room has it) as its zone says. */
void func_00683A80(u8 n);
#ifdef NON_MATCHING   /* regalloc in the flag test; kind 0's loads scheduled differently */
void func_00683A80(u8 n) {
    RoomSwitch *e;
    SwitchZone *z;
    u8 model;
    u8 base;
    u8 type;
    u8 dir;
    u8 *a;

    if (!(D_0048AFB4[D_00306590.unk1C][0] & (1 << n))) {
        return;
    }
    e = (RoomSwitch *)(D_00470090 + n * 0x60 + 0x4730);
    z = func_00551820(n);
    if (z == NULL) {
        return;
    }
    switch (z->kind) {
    case 0:
        type = z->type0;
        dir = z->dir;
        base = D_0023A8F0[z->base0];
        model = z->model0;
        break;
    case 2:
        model = z->model2;
        type = z->type2;
        dir = z->dir;
        base = D_0023A8F0[z->base2];
        break;
    }
    switch (type) {
    case 0:
        e->anim = base + (dir * 2 + 0x12);
        break;
    case 1:
        e->anim = 0x1A;
        break;
    case 2:
        e->anim = 0x11;
        break;
    case 3:
        e->anim = 0x1B;
        break;
    default:
        return;
    }
    func_005A79E0(model, &e->mtx);
    e->model = model;
    e->state = 2;
    e->unk4 = 0;
    a = D_006ED840[e->anim];
    e->step = 0;
    e->unkA = a[e->step];
    e->step++;
    e->unk1 = D_006ED780[a[e->step]][0];
    e->unk9 = a[e->step] << 2;
    func_00682CC0(e, a);
}
#endif

/* A task (func_00543800's, kind 5 in D_0023BDE8's list) that makes a kind-5 zone's effect
   at its point until event flag (or flag, bit 15) unk52 is set. */
#include "zone.h"

typedef struct {
    s8 active;          /* 0x00 */
    u8 unk1;
    u8 kind;            /* 0x02 */
    u8 unk3[5];
    u8 state;           /* 0x08 */
    u8 unk9[3];
    s16 timer;          /* 0x0C */
    s16 unkE;           /* 0x0E: (func_005AD2D0's) the next wait, low 12 bits */
    u8 unk10[4];
    void (*update)(void *t); /* 0x14 */
    s32 unk18;          /* 0x18 */
    u8 unk1C[0x28 - 0x1C];
    void *owner;        /* 0x28 */
    u8 unk2C[0x70 - 0x2C];
    f32 pos[3];         /* 0x70 */
} ZoneTask;

void *func_00543800(void *owner, void *fn);
s32 func_005436F0(void *t);
s32 func_005409B0(u16 id);
s32 func_00540A10(s32 n);
void func_005AD2D0(f32 *pos, s16 *wait);
void func_005A8DA0(ZoneTask *t);

typedef struct {
    f32 v[3];
} F3;

void func_005A8D50(Zone *z) {
    ZoneTask *t = func_00543800(z, func_005A8DA0);


    if (t != NULL) {
        *(F3 *)t->pos = *(F3 *)&z->unk44;
    }
}

void func_005A8DA0(ZoneTask *t) {
    Zone *z = t->owner;
    u16 f = z->unk52;

    if (f & 0x8000) {
        if (func_00540A10(f & 0x3FFF)) {
            t->state = 3;
        }
    } else {
        if (func_005409B0(f)) {
            t->state = 3;
        }
    }
    switch (t->state) {
    case 0:
        t->state++;
        t->timer = 0;
        t->unkE = 0;
    case 1:
        if (--t->timer < 0) {
            func_005AD2D0(t->pos, &t->unkE);
            t->state++;
            t->timer = 0;
        }
        break;
    case 2:
        t->timer++;
        if (t->timer >= 4) {
            t->timer = t->unkE & 0xFFF;
            t->state = 1;
        }
        break;
    default:
        func_005436F0(t);
        break;
    }
}
