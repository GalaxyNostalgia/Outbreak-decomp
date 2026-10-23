#ifndef RELATION_H
#define RELATION_H

#include "common.h"

/* How one character stands to another (game/relations.c fills these each frame). */
typedef struct {
    f32 dist;           /* 0x0 */
    s16 angle;          /* 0x4 */
    s8 hidden;          /* 0x6: no line of sight (or far) */
    s8 unk7;            /* 0x7 */
} Relation;

extern Relation D_00386110[8][8];       /* players to players (online) */
extern Relation D_00385490[8][50];      /* players to things */
extern u8 D_003840E0[8][50];            /* each player's things, nearest first */
extern u8 D_00383F50[50][8];            /* each thing's players, nearest first */
extern Relation D_003844F0[10][50];     /* NPCs to things */
extern u8 D_00383D50[10][50];
extern u8 D_00383B50[50][10];
extern Relation D_00384270[8][10];      /* players to NPCs */
extern u8 D_00383B00[8][10];
extern u8 D_00383AB0[10][8];

#endif
