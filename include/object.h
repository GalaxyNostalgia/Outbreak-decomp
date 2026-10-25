#ifndef OBJECT_H
#define OBJECT_H

#include "common.h"
#include "ctrl.h"

/* A two-bone chain (an arm or a leg) the model bends towards a target (game/ik.c). */
typedef struct {
    f32 m0[16];         /* 0x00: bone 0 relative to its aim */
    f32 m1[16];         /* 0x40: bone 1 relative to its aim */
    s16 mode;           /* 0x80 */
    u16 on;             /* 0x82 */
    f32 angle;          /* 0x84 */
    u8 *b0;             /* 0x88 */
    u8 *b1;             /* 0x8C */
    f32 len0;           /* 0x90 */
    f32 len1;           /* 0x94 */
    u8 unk98[8];
    f32 up[3];          /* 0xA0 */
    f32 tgt[3];         /* 0xAC: where its end reaches */
    u8 unkB8[4];
    f32 unkBC;          /* 0xBC: the end's twist */
} IkChain;              /* 0xC0 */

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
    u8 unk7; \
    u8 act[4];          /* 0x008: action, its step and sub-steps */ \
    s16 unkC;           /* 0x00C: (players) frames of a status left */ \
    u8 unkE[0x14 - 0xE]; \
    void *update;       /* 0x014 */ \
    void (*draw)(struct T *o); /* 0x018 */ \
    struct T *next; /* 0x01C: next in its pool's list */ \
    struct T *prev;     /* 0x020 */ \
    s32 unk24;          /* 0x024: (players) the aim's target */ \
    u8 unk28[0x30 - 0x28]; \
    u8 unk30;           /* 0x030 */ \
    u8 unk31[3]; \
    ObjModel *model;    /* 0x034 */ \
    f32 unk38[3];       /* 0x038: position */ \
    f32 unk44[3];       /* 0x044: the position before the last move */ \
    u8 unk50[0x80 - 0x50]; \
    f32 unk80[3];       /* 0x080: the position last frame */ \
    u8 unk8C[0x90 - 0x8C]; \
    s16 unk90;          /* 0x090 */ \
    s16 unk92;          /* 0x092: heading */ \
    s16 unk94;          /* 0x094 */ \
    s16 unk96; \
    u8 unk98[8]; \
    s16 unkA0[4];       /* 0x0A0: unk90-96 before the last move */ \
    s32 unkA8;          /* 0x0A8 */ \
    s32 unkAC; \
    s32 unkB0; \
    f32 unkB4[3];       /* 0x0B4 */ \
    f32 unkC0[3];       /* 0x0C0: (a speed) */ \
    u8 unkCC[0xD8 - 0xCC]; \
    f32 unkD8[3];       /* 0x0D8 */ \
    f32 unkE4[3];       /* 0x0E4 */ \
    f32 unkF0;          /* 0x0F0 */ \
    u8 unkF4[0x170 - 0xF4]; /* 0x0F4: per model part, drawn */ \
    f32 unk170;         /* 0x170: the height it falls from */ \
    u8 unk174;          /* 0x174: the floor's kind */ \
    u8 unk175; \
    u16 unk176;         /* 0x176: the floor's id */ \
    f32 unk178[3];      /* 0x178: the floor's normal */ \
    f32 unk184;         /* 0x184 */ \
    u8 unk188;          /* 0x188 */ \
    u8 unk189[0x190 - 0x189]; \
    u32 unk190;         /* 0x190: (draw flags) */ \
    u8 unk194[0x198 - 0x194]; \
    s8 drawn;           /* 0x198 */ \
    s8 unk199;          /* 0x199 */ \
    u8 unk19A[2]; \
    f32 unk19C;         /* 0x19C: where the shadow falls (x, z) */ \
    f32 unk1A0;         /* 0x1A0 */ \
    u16 unk1A4;         /* 0x1A4: the shadow's length (two-point shadows) */ \
    s16 unk1A6;         /* 0x1A6: and its direction */ \
    s32 unk1A8;         /* 0x1A8: the blob shadow (s16 [1]: its size) */ \
    f32 unk1AC[2];      /* 0x1AC */ \
    f32 unk1B4[2]; \
    f32 unk1BC[2]; \
    f32 unk1C4[2]; \
    f32 unk1CC[3];      /* 0x1CC: the collision sphere's offset last frame */ \
    s32 unk1D8;         /* 0x1D8 */ \
    void *unk1DC;       /* 0x1DC: -> unkAEC */ \
    u8 unk1E0[2]; \
    u16 unk1E2;         /* 0x1E2: the player's id (Options.unk1C is ours) */ \
    u8 unk1E4[0x200 - 0x1E4]; \
    u8 unk200[4];       /* 0x200 */ \
    u8 unk204[0x20];    /* 0x204: the motion's state */ \
    u8 unk224[0x241 - 0x224]; \
    u8 unk241; \
    u8 unk242[0x4E4 - 0x242]; \
    s32 unk4E4[3];      /* 0x4E4 */ \
    u8 unk4F0[0x514 - 0x4F0]; \
    void *motions[3];   /* 0x514: shared, player and own MotionSets */ \
    s16 unk520;         /* 0x520 */ \
    u8 unk522[2]; \
    s8 unk524;          /* 0x524 */ \
    u8 unk525[0x528 - 0x525]; \
    f32 unk528[3];      /* 0x528: (players) where the probe hit */ \
    f32 unk534[3];      /* 0x534: (players) the direction pushed against */ \
    u8 unk540;          /* 0x540: (players) the probe hit something */ \
    u8 unk541[3]; \
    s16 unk544;         /* 0x544: health */ \
    s16 unk546;         /* 0x546: maximum health */ \
    s16 unk548;         /* 0x548 */ \
    u16 unk54A;         /* 0x54A */ \
    u8 unk54C; \
    u8 unk54D;          /* 0x54D */ \
    u8 unk54E;          /* 0x54E */ \
    u8 unk54F;          /* 0x54F */ \
    u8 unk550[2]; \
    u8 unk552;          /* 0x552: the bone others look at */ \
    u8 unk553;          /* 0x553: casts a shadow */ \
    u8 unk554[0x560 - 0x554]; \
    IkChain ik[7];      /* 0x560: (players) legs (0, 1), arms, ... */ \
    f32 unkAA0;         /* 0xAA0: (feet) the floor under the left foot */ \
    f32 unkAA4;         /* 0xAA4: under the right */ \
    f32 unkAA8;         /* 0xAA8 */ \
    u8 unkAAC[4]; \
    s32 unkAB0;         /* 0xAB0 */ \
    u8 unkAB4[4]; \
    u8 unkAB8;          /* 0xAB8 */ \
    u8 unkAB9;          /* 0xAB9 */ \
    u8 unkABA;          /* 0xABA */ \
    u8 unkABB[0xAD4 - 0xABB]; \
    u8 unkAD4;          /* 0xAD4: a reaction pending */ \
    u8 unkAD5;          /* 0xAD5: which (D_006DDA80) */ \
    u8 unkAD6[0xAEC - 0xAD6]; \
    u8 unkAEC[0xB40 - 0xAEC]; \
    s8 unkB40;          /* 0xB40 */ \
    u8 unkB41[0xB48 - 0xB41]; \
    s32 unkB48;         /* 0xB48: what look mode 4 looks at: 0 the point 0xB54, 1 0xB4C */ \
    struct DrawObjFull *unkB4C; /* 0xB4C: the character (its bone 2, at bone 0's height + 0xB50) */ \
    s32 unkB50;         /* 0xB50 */ \
    f32 unkB54[3];      /* 0xB54 */ \
    s32 unkB60;         /* 0xB60: look mode 4's frames */ \
    u8 unkB64[0xB90 - 0xB64]; \
    f32 unkB90;         /* 0xB90: how far tilted to the ground (0-1) */ \
    f32 unkB94;         /* 0xB94: its speed */ \
    u8 unkB98;          /* 0xB98: the character */ \
    s8 unkB99;          /* 0xB99: uses the shared motions */ \
    u8 unkB9A;          /* 0xB9A */ \
    u8 unkB9B;          /* 0xB9B: condition (0 fine, 1 caution, 2 danger, 3 ...) */ \
    u8 unkB9C;          /* 0xB9C */ \
    u8 unkB9D[0xBA0 - 0xB9D]; \
    s32 unkBA0;         /* 0xBA0: status (2: danger, 8: poisoned...) */ \
    s32 unkBA4;         /* 0xBA4: flags */ \
    f32 unkBA8;         /* 0xBA8: its motion's speed */ \
    s32 unkBAC;         /* 0xBAC */ \
    f32 unkBB0;         /* 0xBB0: D_00712280[kind] */ \
    u16 unkBB4;         /* 0xBB4: (NPCs) their model + 1 */ \
    u16 unkBB6;         /* 0xBB6: (players) the virus held back: 1 */ \
    u16 unkBB8;         /* 0xBB8: 2 */ \
    u16 unkBBA;         /* 0xBBA */ \
    u8 costume;         /* 0xBBC */ \
    u8 unkBBD; \
    u8 unkBBE;          /* 0xBBE: (NPCs) the model's virus rate (D_006E6BF0, by its low 3 bits) */ \
    u8 unkBBF; \
    f32 unkBC0;         /* 0xBC0 */ \
    f32 unkBC4;         /* 0xBC4 */ \
    u8 unkBC8[6]; \
    s16 unkBCE;         /* 0xBCE: (players) the stick's heading in the world */ \
    u8 unkBD0;          /* 0xBD0 */ \
    u8 unkBD1;          /* 0xBD1: the head's look mode (func_0062F9B0) */ \
    s16 unkBD2;         /* 0xBD2: frames left of its turn */ \
    s16 unkBD4[2];      /* 0xBD4: the turn per frame (x, y) */ \
    s16 unkBD8[2];      /* 0xBD8: the head's angles (x, y; 0x10000 a turn) */ \
    s16 unkBDC[2];      /* 0xBDC: the same, as last set */ \
    struct DrawObjFull *unkBE0; /* 0xBE0: who it looks at */ \
    u8 unkBE4[0xBF0 - 0xBE4]; \
    f32 unkBF0[16];     /* 0xBF0: the head bone's matrix */ \
    s16 unkC30;         /* 0xC30: frames before it looks for someone else */ \
    u8 unkC32[0xC50 - 0xC32]; \
    u16 unkC50;         /* 0xC50: the breathing timer */ \
    u8 unkC52;          /* 0xC52: its key */ \
    u8 unkC53[0xC58 - 0xC53]; \
    u16 unkC58;         /* 0xC58: (poison) frames to its next hurt */ \
    u16 unkC5A;         /* 0xC5A: (poison) frames left */ \
    u8 unkC5C[2]; \
    u16 unkC5E;         /* 0xC5E: frames before a dying player falls */ \
    u16 unkC60;         /* 0xC60: (bleeding) frames to its next hurt */ \
    u16 unkC62;         /* 0xC62: (0xBA4 & 0x1000) frames before it turns */ \
    u8 unkC64;          /* 0xC64 */ \
    u8 unkC65;          /* 0xC65: what hit it (1-4) */ \
    s16 unkC66;         /* 0xC66 */ \
    u8 unkC68[0xC7C - 0xC68]; \
    u8 unkC7C;          /* 0xC7C: the item in hand (0xFF: none) */ \
    u8 unkC7D;          /* 0xC7D */ \
    u8 unkC7E;          /* 0xC7E */ \
    u8 unkC7F;          /* 0xC7F */ \
    u8 unkC80[2]; \
    u8 unkC82;          /* 0xC82: the item slot in hand */ \
    u8 unkC83; \
    u8 unkC84[4];       /* 0xC84: effects on the character (script ops 0x39, 0x3A) */ \
    u8 unkC88[0x20];    /* 0xC88: (effects 400-420) */ \
    s16 unkCA8;         /* 0xCA8 */ \
    u8 unkCAA[0xCB0 - 0xCAA]; \
    u8 unkCB0;          /* 0xCB0 */ \
    u8 unkCB1[0xCB8 - 0xCB1]; \
    s32 unkCB8;         /* 0xCB8 */ \
    u8 unkCBC[0xCCC - 0xCBC]; \
    u8 color[4];        /* 0xCCC: r, g, b, a */ \
    u8 unkCD0[0xCDC - 0xCD0]; \
    u8 unkCDC;          /* 0xCDC */ \
    u8 unkCDD; \
    u16 unkCDE;         /* 0xCDE: frames pushed against a wall */ \
    u8 unkCE0;          /* 0xCE0: (0xFF) its motion set is another character's */ \
    u8 unkCE1;          /* 0xCE1: which set (or that character's kind) */ \
    u8 unkCE2;          /* 0xCE2: 0x80 past frame unkCE3, 0x40 ... */ \
    u8 unkCE3;          /* 0xCE3 */ \
    u8 unkCE4[4]; \
    u32 unkCE8;         /* 0xCE8 */ \
    s32 unkCEC;         /* 0xCEC */ \
    s32 unkCF0;         /* 0xCF0 */ \
    f32 unkCF4[3];      /* 0xCF4: the last move */ \
    u8 unkD00[4]; \
    void *motions2[3];  /* 0xD04: (as motions) */ \
    u8 unkD10[0xD28 - 0xD10]; \
    s32 unkD28;         /* 0xD28 */ \
    s32 unkD2C;         /* 0xD2C */ \
    u8 unkD30[2]; \
    u8 unkD32;          /* 0xD32 */ \
    u8 unkD33[0xD74 - 0xD33]; \
    f32 unkD74[3];      /* 0xD74: where a push ends */ \
    f32 unkD80[3];      /* 0xD80: the push per frame */ \
    s16 unkD8C;         /* 0xD8C: the heading a push ends on */ \
    s16 unkD8E;         /* 0xD8E: the turn per frame */ \
    u16 unkD90;         /* 0xD90: the push's frames left */ \
    u8 unkD92[2]; \
    f32 unkD94[3];      /* 0xD94: the start position */ \
    s16 unkDA0;         /* 0xDA0: the start heading */

