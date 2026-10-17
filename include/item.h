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

extern ItemSlot D_003967D0[0x100];
extern ItemInfo D_00716980[];

#endif
