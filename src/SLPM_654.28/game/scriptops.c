#include "common.h"
#include "options.h"
#include "game.h"
#include "script.h"
#include "object.h"
#include "item.h"

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
    u8 unk8[0x1C - 0x8];
    s32 unk1C;          /* 0x1C: (message actions) */
    s32 unk20;          /* 0x20 */
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

ItemSlot *func_005626B0(u16 owner, u16 id, s16 arg);
extern s8 D_0023BE14;                   /* characters in the game */
extern DrawObjFull D_00476DD0[];

s32 func_0065BA30(u32 *args);
#ifdef NON_MATCHING   /* first draft: registers/order differ */
/* 0x39: puts effect args[1] on character args[0] */
s32 func_0065BA30(u32 *args) {
    s16 n = *(s16 *)&args[0];
    s16 id = *(s16 *)&args[1];
    ItemSlot *e;
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
                obj->unkC88[i] = e->index;
                break;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            if (obj->unkC84[i] == 0) {
                obj->unkC84[i] = e->index;
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
    ItemSlot *e;
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
                    obj->unkC88[i] = e->index;
                    return 0;
                }
            }
        } else {
            for (i = 0; i < 4; i++) {
                if (obj->unkC84[i] == 0) {
                    obj->unkC84[i] = e->index;
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
    ItemSlot *e;
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
                    q->unk4C[i] = e->index;
                    return 0;
                }
            }
        }
    }
    return 0;
}
#endif

#include "message.h"

extern s8 D_0048BF50[];
extern u32 D_006E3510[];        /* the message when our character can't talk */
void func_00635780(s32 a);

/* Queue an event message. */
void func_005BF4C0(EventMsg *m) {
    D_003839B0[D_0023BD78].v[0] = m->v[0];
    D_003839B0[D_0023BD78].v[1] = m->v[1];
    D_003839B0[D_0023BD78].v[2] = m->v[2];
    D_003839B0[D_0023BD78].v[3] = m->v[3];
    D_003839B0[D_0023BD78].v[4] = m->v[4];
    D_003839B0[D_0023BD78].v[5] = m->v[5];
    D_003839B0[D_0023BD78].v[6] = m->v[6];
    D_003839B0[D_0023BD78].v[7] = m->v[7];
    D_0023BD78++;
    D_0023BD78 &= 0xF;
}

/* 0x62: a message (args[0..4]); a stock one when our character is in no state to (status 0x80) */
s32 func_0065D090(u32 *args) {
    EventMsg m;

    if (D_00476DD0[D_0048BF60.unk1D].unkBA0 & 0x80) {
        if (D_0048BF60.unk28 == 0) {
            args = D_006E3510;
            D_0048BF60.unkA7 = 1;
        } else {
            return 0;
        }
    }
    D_0048BF60.unk28 = 1;
    if (D_00476DD0[D_0048BF60.unk1D].unkBA0 & 0x120) {
        D_0048BF60.unkA7 = 1;
    } else {
        D_0048BF60.unk28 = 1;
        m.v[0] = args[0];
        m.v[1] = args[1];
        m.v[2] = args[2];
        m.v[3] = args[3];
        m.v[4] = args[4];
        m.v[7] = 0;
        m.v[6] = 0;
        m.v[5] = 0;
        func_005BF4C0(&m);
    }
    if (D_0048BF60.unkA7 != 0) {
        func_00635780(0);
    }
    D_0048BF50[0] = 0;
    return 0;
}

/* if character kind args[0] is (args[1] 1) or isn't (0) one of the players */
s32 func_0065F770(u32 *args) {
    u8 kind = *(u8 *)&args[0];
    u32 i;
    u8 v = *(u8 *)&args[1];
    u8 found = 0;

    D_0048BE70.unk78[CUR]++;
    for (i = 0; i < *(u32 *)&D_0023BE14; i++) {
        if (kind == D_00476DD0[i].unkB98) {
            found = 1;
            break;
        }
    }
    if ((v == 1 && found == 0) || (v == 0 && found == 1)) {
        func_0065AAF0();
        return 2;
    }
    return 0;
}

void func_0065ACF0(void);

/* 0xE4: else (reached from the true branch: skip to the endif) */
s32 func_0065F2A0(u32 *args) {
    func_0065ACF0();
    return 2;
}

/* if our character is (args[1] != 0) or isn't kind args[0] */
s32 func_00661850(u32 *args) {
    u8 kind = *(u8 *)&args[0];
    s32 v = args[1];

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        if (kind != D_00476DD0[D_0048BF60.unk1D].unkB98) {
            func_0065AAF0();
            return 2;
        }
    } else {
        if (kind == D_00476DD0[D_0048BF60.unk1D].unkB98) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}

/* 0xE5: an if that's always true */
s32 func_0065F2C0(u32 *args) {
    D_0048BE70.unk78[CUR]++;
    return 0;
}

