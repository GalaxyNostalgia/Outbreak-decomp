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
void func_00685D80(DrawObjFull *p);
void func_00686090(DrawObjFull *p);
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
        func_00685D80((DrawObjFull *)n);
        func_00686090((DrawObjFull *)n);
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
    s8 unk3;            /* 0x3: 0 once only, 1 once to each player */
    u8 unk4[0xA - 4];
    s16 unkA;           /* 0xA */
    u16 replies[4];     /* 0xC: (as a reply set) the entries that answer it */
    u8 unk14[0x2C - 0x14];
    s32 unk2C;          /* 0x2C: (unk3 1) the players it was said to */
} TalkEntry;

typedef struct {
    u8 unk0[0xA8];
    u16 unkA8;          /* 0xA8: (the speech playing on channel 0) its unk60 */
    u16 unkAA;          /* 0xAA: (channel 1) */
    u8 unkAC[0xB4 - 0xAC];
    DrawObjFull *unkB4; /* 0xB4: the last to talk to us */
    DrawObjFull *unkB8; /* 0xB8 */
    u8 unkBC[0xC4 - 0xBC];
    s32 unkC4[8];       /* 0xC4: per kind */
    s32 unkE4[0x20];    /* 0xE4 */
    u8 unk164[4];
    u32 said[0x10];     /* 0x168: a bit per talk entry: said this frame */
    s32 since[0x200];   /* 0x1A8: per talk entry, frames it has been said for */
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

/* Each frame, character p's talking: its timers, and when it asks to talk (func_006859A0),
   picks what to say: a reply, a call, or one of its entries. */
s16 func_00346C50(const f32 *from, const f32 *to);
f32 func_00191A70(f32 x);
f32 func_00634380(DrawObjFull *p, f32 *pos);

/* Who p talks to: the nearest shown character or NPC in front of it (within a quarter turn
   each way) in its game, unless the last one (unkDC5) is still within 300. */
void func_00686530(DrawObjFull *p) {
    s16 n;
    DrawObjFull *o;
    s16 i;
    f32 best;
    u16 who;
    u16 pl;
    DrawObjFull *q;
    DrawObjFull *list[20];

    for (i = 0, n = 0; i < 8; i++) {
        o = &D_00476DD0[i];
        if (o->active != 0 && o->visible != 0) {
            list[n++] = o;
        }
    }
    for (o = (DrawObjFull *)D_0023BE04; o != NULL; o = o->next) {
        if (o->active != 0 && o->visible != 0) {
            list[n++] = o;
        }
    }
    best = 100.0f;
    who = 0xFFFF;
    pl = who;
    for (i = 0; i < n; i++) {
        q = list[i];
        if (q->active == 0) {
            continue;
        }
        if (q->type == 0 && q->kind == p->kind) {
            continue;
        }
        if (q->unk1E2 != p->unk1E2) {
            continue;
        }
        if (func_00191A70((s16)(func_00346C50(p->unk38, q->unk38) - p->unk92)) < 24576.0f) {
            f32 d = func_00634380(p, q->unk38);

            if (!(best <= d)) {
                best = d;
                if (q->type == 0) {
                    who = q->unkB98;
                    pl = 0xFFFF;
                } else {
                    who = q->unkBB4;
                    pl = p->kind;
                }
            }
        }
    }
    if (p->unkDC5 != 0) {
        if (p->unkDC7 < 0) {
            if (p->unkDC2 >= 0 && func_00634380(p, D_00476DD0[p->unkDC2].unk38) < 300.0f) {
                return;
            }
        } else {
            for (o = (DrawObjFull *)D_0023BE04; o != NULL; o = o->next) {
                if (o->active != 0 && o->unkBB4 == p->unkDC2 && func_00634380(p, o->unk38) < 300.0f) {
                    return;
                }
            }
        }
    }
    p->unkDC5 = 0;
    if (who < 0) {
        p->unkDC2 = -1;
        p->unkDC7 = -1;
    } else {
        p->unkDC2 = who;
        p->unkDC7 = pl;
    }
    if (p->type != 0) {
        p->unkDC2 = -1;
        p->unkDC7 = -1;
    }
}

extern s16 D_0071956A[];        /* (1-based) a talk entry asked for */

/* For our own player: unkA of the talk entry asked for (D_0071956A), else 0x63. */
s32 func_00687130(DrawObjFull *p);
#ifdef NON_MATCHING   /* the 0x63 returns (the first jumps past ld ra) and D_0071956A's sign-extension in its own block */
s32 func_00687130(DrawObjFull *p) {
    TalkEntry *e;
    u8 me;
    s16 n;

    if (p->type == 0) {
        me = D_0048BF60.unk1D;
        if (p->kind != me) {
            return 0x63;
        }
        n = D_0071956A[0];
        if (n == 0) {
            return 0x63;
        }
        e = func_00685190(D_00476DD0[me].unkB98, n - 1, 1);
        if (e == NULL) {
            return 0x63;
        }
        return e->unkA;
    }
    return 0x63;
}
#endif

/* Replies (kind 6): the first p's character may give now; 1 if it is to talk. */
s32 func_00686BB0(DrawObjFull *p);
s32 func_00686BB0(DrawObjFull *p) {
    TalkEntry *e;
    s32 i;
    s32 r;
    TalkEntry *f;

    r = 0;
    for (i = 0; i < 5000; i++) {
        if (p->type != 0) {
            continue;
        }
        e = func_00685190(p->unkB98, i, 0);
        if (e == NULL || e->id < 0 || e->kind != 6) {
            continue;
        }
        if (p->type == 0) {
            f = func_00685190(p->unkB98, e->id, 1);
        } else {
            f = func_00685190(8, e->id, 1);
        }
        if (f == NULL || func_00687130(p) > f->unkA) {
            func_00686F80(p, e);
            if (D_007194C0.unkC4[6] != 0) {
                p->unkDB4 = D_007194C0.unkC4[6];
                D_007194C0.unkC4[6] = 0;
                p->unkDC0 = 1;
                p->unkDBE = 0;
                p->talk = 1;
                break;
            }
        }
    }
    if (p->talkCmd != 0) {
        r = 1;
    }
    return r;
}

void func_00686F10(DrawObjFull *p);
s32 func_006862A0(DrawObjFull *p);

void func_00686F30(void);

/* Says the first talk picked (D_007194C0.unkC4, by kind; kind 3 not for NPCs) that is due and
   whose listener can hear; once-only talks are used up. 1 if one was said. */
#ifdef NON_MATCHING   /* keeps &unkC4[k] in a3 across func_00685190 (a leaf, once that matches) */
s32 func_006862A0(DrawObjFull *p) {
    s32 k;
    s32 r;
    s32 *c;
    TalkEntry *e;
    TalkEntry *f;
    s32 j;
    u32 m;
    DrawObjFull *o;

    for (k = 0, r = 0; k < 5; k++) {
        c = &D_007194C0.unkC4[k];
        if (*c <= 0) {
            continue;
        }
        if (p->type == 0) {
            e = func_00685190(p->unkB98, *c - 1, 0);
            f = func_00685190(p->unkB98, *c - 1, 1);
        } else {
            if (k == 3) {
                continue;
            }
            e = func_00685190(8, *c - 1, 0);
            f = func_00685190(8, *c - 1, 1);
        }
        if (f != NULL && !(func_00687130(p) > f->unkA)) {
            continue;
        }
        if (e == NULL) {
            continue;
        }
        if (p->unkDC7 < 0 && (D_00476DD0[p->unkDC2].unkBA0 & 0x20)) {
            continue;
        }
        if (e->kind != 4) {
            if (e->unk3 == 0) {
                e->id = -1;
            } else if (e->unk3 == 1 && k != 1) {
                if (p->unkDC7 < 0 && p->unkDC2 >= 0) {
                    e->unk2C |= 1 << p->unkDC2;
                }
                m = 0;
                for (j = 0; j < 8; j++) {
                    o = &D_00476DD0[j];
                    if (D_0048BF60.unk1D != o->kind && o->active != 0) {
                        m |= 1 << j;
                    }
                }
                if (m == 0) {
                    e->id = -1;
                } else if ((e->unk2C ^ (m & ~(1 << D_0048BF60.unk1D))) == 0) {
                    e->id = -1;
                }
            }
        }
        if (k == 3) {
            func_00686F30();
        }
        p->unkDB4 = D_007194C0.unkC4[k];
        D_007194C0.unkC4[k] = 0;
        r = 1;
        p->unkDC0 = r;
        p->unkDBE = 0;
        p->talk = r;
        break;
    }
    return r;
}
#endif

/* p answers o when o's last talk was to p (or to everyone): with one of the kind-4 entries
   o's talk lists as its replies. */
s32 func_00686910(DrawObjFull *p, DrawObjFull *o);
#ifdef NON_MATCHING   /* the original sets match with daddiu and tests it with one beqz to the end */
s32 func_00686910(DrawObjFull *p, DrawObjFull *o) {
    s32 r;
    s32 match;
    TalkEntry *f;
    s32 i;
    TalkEntry *e;
    s32 id;

    if (o == NULL) {
        return 0;
    }
    if (o->active == 0) {
        return 0;
    }
    if (o->kind == p->kind) {
        return 0;
    }
    match = 0;
    if (o->unkDC2 < 0 && o->unkDC7 < 0) {
        match = 1;
    } else if (p->type == 0) {
        if (o->unkDC7 < 0) {
            if (o->unkDC2 == p->unkB98) {
                match = 1;
            }
        } else {
            match = 0;
        }
    } else {
        if (o->unkDC7 < 0) {
            match = 0;
        } else if (o->unkDC2 == p->unkBB4) {
            match = 1;
        }
    }
    if (match == 0) {
        return 0;
    }
    r = o->unkDB4;
    if (r == 0) {
        return 0;
    }
    if (p->type == 0) {
        if (o->type == 0) {
            f = func_00685190(o->unkB98, r - 1, 1);
        } else {
            f = func_00685190(8, r - 1, 1);
        }
    } else {
        if (o->type == 0) {
            f = func_00685190(o->unkB98, r - 1, 1);
        } else {
            return 0;
        }
    }
    if (f == NULL) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        id = f->replies[i];
        if (id == 0) {
            continue;
        }
        if (p->type == 0) {
            e = func_00685190(p->unkB98, id & 0xFFFF, 0);
        } else {
            e = func_00685190(8, id & 0xFFFF, 0);
        }
        if (e != NULL && e->id >= 0 && e->kind == 4 && D_007194C0.unkC4[e->kind] == 0) {
            func_00686F80(p, e);
        }
    }
    if (D_007194C0.unkC4[4] != 0) {
        D_007194C0.unkC4[7] = 0;
        D_007194C0.unkC4[6] = 0;
        D_007194C0.unkC4[5] = 0;
        D_007194C0.unkC4[3] = 0;
        D_007194C0.unkC4[2] = 0;
        D_007194C0.unkC4[1] = 0;
        D_007194C0.unkC4[0] = 0;
        if (func_001AF4D0() == 1) {
            o->unkDB4 = 0;
        }
    }
    func_006862A0(p);
    if (p->unkDC0) {
        p->unkDC0 = 2;
    }
    return p->unkDC0;
}
#endif

