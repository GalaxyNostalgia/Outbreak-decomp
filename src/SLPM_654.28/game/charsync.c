#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* GAME.BIN: telling the others (online the other players, offline the partners' code) about a
   character's state: one message kind per call, not the same kind twice in a row. */

typedef struct {
    u8 kind;            /* 0x00 */
    u8 len;             /* 0x01 */
    u8 unk2;
    u8 unk3;
    u8 data[0x3C];      /* 0x04 */
} CharMsg;

typedef struct {
    s8 used;
    u8 room;
    u8 unk2[0x16];
    s8 x;               /* 0x18 */
    s8 y;               /* 0x19 */
    u8 unk1A[0x3C - 0x1A];
} PlacedSlot;

extern s8 D_0048BF88;
extern u16 D_0023BF0C;          /* the last kind sent */
extern DrawObjFull D_00476DD0[];
extern PlacedSlot D_00395720[32];
s32 func_001AF510(void);
s32 func_001AF4D0(void);
s32 func_001AF440(u16 room);
void func_001AF000(s32 to, void *msg, s32 kind, s32 a3);
/* Whether o (action 1) is in one of the steps that count as standing for the others. */
s32 func_006332D0(DrawObjFull *o);
#ifdef NON_MATCHING   /* the return blocks' order */
s32 func_006332D0(DrawObjFull *o) {
    if (o->act[0] != 1) {
        return 0;
    }
    switch (o->act[1]) {
    case 0x0:
    case 0x1:
    case 0x2:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0x6:
    case 0x7:
    case 0x8:
    case 0x9:
    case 0xA:
    case 0xB:
    case 0xC:
    case 0xD:
    case 0xE:
    case 0x31:
    case 0x32:
    case 0x98:
    case 0x99:
    case 0x9A:
    case 0x9B:
    case 0x9C:
    case 0x9D:
    case 0x9E:
    case 0x9F:
    case 0xA0:
    case 0x24:
    case 0x74:
    case 0x75:
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7A:
    case 0x7B:
    case 0x7C:
    case 0x4A:
    case 0x4B:
    case 0x4C:
    case 0x4D:
    case 0x4E:
    case 0x4F:
    case 0x50:
    case 0x51:
    case 0x52:
    case 0x53:
    case 0x54:
    case 0x55:
    case 0xCD:
    case 0xD0:
    case 0xD1:
    case 0x81:
    case 0x82:
    case 0x83:
        return 1;
    }
    return 0;
}
#endif
DrawObjFull *func_00634C00(s32 i);
void func_0018D630(void *dst, void *src, s32 size);
extern u32 D_006E6C70[];        /* per character: the virus count that is 100% */

/* Player i's health in four steps: 0 from 75%, 1 from 50%, 2 from 25%, 3 below. */
s32 func_005C6170(s32 i);
#ifdef NON_MATCHING   /* close: the last two returns are laid out the other way */
s32 func_005C6170(s32 i) {
    DrawObjFull *p = &D_00476DD0[i];
    f32 v;

    if (p->unk546 != 0) {
        v = (f32)p->unk544 / (f32)p->unk546;
    } else {
        v = 1.0f;
    }
    if (!(v < 0.75f)) {
        return 0;
    }
    if (!(v < 0.5f)) {
        return 1;
    }
    return v < 0.25f ? 3 : 2;
}
#endif

/* Player i's virus gauge in four steps: 0 below 50%, 1 below 80%, 2 below 90%, 3. */
s32 func_005C6240(s32 i) {
    DrawObjFull *p = &D_00476DD0[i];
    s32 v = 100.0f * ((f32)(u32)p->unkBAC / (f32)D_006E6C70[p->unkB98]);

    if (v >= 90) {
        return 3;
    }
    if (v >= 80) {
        return 2;
    }
    return v >= 50;
}

s32 func_001AF540(DrawObjFull *o);
void func_0034B7D0(s32 a, s32 b, s32 c);
f32 func_00636600(u32 n);

/* (Online) the message for our virus passing 50, 70 or 90% (0x21-0x23) going from to to. */
void func_00636440(DrawObjFull *o, u32 from, u32 to);
#ifdef NON_MATCHING   /* close: three delay slots the original leaves empty */
void func_00636440(DrawObjFull *o, u32 from, u32 to) {
    f32 a;
    f32 b;

    if (func_001AF540(o)) {
        a = func_00636600(from);
        b = func_00636600(to);
        if (a < 50.0f && !(b < 50.0f)) {
            func_0034B7D0(0x21, 0, 0);
            return;
        }
        if (a < 70.0f && !(b < 70.0f)) {
            func_0034B7D0(0x22, 0, 0);
            return;
        }
        if (a < 90.0f && !(b < 90.0f)) {
            func_0034B7D0(0x23, 0, 0);
        }
    }
}
#endif