/* if bit args[0] of D_0048ADD0 is args[1] */
s32 func_0065F2F0(u32 *args) {
    u32 n = args[0];
    s32 v = args[1];
    u32 *w;

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        w = &D_0048ADD0[n >> 5];
        if (!(*w & (1 << (n & 0x1F)))) {
            func_0065AAF0();
            return 2;
        }
    } else {
        w = &D_0048ADD0[n >> 5];
        if (*w & (1 << (n & 0x1F))) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}

/* if event flag args[0] is set and its D_0048ADD0 bit clear, is args[1] */
s32 func_0065F910(u32 *args);
#ifdef NON_MATCHING   /* and operand order in the flag tests */
s32 func_0065F910(u32 *args) {
    u32 n = args[0];
    s32 v = args[1];

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        if ((D_0048AD90[n >> 5] & (1 << (n & 0x1F)))) {
            if ((D_0048ADD0[n >> 5] & (1 << (n & 0x1F)))) {
                func_0065AAF0();
                return 2;
            }
        } else {
            func_0065AAF0();
            return 2;
        }
    } else {
        if ((D_0048AD90[n >> 5] & (1 << (n & 0x1F))) && !(D_0048ADD0[n >> 5] & (1 << (n & 0x1F)))) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}
#endif

/* if D_0048BF60.unkBA compares with (byte) args[0] by (byte) args[1]: ==, !=, >=, <=, >, < */
s32 func_0065FB30(u32 *args) {
    u8 v = *(u8 *)&args[0];
    u8 op = *(u8 *)&args[1];

    D_0048BE70.unk78[CUR]++;
    switch (op) {
    case 0:
        if (D_0048BF60.unkBA == v) {
            return 0;
        }
        break;
    case 1:
        if (D_0048BF60.unkBA != v) {
            return 0;
        }
        break;
    case 2:
        if (D_0048BF60.unkBA >= v) {
            return 0;
        }
        break;
    case 3:
        if (D_0048BF60.unkBA <= v) {
            return 0;
        }
        break;
    case 4:
        if (D_0048BF60.unkBA > v) {
            return 0;
        }
        break;
    case 5:
        if (D_0048BF60.unkBA < v) {
            return 0;
        }
        break;
    }
    func_0065AAF0();
    return 2;
}

extern u8 D_0048BBE0[];        /* per item: who has it (player + 1) */
void func_00341B50(void);
void func_00341AB0(void);
void func_006ABBA0(void);
void func_006ABD70(u8 a, u8 b, s32 c, s32 d);

/* Who says a message of this kind: 2 our character, 3 the item's holder (or us), 1 and 5 whether
   the item's holder (unkB99) ... */
#ifdef NON_MATCHING   /* the item holder's number is masked late */
u8 func_005BF590(s32 kind) {
    u8 n;
    u8 r = 0;

    switch (kind) {
    case 1:
    case 5:
        if (D_0035FFD0.item < 0) {
            return 0;
        }
        n = D_0048BBE0[D_0035FFD0.item];
        if (n == 0) {
            return 0;
        }
        if (*(u8 *)&(D_00476DD0 - 1)[n].unkB99 != 0) {
            r = 1;
        } else {
            return 0;
        }
        break;
    case 2:
        r = D_00476DD0[D_0048BF60.unk1D].unkB98;
        break;
    case 3:
        n = D_0048BBE0[D_0035FFD0.item];
        if (n == 0) {
            r = D_00476DD0[D_0048BF60.unk1D].unkB98;
        } else {
            r = (D_00476DD0 - 1)[n].unkB98;
        }
        break;
    }
    return r;
}
#endif

/* Show the next queued message, with the ones of the same exchange after it. */
void func_005BF6E0(s16 unused);
#ifdef NON_MATCHING   /* keeps registers across func_005BF590, which has to match first */
void func_005BF6E0(s16 unused) {
    s16 r = D_0023BD7C;
    s32 i;
    s16 k;

    D_0035FFD0.kind = D_003839B0[r].v[0];
    D_0035FFD0.a = D_003839B0[r].v[1];
    D_0035FFD0.b = D_003839B0[r].v[2];
    D_0035FFD0.who = D_003839B0[r].v[3];
    D_0035FFD0.who2 = D_0035FFD0.who;
    D_0035FFD0.item = D_003839B0[r].v[4];
    D_0023BD7C = r + 1;
    D_0023BD7C &= 0xF;
    D_0035FFD0.who = func_005BF590(D_0035FFD0.who);
    if (D_0035FFD0.kind == 3) {
        D_0035FFD0.kind = 1;
    }
    D_0048BF60.unk3B = 0;
    if (D_0035FFD0.kind != 0) {
        D_0048BF60.unk39 = 1;
        D_0048BF60.unk3B++;
        D_0048BF60.msgs[0][0] = D_0035FFD0.a;
        D_0048BF60.msgs[0][1] = D_0035FFD0.b;
        D_0048BF60.msgs[0][2] = D_0035FFD0.who;
        k = D_0023BD7C;
        for (i = 0; i < 0x10; i++) {
            if (k == D_0023BD78) {
                break;
            }
            if (D_003839B0[k].v[0] == 0) {
                break;
            }
            D_0048BF60.msgs[D_0048BF60.unk3B][0] = D_003839B0[k].v[1];
            D_0048BF60.msgs[D_0048BF60.unk3B][1] = D_003839B0[k].v[2];
            D_0048BF60.msgs[D_0048BF60.unk3B][2] = func_005BF590(D_003839B0[k].v[3]);
            k = (k + 1) & 0xF;
            D_0048BF60.unk3B++;
        }
        D_0023BD7C = k;
    }
    if (D_0035FFD0.kind == 3 || D_0035FFD0.kind == 1) {
        func_00341B50();
        func_006ABBA0();
    } else {
        func_00341AB0();
        func_006ABD70(D_0035FFD0.a, D_0035FFD0.b, 0, 0);
    }
}
#endif

