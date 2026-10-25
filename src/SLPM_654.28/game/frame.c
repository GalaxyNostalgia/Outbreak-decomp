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
u8 func_006BC150(DrawObjFull *o, u8 *work, u16 id, s8 i);
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
    s16 frame;          /* 0x0C */
    s16 key;            /* 0x0E: in D_006ED8D0[unk9] */
    s16 ev;             /* 0x10: in D_006EDD70[unk9] (-1 done) */
    u8 unk12[0x20 - 0x12];
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

#define SWITCH(i) ((RoomSwitch *)(D_00470090 + (i) * 0x60 + 0x4730))

extern u8 D_006ED800[];

void func_00683C90(u8 i) {
    RoomSwitch *e = SWITCH(i);

    e->unk4 = 1;
    e->unk9 = D_006ED840[e->anim][e->step] << 2;
}

void func_00683CF0(u8 v, u8 i) {
    RoomSwitch *e = SWITCH(i);

    e->unk4 = 1;
    e->unk9 = D_006ED800[v] << 2;
}

extern u32 D_0023BE14;          /* players */
void func_00540750(DrawObjFull *o);
void func_00682810(u8 i);

/* D_00470090 seen as the switches and the players' pending flags. */
typedef struct {
    u8 unk0[0x4730];
    RoomSwitch sw[0x10];        /* 0x4730 */
    u8 unk4D30[0x6D35 - 0x4D30];
    u8 pend[8];                 /* 0x6D35: per player, func_00540750 to run */
} RoomSw;
#define ROOMSW ((RoomSw *)D_00470090)