typedef struct DrawObjFull {
    OBJ_FIELDS(DrawObjFull)
    u8 unkDA2[0xDA8 - 0xDA2];
    s8 talk;            /* 0xDA8: (players) wants to ad-lib */
    s8 talkCmd;         /* 0xDA9: which (D_0023AFA8 / D_0023AFB0) */
    u8 unkDAA[0xE58 - 0xDAA];
    f32 unkE58[4][2][9];  /* 0xE58: the eyes' drift (func_005E9600; 3 used of each 9) */
    Ctrl ctrl;          /* 0xF78: (players) the controls */
    s16 unkFA8;         /* 0xFA8: (players) the stick's angle when the camera cut */
    s8 unkFAA;          /* 0xFAA: (players) keep the old direction since the cut */
    u8 unkFAB;
    s32 unkFAC;         /* 0xFAC: (players, online) */
    u8 unkFB0[0xFB4 - 0xFB0];
    u8 held;            /* 0xFB4: (enemies) what they hold, 0xFF nothing */
    u8 unkFB5[2];
    u8 way;             /* 0xFB7: how it moves (func_0058C240) */
    u8 wayStep;         /* 0xFB8 */
    u8 unkFB9[0xFCC - 0xFB9];
    u8 unkFCC;          /* 0xFCC */
    u8 unkFCD[0xFF0 - 0xFCD];
    u8 isNpc;           /* 0xFF0: (players) a computer-controlled partner */
    /* the partner's AI (SINGLE.BIN) */
    u8 unkFF1;          /* 0xFF1 */
    u8 unkFF2[2];
    u16 unkFF4;         /* 0xFF4 */
    u16 unkFF6;         /* 0xFF6: unkFF4 before */
    u16 unkFF8;         /* 0xFF8 */
    u16 unkFFA;         /* 0xFFA: unkFF8 before */
    u8 unkFFC[4];       /* 0xFFC: (partners) the thinking step's state */
    u16 unk1000[4];     /* 0x1000: (partners) its timers */
    s8 unk1008[4];      /* 0x1008 */
    s8 unk100C;         /* 0x100C: (partners) the place they wait at */
    s8 unk100D;         /* 0x100D: (partners) the place it heads for */
    s8 unk100E;         /* 0x100E: (partners) the place it goes to next */
    s8 unk100F;         /* 0x100F: unk100E before */
    f32 unk1010[3];     /* 0x1010: (partners) where it heads */
    f32 unk101C[3];     /* 0x101C: unk100E's point */
    f32 unk1028[3];     /* 0x1028: unk100F's */
    s32 unk1034;        /* 0x1034 */
    u16 unk1038;        /* 0x1038: (partners) the heading it wants */
    u8 unk103A;         /* 0x103A: (partners) the moving step's state */
    u8 unk103B[0x1054 - 0x103B];
    u16 unk1054;        /* 0x1054: (partners) how it moves (1, 2, 8, 9: climbing...) */
    u16 unk1056;        /* 0x1056 */
    u16 unk1058;        /* 0x1058: (partners) the room it heads for */
    s8 unk105A;         /* 0x105A */
    u8 unk105B;         /* 0x105B */
    u8 unk105C[4];
    s32 unk1060;        /* 0x1060: (partners) frames left */
    u8 unk1064;         /* 0x1064 */
    s8 unk1065;         /* 0x1065: (partners) the place it was at when it last looked for a way */
    u8 unk1066;
    s8 mode;            /* 0x1067: its D_00838860 entry */
    u8 unk1068[4];
    u8 unk106C;         /* 0x106C: (partners) how near the nearest dangerous enemy is (0-2, 3 none) */
    u8 unk106D;         /* 0x106D: unk106C last frame */
    s8 unk106E;         /* 0x106E: that enemy (D_003FFCA0 index; -1 none) */
    u8 unk106F;         /* 0x106F: unk54F last frame */
    u16 unk1070;        /* 0x1070 */
    u16 unk1072;        /* 0x1072: (partners) frames to the next idle remark */
    s8 unk1074;         /* 0x1074 */
    u8 unk1075;         /* 0x1075 */
    u8 unk1076;         /* 0x1076: unk1075 last frame */
    u8 unk1077;         /* 0x1077: in danger (by D_00838CD0) */
    u8 unk1078;         /* 0x1078: func_005C6170's */
    u8 unk1079;         /* 0x1079: unk1078 last frame */
    u8 unk107A;         /* 0x107A: func_005C6240's */
    u8 unk107B;         /* 0x107B: unk107A last frame */
    u16 unk107C;        /* 0x107C */
    u8 unk107E;         /* 0x107E: (partners) enemies in view */
    u8 unk107F;         /* 0x107F: unk107E last frame */
    u8 unk1080;         /* 0x1080 */
    u8 unk1081;         /* 0x1081: (partners) 1/2 when the other partner leads */
    u8 unk1082;         /* 0x1082: unk1075 changed */
    u8 unk1083;         /* 0x1083 */
    u16 unk1084;        /* 0x1084: frames unk10B8 holds */
    u16 unk1086;        /* 0x1086 */
    u16 unk1088;        /* 0x1088 */
    u16 unk108A;        /* 0x108A */
    u8 unk108C[4];
    s32 unk1090;        /* 0x1090: (partners) an item (func_00835C50's, or ours in hand) */
    u16 unk1094;        /* 0x1094 */
    u16 unk1096;        /* 0x1096 */
    u8 unk1098;         /* 0x1098 */
    u8 unk1099;         /* 0x1099 */
    u16 unk109A;        /* 0x109A */
    s32 unk109C;        /* 0x109C */
    s32 unk10A0;        /* 0x10A0 */
    u32 unk10A4;        /* 0x10A4: (partners) frames stuck */
    s8 unk10A8;         /* 0x10A8 */
    u8 unk10A9;         /* 0x10A9 */
    s16 unk10AA;        /* 0x10AA */
    void *modeInfo;     /* 0x10AC: &D_00838860[mode] */
    u8 unk10B0[8];      /* 0x10B0: from modeInfo */
    u8 unk10B8[8];      /* 0x10B8 */
    u8 unk10C0[8];      /* 0x10C0 */
    u8 unk10C8[8];      /* 0x10C8: unk10B0 + unk10B8 + unk10C0 */
    u8 unk10D0[0x10E0 - 0x10D0];
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
    s8 unk1138;         /* 0x1138: a func_00652B50 request pending */
    u8 unk1139;         /* 0x1139: its argument */
    u8 unk113A[0x1150 - 0x113A];
    s32 unk1150;        /* 0x1150 */
    u8 unk1154;         /* 0x1154 */
    u8 unk1155[0x1162 - 0x1155];
    s8 unk1162;         /* 0x1162 */
    u8 unk1163[0x1170 - 0x1163];
} Npc;                  /* 0x1170 */

#endif