/* Our player's virus gauge, in % (o isn't used). */
f32 func_00636540(DrawObjFull *o) {
    DrawObjFull *p = &D_00476DD0[D_0048BF60.unk1D];

    return 100.0f * ((f32)(u32)p->unkBAC / (f32)D_006E6C70[p->unkB98]);
}

/* A virus count n as our player's %. */
f32 func_00636600(u32 n) {
    DrawObjFull *p = &D_00476DD0[D_0048BF60.unk1D];

    return 100.0f * ((f32)n / (f32)D_006E6C70[p->unkB98]);
}

/* That in four steps: 0 below 50%, 1 below 80%, 2 below 90%, 3. */
u8 func_00637150(DrawObjFull *o) {
    f32 v = func_00636540(o);
    u8 r;

    if (!(v < 90.0f)) {
        r = 3;
    } else if (!(v < 80.0f)) {
        r = 2;
    } else if (!(v < 50.0f)) {
        r = 1;
    } else {
        r = 0;
    }
    return r;
}

/* The message's bytes by their place in the original's stack frame (the message is at 0x50). */
#define M8(off) (*(u8 *)&m.data[(off) - 0x54])
#define M16(off) (*(s16 *)&m.data[(off) - 0x54])
#define M32(off) (*(s32 *)&m.data[(off) - 0x54])
#define MF(off) (*(f32 *)&m.data[(off) - 0x54])

