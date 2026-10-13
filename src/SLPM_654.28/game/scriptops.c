#include "common.h"
#include "options.h"
#include "game.h"
#include "script.h"
#include "object.h"

/* The room script's instructions (the table at D_006E3870, by opcode). Each gets the words after
   the opcode and returns 0 to go on to the next instruction, 1 when it set the pc itself, or 2
   when it jumped (the interpreter re-reads the instruction at the new pc). An if-instruction
   counts a level in unk78 and, when false, skips to the matching else/endif (func_0065AAF0). */

#define CUR ((u8)D_0048BE70.cur)
#define BIT_TEST(a, n) ((1 << ((n) & 0x1F)) & (a)[(u32)(n) >> 5])
#define BIT_SET(a, n) ((a)[(u32)(n) >> 5] |= 1 << ((n) & 0x1F))
#define BIT_CLR(a, n) ((a)[(u32)(n) >> 5] &= ~(1 << ((n) & 0x1F)))

/* The scene's bit arrays (inside D_0048AC10). */
extern u32 D_0048AC10[];
extern u32 D_0048ACD0[];
extern u32 D_0048AD90[];        /* the event flags */
extern u32 D_0048ADD0[];

/* (0x198 bytes each; the first also holds the globals) */
typedef struct {
    u8 unk0;
    s8 count;           /* 0x01 */
    u8 unk2;
    u8 unk3;            /* 0x03 */
    u8 unk4;
    u8 cur;             /* 0x05 */
    s16 unk6;           /* 0x06 */
    u8 unk8[0x24 - 0x8];
    u8 unk24;           /* 0x24: the character kind we play */
    u8 unk25;           /* 0x25 */
    u8 unk26[0x34 - 0x26];
    s8 unk34;           /* 0x34 */
    u8 unk35[0x198 - 0x35];
} Unk3AEF50;

extern Unk3AEF50 D_003AEF50[];

void func_0065AAF0(void);
u32 *func_0065AA60(u32 *pc, u32 n);

/* 0x00, and the unused opcodes */
s32 func_0065AE90(u32 *args) {
    return 0;
}

/* 0x01: marks the loop start (the next instruction) */
s32 func_0065AEA0(u32 *args) {
    D_0048BE70.loop[CUR] = D_0048BE70.pc[CUR] + SCRIPT_LEN(*D_0048BE70.pc[CUR]);
    return 0;
}

/* 0x02: back to the loop start */
s32 func_0065AF00(u32 *args) {
    D_0048BE70.pc[CUR] = D_0048BE70.loop[CUR];
    return 1;
}

/* 0x03: goes forward args[0] instructions */
s32 func_0065AF40(u32 *args) {
    D_0048BE70.pc[CUR] = func_0065AA60(D_0048BE70.pc[CUR], args[0]);
    return 1;
}

/* 0x04: sleeps args[0] frames */
s32 func_0065AFA0(u32 *args) {
    D_0048BE70.wait[CUR] = args[0];
    return 0;
}

s32 func_0065AFD0(u32 *args);
#ifdef NON_MATCHING   /* regalloc: two address registers swapped */
/* 0x05: starts our timer args[0] (args[1] frames, args[2] what for) unless it runs */
s32 func_0065AFD0(u32 *args) {
    u32 i = args[0];
    s32 c = args[1];
    u8 a = args[2];

    if (D_0048BE24[D_0048BF60.unk1D].on[i] != 0) {
        return 0;
    }
    D_0048BE24[D_0048BF60.unk1D].on[i] = 1;
    D_0048BE24[D_0048BF60.unk1D].arg[i] = a;
    D_0048BE24[D_0048BF60.unk1D].count[i] = c;
    return 0;
}
#endif

/* 0x06: stops our timer args[0] */
s32 func_0065B050(u32 *args) {
    D_0048BE24[D_0048BF60.unk1D].on[args[0]] = 0;
    return 0;
}

/* 0x07, 0x0E: on to the next routine */
s32 func_0065B080(u32 *args) {
    D_0048BE70.entry[CUR]++;
    D_0048BE70.pc[CUR] = D_0048BE70.base[CUR][D_0048BE70.entry[CUR]];
    return 1;
}

