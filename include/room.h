#ifndef ROOM_H
#define ROOM_H

#include "common.h"

/* The room being played (GAME.BIN's data at 0x470090, 0x6D40 bytes): its model parts in groups,
   each part with a matrix. */

typedef struct {
    s32 model;          /* 0x00: the model to draw (0 or -1: none) */
    s32 nTex;           /* 0x04 */
    s32 tex[0x20];      /* 0x08: texture slots, as indices into texId */
    u8 unk88;           /* 0x88 */
    u8 unk89[0x98 - 0x89];
} RoomPart;             /* 0x98 */

typedef struct {
    s8 active;          /* 0x0000 */
    s8 visible;         /* 0x0001 */
    u8 unk2;
    u8 nGroups;         /* 0x0003 */
    u8 groupCount[0x1C]; /* 0x0004 */
    u8 unk20[0x68 - 0x20];
    u16 texId[0x400];   /* 0x0068: texture slots (into D_004BEEF0, 0x4C each) */
    RoomPart *parts;    /* 0x0868 */
    u8 unk86C[0x880 - 0x86C];
    f32 offset[4];      /* 0x0880: the room's position */
    u8 unk890[0x8A0 - 0x890];
    u8 partFlags[0x80]; /* 0x08A0: 1 shown, 0x80 hidden */
    s32 unk920[0x80];   /* 0x0920 */
    Mat44 partMtx[0x80]; /* 0x0B20 */
    u8 groupFirst[0x18]; /* 0x2B20: each group's first part */
    u8 groupArg[0x18];  /* 0x2B38 */
    Vec3 groupPos[0x18]; /* 0x2B50 */
    s32 mtxOverride[4]; /* 0x2C70: bits: part uses its own matrix pointer */
    Mat44 *partMtxPtr[0x80]; /* 0x2C80 */
} Room;

extern Room D_00470090;

#endif
