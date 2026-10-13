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
