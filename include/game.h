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
    s16 unkA;           /* 0x0A */
    s16 unkC;           /* 0x0C */
    u8 unkE[0x18 - 0xE];
    s32 unk18;          /* 0x18: frames played (the game ends at 540000, 2.5 hours) */
    u8 unk1C;
    u8 unk1D;           /* 0x1D: our player number */
    u8 unk1E[0x20 - 0x1E];
    s8 unk20[7];        /* 0x20: (indexed by unk27) the player-id table to use */
    s8 unk27;           /* 0x27 */
    s8 unk28;           /* 0x28 */
    u8 unk29;           /* 0x29 */
    u8 unk2A;
    s8 unk2B;           /* 0x2B: the characters are set up already */
    u8 unk2C[0x36 - 0x2C];
    s8 unk36;           /* 0x36 */
    u8 unk37[0x3A - 0x37];
    s8 unk3A;           /* 0x3A */
    u8 unk3B;           /* 0x3B */
    s8 unk3C;           /* 0x3C: a movie is playing */
    u8 unk3D[2];
    u8 unk3F;           /* 0x3F: the player motions are loaded */
    u8 unk40[0x54 - 0x40];
    s32 bgm[3];         /* 0x54: the music on each ADX stream (main's options.c) */
    u8 unk60[2];
    s8 unk62;           /* 0x62: offline */
    s8 unk63;           /* 0x63 */
    s8 unk64;           /* 0x64: a room is being set up */
    u8 unk65[0xA1 - 0x65];
    u8 unkA1[6];        /* 0xA1: per player, its voice bank (character kind + 1; 0: none) */
    s8 unkA7;           /* 0xA7 */
    u8 unkA8[0xBA - 0xA8];
    u8 unkBA;           /* 0xBA: (game start byte 2) */
    u8 unkBB;
    s8 unkBC;           /* 0xBC */
    u8 unkBD;
    u8 unkBE;           /* 0xBE: enemy set entries (D_0071A030) */
    s8 unkBF;           /* 0xBF: characters in the game */
    u8 unkC0[0xC4 - 0xC0];
    u8 unkC4;           /* 0xC4: the offline overlay's per-frame work is paused */
    u8 unkC5;
    u8 unkC6;           /* 0xC6: (game start byte 1) */
    u8 unkC7[0xCA - 0xC7];
    s8 unkCA;           /* 0xCA */
    u8 unkCB;           /* 0xCB: (online) a bit per player */
    u8 unkCC[0xD0 - 0xCC];
} GameState;

extern GameState D_0048BF60;

void func_005CE710(s8 state);

#endif