s32 func_0065B0F0(u32 *args);
#ifdef NON_MATCHING   /* the bit test's mask is computed before the word address */
/* 0x08: if event flag args[0] is (byte) args[1] */
s32 func_0065B0F0(u32 *args) {
    u32 n = args[0];
    s8 v = args[1];

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        if (!BIT_TEST(D_0048AD90, n)) {
            func_0065AAF0();
            return 2;
        }
    } else {
        if (BIT_TEST(D_0048AD90, n)) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}
#endif

/* 0x0B: back to the loop start */
s32 func_0065B1C0(u32 *args) {
    D_0048BE70.pc[CUR] = D_0048BE70.loop[CUR];
    return 1;
}

void func_005403E0(s32 id, u8 arg);
void func_00540450(s32 id, u8 arg);

/* 0x0C: args[0] 0: func_005403E0, 1: sets/clears a scene bit, 2: func_00540450 */
s32 func_0065B200(u32 *args) {
    u32 n = args[1];
    u8 v = args[2];

    switch (args[0]) {
    case 0:
        func_005403E0(n, v);
        break;
    case 1:
        if (v == 0) {
            BIT_CLR(D_0048AC10, n);
        } else {
            BIT_SET(D_0048AC10, n);
        }
        break;
    case 2:
        func_00540450(n, v);
        break;
    }
    return 0;
}

/* 0x0D: sets/clears (args[2]) bit args[1] of D_0048ADD0 (args[0] 0) or D_0048ACD0 */
s32 func_0065B2C0(u32 *args) {
    u32 n = args[1];
    s32 v = args[2];

    if (args[0] == 0) {
        if (v == 0) {
            D_0048ADD0[n >> 5] &= ~(1 << (n & 0x1F));
        } else {
            D_0048ADD0[n >> 5] |= 1 << (n & 0x1F);
        }
    } else if (v == 0) {
        D_0048ACD0[n >> 5] &= ~(1 << (n & 0x1F));
    } else {
        D_0048ACD0[n >> 5] |= 1 << (n & 0x1F);
    }
    return 0;
}

/* 0x0E */
s32 func_0065B3B0(u32 *args) {
    D_0048BE70.entry[CUR]++;
    D_0048BE70.pc[CUR] = D_0048BE70.base[CUR][D_0048BE70.entry[CUR]];
    return 1;
}

/* 0x11 */
s32 func_0065B420(u32 *args) {
    D_003AEF50[args[0]].unk34 = 1;
    return 0;
}

/* 0x12 */
s32 func_0065B460(u32 *args) {
    D_003AEF50[args[0]].unk34 = 0;
    return 0;
}

/* 0x13 */
s32 func_0065B490(u32 *args) {
    Unk3AEF50 *p = D_003AEF50;
    s32 i;

    for (i = 0; i < p->count; i++) {
        p[i].unk34 = 1;
    }
    return 0;
}

void func_0058EB70(u8 a, u16 b);

/* 0x14 */
s32 func_0065B4E0(u32 *args) {
    func_0058EB70(args[0], args[1]);
    return 0;
}

/* 0x16 */
s32 func_0065B510(u32 *args) {
    u8 cur = D_003AEF50[0].cur;

    if (cur == *(u8 *)args) {
        D_003AEF50[0].unk6 = 0;
        D_003AEF50[cur].unk34 = 0;
    }
    return 0;
}

/* 0x15: if (D_003AEF50.unk25 != 4) is args[0] */
s32 func_0065B560(u32 *args) {
    s32 v = args[0];

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        if (D_003AEF50[0].unk25 == 4) {
            func_0065AAF0();
            return 2;
        }
    } else {
        if (D_003AEF50[0].unk25 != 4) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}

void func_0058EC30(f32 scale, s16 a, u8 b, s16 c);

s32 func_0065B5F0(u32 *args);
#ifdef NON_MATCHING   /* the args pointer is copied to a2 */
/* 0x17: args[3] is a percentage */
s32 func_0065B5F0(u32 *args) {
    func_0058EC30((f32)args[3] / 100.0f, *(s16 *)&args[1], *(u8 *)&args[0], *(s16 *)&args[2]);
    return 0;
}
#endif

