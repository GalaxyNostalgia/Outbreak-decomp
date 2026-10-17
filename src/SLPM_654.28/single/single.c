#include "common.h"
#include "game.h"

/* SINGLE.BIN: the offline game's code (loaded at 0x820000; NETAQ.BIN takes its place online). */

/* What the offline script has for each room. */
typedef struct {
    s32 used;           /* 0x00 */
    s32 entry;          /* 0x04 */
    s32 unk8;
    u8 *unkC;
    u8 *unk10;
    u8 unk14[0x94 - 0x14];
    s32 *unk94;         /* 0x94 */
    s32 unk98;
    u8 *unk9C;
    u8 *unkA0;
} SglRoom;              /* 0xA4 */

extern SglRoom D_0084E2B0[0x80];
extern u32 D_0084E2A0[4];

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
    u8 unk38[8];        /* 0x38: per player: in the session */
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
    s16 to;             /* 0x00: 1-9 */
    u8 kind;            /* 0x02 */
    u8 len;             /* 0x03 */
    s32 time;           /* 0x04: the sender's frame */
    u16 from;           /* 0x08 */
    u8 type;            /* 0x0A */
    u8 flags;           /* 0x0B: 0x80 to everyone */
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
            memcpy(D_0083B960, D_0083BB60, 0x200);
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
        pp->unk0 = r->time;
        pp->unk4 = r->from;
        pp->unk6 = r->type;
        pp->unk7 = r->flags;
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
        pp->unk0 = r->time;
        pp->unk4 = r->from;
        pp->unk6 = r->type;
        pp->unk7 = r->flags;
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
        memcpy(m.data, q->buf, q->count);
        func_00820940(q->id, &m, 0, 4);
        if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
            func_001B2600(q->id % 4 * 6 + 0xD, q->id / 4 + 0xC, 0xFFFF0000, D_0083B188);
        }
    }
    for (j = 0; j < 4; j++) {
        D_0083B930[j].end = D_0083B930[j].buf;
        D_0083B930[j].count = 0;
        D_0083B930[j].id = -1;
    }
}
#endif

void func_00820820(void) {
    if (D_0084E040.unk4 != 0) {
        func_008215A0();
        D_0084E040.unk40 = D_0084E040.unk28;
    }
}

/* The offline script (r%03d.sgl, loaded at D_008534C0): word 2 is the offset of a table, a count
   and then 12-byte entries; each entry's block has 16 offsets made pointers when it is loaded. */

typedef struct {
    s16 a;
    s16 b;
    s32 off;            /* 0x04: the entry's block, from the start of the file */
    s32 unk8;
} SglEntry;

extern s32 D_008534C0[];

static s32 func_008260C0(void);
static SglEntry *func_008260E0(s32 i);
static s32 func_008261E0(s32 i);
static u8 *func_00826210(s32 i);
static u8 *func_00826240(s32 i);
static s32 *func_00826370(s32 i);
static s32 func_00826280(s32 i);
static u8 *func_008262D0(s32 i);
static u8 *func_00826320(s32 i);
static s32 func_00826000(void);
static s32 *func_00826020(void);
static u8 *func_00826040(void);
static u8 *func_00826080(void);

typedef struct {
    u8 unk0[0x2A0];
    u8 roomVariant[0x80];   /* 0x2A0 */
} SceneData2A0;

extern SceneData2A0 D_0048AC10;
extern u8 D_00476DD0[];
void func_006CCDE0(s16 room);
void func_006CCD20(void);
void func_006CCDA0(void *players);

