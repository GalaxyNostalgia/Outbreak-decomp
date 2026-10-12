#ifndef TASK_H
#define TASK_H

#include "common.h"

/* The game's top-level tasks: 16 slots, each a function the frame loop calls once a frame
   while the slot is running. Slot 0 is the boot task, slot 4 the current overlay's main task. */
typedef struct Task {
    s16 state;                  /* 0x00: TASK_* */
    s16 timer;                  /* 0x02: frames left in TASK_WAIT */
    void (*fn)(struct Task *);  /* 0x04 */
    u8 step;                    /* 0x08: the task's own state machine */
    u8 sub;                     /* 0x09 */
    u8 unkA[0x14 - 0xA];
    void (*onKill)(void);       /* 0x14: called when the slot is cleared */
    u8 unk18[0x20 - 0x18];
} Task;

#define TASK_FREE 0
#define TASK_RESTART 2          /* start again next frame */
#define TASK_START 4            /* runs from next frame */
#define TASK_RUN 8
#define TASK_RESUME 12
#define TASK_WAIT 16            /* wait `timer` frames, then start again */

#define TASK_MAX 16

extern Task D_002F85D0[TASK_MAX];

#endif
