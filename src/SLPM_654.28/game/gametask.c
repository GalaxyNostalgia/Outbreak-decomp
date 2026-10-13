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

extern s16 D_0023BD78;          /* (pools.c) a queue's read and write positions */
extern s16 D_0023BD7C;
extern u32 D_0023BE14;          /* players */
extern u32 D_003064D0;          /* buttons pressed this frame */

void func_005BA730(GameState *g) {
    D_00306590.unk1C = 0;
    D_00306590.unk1A = D_002F88B0[3];
}
void func_001ADDF0(s32 a);
s32 func_001ADBA0(s32 a);
void func_00349D00(void);
s32 func_0034A290(void);
s32 func_0034A2F0(void);
void func_001ADC30(s32 a);
void func_00349E20(void);
void func_001AD470(void);
void func_0065A960(void);
s32 func_001B6980(s32 slot);
void func_005D2920(s16 a);
void func_005BF6E0(s16 a);
void func_005BDC20(void);
void func_00349B40(void);
void func_001B3960(void);
void func_005BDC80(void);
s32 func_001AF500(void);
void func_006500B0(s32 a);
void func_001BA7A0(s32 a);
void func_001BA870(void);
void func_005D2F00(void);

#ifdef NON_MATCHING   /* first draft */
/* State 14: an event (a movie with the game paused behind it), then on to state 1. */
void func_005D2A90(void) {
    GameState *g = &D_0048BF60;
    u32 i;
    s32 n;

    switch (g->step) {
    case 0:
        func_005BA730(g);
        D_0048BF60.unk29 = 0;
        func_005BA6D0();
        func_005BA750();
        g->unkA = 0;
        g->unkC = 0;
        g->step++;
        break;
    case 1:
        if (func_001AF500() == 1) {
            g->step += 5;
            break;
        }
        if (D_00306590.unk1A != 1) {
            g->step += 5;
            break;
        }
        g->timer = 0;
        g->step++;
        func_001ADDF0(1);
        break;
    case 2:
        if (func_001ADBA0(0) != 0) {
            D_00306590.unk3C = 1;
            func_00349D00();
            g->step++;
        }
        break;
    case 3:
        if (func_0034A290() == 2) {
            func_001ADC30(0);
            g->step++;
        }
        break;
    case 4:
        if (func_0034A2F0() == 0) {
            D_00306590.unk3C = 0;
            func_00349E20();
            g->timer = 0x28;
            g->step++;
            break;
        }
        if (g->unkA == 1) {
            D_00306590.unk3C = 0;
            func_00349E20();
            func_001AD470();
            g->timer = 0x5A;
            g->step++;
        }
        if (D_003064D0 & 0x8000) {
            g->unkA = 1;
        }
        break;
    case 5:
        if (--g->timer == 0) {
            g->step++;
            func_001AD470();
        }
        break;
    case 6:
        func_0065A960();
        if (func_001B6980(0x62) == 0 && D_0023BD7C != D_0023BD78) {
            func_005D2920(D_0023BD7C);
            g->step++;
            break;
        }
        while (D_0023BD7C != D_0023BD78) {
            func_005BF6E0(D_0023BD7C);
        }
        g->unk28 = 0;
        g->unkCA = 0;
        g->step += 3;
        g->unkC = 5;
        break;
    case 7:
        if (func_001ADBA0(1) != 0) {
            D_00306590.unk3C = 1;
            if (D_0023BD7C != D_0023BD78) {
                func_005BF6E0(D_0023BD7C);
                func_005BDC20();
                func_00349B40();
            }
            g->step++;
        }
        break;
    case 8:
        if (func_0034A2F0() == 0) {
            g->step++;
            g->unkC = 10;
            g->unk28 = 0;
            g->unkCA = 0;
            func_00349E20();
            func_001AD470();
            func_001B3960();
            break;
        }
        if (g->step2 == 0 && func_0034A290() == 2) {
            func_001ADC30(1);
            g->step2 = 1;
        }
        func_005BDC80();
        if (func_001AF500() == 1) {
            n = 0;
            for (i = 0; i < D_0023BE14; i++) {
                if (g->unkCB & (1 << i)) {
                    n++;
                }
            }
            if (n == D_0023BE14) {
                func_00349E20();
                func_001AD470();
                break;
            }
        } else if (g->unkC != 0) {
            func_00349E20();
            func_001AD470();
            break;
        }
        if (D_003064D0 & 0x8000) {
            if (func_001AF500() == 1) {
                func_006500B0(2);
            } else {
                g->unkC = 5;
            }
        }
        break;
    case 9:
        func_001BA7A0(6);
        if (--g->unkC == 0) {
            func_001BA870();
            func_005D2F00();
            func_005CE710(1);
        }
        break;
    }
}
#endif

/* After the event: the pools are reset and each character gets its player's id (offline, all of
   them get ours); Options.unk1E is the table entry for our id. */
#include "object.h"

extern s16 D_0023BB98;
typedef struct {
    u8 unk0[0x280];
    u8 players[4][3];   /* 0x280: (byte 1) the player's id */
} ScenePlayers;

extern ScenePlayers D_0048AC10;
extern u8 D_0048AEB0[];
extern DrawObjFull D_00476DD0[];

