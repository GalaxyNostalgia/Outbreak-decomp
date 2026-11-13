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
extern u32 D_003064D0[];        /* buttons pressed this frame (unsized: not $gp-relative) */

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
void func_005BF6E0();           /* (s16, unused: one call passes nothing) */
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
        if (D_003064D0[0] & 0x8000) {
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
        if (D_003064D0[0] & 0x8000) {
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

extern u8 D_00825400[][3];      /* (online) the players: character, ... */
extern u32 D_0023BE14;
extern char *D_006DA350[];      /* room 1's music by our character: without Kevin */
extern char *D_006DA370[];      /* ... with Kevin */
extern char D_0070AEA0[];
extern char D_0070AEC0[];
extern char D_0070AEE0[];
void func_001ADA60(s32 n, s32 byId, char *file, s32 ptid);

/* Kevin (character 1) is playing. */
s32 func_005D2860(void) {
    s32 i;

    if (func_001AF500() == 1) {
        for (i = 0; i < 4; i++) {
            if (D_00825400[i][0] == 1) {
                return 1;
            }
        }
        return 0;
    }
    for (i = 0; i < D_0023BE14; i++) {
        if (D_00476DD0[i].unkB98 == 1) {
            return 1;
        }
    }
    return 0;
}

/* The music for the rooms that start with some (the event's arg is unused). */
#ifdef NON_MATCHING   /* the constant args' order in the calls */
void func_005D2920(s16 a) {
    switch (D_00306590.unk1A) {
    case 1:
        if (func_005D2860() == 0) {
            func_001ADA60(1, 0, D_006DA350[D_00476DD0[D_0048BF60.unk1D].unkB98], 0);
        } else {
            func_001ADA60(1, 0, D_006DA370[D_00476DD0[D_0048BF60.unk1D].unkB98], 0);
        }
        break;
    case 2:
        break;
    case 0x1C:
        func_001ADA60(1, 0, D_0070AEA0, 0);
        break;
    case 0x23:
        func_001ADA60(1, 0, D_0070AEC0, 0);
        break;
    case 0x29:
        func_001ADA60(1, 0, D_0070AEE0, 0);
        break;
    }
}
#endif

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


/* Entering a room: its data, then everything in it. */
void func_001AC9F0(void);
void func_005BB840(void);
void func_001D4C40(void);
void func_0065A620(void);
void func_005BB2D0(u8 room, u8 a, u8 b);
void func_001D7320(void);
void func_005BB5A0(s32 slot, DrawObjFull *obj);

void func_005BAB70(void) {
    s32 i;
    DrawObjFull *obj;

    func_001AC9F0();
    func_001AB8E0();
    func_005BB840();
    func_001D4C40();
    func_0065A620();
    func_005BB2D0(D_00306590.unk1A, D_00306590.unk1C, D_00306590.unk1E);
    func_001D7320();
    for (i = 0, obj = D_00476DD0; i < (s32)D_0023BE14; i++, obj++) {
        func_005BB5A0(i, obj);
    }
    func_001AB8E0();
}

void func_001B1D60(void);
void func_001B1E70(void);
void func_001D2DC0(void);
void func_00688C40(void);
void func_001D33D0(void);
void func_00651EF0(void);
void func_006A3B60(void);
void func_005A9C70(void);
void func_006897A0(void);
void func_001D6960(void);
void func_005BAED0(void);
void func_00639800(void);
void func_00561570(void);
void func_0067B1F0(void);
void func_00651340(void);
void func_006383E0(void);
void func_00346810(void);
void func_00346900(void);
void func_00346990(void);
void func_00638850(void);
void func_00651660(void);
void func_003478F0(void);
void func_00546E30(void);
void func_001B7D60(void);
void func_001AF190(void);
void func_006AB300(u16 a);
void func_006AB4E0(s32 a);

void func_005BAC20(void) {
    func_001AB8E0();
    func_0018D730();
    func_001B1D60();
    func_0018D730();
    func_001B1E70();
    func_001D2DC0();
    func_00688C40();
    func_0018D730();
    func_001D33D0();
    func_00651EF0();
    func_0018D730();
    func_006A3B60();
    func_005A9C70();
    func_006897A0();
    func_001D6960();
    func_005BAED0();
    func_0018D730();
    func_00639800();
    func_00561570();
    func_0067B1F0();
    func_00651340();
    func_006383E0();
    func_00346810();
    func_00346900();
    func_00346990();
    func_00638850();
    func_00651660();
    func_003478F0();
    func_00546E30();
    func_001B7D60();
    func_001AB8E0();
    func_001AF190();
    func_006AB300(D_00306590.unk1C);
    func_006AB4E0(-1);
    D_00306590.unk3E = 0;
}

s32 func_001B9C30(s16 frames, u32 color);
void func_00341AB0(void);
void func_001B2C90(void);
void func_00345480(void);

/* State 1's start: the room, then a fade in. */
void func_005CD9E0(void) {
    D_00306590.unk10[2] = 1;
    D_0048BF60.unk64 = 1;
    func_005BAB70();
    func_005BAC20();
    D_0048BF60.unk64 = 0;
    func_00345480();
    func_001B9C30(0x10, 0xFF000000);
    func_005CE710(2);
    func_00341AB0();
    func_001B2C90();
    D_00306590.unk10[2] = 0;
}

void func_001AF240(u8 player);
void func_001B80D0(void);
void func_001AD680(s32 n, s32 a, s32 b);
s32 func_001B9BA0(s16 frames, u32 color);
void func_001AF620(void);
s32 func_001B9EB0(void);
void func_00341B20(void);
void func_001AD190(s32 n);
void func_001AD420(s32 n);
void func_001AC210(void);
void func_005CDE40(void);
void func_00348DA0(void);
void func_005BC140(void);
s32 func_005BDA70(void);
void func_005BBF80(u8 a);
void func_0064A6E0(void);
void func_005BC020(void);
void func_005BC080(void);
void func_005A78C0(void);
void func_00541080(void);
void func_005437C0(void);
void func_006206C0(void);
void func_00543CA0(void);
void func_005BCBF0(void);
void func_001AEE30(void);
void func_001B8F00(void);
void func_0064FAD0(void);
void func_005BB710(s32 slot, DrawObjFull *obj);
void func_001B0E90(void);
void func_001B2CC0(void);
void func_003419C0(void);
void func_006880C0(void);
void func_00652360(void);
void func_005BAF70(void);
void func_005BAFB0(s32 keep);
void func_005BB0C0(s32 keep);
void func_001B1E30(void);
void func_001B1C90(void);

/* Leaving a room: the characters hidden, and the room's things, effects, routes and the rest
   cleared (less of it when a message is to play in it). */
void func_005BAD50(void) {
    s32 i;
    DrawObjFull *o;

    func_001B2CC0();
    func_003419C0();
    for (i = 0, o = D_00476DD0; i < (s32)D_0023BE14; i++, o++) {
        o->visible = 0;
    }
    D_00470090[1] = 0;
    func_006880C0();
    func_00652360();
    if (D_0023BB98 == 1) {
        func_005BAF70();
        func_005BAFB0(0);
        func_005BB0C0(0);
    } else {
        if ((s8)D_0048BF60.unk65 != 0) {
            func_005BAF70();
        }
        if (D_0048BF60.unk28 != 0) {
            func_005BB0C0(0);
            func_001B1E30();
            func_005BAFB0(0);
        } else {
            func_005BB0C0(1);
            func_005BAFB0(1);
        }
    }
    func_001B1C90();
    func_00551DB0();
    func_005A9740();
    func_00590B60();
    func_006200C0();
    func_00543580();
    func_00588A80();
    func_00543C80();
    func_00601F60();
    func_00543D00();
    func_0067B520();
    func_006631B0();
    func_005A9C70();
    if (func_001AF4D0() == 1) {
        func_00825DF0();
    }
    func_0018D730();
}
void func_005CE280(void);
void func_005811A0(void);
void func_0064F8C0(void);
extern s8 D_0048BF50[];
extern s8 D_0048BF88[];
extern u8 D_0035FFFB[];         /* (the next room's) */
extern u8 D_0035FFFF[];

#include "message.h"
extern u8 D_0048BBE0[];         /* per item: who has it (player + 1) */
extern s32 D_003AEF6C[];
extern s32 D_003AEF70[];
s32 func_005BC430(void);
void func_005BC690(void);
void func_005C05B0(void);
void func_00341B50(void);
void func_001BA0B0(s32 a, s32 b);

/* The message's 16 extra things (0x11A0 bytes each, type 10), cleared, when it has its characters. */
void func_00541010(DrawObjFull *o) {
    u8 *p;
    s32 i;

    if (D_0048BF60.unk28 != 0) {
        D_0035FFD0.things = o;
        for (i = 0, p = (u8 *)o; i < 0x10; i++, p += 0x11A0) {
            memset(p, 0, 0x11A0);
            p[2] = 10;
        }
    }
}

/* The message's work memory and its 8 characters, cleared. */
void func_005C05B0(void) {
    s32 i;
    DrawObjFull *o;

    memset((void *)(D_00306590.unk4C + 0x2FF000), 0, 0x20000);
    D_0035FFD0.objs = (void *)(D_00306590.unk4C + 0x31F000);
    for (i = 0, o = D_0035FFD0.objs; i < 8; i++, o++) {
        memset(o, 0, sizeof(DrawObjFull));
        o->kind = i;
    }
    func_00541010(o);
}

s32 func_005BCD80(void);
void func_005903A0(void);
void func_00681540(void);
void func_005907A0(void);
void func_001BA0F0(u32 color);
void func_0064A670(void);
void func_005BB250(void);
void func_005BB2D0(u8 room, u8 a, u8 b);
void func_005BF950(void);
void func_001D79A0(void);
void func_005A74C0(void);
void func_00546E30(void);
void func_001D4E80(s32 room, s32 a, s32 b, s32 who);
void func_001D56B0(u8 i, u16 id);
void func_001D45B0(void *t, u8 i, u16 id);
void func_001D4FC0(s32 room, s32 a, s32 b, s32 who);
void func_0064A6F0(s32 id, s32 a, s32 x, s32 y);
void func_001D5670(void);
void func_001D43E0(void);
void func_001D7260(s32 mode);

/* The message's scene: its room, its things and their sets (1: not ready yet). */
#ifdef NON_MATCHING   /* the sets loop: the original keeps &sets[i].id and &sets[i].n in their own registers (frame 0x60) */
s32 func_005BC430(void) {
    s32 i;
    u8 *t;
    s32 j;

    if (D_0035FFD0.kind == 1) {
        func_005BCEA0(0);
        return 0;
    }
    if (func_005BCD80() != 0) {
        return 1;
    }
    func_005903A0();
    func_00681540();
    func_005907A0();
    func_001BA0F0(0xFF000000);
    func_0064A670();
    D_00306590.unk1A = D_0035FFD0.room2[0];
    D_00306590.unk1C = D_0035FFD0.room2[1];
    D_00306590.unk1E = D_0035FFD0.room2[2];
    if (D_0023BB98 == 1) {
        func_005BB250();
    }
    func_005BB2D0(D_0035FFD0.room2[0], D_0035FFD0.room2[1], D_0035FFD0.room2[2]);
    func_005BF950();
    func_001D79A0();
    func_005A74C0();
    func_00546E30();
    func_001D4E80(D_00306590.unk1A, D_0035FFD0.a, D_0035FFD0.b, D_0035FFD0.who);
    for (i = 0, t = D_0035FFD0.things; i < D_0035FFD0.unk18; i++, t += 0x11A0) {
        func_001D56B0(i, *(u16 *)(t + 0x1180));
        func_001D45B0(t, i, *(u16 *)(t + 0x1180));
        for (j = 0; j < 8; j++) {
            t[8 + j] = 1;
        }
    }
    func_001D4FC0(D_00306590.unk1A, D_0035FFD0.a, D_0035FFD0.b, D_0035FFD0.who);
    for (i = 0; i < 0x10; i++) {
        for (j = 0; j < D_0035FFD0.sets[i].n; j++) {
            func_0064A6F0(D_0035FFD0.sets[i].id, D_0035FFD0.a, D_0035FFD0.sets[i].a[j][0], D_0035FFD0.sets[i].b[j][0]);
        }
    }
    func_001D5670();
    func_001D43E0();
    func_001D7260(0);
    return 0;
}
#endif

extern MsgWork *D_0023BE7C;     /* the message (D_0035FFD0) */
void func_005BC7D0(void);
void func_006A4D10(void);
void func_006A4D90(void);
s32 func_001ADBA0(s32 n);
void func_001ADC30(s32 n);
s32 func_0034A290(void);
s32 func_0034A2F0(void);
void func_00349B40(void);

/* Everyone has seen the message to its end (offline: we have). */
s32 func_005C0C60(void);
#ifdef NON_MATCHING   /* ours folds both return 1/0 tests into set-if */
s32 func_005C0C60(void) {
    s32 i;
    s32 n;

    if (func_001AF4D0() == 1) {
        if (D_0048BF60.unk3D[1] == 0) {
            return 0;
        }
        return 1;
    }
    for (n = 0, i = 0; i < (u8)D_0048BF60.unk62; i++) {
        if (D_0048BF60.unk3D[1] & (1 << i)) {
            n++;
        }
    }
    if (n != (u8)D_0048BF60.unk62) {
        return 0;
    }
    return 1;
}
#endif

void func_001B3960(void);
void func_001AC210(void);
void func_00349DE0(void);
void func_00349E20(void);
void func_001AD1E0(s32 n);
void func_001ADD90(s32 n);

/* The demo stream (1) resumes. */
void func_005BBF10(void) {
    func_001AD1E0(1);
}

/* The demo stream (1) is let go. */
void func_005BBF20(void) {
    func_001ADD90(1);
}
s32 func_001AF510(void);
void func_005C0E10(void);
extern void (*D_006D8A70[16])(void);
void func_00590400(void);
void func_00681820(s8 i);
void func_005908A0(void);
void func_001BA180(void);
void func_005BF480(void);

/* The loaded messages' scene table (0x10 bytes an entry, 0xFFFF time ends it). */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02 */
    s16 scene;          /* 0x04 */
    u8 unk6[6];
    u8 kind;            /* 0x0C */
    u8 unkD[3];
} MsgScene;

/* Per scene, two lists of moves (0x38 bytes an entry, 0xFFFF time ends it). */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02 */
    u8 unk4[0x38 - 4];
} MsgMove;