void *memset(void *p, s32 c, u32 n);
s32 func_005BCEA0(s32 mode);
void func_001BA0F0(u32 color);
s32 func_001B9C30(s16 frames, u32 color);

/* After a message: its work cleared, func_005BCEA0(1), the fade reset. */
void func_005BDC20(void) {
    D_0048BF60.unk3A = 0;
    D_0048BF60.unk1E[0] = 0;
    memset(&D_0035FFD0, 0, sizeof(D_0035FFD0));
    func_005BCEA0(1);
    func_001BA0F0(0xFF000000);
    func_001B9C30(1, 0xFF000000);
}

/* A message file's tables, found by func_005BD4C0. */
typedef struct {
    s32 unk0;           /* the file's word 0x20 */
    s32 n1;             /* 0x04: entries in t1 (0x18 bytes each) */
    s32 n2;             /* 0x08: in t2 (0x28 bytes) */
    s32 n3;             /* 0x0C: in t3 (0x28 bytes) */
    u8 *t1;             /* 0x10 */
    u8 *t2;             /* 0x14 */
    u8 *t3;             /* 0x18 */
    s32 unk1C;
} MsgFile;

/* past a table: a count, then count entries of size bytes (rounded down to words) */
#define MSG_SKIP(p, size)                \
    {                                    \
        c = *(s32 *)(p);                 \
        (p) += 4;                        \
        (p) += (c * (size)) & ~3;        \
    }

/* Find the tables in a message file. */
s32 func_005BD4C0(u8 *file, MsgFile *f) {
    s32 n;
    s32 i;
    s32 j;
    s32 c;

    f->unk0 = *(s32 *)(file + 0x20);
    n = *(s32 *)(file + 0x24);
    file += 0x28;
    file += (n * 16) & ~3;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < 2; j++) {
            MSG_SKIP(file, 0x38);
        }
    }
    n = *(s32 *)file;
    f->n1 = n;
    file += 4;
    f->t1 = file;
    file += (n * 0x18) & ~3;
    MSG_SKIP(file, 0x24);
    n = *(s32 *)file;
    file += 4;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 8; j++) {
            MSG_SKIP(file, 0x24);
        }
    }
    n = *(s32 *)file;
    file += 4;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 7; j++) {
            MSG_SKIP(file, 0x24);
        }
    }
    n = *(s32 *)file;
    f->n2 = n;
    file += 4;
    f->t2 = file;
    file += (n * 0x28) & ~3;
    n = *(s32 *)file;
    file += 4;
    for (i = 0; i < n; i++) {
        for (j = 0; j < 2; j++) {
            MSG_SKIP(file, 0x30);
        }
    }
    f->n3 = *(s32 *)file;
    f->t3 = file + 4;
    return 0;
}

extern char D_0070A1E0[];       /* "data/edit/eve/r%03d/dm%02d%02d%c.bin" */
extern char D_0070A210[];       /* "dm%02d%02d%c.bin" */
extern s32 D_0023BD40;
extern u8 *D_0023BD50;          /* the loaded messages' data */
extern s32 D_0023BD54;
extern s32 D_0023BD58;
extern s32 D_0023BD5C;
extern s32 D_0023BD60;
extern s32 D_0023BD64;
extern s32 D_0023BD70;
s32 sprintf(char *s, const char *fmt, ...);
s32 func_0018D530(const char *name);
void func_0018D630(void *dst, const void *src, s32 n);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 size);
s32 func_001A96E0(const char *name, void *buf);
s32 func_005BD4C0(u8 *file, MsgFile *out);
s32 func_005BD280(MsgFile *src, MsgFile *acc);
void func_001D7260(s32 mode);

/* Append a message file's tables (src) to the ones gathered so far (acc): a table's closing
   entry (0xFFFF) is dropped first, and the new entries' indices (field 2) are moved past acc's. */
