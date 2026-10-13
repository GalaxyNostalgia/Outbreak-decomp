#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* GAME.BIN: setting up a game from what the select screens chose (D_002F88B0). */

extern u8 D_002F88B0[0x30];
extern DrawObjFull D_00476DD0[8];   /* the players */
extern u32 D_0023BE14;              /* characters in the game */
extern s8 D_0023BB28;
s32 func_001AF500(void);
u16 func_001ADEF0(s32 which);

#ifdef NON_MATCHING   /* first draft */
void func_005BA5A0(void) {
    Options *opt = &D_00306590;
    GameState *g = &D_0048BF60;
    u8 *sel = D_002F88B0;
    DrawObjFull *o;
    s32 i;

    if (func_001AF500() == 0) {
        g->unk62 = 1;
    }
    o = D_00476DD0;
    g->unkC6 = sel[1];
    opt->unk1A = sel[3];
    for (i = 0; i < sel[4]; i++, o++) {
        o->unkB98 = sel[8 + i];
        o->costume = sel[0xC + i];
        o->unkBB4 = ((s16 *)&sel[0x10])[i];
        o->isNpc = 0;
    }
    for (i = 0; i < sel[5]; i++, o++) {
        o->unkB98 = sel[0x18 + i];
        o->costume = sel[0x1A + i];
        o->unkBB4 = ((s16 *)&sel[0x1C])[i];
        o->isNpc = 1;
    }
    D_0023BE14 = sel[4] + sel[5];
    g->unkBF = D_0023BE14;
    g->unkBA = sel[2];
    D_0023BB28 = func_001ADEF0(3) & 3;
}
#endif

extern u8 D_00718B40[0x980];
extern u8 D_0023BF78[2];
extern s32 D_00718AC0[2][0x10];

void func_00685470(void) {
    s32 i;
    s32 j;

    memset(D_00718B40, 0, sizeof(D_00718B40));
    memset(D_0023BF78, 0, sizeof(D_0023BF78));
    for (i = 0; i < 2; i++) {
        for (j = 0xF; j >= 0; j--) {
            D_00718AC0[i][j] = 0;
        }
    }
}

typedef struct {
    s16 a;
    s16 b;
} Pair16;

typedef struct {
    u8 unk0[0xE4];
    s32 unkE4[0x20];
    u8 unk164[0xA3C - 0x164];
} Work7194C0;

extern Work7194C0 D_007194C0;
extern Pair16 D_00717B20[0x3E8];
extern s32 D_0023BF74;
extern s32 D_0023BF70;

typedef struct {
    u8 unk0[0xDB0];
    s32 unkDB0;
    s32 unkDB4;
    s32 unkDB8;
    s16 unkDBC;
    s16 unkDBE;
    s8 unkDC0;
    s8 unkDC1;
    s16 unkDC2;
    s8 unkDC4;
    s8 unkDC5;
    s8 unkDC6;
    s8 unkDC7;
    u8 unkDC8[0x10E0 - 0xDC8];
} PlayerDB0;

#ifdef NON_MATCHING   /* first draft */
void func_00685280(void) {
    s32 i;
    u32 n;
    PlayerDB0 *p;

    memset(&D_007194C0, 0, sizeof(D_007194C0));
    func_00685470();
    for (i = 0x1F; i >= 0; i--) {
        D_007194C0.unkE4[i] = -1;
    }
    for (i = 0; i < 0x3E8; i++) {
        D_00717B20[i].a = -1;
        D_00717B20[i].b = 0;
    }
    D_0023BF74 = 0;
    D_0023BF70 = 0;
    for (n = 0; n < D_0023BE14; n++) {
        p = &((PlayerDB0 *)D_00476DD0)[n];
        p->unkDB4 = 0;
        p->unkDB8 = 0;
        p->unkDBC = 0;
        p->unkDBE = 0;
        p->unkDC0 = 0;
        p->unkDC1 = 0;
        p->unkDC2 = 0;
        p->unkDC4 = 0;
        p->unkDC5 = 0;
        p->unkDC6 = 0;
        p->unkDC7 = 0;
        p->unkDB0 = 0;
    }
}
#endif

