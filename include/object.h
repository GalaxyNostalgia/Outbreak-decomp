#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"

/* The game's characters and things (players 0x10E0 bytes, enemies 0x1170, ...). Their layouts share
   this start. */

typedef struct {
    u8 unk0[0x828];
    u32 unk828;
    u32 unk82C;
} ObjModel;

typedef struct DrawObjFull {
    s8 active;          /* 0x000 */
    s8 visible;
    u8 type;            /* 0x002 */
    u8 kind;            /* 0x003 */
    u8 unk4[0x18 - 4];
    void (*draw)(struct DrawObjFull *o); /* 0x018 */
    struct DrawObjFull *next; /* 0x01C: next in its pool's list */
    u8 unk20[0x34 - 0x20];
    ObjModel *model;    /* 0x034 */
    u8 unk38[0xF4 - 0x38];
    u8 unkF4[0x175 - 0xF4]; /* 0x0F4 */
    u8 unk175;
    u8 unk176[0x198 - 0x176];
    s8 drawn;           /* 0x198 */
    u8 unk199[0x1E2 - 0x199];
    u16 unk1E2;         /* 0x1E2: the player's id (Options.unk1C is ours) */
    u8 unk1E4[0x200 - 0x1E4];
    u8 unk200[0x41];    /* 0x200 */
    u8 unk241;
    u8 unk242[0x54D - 0x242];
    u8 unk54D;          /* 0x54D */
    u8 unk54E[0x553 - 0x54E];
    u8 unk553;          /* 0x553: casts a shadow */
    u8 unk554[0xB98 - 0x554];
    u8 unkB98;          /* 0xB98 */
    u8 unkB99[0xBAC - 0xB99];
    s32 unkBAC;         /* 0xBAC */
    f32 unkBB0;         /* 0xBB0: D_00712280[kind] */
    u16 unkBB4;         /* 0xBB4: (NPCs) their model + 1 */
    s16 unkBB6;         /* 0xBB6 */
    s16 unkBB8;         /* 0xBB8 */
    u16 unkBBA;         /* 0xBBA */
    u8 costume;         /* 0xBBC */
    u8 unkBBD[0xC7C - 0xBBD];
    u8 unkC7C;          /* 0xC7C: the item in hand (0xFF: none) */
    u8 unkC7D[0xC84 - 0xC7D];
    u8 unkC84[4];       /* 0xC84: effects on the character (script ops 0x39, 0x3A) */
    u8 unkC88[0x20];    /* 0xC88: (effects 400-420) */
    u8 unkCA8[0xCCC - 0xCA8];
    u8 color[4];        /* 0xCCC: r, g, b, a */
    u8 unkCD0[0xCE8 - 0xCD0];
    u32 unkCE8;         /* 0xCE8 */
    u8 unkCEC[0xD28 - 0xCEC];
    s32 unkD28;         /* 0xD28 */
    s32 unkD2C;         /* 0xD2C */
    u8 unkD30[0xFB4 - 0xD30];
    u8 held;            /* 0xFB4: (enemies) what they hold, 0xFF nothing */
    u8 unkFB5[0xFF0 - 0xFB5];
    s8 isNpc;           /* 0xFF0: (players) a computer-controlled partner */
    u8 unkFF1[0x10E0 - 0xFF1];
} DrawObjFull;          /* 0x10E0: a player's size (enemies are 0x1170, things 0xD90) */

#endif