s32 func_005BD280(MsgFile *src, MsgFile *acc) {
    s32 base = acc->unk0;
    u8 *q;
    s32 i;
    s32 j;
    u8 *p;

    acc->unk0 = base + src->unk0;
    p = acc->t1 + acc->n1 * 0x18;
    if (acc->n1 != 0 && *(u16 *)(p - 0x16) == 0xFFFF) {
        p -= 0x18;
        acc->n1--;
    }
    for (i = 0; i < src->n1; i++, p += 0x18) {
        func_0018D630(p, src->t1 + i * 0x18, 0x18);
        if (*(u16 *)(p + 2) != 0xFFFF) {
            *(u16 *)(p + 2) += base;
        }
    }
    acc->n1 += src->n1;
    q = acc->t2 + acc->n2 * 0x28;
    if (acc->n2 != 0 && *(u16 *)(q - 0x26) == 0xFFFF) {
        q -= 0x28;
        acc->n2--;
    }
    for (j = 0; j < src->n2; j++, q += 0x28) {
        func_0018D630(q, src->t2 + j * 0x28, 0x28);
        if (*(u16 *)(q + 2) != 0xFFFF) {
            *(u16 *)(q + 2) += base;
        }
    }
    acc->n2 += src->n2;
    q = acc->t3 + acc->n3 * 0x28;
    if (acc->n3 != 0 && *(u16 *)(q - 0x26) == 0xFFFF) {
        q -= 0x28;
        acc->n3--;
    }
    for (j = 0; j < src->n3; j++, q += 0x28) {
        func_0018D630(q, src->t3 + j * 0x28, 0x28);
        if (*(u16 *)(q + 2) != 0xFFFF) {
            *(u16 *)(q + 2) += base;
        }
    }
    acc->n3 += src->n3;
    return 0;
}

/* Load the messages being shown (D_0048BF60.msgs: file dm<text><text><speaker>.bin each) and
   merge their tables in work memory. mode 0 uses unk4C/unk68's memory, 1 unk6C's. */
#ifdef NON_MATCHING   /* register allocation: the original keeps &acc.n1 in fp */
s32 func_005BCEA0(s32 mode) {
    char name[0x80];
    MsgFile f;
    MsgFile acc;
    u8 *buf;
    u8 *p;
    u8 *q;
    s32 i;

    acc.unk0 = 0;
    acc.n1 = 0;
    acc.n2 = 0;
    acc.n3 = 0;
    if (mode == 0) {
        p = (u8 *)D_00306590.unk4C;
        acc.t1 = p;
        acc.t2 = p + 0x4000;
        acc.t3 = p + 0x8000;
        memset(p, 0, 0x3000);
    } else {
        q = D_00306590.unk6C;
        acc.t1 = p = q + 0x18000;
        acc.t2 = q + 0x19000;
        acc.t3 = q + 0x1A000;
        memset(p, 0, 0x3000);
    }
    for (i = 0; i < D_0048BF60.unk3B; i++) {
        if (mode != 0) {
            buf = D_00306590.unk6C;
        } else {
            buf = (u8 *)D_00306590.unk4C + 0x40000;
        }
        memset(name, 0, sizeof(name));
        if (D_00306590.unk17 == 0) {
            sprintf(name, D_0070A1E0, D_00306590.unk1A, D_0048BF60.msgs[i][0], D_0048BF60.msgs[i][1],
                    D_0048BF60.msgs[i][2] + 'a');
            if (func_001CFBB0(name, buf, 1, func_0018D530(name)) == 0) {
                continue;
            }
        } else {
            sprintf(name, D_0070A210, D_0048BF60.msgs[i][0], D_0048BF60.msgs[i][1], D_0048BF60.msgs[i][2] + 'a');
            if (func_001A96E0(name, buf) == 0) {
                continue;
            }
        }
        if (func_005BD4C0(buf, &f) == 0) {
            func_005BD280(&f, &acc);
        }
    }
    D_0023BD70 = 0;
    D_0035FFD0.unk14 = 0;
    D_0023BD64 = 0;
    D_0035FFD0.unk16 = 0;
    D_0023BD60 = 0;
    D_0035FFD0.unk18 = 0;
    D_0023BD5C = 0;
    D_0035FFD0.unk1C = 0;
    D_0023BD58 = 0;
    D_0035FFD0.unk20 = 0;
    D_0023BD54 = 0;
    D_0035FFD0.unk22 = 0;
    D_0023BD40 = 0;
    D_0035FFD0.unk2E = 0;
    D_0035FFD0.n2 = acc.n2;
    D_0035FFD0.n3 = acc.n3;
    D_0035FFD0.count = acc.unk0;
    if (mode == 0) {
        D_0023BD50 = p = D_00306590.unk68 + 0x40000;
        memset(p, 0, 0x18000);
    } else {
        D_0023BD50 = p = D_00306590.unk6C;
        memset(p, 0, 0x18000);
    }
    func_0018D630(p, acc.t1, acc.n1 * 0x18);
    if (acc.n1 != 0) {
        D_0023BD74 = (MsgCue *)p;
    } else {
        D_0023BD74 = NULL;
    }
    D_0023BD6C = (MsgAction *)(q = (u8 *)(((u32)p + acc.n1 * 0x60 + 3) & ~3));
    func_0018D630(q, acc.t2, acc.n2 * 0x28);
    p = (u8 *)(((u32)q + acc.n2 * 0xA0 + 3) & ~3);
    func_0018D630(p, acc.t3, acc.n3 * 0x28);
    if (acc.n3 != 0) {
        D_0023BD68 = (MsgCall *)p;
    } else {
        D_0023BD68 = NULL;
    }
    func_001D7260(mode);
    return 0;
}
#endif

