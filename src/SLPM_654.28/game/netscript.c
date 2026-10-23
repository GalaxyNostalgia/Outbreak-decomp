#include "common.h"
#include "options.h"
#include "game.h"
#include "script.h"
#include "object.h"
#include "zone.h"

/* GAME.BIN: the room script online. Thread 1 runs once for every player (as that player, so
   per-player instructions act for each), and what each run did to the event flags is merged;
   the opcode table D_00708490 says which instructions to skip for someone else. */

typedef s32 (*ScriptOp)(u32 *args);

typedef struct {
    ScriptOp fn;
    u8 notOthers;       /* 0x04: skipped while running as another player */
    u8 notGuests;       /* 0x05: skipped for players 2 and up */
    u8 notMine;         /* 0x06: skipped while running as ourselves (D_0023C00C 2) */
    u8 unk7;
} NetOp;

extern NetOp D_00708490[];
extern s32 D_0023C004;          /* the player it runs as, 1-based (0: the scene) */
extern s32 D_0023C008;          /* their id */
extern s32 D_0023C00C;          /* 0: ours, 1: another player's */

s32 func_0065AAA0(u32 insn);
void func_0065AAF0(void);

#ifdef NON_MATCHING   /* first draft: register use */
void func_006CD1B0(s32 t) {
    u32 insn;
    s32 r;
    s32 skip;
    u32 op;
    u32 *pc;
    NetOp *e;

    D_0048BE70.cur = t;
    if (D_00306590.unk28 == 0) {
        return;
    }
    D_0048BE70.unk78[(u8)t] = 0;
    for (;;) {
        pc = D_0048BE70.pc[(u8)t];
        insn = *pc;
        if (!(D_0048BE70.wait[(u8)t] == 0)) {
            D_0048BE70.wait[(u8)t]--;
            return;
        }
        op = insn & 0xFFFFFF;
        e = &D_00708490[op];
        r = 0;
        skip = 0;
        if (e->notGuests != 0 && D_0023C004 >= 2) {
            skip = 1;
        } else if (D_0023C00C == 1 && e->notOthers != 0) {
            skip = 1;
        } else if (D_0023C00C == 2 && e->notMine != 0) {
            skip = 1;
        } else {
            r = e->fn(pc + 1);
        }
        if (skip != 0 && func_0065AAA0(insn) != 0) {
            D_0048BE70.unk78[(u8)D_0048BE70.cur]++;
            func_0065AAF0();
            r = 2;
        }
        if (r == 2) {
            insn = *D_0048BE70.pc[(u8)t];
        } else if (op == 0xA || op == 0xD) {
            return;
        }
        if (r == 0) {
            D_0048BE70.pc[(u8)t] += SCRIPT_LEN(insn);
        }
    }
}
#endif

typedef struct {
    u32 unk0[0x60];
    u32 ev[0x10];       /* 0x180: = D_0048AD90 */
    u32 ev2[0x10];      /* 0x1C0: = D_0048ADD0 */
} SceneFlags;

extern SceneFlags D_0048AC10;
extern u32 D_0048AD90[0x10];
extern u32 D_0048ADD0[0x10];
extern u32 D_007246B0[0x20];    /* the flags before */
extern u32 D_007246F0[0x10];
extern u32 D_00724530[3][0x20]; /* what each player's run set */
extern u32 D_007243B0[3][0x20]; /* and cleared */
extern u8 D_00708478[][4];      /* by character kind: the thread its player runs */
extern DrawObjFull D_00476DD0[];
void func_00107B50(void *dst, const void *src, s32 n);

