#include "common.h"
#include "game.h"
#include "options.h"

/* The game's object pools: cleared for each game. Names are what they hold so far. */

void *memset(void *dst, s32 c, u32 n);

extern s32 D_0053AA10[0x2BC];
extern s32 D_0053A910[0x40];

void func_001B0090(void) {
    s32 *p = D_0053AA10;
    s32 i;

    for (i = 0; i < 0x2BC; i++) {
        *p++ = -1;
    }
}

void func_001B0160(void) {
    s32 *p = D_0053A910;
    s32 i;

    for (i = 0; i < 0x40; i++) {
        *p++ = -1;
    }
}

void func_001B11C0(void *p) {
    memset(p, 0, 0xC84);
}

void func_001B11D0(void *p) {
    memset(p, 0, 0x1F44);
}

void func_001B11E0(void *p) {
    memset(p, 0, 0x3EC);
}

extern u8 D_004BBA40[0x1F44];
extern u8 D_004B9AF0[0x1F44];
extern u8 D_004B7BA0[0x1F44];
extern u8 D_004A8180[8][0x1F44];
extern u8 D_004948D0[10][0x1F44];
extern u8 D_0048FDB0[6][0xC84];
extern u8 D_0048E620[6][0x3EC];
extern u8 D_0048C030[0x3E];

void func_001B11F0(void) {
    s32 i;

    func_001B11D0(D_004BBA40);
    func_001B11D0(D_004B9AF0);
    func_001B11D0(D_004B7BA0);
    for (i = 0; i < 8; i++) {
        func_001B11D0(D_004A8180[i]);
    }
    for (i = 0; i < 10; i++) {
        func_001B11D0(D_004948D0[i]);
    }
    for (i = 0; i < 6; i++) {
        func_001B11C0(D_0048FDB0[i]);
    }
    for (i = 0; i < 6; i++) {
        func_001B11E0(D_0048E620[i]);
    }
    for (i = 0x3D; i >= 0; i--) {
        D_0048C030[i] = 0xFF;
    }
}

/* 0x480 blocks of 0x190 bytes, handed out from a stack of pointers. */
extern u8 D_004CA110[0x480][0x190];
extern u8 *D_004BEEF0[];        /* the stack's end (it grows down) */
extern u8 **D_0023BE20;         /* top */
extern s16 D_0023BE24;          /* free */

void func_001B0230(void) {
    u8 *p;
    s16 i;

    memset(D_004CA110, 0, sizeof(D_004CA110));
    p = D_004CA110[0x47F];
    D_0023BE20 = D_004BEEF0;
    for (i = 0; i < 0x480; i++) {
        *--D_0023BE20 = p;
        p -= 0x190;
    }
    D_0023BE24 = 0x480;
}

extern u8 D_004BDA90[0x258];

void func_001B02B0(void) {
    s16 i;

    for (i = 0; i < 0x258; i++) {
        memset((u8 *)D_004BEEF0 + i * 0x4C, 0, 0x4C);
        D_004BDA90[i] = 0;
    }
}

extern u8 D_0048AC10[0x1254];

void func_001AE6D0(void) {
    memset(&D_0048BF60, 0, sizeof(D_0048BF60));
    memset(D_0048AC10, 0, sizeof(D_0048AC10));
}

/* Left empty in the release (it took arguments). */
void func_001B69A0(void) {
}

void func_001AE7F0(void) {
    D_00306590.unkD = 1;
}
