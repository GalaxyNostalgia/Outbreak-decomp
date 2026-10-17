#include "common.h"
#include "thing.h"
#include "game.h"
#include "options.h"

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

s32 func_001AF510(void);
void func_00606500(Thing *t, s8 se, u8 *done);
void func_006094A0(Thing *t);

/* With func_001AF510 (Options.unk3B 1 or 2): passes t's event se (0: none) on. */
void func_00576690(Thing *t, s32 se) {
    u8 done;

    if (func_001AF510() == 1 && se != 0) {
        done = 0;
        func_00606500(t, se, &done);
        if (done) {
            func_006094A0(t);
        }
    }
}

void func_0064FB70(s32 type, s32 id, u8 *b, s32 player);
s32 func_0064FF60(Thing *t);
extern u8 *D_006D61E0[4];       /* per mode: per room, the message */

/* With func_001AF510: queues message (3, m) about t's slot (mark type 2) for our player. */
void func_005769F0(Thing *t, s32 unused, u8 m) {
    u8 b[4];

    if (func_001AF510() && t->unkB9C != 1) {
        b[1] = m;
        b[0] = 3;
        b[2] = 0;
        b[3] = 0;
        func_0064FB70(2, t->set, b, D_0048BF60.unk1D);
    }
}

/* Once (0xC2C): message mode (0-3) for this room about t, when t's slot is ours. */
void func_00576A70(Thing *t, s32 mode) {
    if (func_001AF510() && t->unkB9C != 1 && t->unkB9A == 1 && (u8)mode < 4 && t->unkC2C == 0 &&
        func_0064FF60(t)) {
        func_005769F0(t, 0, D_006D61E0[(u8)mode][D_00306590.unk1A]);
        t->unkC2C = 1;
    }
}

/* A knockback: a push (vx, vz) dying away by (dx, dz) each frame. */
typedef struct {
    s16 vx;             /* 0x00 */
    s16 vz;             /* 0x02 */
    u8 unk4[4];
    s16 dx;             /* 0x08 */
    s16 dz;             /* 0x0A */
    u8 unkC[4];
    u8 state;           /* 0x10: 0 off, 1 waiting (timer), 2 start, 3 running */
    u8 arg;             /* 0x11 */
    u8 timer;           /* 0x12 */
    u8 pad13;
} Knock;                /* 0x14 */

/* A knockback's strength (along x and z) and how many frames it lasts. */
typedef struct {
    s16 x;
    s16 z;
    s16 frames;
} KnockDef;

void func_00190E00(f32 *out, const f32 *m);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
f32 func_00191A70(f32 x);
void func_0055C450(Thing *t, u8 arg, Knock *k);

/* Runs t's two knockbacks (k, with their strengths in defs). */
void func_00577870(Thing *t, KnockDef *defs, Knock *ks);
#ifdef NON_MATCHING   /* structure right; register use and a kept &state */
void func_00577870(Thing *t, KnockDef *defs, Knock *ks) {
    f32 m[16];
    f32 v[3];
    f32 ax;
    u8 i;
    Knock *k;
    KnockDef *d;

    for (i = 0; i < 2; i++) {
        k = &ks[i];
        switch (k->state) {
        case 1:
            if (--k->timer <= 0) {
                k->state++;
            }
            break;
        case 2:
            k->state++;
            func_00190E00(m, t->mtx);
            func_001913A0(v, (f32 *)((u8 *)t + 0x12C), m);
            ax = func_00191A70(v[0]);
            d = &defs[i];
            k->vx = d->x;
            k->vz = d->z;
            if (!(ax <= func_00191A70(v[2]))) {
                k->vx = v[0] * k->vx;
                k->vz = v[2] * k->vz;
            } else {
                k->vz = v[0] * k->vz;
                k->vx = v[2] * k->vx;
            }
            k->dx = -(k->vx / d->frames);
            k->dz = -(k->vz / d->frames);
            k->dx /= (s16)D_0023BF7C;
            k->dz /= (s16)D_0023BF7C;
            break;
        case 3:
            if (k->vx != 0) {
                k->vx += k->dx;
                if (k->dx > 0) {
                    if (k->vx > 0) {
                        k->vx = 0;
                    }
                } else if (k->dx < 0) {
                    if (k->vx < 0) {
                        k->vx = 0;
                    }
                }
            }
            if (k->vz != 0) {
                k->vz += ks[i].dz;
                if (ks[i].dz > 0) {
                    if (k->vz > 0) {
                        k->vz = 0;
                    }
                } else if (ks[i].dz < 0) {
                    if (k->vz < 0) {
                        k->vz = 0;
                    }
                }
            }
            if (k->vx == 0 && k->vz == 0) {
                ks[i].state = 0;
                return;
            }
            break;
        case 0:
            continue;
        }
        func_0055C450(t, ks[i].arg, &ks[i]);
    }
}
#endif

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

/* func_00678E60 without blending. */
void func_00679370(Thing *t, s32 motion, s8 mode) {
    f32 start = 0.0f;
    s16 cur = t->unk520;

    if (cur != motion) {
        if (cur == 7 && motion == 5) {
            start = 62.0f;
        } else if (motion == 8 && t->unkC37) {
            start = 56.0f;
        }
        switch (mode) {
        case 1:
            func_00578250(t, motion, 1, start, 0.0f);
            break;
        default:
            func_00578150(t, motion, start, 0.0f);
            break;
        }
    } else {
        func_0055C030(t, 1.0f);
    }
}
