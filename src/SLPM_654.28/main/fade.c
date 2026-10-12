#include "common.h"
#include "options.h"

/* Screen fades. */

/* A fade to or from black: level goes 0..1 by speed each frame, alpha is level * 255. */
typedef struct {
    u8 state;       /* 0x00: 0 idle, 1 fading, 2 done (black) */
    u8 pad1[3];
    s32 fadeIn;     /* 0x04: 0 out (level falls), else in (level rises) */
    u8 pad8[4];
    u32 color;      /* 0x0C */
    u8 alpha;       /* 0x10 */
    u8 pad11[3];
    f32 level;      /* 0x14 */
    f32 speed;      /* 0x18 */
} Fade;

extern Fade D_002F8800;

void func_001BA960(void) {
    Fade *f = &D_002F8800;

    switch (D_002F8800.state) {
    case 0:
        break;
    case 1:
        if (f->fadeIn == 0) {
            if ((f->level -= f->speed) <= 0.0f) {
                f->state = 0;
                f->level = 0.0f;
            }
            f->alpha = 255.0f * f->level;
        } else {
            if (!((f->level += f->speed) < 1.0f)) {
                f->state = 2;
                f->level = 1.0f;
            }
            f->alpha = 255.0f * f->level;
        }
        break;
    case 2:
        break;
    }
}

/* A colour whose alpha (top byte) moves toward a target's by step each frame. */
typedef struct {
    u8 state;       /* 0x00: 0 idle, 1 running */
    u8 pad1[3];
    s16 active;     /* 0x04 */
    s16 mode;       /* 0x06: 1 alpha falls to the target, 2 rises */
    u8 pad8[4];
    u32 color;      /* 0x0C */
    s16 step;       /* 0x10 */
    s16 frames;     /* 0x12 */
    u8 *target;     /* 0x14: a FadeArea */
} AlphaFade;

extern AlphaFade D_002F8820;

#ifdef NON_MATCHING   /* loads the target pointer before the colour; the original the other way round */
void func_001BA570(void) {
    AlphaFade *f = &D_002F8820;
    s32 a;

    func_001BA960();
    switch (D_002F8820.state) {
    case 0:
        if (f->active == 0) {
            break;
        }
        f->state++;
    case 1:
        if (f->active == 0) {
            f->state = 0;
            f->target = NULL;
            break;
        }
        switch (f->mode) {
        case 0:
            f->active = 0;
            break;
        case 1:
            a = (f->color & 0xFF000000) >> 24;
            if (a != f->target[0xD]) {
                a += f->step;
                if (a < f->target[0xD]) {
                    a = f->target[0xD];
                }
                f->color = (a << 24) | (f->color & 0xFFFFFF);
            } else {
                f->active = 0;
            }
            break;
        case 2:
            a = (f->color & 0xFF000000) >> 24;
            if (a != f->target[0xD]) {
                a += f->step;
                if (f->target[0xD] < a) {
                    a = f->target[0xD];
                }
                f->color = (a << 24) | (f->color & 0xFFFFFF);
            }
            break;
        }
        break;
    }
}
#endif

typedef struct {
    s16 x0, y0;
    s16 x1, y1;
    u32 color;
} Rect2D;

void func_001AE9B0(void);
void func_001AE9C0(s32 cmd, u32 arg);
s32 func_001B6980(s32 id);
void func_001AE430(s32 a, s32 b);
s32 func_001AAE10(Rect2D *r);
s32 func_001A0170(u8 mode, u32 c);
s32 func_00195970(u8 cmd, u32 arg);

/* Draw the fade to black over the whole screen. */
void func_001BAAB0(void) {
    Fade *f = &D_002F8800;
    Rect2D r;

    func_001AE9C0(0x6C, 1);
    func_001A0170(0, 0);
    func_00195970(0x6C, 0);
    if (func_001B6980(0x58) == 0 && f->state != 0) {
        func_00195970(0x5F, 5);
        func_00195970(0x60, 0);
        func_001AE430(4, 5);
        r.x0 = 0;
        r.x1 = 640;
        r.y0 = 0;
        r.y1 = 448;
        r.color = (f->color & 0xFFFFFF) | (f->alpha << 24);
        func_001AAE10(&r);
    }
}

typedef struct {
    u8 unk0;
    u8 colorIndex;  /* 0x01 */
    u8 pad2[2];
    s16 x;          /* 0x04 */
    s16 y;
    s16 w;
    s16 h;
    u8 from;        /* 0x0C: alpha */
    u8 to;          /* 0x0D */
    s16 frames;     /* 0x0E */
} FadeArea;

/* The frame's screen effects: the fade to black, then the coloured fade over its area. */
void func_001BA6C0(void) {
    AlphaFade *f = &D_002F8820;
    Rect2D r;

    if (D_00306590.unk0 == 0) {
        func_001BAAB0();
    }
    func_001AE9B0();
    func_001AE9C0(0x5F, 5);
    func_001AE9C0(0x60, 0);
    func_001AE9C0(0x6C, 0);
    if (func_001B6980(0x58) == 0 && f->active != 0) {
        func_001AE430(4, 5);
        r.x0 = ((FadeArea *)f->target)->x;
        r.y0 = ((FadeArea *)f->target)->y;
        r.x1 = r.x0 + ((FadeArea *)f->target)->w;
        r.y1 = r.y0 + ((FadeArea *)f->target)->h;
        r.color = f->color;
        func_001AAE10(&r);
    }
}

extern FadeArea D_002172A0[];
extern u32 D_00239FA8[2];       /* fade colours */

#ifdef NON_MATCHING   /* first draft */
/* Start coloured fade `id` (1-based, D_002172A0). */
void func_001BA7A0(s16 id) {
    AlphaFade *f;
    FadeArea *a;

    f = &D_002F8820;
    D_002F8820.active = id;
    a = &D_002172A0[id - 1];
    D_002F8820.target = (u8 *)a;
    if (a->to < a->from) {
        f->mode = 1;
        f->step = a->from - a->to;
    } else {
        f->mode = 2;
        f->step = a->to - a->from;
    }
    f->frames = a->frames;
    f->step = f->step / f->frames;
    if (f->mode == 1) {
        f->step = -f->step;
    }
    f->color = ((a->from & 0xFF) << 24) | (D_00239FA8[a->colorIndex] & 0xFFFFFF);
}
#endif

void func_001BA940(void);

void func_001BA870(void) {
    D_002F8820.state = 0;
    D_002F8820.active = 0;
    D_002F8820.mode = 0;
    D_002F8820.target = NULL;
    D_002F8820.frames = 0;
    func_001BA940();
}