extern MsgScene *D_0023BD60;
extern MsgMove *D_003613B0[][2];
void func_0067FEB0(u8 kind);
void func_005903E0(MsgMove *m);

/* Each frame of a message: the next scene when its time comes (or both its move lists ended),
   then the scene's moves due now. */
void func_005BDD10(void);
#ifdef NON_MATCHING   /* a1/a2 swapped for the message pointer and &ended */
void func_005BDD10(void) {
    MsgScene *e;
    MsgMove *m;
    u16 t;
    u16 now;
    s32 i;

    e = D_0023BD60;
    if (e != NULL) {
        now = D_0023BE7C->frames;
        t = e->time;
        if (D_00306590.unkC != 0) {
            t >>= 1;
            now >>= 1;
        }
        if (D_0023BE7C->ended == 3 || t == now) {
            D_0023BE7C->ended = 0;
            if (e->time == 0xFFFF) {
                e = NULL;
            } else {
                func_0067FEB0(e->kind);
                D_0023BE7C->scene = e->scene;
                e++;
            }
        }
    }
    D_0023BD60 = e;
    for (i = 0; i < 2; i++) {
        m = D_003613B0[D_0023BE7C->scene][i];
        if (m == NULL) {
            continue;
        }
        t = m->time;
        now = D_0023BE7C->frames;
        if (D_00306590.unkC != 0) {
            t >>= 1;
            now >>= 1;
        }
        if (t == now) {
            func_005903E0(m);
            m++;
            if (m->time == 0xFFFF) {
                m = NULL;
                D_0023BE7C->ended |= 1 << i;
            }
        }
        D_003613B0[D_0023BE7C->scene][i] = m;
    }
}
#endif

