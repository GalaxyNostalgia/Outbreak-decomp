#include "common.h"
#include "pad.h"
#include "options.h"

/* Each player's controls as the game logic reads them: button history, sticks as angles, menu
   auto-repeat. */

#include "ctrl.h"

typedef struct {
    u8 status;
    u8 flags;
    u16 type;
    u8 id[4];
    u32 cur;            /* 0x08 */
    u32 prev;
    u32 pressed;        /* 0x10 */
    u32 released;       /* 0x14 */
    u32 changed;
    PadStick right;     /* 0x1C */
    PadStick left;      /* 0x28 */
    u8 unk34[0x78 - 0x34];
} Input;

extern Input *D_0023BAB8;
extern Input *D_0023BABC;
extern s32 D_0023BB90;          /* players */
extern Ctrl D_003063E0;         /* menus: player 1 with repeat */
extern Ctrl D_00306410[4];
extern Ctrl D_003064D0[4];
extern f32 D_00239CA0;          /* pi */
extern f32 D_00239CA4;          /* 0x10000 / 360 */

/* Radians to the game's angle units (0x10000 a turn). */
u32 func_001918C0(f32 rad) {
    f32 deg = (180.0f * rad) / D_00239CA0;

    if (deg < 0.0f) {
        deg += 360.0f;
    }
    return D_00239CA4 * deg;
}

typedef struct { s16 x, y, z, w; } PadVec;
extern PadVec D_0023BAE8[1];

void func_001A53E0(s32 i, s16 *a, s16 *b, s16 *c) {
    *a = D_0023BAE8[i].x;
    *b = D_0023BAE8[i].y;
    *c = D_0023BAE8[i].z;
}

