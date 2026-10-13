#include "common.h"
#include "options.h"
#include "script.h"
#include "game.h"

/* GAME.BIN: the room script (r%03d.evb): its header is 0x20 offsets, made pointers when it is
   loaded; entries 0x10-0x17 are tables of offsets ending with 0. */

void func_005A7450(void);
void func_0065A520(u32 *script);

void func_0065A370(void) {
    s32 i;
    s32 j;
    u32 *buf;
    u32 *t;

    D_0048BE70.wait[0] = 0;
    D_0048BE70.entry[0] = 0;
    D_00306590.unk28 = 1;
    D_0048BE70.entry[4] = 4;
    D_0048BE70.wait[4] = 0;
    for (i = 0; i < 3; i++) {
        D_0048BE70.unk90[i][0] = 0;
        D_0048BE70.unk90[i][1] = 0;
    }
    D_0048BE70.unkC8 = 0;
    D_0048BE70.unkCC = 0;
    D_0048BE70.unkD0 = 0;
    D_0048BE70.unkD4 = 0;
    for (i = 7; i >= 0; i--) {
        D_0048BE70.unkA8[i] = -1;
        D_0048BE70.unkD8[i] = i;
    }
    buf = (u32 *)D_00306590.script;
    for (i = 0; i < 0x20; i++) {
        if (buf[i] != 0) {
            buf[i] += (u32)buf;
        }
    }
    buf = (u32 *)D_00306590.script;
    for (i = 0x10; i < 0x18; i++) {
        t = (u32 *)buf[i];
        if (t != NULL) {
            for (j = 0; t[j] != 0; j++) {
                t[j] += (u32)buf;
            }
        }
    }
    D_0048BE70.pc[0] = (u32 *)buf[D_0048BE70.entry[0]];
    D_0048BE70.pc[4] = (u32 *)buf[D_0048BE70.entry[4]];
    D_0048BE70.base[0] = (u32 **)buf;
    D_0048BE70.base[4] = (u32 **)buf;
    func_005A7450();
    func_0065A520(buf);
}

/* Without a script: the interpreter runs an empty one. */
extern u8 D_0023AD18[];

void func_0065A2B0(void);
#ifdef NON_MATCHING   /* first draft */
void func_0065A2B0(void) {
    s32 i;
    s8 k;

    D_0048BE70.wait[0] = 0;
    D_0048BE70.wait[4] = 0;
    D_0048BE70.pc[0] = (u32 *)D_0023AD18;
    D_0048BE70.pc[4] = (u32 *)D_0023AD18;
    for (i = 0; i < 3; i++) {
        D_0048BE70.unk90[i][0] = 0;
        D_0048BE70.unk90[i][1] = 0;
    }
    D_0048BE70.unkC8 = 0;
    D_00306590.unk28 = 1;
    D_0048BE70.unkCC = 0;
    D_0048BE70.unkD0 = 0;
    D_0048BE70.unkD4 = 0;
    for (k = 7; k >= 0; k--) {
        D_0048BE70.unkA8[k] = -1;
        D_0048BE70.unkD8[k] = k;
    }
}
#endif

/* The scene's tables from the script: header entries 8-11 (lists ending with 0xFF; 10 and 11 start
   with an enemy type each) and 14-15 (lists ending with 0xFFFF). */
typedef struct {
    u8 unk0[0x9A0];
    u16 lists[4][0x80];     /* 0x9A0 */
    u16 emType[2];          /* 0xDA0 */
    u16 unkDA4[2];          /* 0xDA4 */
    u8 unkDA8[8];
    u16 lists2[2][0x80];    /* 0xDB0 */
    u8 unkFB0[0x1254 - 0xFB0];
} SceneData;

extern SceneData D_0048AC10;
extern u8 D_0048A010[0x800];

#ifdef NON_MATCHING   /* first draft */
void func_0065A520(u32 *script) {
    s32 k;
    s32 j;
    u16 *p;
    u16 v;

    for (k = 8; k < 0xC; k++) {
        p = (u16 *)script[k];
        if (k >= 0xA) {
            D_0048AC10.emType[k - 0xA] = p[0];
            D_0048AC10.unkDA4[k - 0xA] = p[1];
            p += 2;
        }
        for (j = 0; (v = *p) != 0xFF; j++) {
            p++;
            D_0048AC10.lists[k - 8][j] = v;
        }
    }
    for (k = 0xE; k < 0x10; k++) {
        p = (u16 *)script[k];
        for (j = 0; (v = *p) != 0xFFFF; j++) {
            p++;
            D_0048AC10.lists2[k - 0xE][j] = v;
        }
    }
    memset(D_0048A010, 0xFF, sizeof(D_0048A010));
}
#endif

