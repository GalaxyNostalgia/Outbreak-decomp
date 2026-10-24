#ifndef ENEMY_H
#define ENEMY_H

#include "common.h"
#include "model.h"

/* The models, textures and motions loaded for the room's enemies (and NPCs): 16 model slots,
   16 texture slots and 4 motion buffers, shared by every object of a kind and counted. A slot's
   flags say what holds it: 1 the room, 2 (flag 0x40000000), 4 the scenario (0x80000000). Group
   1 is the texture id range 0x1200.., group 2 0x1400... */

typedef struct {
    u8 refs;            /* 0x0 */
    s8 index;           /* 0x1 */
    u8 unk2;
    u8 flags;           /* 0x3 */
    Slot1468 *model;    /* 0x4 */
    u8 group;           /* 0x8 */
    u8 unk9;
    u16 id;             /* 0xA: model id (| 0x8000: the _N variant) */
} EnemyModel;           /* 0xC */

typedef struct {
    u8 refs;            /* 0x00 */
    u8 unk1;
    s16 tex[6];         /* 0x02: texture ids (-1 ends) */
    u8 index;           /* 0x0E */
    u8 flags;           /* 0x0F */
    u8 group;           /* 0x10 */
    u8 unk11;
    u16 id;             /* 0x12 */
} EnemyTex;             /* 0x14 */

typedef struct {
    u8 count;           /* 0x0 */
    u8 per;             /* 0x1 */
    u8 unk2;
    u8 unk3;
    u16 anims[0x640];   /* 0x4 */
} EnemyMotions;         /* 0xC84 */

typedef struct {
    u8 used;            /* 0x0 */
    s8 index;           /* 0x1 */
    s16 sub;            /* 0x2 */
    EnemyMotions *buf;  /* 0x4: 0x1000 bytes */
    u16 type;           /* 0x8: the enemy type */
    s16 unkA;           /* 0xA */
    u16 kind;           /* 0xC: its motion file */
    u16 unkE;
} EnemyMotionSlot;      /* 0x10 */

extern EnemyMotionSlot D_00321350[4];
extern EnemyModel D_00321390[0x10];
extern EnemyTex D_00321450[0x10];

/* An enemy type's variants: what they need (D_00357500[type].defs[variant]). */
typedef struct {
    u8 model;           /* 0x0 */
    u8 tex;             /* 0x1 */
    u8 unk2;            /* 0x2: (0xFF: no motions) */
    u8 unk3;
    u8 shared;          /* 0x4 */
    u8 unk5;
    u8 unk6;            /* 0x6 */
    s8 variant;         /* 0x7 */
    f32 scale;          /* 0x8 */
    s8 unkC;            /* 0xC: passed to func_005902F0 */
    s8 hasData;         /* 0xD: has its own character data block */
    s16 data;           /* 0xE: which (D_00358310[0][data], block data + 5) */
} ThingDef;             /* 0x10 */

typedef struct {
    ThingDef *defs;
    u32 unk4;
} ThingKind;

extern ThingKind D_00357500[];      /* (SUBMAIN.BIN) by enemy type */

/* The enemy kinds the room's enemy sets use, for the two enemy groups (kinds and the motion
   file of each); prev* are the lists before. */
typedef struct {
    s32 kinds[2][32];   /* 0x000 */
    s32 models[2][32];  /* 0x100 */
    s32 prevKinds[2][32]; /* 0x200 */
    s32 prevModels[2][32]; /* 0x300 */
} RoomEnemyKinds;

typedef struct {
    u8 index;           /* 0x00 */
    u8 obj;             /* 0x01: its thing (0xFF: none) */
    u8 type;            /* 0x02: the enemy type */
    u8 variant;         /* 0x03 */
    u8 flag;            /* 0x04: the scene flag it needs */
    u8 unk5;
    u8 unk6;            /* 0x06 */
    u8 unk7;            /* 0x07: 0x80 its target is an enemy; & 0xF which */
    s32 rot[2];         /* 0x08 */
    s32 pos[3];         /* 0x10 */
    s16 hp;             /* 0x1C */
    s16 maxHp;          /* 0x1E */
    u8 unk20;
    s8 unk21;
    u8 room;            /* 0x22 */
    u8 unk23[3];
    u8 always;          /* 0x26: placed whatever the scenario */
    u8 unk27[2];
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    u8 unk2C;
    u8 unk2D;
    u8 unk2E;
    u8 unk2F[10];       /* 0x2F: (unk2F[0] 0xFF: not for types 0xA, 0x13, 0x1D, 0x29) */
    u8 unk39;
    u8 unk3A;
    u8 unk3B[0x42 - 0x3B];
    u8 unk42;           /* 0x42: D_006FB4E0 row */
    u8 unk43[2];
    u8 unk45;           /* 0x45 */
    u8 unk46;
    u8 unk47;
    u8 unk48;
    u8 unk49;
    u8 unk4A;
    u8 unk4B;
    u8 unk4C;
    u8 unk4D;
    u8 unk4E;
    u8 unk4F;
    u8 unk50;
    u8 unk51;
    u8 unk52;
    u8 unk53;
    u8 unk54;
    u8 unk55;
    u8 unk56;
    u8 scale[3];        /* 0x57: % (0: the type's) */
    u8 unk5A;
    u8 unk5B[0x60 - 0x5B];
} EnemySet;

typedef struct {
    u8 unk0[0x2A0];
    u8 roomVariant[0x80]; /* 0x2A0: by room (D_0048AEB0) */
    u8 unk320[0xBA0 - 0x320];
    u16 areaA[0x80];    /* 0xBA0: by room */
    u16 areaB[0x80];    /* 0xCA0 */
    u16 type[2];        /* 0xDA0: each group's enemy type now (0xFFFF: none) */
    u16 sub[2];         /* 0xDA4 */
    u16 playerMotions;  /* 0xDA8: the player motion set loaded */
    u16 unkDAA;
    u16 prevType[2];    /* 0xDAC */
    u16 modelA[0x80];   /* 0xDB0: by area */
    u16 modelB[0x80];   /* 0xEB0 */
    u32 flags[0x10];    /* 0xFB0 */
} SceneEnemies;

/* The NPC models a scenario loads (0xFFFF: none). */
typedef struct {
    u16 model;
    u16 unk2;
} NpcModel;

extern NpcModel D_0048A010[][4];    /* per scenario */

extern RoomEnemyKinds D_0048A810;
extern SceneEnemies D_0048AC10;

#endif
