#include "common.h"
#include "options.h"
#include "game.h"
#include "script.h"
#include "object.h"

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