void func_005BF080(void);
void func_005BF2A0(void);
void func_005BF200(void);
void func_001BA180(void);
void func_005BF480(void);

/* Once a frame while a message is up. */
void func_005BDC80(void) {
    func_005BF080();
    func_005BF2A0();
    func_005BF200();
    func_001BA180();
    func_005BF480();
    if (D_00306590.unkC != 0) {
        D_0035FFD0.frames += 2;
    } else {
        D_0035FFD0.frames += 1;
    }
}

/* The cue's effect, from the room's dm file. */
typedef struct {
    s32 unk0;
    s32 index;          /* 0x04 */
    s16 dur;            /* 0x08 */
    s16 unkA;
    s32 unkC;
    s32 *data;          /* 0x10: offsets in the dm file */
} MsgFx;

extern MsgFx D_0070E530;
extern u8 *D_0023A950[2];
void func_001B3720(s32 a, s32 b, u8 *c, s16 d, s32 e, u8 *f);

/* Start the current cue's effect when its time comes. */
#ifdef NON_MATCHING   /* register allocation */
void func_005BF080(void) {
    MsgCue *e = D_0023BD74;
    u16 t;
    u16 now;
    u8 *base;
    s32 *tab;
    u8 **pp;
    s32 n;
    s32 i;

    if (e == NULL) {
        return;
    }
    now = D_0035FFD0.frames;
    t = e->time;
    if (D_00306590.unkC != 0) {
        t >>= 1;
        now >>= 1;
    }
    if (t == now) {
        base = D_00306590.unk6C + 0x10000;
        tab = (s32 *)(base + 4);
        memset(&D_0070E530, 0, sizeof(D_0070E530));
        if (*tab != 0) {
            D_0070E530.dur = e->dur / 2;
            D_0070E530.index = e->fx;
            D_0070E530.data = (s32 *)(base + tab[D_0070E530.index * 2]);
            n = tab[D_0070E530.index * 2 + 1];
            pp = &D_0023A950[n - 1];
            for (i = 0; i < n; i++) {
                func_001B3720(0, 0, *pp + i * 0x16, D_0070E530.dur, 0x18, base + D_0070E530.data[i]);
            }
        }
        e++;
        if (e->time == 0xFFFF) {
            e = NULL;
        }
    }
    D_0023BD74 = e;
}
#endif

extern u8 D_00470090[];
void func_001BA160(MsgAction *e);

/* Do the actions whose time has come. */
void func_005BF2A0(void) {
    MsgAction *e = D_0023BD6C;
    u16 t;
    u16 now;
    s32 i;
    s32 j;
    s32 k;
    u16 bits;

    if (e == NULL) {
        return;
    }
    for (i = 0; i < D_0035FFD0.n2; i++, e++) {
        now = D_0035FFD0.frames;
        t = e->time;
        if (D_00306590.unkC != 0) {
            t >>= 1;
            now >>= 1;
        }
        if (t == now) {
            if (e->kind == 0) {
                e->time = 0xFFFF;
            } else if (e->kind == 1) {
                for (j = 0; j < 8; j++) {
                    bits = e->bits[j];
                    for (k = 0; k < 0x10; k++) {
                        D_00470090[0x8A0 + j * 0x10 + k] = (bits & (1 << k)) != 0;
                    }
                }
                D_003AEF50[0].unk1C = e->a[0];
                D_003AEF50[0].unk1C |= e->a[1] << 16;
                D_003AEF50[0].unk20 = e->b[0];
                D_003AEF50[0].unk20 |= e->b[1] << 16;
                e->time = 0xFFFF;
            } else if (e->kind == 2) {
                func_001BA160(e);
            }
        }
    }
}

void func_001B9060(s32 arg);

/* Start the vibrations whose time has come. */
void func_005BF200(void) {
    MsgCall *e = D_0023BD68;
    u16 t;
    u16 now;
    s32 i;

    if (e == NULL) {
        return;
    }
    for (i = 0; i < D_0035FFD0.n3; i++, e++) {
        now = D_0035FFD0.frames;
        t = e->time;
        if (D_00306590.unkC != 0) {
            t >>= 1;
            now >>= 1;
        }
        if (t == now) {
            func_001B9060(e->arg);
            e->time = 0xFFFF;
        }
    }
}

/* Count the cue effect down. */
void func_005BF480(void) {
    MsgFx *fx = &D_0070E530;
    s16 d;

    if (D_0070E530.dur != 0) {
        d = fx->dur - 1;
        fx->dur = d;
        if (d <= 0) {
            fx->dur = 0;
            fx->data = NULL;
        }
    }
}