void func_0064C620(DrawObjFull *o, u8 kind);
#ifdef NON_MATCHING   /* first draft */
void func_0064C620(DrawObjFull *o, u8 kind) {
    CharMsg m;
    s32 pri = 4;
    DrawObjFull *p;
    s32 i;
    u16 v;

    if (!func_001AF510() || D_0048BF88 != 0) {
        return;
    }
    switch (o->type) {
    case 0:
        if (o->kind != D_0048BF60.unk1D) {
            return;
        }
        break;
    case 1:
        if (!(o->unkBA0 & 0x10000000)) {
            return;
        }
        break;
    default:
        return;
    }
    if (o->unkBA0 & 4) {
        return;
    }
    if (D_0023BF0C == kind) {
        return;
    }
    m.unk3 = 0;
    m.unk2 = 0;
    switch (kind) {
    case 0:
        return;
    case 1:
        pri = 4;
        if (func_006332D0(o)) {
            o->unkBA0 |= 0x8000000;
        } else {
            o->unkBA0 &= ~0x8000000;
        }
        m.kind = kind;
        m.len = 0x2C;
        M8(0x54) = o->act[0];
        M8(0x55) = o->act[1];
        M8(0x56) = o->act[2];
        M8(0x57) = o->act[3];
        M8(0x74) = o->unk1E2;
        MF(0x58) = o->unk38[0];
        MF(0x5C) = o->unk38[1];
        MF(0x60) = o->unk38[2];
        M16(0x64) = o->unk92;
        M16(0x66) = o->unk544;
        M32(0x68) = o->unkBA0;
        M8(0x6C) = o->unkC7C;
        M8(0x6D) = FIELD(o, u8, 0xC7F);
        M8(0x6E) = FIELD(o, u8, 0xC80);
        M8(0x6F) = FIELD(o, u8, 0xCB0);
        M16(0x70) = FIELD(o, s16, 0xBC8);
        M16(0x72) = FIELD(o, s16, 0xCD4);
        M8(0x75) = FIELD(o, u8, 0xCDC);
        M8(0x77) = func_00637150(o);
        M8(0x78) = FIELD(o, u8, 0xCAA);
        M8(0x79) = FIELD(o, u8, 0xCAB);
        if (o->unkBA4 & 0x20000000) {
            M8(0x7A) = 1;
        } else {
            M8(0x7A) = 0;
        }
        if (o->type == 1) {
            M8(0x76) = o->unkBB4;
        }
        D_0023BF0C = kind;
        break;
    case 2:
        pri = 2;
        if (func_006332D0(o)) {
            o->unkBA0 |= 0x8000000;
        } else {
            o->unkBA0 &= ~0x8000000;
        }
        m.kind = kind;
        m.len = 0x18;
        MF(0x54) = o->unk38[0];
        MF(0x58) = o->unk38[1];
        MF(0x5C) = o->unk38[2];
        M16(0x64) = o->unk92;
        M32(0x60) = o->unkBA0;
        M8(0x66) = func_00637150(o);
        break;
    case 3:
        m.kind = kind;
        pri = 4;
        m.len = 0x20;
        for (i = 0; i < 7; i++) {
            M32(0x54 + i * 4) = FIELD(o, s32, 0xAB8 + i * 4);
        }
        break;
    case 4:
        m.kind = kind;
        m.len = 8;
        pri = 4;
        M8(0x54) = FIELD(o, u8, 0xC78);
        M8(0x55) = FIELD(o, u8, 0xC79);
        M8(0x56) = FIELD(o, u8, 0xC7A);
        M8(0x57) = FIELD(o, u8, 0xC7B);
        break;
    case 5:
        m.kind = kind;
        m.len = 0x34;
        pri = 4;
        M8(0x54) = o->act[0];
        M8(0x55) = o->act[1];
        M8(0x56) = o->act[2];
        M8(0x57) = o->act[3];
        M16(0x58) = o->unk38[0];
        M16(0x5A) = o->unk38[1];
        M16(0x5C) = o->unk38[2];
        M16(0x5E) = o->unk92;
        M16(0x60) = o->unk544;
        M8(0x62) = o->unk1E2;
        M32(0x64) = o->unkBA0;
        M8(0x68) = FIELD(o, u8, 0x148);
        M8(0x69) = FIELD(o, u8, 0x149);
        M8(0x6A) = FIELD(o, u8, 0x14A);
        M8(0x6B) = FIELD(o, u8, 0x14B);
        M8(0x6C) = FIELD(o, u8, 0x140);
        M8(0x6D) = FIELD(o, u8, 0x13E);
        M8(0x6E) = FIELD(o, u8, 0x14C);
        M8(0x6F) = FIELD(o, u8, 0x141);
        M8(0x70) = FIELD(o, u8, 0x143);
        M8(0x71) = func_00637150(o);
        *(Vec3 *)&MF(0x74) = FIELD(o, Vec3, 0x12C);
        M32(0x80) = FIELD(o, s32, 0x11C);
        if (o->type == 1) {
            M8(0x63) = o->unkBB4;
        }
        D_0023BF0C = kind;
        break;
    case 6:
        m.kind = kind;
        m.len = 0x14;
        pri = 4;
        MF(0x54) = FIELD(o, f32, 0xC34);
        MF(0x58) = FIELD(o, f32, 0xC38);
        MF(0x5C) = FIELD(o, f32, 0xC3C);
        M16(0x60) = FIELD(o, s16, 0xC4C);
        break;
    case 7:
        m.kind = kind;
        m.len = 0xC;
        pri = 4;
        M16(0x54) = FIELD(o, u16, 0xD92);
        M8(0x56) = FIELD(o, u8, 0xDA2);
        M8(0x57) = FIELD(o, u8, 0xDA3);
        M8(0x58) = FIELD(o, u8, 0xDA4);
        M8(0x59) = FIELD(o, u8, 0xDA5);
        M8(0x5A) = FIELD(o, u8, 0xC7F);
        if (func_001AF4D0() == 1) {
            if ((FIELD(o, u16, 0xD92) & 1) && FIELD(o, u8, 0xDA3) != 0) {
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA3) - 1], u8, 0xC78) = 2;
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA3) - 1], u8, 0xC79) = 0;
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA3) - 1], u8, 0xC7A) = FIELD(o, u8, 0xC7A);
            }
            if ((FIELD(o, u16, 0xD92) & 4) && FIELD(o, u8, 0xDA4) != 0) {
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA4) - 1], u8, 0xC78) = 2;
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA4) - 1], u8, 0xC79) = 1;
            }
            if ((FIELD(o, u16, 0xD92) & 0x100) && FIELD(o, u8, 0xDA4) != 0) {
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA4) - 1], u8, 0xC78) = 2;
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA4) - 1], u8, 0xC79) = 0;
                FIELD(&D_00476DD0[FIELD(o, u8, 0xDA4) - 1], u8, 0xC7A) = FIELD(o, u8, 0xC7A);
            }
        }
        break;
    case 8:
        m.kind = kind;
        m.len = 0xC;
        pri = 4;
        M16(0x54) = FIELD(o, s16, 0xC64);
        M16(0x56) = FIELD(o, s16, 0xC66);
        M16(0x58) = FIELD(o, s16, 0xC68);
        M16(0x5A) = FIELD(o, s16, 0xC6A);
        if (func_001AF4D0() == 1) {
            v = M16(0x56);
            switch ((v & 0x8000) >> 15) {
            case 0:
                p = &D_00476DD0[v & 0x7FFF];
                break;
            case 1:
                p = func_00634C00(v & 0x7FFF);
                break;
            }
            if (p == NULL) {
                return;
            }
            FIELD(p, s16, 0xC64) = M16(0x54);
            FIELD(p, s16, 0xC66) = M16(0x56);
            FIELD(p, s16, 0xC68) = M16(0x58);
            FIELD(p, s16, 0xC6A) = M16(0x5A);
            FIELD(p, u16, 0xC66) = o->kind;
        }
        break;
    case 9:
        m.len = 8;
        m.kind = kind;
        pri = 4;
        M8(0x54) = FIELD(o, u8, 0xCB0);
        M8(0x55) = D_00395720[FIELD(o, u8, 0xCB0)].x;
        M8(0x56) = D_00395720[FIELD(o, u8, 0xCB0)].y;
        break;
    case 10:
        pri = 4;
        m.kind = kind;
        m.len = 0x10;
        M8(0x54) = FIELD(o, u8, 0xCAA);
        M8(0x55) = FIELD(o, u8, 0xCAB);
        func_0018D630(&M8(0x56), (u8 *)o + 0xC84, pri);
        break;
    case 11:
        m.kind = kind;
        m.len = 0x14;
        pri = 4;
        M32(0x54) = FIELD(o, s32, 0xDB4);
        M16(0x58) = FIELD(o, s16, 0xDC2);
        M16(0x5A) = FIELD(o, s8, 0xDC7);
        M32(0x60) = FIELD(o, s8, 0xDC1);
        M8(0x5C) = FIELD(o, s8, 0xDC0);
        M8(0x5E) = FIELD(o, s8, 0xDA9);
        M8(0x5D) = !(func_00636540(o) < 90.0f);
        FIELD(o, s32, 0xDB8) = (s8)M8(0x5D);
        break;
    case 12:
        pri = 4;
        m.len = 8;
        m.kind = kind;
        func_0018D630(&M8(0x54), (u8 *)o + 0xC84, pri);
        break;
    case 13:
        return;
    case 14:
        m.kind = kind;
        m.len = 0xC;
        pri = 4;
        M32(0x54) = o->unkBA0;
        M16(0x58) = o->unk544;
        break;
    case 15:
        pri = 4;
        m.len = 8;
        m.kind = kind;
        func_0018D630(&M8(0x54), (u8 *)o + 0xC89, pri);
        break;
    }
    switch (o->type) {
    case 0:
        func_001AF000(o->kind + 1, &m, 0, pri);
        break;
    case 1:
        func_001AF000(func_001AF440(D_00306590.unk1C) + 1, &m, 0, pri);
        break;
    }
}
#endif

