#include "common.h"
#include "pad.h"

/* Reading the controller each frame: libpad2's data becomes the game's button bits, stick angles and
   hold counters. */

double atan2(double y, double x);
double sqrt(double x);

/* The game's view of the controller (D_002922C0, D_0023BAB8 points at it). */
typedef struct {
    u8 status;
    u8 flags;
    u16 type;
    u8 id[4];           /* 0x04: port, slot, motor */
    u32 cur;            /* 0x08: buttons held */
    u32 prev;           /* 0x0C */
    u32 pressed;        /* 0x10 */
    u32 released;       /* 0x14 */
    u32 changed;        /* 0x18 */
    PadStick right;     /* 0x1C */
    PadStick left;      /* 0x28 */
    u32 pressed2;       /* 0x34 */
    s16 hold[24];       /* 0x38: frames each button has been held (low byte, up to 255) */
    u8 unk68[0x78 - 0x68];
} Input;

extern Input *D_0023BAB8;
extern u8 D_0023BAAC;           /* a controller of a known type is in */
extern u32 D_00223790[];        /* the game's button bits, in libpad2 order */

void func_001A11D0(Input *in, u32 buttons) {
    in->prev = in->cur;
    in->cur = buttons;
    in->pressed = in->cur & (in->prev ^ in->cur);
    in->released = in->prev & (in->prev ^ in->cur);
    in->changed = in->pressed | in->released;
}

void func_001A1220(Input *in) {
    s16 i;

    for (i = 0; i < 24; i++) {
        if (in->cur & D_00223790[i]) {
            if (*(u8 *)&in->hold[i] != 0xFF) {
                (*(u8 *)&in->hold[i])++;
            }
        } else {
            in->hold[i] = 0;
        }
    }
}

void func_001A5420(void);

#ifdef NON_MATCHING   /* first draft */
void func_001A10F0(void) {
    func_001A5420();
    D_0023BAAC = 0;
    D_0023BAB8->status = D_00292D80[0].status;
    D_0023BAB8->flags = D_00292D80[0].flags;
    D_0023BAB8->type = D_00292D80[0].type;
    *(f32 *)D_0023BAB8->id = *(f32 *)&D_00292D80[0].port;
    if (D_0023BAB8->type != 0 && D_0023BAB8->type != 0x8000) {
        D_0023BAAC++;
    }
    D_0023BAB8->right = D_00292D80[0].right;
    D_0023BAB8->left = D_00292D80[0].left;
    func_001A11D0(D_0023BAB8, D_00292D80[0].buttons);
    func_001A1220(D_0023BAB8);
    D_0023BAB8->pressed2 = D_0023BAB8->pressed;
}
#endif

extern f32 D_00239CAC;      /* stick scale */

#ifdef NON_MATCHING   /* first draft */
/* A stick axis (0-255) to -127..127, with a dead zone from 88 to 167. */
s32 func_001A5550(u32 v) {
    f32 f = v;

    if (f < 128.0f) {
        if (f > 87.0f) {
            f = 87.0f;
        }
        f -= 87.0f;
    } else {
        f -= 168.0f;
        if (f < 0.0f) {
            f = 0.0f;
        }
    }
    return f * D_00239CAC;
}
#endif

extern f32 D_00239CB0;      /* 2 pi */

#ifdef NON_MATCHING   /* first draft */
/* A stick's angle and how far it's pushed; below `dead` it counts as centred. */
void func_001A5940(PadStick *s, s16 dead) {
    f32 rad;

    if ((s->x | s->y) == 0) {
        rad = 0.0f;
    } else {
        rad = atan2(-s->y, s->x);
        if (rad < 0.0f) {
            rad += D_00239CB0;
        }
    }
    s->rad = rad;
    s->angle = 360.0f * (rad / D_00239CB0);
    s->mag = sqrt((double)(s->x * s->x) + (double)(s->y * s->y));
    if (s->mag >= 0x80) {
        s->mag = 0x7F;
    }
    if (s->mag < dead) {
        s->x = 0;
        s->y = 0;
        s->rad = 0.0f;
        s->angle = 0;
        s->mag = 0;
    }
}
#endif

extern u8 D_002136B0[];     /* direction bits for 16 angles */