void func_00825E30(s32 mode);
#ifdef NON_MATCHING   /* first draft */
/* Sets up the offline script's rooms (mode 2: without starting them). */
void func_00825E30(s32 mode) {
    s32 n;
    s32 i;
    SglEntry *e;
    SglRoom *r;

    func_00825DF0();
    n = func_008260C0();
    for (i = 0; i < n; i++) {
        e = func_008260E0(i);
        if (e == NULL || D_0048AC10.roomVariant[e->a] != e->b) {
            continue;
        }
        r = &D_0084E2B0[e->a];
        r->used = 1;
        r->entry = i;
        r->unk8 = func_008261E0(i);
        r->unkC = func_00826210(i);
        r->unk10 = func_00826240(i);
        r->unk94 = func_00826370(i);
        r->unk98 = func_00826280(i);
        r->unk9C = func_008262D0(i);
        r->unkA0 = func_00826320(i);
        if (mode != 2) {
            func_006CCDE0(e->a);
        }
    }
    D_0084E2A0[0] = func_00826000();
    D_0084E2A0[1] = (u32)func_00826020();
    D_0084E2A0[2] = (u32)func_00826040();
    D_0084E2A0[3] = (u32)func_00826080();
    switch (mode) {
    case 0:
        func_006CCD20();
        break;
    case 1:
        func_006CCDA0(D_00476DD0);
        break;
    }
}
#endif


static s32 *func_00825FE0(void) {
    return (s32 *)((u8 *)D_008534C0 + D_008534C0[2]);
}

static s32 func_008260C0(void) {
    return *func_00825FE0();
}

static SglEntry *func_008260E0(s32 i) {
    if (func_008260C0() < i) {
        return NULL;
    }
    return (SglEntry *)(func_00825FE0() + 1) + i;
}

static u8 *func_008261A0(s32 i) {
    SglEntry *e = func_008260E0(i);

    if (e == NULL) {
        return NULL;
    }
    return (u8 *)D_008534C0 + e->off;
}

static s32 func_008261E0(s32 i) {
    s32 *p = (s32 *)func_008261A0(i);

    if (p == NULL) {
        return 0;
    }
    return p[2];
}

static s32 *func_00826370(s32 i) {
    s32 *p = (s32 *)func_008261A0(i);

    if (p == NULL) {
        return NULL;
    }
    return (s32 *)((u8 *)p + p[1]);
}

void func_008263B0(void) {
    s32 i;
    s32 j;
    s32 n;
    s32 *p;

    n = func_008260C0();
    for (i = 0; i < n; i++) {
        p = func_00826370(i);
        for (j = 0; j < 0x10; j++) {
            if (p[j] != 0) {
                p[j] += (s32)p;
            }
        }
    }
}

static s32 *func_00825FC0(void) {
    return (s32 *)((u8 *)D_008534C0 + D_008534C0[1]);
}

static s32 func_00826000(void) {
    return *func_00825FC0();
}

static s32 *func_00826020(void) {
    return func_00825FC0() + 1;
}

static u8 *func_00826040(void) {
    s32 n = func_00826000();

    return (u8 *)func_00826020() + n * 12;
}

static u8 *func_00826080(void) {
    s32 n = func_00826000();

    return func_00826040() + ((n * n + 3) & ~3);
}

static u8 *func_00826210(s32 i) {
    u8 *p = func_008261A0(i);

    if (p == NULL) {
        return NULL;
    }
    return p + 0xC;
}

static u8 *func_00826240(s32 i) {
    s32 k = func_008261E0(i);
    u8 *p = func_00826210(i);

    if (p == NULL) {
        return NULL;
    }
    return p + k * 64;
}

#ifdef NON_MATCHING   /* first draft */
static s32 func_00826280(s32 i) {
    s32 *p = (s32 *)func_008261A0(i);

    if (p == NULL) {
        return 0;
    }
    return p[p[0] / 4 + 1];
}
#endif

static u8 *func_008262D0(s32 i) {
    s32 *p = (s32 *)func_008261A0(i);

    if (p == NULL) {
        return NULL;
    }
    return (u8 *)(p + p[0] / 4 + 2);
}

static u8 *func_00826320(s32 i) {
    s32 *p = (s32 *)func_008261A0(i);

    if (p == NULL) {
        return NULL;
    }
    return (u8 *)(p + p[0] / 4);
}


