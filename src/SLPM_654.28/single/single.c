#include "common.h"
#include "game.h"

/* SINGLE.BIN: the offline game's code (loaded at 0x820000; NETAQ.BIN takes its place online). */

extern u8 D_0084E2B0[0x5200];
extern u8 D_0084E2A0[0x10];

void func_00825DF0(void) {
    memset(D_0084E2B0, 0, sizeof(D_0084E2B0));
    memset(D_0084E2A0, 0, sizeof(D_0084E2A0));
}

/* The offline mode's state (it stands in for the online one's session). */
typedef struct {
    u8 state;           /* 0x00 */
    u8 unk1[2];
    s8 unk3;            /* 0x03: session state (5 once running) */
    s8 unk4;            /* 0x04: running */
    s8 unk5;            /* 0x05: show the flag table */
    u8 unk6;
    u8 unk7;            /* 0x07: our player number */
    s32 frames;         /* 0x08 */
    u32 frames2;        /* 0x0C */
    u8 unk10[0x24 - 0x10];
    s32 unk24;
    s32 unk28;
    u8 unk2C[0x38 - 0x2C];
    u8 unk38[8];        /* 0x38: per player */
    s32 unk40;
    s32 unk44;
    u32 unk48;          /* 0x48: time of the last minute */
    u32 unk4C;          /* 0x4C: time now */
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
} SingleWork;

extern SingleWork D_0084E040;

/* A queue of 0x100-byte messages (offline, these stand in for the network's). */
typedef struct {
    s16 id;
    s16 count;
    u8 *buf;
    u8 *end;            /* where the next message goes */
} MsgQueue;

typedef struct {
    u8 unk0[4];
    s32 unk4;
    u16 unk8;
    u8 unkA;
    u8 unkB;
    u8 data[0xF4];
} MsgRec;

typedef struct {
    s32 unk0;
    u16 unk4;
    u8 unk6;
    u8 unk7;
    u8 data[0xF4];
} Packet;

extern u8 D_0083BD60[0x80][4];   /* scenario flags */
extern u8 D_0083BB60[0x80][4];   /* as sent last */
extern u8 D_0083B960[0x80][4];
extern u8 *D_00838800[10];
extern u8 *D_00838830[10];
extern MsgQueue D_0084DFD0[10];
extern MsgQueue D_0084DF60[10];
extern MsgQueue D_0083B930[4];
extern u8 D_0083B780[4][0x6C];

void func_00820080(void) {
    s32 i;
    u8 **s;
    MsgQueue *p;
    u8 *t;
    MsgQueue *q;

    memset(&D_0084E040, 0, sizeof(D_0084E040));
    memset(D_0083BD60, 0xFF, sizeof(D_0083BD60));
    memset(D_0083BB60, 0xFF, sizeof(D_0083BB60));
    memset(D_0083B960, 0xFF, sizeof(D_0083B960));
    s = D_00838800;
    p = D_0084DFD0;
    for (i = 0; i < 10; i++, s++) {
        t = *s;
        if (t != NULL) {
            p->end = t;
            p->buf = t;
            p->count = 0;
            p->id = i;
            p++;
        }
    }
    s = D_00838830;
    p = D_0084DF60;
    for (i = 0; i < 10; i++, s++) {
        t = *s;
        if (t != NULL) {
            p->end = t;
            p->buf = t;
            p->count = 0;
            p->id = i;
            p++;
        }
    }
    for (i = 0; i < 4; i++) {
        q = &D_0083B930[i];
        q->buf = q->end = D_0083B780[i];
        q->count = 0;
        q->id = -1;
    }
}

#include "options.h"

void func_008201D0(void) {
    func_00820080();
    D_00306590.unk3A = 1;
}

extern s32 D_0023C25C;
extern f32 D_0023C260;
extern char D_0083B080[];
extern char D_0083B090[];
extern char D_0083B0A8[];

s32 func_001B6980(s32 slot);
void func_001B27B0(s32 x, s32 line, const char *fmt, ...);
void func_001B2600(s32 x, s32 line, s32 color, const char *fmt, ...);
u32 func_001AB8E0(void);
void func_00107B50(void *dst, void *src, s32 n);
s32 func_00820530(void);
void func_008207B0(s32 which);
void func_00821760(void);
void func_0064EE00(void);
void func_0064E9F0(void);

/* Once a frame while offline. */
void func_00820200(void) {
    SingleWork *w;

    if (D_00306590.unk10[2] != 0) {
        return;
    }
    w = &D_0084E040;
    if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
        func_001B27B0(0x1E, 1, D_0083B080, D_0084E040.unk58, D_0084E040.unk5C);
        if (D_0023C25C < w->unk28) {
            D_0023C25C = w->unk28;
        }
        func_001B2600(0xF, 4, -1, D_0083B090, D_00306590.unk10[2], w->unk28, D_0023C25C);
        func_001B2600(0x26, 3, 0xFFFF0000, D_0083B0A8, D_0083BD60[D_00306590.unk1C][0],
                      D_0083BD60[D_00306590.unk1C][1], D_0083BD60[D_00306590.unk1C][2],
                      D_0083BD60[D_00306590.unk1C][3]);
        if (D_0084E040.unk5 != 0) {
            func_001B2600(0x26, 4, 0xFFFF0000, D_0083B0A8, D_0083BB60[D_00306590.unk1C][0],
                          D_0083BB60[D_00306590.unk1C][1], D_0083BB60[D_00306590.unk1C][2],
                          D_0083BB60[D_00306590.unk1C][3]);
        }
    }
    switch (w->state) {
    case 0:
        w->state++;
        break;
    case 1:
        func_00820530();
        break;
    case 2:
        if (D_0084E040.unk4 != 0) {
            w->unk4C = func_001AB8E0();
            w->unk50 += w->unk40;
            w->unk54 += w->unk44;
            w->unk40 = 0;
            w->unk44 = 0;
            if (w->unk4C - w->unk48 >= 3600) {
                w->unk48 = w->unk4C;
                w->unk58 = w->unk50;
                w->unk5C = w->unk54;
                w->unk50 = 0;
                w->unk54 = 0;
            }
            D_0023C260 += 1.0f;
            w->frames2 = D_0023C260;
            w->frames++;
            w->unk24 = 0;
            w->unk28 = 0;
        }
        if (D_00306590.unk10[2] == 0) {
            func_00107B50(D_0083B960, D_0083BB60, 0x200);
            func_008207B0(1);
            func_008207B0(0);
            func_00821760();
        }
        if (D_0048BF60.state != 0 && D_0048BF60.state != 7) {
            func_0064EE00();
            func_0064E9F0();
        }
        break;
    case 3:
        func_00820080();
        D_0048BF60.unk29 = 1;
        break;
    }
}

