#ifndef MOTION_H
#define MOTION_H

#include "common.h"

/* A set of motions: anims[] are memory handles of decoded motions, index[] maps a motion number
   to its first anim (per animations a motion has). */
typedef struct {
    u8 count;           /* 0x00 */
    u8 per;             /* 0x01: anims per motion */
    s8 loaded;          /* 0x02 */
    u8 unk3;
    u16 anims[2000];    /* 0x04 */
    u16 index[2000];    /* 0xFA4 */
} MotionSet;            /* 0x1F44 */

/* Per load mode: the index and anim ranges it owns. */
typedef struct {
    u16 index;
    u16 nindex;
    u16 anim;
    u16 nanim;
} MotionRange;

#endif
