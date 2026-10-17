#include "common.h"

/* Motion channels: keyframes (float or 16-bit, linear or with tangents) evaluated at a frame. A
   channel remembers the key it was last at (state) to find the next one fast. */

typedef struct {
    u8 kind;            /* 0x0: 0x2X float keys, 0x1X 16-bit; X: 1 linear, 2 tangents, 3 mixed */
    u8 value;           /* 0x1 */
    u16 nkeys;          /* 0x2 */
    s32 keys;           /* 0x4: offset in the motion's data */
} MotionChannel;

/* The keys (the frame is what the lookups need; the evaluators read the rest by index). */
typedef struct { f32 value, frame; } KeyF1;                         /* 0x21 */
typedef struct { f32 value, frame, in, out; } KeyF2;                /* 0x22 */
typedef struct { s32 type; f32 value, frame, in, out; } KeyF3;      /* 0x23 */
typedef struct { s16 value, frame; } KeyS1;                         /* 0x11 */
typedef struct { s16 value, frame, in, out; } KeyS2;                /* 0x12 */
typedef struct { s32 type; s16 value, frame, in, out; } KeyS3;      /* 0x13 */

extern u8 *D_0023BA10;          /* the current motion's data */

s16 func_0018E390(MotionChannel *c, f32 **a, f32 **b, s16 at, f32 frame);
s16 func_0018E4E0(MotionChannel *c, f32 **a, f32 **b, s16 at, f32 frame);
s16 func_0018E630(MotionChannel *c, f32 **a, f32 **b, s16 at, f32 frame);
s16 func_0018E790(MotionChannel *c, s16 **a, s16 **b, s16 at, f32 frame);
s16 func_0018E920(MotionChannel *c, s16 **a, s16 **b, s16 at, f32 frame);
s16 func_0018EAB0(MotionChannel *c, s16 **a, s16 **b, s16 at, f32 frame);
f32 func_0018EC50(f32 t, f32 t0, f32 v0, f32 t1, f32 v1);
f32 func_0018ECD0(f32 t, f32 t0, f32 v0, f32 d0, f32 t1, f32 v1, f32 d1);

f32 func_0018DF00(f32 frame, MotionChannel *c, s16 *state);
f32 func_0018DF80(f32 frame, MotionChannel *c, s16 *state);
f32 func_0018E010(f32 frame, MotionChannel *c, s16 *state);
f32 func_0018E0E0(f32 frame, MotionChannel *c, s16 *state);
f32 func_0018E190(f32 frame, MotionChannel *c, s16 *state);
f32 func_0018E250(f32 frame, MotionChannel *c, s16 *state);

/* The channel's value at frame. */
f32 func_0018DE40(f32 frame, MotionChannel *c, s16 *state) {
    f32 v;

    switch (c->kind) {
    case 0x21:
        v = func_0018DF00(frame, c, state);
        break;
    case 0x22:
        v = func_0018DF80(frame, c, state);
        break;
    case 0x23:
        v = func_0018E010(frame, c, state);
        break;
    case 0x11:
        v = func_0018E0E0(frame, c, state);
        break;
    case 0x12:
        v = func_0018E190(frame, c, state);
        break;
    case 0x13:
        v = func_0018E250(frame, c, state);
        break;
    }
    return v;
}

/* Float keys (frame, value), linear. */
f32 func_0018DF00(f32 frame, MotionChannel *c, s16 *state) {
    f32 *a = NULL;
    f32 *b = NULL;

    *state = func_0018E390(c, &a, &b, *state, frame);
    if (b == NULL) {
        return a[0];
    }
    return func_0018EC50(frame, a[0], a[1], b[0], b[1]);
}

/* Float keys (frame, value, -, out tangent / in tangent), with tangents. */
f32 func_0018DF80(f32 frame, MotionChannel *c, s16 *state) {
    f32 *a = NULL;
    f32 *b = NULL;

    *state = func_0018E4E0(c, &a, &b, *state, frame);
    if (b == NULL) {
        return a[0];
    }
    return func_0018ECD0(frame, a[0], a[1], a[3], b[0], b[1], b[2]);
}