void func_005D2F00(void) {
    s32 i;
    u8 id;

    func_005A74C0();
    func_006200C0();
    func_005BA9D0();
    if (D_0023BB98 == 0) {
        id = D_0048AC10.players[D_0048BF60.unk1D][1];
        if (id != 0) {
            D_00306590.unk1C = id;
            for (i = 3; i >= 0; i--) {
                D_00476DD0[i].unk1E2 = D_0048AC10.players[i][1];
            }
        } else {
            for (i = 3; i >= 0; i--) {
                D_00476DD0[i].unk1E2 = D_00306590.unk1C;
            }
        }
    }
    D_00306590.unk1E = D_0048AEB0[D_00306590.unk1C];
}

/* After an event: everything in the room is set up again (the characters, the scenario's
   tables, the script's routine 1, the enemies...). */
void func_001AB8E0(void);
void func_001B8AC0(s32 player, u8 kind);
void func_001AE8E0(s32 a);
void func_005BB840(void);
void func_005BA7D0(void);
void func_005BA810(void);
void func_001D6BA0(DrawObjFull *obj);
void func_001D6DD0(DrawObjFull *obj);
void func_001AFBB0(void);
void func_00639180(void);
void func_00561870(void);
void func_0067B2C0(void);
void func_001D7720(void);
void func_00688640(void);
void func_001D6FC0(void);
void func_0065A9A0(void);
void func_001D7090(void);
void func_00574B40(void);
void func_001D7150(void);
void func_001D76A0(void);
void func_001D7B10(void);
void func_001D0300(s32 a, s32 b);
void func_001D5200(void);
void func_0018D730(void);
void func_006AB120(void);
void func_006B9A00(void);
void func_0060DE80(void);

void func_005BA9D0(void) {
    s32 i;
    DrawObjFull *obj;

    D_00306590.unk10[2] = 2;
    func_001AB8E0();
    for (i = 0, obj = D_00476DD0; i < (s32)D_0023BE14; i++, obj++) {
        func_001B8AC0(i, obj->unkB98);
    }
    func_001AE8E0(0);
    func_005BB840();
    for (i = 0, obj = D_00476DD0; i < (s32)D_0023BE14; i++, obj++) {
        obj->kind = i;
    }
    func_005BA7D0();
    func_005BA810();
    for (i = 0, obj = D_00476DD0; i < (s32)D_0023BE14; i++, obj++) {
        func_001D6BA0(obj);
        func_001D6DD0(obj);
    }
    func_001AFBB0();
    func_00639180();
    func_00561870();
    func_0067B2C0();
    func_001D7720();
    func_00688640();
    func_001D6FC0();
    func_0065A9A0();
    func_001D7090();
    func_00574B40();
    func_001D7150();
    func_001D76A0();
    func_001D7B10();
    func_001D0300(0x2600, 2);
    func_001D5200();
    func_0018D730();
    func_006AB120();
    func_006B9A00();
    func_0060DE80();
    func_001AB8E0();
    D_00306590.unk10[2] = 0;
    D_00306590.unk3E = 0;
}

void func_001AF710(void);

void func_005BB840(void) {
    if (D_00306590.unk3E == 0) {
        func_001AF710();
        D_00306590.unk3E = 1;
    }
}

void func_001D5640(void);
void func_001D56A0(void);

void func_005BA7D0(void) {
    func_0018D730();
    func_001D5640();
    func_001D56A0();
    func_0018D730();
}

/* The characters' player ids (offline, from the tables at D_006D8630) and the room's data. */
extern u8 *D_006D8630[];
extern u8 D_002F88D0[];
s32 func_001AF510(void);
void func_005BB5A0(s32 i, DrawObjFull *obj);
s32 func_00347340(DrawObjFull *obj, s32 a, u8 kind);
void func_00347580(DrawObjFull *obj, u8 kind);
void func_001D4C40(void);
void func_001D4E10(u8 room);
void func_001D41F0(void);
void func_001D5600(u16 room);
void func_001D3D30(u8 room);

#ifdef NON_MATCHING   /* first draft: register allocation and the ids sum's order */
void func_005BA810(void) {
    s32 mode = D_0048BF60.unk20[D_0048BF60.unk27];
    u8 *ids;
    DrawObjFull *obj;
    s32 i;

    if (func_001AF510() == 0) {
        ids = D_006D8630[mode] + 1;
    }
    for (i = 0, obj = D_00476DD0; i < (s32)D_0023BE14; i++, obj++) {
        func_005BB5A0(i, obj);
        if (D_00306590.unk17 == 0 && func_00347340(obj, 0, obj->unkB98) != 0) {
            func_00347580(obj, obj->kind);
        }
        if (func_001AF510() == 1) {
            func_001AF500();
            if (mode == 3) {
                ids = D_006D8630[mode] + (D_00476DD0[i].unkB98 & 1) * 4 + 1;
            } else {
                ids = D_006D8630[mode] + i * 4 + 1;
            }
        }
        if (D_002F88D0[0] == 0) {
            obj->unk1E2 = *ids;
        }
    }
    func_0018D730();
    func_001D4C40();
    func_001D4E10(D_00306590.unk1A);
    func_0018D730();
    func_001D41F0();
    func_0018D730();
    func_001D5600(D_00306590.unk1A);
    func_001D3D30(D_00306590.unk1A);
    func_0018D730();
}
#endif

void func_00639100(DrawObjFull *obj);

void func_00639180(void) {
    u32 i;

    for (i = 0; i < D_0023BE14; i++) {
        func_00639100(&D_00476DD0[i]);
    }
}

