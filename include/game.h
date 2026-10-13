#ifndef GAME_H
#define GAME_H

#include "common.h"

/* The game's state while a scenario runs (0x48BF60, 0xD0 bytes; cleared when a game starts). */
typedef struct {
    s8 state;           /* 0x00: the game task's state (func_005CD660's switch) */
    s8 step;            /* 0x01: steps inside a state */
    s8 step2;           /* 0x02 */
    s8 step3;           /* 0x03 */
    u8 unk4[4];
    s16 timer;          /* 0x08 */
    u8 unkA[0x1D - 0xA];
    u8 unk1D;           /* 0x1D: our player number */
    u8 unk1E[0x28 - 0x1E];
    s8 unk28;           /* 0x28 */
    u8 unk29;           /* 0x29 */
    u8 unk2A[0x3A - 0x2A];
    s8 unk3A;           /* 0x3A */
    u8 unk3B;           /* 0x3B */
    s8 unk3C;           /* 0x3C: a movie is playing */
    u8 unk3D[0x54 - 0x3D];
    s32 bgm[3];         /* 0x54: the music on each ADX stream (main's options.c) */
    u8 unk60[3];
    s8 unk63;           /* 0x63 */
    u8 unk64[0xBC - 0x64];
    s8 unkBC;           /* 0xBC */
    u8 unkBD[0xC4 - 0xBD];
    u8 unkC4;           /* 0xC4: the offline overlay's per-frame work is paused */
    u8 unkC5[0xD0 - 0xC5];
} GameState;

extern GameState D_0048BF60;

void func_005CE710(s8 state);

#endif