/* Float keys, each linear (0x10000) or with tangents (0x20000). */
f32 func_0018E010(f32 frame, MotionChannel *c, s16 *state) {
    f32 *a = NULL;
    f32 *b = NULL;

    *state = func_0018E630(c, &a, &b, *state, frame);
    if (b == NULL) {
        return a[1];
    }
    switch (*(s32 *)a) {
    case 0x10000:
        return func_0018EC50(frame, a[1], a[2], b[1], b[2]);
    case 0x20000:
        return func_0018ECD0(frame, a[1], a[2], a[4], b[1], b[2], b[3]);
    }
    return 0.0f;
}

/* 16-bit keys, linear. */
f32 func_0018E0E0(f32 frame, MotionChannel *c, s16 *state) {
    s16 *a = NULL;
    s16 *b = NULL;

    *state = func_0018E790(c, &a, &b, *state, frame);
    if (b == NULL) {
        return a[0];
    }
    return func_0018EC50(frame, a[0], a[1], b[0], b[1]);
}

/* 16-bit keys, with tangents. */
f32 func_0018E190(f32 frame, MotionChannel *c, s16 *state) {
    s16 *a = NULL;
    s16 *b = NULL;

    *state = func_0018E920(c, &a, &b, *state, frame);
    if (b == NULL) {
        return a[0];
    }
    return func_0018ECD0(frame, a[0], a[1], a[3], b[0], b[1], b[2]);
}

/* 16-bit keys, each linear (0x10000) or with tangents (0x20000). */
f32 func_0018E250(f32 frame, MotionChannel *c, s16 *state) {
    s16 *a = NULL;
    s16 *b = NULL;

    *state = func_0018EAB0(c, &a, &b, *state, frame);
    if (b == NULL) {
        return a[2];
    }
    switch (*(s32 *)a) {
    case 0x10000:
        return func_0018EC50(frame, a[2], a[3], b[2], b[3]);
    case 0x20000:
        return func_0018ECD0(frame, a[2], a[3], a[5], b[2], b[3], b[4]);
    }
    return 0.0f;
}

/* Finds the keys around frame: a the one at or before it, b the next (NULL when frame is on a or
   outside the keys). Starts from key at (where the last lookup ended) and returns where it is. */
s16 func_0018E390(MotionChannel *c, f32 **a, f32 **b, s16 at, f32 frame) {
    KeyF1 *k = (KeyF1 *)(c->keys + (u32)D_0023BA10);

    *a = NULL;
    *b = NULL;
    if (frame <= k->frame || c->nkeys == 1) {
        *a = (f32 *)k;
        *b = NULL;
        return 0;
    }
    if (!(frame < k[c->nkeys - 1].frame)) {
        *a = (f32 *)&k[c->nkeys - 1];
        *b = NULL;
        return c->nkeys - 1;
    }
    if (at < 0) {
        at = 0;
    } else if (at >= c->nkeys) {
        at = c->nkeys - 1;
    }
    k += at;
    for (;;) {
        if (k->frame == frame) {
            *a = (f32 *)k;
            *b = NULL;
            return at;
        }
        if (k->frame < frame && frame < k[1].frame) {
            *a = (f32 *)k;
            *b = (f32 *)(k + 1);
            return at;
        }
        if (frame < k->frame) {
            at--;
            k--;
        } else {
            at++;
            k++;
        }
    }
}

s16 func_0018E4E0(MotionChannel *c, f32 **a, f32 **b, s16 at, f32 frame) {
    KeyF2 *k = (KeyF2 *)(c->keys + (u32)D_0023BA10);

    *a = NULL;
    *b = NULL;
    if (frame <= k->frame || c->nkeys == 1) {
        *a = (f32 *)k;
        *b = NULL;
        return 0;
    }
    if (!(frame < k[c->nkeys - 1].frame)) {
        *a = (f32 *)&k[c->nkeys - 1];
        *b = NULL;
        return c->nkeys - 1;
    }
    if (at < 0) {
        at = 0;
    } else if (at >= c->nkeys) {
        at = c->nkeys - 1;
    }
    k += at;
    for (;;) {
        if (k->frame == frame) {
            *a = (f32 *)k;
            *b = NULL;
            return at;
        }
        if (k->frame < frame && frame < k[1].frame) {
            *a = (f32 *)k;
            *b = (f32 *)(k + 1);
            return at;
        }
        if (frame < k->frame) {
            at--;
            k--;
        } else {
            at++;
            k++;
        }
    }
}

