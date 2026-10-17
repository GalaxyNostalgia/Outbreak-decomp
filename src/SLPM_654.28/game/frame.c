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
typedef struct Job {
    u8 active;          /* 0x00 */
    u8 unk1[0x1C - 1];
    void (*fn)(struct Job *j); /* 0x1C */
    u8 unk20[0x110 - 0x20];
} Job;

extern Job D_00387670[3][10];
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
