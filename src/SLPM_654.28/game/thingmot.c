#include "common.h"
#include "thing.h"

/* GAME.BIN: changing an enemy's motion, with the blend time that suits the change. */

extern u8 D_0023BF7C;           /* frames per game frame (1, or 2 at 30 fps) */

/* A motion of an enemy type: its set (in motions) and the frames its layers start on. */
typedef struct {
    s8 set;
    u8 unk1[7];
    s16 frame1;         /* 0x8: other layers */
    s16 frame0;         /* 0xA: layer 0 */
} ThingMotion;          /* 0xC */

typedef struct {
    u8 unk0;
    u8 nlayers;         /* 0x1 */
} MotionSet;

extern ThingMotion *D_006F4B70[];       /* by enemy type */

void func_0055C030(Thing *t, f32 speed);
void func_0055B6D0(Thing *t, MotionSet *ms, s32 frame, u16 layer, f32 start, f32 blend);

/* Starts motion on all of t's layers. */
void func_00578150(Thing *t, s16 motion, f32 start, f32 blend) {
    s32 i;
    ThingMotion *m = &D_006F4B70[t->kind][motion];
    MotionSet *ms = t->motions[m->set];
    s32 frames[2];
    s32 f;

    t->unk520 = motion;
    frames[0] = m->frame1;
    frames[1] = m->frame0;
    for (i = 0; i < ms->nlayers; i++) {
        if (i < 2) {
            f = frames[i];
        } else {
            f = frames[0];
        }
        func_0055B6D0(t, ms, f, i, start, blend);
        t->layers[i].frame = f;
    }
}

/* Starts motion on one of t's layers. */
void func_00578250(Thing *t, s16 motion, s8 layer, f32 start, f32 blend) {
    ThingMotion *m = &D_006F4B70[t->kind][motion];
    MotionSet *ms = t->motions[m->set];
    s32 f;

    t->unk520 = motion;
    if (layer == 0) {
        f = m->frame0;
    } else {
        f = m->frame1;
    }
    func_0055B6D0(t, ms, f, layer, start, blend);
    t->layers[layer].frame = f;
}

/* Starts motion (mode 1: func_00578250's way); the same motion just runs on. */
void func_00678E60(Thing *t, s32 motion, s8 mode);
void func_00678E60(Thing *t, s32 motion, s8 mode) {
    f32 start;
    f32 blend = 10.0f * (f32)(u32)D_0023BF7C;
    s16 cur = t->unk520;

    start = 0.0f;
    if (cur != motion) {
        if (t->act[0] == 0) {
            blend = 0.0f;
        } else if (t->act[0] == 1) {
            if ((motion > 0 && motion <= 3) || (cur > 0 && cur < 4)) {
                blend = 20.0f * (f32)(u32)D_0023BF7C;
            } else if (motion == 8) {
                blend = 5.0f * (f32)(u32)D_0023BF7C;
            } else if (t->act[1] == 6 && (cur == 8 || cur == 12 || cur == 14)) {
                blend = 20.0f * (f32)(u32)D_0023BF7C;
            } else if (cur == 8 && (motion == 0x66 || motion == 0x67)) {
                blend = 15.0f * (f32)(u32)D_0023BF7C;
            } else if ((cur == 0x66 || cur == 0x67) && motion == 5) {
                blend = 15.0f * (f32)(u32)D_0023BF7C;
            }
        } else if (t->act[0] == 4) {
            if ((cur == 0x36 && motion == 0x33) || (cur == 0x37 && motion == 0x32)) {
                blend = 20.0f;
            } else {
                blend = 10.0f;
            }
        } else if (t->act[0] == 5) {
            if (t->unkBE8[0] == 4 && (t->unkBE8[1] == 10 || t->unkBE8[1] == 11) &&
                (cur == 0x32 || cur == 0x33)) {
                blend = 0.0f;
            } else if ((cur == 0x57 && motion == 0x58) || (cur == 0x58 && motion == 0x68)) {
                blend = 10.0f;
            }
        } else if (t->act[0] == 2) {
            if (cur == 0x6B && motion == 0x6F) {
                blend = 0.0f;
            }
            if (cur == 0x6F && motion == 0x70) {
                blend = 0.0f;
            }
            if (cur == 0x70 && motion == 0x6C) {
                blend = 0.0f;
            }
        }
        if (cur == 7 && motion == 5) {
            start = 62.0f;
        } else if (motion == 8 && t->unkC37) {
            start = 56.0f;
        }
        switch (mode) {
        case 1:
            func_00578250(t, motion, 1, start, blend);
            break;
        default:
            func_00578150(t, motion, start, blend);
            break;
        }
    } else {
        func_0055C030(t, 1.0f);
    }
}
