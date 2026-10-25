#include "common.h"
#include "game.h"
#include "object.h"

/* The characters' ad-libs (the R2 "talk" commands): who wants to talk this frame, and the
   speech that follows. */

extern DrawObjFull D_00476DD0[];
extern u32 D_0023BE14;          /* players */
extern Npc *D_0023BE04;
extern s16 D_006EF7B0[];
s32 func_001AF4D0(void);
s32 func_006859A0(DrawObjFull *p);
void func_00686CF0(void);
void func_00685D80(void *o);
void func_00686090(void *o);
void func_006871E0(void);
void func_00685CF0(void);

/* Whether our player p asks for an ad-lib this frame (L2 or a right-stick direction), and
   which (the second set while R1 is held; online, not the 0x82 one). */
#include "ctrl.h"
extern Ctrl D_003063E0;
extern u8 D_0023AFA8[8];
extern u8 D_0023AFB0[8];
s32 func_006AC490(void);

s32 func_006859A0(DrawObjFull *p) {
    s32 r;
    u32 dir;

    if (func_006AC490() != 0) {
        return 0;
    }
    r = 0;
    if (D_003063E0.pressed & 0x2000) {
        r = 1;
        dir = 4;
        p->talk = r;
    } else if (D_003063E0.pressed & 0xF00000) {
        r = 1;
        dir = (u32)(u16)(D_003063E0.langle + 0x2000) >> 14;
        p->talk = r;
    }
    if (p->talk != 0) {
        if (D_003063E0.cur & 0x80) {
            p->talkCmd = D_0023AFB0[dir];
        } else {
            p->talkCmd = D_0023AFA8[dir];
            if (func_001AF4D0() == 1 && p->talkCmd == 0x82) {
                p->talkCmd = 0;
                r = 0;
            }
        }
    }
    return r;
}

void func_00685AA0(void);
#ifdef NON_MATCHING   /* first draft */
void func_00685AA0(void) {
    DrawObjFull *p;
    Npc *n;
    u32 i;
    s8 v;

    if (func_001AF4D0() == 0) {
        p = &D_00476DD0[D_0048BF60.unk1D];
        if (func_006859A0(p) == 0 && (FIELD(&D_0048BF60, u32, 0xC0) & 0x100)) {
            FIELD(&D_00476DD0[D_0048BF60.unk1D], s8, 0xDA8) = 1;
            FIELD(&D_00476DD0[D_0048BF60.unk1D], s8, 0xDA9) = 0;
        }
    } else {
        for (i = 0, p = D_00476DD0; i < D_0023BE14; i++, p++) {
            if (p->kind == 0) {
                if (func_006859A0(p) == 0 && (FIELD(&D_0048BF60, u32, 0xC0) & 0x100)) {
                    FIELD(p, s8, 0xDA8) = 1;
                    FIELD(p, s8, 0xDA9) = 0;
                }
            } else if (FIELD(p, u8, 0x1098) != 0) {
                FIELD(p, s8, 0xDA8) = 1;
                v = D_006EF7B0[FIELD(p, u8, 0x1098) - 1];
                FIELD(p, s8, 0xDA9) = v;
                if (v < 0) {
                    FIELD(p, s8, 0xDA8) = 0;
                    FIELD(p, s8, 0xDA9) = 0;
                }
                FIELD(p, u8, 0x1098) = 0;
            }
        }
    }
    func_00686CF0();
    if (func_001AF4D0() == 1) {
        for (i = 0, p = D_00476DD0; i < D_0023BE14; i++, p++) {
            func_00685D80(p);
            func_00686090(p);
        }
    } else {
        func_00685D80(&D_00476DD0[D_0048BF60.unk1D]);
        for (i = 0, p = D_00476DD0; i < D_0023BE14; i++, p++) {
            func_00686090(p);
        }
    }
    for (n = D_0023BE04; n != NULL; n = n->next) {
        func_00685D80(n);
        func_00686090(n);
    }
    func_006871E0();
    func_00685CF0();
}
#endif

extern DrawObjFull *D_00719578[];   /* the player talking */
extern DrawObjFull *D_00719574[];   /* the NPC talking */

/* The one talking (which: 1 the player, 2 the NPC) as model, when its 0xDC7 >= 0 is a. */
void *func_00686150(s32 a, s32 model, s32 which) {
    DrawObjFull *o;

    if (which & 1) {
        o = D_00719578[0];
        if (o != NULL && (FIELD(o, s8, 0xDC7) >= 0) == a && FIELD(o, s16, 0xDC2) == model) {
            return o;
        }
    }
    if (which & 2) {
        o = D_00719574[0];
        if (o != NULL && (FIELD(o, s8, 0xDC7) >= 0) == a && FIELD(o, s16, 0xDC2) == model) {
            return o;
        }
    }
    return NULL;
}

/* The character's talk entries (func_00685190: id, then kind); D_007194C0 seen from here. */
typedef struct {
    s16 id;             /* 0x0: (negative: none) */
    s8 kind;            /* 0x2 */
} TalkEntry;

typedef struct {
    u8 unk0[0xC4];
    s32 unkC4[8];       /* 0xC4: per kind */
    s32 unkE4[0x20];    /* 0xE4 */
} TalkWork;

extern TalkWork D_007194C0;
#include "options.h"
extern s32 *D_00719EB8[5];      /* per speech slot: its entries' offsets */
extern s32 *D_00719ECC[5];      /* the same, the other set */
extern u8 D_00719EF4[];         /* per character: its speech slot */

/* Talk entry i of character who (8: the room's own, slot 4), from the room's speech data. */
TalkEntry *func_00685190(s32 who, s32 i, s32 alt);
#ifdef NON_MATCHING   /* the slot-4 return's last addu sits in its b's delay slot in the original */
TalkEntry *func_00685190(s32 who, s32 i, s32 alt) {
    s32 *tbl;

    if (who == 8) {
        if (alt != 0) {
            tbl = D_00719ECC[4];
        } else {
            tbl = D_00719EB8[4];
        }
        if (tbl == NULL) {
            return NULL;
        }
        who = tbl[i];
        if (who == 0) {
            return NULL;
        }
        return (TalkEntry *)((s32)D_00306590.speech + 0x50000 + who);
    }
    who = D_00719EF4[who];
    if (alt != 0) {
        tbl = D_00719ECC[who];
    } else {
        tbl = D_00719EB8[who];
    }
    if (tbl == NULL) {
        return NULL;
    }
    i = tbl[i];
    if (i == 0) {
        return NULL;
    }
    return (TalkEntry *)(D_00306590.speech + 0x6000 + who * 0x12800 + i);
}
#endif
void func_00686F80(DrawObjFull *p, TalkEntry *e);
void func_00686DF0(s32 a);

/* Runs our character's kind-3 talk entries. */
void func_00686CF0(void) {
    TalkEntry *e;
    s32 i;

    D_007194C0.unkE4[0] = -1;
    for (i = 0; i < 5000; i++) {
        e = func_00685190(D_00476DD0[D_0048BF60.unk1D].unkB98, i, 0);
        if (e != NULL && e->id >= 0 && e->kind == 3) {
            func_00686F80(&D_00476DD0[D_0048BF60.unk1D], e);
            if (D_007194C0.unkC4[e->kind] != 0) {
                func_00686DF0(D_007194C0.unkC4[e->kind]);
            }
        }
    }
}