extern u8 D_0048BE24[0x40];

void func_0064EF00(void) {
    memset(D_0048BE24, 0, sizeof(D_0048BE24));
}

extern u8 D_0048AE20[0x10];
extern u8 D_0048AE30[0x10];
extern u8 D_0048AE70[0x10];
extern u8 D_0048AE50[0x10];

void func_0064EE90(void) {
    memset(D_0048AE20, 0xFF, sizeof(D_0048AE20));
    memset(D_0048AE30, 0, sizeof(D_0048AE30));
    memset(D_0048AE70, 0, sizeof(D_0048AE70));
    memset(D_0048AE50, -1, sizeof(D_0048AE50));
    func_0064EF00();
}

extern u8 D_0048AEA4[];
extern s8 D_0023BD84;
extern s8 D_0023BD85;
void func_005BA390(void);
s32 func_001AF510(void);

void func_005BA6D0(void) {
    func_005BA390();
    func_005BA5A0();
    func_00685280();
    D_0048BF60.unk36 = 1;
    D_0048AEA4[0] = 0xFF;
    D_0023BD84 = 0;
    D_0023BD85 = 0;
    func_0064EE90();
    func_001AF510();
}

void func_001AB970(u16 scenario);
void func_001D6E30(u16 scenario);
void func_001AA600(s32 scenario);
void func_001AA640(void);
void func_001D7720(void);
void func_00688640(void);
void func_001D6FC0(void);
void func_0034B100(void);
void func_001D7090(void);
s32 func_001B6980(s32 slot);

/* Per-scenario set-up. */
void func_005BA750(void) {
    func_001AB970(D_00306590.unk1A);
    func_001D6E30(D_00306590.unk1A);
    func_001AA600(D_00306590.unk1A);
    func_001AA640();
    func_001D7720();
    func_00688640();
    func_001D6FC0();
    func_0034B100();
    if (func_001B6980(0x58) == 0) {
        func_001D7090();
    }
}

/* The scenario's enemy placements (em_set/e%03d.emd, or sgl_e%03d.emd offline): records of 0x60
   bytes for this difficulty and this game's variant (0-3), copied into D_0071A030 by room. */

typedef struct {
    u8 unk0[2];
    u8 type;            /* 0x02: 0xFF ends the file */
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6[2];
    s32 unk8;
    s32 unkC;
    s32 pos[3];         /* 0x10 */
    u8 room;            /* 0x1C */
    u8 unk1D;
    u8 variant;         /* 0x1E */
    u8 unk1F;
    u8 unk20;
    s8 unk21;
    u8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
    u8 unk26;
    u8 unk27[10];       /* 0x27 */
    u8 unk31;
    u8 unk32;
    u8 hpIndex;         /* 0x33 */
    u8 unk34[6];        /* 0x34 */
    u8 unk3A;
    u8 unk3B[6];        /* 0x3B */
    u8 unk41;
    u8 unk42;
    u8 unk43;
    u8 unk44;
    u8 unk45[3];        /* 0x45 */
    u8 unk48;
    u8 difficulties;    /* 0x49: a bit per difficulty */
    u8 unk4A;
    u8 unk4B;
    u8 unk4C[0x60 - 0x4C];
} EnemyRec;

