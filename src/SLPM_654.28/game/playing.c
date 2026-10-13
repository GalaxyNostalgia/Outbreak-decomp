#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* State 2: playing. */

extern u32 D_003064D0;          /* buttons pressed this frame */
extern u32 D_00306500;          /* buttons held */
extern s16 D_0023BB98;
extern s16 D_0023BB94;
extern DrawObjFull D_00476DD0[];
extern u8 D_002F88D0[];
s32 func_001AF4D0(void);
s32 func_001AF500(void);
s32 func_001B6980(s32 flag);

void func_006AE930(void);
void func_006AC8A0(void);
void func_001B3960(void);
void func_001B8F00(void);
void func_0058A910(void);

void func_005CDAD0(void) {
    func_006AE930();
    func_006AC8A0();
    func_001B3960();
    func_001B8F00();
    func_0058A910();
}

/* Whether the map can't be opened now. */
s32 func_005CDB10(s32 unused) {
    if (FIELD(&D_0048BF60, u8, 0xC7) != 0) {
        return 0;
    }
    if (FIELD(&D_0048BF60, s8, 0xA7) != 0) {
        return 1;
    }
    if (FIELD(&D_0048BF60, u8, 0xBD) != 0) {
        return 1;
    }
    if (FIELD(&D_0048BF60, u8, 0xB9) & 1) {
        return 1;
    }
    return D_00476DD0[0].unk54F == 0x43;
}

s32 func_005CDFA0(void) {
    return D_0048BF60.unk28;
}

s32 func_005CDFB0(void) {
    return FIELD(&D_0048BF60, s8, 0xA6);
}

/* The play time (frames at 60 a second, twice as fast with Options.unkC). */
void func_005CDFC0(void) {
    GameState *g = &D_0048BF60;
    u8 n;

    if (D_00306590.unkC != 0) {
        n = 2;
    } else {
        n = 1;
    }
    g->timeF += n;
    if (g->timeF >= 60) {
        g->timeF -= 60;
        g->timeS = (g->timeS + 1) % 60;
        if (g->timeS == 0) {
            g->timeM = (g->timeM + 1) % 60;
            if (g->timeM == 0) {
                g->timeH = (g->timeH + 1) % 100;
            }
        }
    }
}

void func_0064FAD0(void);
void func_00341A70(void);
void func_0065A9E0(void);
void func_0034B260(void);
void func_006A3B60(void);
void func_006383E0(void);
void func_00651340(void);
void func_006897A0(void);
void func_006BC030(void);
void func_005A9D20(void);
void func_00346810(void);
void func_00346900(void);
void func_00346990(void);
void func_00638850(void);
void func_00651660(void);
void func_005ABAC0(void);
void func_005ABB40(void);
void func_005ABB80(void);
void func_0058E420(void);
void func_00588C30(void);
void func_005A78C0(void);
void func_00560AF0(void);
void func_00551FF0(void);
void func_005437C0(void);
void func_006206C0(void);
void func_00543CA0(void);
void func_006AC510(void);
void func_001B7F70(void);
void func_00685AA0(void);
void func_0067C6E0(void);
void func_00602160(void);
void func_005A9DB0(void);
void func_0064F8C0(void);
void func_0064FE20(void);
void func_005D3010(void);

/* A frame of the game. */
void func_005CDE40(void) {
    func_0064FAD0();
    func_00341A70();
    func_0065A9E0();
    if (func_005CDFA0() != 0) {
        func_0034B260();
        return;
    }
    func_006A3B60();
    func_006383E0();
    func_00651340();
    func_006897A0();
    func_006BC030();
    func_005A9D20();
    func_00346810();
    func_00346900();
    func_00346990();
    func_00638850();
    func_00651660();
    func_005ABAC0();
    func_005ABB40();
    func_005ABB80();
    func_0058E420();
    func_00588C30();
    func_005A78C0();
    func_00560AF0();
    func_00551FF0();
    func_005437C0();
    func_006206C0();
    func_00543CA0();
    func_006AC510();
    func_001B7F70();
    func_00685AA0();
    if (D_0023BB94 == 0) {
        func_0067C6E0();
        func_00602160();
        func_005A9DB0();
        func_0064F8C0();
        func_0064FE20();
        func_005D3010();
        func_0034B260();
    }
}

void func_006AE420(s32 a, u8 player);
void func_001AEC00(void);
void func_001B9140(s32 a);
void func_005CE710(s8 state);
void func_00650050(void);
void func_001AD470(void);
void func_001AC210(void);
void func_001AEE30(void);

