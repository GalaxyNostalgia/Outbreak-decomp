#include "common.h"
#include "options.h"
#include "task.h"
#include "game.h"

/* GAME.BIN: the game's main task (slot 4) and its top-level states. */

extern s16 D_0023BB94;
extern u8 D_00306659[];         /* (Options) how the title is entered next */

void func_001AE6D0(void);
void func_001AEDC0(s32 a);
void func_001B1910(s32 count);
void func_001B67A0(s16 slot, void (*onKill)(void));
void func_001B9E90(void);
void func_005CD880(void);

void func_001B0090(void);
void func_001B0160(void);
void func_001B0230(void);
void func_001B02B0(void);
void func_001B11F0(void);
void func_001B1360(void);
void func_001B13D0(void);
void func_001AE8E0(s32 i);
void func_001AE310(s32 i);
void func_0018D730(void);
void func_0064F600(void);

extern s8 D_0023BE28, D_0023BE29, D_0023BE30, D_0023BE31;
extern u8 D_0047F4D0[0xA340];
extern u8 D_003F3090[0xCC10];
extern u8 D_0042A2C0[0x3FC50];
extern u8 D_00469F10[0x6180];
extern u8 D_00389750[0xA048];
extern u8 D_0039A3D0[0x129B0];
extern u8 D_004BDA30[0x60];
extern u8 D_004BD990[0xA0];
extern u8 D_00470090[0x6D40];
extern u8 D_003BB520[0x2090];
extern u8 D_003B9490[0x2090];
extern u8 D_0070E4D0[0x14];
void func_00638BE0(void);
void func_00651040(void);
void func_00687E90(void);
void func_00560880(void);
void func_00551DB0(void);
void func_005A9740(void);
void func_00590B60(void);
void func_005A74C0(void);
void func_006AC300(void);
void func_006200C0(void);
void func_00543580(void);
void func_00588A80(void);
void func_00543C80(void);
void func_0067ED70(void);
void func_00601F60(void);
void func_00543D00(void);
void func_0067B520(void);
void func_006631B0(void);
s32 func_001AF4D0(void);
void func_00825DF0(void);
void func_00651DF0(void);
void func_00651E10(s32 a);
void func_005BBF30(void);
void func_001B7FE0(void);

/* Clears the game's data and every subsystem. */
void func_005BA390(void) {
    D_0023BE31 = 0;
    D_0023BE30 = 0;
    D_0023BE29 = 0;
    D_0023BE28 = 0;
    memset(D_0047F4D0, 0, sizeof(D_0047F4D0));
    memset(D_003F3090, 0, sizeof(D_003F3090));
    memset(D_0042A2C0, 0, sizeof(D_0042A2C0));
    memset(D_00469F10, 0, sizeof(D_00469F10));
    memset(D_00389750, 0, sizeof(D_00389750));
    memset(D_0039A3D0, 0, sizeof(D_0039A3D0));
    memset(D_004BDA30, 0, sizeof(D_004BDA30));
    memset(D_004BD990, 0, sizeof(D_004BD990));
    memset(D_00470090, 0, sizeof(D_00470090));
    memset(D_003BB520, 0, sizeof(D_003BB520));
    memset(D_003B9490, 0, sizeof(D_003B9490));
    func_00638BE0();
    func_00651040();
    func_00687E90();
    func_00560880();
    func_00551DB0();
    func_005A9740();
    func_00590B60();
    func_005A74C0();
    func_006AC300();
    func_006200C0();
    func_00543580();
    func_00588A80();
    func_00543C80();
    func_0067ED70();
    func_00601F60();
    func_00543D00();
    func_0067B520();
    func_006631B0();
    if (func_001AF4D0() == 1) {
        func_00825DF0();
    }
    func_00651DF0();
    func_00651E10(0xFF);
    memset(D_0070E4D0, 0xFF, sizeof(D_0070E4D0));
    func_005BBF30();
    func_001B7FE0();
    D_0048BF60.bgm[2] = -1;
    D_0048BF60.bgm[1] = -1;
    D_0048BF60.bgm[0] = -1;
}