/* 0x18: if (D_003AEF50.unk3 == args[0]) is args[1] */
s32 func_0065B660(u32 *args) {
    s32 a = args[0];
    s32 b = args[1];

    D_0048BE70.unk78[CUR]++;
    if ((D_003AEF50[0].unk3 == a && b == 1) || (D_003AEF50[0].unk3 != a && b == 0)) {
        return 0;
    }
    func_0065AAF0();
    return 2;
}

void func_0067FC90(u8 a);
void func_0067FCD0(u8 a);
void func_0067FE20(u8 a);

/* 0x21 */
s32 func_0065B6E0(u32 *args) {
    func_0067FC90(*(u8 *)args);
    return 0;
}

/* 0x24 */
s32 func_0065B700(u32 *args) {
    func_0067FCD0(*(u8 *)args);
    return 0;
}

/* 0x25 */
s32 func_0065B720(u32 *args) {
    func_0067FE20(*(u8 *)args);
    return 0;
}

/* 0x26 */
s32 func_0065B740(u32 *args) {
    s32 i;

    for (i = 0; i < 0x20; i++) {
        func_0067FCD0(i);
    }
    return 0;
}

void func_00562580(s16 a);
void func_00562630(s16 a);
void func_00562720(s16 a);
void func_005627B0(s16 a);
s8 func_00562840(s16 a);
void func_005628C0(s16 a);

/* 0x31 */
s32 func_0065B780(u32 *args) {
    func_00562580(*(s16 *)args);
    return 0;
}

/* 0x32 */
s32 func_0065B7A0(u32 *args) {
    func_00562630(*(s16 *)args);
    return 0;
}

/* 0x33 */
s32 func_0065B7C0(u32 *args) {
    func_00562720(*(s16 *)args);
    return 0;
}

/* 0x34 */
s32 func_0065B7E0(u32 *args) {
    func_005627B0(*(s16 *)args);
    return 0;
}

/* 0x35: if func_00562840(args[0]) */
s32 func_0065B800(u32 *args) {
    s16 a = *(s16 *)args;

    D_0048BE70.unk78[CUR]++;
    if (func_00562840(a) == 0) {
        func_0065AAF0();
        return 2;
    }
    return 0;
}

/* 0x36 */
s32 func_0065B870(u32 *args) {
    func_005628C0(*(s16 *)args);
    return 0;
}

typedef struct {
    s8 unk0;
    u8 unk1[0x24 - 1];
    s16 unk24;          /* 0x24 */
} Unk584500;

Unk584500 *func_00584500(s16 id, s16 *out);

/* 0x37: compares func_00584500(args[0])->unk24 with args[1] (args[2]: ==, !=, >=, <=, <, >) */
s32 func_0065B890(u32 *args) {
    s16 v = *(s16 *)&args[1];
    s16 mode = *(s16 *)&args[2];
    s16 tmp;
    Unk584500 *e;

    e = func_00584500(*(s16 *)&args[0], &tmp);
    if (e == NULL || e->unk0 == 0) {
        func_0065AAF0();
        return 2;
    }
    switch (mode) {
    case 0:
        if (e->unk24 != v) {
            func_0065AAF0();
            return 2;
        }
        break;
    case 1:
        if (e->unk24 == v) {
            func_0065AAF0();
            return 2;
        }
        break;
    case 2:
        if (e->unk24 < v) {
            func_0065AAF0();
            return 2;
        }
        break;
    case 3:
        if (e->unk24 > v) {
            func_0065AAF0();
            return 2;
        }
        break;
    case 4:
        if (e->unk24 <= v) {
            func_0065AAF0();
            return 2;
        }
        break;
    case 5:
        if (e->unk24 >= v) {
            func_0065AAF0();
            return 2;
        }
        break;
    }
    return 0;
}

void func_00562970(s16 a, s8 b);

s32 func_0065BA00(u32 *args);
#ifdef NON_MATCHING   /* first draft: registers/order differ */
/* 0x38 */
s32 func_0065BA00(u32 *args) {
    s16 a = *(s16 *)&args[0];

    func_00562970(a, *(s8 *)&args[1]);
    return 0;
}
#endif