typedef struct {
    s8 index;           /* 0x00 */
    u8 unk1;
    u8 type;            /* 0x02 */
    u8 unk3;
    u8 unk4;
    u8 unk5;
    u8 unk6[2];
    s32 unk8;
    s32 unkC;
    f32 pos[3];         /* 0x10 */
    s16 hp;             /* 0x1C */
    s16 hpMax;          /* 0x1E */
    u8 unk20;
    u8 unk21;
    u8 room;            /* 0x22 */
    u8 unk23[3];
    u8 unk26;           /* 0x26 */
    s8 unk27;
    u8 unk28;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    u8 unk2C;
    u8 unk2D;
    u8 unk2E;
    u8 unk2F[10];       /* 0x2F */
    s8 unk39;
    s8 unk3A;
    u8 unk3B;
    s16 unk3C;          /* 0x3C */
    u8 unk3E[2];
    s8 unk40;           /* 0x40 */
    u8 unk41;
    s8 unk42;           /* 0x42 */
    u8 unk43;
    u8 unk44;
    s8 unk45;
    u8 unk46;
    u8 unk47;
    u8 unk48[6];        /* 0x48 */
    u8 unk4E[6];        /* 0x4E */
    u8 unk54;
    u8 unk55;
    u8 unk56;
    u8 unk57[3];        /* 0x57 */
    u8 unk5A;
    u8 unk5B;
    u8 unk5C;
    u8 unk5D;
    u8 unk5E[2];
} EnemySlot;            /* 0x60 */

extern EnemySlot D_0071A030[0x100];
extern u8 D_00386310[0x100];
extern s8 D_007200B0[0x80];         /* each room's first slot */
extern s8 D_00720030[0x80];         /* each room's number of slots */
extern s16 D_006F5EC0[][4][8];      /* hit points by type, difficulty and index */
extern s16 D_006F4F40[][4][8];
extern u8 D_0023AFD0[];
extern char D_0070BFB0[];           /* "data/rom/scene/em_set/sgl_e%03d.emd" */
extern char D_0070BFE0[];           /* "data/rom/scene/em_set/e%03d.emd" */
extern s8 D_0048C01E[];
s32 sprintf(char *buf, const char *fmt, ...);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
s32 func_001AF4D0(void);
u8 func_00688C10(void);
void func_0064F690(u8 type, s16 id);
void func_00689DF0(EnemySlot *s);
void func_001B1DE0(void);

