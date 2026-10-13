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

/* The fields every character shares (up to 0xDA2); T is the struct's own tag. */
#define OBJ_FIELDS(T) \
    s8 active;          /* 0x000 */ \
    s8 visible; \
    u8 type;            /* 0x002 */ \
    u8 kind;            /* 0x003 */ \
    u8 unk4[2]; \
    u8 unk6;            /* 0x006 */ \
    u8 unk7[0x14 - 7]; \
    void *update;       /* 0x014 */ \
    void (*draw)(struct T *o); /* 0x018 */ \
    struct T *next; /* 0x01C: next in its pool's list */ \
    struct T *prev;     /* 0x020 */ \
    u8 unk24[0x30 - 0x24]; \
    u8 unk30;           /* 0x030 */ \
    u8 unk31[3]; \
    ObjModel *model;    /* 0x034 */ \
    f32 unk38[3];       /* 0x038: position */ \
    u8 unk44[0x90 - 0x44]; \
    s16 unk90;          /* 0x090 */ \
    s16 unk92;          /* 0x092: heading */ \
    s16 unk94;          /* 0x094 */ \
    u8 unk96[0xA8 - 0x96]; \
    s32 unkA8;          /* 0x0A8 */ \
    s32 unkAC; \
    s32 unkB0; \
    f32 unkB4[3];       /* 0x0B4 */ \
    u8 unkC0[0xD8 - 0xC0]; \
    f32 unkD8[3];       /* 0x0D8 */ \
    f32 unkE4[3];       /* 0x0E4 */ \
    f32 unkF0;          /* 0x0F0 */ \
    u8 unkF4[0x175 - 0xF4]; /* 0x0F4: per model part, drawn */ \
    u8 unk175; \
    u8 unk176[0x198 - 0x176]; \
    s8 drawn;           /* 0x198 */ \
    s8 unk199;          /* 0x199 */ \
    u8 unk19A[0x1A8 - 0x19A]; \
    s32 unk1A8;         /* 0x1A8 */ \
    f32 unk1AC[2];      /* 0x1AC */ \
    f32 unk1B4[2]; \
    f32 unk1BC[2]; \
    f32 unk1C4[2]; \
    u8 unk1CC[0x1D8 - 0x1CC]; \
    s32 unk1D8;         /* 0x1D8 */ \
    void *unk1DC;       /* 0x1DC: -> unkAEC */ \
    u8 unk1E0[2]; \
    u16 unk1E2;         /* 0x1E2: the player's id (Options.unk1C is ours) */ \
    u8 unk1E4[0x200 - 0x1E4]; \
    u8 unk200[0x41];    /* 0x200 */ \
    u8 unk241; \
    u8 unk242[0x514 - 0x242]; \
    void *motions[3];   /* 0x514: shared, player and own MotionSets */ \
    u8 unk520[4]; \
    s8 unk524;          /* 0x524 */ \
    u8 unk525[0x544 - 0x525]; \
    s16 unk544;         /* 0x544: health */ \
    s16 unk546;         /* 0x546: maximum health */ \
    u8 unk548[0x54D - 0x548]; \
    u8 unk54D;          /* 0x54D */ \
    u8 unk54E[0x553 - 0x54E]; \
    u8 unk553;          /* 0x553: casts a shadow */ \
    u8 unk554[0xAA8 - 0x554]; \
    f32 unkAA8;         /* 0xAA8 */ \
    u8 unkAAC[4]; \
    s32 unkAB0;         /* 0xAB0 */ \
    u8 unkAB4[0xAEC - 0xAB4]; \
    u8 unkAEC[0xB98 - 0xAEC]; \
    u8 unkB98;          /* 0xB98: the character */ \
    s8 unkB99;          /* 0xB99: uses the shared motions */ \
    u8 unkB9A;          /* 0xB9A */ \
    u8 unkB9B;          /* 0xB9B: condition (0 fine, 1 caution, 2 danger, 3 ...) */ \
    u8 unkB9C;          /* 0xB9C */ \
    u8 unkB9D[0xBA0 - 0xB9D]; \
    s32 unkBA0;         /* 0xBA0: status (2: danger, 8: poisoned...) */ \
    s32 unkBA4;         /* 0xBA4: flags */ \
    u8 unkBA8[4]; \
    s32 unkBAC;         /* 0xBAC */ \
    f32 unkBB0;         /* 0xBB0: D_00712280[kind] */ \
    u16 unkBB4;         /* 0xBB4: (NPCs) their model + 1 */ \
    s16 unkBB6;         /* 0xBB6 */ \
    s16 unkBB8;         /* 0xBB8 */ \
    u16 unkBBA;         /* 0xBBA */ \
    u8 costume;         /* 0xBBC */ \
    u8 unkBBD[3]; \
    f32 unkBC0;         /* 0xBC0 */ \
    f32 unkBC4;         /* 0xBC4 */ \
    u8 unkBC8[0xC7C - 0xBC8]; \
    u8 unkC7C;          /* 0xC7C: the item in hand (0xFF: none) */ \
    u8 unkC7D[0xC84 - 0xC7D]; \
    u8 unkC84[4];       /* 0xC84: effects on the character (script ops 0x39, 0x3A) */ \
    u8 unkC88[0x20];    /* 0xC88: (effects 400-420) */ \
    s16 unkCA8;         /* 0xCA8 */ \
    u8 unkCAA[0xCB8 - 0xCAA]; \
    s32 unkCB8;         /* 0xCB8 */ \
    u8 unkCBC[0xCCC - 0xCBC]; \
    u8 color[4];        /* 0xCCC: r, g, b, a */ \
    u8 unkCD0[0xCE8 - 0xCD0]; \
    u32 unkCE8;         /* 0xCE8 */ \
    s32 unkCEC;         /* 0xCEC */ \
    s32 unkCF0;         /* 0xCF0 */ \
    u8 unkCF4[0xD04 - 0xCF4]; \
    void *motions2[3];  /* 0xD04: (as motions) */ \
    u8 unkD10[0xD28 - 0xD10]; \
    s32 unkD28;         /* 0xD28 */ \
    s32 unkD2C;         /* 0xD2C */ \
    u8 unkD30[0xD90 - 0xD30]; \
    s16 unkD90;         /* 0xD90 */ \
    u8 unkD92[2]; \
    f32 unkD94[3];      /* 0xD94: the start position */ \
    s16 unkDA0;         /* 0xDA0: the start heading */

