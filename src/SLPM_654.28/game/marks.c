#include "common.h"
#include "game.h"
#include "object.h"

/* GAME.BIN: a table of 64 marks, each a (type, id) pair (e.g. type 2: an enemy slot). */

typedef struct {
    u8 used;            /* 0x00 */
    u8 type;            /* 0x01 */
    u8 unk2;
    u8 unk3;            /* 0x03 */
    s16 id;             /* 0x04 */
    u8 unk6[0x10 - 6];
    s16 unk10;
    s16 unk12;
    s16 unk14;
    s16 unk16;
    u8 unk18[4];
} Mark;                 /* 0x1C */

extern Mark D_0070F9F8[0x40];

static s32 func_0064F620(u8 type, s16 id) {
    s32 i;
    Mark *m = D_0070F9F8;

    for (i = 0; i < 0x40; i++, m++) {
        if (m->used != 0 && m->type == type && m->id == id) {
            return 1;
        }
    }
    return 0;
}

void func_0064F690(u8 type, s16 id) {
    Mark *m;
    s32 i;

    if (func_0064F620(type, id) != 0) {
        return;
    }
    for (i = 0, m = D_0070F9F8; i < 0x40; i++, m++) {
        if (m->used == 0) {
            m->used = 1;
            m->type = type;
            m->id = id;
            m->unk16 = 0;
            m->unk14 = 0;
            m->unk12 = 0;
            m->unk10 = 0;
            m->unk3 = 0xFF;
            return;
        }
    }
}

/* Who holds each of 8 shared things (online: told to the others). */

typedef struct {
    u8 type;
    u8 len;
    s8 idx;
    u8 unk3;
    u8 data[0x6C];
} NetMsg;

extern u8 D_0048AE20[0x10];         /* holder, 0xFF none */
extern s16 D_0048AE30[8];
extern s16 D_0048AE70[8];
s32 func_001AF510(void);
s32 func_001AF4D0(void);
void func_001AF000(s32 to, NetMsg *m, s32 kind, s32 a3);

typedef struct {
    u8 type;
    u8 len;
    s8 idx;
    u8 unk3;
    s8 i;
    s8 take;
    s16 count;
} LockMsg;

#ifdef NON_MATCHING   /* the message's stack slot and store order differ */
void func_0064E600(s32 i, s32 take, s16 value) {
    LockMsg m;

    if (func_001AF510() == 1 && func_001AF4D0() == 0) {
        if (take != 0) {
            if (D_0048AE20[i] != 0xFF && D_0048AE20[i] != D_0048BF60.unk1D) {
                D_0048AE30[i] = 0;
                return;
            }
            D_0048AE30[i] = value;
            D_0048AE70[i]++;
        } else {
            D_0048AE30[i] = value;
        }
        m.i = i;
        m.type = 2;
        m.take = take;
        m.len = 8;
        m.unk3 = 3;
        m.count = D_0048AE70[i];
        func_001AF000(5, (NetMsg *)&m, 1, 4);
        return;
    }
    if (take != 0) {
        if (D_0048AE20[i] != 0xFF && D_0048AE20[i] != D_0048BF60.unk1D) {
            D_0048AE30[i] = 0;
            return;
        }
        D_0048AE20[i] = D_0048BF60.unk1D;
        return;
    }
    D_0048AE20[i] = 0xFF;
}
#endif

typedef struct {
    u8 unk0[0x210];
    u8 owner[0x10];         /* 0x210 */
    s16 timer[0x10];        /* 0x220 */
    s16 unk240[0x10];       /* 0x240 */
} SceneData210;

extern SceneData210 D_0048AC10;

void func_0064EE00(void) {
    s32 i;

    for (i = 0; i < 0x10; i++) {
        if (D_0048AC10.owner[i] != D_0048BF60.unk1D && D_0048AC10.timer[i] != 0) {
            D_0048AC10.timer[i]--;
            if (D_0048AC10.timer[i] == 0) {
                func_0064E600(i, 0, 0);
            }
        }
    }
}

typedef struct {
    u8 type;
    u8 len;
    s8 idx;
    u8 unk3;
    s8 a;
    s8 b;
    s8 c;
    u8 pad;
    s16 d;
} OwnerMsg;

#ifdef NON_MATCHING   /* first draft */
/* Online: tells the others about a thing's holder. */
void func_0064E940(s8 a, s8 b, s8 c, s16 d) {
    OwnerMsg m;

    if (func_001AF510() == 0) {
        return;
    }
    if (func_001AF4D0() == 1) {
        return;
    }
    m.len = 0xA;
    m.type = 3;
    m.unk3 = 3;
    m.a = a;
    m.b = b;
    m.c = c;
    m.d = d;
    func_001AF000(7, (NetMsg *)&m, 0, 4);
}
#endif

typedef struct {
    u8 unk0[0x1214];
    u8 unk1214[4];
    u8 unk1218[4];
    u16 unk121C[4];
} SceneData1214;

extern u32 D_0023BE14;              /* players */
s32 func_001AF110(s32 i);
void func_0064BFC0(u16 a, u8 b, s32 c);

#ifdef NON_MATCHING   /* first draft */
/* Online: drops what players who left were holding. */
void func_0064EB50(void) {
    s32 i;
    s32 j;
    SceneData1214 *d;

    if (func_001AF4D0() == 1) {
        return;
    }
    for (i = 0; i < D_0023BE14; i++) {
        if (func_001AF110(i) != 0) {
            continue;
        }
        d = (SceneData1214 *)((u8 *)&D_0048AC10 + i * 0x10);
        for (j = 0; j < 4; j++) {
            if (d->unk1214[j] != 0) {
                func_0064BFC0(d->unk121C[j], d->unk1218[j], 0);
                d->unk1214[j] = 0;
                d->unk1218[j] = 0;
                d->unk121C[j] = 0;
            }
        }
    }
}
#endif

s32 func_001AF0A0(void);
s32 func_00634640(DrawObjFull *o, s32 a);
extern DrawObjFull D_00476DD0[8];

void func_0064E9F0(void);
#ifdef NON_MATCHING   /* first draft */
/* Things whose holder is gone (dead offline, out of the session online) are let go. */
void func_0064E9F0(void) {
    s32 i;

    if (func_001AF510() == 0) {
        return;
    }
    func_0064EB50();
    if (func_001AF0A0() == 0) {
        return;
    }
    if (func_001AF4D0() == 1) {
        for (i = 0; i < 0x10; i++) {
            if (D_0048AC10.owner[i] != 0xFF && func_00634640(&D_00476DD0[D_0048AC10.owner[i]], 0) != 0) {
                D_0048AC10.owner[i] = 0xFF;
                D_0048AC10.unk240[i] = -1;
                func_0064E940(D_0048AC10.owner[i], 0xFF, 0, -1);
            }
        }
        return;
    }
    for (i = 0; i < 0x10; i++) {
        if (D_0048AC10.owner[i] != 0xFF && func_001AF110(D_0048AC10.owner[i]) == 0) {
            D_0048AC10.owner[i] = 0xFF;
            D_0048AC10.unk240[i] = -1;
            func_0064E940(D_0048AC10.owner[i], 0xFF, 0, -1);
        }
    }
}
#endif
