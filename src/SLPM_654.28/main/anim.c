#include "common.h"
#include "model.h"

/* The skeletal animation library: playing motions (memory handles) on a model's bones, each bone
   in a layer. */


/* A motion (the memory block a handle gives). */
typedef struct {
    u16 flags;                  /* 0x0: 0x8000 has unkC */
    u8 unk2[6];
    f32 length;                 /* 0x8 */
    f32 unkC;                   /* 0xC */
    s32 tracks;                 /* 0x10: offset of its node tracks */
} AnimMotion;

extern AnimMotion *D_0023BA14;  /* the motion being set */
extern s32 D_00239F00;          /* its handle */

AnimMotion *func_0018D720(s32 handle);
void func_0018DE30(void *p);

/* Makes motion the current one. */
void func_00191E20(s32 motion) {
    D_0023BA14 = func_0018D720(motion);
    func_0018DE30(D_0023BA14);
}

struct AnimTrack;
s32 func_001927B0(struct AnimTrack *t, f32 *bind, f32 time, f32 *tmp, f32 *m, u8 *x);

/* How far bone b's own motion moves it from time t0 to time t1 (its matrices' translations). */
s32 func_00191F90(f32 t0, f32 t1, f32 *out, FlBone *b) {
    f32 m0[16];
    f32 m1[16];
    f32 tmp[12];
    u32 *track;

    func_00191E20(b->motion);
    track = (u32 *)(b->track + (u32)D_0023BA14);
    func_001927B0((struct AnimTrack *)track, b->bind[0], t0, tmp, m0, b->unk15C);
    func_001927B0((struct AnimTrack *)track, b->bind[0], t1, tmp, m1, b->unk15C);
    out[0] = m1[12] - m0[12];
    out[1] = m1[13] - m0[13];
    out[2] = m1[14] - m0[14];
    return 1;
}

FlBone *func_001926D0(FlBone *n, u16 id);
void func_001912D0(f32 *out, const f32 *a, const f32 *b, f32 s, f32 u);
s32 func_001920E0(FlBone *n, FlBone *m, f32 a, f32 b, u16 id);

/* Blends the pose of layer id of tree src into tree dst's: a parts dst's, b parts src's. */
s32 func_00192060(FlBone *dst, FlBone *src, u16 id, f32 a, f32 b) {
    func_001920E0(func_001926D0(dst, id), func_001926D0(src, id), a, b, id);
    return 1;
}

/* n = n * a + m * b over the nodes of layer id from n on (and m's matching ones): their scales
   and local matrices. */
s32 func_001920E0(FlBone *n, FlBone *m, f32 a, f32 b, u16 id) {
    FlBone *root = n;

top:
    if (n->layer == id) {
        n->cur[0][0] = n->cur[0][0] * a + m->cur[0][0] * b;
        n->cur[0][1] = n->cur[0][1] * a + m->cur[0][1] * b;
        n->cur[0][2] = n->cur[0][2] * a + m->cur[0][2] * b;
        func_001912D0((f32 *)((u8 *)n + 0x40), (f32 *)((u8 *)n + 0x40), (f32 *)((u8 *)m + 0x40), a, b);
        if (n->child != NULL) {
            m = m->child;
            n = n->child;
            goto top;
        }
    }
    if (n->next != NULL && n != root) {
        m = m->next;
        n = n->next;
        goto top;
    }
    while (n != root) {
        while (n->next != NULL) {
            n = n->next;
            m = m->next;
            if (n->layer == id) {
                goto top;
            }
        }
        n = n->parent;
        m = m->parent;
    }
    return 1;
}

/* Takes the motions off all of root's tree. */
s32 func_001924A0(FlBone *n) {
    FlBone *root = n;

    for (;;) {
        n->motion = 0;
        n->track = 0;
        if (n->child != NULL) {
            n = n->child;
            continue;
        }
        if (n->next != NULL) {
            n = n->next;
            continue;
        }
        while (n != root) {
            if (n->next != NULL) {
                n = n->next;
                goto found;
            }
            n = n->parent;
        }
        return 1;
    found:;
    }
}

/* The first node in layer id under root (depth first), or NULL. */
FlBone *func_001926D0(FlBone *n, u16 id);

/* Clears layer id of root's tree. */

s32 func_00192540(FlBone *n, u16 id);

s32 func_00192500(FlBone *root, u16 id) {
    FlBone *n = func_001926D0(root, id);

    if (n != NULL) {
        func_00192540(n, id);
    }
    return 1;
}

s32 func_00192640(FlBone *n, u32 *data, u16 id);

/* Takes the motion off the nodes of layer id from root on (descending only into those). */
s32 func_00192540(FlBone *n, u16 id) {
    FlBone *root = n;

top:
    if (n->layer == id) {
        n->motion = 0;
        n->track = 0;
        if (n->child != NULL) {
            n = n->child;
            goto top;
        }
    }
    if (n->next != NULL && n != root) {
        n = n->next;
        goto top;
    }
    while (n != root) {
        if (n->next != NULL) {
            n = n->next;
            if (n->layer == id) {
                goto top;
            }
        }
        n = n->parent;
    }
    return 1;
}

/* Puts motion on root's nodes in layer id. */
s32 func_001925C0(FlBone *root, s32 motion, u16 id) {
    u32 *data;
    FlBone *n;
    AnimMotion *m;

    D_0023BA14 = m = func_0018D720(motion);
    data = (u32 *)(m->tracks + (u32)D_0023BA14);
    D_00239F00 = motion;
    n = func_001926D0(root, id);
    if (n != NULL) {
        func_00192640(n, data, id);
    }
    return 1;
}