#ifdef NON_MATCHING   /* first draft */
void func_00688640(void) {
    char name[0x100];
    s32 *file;
    EnemyRec *r;
    EnemySlot *s;
    s16 i;
    s16 n;
    s8 count;
    u16 room;
    u16 slot8;
    u16 variant;
    u8 diff;
    s16 hp;

    memset(D_0071A030, 0, sizeof(D_0071A030));
    for (i = 0; i < 0x100; i++) {
        D_00386310[i] = 0xFF;
    }
    file = (s32 *)D_00306590.unk4C;
    for (i = 0; i < 0x100; i++) {
        name[i] = 0;
    }
    if (func_001AF4D0() == 1) {
        sprintf(name, D_0070BFB0, D_00306590.unk1A);
    } else {
        sprintf(name, D_0070BFE0, D_00306590.unk1A);
    }
    if (func_001CFBB0(name, file, 0x10001, 0) == 0) {
        return;
    }
    diff = 1 << D_0048BF60.unkBA;
    r = (EnemyRec *)&file[file[0] / 4];
    for (i = 0; i < 0x80; i++) {
        D_007200B0[i] = 0;
        D_00720030[i] = 0;
    }
    n = 0;
    count = 0;
    room = 0xFF;
    slot8 = func_001ADEF0(0) & 7;
    variant = func_001ADEF0(2) & 3;
    if (func_001AF4D0() == 1) {
        variant = D_0048BF60.unkBA & 3;
    }
    for (; r->type != 0xFF; r++) {
        if (r->variant != variant || !(r->difficulties & diff)) {
            continue;
        }
        s = &D_0071A030[n];
        s->index = count;
        s->unk27 = count;
        s->type = r->type;
        s->pos[0] = r->pos[0];
        s->pos[1] = r->pos[1];
        s->pos[2] = r->pos[2];
        s->unk8 = r->unk8;
        s->unkC = r->unkC;
        s->unk3 = r->unk3;
        s->unk4 = r->unk22;
        s->unk5 = r->unk4;
        s->room = r->room;
        s->unk29 = r->unk1F;
        s->unk2A = r->unk20;
        s->unk2B = r->unk5;
        s->unk3A = r->unk21;
        s->unk2D = r->unk23;
        s->unk41 = r->unk24;
        s->unk43 = r->unk25;
        s->unk44 = r->unk26;
        s->unk45 = 0;
        for (i = 0; i < 10; i++) {
            s->unk2F[i] = r->unk27[i];
        }
        hp = 0x7FFF;
        if (r->hpIndex < 8) {
            if (func_00688C10()) {
                hp = D_006F5EC0[s->type][D_0048BF60.unkBA & 3][r->hpIndex];
            } else {
                hp = D_006F4F40[s->type][D_0048BF60.unkBA & 3][r->hpIndex];
            }
            s->hp = hp;
        } else {
            s->hp = 0x7FFF;
        }
        s->hpMax = hp;
        s->unk3C = r->unk44;
        s->unk20 = r->unk3A;
        s->unk26 = r->unk1D;
        s->unk46 = r->unk31;
        s->unk47 = r->unk32;
        s->unk48[0] = r->unk34[0];
        s->unk48[1] = r->unk34[1];
        s->unk48[2] = r->unk34[2];
        s->unk48[3] = r->unk34[3];
        s->unk48[4] = r->unk34[4];
        s->unk48[5] = r->unk34[5];
        s->unk4E[0] = r->unk3B[0];
        s->unk4E[1] = r->unk3B[1];
        s->unk4E[2] = r->unk3B[2];
        s->unk4E[3] = r->unk3B[3];
        s->unk4E[4] = r->unk3B[4];
        s->unk4E[5] = r->unk3B[5];
        s->unk55 = r->unk42;
        s->unk56 = r->unk43;
        s->unk57[0] = r->unk45[0];
        s->unk57[1] = r->unk45[1];
        s->unk57[2] = r->unk45[2];
        if (s->unk57[0] != 0) {
            s->unk57[0] += 0x31;
        }
        if (s->unk57[1] != 0) {
            s->unk57[1] += 0x31;
        }
        if (s->unk57[2] != 0) {
            s->unk57[2] += 0x31;
        }
        s->unk5A = r->unk48;
        s->unk5B = r->difficulties;
        s->unk5C = D_0023AFD0[r->unk4A];
        s->unk5D = r->unk4B;
        if (r->room != room) {
            room = r->room;
            D_007200B0[room] = n;
        }
        s->index = n;
        if (s->unk26 != 0) {
            func_0064F690(2, n);
        }
        n++;
        count++;
        D_00720030[room]++;
        s->unk42 = slot8;
        slot8 = (slot8 + 1) & 7;
        if (s->unk26 != 0) {
            func_00689DF0(s);
        }
    }
    D_0048C01E[0] = n;
    func_001B1DE0();
}
#endif

/* Which loaded enemy model set (2) holds a type, and its index per room. */
extern u16 D_0048B9B0[2];
extern u16 D_0048B7B0[2][0x80];

#ifdef NON_MATCHING   /* first draft */
u16 func_00577730(s8 type, s16 room) {
    s8 i;

    for (i = 0; i < 2; i++) {
        if (type == D_0048B9B0[i]) {
            return D_0048B7B0[i][room];
        }
    }
    return 0xFFFF;
}
#endif

extern s8 **D_006FD570[];           /* by scenario: by model index: by unk5D: rooms (-1 ends) */

s8 *func_006A1ED0(EnemySlot *s) {
    u16 m = func_00577730((s8)s->type, s->room);
    s8 **t;
    s8 *list;

    if (m == 0xFFFF) {
        return NULL;
    }
    t = (s8 **)D_006FD570[D_00306590.unk1A][m];
    if (t == NULL) {
        return NULL;
    }
    list = t[s->unk5D];
    if (list == NULL) {
        return NULL;
    }
    if (list[0] < 0) {
        return NULL;
    }
    return list;
}