/* A talk condition: its kind (func_005C6480), whether it's negated, and a value. */
typedef struct {
    u8 kind;
    u8 not;             /* 0x1: (or the comparison) */
    u16 arg;            /* 0x2 */
    s32 val;            /* 0x4 */
} TalkCond;

u16 func_001ADEF0(s32 n);

/* Kind 0xE: a random 0-9 against val (op c->not: ==, !=, <, >, <=, >=). */
s32 func_005C74D0(TalkCond *c, DrawObjFull *p) {
    u8 r = (func_001ADEF0(1) & 0xFFFF) % 10;

    switch (c->not) {
    case 0:
        return r == c->val;
    case 1:
        return r != c->val;
    case 2:
        return r < c->val;
    case 3:
        return r > c->val;
    case 4:
        return r <= c->val;
    case 5:
        return r >= c->val;
    default:
        return 0;
    }
}

DrawObjFull *func_00651300(s16 id);

/* Kind 0x16: the state (0xBA0) of the one p talks to (0xDC2: a player, or an NPC by id). */
s32 func_005C7CC0(TalkCond *c, DrawObjFull *p) {
    DrawObjFull *q;
    s32 f;

    if (p->unkDC7 < 0) {
        if (p->unkDC2 < 0) {
            return 0;
        }
        q = &D_00476DD0[p->unkDC2];
    } else {
        q = func_00651300(p->unkDC2);
        if (q == NULL) {
            return 0;
        }
    }
    f = q->unkBA0;
    if (c->not == 0) {
        switch (c->val) {
        case 0:
            return !((f & 0x33) != 0);
        case 1:
            return (f & 1) != 0;
        case 2:
            return (f & 2) != 0;
        case 3:
            return 0;
        case 4:
            return (f & 0x10) != 0;
        case 5:
            return (f & 0x20) != 0;
        default:
            return 0;
        }
    } else if (c->not == 1) {
        switch (c->val) {
        case 0:
            return (f & 0x33) != 0;
        case 1:
            return !((f & 1) != 0);
        case 2:
            return !((f & 2) != 0);
        case 3:
            return 0;
        case 4:
            return !((f & 0x10) != 0);
        case 5:
            return !((f & 0x20) != 0);
        default:
            return 0;
        }
    }
    return 0;
}

