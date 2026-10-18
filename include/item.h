#ifndef ITEM_H
#define ITEM_H

#include "common.h"

/* The room's items (0x100 of them, D_003967D0). */
typedef struct {
    s8 active;          /* 0x00 */
    s8 unk1;            /* 0x01 */
    u8 index;           /* 0x02: its slot */
    s8 unk3;            /* 0x03 */
    u16 place;          /* 0x04 */
    u16 unk6;           /* 0x06: the item id */
    f32 pos[3];         /* 0x08 */
    u16 unk14;          /* 0x14 */
    u16 unk16;          /* 0x16 */
    s32 flags;          /* 0x18 */
    void *obj;          /* 0x1C: its thing in the room (flag 0x20000) */
    s32 unk20;          /* 0x20 */
    s16 unk24;          /* 0x24: count */
    u8 unk26[2];
    s16 owner;          /* 0x28: (character + 1) */
    u8 unk2A;
    u8 type;            /* 0x2B: D_00716980's entry (0xFF: none) */
    u8 unk2C;           /* 0x2C */
    u8 unk2D;           /* 0x2D */
    u16 unk2E;          /* 0x2E */
    f32 unk30[3];       /* 0x30 */
} ItemSlot;

/* Per item type (0x18 bytes, D_00716980). */
typedef struct {
    u32 flags;          /* 0x00 */
    u8 unk4[0xC - 4];
    s8 unkC;            /* 0x0C: (D_006EF3A0's column) */
    s8 unkD;            /* 0x0D: (D_007175A0's entry) */
    s8 weapon;          /* 0x0E: the weapon sound bank (-1: none) */
    s8 unkF;            /* 0x0F */
    u8 unk10[2];
    u16 id;             /* 0x12 */
    u8 unk14;           /* 0x14: its kind (D_006D5C30: the thing's update) */
    u8 unk15[3];
} ItemInfo;


/* An item's thing in the room (func_00560E80's; 0x250 bytes from the pool D_00386950). */
typedef struct ItemObj {
    s8 active;          /* 0x000 */
    u8 unk1;            /* 0x001: its model is loaded */
    u8 unk2;            /* 0x002: (8: an item) */
    u8 unk3;
    u8 unk4;            /* 0x004 */
    u8 unk5;
    u8 slot;            /* 0x006: its place in the pool */
    u8 unk7[0xE - 7];
    s16 unkE;           /* 0x00E */
    u8 unk10[2];
    s16 unk12;          /* 0x012 */
    void *update;       /* 0x014: by the item's kind (D_006D5C30) */
    void *draw;         /* 0x018 */
    struct ItemObj *prev;   /* 0x01C */
    struct ItemObj *next;   /* 0x020 */
    u8 unk24[0x30 - 0x24];
    u8 partOn[6];       /* 0x030 */
    u16 id;             /* 0x036 */
    u8 unk38[8];
    Mat44 parts[6];     /* 0x040: its bones (from the room's object model) */
    f32 mat[16];        /* 0x1C0 */
    f32 pos[3];         /* 0x200 */
    u8 unk20C[0x218 - 0x20C];
    f32 unk218;         /* 0x218 */
    s16 rot[3];         /* 0x21C */
    u8 unk222[0x22C - 0x222];
    f32 scale[3];       /* 0x22C */
    s16 unk238;         /* 0x238 */
    u8 unk23A[2];
    s32 flags;          /* 0x23C */
    u8 index;           /* 0x240 */
    u8 unk241[1];       /* 0x241 */
    s16 model;          /* 0x242 */
    u8 unk244[0x250 - 0x244];
} ItemObj;

extern ItemSlot D_003967D0[0x100];
extern ItemInfo D_00716980[];

#endif
