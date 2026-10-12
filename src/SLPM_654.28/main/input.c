#include "common.h"
#include "libpad2.h"
#include "pad.h"

/* Controllers. */

extern u8 D_00292B80[];         /* socket buffer */
extern s32 D_00239F08;          /* which controllers to open: -1 port 1 only, 0 every port of a multitap... */
extern u64 D_00239F10;
extern s32 D_0023BAD8;          /* slots per port */
extern u8 D_0023BADC;
extern u8 D_0023BAE0;
typedef struct { s16 x, y, z, w; } PadVec;
extern PadVec D_0023BAE8[1];

void func_001A0DD0(void *dst, s32 size);
s32 func_001A51A0(void);
void func_001A53B0(s32 i, s16 a, s16 b, s16 c);

s32 func_001A5B20(void) {
    if (sceDbcInit() != 1) {
        return 0;
    }
    return scePad2Init(0) == 1;
}

void func_001A54F0(void) {
    func_001A0DD0(D_00292D80, sizeof(D_00292D80));
    func_001A0DD0(D_00292D40, sizeof(D_00292D40));
    func_001A0DD0(D_00292D20, sizeof(D_00292D20));
    func_001A0DD0(&D_00292D00, sizeof(D_00292D00));
}

void func_001A5B60(s32 port, s32 slot, PadPort *pp) {
    scePad2SocketParam param;
    u8 socket;

    param.number = 0;
    param.port = port;
    param.slot = slot;
    param.flag = 2;
    socket = scePad2CreateSocket(&param, pp->buffer);
    pp->socket = socket;
    if (socket >= 0) {
        pp->state = 1;
        pp->tries = 0;
        pp->port = port;
        pp->slot = slot;
    }
}

/* Not matching yet: first draft; the original copies the neutral state through a stack temporary. */
#ifdef NON_MATCHING
s32 func_001A51A0(void) {
    PadData neutral;
    u64 tmp;
    s32 i;

    if (func_001A5B20() == 0) {
        return 0;
    }
    func_001A54F0();
    D_00292D00.status = 0xFF;
    D_00292D00.buttons = 0xFFFF;
    neutral = D_00292D00;
    D_00292D20[0] = neutral;
    D_00292D60[0].state = 0;
    D_00292D60[0].tries = 0;
    D_00292D60[0].port = 0;
    D_00292D60[0].slot = 0;
    D_00292D60[0].vibrate = 0;
    D_00292D60[0].buffer = D_00292B80;
    if (D_00239F08 == -1) {
        func_001A5B60(0, 0, &D_00292D60[0]);
    } else if (D_00239F08 == 0) {
        for (i = 0; i < D_0023BAD8; i++) {
            func_001A5B60(D_00239F08, i, &D_00292D60[i]);
        }
        if (i <= 0) {
            func_001A5B60(1, 0, &D_00292D60[i]);
        }
    } else {
        func_001A5B60(0, 0, &D_00292D60[0]);
        for (i = 0; i < D_0023BAD8; i++) {
            func_001A5B60(D_00239F08, i, &D_00292D60[i + 1]);
        }
    }
    tmp = D_00239F10;
    *(u64 *)D_0023BAE8 = tmp;
    D_00292D80[0].port = D_00292D60[0].port;
    D_00292D80[0].slot = D_00292D60[0].slot;
    D_0023BADC = 0;
    D_0023BAE0 = 0;
    return 1;
}
#endif

void func_001A53B0(s32 i, s16 a, s16 b, s16 c) {
    D_0023BAE8[i].x = a;
    D_0023BAE8[i].y = b;
    D_0023BAE8[i].z = c;
}

/* Input setup: controllers, then the button-mapping table. */
extern u8 D_002922C0[0x78];
extern u8 *D_0023BAB8;
extern u8 *D_0023BABC;
typedef struct { s16 a, b, c, d; } PadCfg;
extern PadCfg D_00223780[];      /* default analog setting */
extern PadCfg D_0023BAB0[1];

void func_001A0EA0(void);
void func_001A0EB0(PadCfg *cfg, s32 i);

s32 func_001A0E40(void) {
    s32 ok = func_001A51A0();

    func_001A0EA0();
    D_0023BAB8 = D_002922C0;
    D_0023BABC = D_002922C0;
    func_001A0EB0(D_00223780, 0);
    return ok;
}

void func_001A0EA0(void) {
    func_001A0DD0(D_002922C0, sizeof(D_002922C0));
}

void func_001A0EB0(PadCfg *cfg, s32 i) {
    D_0023BAB0[i] = *cfg;
    func_001A53B0(i, D_0023BAB0[i].a, D_0023BAB0[i].b, D_0023BAB0[i].c);
}