/* Each frame: the players' pending func_00540750, then the switches' animations. */
#ifdef NON_MATCHING   /* regalloc in the switch loop (the original keeps the field addresses off D_00470090 + i * 0x60) */
void func_005A7660(void) {
    s32 i;
    u8 p;
    u8 *a;
    u8 *c;

    if (func_001AF4D0() == 1) {
        for (i = 0; i < D_0023BE14; i++) {
            if (ROOMSW->pend[i] != 0 && D_00476DD0[i].unk54F != 0x32) {
                ROOMSW->pend[i] = 0;
                func_00540750(&D_00476DD0[i]);
            }
        }
    } else {
        p = D_0048BF60.unk1D;
        a = ROOMSW->pend;
        if (a[p] != 0 && D_00476DD0[p].unk54F != 0x32) {
            a[p] = 0;
            func_00540750(&D_00476DD0[p]);
        }
    }
    for (i = 0; i < 0x10; i++) {
        func_00682810(i);
        if (ROOMSW->sw[i].state != 2) {
            continue;
        }
        a = D_006ED840[ROOMSW->sw[i].anim];
        if (ROOMSW->sw[i].unk2 != 0 && ROOMSW->sw[i].unk1 == 2) {
            c = a + ROOMSW->sw[i].step;
            if (c[1] != 0xFF) {
                if (D_006ED780[c[1]][0] != 2) {
                    ROOMSW->sw[i].state = 1;
                    func_00683CF0(a[ROOMSW->sw[i].step], i);
                }
            } else {
                ROOMSW->sw[i].unk1 = D_006ED780[c[-1]][1];
                ROOMSW->sw[i].state = 0;
            }
        } else {
            ROOMSW->sw[i].step++;
            c = a + ROOMSW->sw[i].step;
            if (*c != 0xFF) {
                ROOMSW->sw[i].unk1 = D_006ED780[*c][0];
                ROOMSW->sw[i].state = 1;
                func_00683C90(i);
            } else {
                ROOMSW->sw[i].unk1 = D_006ED780[c[-1]][1];
                ROOMSW->sw[i].state = 0;
            }
        }
    }
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

/* The switches' keyframes (a value at a frame) and timed events. */
typedef struct {
    f32 v;
    s32 t;
} SwitchKey;
typedef struct {
    u16 a;
    s32 t;
} SwitchEv;
extern SwitchKey *D_006ED8D0[];
extern SwitchEv *D_006EDD70[];
s32 func_001918C0(f32 a);
void func_0018F7C0(f32 *m);
void func_00190D50(f32 *out, const f32 *a, const f32 *b);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
void func_00340310(s32 rx, s32 ry, s32 rz, Mat44 *m);
void func_00683D50(f32 *pos, u8 a, u16 b);
#define RPART(k) ((Mat44 *)(r + (k) * 0x40 + 0xB20))

/* Runs switch i's animation: its matrix is its model's, turned or moved by the keyframed
   value as its kind (unkA) says, once or twice a frame (D_00306590.unkC). */
#ifdef NON_MATCHING   /* s-register numbering and add.s operand order */
void func_00682810(u8 i) {
    u8 *r = D_00470090;
    RoomSwitch *e = (RoomSwitch *)(r + i * 0x60 + 0x4730);
    SwitchKey *k;
    SwitchEv *ev;
    SwitchEv *evs;
    s32 n;
    s32 j;
    f32 f;
    f32 *vz;
    f32 *vy;
    f32 *oz;
    f32 v[3];
    f32 out[4];
    Mat44 m;

    switch (e->unk4) {
    case 0:
        return;
    case 1:
        e->frame = 0;
        e->key = 0;
        e->ev = 0;
        e->unk4++;
        e->mtx = *RPART(e->model);
    case 2:
        break;
    }
    n = D_00306590.unkC != 0 ? 2 : 1;
    vz = &v[2];
    vy = &v[1];
    oz = &out[2];
    for (j = 0; j < n; j++) {
        k = D_006ED8D0[e->unk9] + e->key;
        f = (k[1].v - k[0].v) * (f32)(e->frame - k[0].t) / (f32)(k[1].t - k[0].t);
        f += k[0].v;
        switch (e->unkA) {
        case 0:
            func_00340310(0, func_001918C0(f) & 0xFFFF, 0, &m);
            func_0018F7C0((f32 *)&e->mtx);
            func_00190D50((f32 *)&e->mtx, (f32 *)&m, (f32 *)RPART(e->model));
            e->mtx.m[3][0] = RPART(e->model)->m[3][0];
            e->mtx.m[3][1] = RPART(e->model)->m[3][1];
            e->mtx.m[3][2] = RPART(e->model)->m[3][2];
            e->mtx.m[3][3] = RPART(e->model)->m[3][3];
            break;
        case 1:
            e->mtx = *RPART(e->model);
            v[0] = f;
            *vz = 0.0f;
            *vy = 0.0f;
            func_001913A0(out, v, (f32 *)&e->mtx);
            e->mtx.m[3][0] = out[0] + RPART(e->model)->m[3][0];
            e->mtx.m[3][2] = *oz + RPART(e->model)->m[3][2];
            break;
        case 2:
            e->mtx = *RPART(e->model);
            e->mtx.m[3][1] = f + RPART(e->model)->m[3][1];
            break;
        case 3:
            e->mtx = *RPART(e->model);
            *vz = f;
            *vy = 0.0f;
            v[0] = 0.0f;
            func_001913A0(out, v, (f32 *)&e->mtx);
            e->mtx.m[3][0] = out[0] + RPART(e->model)->m[3][0];
            e->mtx.m[3][2] = *oz + RPART(e->model)->m[3][2];
            break;
        }
        e->frame++;
        evs = D_006EDD70[e->unk9];
        if (evs != NULL && e->ev != -1) {
            ev = &evs[e->ev];
            if (ev->t < e->frame) {
                func_00683D50(e->mtx.m[3], r[e->model + 0x2E80], ev->a);
                if (ev[1].t == -1) {
                    e->ev = -1;
                } else {
                    e->ev++;
                }
            }
        }
        if (k[1].t < e->frame) {
            e->key++;
            if (k[2].t == -1) {
                e->unk4 = 0;
                e->state = 2;
                return;
            }
        }
    }
}
#endif

#include "item.h"
#include "model.h"

/* An animated item thing: each of its bones gets six keyframed channels (position, then
   rotation) until all are done; a carried one shows its holder's pose when it aims. */
extern RoomObjModel D_0039A3D0;
void func_00562F10(ItemObj *o);
f32 func_006826A0(u8 *data, s16 frame, s32 n, s32 *end);
void func_0018FCA0(f32 rx, f32 ry, f32 rz, f32 *m);
void func_0018FAB0(f32 *out, const f32 *m);
void func_0018FA30(f32 x, f32 y, f32 z, f32 *m);

void func_00682230(ItemObj *o);
#ifdef NON_MATCHING   /* regalloc; the original reads v[1..3] through address registers and doesn't fold w - 1 into the address */
void func_00682230(ItemObj *o) {
    s32 b;
    s32 c;
    s32 *ch;
    s8 *d;
    s32 bit;
    u16 w;
    f32 *v4;
    f32 *v5;
    s32 n;
    f32 pos[3];
    Mat44 m;
    f32 v[6];

    func_00562F10(o);
    if (!(o->flags & 0x20000)) {
        return;
    }
    w = D_003967D0[o->index].owner;
    if (w != 0 && (w & 0xFFFF) < 5) {
        if (D_00476DD0[w - 1].unkBA0 & 0x200) {
            for (b = 0; b < D_0039A3D0.count[o->model]; b++) {
                o->partOn[b] = 1;
                o->parts[b] = D_0039A3D0.world[o->model][b];
            }
        }
    } else {
        o->anim = 0;
    }
    switch (o->anim) {
    case 0:
        o->frame = 0;
        o->done[0] = 0;
        o->done[1] = 0;
        o->done[2] = 0;
        o->done[3] = 0;
        o->done[4] = 0;
        o->done[5] = 0;
        o->unk3E[0] = 0;
        w = D_003967D0[o->index].owner;
        if (w != 0) {
            return;
        }
        if ((w & 0xFFFF) < 5) {
            return;
        }
        for (b = 0; b < D_0039A3D0.count[o->model]; b++) {
            o->partOn[b] = 1;
            o->parts[b] = D_0039A3D0.world[o->model][b];
        }
        return;
    case 1:
        v5 = &v[5];
        v4 = &v[4];
        for (b = 0; b < 6; b++) {
            d = &o->done[b];
            if (*d == 0x3F) {
                continue;
            }
            if (o->animTbl[b] == 0) {
                *d = 0x3F;
                continue;
            }
            ch = (s32 *)(o->animData + o->animTbl[b]);
            for (c = 0; c < 6; c++) {
                bit = 1 << c;
                v[c] = 0.0f;
                if (*d & bit) {
                    continue;
                }
                if (ch[c * 2] == 0) {
                    *d |= (s8)bit;
                    continue;
                }
                n = 0;
                v[c] = func_006826A0(o->animData + ch[c * 2], o->frame, ch[c * 2 + 1], &n);
                if (n != 0) {
                    *d |= (s8)bit;
                }
            }
            func_0018FCA0(v[3], *v4, *v5, (f32 *)&o->parts[b]);
            m = D_0039A3D0.world[o->model][b];
            func_0018FAB0(pos, (f32 *)&m);
            func_0018FA30(v[0], v[1], v[2], (f32 *)&o->parts[b]);
        }
        o->frame += 2;
        n = 0;
        for (b = 0; b < 6; b++) {
            if (o->done[b] == 0x3F) {
                n++;
            }
        }
        if (n == 6) {
            o->anim = 0;
        }
        break;
    }
}
#endif

/* An item glint: a sparkle effect (one of three) at pos, and the next wait: wait's top bits
   count 0..2, its low 12 the frames. */
#include "effect.h"
u16 func_001ADEF0(s32 n);
Obj250L *func_00620DB0(Mat44 *m, f32 *ofs, s32 mask, s16 id, s32 variant, s32 flags,
                       void *update);
void func_00621DA0(Obj250L *e);
typedef struct {
    f32 v[3];
} GlintVec;

void func_005AD2D0(f32 *pos, s16 *wait) {
    Obj250L *e;
    s16 n;
    s16 t;

    e = func_00620DB0(NULL, pos, 0, (func_001ADEF0(1) & 0xFFFF) % 3 + 0x24, 0, 0x1A193,
                      func_00621DA0);
    if (e == NULL) {
        return;
    }
    *(GlintVec *)&e->pos = *(GlintVec *)pos;
    e->unkC = 0;
    e->unkF0 = 30.0f;
    n = *wait >> 12;
    t = (n == 2) ? 0 : n + 1;
    *wait = t << 12;
    if (t == 0) {
        n = e->unkE / 2 + t * 30 - 4;
    } else {
        n = e->unkE / 2 + (t + 1) * 30 - 4;
    }
    if (n < 0) {
        n = 0;
    }
    *wait |= n;
}

/* A task (func_00543800's) that turns room model `model` by the keyframes D_0023AB00 (with
   D_0023AB04's timing) while go is 1, then puts the model's matrix back. */
typedef struct {
    u8 unk0[6];
    u8 model;           /* 0x06 */
    u8 unk7;
    u8 state;           /* 0x08 */
    u8 unk9[3];
    s16 frame;          /* 0x0C */
    s16 key;            /* 0x0E */
    s16 go;             /* 0x10 */
    u8 unk12[0x30 - 0x12];
    Mat44 mtx;          /* 0x30 */
} TurnTask;

extern SwitchKey *D_0023AB00;
extern SwitchKey *D_0023AB04;
void func_005A7A60(u8 k);

void func_005D3F50(TurnTask *t);
#ifdef NON_MATCHING   /* the keyframe tables' loads scheduled differently; base register operand order in the copies */
void func_005D3F50(TurnTask *t) {
    s8 j;
    u8 *r;
    SwitchKey *k;
    SwitchKey *e;
    f32 f;
    Mat44 m;

    r = D_00470090;
    switch (t->state) {
    case 0:
        t->state++;
        t->frame = 0;
        t->key = 0;
        t->mtx = *(Mat44 *)(D_00470090 + t->model * 0x40 + 0xB20);
        func_005A79E0(t->model, &t->mtx);
        break;
    case 1:
        for (j = 0; j < D_00306590.unkC + 1; j++) {
            if (t->go != 1) {
                continue;
            }
            k = D_0023AB00 + t->key;
            e = D_0023AB04 + t->key;
            f = (k[1].v - k[0].v) * (f32)(t->frame - k[0].t) / (f32)(k[1].t - k[0].t);
            f += k[0].v;
            t->frame++;
            if (e[1].t <= t->frame) {
                t->key++;
                e++;
            }
            if (e[1].t < 0) {
                t->go = 0;
                t->state = 0;
            }
            func_00340310(0, func_001918C0(f) & 0xFFFF, 0, &m);
            func_0018F7C0((f32 *)&t->mtx);
            func_00190D50((f32 *)&t->mtx, (f32 *)RPART(t->model), (f32 *)&m);
            t->mtx.m[3][0] = RPART(t->model)->m[3][0];
            t->mtx.m[3][1] = RPART(t->model)->m[3][1];
            t->mtx.m[3][2] = RPART(t->model)->m[3][2];
            t->mtx.m[3][3] = RPART(t->model)->m[3][3];
        }
        break;
    case 2:
        func_005A7A60(t->model);
        *(Mat44 *)(D_00470090 + t->model * 0x40 + 0xB20) = t->mtx;
        func_005436F0(t);
        break;
    }
}
#endif