/* Per set, 7 lists of moves (0x24 bytes an entry, 0xFFFF time ends it). */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02 */
    u8 unk4[0x24 - 4];
} MsgSetMove;

extern MsgSetMove *D_00360E80[16][8];
void func_005BDFF0(s32 id, MsgSetMove *m);

/* Each frame of a message: the sets' moves due now. */
void func_005BDEA0(void);
#ifdef NON_MATCHING   /* register allocation of the loop pointers (20 differ) */
void func_005BDEA0(void) {
    MsgSetMove *m;
    MsgSetMove **pp;
    u16 t;
    u16 now;
    s32 i;
    s32 j;
    s8 *ty;
    s32 *id;
    MsgSetMove **row;

    for (i = 0; i < D_0035FFD0.unk16; i++) {
        row = D_00360E80[i];
        ty = &D_0035FFD0.sets[i].type;
        id = &D_0035FFD0.sets[i].id;
        for (j = 0; j < 7; j++) {
            pp = &row[j];
            m = *pp;
            if (m == NULL) {
                continue;
            }
            now = D_0023BE7C->frames;
            t = m->time;
            if (D_00306590.unkC != 0) {
                t >>= 1;
                now >>= 1;
            }
            if (t == now) {
                switch (*ty) {
                case 0:
                case 1:
                case 2:
                    func_005BDFF0(*id, m);
                    break;
                }
                m++;
                if (m->time == 0xFFFF) {
                    m = NULL;
                }
            }
            *pp = m;
        }
    }
}
#endif

