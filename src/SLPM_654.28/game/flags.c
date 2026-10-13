#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"
#include "sce.h"
#include "script.h"

/* Offline play: the script runs once per character, and what each run changes in the scene's
   event flags is merged afterwards. */

typedef struct {
    u8 unk0[0x180];
    u32 flags[2][0x10];     /* 0x180: the scene's event flags */
} SceneFlags;

extern SceneFlags D_0048AC10;
extern DrawObjFull D_00476DD0[];
extern u32 D_007246B0[2][0x10];         /* the flags before the runs */
extern u32 D_00724530[3][2][0x10];      /* set by player p's run */
extern u32 D_007243B0[3][2][0x10];      /* cleared by player p's run */
extern u8 D_00708478[][4];              /* the thread for each character kind */
extern s32 D_0023C004;                  /* the character kind + 1 running the script */
extern s32 D_0023C008;                  /* its player id */
extern s32 D_0023C00C;                  /* 1 when it is not ours */

void func_006CD1B0(s32 t);

void func_006CCEE0(s8 t);
#ifdef NON_MATCHING   /* the switch keeps a branch to the next instruction for case 4; rest matches */
void func_006CCEE0(s8 t) {
    DrawObjFull *obj;
    s32 p;
    s32 i;

    switch ((u8)t) {
    case 4:
    case 5:
    default:
        D_0023C00C = 0;
        D_0023C008 = 0;
        D_0023C004 = 0;
        func_006CD1B0(t);
        break;
    case 1:
        memcpy(D_007246B0[0], D_0048AC10.flags[0], 0x40);
        memcpy(D_007246B0[1], D_0048AC10.flags[1], 0x40);
        for (p = 0; p < 3; p++) {
            D_0048BF60.unk1D = p;
            obj = &D_00476DD0[p];
            if (obj->active != 0) {
                if (obj->unk1E2 == D_00306590.unk1C) {
                    D_0023C00C = 0;
                } else {
                    D_0023C00C = 1;
                }
                D_0023C008 = obj->unk1E2;
                D_0023C004 = obj->kind + 1;
                memcpy(D_0048AC10.flags[0], D_007246B0[0], 0x40);
                memcpy(D_0048AC10.flags[1], D_007246B0[1], 0x40);
                func_006CD1B0(D_00708478[obj->kind][0]);
                for (i = 0; i < 0x10; i++) {
                    D_00724530[p][0][i] = D_0048AC10.flags[0][i] & ~D_007246B0[0][i];
                    D_00724530[p][1][i] = D_0048AC10.flags[1][i] & ~D_007246B0[1][i];
                    D_007243B0[p][0][i] = ~D_0048AC10.flags[0][i] & D_007246B0[0][i];
                    D_007243B0[p][1][i] = D_007246B0[1][i] & ~D_0048AC10.flags[1][i];
                }
            }
        }
        for (p = 0; p < 3; p++) {
            for (i = 0; i < 0x10; i++) {
                D_007246B0[0][i] |= D_00724530[p][0][i];
                D_007246B0[1][i] |= D_00724530[p][1][i];
                D_007246B0[0][i] &= ~D_007243B0[p][0][i];
                D_007246B0[1][i] &= ~D_007243B0[p][1][i];
            }
        }
        memcpy(D_0048AC10.flags[0], D_007246B0[0], 0x40);
        memcpy(D_0048AC10.flags[1], D_007246B0[1], 0x40);
        D_0048BF60.unk1D = 0;
        break;
    }
}
#endif

/* The offline interpreter: like func_0065A770, but each opcode says whether it runs for the
   other characters (unk4: skipped for one that is not ours, unk6: skipped in mode 2, unk5:
   skipped for characters after the first). A skipped instruction can still jump. */
typedef struct {
    s32 (*fn)(u32 *args);
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
} ScriptOpInfo;

extern ScriptOpInfo D_00708490[];
s32 func_0065AAA0(u32 insn);
void func_0065AAF0(void);

#ifdef NON_MATCHING   /* like func_0065A770: the wait test is a materialized !(wait != 0) and &wait[t] is kept in a register */
void func_006CD1B0(s32 t) {
    ScriptOpInfo *info;
    u32 insn;
    u32 op;
    s32 r;
    s32 skip;

    D_0048BE70.cur = t;
    if (D_00306590.unk28 == 0) {
        return;
    }
    D_0048BE70.unk78[(u8)t] = 0;
    for (;;) {
        insn = *D_0048BE70.pc[(u8)t];
        if (D_0048BE70.wait[(u8)t] != 0) {
            D_0048BE70.wait[(u8)t]--;
            return;
        }
        op = insn & 0xFFFFFF;
        r = 0;
        info = &D_00708490[op];
        skip = 0;
        if (info->unk5 != 0 && D_0023C004 >= 2) {
            skip = 1;
        } else if (D_0023C00C == 1 && info->unk4 != 0) {
            skip = 1;
        } else if (D_0023C00C == 2 && info->unk6 != 0) {
            skip = 1;
        } else {
            r = info->fn(D_0048BE70.pc[(u8)t] + 1);
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
            D_0048BE70.pc[(u8)t] += 1 << ((insn & 0xFF000000) >> 24);
        }
    }
}
#endif
