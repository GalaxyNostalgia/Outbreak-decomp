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