/* One of a message's 16 things (0x11A0 bytes). */
typedef struct {
    s8 on;              /* 0x00 */
    s8 shown;           /* 0x01 */
    u8 type;            /* 0x02: 10 */
    u8 unk3[0x18 - 3];
    s16 rot[3];         /* 0x18 */
    u8 unk1E[2];
    s32 x;              /* 0x20: (floats, copied as words) */
    s32 y;              /* 0x24 */
    f32 z;              /* 0x28 */
    u8 unk2C[0x1178 - 0x2C];
    void *placed;       /* 0x1178: the move that placed it */
    s32 unk117C;        /* 0x117C */
    u16 id;             /* 0x1180 */
    u8 unk1182[0x118C - 0x1182];
    s32 flags;          /* 0x118C: 1 unk117C set, 2 placed */
    u8 unk1190[0x11A0 - 0x1190];
} MsgThing;

/* A thing's move (0x24 bytes, like MsgSetMove). */
typedef struct {
    u8 unk0;
    u8 op;              /* 0x01: 0 place, 1 show/hide, 2 unk117C, 3 attach */
    u16 time;           /* 0x02 */
    s32 a;              /* 0x04 */
    s32 b;              /* 0x08 */
    f32 z;              /* 0x0C */
    s16 ang[3];         /* 0x10: degrees */
    u8 unk16[0x20 - 0x16];
    s32 who;            /* 0x20: (3) a thing (b != 0) or a set */
} MsgThingMove;

extern MsgThingMove *D_00360AE0[16][7];
void func_005417C0(MsgThing *t, s32 to, MsgThingMove *m, s32 n);

