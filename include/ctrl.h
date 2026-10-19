#ifndef CTRL_H
#define CTRL_H

#include "common.h"

/* A player's controls as the game logic reads them: button history, sticks as angles, menu
   auto-repeat (D_00306410 per pad, and each player's own at 0xF78). */
typedef struct {
    u32 pressed;        /* 0x00 */
    u32 released;       /* 0x04 */
    u32 cur;            /* 0x08 */
    u32 prev[3];        /* 0x0C: the last three frames */
    u32 repeat;         /* 0x18: pressed, plus held directions repeating */
    s32 repeatTimer;    /* 0x1C */
    s16 rmag;           /* 0x20 */
    s16 rangle;         /* 0x22: 0x10000 per turn, 0 up */
    s16 rx;             /* 0x24 */
    s16 ry;
    s16 lmag;           /* 0x28 */
    s16 langle;
    s16 lx;             /* 0x2C */
    s16 ly;
} Ctrl;                 /* 0x30 */

#endif