#ifdef NON_MATCHING   /* first draft */
/* An axis past the dead zone, rescaled to reach 127. */
s16 func_001A1050(f32 scale, s16 v, s16 dead) {
    s32 sign = 1;

    if (v != 0) {
        if (v < 0) {
            v = -v;
            sign = -1;
        }
        if (v < dead) {
            v = 0;
        }
        if (v != 0) {
            v = (s32)(scale * (v - (dead - 1)));
        }
        v = v * sign;
    }
    return v;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* A stick with the player's dead zone applied. */
void func_001A0F00(s32 player, s32 which, PadStick *out) {
    s16 unused;
    s16 dead;
    s16 dead2;
    f32 scale;
    s32 d;

    *out = (&D_0023BABC[player].right)[which];
    func_001A53E0(player, &unused, &dead, &dead2);
    if (which != 0) {
        dead = dead2;
    }
    d = dead - 1;
    scale = 127.0f / (127.0f - d);
    if (out->mag != 0) {
        out->mag = (u32)(scale * (out->mag - d));
    }
    out->x = func_001A1050(scale, out->x, dead);
    out->y = func_001A1050(scale, out->y, dead);
}
#endif

#ifdef NON_MATCHING   /* first draft: the original doesn't share these two helpers the same way */
static void ctrl_clear(Ctrl *c) {
    c->cur = 0;
    c->pressed = 0;
    c->released = 0;
    c->rmag = 0;
    c->lmag = 0;
    c->rangle = 0;
    c->langle = 0;
    c->rx = 0;
    c->ry = 0;
    c->lx = 0;
    c->ly = 0;
}

static void ctrl_update(Ctrl *c, Input *in) {
    c->prev[2] = c->prev[1];
    c->prev[1] = c->prev[0];
    c->prev[0] = c->cur;
    c->cur = in->cur;
    c->pressed = in->pressed;
    c->released = in->released;
    c->rx = in->right.x;
    c->ry = in->right.y;
    c->lx = in->left.x;
    c->ly = in->left.y;
    c->rmag = in->right.mag;
    c->lmag = in->left.mag;
    c->rangle = (func_001918C0(in->right.rad) & 0xFFFF) - 0x4000;
    c->langle = (func_001918C0(in->left.rad) & 0xFFFF) - 0x4000;
}

void func_001AF820(void) {
    s32 i;
    PadStick r;
    PadStick l;

    for (i = 0; i < D_0023BB90; i++) {
        if (D_0023BAB8[i].status != 1) {
            ctrl_clear(&D_003064D0[i]);
            ctrl_clear(&D_00306410[i]);
        }
        ctrl_update(&D_003064D0[i], &D_0023BAB8[i]);
        func_001A0F00(i, 0, &r);
        func_001A0F00(i, 1, &l);
        ctrl_update(&D_00306410[i], &D_0023BABC[i]);
    }
}
#endif

/* Menu auto-repeat for the directions (0xF000F): 12 frames, then every 3. */
void func_001AFA90(Ctrl *c) {
    c->repeat = 0;
    if (c->pressed & 0xF000F) {
        c->repeatTimer = 12;
        c->repeat = c->pressed;
        return;
    }
    if (c->cur & 0xF000F) {
        if (--c->repeatTimer < 0) {
            c->repeatTimer = 2;
            c->repeat = c->cur;
        }
    } else {
        c->repeatTimer = 12;
    }
    c->repeat = c->repeat | (c->pressed & 0x8FF0);
}

extern u32 D_002168C0[][3];     /* confirm/cancel/... buttons for each button layout */

#include "object.h"

s32 func_001AF500(void);
s32 func_001AF4D0(void);
extern s8 D_0023BB38[8];        /* per character: its pad */
extern u8 D_0048BF7D[];         /* our player number */

/* A player's controls for this frame (at 0xF78): online ours from pad 0, offline from the
   character's pad (partners have none); confirm/cancel/... mapped by the button layout. */
void func_001AFC60(DrawObjFull *p);
#ifdef NON_MATCHING   /* close: the scheduling of the pad's address */
void func_001AFC60(DrawObjFull *p) {
    Ctrl *c;
    u32 cur;
    u32 v;
    u32 m0;
    u32 m1;
    u32 m2;
    s8 pad;
    s32 n;

    pad = 0;
    if (func_001AF500() == 1) {
        if (p->kind != D_0048BF7D[0]) {
            return;
        }
    } else if (func_001AF4D0() == 0) {
        pad = D_0023BB38[p->kind];
    } else if (p->isNpc) {
        return;
    }
    n = pad;
    if (n == -1) {
        return;
    }
    c = &p->ctrl;
    c->prev[2] = c->prev[1];
    c->prev[1] = c->prev[0];
    c->prev[0] = c->cur;
    cur = D_00306410[n].cur;
    m0 = D_002168C0[D_00306590.unk2C][0];
    m1 = D_002168C0[D_00306590.unk2C][1];
    m2 = D_002168C0[D_00306590.unk2C][2];
    v = 0;
    if (cur & 0x100) {
        v |= m0;
    }
    if (cur & 0x10) {
        v |= m1;
    }
    if (cur & 0x20) {
        v |= m2;
    }
    c->cur = (cur & ~0x130) | v;
    c->pressed = c->cur & ~c->prev[0];
    c->released = c->prev[0] & ~c->cur;
    c->rx = D_00306410[n].rx;
    c->ry = D_00306410[n].ry;
    c->lx = D_00306410[n].lx;
    c->ly = D_00306410[n].ly;
    c->rmag = D_00306410[n].rmag;
    c->lmag = D_00306410[n].lmag;
    c->rangle = D_00306410[n].rangle;
    c->langle = D_00306410[n].langle;
}
#endif
extern u32 D_48C020[];          /* (SUBMAIN.BIN) menu buttons */

#ifdef NON_MATCHING   /* first draft */
void func_001AFE90(void) {
    Ctrl *m = &D_003063E0;
    Ctrl *c = &D_003064D0[0];
    u32 v;

    m->prev[2] = m->prev[1];
    m->prev[1] = m->prev[0];
    m->prev[0] = m->cur;
    m->cur = c->cur;
    m->rx = c->rx;
    m->ry = c->ry;
    m->pressed = m->cur & ~m->prev[0];
    m->released = m->prev[0] & ~m->cur;
    m->lx = c->lx;
    m->ly = c->ly;
    m->rmag = c->rmag;
    m->lmag = c->lmag;
    m->rangle = c->rangle;
    m->langle = c->langle;
    func_001AFA90(m);
    m->pressed |= (m->pressed & 0xF0000) >> 16;
    m->cur |= (m->cur & 0xF0000) >> 16;
    m->repeat |= (m->repeat & 0xF0000) >> 16;
    v = 0;
    if (m->pressed & 0x100) {
        v |= D_002168C0[D_00306590.unk2C][0];
    }
    if (m->pressed & 0x10) {
        v |= D_002168C0[D_00306590.unk2C][1];
    }
    if (m->pressed & 0x20) {
        v |= D_002168C0[D_00306590.unk2C][2];
    }
    D_48C020[0] = (m->pressed & ~0x130) | v;
}
#endif