/* Each frame of a message: the things' moves due now. */
void func_005BE4C0(void);
#ifdef NON_MATCHING   /* the field pointers' stack slots and registers (247 vs 248) */
void func_005BE4C0(void) {
    MsgThing *things = D_0035FFD0.things;
    MsgThing *t;
    MsgThingMove *m;
    MsgThingMove **pp;
    u16 tm;
    u16 now;
    s32 i;
    s32 j;
    s32 *px;
    s32 *py;
    f32 *pz;
    s16 *r0;
    s16 *r1;
    s16 *r2;
    void **pl;
    s32 *pu;

    for (i = 0; i < D_0035FFD0.unk18; i++) {
        t = &things[i];
        px = &t->x;
        py = &t->y;
        pz = &t->z;
        r0 = &t->rot[0];
        r1 = &t->rot[1];
        r2 = &t->rot[2];
        pl = &t->placed;
        pu = &t->unk117C;
        for (j = 0; j < 7; j++) {
            pp = &D_00360AE0[i][j];
            m = *pp;
            if (m == NULL) {
                continue;
            }
            now = D_0023BE7C->frames;
            tm = m->time;
            if (D_00306590.unkC != 0) {
                tm >>= 1;
                now >>= 1;
            }
            if (tm == now) {
                switch (m->op) {
                case 0:
                    t->on = 1;
                    t->shown = 1;
                    *px = m->a;
                    *py = m->b;
                    *pz = m->z;
                    *r0 = (u32)(65536.0f * m->ang[0] / 360.0f + 0.5f);
                    *r1 = (u32)(65536.0f * m->ang[1] / 360.0f + 0.5f);
                    *r2 = (u32)(65536.0f * m->ang[2] / 360.0f + 0.5f);
                    *pl = m;
                    t->flags |= 2;
                    break;
                case 2:
                    t->flags |= 1;
                    *pu = m->a;
                    break;
                case 3:
                    if (m->a != 0) {
                        if (m->b != 0) {
                            func_005417C0(t, (s32)D_0035FFD0.things + m->who * 0x11A0, m, m->a - 1);
                        } else {
                            func_005417C0(t, D_0035FFD0.sets[m->who].id, m, m->a - 1);
                        }
                    }
                    break;
                case 1:
                    t->shown = m->a;
                    break;
                }
                m++;
                if (m->time == 0xFFFF) {
                    m = NULL;
                }
            }
            *pp = m;
        }
    }
}
#endif

/* A message's effects (0x24 bytes an entry, MsgWork.unk2E of them; 0xFFFF time once done). */
typedef struct {
    f32 x, y, z;
} MsgFxPos;

typedef struct {
    s16 x, y, z, w;
} MsgFxRot;

typedef struct {
    u16 unk0;
    u16 time;           /* 0x02 */
    u16 id;             /* 0x04: 0x8000: a sound (unk6) */
    s16 unk6;           /* 0x06 */
    MsgFxPos pos;       /* 0x08 */
    MsgFxRot rot;       /* 0x14 */
    u8 unk1C[4];
    s16 n;              /* 0x20: 1: on nothing */
    s16 unk22;          /* 0x22 */
} MsgEffect;

extern MsgEffect *D_0023BD70;
void func_005479C0(s32 n);
s32 func_005BF410(s32 a, s32 n, s32 c);
void func_00650200(s32 id, s32 on, f32 *pos, s16 *rot);

/* Each frame of a message: the effects due now. */
void func_005BE8A0(void) {
    MsgFxPos pos;
    MsgFxRot rot;
    MsgEffect *e;
    s32 i;
    u16 t;
    u16 now;
    s32 n;

    e = D_0023BD70;
    if (e == NULL) {
        return;
    }
    for (i = 0; i < (u8)D_0035FFD0.unk2E; i++, e++) {
        now = D_0023BE7C->frames;
        t = e->time;
        if (D_00306590.unkC != 0) {
            t >>= 1;
            now >>= 1;
        }
        if (t == now) {
            if (e->id & 0x8000) {
                func_005479C0(e->unk6 & 0x7FFF);
            } else {
                pos = e->pos;
                rot = e->rot;
                n = e->n - 1;
                if (n == 0) {
                    func_00650200((s16)e->id, 0, &pos.x, &rot.x);
                } else {
                    func_00650200((s16)e->id, func_005BF410(e->unk6, n, e->unk22), &pos.x, &rot.x);
                }
            }
            e->time = 0xFFFF;
        }
    }
}

/* Per light (MsgWork.unk1C of them), two lists of changes (0x30 bytes an entry). */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02 */
    u8 unk4[0x30 - 4];
} MsgLightMove;

extern MsgLightMove *D_003612A0[][2];
void func_00681570(MsgLightMove *m, s8 i);

/* Each frame of a message: the lights' changes due now. */
void func_005BE9E0(void) {
    MsgLightMove *m;
    s32 i;
    s32 j;
    MsgLightMove **row;
    MsgLightMove **pp;
    u16 t;
    u16 now;

    for (i = 0; i < D_0035FFD0.unk1C; i++) {
        row = D_003612A0[i];
        for (j = 0; j < 2; j++) {
            pp = &row[j];
            m = *pp;
            if (m == NULL) {
                continue;
            }
            now = D_0023BE7C->frames;
            t = m->time;
            if (D_00306590.unkC != 0) {
                t >>= 1;
                now >>= 1;
            }
            if (t == now) {
                func_00681570(m, i);
                m++;
                if (m->time == 0xFFFF) {
                    m = NULL;
                }
            }
            *pp = m;
        }
    }
}