/* Gives the nodes of layer id from root on (descending only into those) the current motion and
   its tracks, in order. */
s32 func_00192640(FlBone *n, u32 *data, u16 id) {
    FlBone *root = n;

top:
    if (n->layer == id) {
        n->motion = D_00239F00;
        n->track = *data++;
        if (n->child != NULL) {
            n = n->child;
            goto top;
        }
    }
    if (n->next != NULL && n != root) {
        n = n->next;
        goto top;
    }
    while (n != root) {
        while (n->next != NULL) {
            n = n->next;
            if (n->layer == id) {
                goto top;
            }
        }
        n = n->parent;
    }
    return 1;
}

FlBone *func_001926D0(FlBone *n, u16 id) {
    FlBone *root = n;

    for (;;) {
        if (n->layer == id) {
            return n;
        }
        if (n->child != NULL) {
            n = n->child;
            continue;
        }
        if (n->next != NULL) {
            n = n->next;
            continue;
        }
        while (n != root) {
            if (n->next != NULL) {
                n = n->next;
                goto found;
            }
            n = n->parent;
        }
        return NULL;
    found:;
    }
}

/* A motion's length in frames. */
f32 func_00192740(s32 motion) {
    return func_0018D720(motion)->length;
}

/* A motion's unkC (and 1), or 0 when it has none. */
s8 func_00192760(s32 motion, f32 *out) {
    AnimMotion *m = func_0018D720(motion);

    if (m->flags & 0x8000) {
        *out = m->unkC;
        return 1;
    }
    *out = 0.0f;
    return 0;
}

s32 func_00193620(void *track, f32 *bind, f32 *cur, f32 frame, void *state);
s32 func_00193340(f32 *m, f32 *v);
s32 func_00192AF0(f32 frame, FlBone *b, u16 id);

/* Poses root's layer id at frame. */
s32 func_00192AA0(FlBone *root, u16 id, f32 frame) {
    FlBone *b = func_001926D0(root, id);

    func_00192AF0(frame, b, id);
    return 1;
}

/* Poses b's tree (the bones of layer id, from their motions) at frame. */
s32 func_00192AF0(f32 frame, FlBone *b, u16 id) {
    s32 i;

    func_00191E20(b->motion);
    if (b->layer == id) {
        for (i = 0; i < 10; i++) {
            (&b->cur[0][0])[i] = (&b->bind[0][0])[i];
        }
        if (b->motion != 0) {
            func_00193620((void *)(b->track + (u32)D_0023BA14), &b->bind[0][0], &b->cur[0][0], frame, b->unk15C);
        }
        func_00193340((f32 *)b->local, &b->cur[0][0]);
        if (b->child != NULL) {
            func_00192AF0(frame, b->child, id);
        }
    }
    if (b->next != NULL) {
        func_00192AF0(frame, b->next, id);
    }
    return 1;
}

s32 func_00192BD0(FlBone *root, u16 id, f32 frame) {
    return func_00192AA0(root, id, frame);
}

/* A node's track in a motion: its channels' keys (8 bytes each, byte 1 the channel). */
typedef struct AnimTrack {
    u16 flags;          /* 0xF000: 0x1000 channels as stored, 0x2000 packed (angles in 0x10000
                           a turn, the rest in 16ths) */
    s16 n;              /* 0x2: channels */
    s32 keys;           /* 0x4: offset from the motion */
} AnimTrack;

f32 func_0018DE40(f32 time, u8 *chan, u16 *last);
void func_0018F830(f32 x, f32 y, f32 z, f32 *m);
void func_001903E0(f32 rx, f32 ry, f32 rz, f32 *m);
extern f32 D_00239D34;          /* 2 pi / 0x10000 */

/* The matrix m of a node at time: v gets its scale, rotation and translation (the bind pose's,
   bind, with the channels track animates); last keeps each channel's last key. */
s32 func_001927B0(AnimTrack *t, f32 *bind, f32 time, f32 *v, f32 *m, u8 *last) {
    s32 i;
    u8 *c;
    u8 k;
    u16 k2;
    s32 j;
    f32 *p;
    u8 *c2;

    for (i = 0; i < 10; i++) {
        v[i] = bind[i];
    }
    if (t != NULL) {
        switch (t->flags & 0xF000) {
        case 0x1000:
            c = (u8 *)(t->keys + (u32)D_0023BA14);
            for (i = 0; i < t->n; i++, c += 8) {
                k = c[1];
                v[k] = func_0018DE40(time, c, (u16 *)last + k);
            }
            break;
        case 0x2000:
            c2 = (u8 *)(t->keys + (u32)D_0023BA14);
            for (j = 0; j < t->n; j++, c2 += 8) {
                k2 = c2[1];
                p = &v[k2];
                *p = func_0018DE40(time, c2, (u16 *)last + k2);
                switch (k2) {
                case 3:
                case 4:
                case 5:
                    *p = *p * D_00239D34;
                    break;
                default:
                    *p = *p / 16.0f;
                    break;
                }
            }
            break;
        }
    }
    func_0018F830(v[0], v[1], v[2], m);
    func_001903E0(v[3], v[4], v[5], m);
    m[12] = v[6];
    m[13] = v[7];
    m[14] = v[8];
    return 1;
}