/* Room table entry [args[0]][args[1]] = the difficulty's value args[2] (< 100), or the script's
   own table's (100 and up). */
typedef struct {
    u16 v[0x10];
} DiffVals;

extern DiffVals D_006E36D0[];
extern u16 D_00472F90[0x80][0x18];
s32 func_0065FD70(s32 *args) {
    s32 a = args[0];
    s32 b = args[1];
    s32 v = args[2];

    if (v < 100) {
        D_00472F90[a][b] = D_006E36D0[D_0048BF60.unkBA].v[v & 0xF];
    } else {
        D_00472F90[a][b] =
            ((s32 *)(*(s32 *)(D_00306590.script + 0x34) + (D_0048BF60.unkBA << 6)))[(v - 100) & 0xF];
    }
    return 0;
}

/* Sets bit args[1] of entry args[0]'s flags. */
extern u16 D_0048AFB0[][6];

s32 func_0065FEC0(s32 *args);
#ifdef NON_MATCHING   /* the constant 1's register */
s32 func_0065FEC0(s32 *args) {
    D_0048AFB0[args[0]][5] |= 1 << args[1];
    return 0;
}
#endif

void func_00540620(s32 row, s32 bit, s32 col);
void func_00540670(s32 row, s32 bit, s32 col);
void func_005406B0(s32 row, s32 bit, s32 col);
void func_00540700(s32 row, s32 bit, s32 col);

/* Flag word bits (D_0048AFB0, column 0): set, clear, and set/clear telling the others. */
s32 func_0065C220(s32 *args);
#ifdef NON_MATCHING   /* args[1] is read through args + 1 first */
s32 func_0065C220(s32 *args) {
    func_00540670(args[0], args[1], 0);
    return 0;
}
#endif

s32 func_0065C250(s32 *args);
#ifdef NON_MATCHING   /* args[1] is read through args + 1 first */
s32 func_0065C250(s32 *args) {
    func_00540620(args[0], args[1], 0);
    return 0;
}
#endif

s32 func_0065C280(s32 *args);
#ifdef NON_MATCHING   /* args[1] is read through args + 1 first */
s32 func_0065C280(s32 *args) {
    func_005406B0(args[0], args[1], 0);
    return 0;
}
#endif

s32 func_0065C2B0(s32 *args);
#ifdef NON_MATCHING   /* args[1] is read through args + 1 first */
s32 func_0065C2B0(s32 *args) {
    func_00540700(args[0], args[1], 0);
    return 0;
}
#endif

extern u16 D_0048AFB0[][6];

/* if flag word args[0]'s bit args[1] (column 0) is args[2] */
s32 func_0065EA10(u32 *args);
#ifdef NON_MATCHING   /* the bit test's and takes its operands the other way round */
s32 func_0065EA10(u32 *args) {
    s32 row = args[0];
    s32 bit = args[1];
    s32 v = args[2];

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        if (!((1 << bit) & D_0048AFB0[row][0])) {
            func_0065AAF0();
            return 2;
        }
    } else {
        if ((1 << bit) & D_0048AFB0[row][0]) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}
#endif

extern u16 D_0048BF28[8];
extern s16 D_0048BF18[8];

/* Script op: entry args[2] (of 8) of a table: args[1] (| 0x8000 with args[0]) and args[3]. */
s32 func_0065C300(s32 *args) {
    s32 f = args[0];
    s32 a = args[1];
    u32 i = args[2];
    s32 w = args[3];
    u16 v;

    if (i >= 8) {
        return 0;
    }
    v = a;
    if (f != 0) {
        v |= 0x8000;
    }
    D_0048BF28[i] = v;
    D_0048BF18[i] = w;
    return 0;
}

/* For the players playing character args[0]: their 3-byte state in the scene (0x280) = {0,
   args[1], args[2]}; when that's us, a flag for the script. */
s32 func_0065D270(u32 *args) {
    u8 kind;
    u8 me;
    s32 i;
    u8 b;
    u8 c;
    DrawObjFull *p;
    u8 *s;

    kind = args[0];
    b = args[1];
    c = args[2];
    me = D_0048BF60.unk1D;

    for (i = 0; i < 4; i++) {
        p = &D_00476DD0[i];
        if (kind == p->unkB98) {
            s = (u8 *)D_0048AC10 + i * 3;
            s[0x280] = 0;
            s[0x281] = b;
            s[0x282] = c;
            if (me == p->kind) {
                D_0048BE70.unkE0 = 1;
            }
        }
    }
    return 0;
}

void func_005C1AC0(DrawObjFull *p, u8 slot);

/* Gives the player playing character 7 (if she has item 0x19D) args[1] of item args[0]
   (0x12C-0x12E), up to 3 in all. */