/* Whether p answers someone who talked to it: our player answers the last talker (others
   answer us); NPCs answer unkB8, then unkB4. */
s32 func_00686840(DrawObjFull *p) {
    if (p->type == 0) {
        if (p->kind == D_0048BF60.unk1D) {
            if (func_00686910(p, D_007194C0.unkB4) != 0) {
                return 1;
            }
        } else if (func_00686910(p, &D_00476DD0[D_0048BF60.unk1D]) != 0) {
            return 1;
        }
    } else {
        if (func_00686910(p, D_007194C0.unkB8) != 0) {
            return 1;
        }
        if (func_00686910(p, D_007194C0.unkB4) != 0) {
            return 1;
        }
    }
    return 0;
}
s32 func_006862A0(DrawObjFull *p);

#ifdef NON_MATCHING   /* a nop left in func_00686F10's delay slot */
void func_00685D80(DrawObjFull *p) {
    s32 i;
    s32 k;
    u32 j;
    s32 n;
    TalkEntry *e;

    if (p->type == 0) {
        if (D_0048BF60.unk1D == p->kind) {
            if (D_0048BF60.unkA7 != 0) {
                return;
            }
            p->talkWait--;
            if (p->talkWait > 0) {
                p->talk = 0;
                p->talkCmd = 0;
                return;
            }
            p->talkWait = 0;
        }
        for (i = 0; i < 4; i++) {
            p->others[i] = -1;
        }
        for (j = 0, k = 0; j < D_0023BE14; j++) {
            if (p->unkB98 != D_00476DD0[j].unkB98) {
                p->others[k] = D_00476DD0[j].unkB98;
                k++;
            }
        }
    }
    if (p->active == 0) {
        return;
    }
    if (p->unkDBC != 0) {
        p->unkDBC--;
        if (p->unkDBC == 0) {
            p->unkDC0 = 0;
            p->unkDAA = 0;
            p->unkDC5 = 0;
        }
    }
    if (p->unkDBE != 0) {
        p->unkDBE--;
        if (p->unkDBE == 0) {
            p->unkDC1 = 0;
        }
    }
    if (p->unkDC8 != 0) {
        p->unkDD0++;
    }
    if (p->type == 0 && func_001AF4D0() == 0 && p->kind != D_0048BF60.unk1D) {
        return;
    }
    if (p->talk == 0) {
        return;
    }
    if (func_006AC490() != 0) {
        p->talk = 0;
        p->talkCmd = 0;
        return;
    }
    func_00686530(p);
    D_007194C0.unkC4[7] = 0;
    D_007194C0.unkC4[6] = 0;
    D_007194C0.unkC4[5] = 0;
    D_007194C0.unkC4[4] = 0;
    D_007194C0.unkC4[3] = 0;
    D_007194C0.unkC4[2] = 0;
    D_007194C0.unkC4[1] = 0;
    D_007194C0.unkC4[0] = 0;
    p->unkDC0 = 0;
    p->unkDC8 = 1;
    p->unkDCC = p->unkDD0;
    p->unkDD0 = 0;
    if (func_00686BB0(p) == 0) {
        if (p->kind == D_0048BF60.unk1D) {
            func_00686F10(p);
        }
        if (func_00686840(p) == 0) {
            n = p->type == 0 ? 5000 : 6000;
            for (i = 0; i < n; i++) {
                if (p->type == 0) {
                    e = func_00685190(p->unkB98, i, 0);
                } else {
                    e = func_00685190(8, i, 0);
                }
                if (e != NULL && e->id >= 0 && e->kind < 3 && D_007194C0.unkC4[e->kind] == 0) {
                    func_00686F80(p, e);
                }
            }
            func_006862A0(p);
        }
    }
    p->talk = 0;
}
#endif

