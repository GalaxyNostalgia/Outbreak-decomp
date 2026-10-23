#ifndef JOB_H
#define JOB_H

#include "common.h"

/* Per player, ten timed jobs (D_00387670[3][10], 0x110 bytes each; GAME.BIN's frame.c runs
   them). */
typedef struct Job {
    u8 active;          /* 0x00 */
    u8 slot;            /* 0x01 */
    u8 kind;            /* 0x02 */
    u8 unk3[0x1C - 3];
    void (*fn)(struct Job *j); /* 0x1C */
    u8 unk20[0x60 - 0x20];
    s32 args[5];        /* 0x60: the script op's (func_0065F070) */
    u8 unk74[0x100 - 0x74];
    u8 player;          /* 0x100 */
    u8 unk101[0x110 - 0x101];
} Job;

extern Job D_00387670[3][10];

#endif