/* State 2: playing (or the map, while unkC4). */
void func_005CDB90(void);
#ifdef NON_MATCHING   /* first draft */
void func_005CDB90(void) {
    if (D_0048BF60.unkC4 != 0) {
        func_006AE420(4, D_0048BF60.unk1D);
        func_006AC510();
        if (FIELD(&D_0048BF60, u8, 0xC7) == 2) {
            func_005CDAD0();
            func_001AEC00();
            func_001B9140(3);
            func_005CE710(0xF);
            return;
        }
        goto map;
    }
    func_005CDE40();
    if (func_005CDFA0() != 0) {
        func_00650050();
        func_005CDAD0();
        func_005CE710(4);
        return;
    }
    if (func_005CDFB0() != 0) {
        func_005CDAD0();
        func_001AD470();
        func_001AC210();
        func_001AEE30();
        func_005CE710(0xC);
        return;
    }
    func_005CDFC0();
    D_0048BF60.unk18++;
    if (D_00306590.unk18 != 0) {
        func_006AE930();
        func_006AC8A0();
        D_0048BF60.unk2B = 1;
        func_005CE710(3);
        if (func_001AF4D0() == 0) {
            if (FIELD(&D_00306590, u8, 0xB7) != 0) {
                D_0048BF60.unk18 += 150;
            } else {
                D_0048BF60.unk18 += 360;
            }
        }
    }
    if (func_001B6980(0x52) != 0 && (D_00306500 & 0x100)) {
        D_0048BF60.unk29 = 1;
    }
    if (D_0048BF60.unk29 != 0) {
        D_0048BF60.unk2B = 1;
        FIELD(&D_0048BF60, s8, 0xA6) = 1;
        func_005CDAD0();
        func_001AEE30();
        func_005CE710(0xC);
        return;
    }
map:
    if (func_001AF500() != 1 && D_0023BB98 == 0) {
        if (FIELD(&D_0048BF60, u8, 0xC7) == 1) {
            D_0048BF60.unkC4 = 1;
        } else if (func_005CDB10(1) == 0) {
            if (D_003064D0 & 0x4000) {
                if (D_0048BF60.unkC4 != 0) {
                    func_006AE930();
                }
                D_0048BF60.unkC4 ^= 1;
            }
        } else {
            if (D_0048BF60.unkC4 != 0) {
                func_006AE930();
            }
            D_0048BF60.unkC4 = 0;
        }
    }
}
#endif

void func_005BAB70(void);
void func_005BAC20(void);
void func_0034EBC0(void);
void func_00345480(void);
void func_0034F050(void);
s32 func_001B9C30(s16 frames, u32 color);
void func_00341AB0(void);
void func_001B2C90(void);

/* State 8: back in the room (after a movie, say). */
void func_005CDA50(void) {
    D_00306590.unk10[2] = 1;
    D_0048BF60.unk64 = 1;
    func_005BAB70();
    func_0034EBC0();
    D_002F88D0[0] = 0;
    func_005BAC20();
    D_0048BF60.unk64 = 0;
    func_00345480();
    func_0034F050();
    func_001B9C30(0x10, 0xFF000000);
    func_005CE710(2);
    func_00341AB0();
    func_001B2C90();
    D_00306590.unk10[2] = 0;
}

void func_00341B20(void);

/* State 5. */
void func_005CE740(void) {
    GameState *g = &D_0048BF60;

    switch (D_0048BF60.step) {
    case 0:
        if (D_0048BF60.unk29 != 0) {
            D_0048BF60.unk29 = 0;
        }
        func_00341B20();
        g->step++;
    case 1:
        g->step++;
    case 2:
        func_00341AB0();
        func_005CE710(7);
        break;
    }
}

void func_001BA7A0(s32 a);
void func_001AD680(s32 stream, s32 a, s32 frames);
s32 func_001AF510(void);
void func_001AF000(s32 cmd, u8 *data, s32 a, s32 size);

/* Once a frame: the game over count-down (state in unkAA, timer unkAB). */
void func_005D3010(void);
#ifdef NON_MATCHING   /* first draft */
void func_005D3010(void) {
    u8 msg[6];

    if (D_0023BB98 != 0) {
        return;
    }
    switch (FIELD(&D_0048BF60, s8, 0xAA)) {
    case 0:
        if (FIELD(&D_0048BF60, s8, 0xA7) == 0) {
            break;
        }
        FIELD(&D_0048BF60, s8, 0xAA)++;
    case 1:
        func_001BA7A0(5);
        func_001AD680(0, 0, 0xA);
        func_001AD680(1, 0, 0xA);
        FIELD(&D_0048BF60, s8, 0xAA)++;
        FIELD(&D_0048BF60, s8, 0xAB) = 0x50;
        break;
    case 2:
        if (--FIELD(&D_0048BF60, s8, 0xAB) <= 0) {
            FIELD(&D_0048BF60, s8, 0xA6) = 1;
            if (func_001AF510() == 1) {
                msg[0] = 4;
                msg[1] = 6;
                msg[2] = 0;
                msg[3] = 0;
                msg[5] = 0;
                msg[4] = D_0048BF60.unk1D;
                func_001AF000(6, msg, 0, 4);
                return;
            }
            FIELD(&D_0048BF60, u8, 0xB6) |= 1 << D_0048BF60.unk1D;
        }
        break;
    }
}
#endif