/* Kind 0x13: p's health against val (op c->not: ==, !=, <, >, <=, >=). */
s32 func_005C7980(TalkCond *c, DrawObjFull *p) {
    switch (c->not) {
    case 0:
        return p->unk544 == c->val;
    case 1:
        return p->unk544 != c->val;
    case 2:
        return p->unk544 < c->val;
    case 3:
        return p->unk544 > c->val;
    case 4:
        return p->unk544 <= c->val;
    case 5:
        return p->unk544 >= c->val;
    default:
        return 0;
    }
}

extern u8 D_30659C[];
extern u32 D_00719628[];        /* the timers running (a bit each) */
extern s32 D_00719668[];        /* the timers (frames) */

/* Kind 1: timer arg (in seconds) against val, while it runs. */
s32 func_005C6AD0(TalkCond *c, DrawObjFull *p);
#ifdef NON_MATCHING   /* scheduling of the timer bit test */
s32 func_005C6AD0(TalkCond *c, DrawObjFull *p) {
    s32 fps = D_30659C[0] != 0 ? 0x1E : 0x3C;
    s32 t = c->arg;

    if (D_00719628[(u32)t >> 5] & (1 << (t % 32))) {

        switch (c->not) {
        case 0:
            return c->val == D_00719668[t] / fps;
        case 1:
            return c->val != D_00719668[t] / fps;
        case 2:
            return D_00719668[t] / fps < c->val;
        case 3:
            return D_00719668[t] / fps > c->val;
        case 4:
            return D_00719668[t] / fps <= c->val;
        case 5:
            return D_00719668[t] / fps >= c->val;
        }
    }
    return 0;
}
#endif

/* Kind 0x15: p's state (0xBA0): 0 fine (none of 0xB3), 1, 2, 4 0x10, 5 0x20, 6 0x80 (or not). */
s32 func_005C7B50(TalkCond *c, DrawObjFull *p) {
    if (c->not == 0) {
        switch (c->val) {
        case 0:
            return !((p->unkBA0 & 0xB3) != 0);
        case 1:
            return (p->unkBA0 & 1) != 0;
        case 2:
            return (p->unkBA0 & 2) != 0;
        case 3:
            return 0;
        case 4:
            return (p->unkBA0 & 0x10) != 0;
        case 5:
            return (p->unkBA0 & 0x20) != 0;
        case 6:
            return (p->unkBA0 & 0x80) != 0;
        default:
            return 0;
        }
    } else if (c->not == 1) {
        switch (c->val) {
        case 0:
            return (p->unkBA0 & 0xB3) != 0;
        case 1:
            return !((p->unkBA0 & 1) != 0);
        case 2:
            return !((p->unkBA0 & 2) != 0);
        case 3:
            return 0;
        case 4:
            return !((p->unkBA0 & 0x10) != 0);
        case 5:
            return !((p->unkBA0 & 0x20) != 0);
        case 6:
            return !((p->unkBA0 & 0x80) != 0);
        default:
            return 0;
        }
    }
    return 0;
}

