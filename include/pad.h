#ifndef PAD_H
#define PAD_H

#include "common.h"

/* Controllers: what libpad2 reads, the state of each socket, and what the game uses. */

typedef struct {
    u8 status;          /* 0xFF: nothing read */
    u8 unk1;
    u16 buttons;        /* active low as read; the game flips it */
    u8 stick[4];        /* 0x04: left x, y, right x, y (0-255, 128 centre) */
    u8 pressure[12];    /* 0x08 */
    u8 unk14[0x20 - 0x14];
} PadData;              /* 0x20 */

typedef struct {
    u8 state;           /* 0 none, 1 socket open, 2 connected */
    u8 tries;           /* reads since connecting (the profile is read on the first) */
    u8 port;
    u8 slot;
    u8 unk4;
    u8 vibrate;         /* 0x05: has a motor */
    u8 socket;
    u8 kind;            /* 0x07: 0 digital, 1 analog, 2 analog with pressure */
    void *buffer;       /* the library's DMA buffer for this socket */
    u32 profile;        /* 0x0C: scePad2GetButtonProfile */
    s32 actuators;      /* 0x10 */
    s32 vibTimer;       /* 0x14: frames until the motor stops */
} PadPort;              /* 0x18 */

typedef struct {
    s16 x;
    s16 y;
    s16 mag;            /* 0-127 */
    s16 angle;          /* degrees */
    f32 rad;
} PadStick;             /* 0x0C */

typedef struct {
    u8 status;          /* the socket's state */
    u8 flags;           /* 0x20 right stick, 0x40 left stick */
    u16 type;           /* 0x02 */
    u8 port;
    u8 slot;
    u8 vibrate;         /* 0x06 */
    u8 unk7;
    u32 buttons;        /* 0x08: the game's button bits */
    PadStick right;     /* 0x0C */
    PadStick left;      /* 0x18 */
} PadInfo;              /* 0x24 */

typedef struct {
    u32 mask;           /* libpad2 button bit */
    u32 unk4;
} PadMap;

extern PadData D_00292D00;          /* "nothing pressed" */
extern PadData D_00292D20[1];       /* last frame */
extern PadData D_00292D40[1];       /* this frame */
extern PadPort D_00292D60[2];
extern PadInfo D_00292D80[1];
extern PadMap D_00292C80[1][16];    /* button map in use */

#endif