void func_00686290(DrawObjFull *p, s32 n) {
}

s32 func_00685590(DrawObjFull *p);
void func_0064C620(DrawObjFull *o, u8 kind);

/* Each frame after the talk is picked: says it (func_00685590) when one was chosen; our player
   then waits a second (half online) before the next. */
void func_00686090(DrawObjFull *p) {
    s32 r;

    if (p->unkDC0) {
        if (p->unkDC0 == 2) {
            p->unkDC1 = 1;
        }
        func_00686290(p, 2);
        r = func_00685590(p);
        if (p->type == 0) {
            if (p->kind == D_0048BF60.unk1D && r != 0) {
                p->talkWait = D_00306590.unkC != 0 ? 30 : 60;
                func_0064C620(p, 0xB);
            }
        }
        p->unkDAA = 1;
        p->unkDC0 = 0;
    } else {
        func_00686290(p, 1);
    }
}

/* The speeches being said (16 of 0x98 bytes, D_00718B40), and per channel which are heard
   (D_00718AC0: [0] the one playing). */
typedef struct {
    s16 active;         /* 0x00 */
    u8 unk2[6];
    s16 timer;          /* 0x08 */
    u8 unkA[3];
    s8 unkD;            /* 0x0D */
    u8 unkE[0x5C - 0xE];
    DrawObjFull *who;   /* 0x5C */
    u16 unk60;          /* 0x60 */
    u8 unk62[0x98 - 0x62];
} Speech;