typedef struct DrawObjFull {
    OBJ_FIELDS(DrawObjFull)
    u8 unkDA2[0xFB4 - 0xDA2];
    u8 held;            /* 0xFB4: (enemies) what they hold, 0xFF nothing */
    u8 unkFB5[0xFF0 - 0xFB5];
    s8 isNpc;           /* 0xFF0: (players) a computer-controlled partner */
    u8 unkFF1[0x10E0 - 0xFF1];
} DrawObjFull;          /* 0x10E0: a player's size (enemies are 0x1170, things 0xD90) */

/* The NPCs (survivors and such, pool D_003E8230[10]: 0x1170 bytes). */
typedef struct Npc {
    OBJ_FIELDS(Npc)
    u8 unkDA2[0xE58 - 0xDA2];
    s32 unkE58[4][2][9]; /* 0xE58 */
    u8 unkF78[0xFB4 - 0xF78];
    u8 held;            /* 0xFB4 */
    u8 slot;            /* 0xFB5: its D_003614F0 record */
    u8 unkFB6;
    s8 unkFB7;          /* 0xFB7 */
    s8 unkFB8[4];       /* 0xFB8 */
    u8 unkFBC[0xFCF - 0xFBC];
    s8 unkFCF;          /* 0xFCF: state */
    s8 unkFD0[4];       /* 0xFD0 */
    u8 unkFD4[2];
    u8 unkFD6;          /* 0xFD6 */
    s8 unkFD7;          /* 0xFD7 */
    u8 unkFD8[0x1020 - 0xFD8];
    s8 unk1020;         /* 0x1020 */
    u8 unk1021[0x102C - 0x1021];
    s8 unk102C;         /* 0x102C */
    u8 unk102D[0x1040 - 0x102D];
    s32 unk1040[3];     /* 0x1040 */
    u8 unk104C[0x1134 - 0x104C];
    u8 unk1134[4];      /* 0x1134 */
    u8 unk1138[0x1150 - 0x1138];
    s32 unk1150;        /* 0x1150 */
    u8 unk1154;         /* 0x1154 */
    u8 unk1155[0x1162 - 0x1155];
    s8 unk1162;         /* 0x1162 */
    u8 unk1163[0x1170 - 0x1163];
} Npc;                  /* 0x1170 */

#endif
