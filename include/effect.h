#ifndef EFFECT_H
#define EFFECT_H

#include "common.h"
#include "object.h"

/* The effects (0x250 bytes): 0xA8 of them in two lists (by list) plus a free list. In a list,
   next/prev link them; in the free list, prev is the next free one. */
typedef struct Obj250L {
    s8 unk0;            /* 0x000: in use */
    s8 unk1;            /* 0x001: visible */
    u8 unk2;            /* 0x002: list + 6 */
    u8 kind;            /* 0x003 */
    u8 unk4;
    u8 unk5;            /* 0x005 */
    u8 unk6;            /* 0x006: its number in the list when made */
    u8 list;            /* 0x007 */
    u8 unk8[0xC - 8];
    s16 unkC;           /* 0x00C */
    s16 unkE;           /* 0x00E */
    s16 unk10;
    s16 unk12;
    void *update;       /* 0x014 */
    void *draw;         /* 0x018 */
    struct Obj250L *next; /* 0x01C */
    struct Obj250L *prev; /* 0x020 */
    void *target;       /* 0x024 */
    u8 unk28[0x2C - 0x28];
    DrawObjFull *owner; /* 0x02C: drawn only when its owner was */
    u8 unk30;
    u8 nparts;          /* 0x031 */
    u8 unk32[0x38 - 0x32];
    Vec3 pos;           /* 0x038 */
    u8 unk44[0x114 - 0x44];
    s16 partIds[16];    /* 0x114 */
    void *parts[16];    /* 0x134 */
    u8 unk174[0x1F0 - 0x174];
    Mat44 *mtx;         /* 0x1F0: follows this matrix (pos is then an offset) */
    void *unk1F4;       /* 0x1F4 */
    s32 unk1F8;         /* 0x1F8 */
    u8 unk1FC[3];
    s8 unk1FF;          /* 0x1FF */
    u32 flags;          /* 0x200 */
    s16 id;             /* 0x204 */
    s16 unk206;         /* 0x206 */
    u8 unk208[0x23A - 0x208];
    s16 unk23A;         /* 0x23A */
    u8 unk23C[0x240 - 0x23C];
    s16 unk240;         /* 0x240 */
    u8 unk242[0x250 - 0x242];
} Obj250L;

#endif
