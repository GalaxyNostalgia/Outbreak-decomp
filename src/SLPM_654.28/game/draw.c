#include "common.h"
#include "object.h"

/* GAME.BIN: drawing the enemies. */

typedef struct { u8 unk0[0x14]; } AnimEntry;

extern DrawObjFull *D_0023BE04;     /* enemies in use */
extern u8 D_0048AEA3[];
extern s8 D_003B9490[][0x824];      /* what each enemy holds */
extern AnimEntry D_00355A40[];      /* (SUBMAIN.BIN) */
s8 func_005902F0(void *a, s32 b);
void func_00343E10(s8 *held, AnimEntry *anim, void *obj);

void func_00651720(void) {
    DrawObjFull *o;
    s8 *held;

    for (o = D_0023BE04; o != NULL; o = o->next) {
        if (o->active != 0 && o->visible != 0 && func_005902F0(o->unk38, 1) == 0 && D_0048AEA3[0] == 0) {
            o->draw(o);
            o->drawn = 1;
            if (o->held != 0xFF) {
                held = D_003B9490[o->held];
                if (held[0] != 0) {
                    func_00343E10(held, &D_00355A40[o->unkBB4], o);
                }
            }
        } else {
            o->drawn = 0;
        }
    }
}

/* Drawing the things (0xD90-byte objects: doors, items in the room...). */

typedef struct {
    u8 unk0[0xC];
    s8 unkC;            /* passed to func_005902F0 */
    u8 unkD[3];
} ThingDef;

typedef struct {
    ThingDef *defs;     /* by unk54D */
    u32 unk4;
} ThingKind;

extern DrawObjFull *D_0023BDF4;     /* things in use */
extern ThingKind D_00357500[];      /* (SUBMAIN.BIN) by kind */
extern u8 D_0048AEA2[];

void func_00688010(void) {
    DrawObjFull *o;
    ThingDef *defs;

    for (o = D_0023BDF4; o != NULL; o = o->next) {
        defs = D_00357500[o->kind].defs;
        if (o->active != 0 && o->visible != 0 && func_005902F0(o->unk38, defs[o->unk54D].unkC) == 0 &&
            D_0048AEA2[0] == 0) {
            o->draw(o);
            o->drawn = 1;
        } else {
            o->drawn = 0;
        }
    }
}

/* Drawing the effects (0x250-byte objects). Some are lit by a white copy of the first light. */

#include "scene.h"

typedef struct Effect {
    s8 active;          /* 0x000 */
    s8 visible;
    u8 unk2[0xE - 2];
    s16 unkE;           /* 0x00E */
    u8 unk10[0x18 - 0x10];
    void (*draw)(struct Effect *e); /* 0x018 */
    struct Effect *next; /* 0x01C */
    u8 unk20[0x200 - 0x20];
    u8 unk200[0x3C];    /* 0x200 */
    u32 flags;          /* 0x23C */
    u8 owner;           /* 0x240 */
} Effect;

typedef struct {
    u8 unk0[0x28];
    u16 player;         /* 0x28: 1-4 */
    u8 unk2A[0x3C - 0x2A];
} EffectOwner;

extern s8 D_0048C02A[];
extern Effect *D_0023BDB8;          /* effects in use */
extern EffectOwner D_003967D0[];
extern DrawObjFull D_00476DD0[8];   /* the players */
extern u8 D_003AEF74;
extern Vec3 D_006D5C00[];

void func_00107B50(void *dst, void *src, s32 n);
s32 func_00562B70(Effect *e, u32 flag);
void func_001B1460(void (*fn)(void *), void *a, void *b);
void func_00560D70(void *e);
void func_001AE9C0(s32 cmd, u32 arg);
void func_00681440(void);

void func_00560B60(void);
#ifdef NON_MATCHING   /* first draft */
void func_00560B60(void) {
    Light l;
    Effect *e;
    u16 p;
    DrawObjFull *o;

    if (D_0048C02A[0] != 0) {
        return;
    }
    func_00107B50(&l, &D_00305FA0.b[0], sizeof(Light));
    l.w[3] = 1.0f;
    l.w[2] = 1.0f;
    l.w[1] = 1.0f;
    l.w[11] = 1.0f;
    l.w[10] = 1.0f;
    l.w[9] = 1.0f;
    for (e = D_0023BDB8; e != NULL; e = e->next) {
        if (e->active == 0 || e->visible == 0) {
            continue;
        }
        if (func_00562B70(e, 0x04000000) != 0) {
            func_001B1460(func_00560D70, e, e->unk200);
        } else if (e->flags & 0x20000) {
            p = D_003967D0[e->owner].player - 1;
            if (p < 4) {
                o = &D_00476DD0[p];
                if (o->kind == D_003AEF74 || func_005902F0(o->unk38, 1) == 0) {
                    func_001AE9C0(0x67, o->unkCE8);
                    e->draw(e);
                    func_001AE9C0(0x67, -1);
                }
            } else {
                e->draw(e);
            }
        } else {
            if (e->flags & 0x1000) {
                func_00107B50(&l.w[13], &D_006D5C00[e->unkE], sizeof(Vec3));
                func_001AE9C0(0x5A, (u32)&l);
            }
            e->draw(e);
            if (e->flags & 0x1000) {
                func_00681440();
            }
        }
    }
    func_00681440();
}
#endif

extern DrawObjFull *D_0023BDAC;     /* 0x1F0-byte objects in use */

void func_00552030(void) {
    DrawObjFull *o;

    for (o = D_0023BDAC; o != NULL; o = o->next) {
        if (o->active & o->visible) {
            o->draw(o);
        }
    }
}

extern DrawObjFull *D_0023BDE8;     /* 0xC0-byte objects in use */

void func_00543860(void) {
    DrawObjFull *o;

    for (o = D_0023BDE8; o != NULL; o = o->next) {
        if ((o->active & o->visible) && o->draw != NULL) {
            o->draw(o);
        }
    }
}