/* A message's camera cuts (0x10 bytes an entry, MsgWork.unk20 of them). */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02 */
    u16 id;             /* 0x04 */
    u8 flag;            /* 0x06 */
    u8 unk7;
    f32 a;              /* 0x08 */
    f32 b;              /* 0x0C */
} MsgCut;

extern MsgCut *D_0023BE8C;
void *func_005C0C00(s32 id);
void func_005907C0(void *p, s32 flag, f32 a, f32 b);

/* Each frame of a message: the cuts due now. */
void func_005BEF10(void) {
    MsgCut *e;
    void *p;
    u16 t;
    u16 now;
    s32 i;

    e = D_0023BE8C;
    if (e == NULL) {
        return;
    }
    for (i = 0; i < D_0035FFD0.unk20; i++, e++) {
        now = D_0023BE7C->frames;
        t = e->time;
        if (D_00306590.unkC != 0) {
            t >>= 1;
            now >>= 1;
        }
        if (t == now) {
            p = func_005C0C00(e->id);
            if (p != NULL) {
                func_005907C0(p, e->flag, e->a, e->b);
            }
            e->time = 0xFFFF;
        }
    }
}

/* A message's scene effects (0x28 bytes an entry, MsgWork.unk22 of them). */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02 */
    u8 kind;            /* 0x04 */
    u8 unk5[0x28 - 5];
} MsgSceneFx;

extern MsgSceneFx *D_0023BD64;

/* Each frame of a message: the scene effects due now. */
void func_005BEE70(void) {
    MsgSceneFx *e;
    u16 t;
    u16 now;
    s32 i;

    e = D_0023BD64;
    if (e == NULL) {
        return;
    }
    for (i = 0; i < D_0035FFD0.unk22; i++, e++) {
        now = D_0023BE7C->frames;
        t = e->time;
        if (D_00306590.unkC != 0) {
            t >>= 1;
            now >>= 1;
        }
        if (t == now) {
            func_0067FEB0(e->kind);
            e->time = 0xFFFF;
        }
    }
}

/* (Nothing.) */
void func_005BDD00(void) {
}

void func_0064C540(s16 *m);

/* Each frame of a message: START (not in its first or last half second) asks to skip it. */
void func_005BEFD0(void) {
    s16 m[5];
    u16 now;
    u16 end;

    m[0] = D_0035FFD0.kind;
    m[1] = D_0035FFD0.a;
    m[2] = D_0035FFD0.b;
    m[3] = D_0035FFD0.who;
    m[4] = D_0035FFD0.who2;
    now = D_0023BE7C->frames;
    end = D_0023BE7C->count;
    if (D_00306590.unkC != 0) {
        now >>= 1;
        end >>= 1;
    }
    if (now >= 0x20 && end - now >= 0x20 && (D_003064D0[0] & 0x8000)) {
        func_0064C540(m);
    }
}

/* The message plays: (0) its text or movie starts, (1) its stream is ready, (2) the movie ends,
   (3) each frame until it's over. */
void func_005BD690(void) {
    s32 end;
    s32 i;

    switch (D_0023BE7C->sub) {
    case 0:
        if (D_0023BE7C->kind == 1) {
            if (D_0048BF60.unk3B == 1) {
                func_006A4D10();
            } else {
                func_006A4D90();
            }
        } else {
            func_006A4D10();
        }
        D_0023BE7C->sub++;
        break;
    case 1:
        if (D_0023BE7C->kind == 1) {
            if (D_0048BF60.unk3B == 1 && func_001ADBA0(1) == 0) {
                break;
            }
        } else if (func_001ADBA0(1) == 0) {
            break;
        }
        if (D_0023BE7C->kind == 0) {
            D_0023BE7C->sub = 3;
            func_001ADC30(1);
            func_001B9C30(1, 0xFF000000);
            func_001BA0F0(0xFF000000);
        } else {
            func_00349B40();
            D_0023BE7C->sub++;
        }
        D_0023BE7C->frames = 0;
        D_0023BE7C->done = 0;
        D_0023BE7C->scene = 0;
        D_0023BE7C->ended = 0;
        break;
    case 2:
        if (func_0034A290() == 2) {
            D_0023BE7C->sub++;
            if (D_0048BF60.unk3B == 1) {
                func_001ADC30(1);
            }
            if (D_0048BF60.unk3B > 1) {
                func_005BBF10();
            }
            func_001BA0F0(0xFF000000);
            func_001B9C30(1, 0xFF000000);
        }
        break;
    case 3:
        end = 0;
        if (D_0023BE7C->kind != 0) {
            if (func_0034A2F0() == 0) {
                end = 1;
            }
        } else {
            end |= D_0023BE7C->done;
        }
        if (func_005C0C60() != 0) {
            D_0023BE7C->step++;
            D_0023BE7C->sub = 0;
            D_0023BE7C->sub2 = 0;
            D_0048BF60.unk3D[1] = 0;
            func_001B3960();
            func_001AD420(1);
            func_001AD420(2);
            func_001AC210();
            if (D_0023BE7C->kind != 0) {
                func_00349DE0();
                func_00349E20();
                func_005BBF20();
            }
            if (func_001AF510() == 1) {
                func_005C0E10();
            }
        } else if (end != 0) {
            if (D_0023BE7C->kind != 0) {
                func_00349DE0();
                func_005BBF20();
            }
            D_0023BE7C->step++;
            D_0023BE7C->sub = 0;
            D_0023BE7C->sub2 = 0;
            D_0048BF60.unk3D[1] = 0;
            func_001AD420(1);
            func_001AD420(2);
            func_001AC210();
            func_001B3960();
        } else {
            for (i = 0; i < 0x10; i++) {
                D_006D8A70[i]();
            }
            func_00590400();
            for (i = 0; i < D_0035FFD0.unk1C; i++) {
                func_00681820(i);
            }
            func_005908A0();
            func_001BA180();
            func_005BF480();
            if (D_00306590.unkC != 0) {
                D_0023BE7C->frames += 2;
            } else {
                D_0023BE7C->frames += 1;
            }
            if (D_0023BE7C->frames >= D_0023BE7C->count) {
                D_0023BE7C->done = 1;
            }
        }
        break;
    }
}