s32 func_00662BB0(u32 *args);
#ifdef NON_MATCHING   /* first draft: registers */
s32 func_00662BB0(u32 *args) {
    s32 n = 0;
    u8 has = 0;
    s32 count = args[1];
    u16 id = args[0];
    s32 who = -1;
    DrawObjFull *p;
    s32 i;
    s32 k;
    ItemSlot *e;
    u16 it;
    u8 slot;

    if (count == 0) {
        return 0;
    }
    if (id < 0x12C || id >= 0x12F) {
        return 0;
    }
    for (i = 0, p = D_00476DD0; i < *(s32 *)&D_0023BE14; i++, p++) {
        if (p->active != 0 && p->unkB98 == 7) {
            who = i;
            for (k = 0; k < 0x20; k++) {
                if ((slot = p->unkC88[k]) != 0) {
                    it = D_003967D0[slot].unk6;
                    if (it == 0x19D) {
                        has = 1;
                    } else if (id == it) {
                        n++;
                    }
                }
            }
            break;
        }
    }
    if (who < 0 || has == 0 || n >= 3) {
        return 0;
    }
    if (count + n >= 4) {
        count = 3 - n;
    }
    for (k = 0; k < count; k++) {
        e = func_005626B0(who, id, 1);
        if (e == NULL) {
            return 0;
        }
        func_005C1AC0(p, e->index);
    }
    return 0;
}
#endif

/* Adds (args[1], args[2]) to room args[0]'s 4 pairs (in the first free one). */
typedef struct {
    u16 a;
    u16 b;
} RoomPair;

extern RoomPair D_0048A010[][4];

s32 func_0065DE30(u32 *args);
#ifdef NON_MATCHING   /* registers, the room offset recomputed */
s32 func_0065DE30(u32 *args) {
    s32 room = args[0];
    u16 a = args[1];
    u16 b = args[2];
    s32 i;

    for (i = 0; i < 4; i++) {
        if (D_0048A010[room][i].a == 0xFFFF && D_0048A010[room][i].b == 0xFFFF) {
            D_0048A010[room][i].a = a;
            D_0048A010[room][i].b = b;
            break;
        }
    }
    return 0;
}
#endif

/* NPCs by model id: ids from 200 are the room's (D_0048BE80[id] + 200). */
extern u8 D_0048BE80[];
void func_00652820(u32 id);
void func_00652540(u16 id, s32 a, f32 *pos, u16 b);
void func_00652610(u16 id, s32 a, s32 flags);

s32 func_0065D480(u32 *args) {
    D_0048BF60.unk60 = 1;
    return 0;
}

s32 func_0065DA70(u32 *args) {
    u32 id = args[0];

    if (id >= 200) {
        id = D_0048BE80[id] + 200;
    }
    func_00652820(id);
    return 0;
}

/* A sound at (args[2..4]). */
s32 func_0065DAB0(u32 *args) {
    s32 id = *(u16 *)args;
    f32 pos[3];
    s32 a;

    if (id >= 200) {
        id = (u16)(D_0048BE80[id] + 200);
    }
    a = args[1];
    pos[0] = (s32)args[2];
    pos[1] = (s32)args[3];
    pos[2] = (s32)args[4];
    func_00652540(id, a, pos, args[5]);
    return 0;
}

s32 func_0065DB30(u16 *args);
#ifdef NON_MATCHING   /* args[1]'s address is made in both paths */
s32 func_0065DB30(u16 *args) {
    s32 id = args[0];

    if (id >= 200) {
        id = (u16)(D_0048BE80[id] + 200);
    }
    func_00652610(id, *(s32 *)(args + 2), 0x40000000);
    return 0;
}
#endif

s32 func_0065DB90(u32 *args) {
    s32 id = *(u16 *)args;

    if (id >= 200) {
        id = (u16)(D_0048BE80[id] + 200);
    }
    func_00652610(id, -1, 0x40000008);
    return 0;
}

s32 func_0065DBF0(u32 *args) {
    s32 id = *(u16 *)args;

    if (id >= 200) {
        id = (u16)(D_0048BE80[id] + 200);
    }
    func_00652610(id, -1, (args[1] != 0 ? 0x80000000 : 0x40000000) | 0x10);
    return 0;
}

extern s8 D_0048AF30[];

s32 func_0065DC50(u32 *args) {
    D_0048AF30[args[0]] = args[1];
    return 0;
}

/* Script variable args[0] (0-3, at 0xC8) = args[1] */
s32 func_00660740(u32 *args) {
    s32 i = args[0];

    if ((u32)i > 3) {
        return 0;
    }
    (&D_0048BE70.unkC8)[i] = args[1];
    return 0;
}

s32 func_006609A0(u32 *args) {
    s32 i;

    for (i = 3; i >= 0; i--) {
        (&D_0048BE70.unkC8)[i] = 0;
    }
    return 0;
}

void func_00687E60(s32 a, s32 on);

s32 func_006609D0(u32 *args) {
    func_00687E60(args[0], 1);
    return 0;
}