u8 func_001A5A90(PadStick *s) {
    if (s->mag < 0x25) {
        return 0;
    }
    return D_002136B0[(u32)(s->angle / 22.5f)];
}

typedef struct { s16 x, y, z, w; } PadVec;
extern PadVec D_0023BAE8[1];    /* dead zones */
extern u8 D_002136A0[];         /* d-pad: opposite directions cancel */
extern u8 D_0023BADC;
extern u8 D_0023BAE0;
s32 func_001A5600(s32 port);

#ifdef NON_MATCHING   /* first draft */
void func_001A5420(void) {
    PadInfo *p;
    PadVec *dz;
    s32 r;
    s32 l;

    if (func_001A5600(0) != 0) {
        p = &D_00292D80[0];
        dz = &D_0023BAE8[0];
        p->vibrate = D_00292D60[0].vibrate;
        p->buttons = (p->buttons & 0xFFF0) | D_002136A0[p->buttons & 0xF];
        func_001A5940(&p->right, dz->y);
        func_001A5940(&p->left, dz->z);
        r = func_001A5A90(&p->right) & 0xFF;
        l = (func_001A5A90(&p->left) & 0xFF) << 20;
        p->buttons |= r << 16;
        p->buttons |= l;
    }
    D_0023BADC = D_0023BAE0;
}
#endif

s32 scePad2GetState(s32 socket);
s32 scePad2Read(s32 socket, u8 *data);
s32 scePad2GetButtonProfile(s32 socket, u8 *profile);
s32 func_00189308(s32 socket, u8 *info);                       /* libdbc: actuator info */
s32 func_00189388(s32 socket, s32 n, u8 *mode, s32 len, u8 *data);   /* libdbc: send actuator data */
extern PadMap D_00213620[16];       /* map for a digital pad */
extern PadMap D_002135A0[16];       /* map for an analog pad */

#ifdef NON_MATCHING   /* first draft (the original unrolls the copies) */
/* The motor: level 0-7 for `frames` frames (0: stop). */
s32 func_001A6450(s32 port, s32 level, s32 frames) {
    u8 data[3];
    u8 mode;
    s32 n;
    s32 m;
    u8 v;

    if (D_00292D60[port].actuators == 0) {
        return 0;
    }
    D_00292D60[port].vibTimer = frames;
    if (frames == 0) {
        m = 3;
        D_00292D60[port].actuators = m;
        if (m != 0) {
            mode = 3;
            n = 2;
            data[0] = 0;
            data[1] = 0;
        } else {
            data[0] = 0;
            mode = 2;
            n = 1;
        }
    } else {
        v = ((level & 7) << 5) | 0xF;
        m = 3;
        D_00292D60[port].actuators = m;
        if (m != 0) {
            mode = 3;
            n = 2;
            data[0] = ((v & 0x7F) << 1) | 1;
            data[1] = (v & 0x80) >> 7;
        } else {
            data[0] = v;
            mode = 2;
            n = 1;
        }
    }
    func_00189388(D_00292D60[port].socket, 1, &mode, n, data);
    return 1;
}
#endif

