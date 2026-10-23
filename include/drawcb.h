#ifndef DRAWCB_H
#define DRAWCB_H

#include "common.h"

/* A queued draw callback (main's pool.c): fn(cb) runs when the list is drawn, far to near. */
typedef struct DrawCb {
    u32 next;
    u32 depth;          /* 0x04: its bucket (0-127, by distance from the camera) */
    f32 pos[4];         /* 0x08 */
    void *arg;          /* 0x18 */
    void (*fn)(struct DrawCb *cb); /* 0x1C */
} DrawCb;

#endif