void func_006CCEE0(s32 t);
#ifdef NON_MATCHING   /* the case tests' order, one and's operands */
void func_006CCEE0(s32 t) {
    s32 p;
    s32 i;
    s32 q;
    s32 k;
    DrawObjFull *o;

    switch ((u8)t) {
    case 1:
        goto all;
    case 5:
        goto one;
    case 4:
        goto one;
    }
one:
    D_0023C00C = 0;
    D_0023C008 = 0;
    D_0023C004 = 0;
    func_006CD1B0(t);
    return;
all:
    func_00107B50(D_007246B0, D_0048AD90, 0x40);
    func_00107B50(D_007246F0, D_0048ADD0, 0x40);
    for (p = 0; p < 3; p++) {
        D_0048BF60.unk1D = p;
        o = &D_00476DD0[p];
        if (o->active == 0) {
            continue;
        }
        if (o->unk1E2 == D_00306590.unk1C) {
            D_0023C00C = 0;
        } else {
            D_0023C00C = 1;
        }
        D_0023C008 = o->unk1E2;
        D_0023C004 = o->kind + 1;
        func_00107B50(D_0048AD90, D_007246B0, 0x40);
        func_00107B50(D_0048ADD0, D_007246F0, 0x40);
        func_006CD1B0(D_00708478[o->kind][0]);
        for (i = 0; i < 0x10; i++) {
            D_00724530[p][i] = D_0048AC10.ev[i] & ~D_007246B0[i];
            D_00724530[p][i + 0x10] = D_0048AC10.ev2[i] & ~D_007246B0[i + 0x10];
            D_007243B0[p][i] = ~D_0048AC10.ev[i] & D_007246B0[i];
            D_007243B0[p][i + 0x10] = D_007246B0[i + 0x10] & ~D_0048AC10.ev2[i];
        }
    }
    for (p = 0; p < 3; p++) {
        for (i = 0; i < 0x10; i++) {
            D_007246B0[i] |= D_00724530[p][i];
            D_007246B0[i + 0x10] |= D_00724530[p][i + 0x10];
            D_007246B0[i] &= ~D_007243B0[p][i];
            D_007246B0[i + 0x10] &= ~D_007243B0[p][i + 0x10];
        }
    }
    func_00107B50(D_0048AD90, D_007246B0, 0x40);
    func_00107B50(D_0048ADD0, D_007246F0, 0x40);
    D_0048BF60.unk1D = 0;
}
#endif

/* The per-player flags: run as the scene (D_0023C004 0) a flag is all three players' (rows 1-3
   of D_0048ABD0, 0x10 words each; the second set from D_0048AC90), else the player's own. */
extern u32 D_0048ABD0[][0x10];
extern u32 D_0048AC50[0x10];
extern u32 D_0048AC90[][0x10];
extern u32 D_0048ACD0[0x10];
extern u32 D_0048AD10[0x10];
extern u32 D_0048AD50[0x10];

static s32 func_006CD3A0(u32 n) {
    s32 p = D_0023C004;

    if (p == 0) {
        return (1 << (n & 0x1F)) & D_0048AC10.unk0[n >> 5];
    }
    return (1 << (n & 0x1F)) & D_0048ABD0[p][n >> 5];
}

