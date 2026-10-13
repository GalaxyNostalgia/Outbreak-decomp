#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

/* The camera (main's data at 0x306320), handed to the renderer with func_001AE9C0(0x18, ...). */
typedef struct {
    s32 unk0;
    s32 unk4;
    s32 width;          /* 0x08: 0x280, or 0x200 when Options.unkF is set */
    s32 height;         /* 0x0C */
    s32 unk10;
    f32 unk14;          /* 0x14 */
    Vec3 pos;           /* 0x18 */
    Vec3 target;        /* 0x24 */
    Vec3 unk30;
    Vec3 up;            /* 0x3C */
    Vec3 unk48;
    f32 unk54;          /* 0x54 */
    f32 far;            /* 0x58 */
    f32 near;           /* 0x5C */
    f32 fov;            /* 0x60: degrees */
    s32 unk64;
    s32 unk68;
    s32 roll;           /* 0x6C */
} Camera;

extern Camera D_00306320;
extern Mat44 D_003062E0;        /* the view matrix */

#endif
