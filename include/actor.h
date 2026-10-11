#ifndef ACTOR_H
#define ACTOR_H

#include "common.h"

/* Game object shared by players and enemies (game overlay). Field names are
   guesses until more code is decompiled; offsets are what the code uses. */
typedef struct Actor {
    /* 0x000 */ u8 unk0[8];
    /* 0x008 */ u8 state;
    /* 0x009 */ u8 substate;
    /* 0x00A */ u8 unkA[0x38 - 0x0A];
    /* 0x038 */ Vec3 pos;
    /* 0x044 */ u8 unk44[0x80 - 0x44];
    /* 0x080 */ Vec3 prevPos;
    /* 0x08C */ u8 unk8C[0xBA3 - 0x8C];
    /* 0xBA3 */ s8 unkBA3;
    /* 0xBA4 */ u8 unkBA4[0xBD4 - 0xBA4];
    /* 0xBD4 */ f32 unkBD4;
    /* 0xBD8 */ u8 unkBD8[0xC3C - 0xBD8];
    /* 0xC3C */ f32 unkC3C;
    /* 0xC40 */ u8 unkC40[0xD00 - 0xC40];
    /* 0xD00 */ u8 unkD00[0x18];
    /* 0xD18 */ u8 unkD18[0x10];
} Actor;

#endif
