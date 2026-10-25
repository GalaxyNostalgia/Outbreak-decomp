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
    u8 state;           /* 0x06 */
    u8 unk7;            /* 0x07 */
    s8 unk8;            /* 0x08 */
    u8 unk9[0x10 - 9];
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

extern u8 D_0048BF7D[];         /* our player number */

/* Whether (type, id) is marked by us (unk3: the player + 1). */
s32 func_0064FD50(s32 type, s32 id) {
    Mark *m;
    s32 i;

    for (m = D_0070F9F8, i = 0; i < 0x40; i++, m++) {
        if (m->used == 1 && m->type == type && m->id == id && m->unk3 == D_0048BF7D[0] + 1) {
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

void func_0064E600(s32 i, s32 take, s16 value);
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
    u8 unk260[0x3A4 - 0x260];
    u16 rooms[0x40][6];     /* 0x3A4: per room, [0] its switches done (D_0048AFB4) */
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

void func_0064E940(s8 a, s8 b, s8 c, s16 d);
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
void func_0064BFC0(s32 id, u8 on, s32 player);

void func_0064EB50(void);
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
u8 func_00634640(DrawObjFull *o, s32 a);
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

extern u8 D_0048BFF8[4];        /* our effects as last seen */
extern u8 D_0048BFFC[4];
extern DrawObjFull D_00476DD0[];
void func_0064C620(DrawObjFull *p, s32 what);
void func_00107B50(void *dst, const void *src, s32 n);

/* Once a frame: tells (func_0064C620) when our character's effects change. */
void func_0064FE20(void);
#ifdef NON_MATCHING   /* first draft */
void func_0064FE20(void) {
    DrawObjFull *p = &D_00476DD0[D_0048BF60.unk1D];
    u8 *a;
    u8 *b;
    s32 i;

    if (p->active == 0) {
        return;
    }
    for (i = 0, a = p->unkC84, b = D_0048BFF8; i < 4; i++, a++, b++) {
        if (*a != *b) {
            if (FIELD(&D_0048BF60, u8, 0xA0) == 0) {
                func_0064C620(p, 0xC);
                func_00107B50(D_0048BFF8, p->unkC84, 4);
            }
            break;
        }
    }
    if (p->unkB98 == 6) {
        for (i = 0, a = &p->unkC88[1], b = D_0048BFFC; i < 4; i++, a++, b++) {
            if (*a != *b) {
                if (FIELD(&D_0048BF60, u8, 0xA0) == 0) {
                    func_0064C620(p, 0xF);
                    func_00107B50(D_0048BFFC, &p->unkC88[1], 4);
                }
                break;
            }
        }
    }
    FIELD(&D_0048BF60, u8, 0xA0) = 0;
}
#endif

s32 func_001AF0A0(void);
void func_0064FDD0(u8 player);

/* (Online) tells the others we are leaving the game. */
void func_00650050(void);
#ifdef NON_MATCHING   /* first draft */
void func_00650050(void) {
    struct {
        u8 type;
        u8 len;
    } m;

    if (func_001AF510() == 1) {
        func_0064FDD0(D_0048BF60.unk1D);
        if (func_001AF0A0() == 0) {
            m.type = 5;
            m.len = 4;
            func_001AF000(5, (NetMsg *)&m, 0, 4);
        }
    }
}
#endif

extern u8 D_0070F8B0[0x848];   /* {queued, changes, the message (to 0x148)}, then the marks */
u8 func_0064F7A0(Mark *m);
void func_0055D150(Mark *m, u8 state);
void func_0055D330(Mark *m);

/* Once a frame: sends the marks' changes (and the queued 5-byte messages) to the others. */
void func_0064F8C0(void);
#ifdef NON_MATCHING   /* first draft */
void func_0064F8C0(void) {
    u8 msg[4];
    u8 body[0x10];
    s32 up = func_001AF0A0();
    u8 *q = D_0070F8B0 + 8;
    u8 *out;
    Mark *m;
    s32 i;
    u8 s;

    if (up != 0) {
        for (i = 0, m = D_0070F9F8; i < 0x40; i++, m++) {
            if (m->used != 0) {
                if (m->unk2 == 0) {
                    m->unk2 = 1;
                } else if (m->unk3 == 0xFF) {
                    func_0055D330(m);
                }
            }
        }
        q = D_0070F8B0 + 8;
    }
    while (((s32 *)D_0070F8B0)[0] != 0) {
        msg[0] = 0;
        msg[1] = 9;
        msg[3] = 0;
        msg[2] = D_0048BF60.unk1D | 0x80;
        func_00107B50(body, q, 5);
        func_001AF000(0xA, (NetMsg *)msg, 0, 4);
        q += 5;
        ((s32 *)D_0070F8B0)[0]--;
    }
    if (up == 0) {
        return;
    }
    out = D_0070F8B0 + 0xC + ((s32 *)D_0070F8B0)[1] * 2;
    if ((u32)(((s32 *)D_0070F8B0)[1] * 2 + 4) >= 0x141) {
        return;
    }
    for (i = 0, m = D_0070F9F8; i < 0x40; i++, m++) {
        if (m->used == 0) {
            continue;
        }
        s = func_0064F7A0(m);
        if (m->unk3 != s) {
            m->unk3 = s;
            out[0] = i;
            out[1] = s;
            out += 2;
            ((s32 *)D_0070F8B0)[1]++;
            if (s == 0xFF) {
                func_0055D150(m, s);
            }
        }
    }
    if (((s32 *)D_0070F8B0)[1] > 0) {
        D_0070F8B0[8] = 1;
        D_0070F8B0[9] = ((s32 *)D_0070F8B0)[1] * 2 + 4;
        D_0070F8B0[0xB] = 0;
        D_0070F8B0[0xA] = 0xFF;
        func_001AF000(0xA, (NetMsg *)(D_0070F8B0 + 8), 0, 4);
    }
}
#endif

/* The mark (type, id) of a player (-1: none), or -1. */
static s32 func_0064FAF0(Mark *m, s32 type, s32 id, s32 player) {
    u8 p;
    s32 i;

    if (player == -1) {
        p = 0xFF;
    } else {
        p = (u8)player + 1;
    }
    for (i = 0; i < 0x40; i++, m++) {
        if (m->used != 0 && m->type == type && m->id == id && m->unk3 == p) {
            return i;
        }
    }
    return -1;
}

typedef struct {
    s32 n;              /* 0x00: queued */
    s32 changes;        /* 0x04 */
    u8 msg[0x40][5];    /* 0x08 */
} MarkQueue;

/* Queues a 5-byte message about that mark: its index and b[0..3]. */
void func_0064FB70(s32 type, s32 id, u8 *b, s32 player) {
    MarkQueue *q = (MarkQueue *)D_0070F8B0;
    s32 i = func_0064FAF0(D_0070F9F8, type, id, player);

    if (i != -1 && q->n != 0x40) {
        u8 *e = q->msg[q->n];

        e[0] = i & 0x3F;
        e[1] = b[0];
        e[2] = b[1];
        e[3] = b[2];
        e[4] = b[3];
        q->n++;
    }
}

/* Event flag changes, online told to the others; who changed each flag last (0xFF: kept). */
extern u8 D_0048BBE0[];

typedef struct {
    u8 type;
    u8 len;
    u8 unk2[2];
    s16 id;
    s8 on;
    u8 unk7[0x29];
} FlagMsg;

void func_0064BFC0(s32 id, u8 on, s32 player) {
    FlagMsg m;

    if (D_00476DD0[D_0048BF60.unk1D].unkBA0 & 0x5C0) {
        return;
    }
    if (func_001AF510() == 0) {
        D_0048BBE0[id] = player;
        return;
    }
    m.len = 8;
    m.on = on;
    m.type = 0;
    m.id = id;
    func_001AF000(5, (NetMsg *)&m, 0, 4);
    if (D_0048BBE0[id] != 0xFF) {
        D_0048BBE0[id] = player;
    }
}

/* Tells the others enemy i's unk2E (v). */
void func_0064F390(s8 i, s8 v) {
    struct {
        u8 type;
        u8 len;
        u8 unk2[2];
        s8 i;
        s8 v;
        u8 unk6[0x2A];
    } m;

    if (func_001AF510() != 0) {
        m.type = 6;
        m.len = 6;
        m.i = i;
        m.v = v;
        func_001AF000(5, (NetMsg *)&m, 0, 4);
    }
}

/* Whether o's enemy slot (type 2, id unkB98) has a mark of our player's (unk3: player + 1). */
s32 func_0064FF60(DrawObjFull *o) {
    u8 me = D_0048BF60.unk1D;
    s32 found = 0;
    Mark *m = D_0070F9F8;
    u16 i;
    u8 p;

    if (me == -1) {
        p = 0xFF;
    } else {
        p = (me & 0xFF) + 1;
    }
    for (i = 0; i < 0x40; i++, m++) {
        if (m->used != 0 && m->id == o->unkB98 && m->type == 2 && m->unk3 == p) {
            found = 1;
            break;
        }
    }
    return found;
}

/* Each frame: the room's switches being worked (their progress at D_00470090 + 0x4700) move
   on (func_00540A30), and the switches their zones stand for; online, the changes go out in
   one message (up to 10). */
#include "options.h"
typedef struct {
    u8 room;
    u8 bit;
    s16 value;
} SwitchMove;

typedef struct {
    u8 type;            /* 0x0 */
    u8 len;             /* 0x1 */
    u8 unk2[2];
    u8 n;               /* 0x4 */
    u8 unk5[3];
    SwitchMove e[10];   /* 0x8 */
} SwitchMsg;

typedef struct {
    u8 unk0[0x30];
    u8 kind;            /* 0x30 */
    u8 unk31[0x44 - 0x31];
    s32 room;           /* 0x44 */
    s32 bit;            /* 0x48 */
} SwitchZoneRef;

typedef struct {
    u8 unk0[0x4700];
    s16 sw[0x18];           /* 0x4700: per switch, how far it is worked */
} RoomSwitches;
extern RoomSwitches D_00470090;
void func_00540A30(u8 room, u8 bit, s32 value);
void func_005406B0(s32 row, s32 bit, s32 col);
SwitchZoneRef *func_00551820(u8 id);

void func_0064EF20(void) {
    s32 i;
    u32 b;
    f32 f;
    u8 *n;
    SwitchZoneRef *z;
    SwitchMsg m;

    n = &m.n;
    for (i = 0, *n = 0; i < 0x18; i++) {
        f = D_00470090.sw[i];
        if (f < 1.0f) {
            continue;
        }
        b = 1 << i;
        if (!(D_0048AC10.rooms[D_00306590.unk1C][0] & b)) {
            func_00540A30(D_00306590.unk1C, i, (s32)f);
            if (!(D_0048AC10.rooms[D_00306590.unk1C][0] & b)) {
                m.e[*n].value = (s32)f;
                m.e[*n].bit = i;
                m.e[*n].room = D_00306590.unk1C;
                (*n)++;
                if (*n >= 10) {
                    break;
                }
            } else {
                func_005406B0(D_00306590.unk1C, i, 2);
            }
        }
        if (i < 0x10 && (z = func_00551820(i)) != NULL && z->kind == 0 &&
            !(D_0048AC10.rooms[z->room][0] & (1 << z->bit))) {
            func_00540A30(z->room, z->bit, (s32)f);
            if (!(D_0048AC10.rooms[z->room][0] & (1 << z->bit))) {
                m.e[*n].value = (s32)f;
                m.e[*n].bit = z->bit;
                m.e[*n].room = z->room;
                (*n)++;
                if (*n >= 10) {
                    break;
                }
            } else {
                func_005406B0(z->room, z->bit, 2);
            }
        }
    }
    if (*n != 0) {
        m.type = 4;
        m.len = 0x30;
        if (func_001AF510() == 1) {
            func_001AF000(5, (NetMsg *)&m, 0, 2);
        }
    }
}

/* Which net player (+ 1, 0xFF: none) has the room a mark's thing is in: a player's (type 0),
   a D_003614F0 record's (1) or an enemy set entry's (2). */
typedef struct {
    u8 unk0;
    u8 room;            /* 0x01 */
    u8 unk2[0x78 - 2];
} Rec3614F0;

typedef struct {
    u8 unk0[0x22];
    u8 room;            /* 0x22 */
    u8 unk23[0x60 - 0x23];
} EnemySetRoom;

extern Rec3614F0 D_003614F0[];
extern EnemySetRoom D_0071A030[];
extern DrawObjFull D_00476DD0[];
s32 func_001AF440(u16 room);

u8 func_0064F7A0(Mark *m) {
    Rec3614F0 *rec;
    EnemySetRoom *es;
    DrawObjFull *o;
    s32 r;
    u8 v;

    switch (m->type) {
    case 1:
        rec = &D_003614F0[m->id];
        r = func_001AF440(rec->room);
        if (r == -1) {
            v = 0xFF;
        } else {
            v = r + 1;
        }
        return v;
    case 2:
        es = &D_0071A030[m->id];
        r = func_001AF440(es->room);
        if (r == -1) {
            v = 0xFF;
        } else {
            v = r + 1;
        }
        return v;
    case 0:
        o = &D_00476DD0[m->id];
        r = func_001AF440(o->unk1E2);
        if (r == -1) {
            return 0xFF;
        }
        return r + 1;
    default:
        return 0;
    }
}

/* A mark's thing gets ... by its type (0 a player: single's func_00825DA0, 1 a D_003614F0
   record, 2 an enemy). */
void func_0055D3A0(Mark *m);
void func_0055D510(Mark *m);
void func_00825DA0(Mark *m);

void func_0055D330(Mark *m) {
    switch (m->type) {
    case 1:
        func_0055D3A0(m);
        break;
    case 2:
        func_0055D510(m);
        break;
    case 0:
        func_00825DA0(m);
        break;
    }
}

void func_005CCE40(s8 room, s8 a, u8 *out);
void func_00576B30(EnemySetRoom *e, u8 a, u8 b);
void func_0055D5F0(EnemySetRoom *e, Mark *m, u8 a, s32 b);

/* An enemy mark's step: 1 after its wait, 3 ... */
#ifdef NON_MATCHING   /* argument load order of r vs m->unk8 in the func_00576B30 call */
void func_0055D510(Mark *m) {
    EnemySetRoom *e = &D_0071A030[m->id];
    u8 r;

    switch (m->state) {
    case 0:
        break;
    case 1:
        switch (m->unk7) {
        case 1:
            if (m->unk12 <= 0) {
                func_005CCE40((s8)e->room, m->unk8, &r);
                func_00576B30(e, m->unk8, r);
                m->state = 0;
            } else {
                m->unk12--;
            }
            break;
        }
        break;
    case 2:
        break;
    case 3:
        func_0055D5F0(e, m, m->unk7, 1);
        break;
    }
}
#endif
