#ifndef SGL_H
#define SGL_H

#include "common.h"

/* SINGLE.BIN: what the offline script (r%03d.sgl) has for each room (D_0084E2B0, by room). */

/* A place a partner can go to (0x40 bytes). */
typedef struct {
    u8 used;            /* 0x00 */
    s8 n;               /* 0x01: how many ways lead on (next) */
    u16 who;            /* 0x02 */
    f32 pos[3];         /* 0x04 */
    s32 sw;             /* 0x10: the switch that must allow it (-1: none) */
    s8 next[4];         /* 0x14: the places it leads to (-1: none) */
    u16 kind[4];        /* 0x18: each way's kind (1-13: doors and such; how the partner moves) */
    u8 unk20[4];
    u16 cost[4];        /* 0x24: each way's length */
    u32 keys[4];        /* 0x2C: each way's four conditions (a byte each, on the room's ids) */
    u8 unk3C[4];
} SglPlace;

/* A switch the script keeps per player (what script op func_006CDA90 sets). */
typedef struct {
    u8 on;              /* 0x0 */
    u8 kind;            /* 0x1: 0 by id, 2 by id and id2 */
    u8 id;              /* 0x2 */
    u8 id2;             /* 0x3 */
    s16 val;            /* 0x4 */
    u8 who;             /* 0x6: (kind 1) who it is for: 2 a character, 3 a player, 4 partners... */
    u8 whoArg;          /* 0x7: that character or player */
    u8 unk8[0x10 - 8];
} SglSwitch;

typedef struct {
    s32 used;           /* 0x00 */
    s32 entry;          /* 0x04: its entry in the script */
    s32 nplaces;        /* 0x08 */
    SglPlace *places;   /* 0x0C */
    u8 *routes;         /* 0x10: [to][from][4]: which of from's next leads to `to` (0xFF: none) */
    u8 seen[0x80];      /* 0x14: per place, a bit per player */
    s32 *unk94;         /* 0x94 */
    s32 n;              /* 0x98: switches */
    SglSwitch *sw;      /* 0x9C */
    u8 *ids;            /* 0xA0: per id, set by func_006CE9A0 */
} SglRoom;              /* 0xA4 */

/* A link between rooms: a place of one room (and variant) leading on (0xC bytes). */
typedef struct {
    s8 room;            /* 0x0 */
    s8 variant;         /* 0x1 */
    s8 place;           /* 0x2 */
    s8 unk3;            /* 0x3 */
    u8 unk4[0xC - 4];
} SglLink;

/* The links (and a byte per link per player in the buffers). */
typedef struct {
    s32 size;           /* 0x0: how many links (each player's buffer's size) */
    SglLink *links;     /* 0x4 */
    u8 *unk8;           /* 0x8 */
    u8 *buf;            /* 0xC */
} SglBufs;

extern SglRoom D_0084E2B0[0x80];
extern SglBufs D_0084E2A0;

#endif