/* Clears the game's state for a new game. */
void func_005BA2B0(void) {
    func_001B0090();
    func_001B0160();
    func_001B11F0();
    func_001B0230();
    func_001B02B0();
    func_001B1360();
    func_001B13D0();
    D_00306590.unk18 = 0;
    D_00306590.unk19 = 0;
    D_00306590.unk1A = 0;
    D_00306590.unk1C = 0;
    D_00306590.unk1E = 0;
    D_00306590.unk20 = 0;
    D_00306590.unk22 = 0;
    D_00306590.unk24 = 0;
    D_00306590.unk26 = 0;
    D_00306590.unk28 = 0;
    D_00306590.unk29 = 0;
    memset(&D_0048BF60, 0, sizeof(D_0048BF60));
    func_005BA390();
    func_001AE8E0(0);
    func_001AE310(0);
    func_0018D730();
    func_0064F600();
}

void func_005CD5E0(void) {
    func_005BA2B0();
    func_001AE6D0();
    D_00306590.unk0 = 1;
    D_0023BB94 = 0;
    func_005CE710(0);
    func_001B1910(0x12C0);
    func_001B9E90();
    D_00306590.unk10[2] = 0;
    func_001AEDC0(0);
    func_001B67A0(4, func_005CD880);
    D_00306659[0] = 1;
}

void func_001AEEA0(void);
void func_001AEF80(void);
void func_001B2D00(void);
void func_005CD7F0(void);
void func_005CD890(void);
void func_005CD9E0(void);
void func_005CDA50(void);
void func_005CDB90(void);
void func_005CE080(void);
void func_005CE2F0(void);
void func_005CE740(void);
void func_005CE7E0(void);
void func_005CE900(void);
void func_005CECA0(void);
void func_005D25D0(void);
void func_005D2A90(void);

void func_005CD660(void) {
    func_001AEEA0();
    func_001B2D00();
    switch (D_0048BF60.state) {
    case 0:
        func_005CD890();
        break;
    case 1:
        func_005CD9E0();
        break;
    case 2:
        func_005CDB90();
        break;
    case 3:
        func_005CE080();
        break;
    case 4:
        func_005CE2F0();
        break;
    case 5:
        func_005CE740();
        break;
    case 6:
        func_005CE7E0();
        break;
    case 7:
        func_005CE900();
        break;
    case 8:
        func_005CDA50();
        break;
    case 12:
        func_005CECA0();
        break;
    case 13:
        func_005D25D0();
        break;
    case 14:
        func_005D2A90();
        break;
    case 15:
        func_005CD7F0();
        return;
    }
    func_001AEF80();
}

void func_005CD580(Task *t) {
    switch (t->step) {
    case 0:
        func_005CD5E0();
        t->step++;
        break;
    case 1:
        func_005CD660();
        break;
    }
}

void func_005CE710(s8 state) {
    D_0048BF60.state = state;
    D_0048BF60.step3 = 0;
    D_0048BF60.step2 = 0;
    D_0048BF60.step = 0;
    D_0048BF60.timer = 0;
}

extern s32 D_0023BE98;
extern u8 D_002F88B0[0x30];      /* how the game was started (title.c) */
void func_001B69A0();
void func_001AE7F0(void);
void func_005BA6D0(void);
void func_005BA750(void);
void func_005BA9D0(void);
void func_0034E9E0(void);
void func_00341AB0(void);

/* State 0: the game starts. */
void func_005CD890(void) {
    switch (D_0048BF60.step) {
    case 0:
        D_00306590.unk19 = 0;
        D_0023BE98 = 0;
        if (D_00306590.online == 1) {
            D_0048BF60.step++;
            break;
        }
        func_001B69A0(1);
        if (D_002F88B0[0x20] != 0) {
            if (D_00306590.unk19 != 0) {
                D_0048BF60.unk29 = 0;
                D_00306590.unk19 = 0;
            }
            func_005BA6D0();
            func_0034E9E0();
            func_005BA750();
            func_005BA9D0();
            func_005CE710(8);
        } else {
            if (D_00306590.unk19 != 0) {
                D_00306590.unk19 = 0;
                D_0048BF60.unk29 = 1;
            }
            func_005CE710(0xE);
        }
        func_00341AB0();
        break;
    case 1:
        D_0048BF60.step++;
        break;
    case 2:
        func_001AE7F0();
        func_001B69A0();
        if (D_00306590.unk19 != 0) {
            D_00306590.unk19 = 0;
            D_0048BF60.unk29 = 1;
        }
        func_005CE710(5);
        break;
    }
}