/* Kind 0x30: the character of the other player p's 0x80 talk command names is val (or not). */
s16 func_005C92E0(TalkCond *c, DrawObjFull *p) {
    s32 k = p->talkCmd;

    if (k == 0) {
        return 0;
    }
    if (!((s8)k & 0x80)) {
        return 0;
    }
    if (c->not == 0) {
        return p->others[(s8)k & 0x7F] == c->val;
    }
    return p->others[(s8)k & 0x7F] != c->val;
}

f32 func_00341860(f32 x);

/* Kind 0x2C: whether another character in p's room is within (op c->not) arg across the floor;
   val 0 asks for none. */
s32 func_005C9060(TalkCond *c, DrawObjFull *p) {
    f32 r = c->arg;
    DrawObjFull *q;
    s32 i;
    s32 f;
    f32 d;
    f32 dx;
    f32 dz;

    f = 0;
    q = D_00476DD0;
    for (i = 0; i < 8; i++, q++) {
        if ((q->active & q->visible) && (p->type != 0 || q != p) && p->unk1E2 == q->unk1E2) {
            dx = q->unk38[0] - p->unk38[0];
            dz = q->unk38[2] - p->unk38[2];
            d = func_00341860(dx * dx + dz * dz);
            switch (c->not) {
            case 0:
                if (d <= r) {
                    f = 1;
                }
                break;
            case 1:
                if (!(d <= r)) {
                    f = 1;
                }
                break;
            case 2:
                if (d < r) {
                    f = 1;
                }
                break;
            case 3:
                if (!(d <= r)) {
                    f = 1;
                }
                break;
            case 4:
                if (d <= r) {
                    f = 1;
                }
                break;
            case 5:
                if (!(d < r)) {
                    f = 1;
                }
                break;
            }
        }
    }
    if (c->val == 0) {
        return !(f != 0);
    }
    return f != 0;
}

/* Kind 0x2E: p's talk command (0xDA9) is val (or not); none, or 0x80 ones, never. */
s16 func_005C9210(TalkCond *c, DrawObjFull *p) {
    s32 k = p->talkCmd;

    if (k == 0) {
        return 0;
    }
    if ((s8)k & 0x80) {
        return 0;
    }
    if (c->not == 0) {
        return (s8)k == c->val;
    }
    return (s8)k != c->val;
}

s16 func_005C6A40(u8 *c, DrawObjFull *p);
s16 func_005C6CA0(u8 *c, DrawObjFull *p);
s16 func_005C6CF0(u8 *c, DrawObjFull *p);
s16 func_005C6DA0(u8 *c, DrawObjFull *p);
s16 func_005C6E50(u8 *c, DrawObjFull *p);
s16 func_005C6F40(u8 *c, DrawObjFull *p);
s16 func_005C7090(u8 *c, DrawObjFull *p);
s16 func_005C71A0(u8 *c, DrawObjFull *p);
s16 func_005C7210(u8 *c, DrawObjFull *p);
s16 func_005C7290(u8 *c, DrawObjFull *p);
s16 func_005C7300(u8 *c, DrawObjFull *p);
s16 func_005C7370(u8 *c, DrawObjFull *p);
s16 func_005C7420(u8 *c, DrawObjFull *p);
s16 func_005C75B0(u8 *c, DrawObjFull *p);
s16 func_005C7600(u8 *c, DrawObjFull *p);
s16 func_005C7650(u8 *c, DrawObjFull *p);
s16 func_005C77F0(u8 *c, DrawObjFull *p);
s16 func_005C7A30(u8 *c, DrawObjFull *p);
s16 func_005C7E50(u8 *c, DrawObjFull *p);
s16 func_005C7EF0(u8 *c, DrawObjFull *p);
s16 func_005C7F90(u8 *c, DrawObjFull *p);
s16 func_005C7FB0(u8 *c, DrawObjFull *p);
s16 func_005C8040(u8 *c, DrawObjFull *p);
s16 func_005C80F0(u8 *c, DrawObjFull *p);
s16 func_005C8130(u8 *c, DrawObjFull *p);
s16 func_005C81E0(u8 *c, DrawObjFull *p);
s16 func_005C8390(u8 *c, DrawObjFull *p);
s16 func_005C8540(u8 *c, DrawObjFull *p);
s16 func_005C8650(u8 *c, DrawObjFull *p);
s16 func_005C8730(u8 *c, DrawObjFull *p);
s16 func_005C8820(u8 *c, DrawObjFull *p);
s16 func_005C8910(u8 *c, DrawObjFull *p);
s16 func_005C89B0(u8 *c, DrawObjFull *p);
s16 func_005C8CF0(u8 *c, DrawObjFull *p);
s16 func_005C8DA0(u8 *c, DrawObjFull *p);
s16 func_005C8E50(u8 *c, DrawObjFull *p);
s16 func_005C8F00(u8 *c, DrawObjFull *p);
s16 func_005C8FB0(u8 *c, DrawObjFull *p);
s16 func_005C9270(u8 *c, DrawObjFull *p);

