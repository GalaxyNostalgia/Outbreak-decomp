#include "common.h"
#include "options.h"
#include "task.h"
#include "sce.h"
#include "ps2mem.h"
#include "message.h"

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

void func_001BA570(void);
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

/* The brightness option: above 0x80 the screen is brightened (added white), below it darkened. */
void func_001BAB90(void) {
    u32 b = D_00306590.brightness;
    s32 c;
    Rect2D r;

    if (b == 0x80) {
        return;
    }
    if (b > 0x80) {
        func_001AE430(4, 1);
        b -= 0x80;
        c = 0xFF;
    } else {
        func_001AE430(4, 5);
        c = 0;
        b = 0x80 - b;
        b *= 2;
    }
    r.x1 = 640;
    r.y0 = 0;
    r.y1 = 448;
    r.x0 = 0;
    r.color = c | (c << 8 | (b << 24 | c << 16));
    func_001AE9C0(0x5F, 5);
    func_001AE9C0(0x60, 0);
    func_001AE9C0(0x6C, 0);
    func_001AAE10(&r);
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

void func_001BA7A0(s16 id);
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

/* Letterbox bars (top and bottom), fading in or out. */
typedef struct {
    u8 mode;            /* 0x00: 0 idle, 1 fading in, 2 fading out */
    s32 timer;          /* 0x04 */
    s32 duration;       /* 0x08 */
    s16 x0, y0, x1, y1; /* 0x0C: top bar */
    u32 color;          /* 0x14 */
    s16 bx0, by0, bx1, by1; /* 0x18: bottom bar */
    u32 color2;         /* 0x20 */
} Letterbox;

extern Letterbox D_002F8860;

void func_001BA060(s32 top, s32 bottom);

void func_001B9ED0(Task *t);
#ifdef NON_MATCHING   /* first draft */
void func_001B9ED0(Task *t) {
    if (t->step == 0) {
        t->step++;
        memset(&D_002F8860, 0, sizeof(Letterbox));
        func_001BA060(0x40, 0x40);
        return;
    }
    switch (D_002F8860.mode) {
    case 0:
        break;
    case 2:
        if (D_002F8860.timer > 0) {
            D_002F8860.timer--;
        }
        if (D_002F8860.timer == 0) {
            D_002F8860.mode = 0;
            break;
        }
        D_002F8860.color &= 0xFFFFFF;
        D_002F8860.color |= ((D_002F8860.timer * 255 / D_002F8860.duration) & 0xFF) << 24;
        D_002F8860.color2 = D_002F8860.color;
        break;
    case 1:
        if (D_002F8860.timer > 0) {
            D_002F8860.timer--;
        }
        D_002F8860.color &= 0xFFFFFF;
        D_002F8860.color |= (((D_002F8860.duration - D_002F8860.timer) * 255 / D_002F8860.duration) & 0xFF) << 24;
        D_002F8860.color2 = D_002F8860.color;
        break;
    }
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Bars of these heights at the top and bottom of the 640x448 screen. */
void func_001BA060(s32 top, s32 bottom) {
    D_002F8860.y1 = top;
    D_002F8860.y0 = 0;
    D_002F8860.x0 = 0;
    D_002F8860.by1 = 0x1C0;
    D_002F8860.by0 = 0x1C0 - bottom;
    D_002F8860.x1 = 0x280;
    D_002F8860.bx1 = 0x280;
    D_002F8860.bx0 = 0;
}
#endif

/* A full-screen fade with a float level. */
typedef struct {
    u8 mode;            /* 0x00: 1 rising, 2 falling, 0x63 done */
    s32 wait;           /* 0x04 */
    u32 color;          /* 0x08 */
    f32 level;          /* 0x0C */
    f32 step;           /* 0x10 */
    s16 x, y, w, h;     /* 0x14 */
    u8 pad1C[4];
} LevelFade;

extern LevelFade D_002F8890;

void func_001B9A60(Task *t) {
    if (t->step == 0) {
        t->step = 1;
        memset(&D_002F8890, 0, sizeof(LevelFade));
        D_002F8890.x = 0;
        D_002F8890.w = 0x280;
        D_002F8890.h = 0x1E0;
        D_002F8890.y = 0;
        return;
    }
    switch (D_002F8890.mode) {
    case 0:
        break;
    case 1:
        D_002F8890.level += D_002F8890.step;
        if (!(D_002F8890.level <= 1.0f)) {
            D_002F8890.level = 1.0f;
            D_002F8890.mode = 0x63;
            D_002F8890.wait = 0x63;
        }
        break;
    case 2:
        D_002F8890.level += D_002F8890.step;
        if (D_002F8890.level < 0.0f) {
            D_002F8890.level = 0.0f;
            D_002F8890.wait = 0x63;
            D_002F8890.mode = 0;
        }
        break;
    default:
        if (D_002F8890.wait == 0) {
            D_002F8890.mode = 0;
        }
        break;
    }
}

/* Fades the screen out to color over frames (fewer at double speed). */
s32 func_001B9C30(s16 frames, u32 color) {
    LevelFade *f = &D_002F8890;

    if (D_002F8890.mode != 2 && D_002F8890.mode != 1) {
        f->level = 1.0f;
    }
    f->color = color;
    frames = frames >> D_00306590.unkC;
    if (frames == 0) {
        frames = 1;
    }
    f->step = -1.0f / frames;
    D_002F8890.mode = 2;
    D_002F8890.wait = 2;
    return 1;
}

void func_001B9E90(void) {
    D_002F8890.mode = 0;
    D_002F8890.wait = 0;
}

/* Drawing the fades. */

extern Rect2D D_002F884C;
s32 func_001B6980(s32 slot);
s32 func_001AAE10(Rect2D *r);
void func_001AE430(s32 a, s32 b);
void func_001AE9C0(s32 cmd, u32 arg);
void func_001B9DA0(void);

extern u8 D_002F8840[0x18];     /* the message backdrop's state (the rectangle at +0xC) */
extern MsgAction *D_0023BD40;   /* the message backdrop's colour now (a kind-2 message action) */

/* A kind-2 message action sets the backdrop's colour from now on. */
void func_001BA160(MsgAction *e) {
    if (e->kind == 2) {
        D_0023BD40 = e;
    }
}

/* The backdrop's colour: blended toward the next kind-2 action's by the time between them. */
void func_001BA180(void);
#ifdef NON_MATCHING   /* first draft: control flow and the colour's operand order */
void func_001BA180(void) {
    MsgAction *n;
    s32 i;
    u32 t0;
    s32 d;
    f32 f;
    u32 c;

    if (D_0023BD40 == NULL) {
        return;
    }
    for (i = 1; i <= 0x20; i++) {
        n = &D_0023BD40[i];
        if (n->kind != 2) {
            continue;
        }
        if (n->time == 0xFFFF || D_0023BD40->time >= n->time) {
            n = NULL;
        }
        if (n == NULL) {
            c = D_0023BD40->bits[2] | (D_0023BD40->bits[1] << 8) | (D_0023BD40->bits[3] << 24) |
                (D_0023BD40->bits[0] << 16);
        } else {
            t0 = D_0023BD40->time >> 1;
            d = (n->time >> 1) - t0;
            if (d == 0) {
                f = 1.0f;
            } else {
                f = (f32)(D_0035FFD0.frames / 2 - t0) / (f32)d;
            }
            c = (s32)(D_0023BD40->bits[2] + f * (n->bits[2] - (f32)D_0023BD40->bits[2])) |
                ((s32)(D_0023BD40->bits[1] + f * (n->bits[1] - (f32)D_0023BD40->bits[1])) << 8) |
                ((s32)(D_0023BD40->bits[3] + f * (n->bits[3] - (f32)D_0023BD40->bits[3])) << 24) |
                ((s32)(D_0023BD40->bits[0] + f * (n->bits[0] - (f32)D_0023BD40->bits[0])) << 16);
        }
        D_002F884C.color = c;
        return;
    }
}
#endif

/* The full-screen rectangle behind messages, in this colour (drawn while D_0048BF60.unk28). */
void func_001BA0F0(u32 color) {
    memset(D_002F8840, 0, sizeof(D_002F8840));
    D_002F884C.color = color;
    D_0023BD40 = NULL;
    D_002F884C.x1 = 0x280;
    D_002F884C.y1 = 0x1E0;
    D_002F884C.x0 = 0;
    D_002F884C.y0 = 0;
}

void func_001B9CC0(void) {
    if (func_001B6980(0x58) != 0) {
        return;
    }
    if (D_00306590.unk0 != 0 && *(s8 *)PS2_RAM(0x48BF88) != 0 && (D_002F884C.color & 0xFF000000)) {
        func_001AE9C0(0x6C, 0);
        func_001AE430(4, 5);
        func_001AE9C0(0x5E, 0x32);
        func_001AAE10(&D_002F884C);
    }
    if (D_002F8860.mode != 0) {
        func_001AE9C0(0x6C, 0);
        func_001AE430(4, 5);
        func_001AE9C0(0x5E, 0x32);
        func_001AAE10((Rect2D *)&D_002F8860.x0);
        func_001AAE10((Rect2D *)&D_002F8860.bx0);
    }
    func_001B9DA0();
}

#ifdef NON_MATCHING   /* first draft */
void func_001B9DA0(void) {
    u32 a;

    if (func_001B6980(0x58) == 0 && D_002F8890.wait != 0) {
        a = 255.0f * D_002F8890.level;
        if (a != 0) {
            func_001AE9C0(0x6C, 0);
            *(u32 *)&D_002F8890.pad1C = (D_002F8890.color & 0xFFFFFF) | ((a & 0xFF) << 24);
            func_001AE430(4, 5);
            func_001AE9C0(0x5E, 0x32);
            func_001AAE10((Rect2D *)&D_002F8890.x);
        }
    }
}
#endif