#ifdef NON_MATCHING   /* first draft */
/* Queues a message for its receiver (offline, everything stays in this machine). */
void func_00820860(MsgRec *m) {
    MsgQueue *q;
    MsgRec *d;
    u32 i;
    u8 *src;
    u8 *dst;

    if (m->to == 0) {
        return;
    }
    if (m->flags == 0) {
        q = &D_0084DFD0[m->to - 1];
    } else {
        q = &D_0084DF60[m->to - 1];
    }
    d = (MsgRec *)q->end;
    d->to = m->to;
    d->kind = m->kind;
    d->len = m->len;
    d->time = m->time;
    d->from = m->from;
    d->type = m->type;
    d->flags = m->flags;
    src = m->data;
    dst = d->data;
    for (i = 0; i < 0xF4; i++) {
        *dst++ = *src++;
    }
    q->count++;
    q->end += 0x100;
}
#endif

extern char D_0083B0B8[];

#ifdef NON_MATCHING   /* first draft */
/* Sends a message: kind 0 to one receiver, 1 to everyone. */
void func_00820940(s16 to, Msg *msg, s32 kind, s32 a3) {
    MsgRec r;
    MsgRec *p;

    if (to <= 0 || to >= 10) {
        return;
    }
    if (msg->len > 0xF0) {
        func_001B2600(0xD, 5, 0xFFFF0000, D_0083B0B8);
        return;
    }
    r.kind = a3;
    p = &r;
    r.len = msg->len;
    r.time = D_0084E040.frames;
    r.type = msg->type;
    switch (kind) {
    case 0:
        p->to = to;
        p->from = p->to;
        p->flags = 0;
        break;
    case 1:
        p->to = 7;
        p->from = D_0048BF60.unk1D + 1;
        p->flags = 0x80;
        break;
    }
    memcpy(p->data, msg, p->len);
    D_0084E040.unk24++;
    D_0084E040.unk28 += p->len + 0x10;
    func_00820860(p);
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Sends the scenario flags that changed this frame. */
void func_00821760(void) {
    struct {
        Msg m;
    } buf;
    u8 *out;
    s32 n;
    s32 i;
    s32 j;
    u8 *a;
    u8 *b;

    if (D_0084E040.unk5 == 0) {
        return;
    }
    buf.m.type = 4;
    buf.m.len = 0;
    buf.m.data[0] = 2;
    out = &buf.m.data[2];
    buf.m.idx = D_0048BF60.unk1D;
    n = 0;
    buf.m.unk3 = 4;
    buf.m.data[1] = 0;
    for (i = 0; i < 0x80; i++) {
        a = D_0083BB60[i];
        b = D_0083B960[i];
        for (j = 0; j < 4; j++, a++, b++) {
            if (*a != *b) {
                out[0] = i;
                n++;
                out[1] = D_0083B960[i][0];
                out[2] = D_0083B960[i][1];
                out[3] = D_0083B960[i][2];
                out[4] = D_0083B960[i][3];
                out += 5;
                memcpy(D_0083BB60[i], D_0083B960[i], 4);
                break;
            }
        }
    }
    if (n == 0) {
        return;
    }
    if (n >= 9) {
        while (1) {
        }
    }
    buf.m.len = n * 5 + 6;
    func_00820940(7, &buf.m, 1, 4);
}
#endif

s32 func_00821750(void) {
    return D_0084E040.unk5;
}

s32 func_00821D00(s32 i) {
    return D_0084E040.unk38[i];
}

/* Which player is in room (-1: nobody). */
s32 func_00821D20(s32 room) {
    u8 p;

    if (room >= 0x80) {
        return -1;
    }
    p = D_0083BD60[room][0];
    if (p == 0xFF) {
        return -1;
    }
    return p;
}

extern char D_0083B170[];
extern char D_0083B190[];
void func_00107B50(void *dst, const void *src, s32 n);
void func_00821390(s16 to, Msg *m, s32 kind);

/* Sends message m to 'to' (1-9; 10: 8): to players 1-4 (kind 0) it's queued, else sent now
   (when there's room). */
void func_00821170(s32 to, Msg *m, s32 kind, s32 a3);
#ifdef NON_MATCHING   /* first draft */
void func_00821170(s32 to, Msg *m, s32 kind, s32 a3) {
    SingleWork *w = &D_0084E040;
    s32 n;
    s16 c;

    if (D_0084E040.unk4 == 0) {
        return;
    }
    if (w->frames == 0 && to != 6 && to != 7) {
        return;
    }
    if (to == 10) {
        to = 8;
    } else {
        if (to <= 0 || to >= 10) {
            return;
        }
        m->idx = D_0048BF60.unk1D;
    }
    if (to > 0 && to < 5 && kind == 0) {
        func_00821390(to, m, kind);
        return;
    }
    if (w->unk3 < 2) {
        if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
            func_001B2600(0xF, 3, 0xFFFF0000, D_0083B150, w->unk3);
        }
        return;
    }
    c = D_0083B930[D_0048BF60.unk1D].count;
    n = w->unk28;
    if (c != 0) {
        n += c + 0x10;
    }
    if (n >= 0x200) {
        if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
            func_001B2600(0xF, 3, 0xFFFF0000, D_0083B170, n);
        }
        return;
    }
    func_00820940(to, m, kind, a3);
    if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
        func_001B2600(to % 4 * 6 + 0xD, to / 4 + 0xC, 0xFFFF0000, D_0083B188, to);
    }
}
#endif