/* if this player's event flag args[0] is args[1] */
s32 func_006CE750(u32 *args) {
    u32 n = args[0];
    s32 v = args[1];

    D_0048BE70.unk78[(u8)D_0048BE70.cur]++;
    if (v != 0) {
        if (func_006CD3A0(n) == 0) {
            func_0065AAF0();
            return 2;
        }
    } else {
        if (func_006CD3A0(n) != 0) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}

s32 func_006CD410(u32 n);

/* if this player's event flag args[0] is set and its second-set flag clear, is args[1] */
s32 func_006CE7E0(u32 *args) {
    u32 n = args[0];
    s32 v = args[1];

    D_0048BE70.unk78[(u8)D_0048BE70.cur]++;
    if (v != 0) {
        if (func_006CD3A0(n) != 0) {
            if (func_006CD410(n) != 0) {
                func_0065AAF0();
                return 2;
            }
        } else {
            func_0065AAF0();
            return 2;
        }
    } else {
        if (func_006CD3A0(n) != 0 && func_006CD410(n) == 0) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}

/* if this player's second-set event flag args[0] is args[1] */
s32 func_006CE8A0(u32 *args) {
    u32 n = args[0];
    s32 v = args[1];

    D_0048BE70.unk78[(u8)D_0048BE70.cur]++;
    if (v != 0) {
        if (func_006CD410(n) == 0) {
            func_0065AAF0();
            return 2;
        }
    } else {
        if (func_006CD410(n) != 0) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}

s32 func_006CD410(u32 n);
#ifdef NON_MATCHING   /* first draft */
s32 func_006CD410(u32 n) {
    s32 p = D_0023C004;
    s32 bit;
    u32 w;

    if (p == 0) {
        bit = 1 << (n & 0x1F);
        w = D_0048ACD0[n >> 5];
    } else {
        bit = 1 << (n & 0x1F);
        w = D_0048AC90[p][n >> 5];
    }
    return w & bit;
}
#endif

void func_006CD480(u32 n) {
    s32 p = D_0023C004;

    if (p == 0) {
        D_0048AC10.unk0[n >> 5] |= 1 << (n & 0x1F);
        D_0048AC50[n >> 5] |= 1 << (n & 0x1F);
        D_0048AC90[0][n >> 5] |= 1 << (n & 0x1F);
    } else {
        D_0048ABD0[p][n >> 5] |= 1 << (n & 0x1F);
    }
}

void func_006CD530(u32 n) {
    s32 p = D_0023C004;

    if (p == 0) {
        D_0048ACD0[n >> 5] |= 1 << (n & 0x1F);
        D_0048AD10[n >> 5] |= 1 << (n & 0x1F);
        D_0048AD50[n >> 5] |= 1 << (n & 0x1F);
    } else {
        D_0048AC90[p][n >> 5] |= 1 << (n & 0x1F);
    }
}

void func_006CD5E0(u32 n) {
    s32 p = D_0023C004;
    u32 mask;

    if (p == 0) {
        mask = ~(1 << (n & 0x1F));
        D_0048AC10.unk0[n >> 5] &= mask;
        D_0048AC50[n >> 5] &= mask;
        D_0048AC90[0][n >> 5] &= mask;
    } else {
        D_0048ABD0[p][n >> 5] &= ~(1 << (n & 0x1F));
    }
}

void func_006CD690(u32 n) {
    s32 p = D_0023C004;
    u32 mask;

    if (p == 0) {
        mask = ~(1 << (n & 0x1F));
        D_0048ACD0[n >> 5] &= mask;
        D_0048AD10[n >> 5] &= mask;
        D_0048AD50[n >> 5] &= mask;
    } else {
        D_0048AC90[p][n >> 5] &= ~(1 << (n & 0x1F));
    }
}

void func_005403E0(s32 id, s32 on);
void func_00540450(s32 id, s32 on);

/* Script op online: flag args[1] set (args[2]) as args[0] says: 0 an event flag (told as ours),
   1 a per-player flag, 2 an event flag (told as the scene's). */
s32 func_006CD950(u32 *args);
#ifdef NON_MATCHING   /* the case tests */
s32 func_006CD950(u32 *args) {
    s32 kind = args[0];
    u8 on = args[2];
    s32 n = args[1];

    if (kind == 2) {
        func_00540450(n, on);
    } else if (kind == 1) {
        if (on == 0) {
            func_006CD5E0(n);
        } else {
            func_006CD480(n);
        }
    } else if (kind == 0) {
        func_005403E0(n, on);
    }
    return 0;
}
#endif

/* Script op: flag args[1] of the second event set on or off (args[2]), locally (args[0] 0) or
   told to the others too. */
s32 func_006CD9E0(u32 *args) {
    s32 shared = args[0];
    u32 n = args[1];
    s32 on = args[2];

    if (shared == 0) {
        if (on == 0) {
            D_0048ADD0[n >> 5] &= ~(1 << (n & 0x1F));
        } else {
            D_0048ADD0[n >> 5] |= 1 << (n & 0x1F);
        }
    } else if (on == 0) {
        func_006CD690(n);
    } else {
        func_006CD530(n);
    }
    return 0;
}

extern u32 D_0023BE14;          /* players */
void func_008223C0(DrawObjFull *p, s32 mode);   /* SINGLE.BIN */

/* Script op: the partner AI mode args[2] for player args[1] (args[0] 0), or for the partners
   playing character args[1]. */
s32 func_006CF070(u32 *args);
#ifdef NON_MATCHING   /* first draft */
s32 func_006CF070(u32 *args) {
    s32 mode = args[2];
    s32 who = args[1];
    DrawObjFull *p;
    u32 i;

    if (args[0] == 0) {
        p = &D_00476DD0[who];
        if (p->active != 0 && p->isNpc != 0) {
            func_008223C0(p, mode);
        }
    } else {
        for (i = 0, p = D_00476DD0; i < D_0023BE14; i++, p++) {
            if (p->active != 0 && p->isNpc != 0 && p->unkB98 == who) {
                func_008223C0(p, mode);
            }
        }
    }
    return 0;
}
#endif

/* The partner script ops (offline): on player args[0] when it is a partner. */
s32 func_006CF400(u32 *args) {
    s32 v = args[1];
    DrawObjFull *p = &D_00476DD0[args[0]];

    if (p->active == 0 || p->isNpc == 0) {
        return 0;
    }
    switch (v) {
    case 0:
        p->unkFF1 = 0;
        break;
    case 1:
        p->unkFF1 = 1;
        break;
    }
    return 0;
}

/* Every partner: unk1034 = (args[0] == 1). */
s32 func_006CF480(u32 *args);
#ifdef NON_MATCHING   /* first draft */
s32 func_006CF480(u32 *args) {
    s32 v = args[0];
    s32 i;
    DrawObjFull *p;

    for (i = 0; i < 8; i++) {
        p = &D_00476DD0[i];
        if (p->active != 0 && p->isNpc != 0) {
            if (v != 1) {
                p->unk1034 = 0;
            } else {
                p->unk1034 = 1;
            }
        }
    }
    return 0;
}
#endif

void func_00823340(DrawObjFull *obj, u16 v);    /* SINGLE.BIN */
void func_0082A690(DrawObjFull *obj, u16 v);    /* SINGLE.BIN */

s32 func_006CF4F0(u32 *args);
#ifdef NON_MATCHING   /* first draft */
s32 func_006CF4F0(u32 *args) {
    s32 a = args[1];
    s32 k = args[2];
    DrawObjFull *p = &D_00476DD0[args[0]];

    if (p->active == 0 || p->isNpc == 0) {
        return 0;
    }
    p->unk10A0 = a;
    switch (k) {
    case 1:
        p->unk109C = 2;
        break;
    case 2:
        p->unk109C = 0;
        break;
    case 3:
        p->unk109C = 1;
        break;
    default:
        p->unk109C = k != 0 ? -1 : 4;
        break;
    }
    func_00823340(p, 0);
    func_0082A690(p, 0);
    return 0;
}
#endif

s32 func_006CF5E0(u32 *args) {
    DrawObjFull *p = &D_00476DD0[args[0]];

    if (p->active == 0 || p->isNpc == 0) {
        return 0;
    }
    p->unk10A0 = 0;
    p->unk109C = -1;
    func_00823340(p, 0);
    func_0082A690(p, 0);
    return 0;
}

/* Every partner: unk10A8 = args[0] (0-2). */
s32 func_006CF670(u32 *args);
#ifdef NON_MATCHING   /* first draft */
s32 func_006CF670(u32 *args) {
    s32 v = args[0];
    s32 i;
    DrawObjFull *p;

    for (i = 0; i < 8; i++) {
        p = &D_00476DD0[i];
        if (p->active != 0 && p->isNpc != 0) {
            if (v == 2) {
                p->unk10A8 = 2;
            } else if (v == 1) {
                p->unk10A8 = 1;
            } else {
                p->unk10A8 = 0;
            }
        }
    }
    return 0;
}
#endif

void func_005C5A90(DrawObjFull *p, s32 a);

/* Partner args[0] leaves. */
s32 func_006CF700(u32 *args) {
    DrawObjFull *p = &D_00476DD0[args[0]];

    if (p->active == 0 || p->isNpc == 0) {
        return 0;
    }
    p->unkBA0 |= 0x5E0;
    func_005C5A90(p, 0);
    p->active = 0;
    p->visible = 0;
    return 0;
}

void func_00827130(f32 *out, u16 id, s8 place);
s32 func_008270C0(f32 r, f32 *pos, f32 *to);

/* if partner args[0] is player args[1] at place args[2] and within 30 of it */
s32 func_006CF790(u32 *args) {
    s32 id = args[1];
    s32 place = args[2];
    DrawObjFull *p = &D_00476DD0[args[0]];
    s32 r = 2;
    f32 pos[4];

    if (p->active == 0 || p->isNpc == 0) {
        func_0065AAF0();
        return 2;
    }
    if (p->unk1E2 == id && p->unk100C == place) {
        func_00827130(pos, p->unk1E2, p->unk100C);
        if (func_008270C0(30.0f, p->unk38, pos) == 0) {
            r = 0;
        }
    }
    if (r != 0) {
        func_0065AAF0();
    }
    return r;
}

/* Our partner leaves when event flag args[1] (args[0] 0: the shared one, else this player's)
   is args[2]. */
s32 func_006CF860(u32 *args);
#ifdef NON_MATCHING   /* and operand order in the flag tests */
s32 func_006CF860(u32 *args) {
    s32 which = args[0];
    u32 n = args[1];
    s32 v = args[2];
    DrawObjFull *p = &D_00476DD0[D_0048BF60.unk1D];
    s32 hit = 0;

    if (p->active == 0 || p->isNpc == 0) {
        return 0;
    }
    if (which == 0) {
        if (v != 0 && ((1 << (n & 0x1F)) & ((s32 *)D_0048AD90)[n >> 5])) {
            hit = 1;
        } else if (v == 0 && !((1 << (n & 0x1F)) & ((s32 *)D_0048AD90)[n >> 5])) {
            hit = 1;
        }
    } else {
        if (v != 0 && func_006CD3A0(n) != 0) {
            hit = 1;
        } else if (v == 0 && func_006CD3A0(n) == 0) {
            hit = 1;
        }
    }
    if (hit != 0) {
        func_005C5A90(p, 0);
        p->active = 0;
        p->visible = 0;
    }
    return 0;
}
#endif

void func_00826420(s32 a, s32 b, s32 c);        /* SINGLE.BIN */

s32 func_006CF040(u32 *args) {
    func_00826420(args[0], args[1], args[2]);
    return 0;
}

/* (SINGLE.BIN) per room: its script's routine table */
typedef struct {
    u32 **tbl;
    u8 unk4[0xA4 - 4];
} RoomScriptRef;

extern RoomScriptRef D_0084E344[];

/* (SINGLE.BIN) the offline script's per-player switch entries: what func_006CDA90 sets */
typedef struct {
    u8 on;              /* 0x0 */
    u8 kind;            /* 0x1: 0 by id, 2 by id and id2 */
    u8 id;              /* 0x2 */
    u8 id2;             /* 0x3 */
    s16 val;            /* 0x4 */
    u8 unk6[0x10 - 6];
} SglSwitch;

typedef struct {
    u8 unk0[0x98];
    s32 n;              /* 0x98 */
    SglSwitch *sw;      /* 0x9C */
    u8 *seen;           /* 0xA0: per id, set by func_006CE9A0 */
} SglRoomSw;            /* 0xA4 */

extern SglRoomSw D_0084E2B0[];

/* Sets the value of the zones (and, for the player, the offline switches) of kind 0 with id
   args[0] or kind 2 with ids args[0], args[2] to args[1]. */
s32 func_006CDA90(u32 *args) {
    s32 v;
    s32 id;
    s32 id2;
    Zone *z;
    SglSwitch *e;
    s32 i;

    id = args[0];
    v = args[1];
    id2 = args[2];

    if (D_0023C00C == 0) {
        if (D_0023BD88 == 0x80) {
            return 0;
        }
        z = D_0023BD90;
        while (z != NULL) {
            if (z->kind == 0) {
                if (z->u.id == id) {
                    z->unk6C = v;
                    z->unk72 = 0;
                }
                z = z->next;
            } else if (z->kind == 2) {
                if (z->u.id == id && FIELD(z, s32, 0x50) == id2) {
                    z->unk6C = v;
                }
                z = z->next;
            } else {
                z = z->next;
            }
        }
    }
    e = D_0084E2B0[D_0023C008].sw;
    for (i = 0; i < D_0084E2B0[D_0023C008].n; i++, e++) {
        if (e->on != 0) {
            if (e->kind == 0 && e->id == id) {
                e->val = v;
            }
            if (e->kind == 2 && e->id == id && e->id2 == id2) {
                e->val = v;
            }
        }
    }
    return 0;
}

Zone *func_005518B0(u8 id, s32 which);

/* Sets the value (0x66) of the zone with id args[0] (kind 2: both of its halves) to args[1]. */
s32 func_006CEA10(u32 *args) {
    s32 id = args[0];
    s32 v = (u16)args[1];
    Zone *z;

    if (D_0023C00C == 0) {
        z = func_005518B0(id, 0);
        if (z == NULL) {
            return 0;
        }
        if (z->kind == 0) {
            FIELD(z, s16, 0x66) = v;
        } else if (z->kind == 2) {
            v = FIELD(z, s16, 0x66) = v;
            z = func_005518B0(id, 1);
            FIELD(z, s16, 0x66) = v;
        }
    }
    return 0;
}

void func_005AC1F0(s32 n);

void func_005AC1C0(s32 n);

/* Clears what func_006CE9A0 marks. */
s32 func_006CE930(u32 *args) {
    s32 n = args[0];

    if (D_0023C00C == 0) {
        func_005AC1C0(n);
    }
    D_0084E2B0[D_0023C008].seen[n] = 0;
    return 0;
}


/* Marks id args[0] (in the scene: func_005AC1F0's flag too) as done for this player. */
s32 func_006CE9A0(u32 *args) {
    s32 n = args[0];

    if (D_0023C00C == 0) {
        func_005AC1F0(n);
    }
    D_0084E2B0[D_0023C008].seen[n] = 1;
    return 0;
}

/* Arms the kind-13 zones with id args[0], with args[1] (at 0x62). */
s32 func_006CEAA0(u32 *args) {
    s32 id;
    s16 v;
    Zone *z;

    id = args[0];
    v = args[1];
    if (D_0023C00C == 0) {
        if (D_0023BD88 == 0x80) {
            return 0;
        }
        z = D_0023BD90;
        while (z != NULL) {
            if (z->kind == 13) {
                if (z->unk44 == id) {
                    z->on = 1;
                    FIELD(z, s16, 0x62) = v;
                }
                z = z->next;
            } else {
                z = z->next;
            }
        }
    }
    return 0;
}

/* Arms the zones of kind args[0] with id args[1] (and second id args[2] for kinds 2 and 3;
   where a kind keeps its id differs) and the matching offline switches. */
s32 func_006CEB20(u32 *args) {
    s32 kind;
    s32 id;
    s32 id2;
    Zone *z;
    SglSwitch *e;
    s32 i;

    kind = args[0];
    id = args[1];
    id2 = args[2];
    if (D_0023C00C == 0) {
        if (D_0023BD88 == 0x80) {
            return 0;
        }
        z = D_0023BD90;
        while (z != NULL) {
            if (kind != z->kind) {
                z = z->next;
            } else {
                switch (z->kind) {
                case 0:
                    if (z->u.id == id) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                case 1:
                    if (z->u.id == id) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                case 2:
                    if (z->kind == 2) {
                        if (z->u.id == id && FIELD(z, s32, 0x50) == id2) {
                            z->on = 1;
                        }
                        z = z->next;
                        break;
                    }
                    z = z->next;
                    break;
                case 3:
                    if (z->u.id == id && (z->unk44 & 1) == id2) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                case 4:
                    if (z->unk44 == id) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                case 6:
                    if (FIELD(z, s32, 0x54) == id) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                case 7:
                    if (FIELD(z, s32, 0x70) == id) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                case 11:
                    if (z->u.id == id) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                case 13:
                    if (z->unk44 == id) {
                        z->on = 1;
                    }
                    z = z->next;
                    break;
                default:
                    z = z->next;
                    break;
                }
            }
        }
    }
    e = D_0084E2B0[D_0023C008].sw;
    for (i = 0; i < D_0084E2B0[D_0023C008].n; i++, e++) {
        if (e->kind == kind && e->id == id && (e->kind != 2 || e->id2 == id2)
            && (e->kind != 3 || (e->id2 & 1) == id2)) {
            e->on = 1;
        }
    }
    return 0;
}

/* Disarms the zones of kind args[0] with id args[1] (and second id args[2] for kinds 2 and 3;
   where a kind keeps its id differs) and the matching offline switches. */
s32 func_006CEDB0(u32 *args) {
    s32 kind;
    s32 id;
    s32 id2;
    Zone *z;
    SglSwitch *e;
    s32 i;

    kind = args[0];
    id = args[1];
    id2 = args[2];
    if (D_0023C00C == 0) {
        if (D_0023BD88 == 0x80) {
            return 0;
        }
        z = D_0023BD90;
        while (z != NULL) {
            if (kind != z->kind) {
                z = z->next;
            } else {
                switch (z->kind) {
                case 0:
                    if (z->u.id == id) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                case 1:
                    if (z->u.id == id) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                case 2:     /* (the original re-tests the kind, falling into case 3) */
                    if (z->kind == 2) {
                        if (z->u.id == id && FIELD(z, s32, 0x50) == id2) {
                            z->on = 0;
                        }
                        z = z->next;
                        break;
                    }
                    z = z->next;
                case 3:
                    if (z->u.id == id && (z->unk44 & 1) == id2) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                case 4:
                    if (z->unk44 == id) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                case 6:
                    if (FIELD(z, s32, 0x54) == id) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                case 7:
                    if (FIELD(z, s32, 0x70) == id) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                case 11:
                    if (z->u.id == id) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                case 13:
                    if (z->unk44 == id) {
                        z->on = 0;
                    }
                    z = z->next;
                    break;
                default:
                    z = z->next;
                    break;
                }
            }
        }
    }
    e = D_0084E2B0[D_0023C008].sw;
    for (i = 0; i < D_0084E2B0[D_0023C008].n; i++, e++) {
        if (e->kind == kind && e->id == id && (e->kind != 2 || e->id2 == id2)
            && (e->kind != 3 || (e->id2 & 1) == id2)) {
            e->on = 0;
        }
    }
    return 0;
}

/* Disarms the kind-3 zones (sub-kinds 0 and 2) and the kind-4 zones (0x64 0) with id args[0],
   and the matching offline switches. */
s32 func_006CDD80(u32 *args) {
    s32 id;
    Zone *z;
    SglSwitch *e;
    s32 i;

    id = args[0];
    if (D_0023C00C == 0) {
        if (D_0023BD88 == 0x80) {
            return 0;
        }
        z = D_0023BD90;
        while (z != NULL) {
            if (z->kind == 3 && (z->unk44 == 0 || z->unk44 == 2)) {
                if (z->u.id == id) {
                    z->on = 0;
                }
                z = z->next;
            } else if (z->kind == 4 && FIELD(z, s32, 0x64) == 0) {
                if (z->u.id == id) {
                    z->on = 0;
                }
                z = z->next;
            } else {
                z = z->next;
            }
        }
    }
    e = D_0084E2B0[D_0023C008].sw;
    for (i = 0; i < D_0084E2B0[D_0023C008].n; i++, e++) {
        if (e->kind == 3 && e->id == id && (e->id2 == 0 || e->id2 == 2)) {
            e->on = 0;
        }
    }
    return 0;
}

/* Arms the kind-3 zones (sub-kinds 0 and 2; 2 also takes args[1] at 0x58) and the kind-4 zones
   (0x64 0) with id args[0], and the matching offline switches. */
s32 func_006CDC00(u32 *args) {
    s32 id;
    Zone *z;
    u16 v;
    SglSwitch *e;
    s32 i;

    id = args[0];
    v = args[1];
    if (D_0023C00C == 0) {
        if (D_0023BD88 == 0x80) {
            return 0;
        }
        z = D_0023BD90;
        while (z != NULL) {
            if (z->kind == 3 && (z->unk44 == 0 || z->unk44 == 2)) {
                if (z->u.id == id) {
                    z->on = 1;
                    if (z->unk44 == 2) {
                        FIELD(z, u16, 0x58) = v;
                    }
                }
                z = z->next;
            } else if (z->kind == 4 && FIELD(z, s32, 0x64) == 0) {
                if (z->u.id == id) {
                    z->on = 1;
                }
                z = z->next;
            } else {
                z = z->next;
            }
        }
    }
    e = D_0084E2B0[D_0023C008].sw;
    for (i = 0; i < D_0084E2B0[D_0023C008].n; i++, e++) {
        if (e->kind == 3 && e->id == id && (e->id2 == 0 || e->id2 == 2)) {
            e->on = 1;
        }
    }
    return 0;
}

/* Arms the kind-3 zones (sub-kinds 1 and 3; 3 also takes args[1] at 0x58) and the kind-4 zones
   (0x64 1) with id args[0], and the matching offline switches. */
s32 func_006CDFF0(u32 *args) {
    s32 id;
    Zone *z;
    u16 v;
    SglSwitch *e;
    s32 i;

    id = args[0];
    v = args[1];
    if (D_0023C00C == 0) {
        if (D_0023BD88 == 0x80) {
            return 0;
        }
        z = D_0023BD90;
        while (z != NULL) {
            if (z->kind == 3 && (z->unk44 == 1 || z->unk44 == 3)) {
                if (z->u.id == id) {
                    z->on = 1;
                    if (z->unk44 == 3) {
                        FIELD(z, u16, 0x58) = v;
                    }
                }
                z = z->next;
            } else if (z->kind == 4 && FIELD(z, s32, 0x64) == 1) {
                if (z->u.id == id) {
                    z->on = 1;
                }
                z = z->next;
            } else {
                z = z->next;
            }
        }
    }
    e = D_0084E2B0[D_0023C008].sw;
    for (i = 0; i < D_0084E2B0[D_0023C008].n; i++, e++) {
        if (e->kind == 3 && e->id == id && (e->id2 == 1 || e->id2 == 3)) {
            e->on = 1;
        }
    }
    return 0;
}

/* Starts thread t on routine 8 of room's script (and thread 5 on routine 12). */
void func_006CCE30(s32 t, s32 room) {
    u32 **tbl;

    D_00306590.unk29 = 1;
    tbl = D_0084E344[room].tbl;
    D_0048BE70.entry[t] = 8;
    D_0048BE70.wait[t] = 0;
    D_0048BE70.pc[t] = tbl[8];
    D_0048BE70.base[t] = tbl;
    D_0048BE70.entry[5] = 0xC;
    D_0048BE70.wait[5] = 0;
    D_0048BE70.pc[5] = tbl[0xC];
    D_0048BE70.base[5] = tbl;
}

/* Starts each of the first three characters' threads on their rooms' scripts. */
void func_006CCD20(void) {
    s32 i;

    for (i = 0; i < 3; i++) {
        if (D_00476DD0[i].active != 0) {
            func_006CCE30(*(s32 *)D_00708478[i], D_00476DD0[i].unk1E2);
        }
    }
}

/* Offline, for a partner: starts its character's thread on its room's script. */
void func_006CCDA0(DrawObjFull *o) {
    if (o->kind < 3) {
        func_006CCE30(*(s32 *)D_00708478[o->kind], o->unk1E2);
    }
}

/* Runs the first character's thread on room's script, as our own player. */
void func_006CCDE0(s32 room) {
    func_006CCE30(*(s32 *)D_00708478[0], room);
    D_0023C00C = 2;
    D_0023C008 = room;
    D_0023C004 = 1;
    func_006CD1B0(D_00708478[0][0]);
}