s16 func_0018E630(MotionChannel *c, f32 **a, f32 **b, s16 at, f32 frame) {
    KeyF3 *k = (KeyF3 *)(c->keys + (u32)D_0023BA10);

    *a = NULL;
    *b = NULL;
    if (frame <= k->frame || c->nkeys == 1) {
        *a = (f32 *)k;
        *b = NULL;
        return 0;
    }
    if (!(frame < k[c->nkeys - 1].frame)) {
        *a = (f32 *)&k[c->nkeys - 1];
        *b = NULL;
        return c->nkeys - 1;
    }
    if (at < 0) {
        at = 0;
    } else if (at >= c->nkeys) {
        at = c->nkeys - 1;
    }
    k += at;
    for (;;) {
        if (k->frame == frame) {
            *a = (f32 *)k;
            *b = NULL;
            return at;
        }
        if (k->frame < frame && frame < k[1].frame) {
            *a = (f32 *)k;
            *b = (f32 *)(k + 1);
            return at;
        }
        if (frame < k->frame) {
            at--;
            k--;
        } else {
            at++;
            k++;
        }
    }
}

s16 func_0018E790(MotionChannel *c, s16 **a, s16 **b, s16 at, f32 frame) {
    KeyS1 *k = (KeyS1 *)(c->keys + (u32)D_0023BA10);

    *a = NULL;
    *b = NULL;
    if (frame <= k->frame || c->nkeys == 1) {
        *a = (s16 *)k;
        *b = NULL;
        return 0;
    }
    if (!(frame < k[c->nkeys - 1].frame)) {
        *a = (s16 *)&k[c->nkeys - 1];
        *b = NULL;
        return c->nkeys - 1;
    }
    if (at < 0) {
        at = 0;
    } else if (at >= c->nkeys) {
        at = c->nkeys - 1;
    }
    k += at;
    for (;;) {
        if (k->frame == frame) {
            *a = (s16 *)k;
            *b = NULL;
            return at;
        }
        if (k->frame < frame && frame < k[1].frame) {
            *a = (s16 *)k;
            *b = (s16 *)(k + 1);
            return at;
        }
        if (frame < k->frame) {
            at--;
            k--;
        } else {
            at++;
            k++;
        }
    }
}

s16 func_0018E920(MotionChannel *c, s16 **a, s16 **b, s16 at, f32 frame) {
    KeyS2 *k = (KeyS2 *)(c->keys + (u32)D_0023BA10);

    *a = NULL;
    *b = NULL;
    if (frame <= k->frame || c->nkeys == 1) {
        *a = (s16 *)k;
        *b = NULL;
        return 0;
    }
    if (!(frame < k[c->nkeys - 1].frame)) {
        *a = (s16 *)&k[c->nkeys - 1];
        *b = NULL;
        return c->nkeys - 1;
    }
    if (at < 0) {
        at = 0;
    } else if (at >= c->nkeys) {
        at = c->nkeys - 1;
    }
    k += at;
    for (;;) {
        if (k->frame == frame) {
            *a = (s16 *)k;
            *b = NULL;
            return at;
        }
        if (k->frame < frame && frame < k[1].frame) {
            *a = (s16 *)k;
            *b = (s16 *)(k + 1);
            return at;
        }
        if (frame < k->frame) {
            at--;
            k--;
        } else {
            at++;
            k++;
        }
    }
}

s16 func_0018EAB0(MotionChannel *c, s16 **a, s16 **b, s16 at, f32 frame) {
    KeyS3 *k = (KeyS3 *)(c->keys + (u32)D_0023BA10);

    *a = NULL;
    *b = NULL;
    if (frame <= k->frame || c->nkeys == 1) {
        *a = (s16 *)k;
        *b = NULL;
        return 0;
    }
    if (!(frame < k[c->nkeys - 1].frame)) {
        *a = (s16 *)&k[c->nkeys - 1];
        *b = NULL;
        return c->nkeys - 1;
    }
    if (at < 0) {
        at = 0;
    } else if (at >= c->nkeys) {
        at = c->nkeys - 1;
    }
    k += at;
    for (;;) {
        if (k->frame == frame) {
            *a = (s16 *)k;
            *b = NULL;
            return at;
        }
        if (k->frame < frame && frame < k[1].frame) {
            *a = (s16 *)k;
            *b = (s16 *)(k + 1);
            return at;
        }
        if (frame < k->frame) {
            at--;
            k--;
        } else {
            at++;
            k++;
        }
    }
}