/* Queues m for player 'to' (1-4), in 4-byte steps (another player's queue: sent now). */
#ifdef NON_MATCHING   /* first draft */
void func_00821390(s16 to, Msg *m, s32 kind) {
    MsgQueue *q;
    u8 len;
    s32 n;
    s32 total;
    s16 c;

    if (to == 0) {
        return;
    }
    len = m->len;
    q = &D_0083B930[to - 1];
    if (len == 0) {
        return;
    }
    n = (len + 3) / 4 * 4;
    if (q->id != -1 && q->id != to) {
        func_00820940(to, m, kind, 4);
        if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
            func_001B2600(to % 4 * 6 + 0xD, to / 4 + 0xC, 0xFFFF0000, D_0083B188, to);
        }
    }
    total = D_0084E040.unk28;
    if (q->count != 0) {
        total += q->count + 0x10;
    }
    if (total >= 0x200) {
        if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
            func_001B2600(0xF, 3, 0xFFFF0000, D_0083B170, total);
        }
        return;
    }
    q->id = to;
    c = q->count + n;
    if (c >= 0x6C) {
        if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
            func_001B2600(0xF, 3, 0xFFFF0000, D_0083B190, c);
        }
        return;
    }
    q->count = c;
    func_00107B50(q->end, m, len);
    q->end += n;
}
#endif

extern char D_0083B0C0[];       /* "RS %d" */
extern s8 D_0084E045;

/* Player p (0 only, as written) takes (take) or tells the others it would take room object obj:
   offline, each object's four takers are kept in D_0083B960, a player in one row at most. */
void func_00821950(s32 p, s32 obj, s32 take);
#ifdef NON_MATCHING   /* first draft */
void func_00821950(s32 p, s32 obj, s32 take) {
    u8 m[8];
    u8 *row;
    u8 *q;
    s32 i;
    s32 k;
    s32 r;
    u16 v;

    if (p != 0 || p >= 4) {
        return;
    }
    if (take == 0) {
        v = FIELD(&D_00476DD0[p * 0x10E0], u16, 0x1E2);
        if (v >= 0x80 || p != D_0048BF60.unk1D) {
            return;
        }
        m[6] = v;
        m[0] = 4;
        m[1] = 8;
        m[2] = D_0048BF60.unk1D;
        m[5] = D_0048BF60.unk1D;
        m[3] = 4;
        m[4] = 0;
        m[7] = 0;
        func_00820940(7, (Msg *)m, 0, 4);
        return;
    }
    if (D_0084E045 == 0) {
        return;
    }
    row = D_0083B960[obj];
    for (i = 0, q = row; i < 4; i++, q++) {
        if (*q == (u8)p) {
            return;
        }
    }
    for (i = 0; i < 4; i++, row++) {
        if (*row == 0xFF) {
            *row = p;
            break;
        }
    }
    for (k = 0; k < 0x200; k++) {
        r = k / 4;
        if (r != obj && D_0083B960[0][k] == (u8)p) {
            q = D_0083B960[r];
            for (i = 0; i < 4; i++, q++) {
                if (*q == (u8)p) {
                    break;
                }
            }
            for (; i < 3; i++, q++) {
                q[0] = q[1];
            }
            *q = 0xFF;
        }
    }
}
#endif

