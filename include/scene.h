#ifndef SCENE_H
#define SCENE_H

#include "common.h"

/* The scene's lighting and fog (main's data at 0x305FA0), set up by GAME.BIN's func_0067ED70 and
   handed to the renderer by func_0067F290's three modes. */

typedef struct {
    f32 w[26];          /* +0x04 and +0x24: vectors; +0x34: direction (mode 0 sets it from angles) */
} Light;                /* 0x68: what renderer commands 0x5A-0x5C take */

typedef struct {
    s32 x, y, z, pad;
} Angles;

typedef struct {
    Light a[3];         /* 0x000: mode 0 */
    Light b[3];         /* 0x138: mode 2 */
    f32 fogNear;        /* 0x270 */
    f32 fogFar;         /* 0x274 */
    u8 ambient[4];      /* 0x278: mode 0, plus offset[] */
    Angles angA[3];     /* 0x27C */
    Angles angB[3];     /* 0x2AC */
    u8 fogColor[3];     /* 0x2DC */
    u8 unk2DF;
    u16 enable[6];      /* 0x2E0: a[0..2], b[0..2] */
    u8 color1[4];       /* 0x2EC: mode 1 */
    s16 offset1[4];     /* 0x2F0 */
    u8 color2[4];       /* 0x2F8: mode 2 */
    s16 offset2[4];     /* 0x2FC */
    Vec3 unk304;        /* 0x304 */
    Vec3 unk310;        /* 0x310 */
    s8 mode;            /* 0x31C: the mode handed to the renderer, -1 none */
    s8 unk31D;
    s8 unk31E;
    u8 flags;           /* 0x31F */
    s32 unk320;
    s32 unk324;
    s32 unk328;
    s16 unk32C;
    s16 unk32E;
    s8 offset0[4];      /* 0x330: added to ambient[] */
} Scene;                /* 0x334 */

extern Scene D_00305FA0;

#endif