s32 func_00660A00(u32 *args) {
    func_00687E60(args[0], 0);
    return 0;
}

/* The scene's bytes 0x294-0x295 (in D_0048AC10). */
typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2[0xE];
} Scene294;

extern Scene294 D_0048AEA4;

/* Byte 0x295 = args[0]; when it changes, byte 0x294 = 0xFF. */
s32 func_00660A30(u32 *args) {
    if (D_0048AEA4.unk1 != args[0]) {
        D_0048AEA4.unk1 = args[0];
        D_0048AEA4.unk0 = 0xFF;
    }
    return 0;
}

#include "zone.h"

void func_00593FA0(Zone *z);

/* The kind-5 zones with id args[0]: armed (unk3C), with args[1] for its slot args[2] & 7. */
s32 func_00660A70(u32 *args);
#ifdef NON_MATCHING   /* the loop's shape */
s32 func_00660A70(u32 *args) {
    Zone *z;
    s32 id;
    s32 slot;
    u16 v;

    v = args[1];
    slot = args[2];
    id = args[0];
    if (D_0023BD88 == 0x80) {
        return 0;
    }
    z = D_0023BD90;
    while (z != NULL) {
        if (z->kind == 5 && z->u.id == id) {
            func_00593FA0(z);
            slot &= 7;
            z->on = 1;
            z->unk50 = v;
            z->unk59 |= 1 << slot;
            z->unk5A[slot] = v;
        }
        z = z->next;
    }
    return 0;
}
#endif

s32 func_00652370(s32 model, s32 x, s32 y, s32 z);

/* The NPC of model args[0] placed at (args[1..3]). */
s32 func_0065D990(u32 *args) {
    u32 id = args[0];

    if (id >= 200) {
        id = D_0048BE80[id] + 200;
    }
    func_00652370(id, args[1], args[2], args[3]);
    return 0;
}

extern u32 D_0048BBC0[];

/* Enemy group args[0] gone (its bit in D_0048BBC0). */
s32 func_00662380(u32 *args) {
    u32 n = args[0];

    D_0048BBC0[n >> 5] |= 1 << (n & 0x1F);
    return 0;
}

void func_00578AA0(u8 a, u8 b, u8 c);

s32 func_00662460(u32 *args) {
    func_00578AA0(args[0], args[1], args[2]);
    return 0;
}

s32 func_001AF440(s32 room);

#ifdef PLATFORM_PC
u8 func_00634640(DrawObjFull *p, s32 a);
#else
s32 func_00634640(DrawObjFull *p, s32 a);   /* (returns u8, but this file saw s32: no masking) */
#endif

/* if every player still in the game is in room args[0] (args[1] 1) / none is (args[1] 0);
   false when nobody is left */
s32 func_00661450(u32 *args) {
    s32 n = 0;
    u16 room = args[0];
    s32 v = args[1];
    s32 i;
    DrawObjFull *p;

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        for (i = 0; i < *(s32 *)&D_0023BE14; i++) {
            p = &D_00476DD0[i];
            if (func_00634640(p, 0) == 0) {
                if (room != p->unk1E2) {
                    func_0065AAF0();
                    return 2;
                }
                n++;
            }
        }
    } else {
        for (i = 0; i < *(s32 *)&D_0023BE14; i++) {
            p = &D_00476DD0[i];
            if (func_00634640(p, 0) == 0) {
                if (room == p->unk1E2) {
                    func_0065AAF0();
                    return 2;
                }
                n++;
            }
        }
    }
    if (n == 0) {
        func_0065AAF0();
        return 2;
    }
    return 0;
}


/* if we are the player in this room is args[0] */
s32 func_00661DE0(u32 *args) {
    s32 v = args[0];

    D_0048BE70.unk78[CUR]++;
    if (v != 0) {
        if (D_0048BF60.unk1D != func_001AF440(D_00306590.unk1C)) {
            func_0065AAF0();
            return 2;
        }
    } else {
        if (D_0048BF60.unk1D == func_001AF440(D_00306590.unk1C)) {
            func_0065AAF0();
            return 2;
        }
    }
    return 0;
}

s32 func_001AF4D0(void);

/* if func_001AF4D0 (the talk state) is 1 (args[0] 0) or 0 (args[0] 1) */
s32 func_00662D90(u32 *args) {
    s32 r;

    D_0048BE70.unk78[CUR]++;
    r = 2;
    switch (args[0]) {
    case 0:
        if (func_001AF4D0() == 1) {
            r = 0;
        }
        break;
    case 1:
        if (func_001AF4D0() == 0) {
            r = 0;
        }
        break;
    }
    if (r != 0) {
        func_0065AAF0();
    }
    return r;
}

void func_001B2C70(void);

/* 0x190 (does nothing in the release) */
s32 func_00662E40(u32 *args) {
    func_001B2C70();
    return 0;
}

void func_001B2C80(void);

/* (does nothing in the release either) */
s32 func_00662E60(u32 *args) {
    func_001B2C80();
    return 0;
}