extern s32 D_0084E048;
extern s16 D_0023BBA0;
extern u8 D_0023BB28;
void func_001B27B0(s32 x, s32 line, const char *fmt, ...);
void func_0064C130(u8 *m, u8 from);
void func_00822090(u8 a, u8 *p);
void func_0064E590(u8 a, u8 b, u8 c);
void func_00821FE0(u8 *m, u8 from);
void func_00821B10(u8 a, u8 b, s32 c);
void func_0064FCB0(u8 n, u8 *m, s32 a);
void func_00609590(s32 n, u8 *m, s32 a);
void func_006531C0(u8 *m, s32 a);

/* A message of kind n received (m[2]: who from). */
#ifdef NON_MATCHING   /* first draft */
void func_00820AC0(s32 n, u8 *m) {
    u8 from;
    u8 bit;

    if (func_001B6980(0) != 0 && func_001B6980(0x68) != 0) {
        func_001B27B0(n % 4 * 6 + 0xD, n / 4 + 7, D_0083B0C0, n);
    }
    if (n < 0 || (u32)n >= 10) {
        return;
    }
    from = m[2];
    switch (n) {
    case 5:
        if (D_0048BF60.unkA6 == 0 && from != D_0048BF60.unk1D) {
            func_0064C130(m, from);
        }
        break;
    case 6:
        if (D_0048BF60.state == 7) {
            switch (m[0]) {
            case 0:
                if (from != D_0048BF60.unk1D) {
                    D_0048BF60.unk20[0] = m[4];
                    D_0048BF60.unk2F = m[5];
                    D_0023BBA0 = m[6] | (m[7] << 8);
                }
                break;
            case 1:
                if (D_0084E045 == 0) {
                    func_00822090(m[4], m + 5);
                }
                break;
            case 2:
                D_0048BF60.unk62 = m[4];
                break;
            case 3:
                D_0048BF60.unkB7 = 1;
                break;
            case 5:
                if (from != D_0048BF60.unk1D) {
                    D_0048BF60.unkBA = m[4];
                    D_0048BF60.unkBB = 1;
                }
                break;
            case 6:
                if (from != D_0048BF60.unk1D) {
                    D_0023BB28 = m[4];
                    D_0048BF60.unkBB = 1;
                }
                break;
            }
        } else if (m[0] == 4) {
            bit = 1 << m[4];
            if (!(D_0048BF60.unkB6 & bit)) {
                D_0048BF60.unkB6 |= bit;
            }
        }
        break;
    case 7:
        switch (m[0]) {
        case 0:
            func_0064E590(m[4], m[5], m[6]);
            break;
        case 1:
            if (D_0084E045 == 0 && m[8] == D_0048BF60.unk1D) {
                D_0084E048 = *(s32 *)(m + 4);
            }
            break;
        case 2:
            func_00821FE0(m, from);
            break;
        case 3:
            m[0] = m[3];
            func_0064C130(m, from);
            break;
        case 4:
            switch (m[4]) {
            case 0:
                func_00821950(m[5], m[6], 1);
                break;
            case 1:
                func_00821B10(m[5], m[6], 1);
                break;
            case 2:
                break;
            }
            break;
        }
        break;
    case 8:
        if (from & 0x80) {
            func_0064FCB0(n, m, 0);
        } else if (D_0048BF60.unkA6 == 0) {
            func_00609590(n, m, 0);
        }
        break;
    case 9:
        if (D_0048BF60.unkA6 == 0) {
            func_006531C0(m, 0);
        }
        break;
    }
}
#endif
