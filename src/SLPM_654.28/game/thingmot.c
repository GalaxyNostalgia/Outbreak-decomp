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

/* func_00678E60 from frame start, with fewer special blends. */
void func_00679280(Thing *t, s32 motion, s8 mode, f32 start);
#ifdef NON_MATCHING   /* the original loads 10.0f again for the second blend (ours reuses it) */
void func_00679280(Thing *t, s32 motion, s8 mode, f32 start) {
    f32 blend = 10.0f * (f32)(u32)D_0023BF7C;
    s16 cur = t->unk520;

    if (cur != motion) {
        if (t->act[0] == 2) {
            if (t->act[1] == 10) {
                blend = 0.0f;
            }
        } else if (t->act[0] == 5 && cur == 0x5B && motion == 5) {
            blend = 10.0f;
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
#endif

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

void func_0061FBC0(Thing *t, f32 speed);
void func_00653870(Thing *t, f32 speed);
void func_0061F210(Thing *t, f32 speed);
void func_00692000(Thing *t, f32 speed);
void func_00601A10(Thing *t, f32 speed);
void func_006999A0(Thing *t, f32 speed);
void func_006A17B0(Thing *t, f32 speed);
void func_005E4850(Thing *t, f32 speed);
void func_0057EE30(Thing *t, f32 speed);
void func_0064B830(Thing *t, f32 speed);
void func_0068F0D0(Thing *t, f32 speed);
void func_005EAC60(Thing *t, f32 speed);
void func_005DA3C0(Thing *t, f32 speed);
void func_005E8620(Thing *t, f32 speed);
void func_0069BDC0(Thing *t, f32 speed);
void func_00690D40(Thing *t, f32 speed);
void func_0064B260(Thing *t, f32 speed);
void func_005F3950(Thing *t, f32 speed);
void func_0062C340(Thing *t, f32 speed);
void func_005AFDF0(Thing *t, f32 speed);
void func_006C5480(Thing *t, f32 speed);
void func_005EA8B0(Thing *t, f32 speed);
void func_006BAC70(Thing *t, f32 speed);

/* A visible thing's per-kind work for its motion advancing by speed (by its enemy type). */
void func_00578B30(Thing *t, f32 speed) {
    if (t->visible != 0) {
        switch (t->kind) {
        case 0x0:
            func_0061FBC0(t, speed);
            break;
        case 0x1:
            func_00653870(t, speed);
            break;
        case 0x2:
            func_0061F210(t, speed);
            break;
        case 0x4:
            func_00692000(t, speed);
            break;
        case 0x5:
            func_00601A10(t, speed);
            break;
        case 0x6:
            func_006999A0(t, speed);
            break;
        case 0x7:
            func_006A17B0(t, speed);
            break;
        case 0x9:
            func_005E4850(t, speed);
            break;
        case 0xA:
            func_0057EE30(t, speed);
            break;
        case 0x12:
            func_0064B830(t, speed);
            break;
        case 0x13:
            func_0068F0D0(t, speed);
            break;
        case 0x14:
            func_005EAC60(t, speed);
            break;
        case 0x16:
            func_005DA3C0(t, speed);
            break;
        case 0x17:
            func_005E8620(t, speed);
            break;
        case 0x1A:
            func_0069BDC0(t, speed);
            break;
        case 0x1B:
            func_00690D40(t, speed);
            break;
        case 0x1D:
            func_0064B260(t, speed);
            break;
        case 0x1E:
            func_005F3950(t, speed);
            break;
        case 0x24:
            func_0062C340(t, speed);
            break;
        case 0x29:
            func_005AFDF0(t, speed);
            break;
        case 0x2B:
            func_006C5480(t, speed);
            break;
        case 0x37:
            func_005EA8B0(t, speed);
            break;
        case 0x3B:
            func_006BAC70(t, speed);
            break;
        }
    }
}

/* Things done on frames of a thing's motions (func_00543460: whether this step of speed passes
   frame of layer 0). */
s32 func_00543460(Thing *t, s32 layer, f32 frame, f32 speed);
u16 func_001ADEF0(s32 n);
void func_001B79B0(Thing *t, s32 sound, s32 a);
void func_001B6FD0(f32 *pos, s32 sound);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
void func_005524F0(Thing *t, f32 *m, f32 *pos, s16 *rot, s32 a, s32 b);
void func_005A9960(Thing *t, u8 sound, s32 a);
void func_00540260(Thing *t, u8 a);
extern f32 D_006F4D80[];        /* (a point in the thing's space) */

typedef struct {
    f32 m[16];
} ThingMat;

/* Sound (one of range from it) from the thing. */
void func_00578DB0(s32 frame, Thing *t, s32 sound, s32 range, f32 speed) {
    if (func_00543460(t, 0, frame, speed)) {
        if (range > 1) {
            sound += (func_001ADEF0(1) & 0xFFFF) % range;
        }
        func_001B79B0(t, sound, 0);
    }
}

/* func_005524F0 at D_006F4D80 in the thing's space. */
void func_00579170(s32 frame, Thing *t, f32 speed) {
    f32 p[4];
    ThingMat m;

    if (func_00543460(t, 0, frame, speed)) {
        m = *(ThingMat *)t->mtx;
        func_001913A0(p, D_006F4D80, m.m);
        p[0] += t->pos[0];
        p[1] += t->pos[1];
        p[2] += t->pos[2];
        func_005524F0(t, m.m, p, t->rot, 0, 0);
    }
}

/* Mode 0: voice sound (0x115 cleared), 1; mode 2: 0x115 bit 0. */
s32 func_00579240(s32 frame, Thing *t, s8 mode, s32 sound, f32 speed);
#ifdef NON_MATCHING   /* the return paths (an extra b to the epilogue) */
s32 func_00579240(s32 frame, Thing *t, s8 mode, s32 sound, f32 speed) {
    if (func_00543460(t, 0, frame, speed)) {
        switch (mode) {
        case 2:
            t->parts[0x21] |= 1;
            break;
        case 0:
            t->parts[0x21] = 0;
            func_005A9960(t, sound, 1);
            return 1;
        default:
            return 0;
        }
    }
    return 0;
}
#endif

void func_005792F0(s32 frame, Thing *t, f32 speed) {
    if (func_00543460(t, 0, frame, speed)) {
        func_00540260(t, FIELD(t, u8, 0xABB));
    }
}

/* Voice sound, unless in action 3 or 4. */
void func_00579340(s32 frame, Thing *t, s32 sound, f32 speed) {
    if (func_00543460(t, 0, frame, speed) && t->act[0] != 3 && t->act[0] != 4) {
        func_005A9960(t, sound, 1);
    }
}

/* Sound (one of range from it) at pos. */
void func_005793B0(s32 frame, Thing *t, f32 *pos, s32 sound, s32 range, f32 speed) {
    if (func_00543460(t, 0, frame, speed)) {
        if (range > 1) {
            sound += (func_001ADEF0(1) & 0xFFFF) % range;
        }
        func_001B6FD0(pos, sound);
    }
}
s32 func_00604000(Thing *t, s32 a);
s32 func_001AF4D0(void);

/* Where bone slot off of t's model is (its world translation). */
#define BONE(t, off) ((f32 *)(FIELD((t)->model, u8 *, off) + 0x30))

/* Kind 1's sounds (steps and the like) on frames of its motions; 0xD7D picks the other set. */
#ifdef NON_MATCHING   /* first draft: scheduling around the calls of case 0x85 and later */
void func_00653870(Thing *t, f32 speed) {
    s32 snd;
    s32 snd2;
    s32 snd1;
    s32 range;
    s32 n;

    if (FIELD(t, u8, 0xD7D) != 0) {
        snd2 = 0x10;
        snd = 0xF;
        snd1 = 0x10;
        range = 0;
    } else {
        snd = 0;
        snd2 = 4;
        snd1 = 9;
        range = 2;
    }
    switch (FIELD(t, u16, 0x1F2)) {
    case 0x64:
    case 0x65:
    case 0x66:
        if (func_00604000(t, 0xFA0) != 0 && !(func_001ADEF0(1) & 0xFFFF & 3)) {
            func_00578DB0(0, t, 0x48, 0x10, speed);
        }
        break;
    case 0x6E:
        func_00578DB0(0x10, t, snd, range, speed);
        func_00578DB0(0x7C, t, snd, range, speed);
        break;
    case 0x6F:
        func_00578DB0(0x62, t, snd, range, speed);
        func_00578DB0(0xA4, t, snd, range, speed);
        break;
    case 0x70:
        func_00578DB0(0xC, t, snd, range, speed);
        func_00578DB0(0x50, t, snd, range, speed);
        func_00578DB0(0x6E, t, snd, range, speed);
        break;
    case 0x71:
        if (t->act[0] == 5 && t->act[1] == 0x17) {
            break;
        }
        func_00578DB0(0x14, t, snd, range, speed);
        func_00578DB0(0x38, t, snd, range, speed);
        break;
    case 0x72:
        func_00578DB0(0x14, t, snd, range, speed);
        func_00578DB0(0x38, t, snd, range, speed);
        break;
    case 0x73:
        func_00578DB0(0x1C, t, snd, range, speed);
        func_00578DB0(0x32, t, snd, range, speed);
        break;
    case 0x74:
        func_00578DB0(0x52, t, 3, 0, speed);
        func_00578DB0(0xA6, t, 3, 0, speed);
        break;
    case 0x75:
        func_00578DB0(0x22, t, snd, range, speed);
        func_00578DB0(0x58, t, snd, range, speed);
        func_00578DB0(0xB8, t, snd, range, speed);
        func_00578DB0(0xEE, t, snd, range, speed);
        break;
    case 0x76:
        func_00578DB0(0x18, t, snd, range, speed);
        func_00578DB0(0x3E, t, snd, range, speed);
        func_00578DB0(0x62, t, snd, range, speed);
        func_00578DB0(0x84, t, snd, range, speed);
        break;
    case 0x77:
        func_00578DB0(0x16, t, snd, range, speed);
        func_00578DB0(0x4C, t, snd, range, speed);
        func_00578DB0(0x9C, t, snd, range, speed);
        break;
    case 0x78:
        func_00578DB0(0x1C, t, snd, range, speed);
        func_00578DB0(0x38, t, snd, range, speed);
        func_00578DB0(0x5E, t, snd, range, speed);
        break;
    case 0x79:
        func_00578DB0(0x7A, t, 3, 0, speed);
        func_00578DB0(0x90, t, 3, 0, speed);
        func_00578DB0(0xB6, t, 3, 0, speed);
        func_00578DB0(0xD4, t, 3, 0, speed);
        break;
    case 0x7A:
        func_00578DB0(0x38, t, snd, range, speed);
        func_00578DB0(0x68, t, snd, range, speed);
        break;
    case 0x7B:
        func_00578DB0(0x2C, t, snd, range, speed);
        func_00578DB0(0x58, t, snd, range, speed);
        break;
    case 0x7C:
        func_00578DB0(0x36, t, snd1, 0, speed);
        func_00578DB0(0x68, t, snd2, 0, speed);
        func_00578DB0(0x74, t, snd2, 0, speed);
        func_00578DB0(0xA0, t, snd2, 0, speed);
        break;
    case 0x82:
        if (t->act[0] == 1 && t->act[1] == 0x12) {
            func_00578DB0(0x12, t, 8, 0, speed);
        } else if (t->act[0] == 1 && t->act[1] == 0x1D) {
            func_00578DB0(0x12, t, 8, 0, speed);
        } else if (t->act[0] == 5 && t->act[1] == 0x18) {
            func_005793B0(0x12, t, BONE(t, 0x86C), 0x37, 2, speed);
        } else if (t->act[0] == 5 && (t->act[1] == 0x24 || t->act[1] == 0xB || t->act[1] == 0x1C)) {
            switch (t->unkBAC[6]) {
            case 1:
                func_005793B0(0x12, t, BONE(t, 0x86C), 0x37, 2, speed);
                break;
            case 2:
                func_005793B0(0x12, t, BONE(t, 0x86C), 0x8D, 2, speed);
                break;
            }
        }
        break;
    case 0x83:
        if (t->act[0] == 1 && t->act[1] == 0x12) {
            func_00578DB0(0x18, t, 8, 0, speed);
            func_00578DB0(0x5E, t, 8, 0, speed);
            func_00578DB0(0x82, t, 8, 0, speed);
            func_00579240(0x18, t, 0, 0x16, speed);
            func_00579240(0x1A, t, 2, 0, speed);
        } else if (t->act[0] == 1 && t->act[1] == 0x1D) {
            func_00578DB0(0x18, t, 8, 0, speed);
            func_00578DB0(0x5E, t, 8, 0, speed);
            func_00578DB0(0x82, t, 8, 0, speed);
            func_00579240(0x18, t, 0, 0x16, speed);
            func_00579240(0x1A, t, 2, 0, speed);
        } else if (t->act[0] == 5 && t->act[1] == 0x18) {
            func_005793B0(0x18, t, BONE(t, 0x86C), 0x37, 2, speed);
            func_005793B0(0x5E, t, BONE(t, 0x86C), 0x37, 2, speed);
            func_005793B0(0x82, t, BONE(t, 0x85C), 0x37, 2, speed);
        } else if (t->act[0] == 5 && (t->act[1] == 0x24 || t->act[1] == 0xB || t->act[1] == 0x1C)) {
            switch (t->unkBAC[6]) {
            case 1:
                func_005793B0(0x18, t, BONE(t, 0x86C), 0x37, 2, speed);
                func_005793B0(0x5E, t, BONE(t, 0x86C), 0x37, 2, speed);
                func_005793B0(0x82, t, BONE(t, 0x85C), 0x37, 2, speed);
                break;
            case 2:
                func_005793B0(0x18, t, BONE(t, 0x86C), 0x8D, 2, speed);
                func_005793B0(0x5E, t, BONE(t, 0x86C), 0x8D, 2, speed);
                func_005793B0(0x82, t, BONE(t, 0x85C), 0x8D, 2, speed);
                break;
            }
        } else {
            func_00579240(0x18, t, 0, 0x16, speed);
            func_00579240(0x1A, t, 2, 0, speed);
        }
        break;
    case 0x84:
        func_00578DB0(0x28, t, snd, range, speed);
        func_00578DB0(0x3C, t, snd, range, speed);
        break;
    case 0x85:
        func_00578DB0(0x20, t, snd, range, speed);
        func_00578DB0(0x48, t, snd, range, speed);
        func_00578DB0(0x6E, t, snd, range, speed);
        func_00578DB0(0xA2, t, snd, range, speed);
        func_00578DB0(4, t, 0x48, 0x10, speed);
        if (func_001AF4D0() == 1) {
            if (t->unkB9B & 1) {
                func_00579240(0x16, t, 0, 0x77, speed);
                func_00579240(0x1C, t, 2, 0, speed);
                func_00579240(0x32, t, 0, 0x78, speed);
                func_00579240(0x38, t, 2, 0, speed);
            } else {
                func_00579240(0x16, t, 0, 0x75, speed);
                func_00579240(0x1C, t, 2, 0, speed);
                func_00579240(0x32, t, 0, 0x76, speed);
                func_00579240(0x38, t, 2, 0, speed);
            }
        } else if (t->unkB9B & 1) {
            func_00579240(0x16, t, 0, 0x5C, speed);
            func_00579240(0x1C, t, 2, 0, speed);
            func_00579240(0x32, t, 0, 0x5D, speed);
            func_00579240(0x38, t, 2, 0, speed);
        } else {
            func_00579240(0x16, t, 0, 0x5A, speed);
            func_00579240(0x1C, t, 2, 0, speed);
            func_00579240(0x32, t, 0, 0x5B, speed);
            func_00579240(0x38, t, 2, 0, speed);
        }
        break;
    case 0x86:
        func_00578DB0(0x3A, t, snd, range, speed);
        func_00578DB0(0x56, t, snd, range, speed);
        func_00578DB0(4, t, 0x48, 0x10, speed);
        if (t->unkB9B & 1) {
            func_00579240(0xE, t, 0, 0x38, speed);
            func_00579240(0x16, t, 2, 0, speed);
        } else {
            func_00579240(0xE, t, 0, 0x29, speed);
            func_00579240(0x16, t, 2, 0, speed);
        }
        break;
    case 0x87:
        func_00578DB0(0x1C, t, snd, range, speed);
        func_00578DB0(0x40, t, snd, range, speed);
        func_00578DB0(0x56, t, snd, range, speed);
        func_00578DB0(0x92, t, snd, range, speed);
        func_00578DB0(0xBA, t, snd, range, speed);
        func_00578DB0(0xD2, t, snd, range, speed);
        func_00578DB0(0x10C, t, snd, range, speed);
        func_00578DB0(0x124, t, snd, range, speed);
        func_00578DB0(0x144, t, snd, range, speed);
        func_005792F0(0x88, t, speed);
        break;
    case 0x88:
        func_00578DB0(0x56, t, 3, 0, speed);
        func_00578DB0(0x66, t, 3, 0, speed);
        func_00578DB0(0x84, t, 3, 0, speed);
        func_00578DB0(0xAA, t, 3, 0, speed);
        func_00578DB0(0xCC, t, 3, 0, speed);
        break;
    case 0x8C:
        func_00578DB0(2, t, 5, 0, speed);
        func_00578DB0(0x80, t, snd, range, speed);
        func_00578DB0(0x9A, t, snd, range, speed);
        break;
    case 0x8D:
        func_00578DB0(2, t, 5, 0, speed);
        func_00578DB0(0xA4, t, snd, range, speed);
        func_00578DB0(0xB8, t, snd, range, speed);
        break;
    case 0x8E:
        func_00578DB0(2, t, 5, 0, speed);
        func_00578DB0(0x6E, t, snd, range, speed);
        func_00578DB0(0x106, t, snd, range, speed);
        break;
    case 0x8F:
        func_00578DB0(2, t, 5, 0, speed);
        func_00578DB0(0xA2, t, snd, range, speed);
        func_00578DB0(0xD8, t, snd, range, speed);
        func_00578DB0(0xE6, t, snd, range, speed);
        break;
    case 0x90:
        func_00578DB0(2, t, 5, 0, speed);
        func_00578DB0(0x56, t, snd, range, speed);
        func_00578DB0(0x78, t, snd, range, speed);
        break;
    case 0x91:
        func_00578DB0(2, t, 5, 0, speed);
        func_00578DB0(0xA0, t, snd, range, speed);
        break;
    case 0x92:
        func_00578DB0(0x12, t, snd, range, speed);
        func_00578DB0(0x2C, t, snd, range, speed);
        func_00578DB0(0x48, t, snd1, 0, speed);
        func_00578DB0(0x82, t, snd2, 0, speed);
        func_00578DB0(0x9E, t, snd2, 0, speed);
        break;
    case 0xC8:
        func_00578DB0(5, t, 6, 0, speed);
        func_00579170(0x32, t, speed);
        break;
    case 0x12C:
    case 0x12D:
    case 0x12E:
    case 0x12F:
    case 0x130:
    case 0x131:
    case 0x132:
    case 0x133:
    case 0x134:
    case 0x135:
    case 0x136:
    case 0x137:
    case 0x138:
    case 0x139:
        func_00578DB0(2, t, 0x38, 8, speed);
        break;
    case 0x13A:
        func_00578DB0(2, t, 0x38, 8, speed);
        func_00578DB0(0x10, t, snd, range, speed);
        func_00578DB0(0x2A, t, snd, range, speed);
        func_00578DB0(0x3A, t, 3, 0, speed);
        func_00578DB0(0x7A, t, snd, range, speed);
        break;
    case 0x13B:
        func_00578DB0(2, t, 0x38, 8, speed);
        func_00578DB0(0x20, t, snd, range, speed);
        func_00578DB0(0x36, t, snd, range, speed);
        func_00578DB0(0x68, t, snd, range, speed);
        break;
    case 0x13C:
        if (t->act[0] == 2 && t->act[1] == 0xA) {
            break;
        }
        if (t->act[0] == 5 && t->act[1] == 0x20) {
            break;
        }
        func_00578DB0(2, t, 0x40, 8, speed);
        func_00578DB0(0x32, t, snd2, 0, speed);
        break;
    case 0x13D:
        func_00578DB0(2, t, 0x40, 8, speed);
        func_00578DB0(0x40, t, snd2, 0, speed);
        func_00578DB0(0x4A, t, snd2, 0, speed);
        break;
    case 0x13E:
        func_00578DB0(2, t, 0x38, 8, speed);
        func_00578DB0(0x32, t, snd2, 0, speed);
        func_00578DB0(0x92, t, snd2, 0, speed);
        break;
    case 0x13F:
        func_00578DB0(2, t, 0x38, 8, speed);
        func_00578DB0(0x34, t, snd1, 0, speed);
        func_00578DB0(0x6C, t, snd2, 0, speed);
        func_00578DB0(0x86, t, snd2, 0, speed);
        break;
    case 0x140:
        func_00578DB0(2, t, 0x40, 8, speed);
        func_00578DB0(0x26, t, 3, 0, speed);
        func_00578DB0(0x42, t, 3, 0, speed);
        func_00578DB0(0x58, t, 3, 0, speed);
        func_00578DB0(0x8A, t, snd2, 0, speed);
        func_00578DB0(0x92, t, snd2, 0, speed);
        break;
    case 0x141:
        func_00578DB0(2, t, 0x40, 8, speed);
        func_00578DB0(0x18, t, 3, 0, speed);
        func_00578DB0(0x2E, t, 3, 0, speed);
        func_00578DB0(0x6A, t, snd2, 0, speed);
        func_00578DB0(0x84, t, snd2, 0, speed);
        break;
    case 0x142:
        func_00578DB0(2, t, 0x40, 8, speed);
        func_00578DB0(0x2A, t, snd2, 0, speed);
        func_00578DB0(0x42, t, snd2, 0, speed);
        break;
    case 0x143:
    case 0x144:
        func_00578DB0(0x22, t, snd1, 0, speed);
        func_00578DB0(0x50, t, snd2, 0, speed);
        break;
    case 0x14B:
        func_00578DB0(0x30, t, snd2, 0, speed);
        break;
    case 0x14F:
        func_00578DB0(0x15, t, snd, range, speed);
        break;
    case 0x150:
        func_00578DB0(0x12, t, snd, range, speed);
        func_00578DB0(0x5A, t, snd2, 0, speed);
        func_00578DB0(0x6E, t, snd2, 0, speed);
        break;
    case 0x151:
        func_00578DB0(0x38, t, snd, range, speed);
        func_00578DB0(0x46, t, snd, range, speed);
        break;
    case 0x152:
        func_00578DB0(0x46, t, snd2, 0, speed);
        func_00578DB0(0x56, t, snd2, 0, speed);
        break;
    case 0x153:
        func_00578DB0(0x24, t, snd2, 0, speed);
        func_00578DB0(0x68, t, snd2, 0, speed);
        func_00578DB0(0x72, t, snd2, 0, speed);
        func_00578DB0(0x80, t, snd2, 0, speed);
        break;
    case 0x154:
        func_00578DB0(0x22, t, snd2, 0, speed);
        func_00578DB0(0x7C, t, snd2, 0, speed);
        break;
    case 0x155:
        func_00578DB0(2, t, 0x12, 0, speed);
        break;
    case 0x156:
        func_00578DB0(0x16, t, 0x12, 0, speed);
        func_00578DB0(0x52, t, 0x12, 0, speed);
        break;
    case 0x157:
        func_00578DB0(0x22, t, 0x40, 8, speed);
        break;
    case 0x158:
        func_00578DB0(0x14, t, 0x12, 0, speed);
        func_00578DB0(0x50, t, 0x12, 0, speed);
        break;
    case 0x159:
        func_00578DB0(0x20, t, snd, range, speed);
        func_00578DB0(0x3A, t, snd, range, speed);
        func_00578DB0(0x52, t, snd, range, speed);
        func_00578DB0(0x70, t, snd, range, speed);
        break;
    case 0x15A:
        func_00578DB0(0x16, t, snd, range, speed);
        func_00578DB0(0x24, t, snd, range, speed);
        func_00578DB0(0x3A, t, snd, range, speed);
        func_00578DB0(0x4A, t, snd, range, speed);
        func_00578DB0(0xD0, t, snd, range, speed);
        break;
    case 0x190:
        func_00578DB0(0x40, t, snd2, 0, speed);
        func_00578DB0(0x4E, t, snd2, 0, speed);
        break;
    case 0x191:
        func_00578DB0(0xC, t, snd, range, speed);
        func_00578DB0(0x14, t, snd, range, speed);
        break;
    case 0x193:
        func_00578DB0(0x12, t, snd, range, speed);
        func_00578DB0(0x2C, t, snd, range, speed);
        func_00578DB0(0x4A, t, snd, range, speed);
        break;
    case 0x194:
        func_00578DB0(0xD0, t, snd, range, speed);
        func_00578DB0(0xF0, t, snd, range, speed);
        func_00578DB0(0x108, t, snd, range, speed);
        break;
    case 0x196:
        func_00578DB0(0x5A, t, snd1, 0, speed);
        break;
    case 0x197:
        func_00578DB0(0x66, t, snd, range, speed);
        func_00578DB0(0x80, t, snd, range, speed);
        func_00578DB0(0x90, t, snd, range, speed);
        func_00578DB0(0xAE, t, snd, range, speed);
        func_00578DB0(0x100, t, snd, range, speed);
        break;
    case 0x198:
        func_00578DB0(0x70, t, snd, range, speed);
        func_00578DB0(0x98, t, snd, range, speed);
        func_00578DB0(0xB4, t, snd, range, speed);
        func_00578DB0(0xC6, t, snd, range, speed);
        break;
    case 0x199:
        func_00578DB0(0x3E, t, 0x30, 0, speed);
        break;
    case 0x19B:
        func_00578DB0(0x30, t, snd, range, speed);
        func_00578DB0(0x46, t, snd, range, speed);
        func_00578DB0(0x70, t, snd, range, speed);
        func_00578DB0(0x92, t, snd, range, speed);
        func_00578DB0(0xAC, t, snd, range, speed);
        break;
    case 0x19C:
        func_00578DB0(0x16, t, snd, range, speed);
        func_00578DB0(0x20, t, snd, range, speed);
        break;
    case 0x19D:
        func_00578DB0(0x1C, t, snd, range, speed);
        func_00578DB0(0x36, t, snd, range, speed);
        func_00578DB0(0x4C, t, snd, range, speed);
        func_00578DB0(2, t, 0x48, 0x10, speed);
        if (t->unk4 != 6) {
            if (func_001AF4D0() == 1) {
                n = (t->unkB9B & 1) ? 0x73 : 0x71;
            } else {
                n = (t->unkB9B & 1) ? 0x56 : 1;
            }
            func_00579340(0xA, t, n, speed);
        }
        break;
    case 0x19F:
        func_00578DB0(0x40, t, snd, range, speed);
        func_00578DB0(0x5A, t, snd, range, speed);
        func_00578DB0(0xBE, t, snd, range, speed);
        break;
    case 0x1A0:
        func_00578DB0(0x92, t, snd, range, speed);
        func_00578DB0(0xB8, t, snd, range, speed);
        func_00578DB0(0xE0, t, snd, range, speed);
        break;
    case 0x1A1:
        func_00578DB0(0x66, t, snd, range, speed);
        func_00578DB0(0x7C, t, snd, range, speed);
        func_00578DB0(0xA2, t, snd1, 0, speed);
        break;
    case 0x1A2:
        func_00578DB0(0x24, t, 3, 0, speed);
        func_00578DB0(0x5C, t, 3, 0, speed);
        break;
    case 0x1A3:
        if (func_001AF4D0() == 1) {
            n = (t->unkB9B & 1) ? 0x74 : 0x72;
        } else {
            n = (t->unkB9B & 1) ? 0x57 : 2;
        }
        func_00579340(0x13, t, n, speed);
        break;
    case 0x1A6:
        func_00578DB0(0x3E, t, 0xC, 0, speed);
        break;
    case 0x1A7:
        func_00578DB0(0x4C, t, snd2, 0, speed);
        break;
    case 0x1C2:
        func_00579240(4, t, 0, 0x28, speed);
        func_00579240(2, t, 2, 0, speed);
        break;
    case 0x1C4:
        func_00578DB0(2, t, 0x48, 0x10, speed);
        break;
    case 0x1C5:
        func_00578DB0(0x16, t, 0x30, 0, speed);
        break;
    case 0x1F4:
        func_00578DB0(0x18, t, 0x30, 0, speed);
        break;
    case 0x1F5:
        func_00578DB0(0x28, t, 0x30, 0, speed);
        break;
    case 0x1F6:
        func_00578DB0(0x22, t, 0x30, 0, speed);
        break;
    case 0x1F7:
        func_00578DB0(0x16, t, 0x30, 0, speed);
        break;
    case 0x1FA:
        func_00578DB0(0x1A, t, 0x31, 0, speed);
        break;
    case 0x1FB:
        func_00578DB0(0x1A, t, 0x31, 0, speed);
        break;
    case 0x1FC:
        func_00578DB0(0xE, t, 0x30, 0, speed);
        break;
    }
}
#endif