extern s8 D_0048BF88[];
extern s16 D_0023BF7A;
extern s16 D_0023BF78;
extern Speech *D_00718AC0[2][0x10];
extern Speech D_00718B40[0x10];
void func_00687AD0(Speech *s, s8 a);
void func_00687420(Speech *s, s32 ch);

#ifdef NON_MATCHING   /* the second clearing loop's store stays out of its bnez delay slot in the original */
void func_006871E0(void) {
    s32 i;
    s32 j;
    Speech *s;

    if (D_0048BF88[0] == 0 && D_0048BF60.unkBD == 0 && !(D_0048BF60.unkB9 & 4)) {
        D_0023BF7A = 0;
        D_0023BF78 = 0;
        for (i = 0; i < 2; i++) {
            for (j = 0xF; j >= 0; j--) {
                D_00718AC0[i][j] = 0;
            }
        }
        for (i = 0, s = D_00718B40; i < 0x10; i++, s++) {
            if (s->active != 0) {
                s->timer++;
                func_00687AD0(s, s->unkD);
            }
        }
        for (i = 0; i < 2; i++) {
            for (j = 1; j < 0x10; j++) {
                if (D_00718AC0[i][j] != NULL) {
                    D_00718AC0[i][j]->active = 0;
                }
                D_00718AC0[i][j] = NULL;
            }
        }
        D_007194C0.unkB4 = NULL;
        D_007194C0.unkA8 = 0;
        if ((s = D_00718AC0[0][0]) != NULL) {
            func_00687420(s, 0);
            D_007194C0.unkA8 = s->unk60;
            D_007194C0.unkB4 = s->who;
        }
        D_007194C0.unkB8 = NULL;
        D_007194C0.unkAA = 0;
        if ((s = D_00718AC0[1][0]) != NULL) {
            func_00687420(s, 1);
            D_007194C0.unkAA = s->unk60;
            D_007194C0.unkB8 = s->who;
        }
    }
}
#endif

/* Counts how long each talk entry has been going (0 once it stops). */
void func_00685CF0(void) {
    s32 i;

    for (i = 0; i < 0x200; i++) {
        if (D_007194C0.said[i / 32] & (1 << (i % 32))) {
            D_007194C0.since[i]++;
        } else {
            D_007194C0.since[i] = 0;
        }
    }
}