/* Every room's ... set from the difficulty's table. */
typedef struct {
    u8 unk0[8];
    u16 unk8;
    u8 unkA[0x20 - 0xA];
} DiffEntry;

typedef struct {
    u8 unk0[0x2F00];
    u16 unk2F00[0x80][0x18];
} RoomTbl;

extern DiffEntry D_006E36D0[];
extern RoomTbl D_00470090;
extern u8 D_0048C01A[];

#ifdef NON_MATCHING   /* first draft */
void func_005A7450(void) {
    s32 i;
    s32 j;

    for (i = 0; i < 0x80; i++) {
        for (j = 0x17; j >= 0; j--) {
            D_00470090.unk2F00[i][j] = D_006E36D0[D_0048C01A[0]].unk8;
        }
    }
}
#endif

/* The interpreter: runs thread t until an instruction yields (0x0A, 0x0D) or it sleeps. Each
   instruction is a word: the opcode in the low 24 bits, log2 of its length in words above. */
typedef s32 (*ScriptOp)(u32 *args);

extern ScriptOp D_006E3870[];
s32 func_001AF4D0(void);
void func_006CCEE0(s8 t);

void func_0065A770(s8 t);
#ifdef NON_MATCHING   /* first draft */
void func_0065A770(s8 t) {
    u32 insn;
    u32 op;
    s32 r;

    if (func_001AF4D0() == 1) {
        func_006CCEE0(t);
        return;
    }
    D_0048BE70.cur = t;
    if (D_00306590.unk28 == 0) {
        return;
    }
    D_0048BE70.unk78[(u8)t] = 0;
    for (;;) {
        insn = *D_0048BE70.pc[(u8)t];
        if (!(D_0048BE70.wait[(u8)t] == 0)) {
            D_0048BE70.wait[(u8)t]--;
            return;
        }
        op = insn & 0xFFFFFF;
        r = D_006E3870[op](D_0048BE70.pc[(u8)t] + 1);
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

void func_0065A960(void) {
    D_0048BE70.entry[0] = 0;
    D_0048BE70.pc[0] = ((u32 **)D_00306590.script)[D_0048BE70.entry[0]];
    func_0065A770(0);
}

void func_0065A9A0(void) {
    D_0048BE70.entry[0] = 1;
    D_0048BE70.pc[0] = ((u32 **)D_00306590.script)[D_0048BE70.entry[0]];
    func_0065A770(0);
}

void func_005403E0(s32 id, s32 arg);
void func_005405F0(s32 id, s32 arg, s32 arg2);

void func_0065A8C0(s32 arg);
#ifdef NON_MATCHING   /* the else branch's andi is hoisted into the delay slot */
/* The script's delayed sounds. */
void func_0065A8C0(s32 arg) {
    s32 i;

    for (i = 0; i < 8; i++) {
        if (D_0048BE70.unkA8[i] > 0) {
            D_0048BE70.unkA8[i]--;
            if (D_0048BE70.unkA8[i] == 0) {
                if (D_0048BE70.unkB8[i] & 0x8000) {
                    func_005405F0(D_0048BE70.unkB8[i] & 0x7FFF, 1, 0);
                } else {
                    func_005403E0(D_0048BE70.unkB8[i] & 0x7FFF, 1);
                }
            }
        }
    }
}
#endif

/* Each frame: the delayed sounds, thread 0, func_005A7630, thread 1. After 2.5 hours the game
   is over. */
void func_005A7630(void);
void func_00551D40(void);
void func_00635780(s32 arg);

void func_0065A9E0(s32 arg) {
    if (D_0048BF60.unk18 >= 540000) {
        arg = 1;
        if (D_0048BF60.unkA7 == 0) {
            func_00635780(1);
            return;
        }
    }
    func_0065A8C0(arg);
    func_0065A770(0);
    func_005A7630();
    func_0065A770(1);
    func_00551D40();
}

/* Skips n instructions. */
u32 *func_0065AA60(u32 *pc, u32 n) {
    u32 i;

    for (i = 0; i < n; i++) {
        pc += SCRIPT_LEN(*pc);
    }
    return pc;
}

/* 1 when the instruction opens an if-block (the opcodes in D_006E3640, ending with -1). */
extern s32 D_006E3640[];

s32 func_0065AAA0(u32 insn) {
    s32 i;
    s32 v;

    for (i = 0; (v = D_006E3640[i]) != -1; i++) {
        if ((insn & 0xFFFFFF) == v) {
            return 1;
        }
    }
    return 0;
}

void func_0065AAF0(void);
#ifdef NON_MATCHING   /* one more instruction: the 0xE4 test's scheduling */
/* A false if: skips to the matching else (0xE4, run what follows) or endif (0xE5). */
void func_0065AAF0(void) {
    s32 depth;
    u32 insn;
    u32 op;

    depth = D_0048BE70.unk78[(u8)D_0048BE70.cur];
    for (;;) {
        insn = *D_0048BE70.pc[(u8)D_0048BE70.cur];
        D_0048BE70.pc[(u8)D_0048BE70.cur] += SCRIPT_LEN(insn);
        insn = *D_0048BE70.pc[(u8)D_0048BE70.cur];
        D_0048BE70.unk78[(u8)D_0048BE70.cur] += func_0065AAA0(insn);
        op = insn & 0xFFFFFF;
        if (op == 0xE4) {
            if (depth == D_0048BE70.unk78[(u8)D_0048BE70.cur]) {
                D_0048BE70.pc[(u8)D_0048BE70.cur] += SCRIPT_LEN(*D_0048BE70.pc[(u8)D_0048BE70.cur]);
                return;
            }
        }
        if (op == 0xE5) {
            if (depth == D_0048BE70.unk78[(u8)D_0048BE70.cur]) {
                D_0048BE70.unk78[(u8)D_0048BE70.cur]--;
                D_0048BE70.pc[(u8)D_0048BE70.cur] += SCRIPT_LEN(*D_0048BE70.pc[(u8)D_0048BE70.cur]);
                return;
            }
            D_0048BE70.unk78[(u8)D_0048BE70.cur]--;
        }
        if (op == 0xA) {
            D_0048BE70.unk78[(u8)D_0048BE70.cur] = 0;
            return;
        }
        if (op == 0xD) {
            D_0048BE70.unk78[(u8)D_0048BE70.cur] = 0;
            return;
        }
    }
}
#endif

/* At an else: skips to the matching endif. */
void func_0065ACF0(void) {
    s32 depth;
    u32 insn;
    u32 op;

    depth = D_0048BE70.unk78[(u8)D_0048BE70.cur];
    for (;;) {
        insn = *D_0048BE70.pc[(u8)D_0048BE70.cur];
        D_0048BE70.pc[(u8)D_0048BE70.cur] += SCRIPT_LEN(insn);
        insn = *D_0048BE70.pc[(u8)D_0048BE70.cur];
        D_0048BE70.unk78[(u8)D_0048BE70.cur] += func_0065AAA0(insn);
        op = insn & 0xFFFFFF;
        if (op == 0xE5) {
            if (depth == D_0048BE70.unk78[(u8)D_0048BE70.cur]) {
                D_0048BE70.unk78[(u8)D_0048BE70.cur]--;
                D_0048BE70.pc[(u8)D_0048BE70.cur] += SCRIPT_LEN(*D_0048BE70.pc[(u8)D_0048BE70.cur]);
                return;
            }
            D_0048BE70.unk78[(u8)D_0048BE70.cur]--;
        }
        if (op == 0xA) {
            D_0048BE70.unk78[(u8)D_0048BE70.cur] = 0;
            return;
        }
        if (op == 0xD) {
            D_0048BE70.unk78[(u8)D_0048BE70.cur] = 0;
            return;
        }
    }
}

/* Threads 1-3 and 5 start on the built-in idle scripts. */
extern u32 D_006E34F0[];

void func_0065A620(void) {
    D_0048BE70.wait[1] = 0;
    D_0048BE70.wait[2] = 0;
    D_0048BE70.pc[1] = D_006E34F0;
    D_0048BE70.pc[5] = (u32 *)D_0023AD18;
    D_00306590.unk29 = 1;
    D_0048BE70.pc[2] = D_006E34F0;
    D_0048BE70.pc[3] = D_006E34F0;
    D_0048BE70.wait[3] = 0;
    D_0048BE70.wait[5] = 0;
}

/* The scene variant: the script's table 0x18 (offsets, ending with -1) entry D_0048AEA5, at
   Options.unk1C; -1 when there's none. */
extern u8 D_0048AEA5[];

s32 func_00685060(void);
#ifdef NON_MATCHING   /* first draft */
s32 func_00685060(void) {
    u8 *script = D_00306590.script;
    s32 *table = ((s32 **)script)[0x18];
    s32 n = 0;
    u8 i;

    if (table == NULL) {
        return -1;
    }
    while (table[n] != -1) {
        n++;
    }
    i = D_0048AEA5[0];
    if (n < i) {
        return -1;
    }
    return script[table[i] + D_00306590.unk1C];
}
#endif
