#ifndef MODEL_H
#define MODEL_H

#include "common.h"

/* A loaded model: 16 slots of 0x1468 bytes (the characters' are D_0047F4D0[4]), each with a
   0x1300-byte buffer after them. */
typedef struct {
    u32 *unk0;          /* 0x000: nblocks blocks (-> unk820) */
    u32 *unk4;          /* 0x004: (-> unkA20) */
    u16 ids[0x400];     /* 0x008: nids entries of D_004BDA90 / D_004BEEF0 */
    u8 *buf;            /* 0x808: nparts entries of 0x98 bytes (word 0: a packet group) */
    s32 unk80C;         /* 0x80C: a memory handle */
    u16 unk810;
    u16 nparts;         /* 0x812 */
    u16 nids;           /* 0x814 */
    u16 nblocks;        /* 0x816 */
    u8 *unk818;         /* -> unkC20 */
    u8 unk81C;          /* 0x81C: 1: unk4's blocks are shared */
    u8 unk81D[3];
    u8 unk820[0x200];
    u8 unkA20[0x200];
    u8 unkC20[0x1460 - 0xC20];
    u8 *unk1460;        /* 0x1460: (characters) their D_0023BCD0 block */
    u8 *unk1464;        /* 0x1464: unk1460 + 0x74 */
} Slot1468;

/* Materials: 0x258 of them (D_004BEEF0; D_004BDA90 says which are in use), a model's listed in
   Slot1468.ids. Colours as in the model file, in another order. */
typedef struct {
    s32 unk0;
    f32 col2[4];        /* 0x04: the file's second colour */
    f32 col3[4];        /* 0x14: its third */
    f32 col1[4];        /* 0x24: its first */
    s32 unk34[4];
    s32 tex;            /* 0x44: texture handle (0: none) */
    s32 power;          /* 0x48 */
} Material;             /* 0x4C */

/* A model part (Slot1468.buf, 0x98 bytes each). */
typedef struct {
    s32 group;          /* 0x00: its packet group (-1: none) */
    s32 nmats;          /* 0x04 */
    s32 mats[0x20];     /* 0x08: indices into the model's material ids */
    s8 lit;             /* 0x88: it has a lighting block (func_001D5B40) */
    u8 unk89[0x98 - 0x89];
} ModelPart;

/* A material in a model file (table 9). */
typedef struct {
    f32 col1[4];        /* 0x00 */
    f32 col2[4];        /* 0x10 */
    f32 col3[4];        /* 0x20 */
    f32 power;          /* 0x30 */
    s32 textured;       /* 0x34 */
    u8 unk38[0x100 - 0x38];
    s32 tex;            /* 0x100: the texture (table 10 entry) */
} FileMaterial;

/* The game's work memory: one block taken from the heap when a game starts, cut into slots; its
   first part is handed out as part buffers (count entries of 0x98 bytes). */
typedef struct {
    u32 mark[2];        /* heap mark the block is taken under */
    s16 count;          /* 0x08: entries of 0x98 bytes at unk10 */
    s16 used;           /* 0x0A: entries handed out */
    s16 nbufs;          /* 0x0C: buffers handed out */
    u8 unkE[2];
    u8 *unk10;          /* 0x10: count * 0x98 bytes */
    u32 unk14;          /* 0x14: the slots below, 0x2B680 bytes */
    u8 unk18[4];
    u8 *bufs[100];      /* 0x1C */
    s16 counts[100];    /* 0x1AC: entries in each */
    u8 owner[100];      /* 0x274: who has it + 1 (0: free to reuse) */
    u8 unk2D8[0x33C - 0x2D8];
} GameWork;

extern GameWork D_0029B140;

#endif