typedef struct {
    u8 unk0[2];
    u8 unk2;            /* 0x02 */
} Unk5626B0;

Unk5626B0 *func_005626B0(u16 owner, u16 id, s16 arg);
extern s8 D_0023BE14;                   /* characters in the game */
extern DrawObjFull D_00476DD0[];

s32 func_0065BA30(u32 *args);
#ifdef NON_MATCHING   /* first draft: registers/order differ */
/* 0x39: puts effect args[1] on character args[0] */
s32 func_0065BA30(u32 *args) {
    s16 n = *(s16 *)&args[0];
    s16 id = *(s16 *)&args[1];
    Unk5626B0 *e;
    DrawObjFull *obj;
    s32 i;

    if (n < 0 || id < 0) {
        return 0;
    }
    e = func_005626B0(n, id, *(s16 *)&args[2]);
    if (e == NULL) {
        return 0;
    }
    if (n >= D_0023BE14) {
        return 0;
    }
    obj = &D_00476DD0[n];
    if (obj->active == 0) {
        return 0;
    }
    if (id >= 400 && id < 421) {
        for (i = 0; i < 0x20; i++) {
            if (obj->unkC88[i] == 0) {
                obj->unkC88[i] = e->unk2;
                break;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (obj->unkC84[i] == 0) {
                obj->unkC84[i] = e->unk2;
                break;
            }
        }
    }
    return 0;
}
#endif

s32 func_0065BB60(u32 *args);
#ifdef NON_MATCHING   /* first draft: registers/order differ */
/* 0x3A: puts effect args[1] on the character of kind args[0] */
s32 func_0065BB60(u32 *args) {
    s8 kind = *(s8 *)&args[0];
    s16 arg = *(s16 *)&args[2];
    s16 id = *(s16 *)&args[1];
    Unk5626B0 *e;
    DrawObjFull *obj;
    s32 n;
    s32 i;

    if (kind < 0 || id < 0) {
        return 0;
    }
    obj = D_00476DD0;
    for (n = 0; n < D_0023BE14; n++, obj++) {
        if (obj->active == 0 || obj->unkB98 != kind) {
            continue;
        }
        e = func_005626B0(n, id, arg);
        if (e == NULL) {
            return 0;
        }
        if (id >= 400 && id < 421) {
            for (i = 0; i < 0x20; i++) {
                if (obj->unkC88[i] == 0) {
                    obj->unkC88[i] = e->unk2;
                    return 0;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (obj->unkC84[i] == 0) {
                    obj->unkC84[i] = e->unk2;
                    return 0;
                }
            }
        }
    }
    return 0;
}
#endif

typedef struct {
    u8 unk0;
    u8 unk1[3];
    u16 unk4;           /* 0x04 */
    u8 unk6[0x4C - 6];
    u8 unk4C[4];        /* 0x4C */
    u8 unk50[0x78 - 0x50];
} Unk3614F0;

extern Unk3614F0 D_003614F0[0x40];

s32 func_0065BCB0(u32 *args);
#ifdef NON_MATCHING   /* first draft: registers/order differ */
/* 0x3B: puts effect args[1] on enemy args[0] */
s32 func_0065BCB0(u32 *args) {
    s16 n = *(s16 *)&args[0];
    s16 arg = *(s16 *)&args[2];
    s16 id = *(s16 *)&args[1];
    Unk5626B0 *e;
    Unk3614F0 *q;
    s32 k;
    s32 i;

    if (n < 0 || id < 0) {
        return 0;
    }
    e = func_005626B0(n + 4, id, arg);
    if (e == NULL) {
        return 0;
    }
    q = D_003614F0;
    for (k = 0; k < 0x40; k++, q++) {
        if (q->unk0 != 0 && q->unk4 == n) {
            for (i = 0; i < 4; i++) {
                if (q->unk4C[i] == 0) {
                    q->unk4C[i] = e->unk2;
                    return 0;
                }
            }
        }
    }
    return 0;
}
#endif