/* While changing rooms: the queued messages play (1 once the queue is empty). */
s32 func_005BDA70(void) {
    s32 r;

    D_0023BE7C = &D_0035FFD0;
    r = 0;
    switch (D_0023BE7C->step) {
    case 0:
        D_0023BE7C->step++;
        break;
    case 1:
        func_005BD690();
        break;
    case 2:
        if (D_0023BD7C == D_0023BD78) {
            D_0023BE7C->step = 0;
            r = 1;
            break;
        }
        func_001B9BA0(1, 0xFF000000);
        func_001AE8E0(0);
        D_0023BE7C->step++;
        D_0023BE7C->sub = 0;
        D_0023BE7C->sub2 = 0;
        D_0023BE7C->sub3 = 0;
        break;
    case 3:
        if (func_001B9EB0() == 0) {
            break;
        }
        func_001AD420(1);
        func_001AD420(2);
        func_005BF6E0();
        if (D_0035FFD0.item == 0) {
            D_0035FFD0.owner = 0;
        } else {
            D_0035FFD0.owner = D_0048BBE0[D_0035FFD0.item];
        }
        memset(D_0035FFD0.sets, 0, 0x9D8);
        func_005BC7D0();
        func_005BAD50();
        if (func_005BC430() != 0) {
            D_0048BF60.unk28 = 0;
            D_0048BF60.unkCA = 0;
        } else {
            D_0048BF60.unk28 = 1;
        }
        D_0023BE7C->step = 1;
        D_0023BE7C->sub = 0;
        D_0023BE7C->sub2 = 0;
        D_0023BE7C->sub3 = 0;
        func_001B2C90();
        break;
    }
    return r;
}

/* The next queued message comes up: the players it's for (those near its item's owner, or
   all), the room, and its fade. */
#ifdef NON_MATCHING   /* regalloc in the player loop (11 differ) */
void func_005BC140(void) {
    s32 i;
    s32 n;
    s32 cnt;
    s32 st;
    DrawObjFull *o;

    D_0048BF60.unk3A = 0;
    D_0048BF60.unk1E[0] = 0;
    D_0048BF60.unk3D[1] = 0;
    memset(&D_0035FFD0, 0, sizeof(D_0035FFD0));
    if (D_0023BD7C == D_0023BD78) {
        D_0048BF60.unk28 = 0;
        D_0048BF60.unkCA = 0;
        return;
    }
    func_005BF6E0(D_0023BD7C);
    D_0048BF60.unk28 = 1;
    D_0048BF60.unkCA = 1;
    func_005C05B0();
    if (D_0035FFD0.item == 0) {
        D_0035FFD0.owner = 0;
    } else {
        D_0035FFD0.owner = D_0048BBE0[D_0035FFD0.item];
    }
    cnt = D_0023BE14;
    D_0035FFD0.n = 0;
    for (i = 0, n = 0; i < cnt; i++) {
        o = &D_00476DD0[i];
        o->visible = 0;
        st = o->unkBA0;
        if (st & 0x5E0) {
            continue;
        }
        if (D_0035FFD0.who2 == 4 || D_0035FFD0.who2 == 5) {
            if (D_0035FFD0.owner == 0) {
                continue;
            }
            if (D_00476DD0[D_0035FFD0.owner - 1].unk1E2 != o->unk1E2) {
                continue;
            }
        } else if (D_0035FFD0.who2 == 6) {
            if (D_0035FFD0.owner - 1 != i && D_0048AC10.players[D_0035FFD0.owner - 1][1] != o->unk1E2) {
                continue;
            }
        }
        D_0035FFD0.chr[n] = o->unkB98;
        D_0035FFD0.status[n] = st;
        D_0035FFD0.pl[n] = o->kind;
        n++;
        D_0035FFD0.n++;
    }
    if (D_0035FFD0.owner != 0) {
        for (i = 0; i < D_0035FFD0.n; i++) {
            if (D_0035FFD0.pl[i] == D_0035FFD0.owner - 1) {
                D_0035FFD0.owner = i + 1;
                break;
            }
        }
    }
    D_0035FFD0.room[0] = D_00306590.unk1A;
    D_0035FFD0.room[1] = D_00306590.unk1C;
    D_0035FFD0.room[2] = D_00306590.unk1E;
    if (D_0035FFD0.kind != 1) {
        func_005BAD50();
        func_005BC690();
        func_00341AB0();
    } else {
        func_00341B50();
    }
    D_003AEF6C[0] = 0;
    D_003AEF70[0] = -1;
    if (func_005BC430() != 0) {
        D_0048BF60.unk28 = 0;
        D_0048BF60.unkCA = 0;
    }
    func_001B9C30(1, 0xFF000000);
    func_001BA0B0(1, 0);
    func_001B2C90();
}
#endif