/* A talk (or event) condition (8 bytes: its kind first) for p: each kind's check; 0xFF is
   always true. */
s16 func_005C6480(u8 *c, DrawObjFull *p);
#ifdef NON_MATCHING   /* the compare chain: the original fills every other beq's delay slot */
s16 func_005C6480(u8 *c, DrawObjFull *p) {
    s16 r;

    switch (c[0]) {
    default:
        r = 0;
        break;
    case 0x0:
        r = func_005C6A40(c, p);
        break;
    case 0x1:
        r = func_005C6AD0((TalkCond *)c, p);
        break;
    case 0x2:
        r = func_005C6CA0(c, p);
        break;
    case 0x3:
        r = func_005C6CF0(c, p);
        break;
    case 0x4:
        r = func_005C6DA0(c, p);
        break;
    case 0x5:
        r = func_005C6E50(c, p);
        break;
    case 0x6:
        r = func_005C6F40(c, p);
        break;
    case 0x7:
        r = func_005C7090(c, p);
        break;
    case 0x8:
        r = func_005C71A0(c, p);
        break;
    case 0x9:
        r = func_005C7210(c, p);
        break;
    case 0xA:
        r = func_005C7290(c, p);
        break;
    case 0xB:
        r = func_005C7300(c, p);
        break;
    case 0xC:
        r = func_005C7370(c, p);
        break;
    case 0xD:
        r = func_005C7420(c, p);
        break;
    case 0xE:
        r = func_005C74D0((TalkCond *)c, p);
        break;
    case 0xF:
        r = func_005C75B0(c, p);
        break;
    case 0x10:
        r = func_005C7600(c, p);
        break;
    case 0x11:
        r = func_005C7650(c, p);
        break;
    case 0x12:
        r = func_005C77F0(c, p);
        break;
    case 0x13:
        r = func_005C7980((TalkCond *)c, p);
        break;
    case 0x14:
        r = func_005C7A30(c, p);
        break;
    case 0x15:
        r = func_005C7B50((TalkCond *)c, p);
        break;
    case 0x16:
        r = func_005C7CC0((TalkCond *)c, p);
        break;
    case 0x17:
        r = func_005C7E50(c, p);
        break;
    case 0x18:
        r = func_005C7EF0(c, p);
        break;
    case 0x19:
        r = func_005C7F90(c, p);
        break;
    case 0x1A:
        r = func_005C7FB0(c, p);
        break;
    case 0x1B:
        r = func_005C8040(c, p);
        break;
    case 0x1D:
        r = func_005C80F0(c, p);
        break;
    case 0x1E:
        r = func_005C8130(c, p);
        break;
    case 0x1F:
        r = func_005C81E0(c, p);
        break;
    case 0x20:
        r = func_005C8390(c, p);
        break;
    case 0x21:
        r = func_005C8540(c, p);
        break;
    case 0x22:
        r = func_005C8650(c, p);
        break;
    case 0x23:
        r = func_005C8910(c, p);
        break;
    case 0x24:
        r = func_005C8730(c, p);
        break;
    case 0x25:
        r = func_005C89B0(c, p);
        break;
    case 0x26:
        r = func_005C8820(c, p);
        break;
    case 0x27:
        r = func_005C8CF0(c, p);
        break;
    case 0x28:
        r = func_005C8DA0(c, p);
        break;
    case 0x29:
        r = func_005C8E50(c, p);
        break;
    case 0x2A:
        r = func_005C8F00(c, p);
        break;
    case 0x2B:
        r = func_005C8FB0(c, p);
        break;
    case 0x2C:
        r = func_005C9060((TalkCond *)c, p);
        break;
    case 0x2E:
        r = func_005C9210((TalkCond *)c, p);
        break;
    case 0x2F:
        r = func_005C9270(c, p);
        break;
    case 0x30:
        r = func_005C92E0((TalkCond *)c, p);
        break;
    case 0xFF:
        r = 1;
        break;
    }
    return r;
}
#endif