#ifdef NON_MATCHING   /* first draft (the original unrolls the copies) */
/* Read a socket: connect, read the button profile once, then the buttons and sticks every frame. */
void func_001A5C20(s32 port) {
    PadData *raw = &D_00292D40[port];
    PadPort *pp;
    u8 prof[4];
    u8 info[4];
    u8 d[0x20];
    s32 i;

    *raw = D_00292D20[port];
    pp = &D_00292D60[port];
    switch (scePad2GetState(pp->socket)) {
    case 1:
        pp->state = 2;
        if (pp->tries == 0) {
            *raw = D_00292D00;
            if (scePad2GetButtonProfile(pp->socket, prof) < 0) {
                return;
            }
            pp->profile = prof[0] | (prof[1] << 8) | ((prof[3] << 24) | (prof[2] << 16));
            if (func_00189308(pp->socket, info) >= 0) {
                pp->actuators = info[0];
            }
            if (pp->profile & 0xFFFF0000) {
                if (pp->profile & 0xFFF00000) {
                    pp->kind = 2;
                } else {
                    pp->kind = 1;
                }
            } else {
                pp->kind = 0;
            }
            switch (pp->kind) {
            case 0:
                D_00292D80[port].type = 1;
                D_00292D80[port].flags = 0;
                for (i = 0; i < 16; i++) {
                    D_00292C80[port][i] = D_00213620[i];
                }
                break;
            case 1:
                D_00292D80[port].type = 1;
                D_00292D80[port].flags = 0x60;
                for (i = 0; i < 16; i++) {
                    D_00292C80[port][i] = D_002135A0[i];
                }
                break;
            case 2:
                D_00292D80[port].type = 1;
                D_00292D80[port].flags = 0x73;
                for (i = 0; i < 16; i++) {
                    D_00292C80[port][i] = D_002135A0[i];
                }
                break;
            }
            if (!(pp->actuators & 3)) {
                pp->vibrate = 0;
            } else {
                pp->vibrate = 1;
            }
            pp->tries++;
        } else {
            if (scePad2Read(pp->socket, d) < 0) {
                return;
            }
            pp->state = 1;
            raw->status = 0;
            raw->unk1 = 0;
            raw->buttons = (d[0] << 8) | d[1];
            switch (pp->kind) {
            case 0:
                break;
            case 1:
                raw->stick[0] = d[2];
                raw->stick[1] = d[3];
                raw->stick[2] = d[4];
                raw->stick[3] = d[5];
                break;
            case 2:
                raw->stick[0] = d[2];
                raw->stick[1] = d[3];
                raw->stick[2] = d[4];
                raw->stick[3] = d[5];
                for (i = 0; i < 12; i++) {
                    raw->pressure[i] = d[6 + i];
                }
                break;
            default:
                *raw = D_00292D00;
                break;
            }
            if (pp->vibTimer != 0 && --pp->vibTimer == 0) {
                func_001A6450(port, 0, 0);
            }
        }
        D_00292D80[port].status = pp->state;
        return;
    case 0:
        pp->state = 1;
        break;
    default:
        pp->state = 2;
        break;
    }
    pp->tries = 0;
    pp->unk4 = 0;
    pp->vibrate = 0;
    pp->profile = 0;
    pp->actuators = 0;
    *raw = D_00292D00;
    D_00292D80[port].type = 0;
    D_00292D80[port].flags = 0;
    D_00292D80[port].status = pp->state;
}
#endif

void func_001A5C20(s32 port);
s32 func_001A5550(u32 v);

/* This frame's buttons and sticks for a port. */
#ifdef NON_MATCHING   /* first draft (the original unrolls the copies) */
s32 func_001A5600(s32 port) {
    PadData *raw;
    PadInfo *out;
    s32 i;
    u32 b;

    if (D_00292D60[port].state == 0) {
        return 0;
    }
    func_001A5C20(port);
    raw = &D_00292D40[port];
    if (raw->status == 0xFF) {
        out = &D_00292D80[port];
        out->buttons = 0;
        out->right.x = 0;
        out->right.y = 0;
        out->left.x = 0;
        out->left.y = 0;
        return 1;
    }
    out = &D_00292D80[port];
    if (out->type != 0 && !(out->type & 1)) {
        out->type = 0x8000;
    }
    raw->buttons = ~raw->buttons;
    switch (out->type) {
    case 0x20:
    case 0x10:
    case 0x08:
    case 0x04:
    case 0x02:
    case 0x01:
        b = 0;
        for (i = 0; i < 16; i++) {
            if (raw->buttons & D_00292C80[port][i].mask) {
                b |= D_00223790[i];
            }
        }
        out->buttons = b;
        if (out->flags & 0x20) {
            out->right.x = func_001A5550(raw->stick[2]);
            out->right.y = func_001A5550(raw->stick[3]);
        }
        if (out->flags & 0x40) {
            out->left.x = func_001A5550(raw->stick[0]);
            out->left.y = func_001A5550(raw->stick[1]);
        }
        D_00292D20[port] = *raw;
        D_00292D20[port].buttons = ~D_00292D20[port].buttons;
        break;
    default:
        out->buttons = 0;
        out->right.x = 0;
        out->right.y = 0;
        out->left.x = 0;
        out->left.y = 0;
        break;
    }
    return 1;
}
#endif