extern u32 D_0023BE14;          /* players */

#ifdef NON_MATCHING   /* registers and the copy loop's shape differ */
void func_00820A60(void) {
    u32 i;

    for (i = 0; i < D_0023BE14; i++) {
        D_0084E040.unk38[i] = 1;
    }
}
#endif

#ifdef NON_MATCHING   /* registers and the copy loop's shape differ */
s32 func_00820530(void) {
    D_0084E040.unk7 = 0;
    D_0084E040.unk4 = 1;
    D_0084E040.unk5 = 1;
    D_0084E040.unk3 = 5;
    D_0084E040.state++;
    D_0084E040.unk48 = func_001AB8E0();
    D_00306590.unk3B = 2;
    D_0048BF60.unk1D = D_0084E040.unk7;
    func_00820A60();
    return 1;
}
#endif

static inline void copy_bytes(u8 *d, u8 *src, u32 n) {
    u32 i;

    for (i = 0; i < n; i++) {
        *d++ = *src++;
    }
}

void func_00820EF0(s32 n, Packet *pkt);
void func_00820AC0(s32 n, u8 *data);

#ifdef NON_MATCHING   /* registers and the copy loop's shape differ */
void func_008205B0(s32 n) {
    MsgQueue *q;
    MsgRec *r;
    Packet pkt;
    Packet *pp;

    if (n == 0) {
        return;
    }
    q = &D_0084DF60[n - 1];
    r = (MsgRec *)q->buf;
    pp = &pkt;
    while (q->count != 0) {
        pp->unk0 = r->unk4;
        pp->unk4 = r->unk8;
        pp->unk6 = r->unkA;
        pp->unk7 = r->unkB;
        copy_bytes(pp->data, r->data, 0xF4);
        func_00820EF0(n, pp);
        r++;
        q->count--;
        if ((s32)r >= (s32)q->end) {
            q->count = 0;
        }
    }
    q->end = q->buf;
}
#endif

#ifdef NON_MATCHING   /* registers and the copy loop's shape differ */
void func_008206B0(s32 n) {
    MsgQueue *q;
    MsgRec *r;
    Packet pkt;
    Packet *pp;

    if (n == 0) {
        return;
    }
    q = &D_0084DFD0[n - 1];
    r = (MsgRec *)q->buf;
    pp = &pkt;
    while (q->count != 0) {
        pp->unk0 = r->unk4;
        pp->unk4 = r->unk8;
        pp->unk6 = r->unkA;
        pp->unk7 = r->unkB;
        copy_bytes(pp->data, r->data, 0xF4);
        func_00820AC0(n, pp->data);
        r++;
        q->count--;
        if ((s32)r >= (s32)q->end) {
            q->count = 0;
        }
    }
    q->end = q->buf;
}
#endif

void func_008207B0(s32 which) {
    s32 i;

    if (which == 1) {
        for (i = 1; i < 10; i++) {
            func_008205B0(i);
        }
    } else {
        for (i = 1; i < 10; i++) {
            func_008206B0(i);
        }
    }
}

/* A message: a 4-byte header and up to 0x6C bytes. */
typedef struct {
    u8 type;
    u8 len;
    s8 idx;
    u8 unk3;
    u8 data[0x6C];
} Msg;

void func_00820940(s16 to, Msg *m, s32 kind, s32 a3);
extern char D_0083B150[];
extern char D_0083B188[];

#ifdef NON_MATCHING   /* registers differ */
/* Sends what the four outgoing buffers hold. */
void func_008215A0(void) {
    s32 i;
    s32 j;
    MsgQueue *q;
    Msg m;

    for (i = 0; i < 4; i++) {
        q = &D_0083B930[i];
        if (q->id == -1 || q->count == 0) {
            continue;
        }
        if (D_0084E040.unk3 < 2) {
            if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
                func_001B2600(0xF, 3, 0xFFFF0000, D_0083B150, D_0084E040.unk3);
            }
            break;
        }
        m.type = 0;
        m.len = (u8)q->count + 4;
        m.idx = i;
        m.unk3 = 0;
        func_00107B50(m.data, q->buf, q->count);
        func_00820940(q->id, &m, 0, 4);
        if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
            func_001B2600(q->id % 4 * 6 + 0xD, q->id / 4 + 0xC, 0xFFFF0000, D_0083B188);
        }
    }
    for (j = 0; j < 4; j++) {
        D_0083B930[j].end = D_0083B930[i].buf;
        D_0083B930[i].count = 0;
        D_0083B930[i].id = -1;
    }
}
#endif

void func_00820820(void) {
    if (D_0084E040.unk4 != 0) {
        func_008215A0();
        D_0084E040.unk40 = D_0084E040.unk28;
    }
}