/* State 4: changing rooms: (0) out with a fade (or straight to 2 for a cut), (1) until the
   streams stop, (2) the old room goes, (3) the new one loads (func_005BDA70) and fades in, (4)
   online until everyone's there (else on to 5), (5) the room starts: the characters, the items,
   the fade in, back to state 2. */
#ifdef NON_MATCHING   /* first draft (266 vs 262) */
void func_005CE2F0(void) {
    GameState *g = &D_0048BF60;
    DrawObjFull *p;
    s32 i;

    switch (D_0048BF60.step) {
    case 0:
        g->timer = 0;
        func_001AF240(D_0048BF60.unk1D);
        func_001AC9F0();
        if ((s8)g->unk65 != 0) {
            g->step = 2;
            return;
        }
        func_001B80D0();
        func_001AD680(0, 0, 10);
        func_001AD680(1, 0, 10);
        func_001B9BA0(0x10, 0xFF000000);
        func_001AF620();
        g->step++;
        return;
    case 1:
        if (func_001B9EB0() != 0) {
            func_001AE8E0(0);
            func_00341B20();
            func_001AD190(0);
            func_001AD420(1);
            func_001AD420(2);
            func_001AC210();
            g->step++;
            return;
        }
        func_005CDE40();
        return;
    case 2:
        func_001AD190(0);
        func_001AD420(1);
        func_001AD420(2);
        func_001AC210();
        func_001B80D0();
        func_00348DA0();
        func_005BC140();
        if (D_0048BF88[0] == 0) {
            func_001B9BA0(0x10, 0xFF000000);
            g->step = 4;
            return;
        }
        func_001B2C90();
        g->step++;
        return;
    case 3:
        if (func_005BDA70() != 0) {
            func_001B9BA0(1, 0xFF000000);
            g->step++;
        }
        if (D_0035FFFF[0] != 1) {
            func_005BBF80(D_0035FFFF[0]);
            func_006897A0();
            func_0064A6E0();
            func_005BC020();
            func_00346900();
            func_00346990();
            func_005BC080();
            func_005A78C0();
            func_00541080();
            func_005437C0();
            func_006206C0();
            func_00543CA0();
        }
        return;
    case 4:
        if (g->unk29 != 0) {
            func_005BCBF0();
            g->unkA6 = 1;
            g->unkA7 = 0;
            D_00306590.unk1A = D_0035FFFB[0];
            func_001AEE30();
            func_005CE710(0xC);
            func_001B8F00();
            return;
        }
        if (g->unkA7 != 0) {
            func_005BCBF0();
            g->unkA6 = 1;
            g->unk29 = 1;
            D_00306590.unk1A = D_0035FFFB[0];
            func_001AEE30();
            func_005CE710(0xC);
            func_001B8F00();
            return;
        }
        func_00341B20();
        func_001AE8E0(0);
        g->step++;
        return;
    case 5:
        func_0064FAD0();
        for (i = 0, p = D_00476DD0; i < 8; i++, p++) {
            func_005BB710(i, p);
        }
        func_001B0E90();
        func_005BAD50();
        func_005BCBF0();
        D_0048BF50[0] = 0;
        if ((s8)g->unk65 != 0) {
            g->unk65 = 0;
            func_005D2F00();
        } else {
            g->unk2B = 1;
        }
        if (D_0048BF60.unk37 == 0) {
            g->unk37 = 3;
        }
        func_005CE280();
        D_00306590.unk18 = 0;
        for (i = 0, p = D_00476DD0; i < 8; i++, p++) {
            func_001D6BA0(p);
        }
        func_006383E0();
        func_006897A0();
        func_00346900();
        func_00346990();
        func_001B9C30(0x10, 0xFF000000);
        func_005CE710(2);
        func_00341AB0();
        func_001B2C90();
        func_005811A0();
        func_0064F8C0();
        break;
    }
}
#endif