#ifdef NON_MATCHING   /* first draft */
s8 func_006A1F80(s8 room, s8 *list) {
    s8 i;

    for (i = 0; list[i] >= 0; i++) {
        if (room == list[i]) {
            return i;
        }
    }
    return -1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
void func_00689DF0(EnemySlot *s) {
    s8 *list;
    s8 r = -1;

    list = func_006A1ED0(s);
    if (list != NULL) {
        r = func_006A1F80(s->room, list);
        if (r < 0) {
            r = -1;
        }
    }
    s->unk40 = r;
}
#endif

/* The room's items, then (offline, with the start option's bit 0) the ones left alone get
   unk24 = 1. */
#include "item.h"
void func_00561640(void);
void func_00561970(u16 room);
void func_00561B60(u16 room);
void func_00561CE0(u16 room);
void func_00562100(void);
s32 func_00583EA0(u8 i, s32 mask);
s32 func_001AF4D0(void);

void func_00561870(void) {
    u16 room = D_00306590.unk1A;
    s32 i;
    ItemSlot *e;

    func_00561640();
    if (func_001AF4D0() == 1) {
        func_00561B60(room);
    } else {
        func_00561970(room);
    }
    func_00561CE0(room);
    if (func_001AF4D0() == 1 && (D_0048BF60.unkC6 & 1)) {
        for (i = 0, e = D_003967D0; i < 0x100; i++, e++) {
            if (e->active != 0 && e->unk6 != 0x20 && func_00583EA0(i, 1) != 0
                && func_00583EA0(i, 0x6000) != 0 && e->unk24 <= 0) {
                e->unk24 = 1;
            }
        }
    }
    func_00562100();
}

/* Ten message strings (D_006D5F20's ids) copied out, at most 38 characters each. */
extern u16 D_006D5F20[10];
extern char *D_0070E280[10];
extern char D_0070E2B0[10][0x28];
char *func_001C0020(s32 id);
u32 strlen(const char *s);

void func_00574B40(void) {
    s32 n;
    s32 i;
    char *src;
    s32 j;
    char *dst;

    for (i = 0; i < 10; i++) {
        src = func_001C0020(D_006D5F20[i]);
        n = strlen(src);
        if (n >= 0x27) {
            n = 0x26;
        }
        dst = D_0070E2B0[i];
        D_0070E280[i] = dst;
        for (j = 0; j < n; j++) {
            *dst++ = *src++;
        }
        *dst = 0;
    }
}

/* The room's doors (door%03d.dat). */
typedef struct {
    u8 *buf;            /* 0x00 */
    u8 *unk4;           /* 0x04 */
    u8 *unk8;           /* 0x08: the file's body (after a 0x10 header) */
    s32 unkC;           /* 0x0C */
    s32 unk10;
    s32 size;           /* 0x14 */
    u8 *end;            /* 0x18: 0x80 bytes cleared after the file */
} DoorData;

extern DoorData D_00720990;
extern char D_0070DA80[];   /* "data\rom\scene\item_msg\door%03d.dat" */
extern char D_0070DAA8[];   /* "door%03d.dat" */
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
s32 func_001A96E0(const char *name, void *buf);

void func_006AB120(void);
#ifdef NON_MATCHING   /* the d->buf load is scheduled before the branch join */
void func_006AB120(void) {
    char name[0x100];
    DoorData *d;
    u8 *buf;

    if (D_00306590.door == NULL) {
        return;
    }
    memset(D_00306590.door, 0, 0x4000);
    memset(&D_00720990, 0, sizeof(D_00720990));
    D_00720990.buf = D_00306590.door;
    d = &D_00720990;
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_0070DA80, D_00306590.unk1A);
        d->size = func_001CFBB0(name, D_00720990.buf, 1, 0);
    } else {
        sprintf(name, D_0070DAA8, D_00306590.unk1A);
        d->size = func_001A96E0(name, D_00720990.buf);
    }
    buf = d->buf;
    d->unk4 = ((u8 **)buf)[0];
    d->unkC = ((s32 *)buf)[1];
    d->unk8 = buf + 0x10;
    d->unk4 = (u8 *)((u32)d->unk4 + (u32)d->unk8);
    d->end = d->buf + d->size;
    memset(d->end, 0, 0x80);
}
#endif
