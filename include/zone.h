#ifndef ZONE_H
#define ZONE_H

#include "common.h"

/* The room's trigger zones (a list from D_0023BD90, next at 0x28; D_0023BD88 0x80: none). */
typedef struct Zone {
    u8 unk0[0x20];
    s32 top;            /* 0x20 */
    s32 bottom;         /* 0x24 */
    struct Zone *next;  /* 0x28 */
    u8 unk2C[4];
    u8 kind;            /* 0x30 */
    u8 unk31;           /* 0x31: who may set it off */
    u8 unk32;           /* 0x32: priority (the lowest wins) */
    u8 unk33;
    void *who;          /* 0x34: who stepped in */
    u8 unk38[4];
    u8 on;              /* 0x3C */
    u8 unk3D;           /* 0x3D */
    u8 unk3E;           /* 0x3E: the kind of who */
    u8 unk3F;
    union {
        s32 *def;       /* 0x40: (kind 0x85) def[0x10] its switch (D_00470090 + 0x4730) */
        s32 id;         /* (kind 5) */
    } u;
    s32 unk44;          /* 0x44: (kind 3) sub-kind; (kind 4, 13) id */
    s32 unk48;          /* 0x48: (for 2) the item it wants in hand */
    u8 unk4C[0x50 - 0x4C];
    u16 unk50;          /* 0x50 */
    u8 unk52[0x59 - 0x52];
    u8 unk59;           /* 0x59: which of unk5A are set */
    u16 unk5A[8];       /* 0x5A */
    u8 unk6A[2];
    s32 unk6C;          /* 0x6C */
    u8 unk70[2];
    u8 unk72;           /* 0x72 */
} Zone;

extern Zone *D_0023BD90;
extern s16 D_0023BD88;

#endif
