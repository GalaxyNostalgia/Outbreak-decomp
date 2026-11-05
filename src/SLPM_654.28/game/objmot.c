#include "common.h"
#include "object.h"
#include "options.h"
#include "game.h"

/* GAME.BIN: starting a motion on one layer of a character (players, NPCs, enemies alike). */

/* A character's motion layer (0x20 bytes from 0x1E4). */
typedef struct {
    f32 start;          /* 0x00: the frame it started on */
    f32 unk4;           /* 0x04: the motion's length */
    f32 unk8;           /* 0x08 */
    s8 unkC;            /* 0x0C */
    s8 flags;           /* 0x0D: 0x8 blending in, 0x10 (the blend time was negative) */
    s16 motion;         /* 0x0E */
    f32 blendStep;      /* 0x10 */
    f32 blendPos;       /* 0x14: 0 to 1 */
    u16 blendFrames;    /* 0x18 */
    u8 unk1A[6];
} ObjLayer;

/* A loaded motion set: the motion ids per (motion, layer). */
typedef struct {
    u8 first;           /* 0x0: its first layer */
    u8 nlayers;         /* 0x1 */
    u8 kind;            /* 0x2: 0 and 2 by frame/100 and frame%100; 1 through an index */
    u8 pad3;
    u16 ids[1];         /* 0x4 */
} MotionSet;

typedef struct {
    f32 x, y, z;
} Vec3f;

/* The model's root bone: 0x70 its motion this frame. */
typedef struct {
    u8 unk0[0x70];
    Vec3f delta;        /* 0x70 */
} AnimRoot;

/* What a character's model animates with: the animation and its root bone. */
typedef struct {
    void *anim;
    AnimRoot *root;     /* 0x4 */
} ObjAnim;

f32 func_00191A70(f32 x);
void func_00191F90(f32 from, f32 to, f32 *out, AnimRoot *root);
void func_00192500(void *anim, u16 layer);
void func_001925C0(void *anim, s32 id, u16 layer);
void func_00192BD0(void *anim, u16 layer, f32 frame);
f32 func_00192740(s32 id);
s8 func_00192760(s32 id, f32 *out);
void func_005A3A70(DrawObjFull *o, u16 layer);
void func_0055C3D0(DrawObjFull *o, ObjAnim *a);
u8 func_0055C190(DrawObjFull *o, u16 layer);
void *memset(void *p, s32 c, u32 n);

/* The root positions (0x4E4, 0x4F0, 0x4FC, 0x508). */
#define R(o) ((f32 *)(o)->unk4E4)

/* Starts motion (in ms) on layer of o from frame start, blending over blend frames. 1 when
   there's no such motion (layer 2's is then cleared). */
s32 func_0055B6D0(DrawObjFull *o, MotionSet *ms, s32 motion, u16 layer, f32 start, f32 blend);
#ifdef NON_MATCHING   /* close: register choices and two operand orders */
s32 func_0055B6D0(DrawObjFull *o, MotionSet *ms, s32 motion, u16 layer, f32 start, f32 blend) {
    ObjLayer *l = &((ObjLayer *)&o->unk1E4)[layer];
    ObjAnim *a = *(ObjAnim **)o->model;
    f32 speed;
    f32 scale;
    u32 id;

    if (motion & 0x4000) {
        return 1;
    }
    if (motion >= 2000) {
        return 1;
    }
    switch (ms->kind) {
    case 0:
    case 2:
        id = ms->ids[(layer - ms->first) + (motion / 100 * 200 + motion % 100 * ms->nlayers)];
        break;
    case 1:
        id = ms->ids[ms->ids[0x7D0 + (motion & 0x7FFF)] + layer];
        if (id == 0 && layer == 2) {
            func_00192500(a->anim, layer);
            memset(l, 0, sizeof(ObjLayer));
            return 1;
        }
        break;
    }
    if (id == 0) {
        return 1;
    }
    l->motion = motion;
    l->start = start;
    if (D_00306590.unkC) {
        speed = 2.0f;
    } else {
        speed = 1.0f;
    }
    blend /= speed;
    l->blendFrames = blend;
    l->flags &= ~8;
    l->flags &= ~0x10;
    if (blend != 0.0f) {
        if (blend < 0.0f) {
            l->flags |= 0x10;
            blend = func_00191A70(blend);
            l->blendFrames = blend;
        }
        l->flags |= 8;
        l->blendStep = 1.0f / (1.0f + blend);
        l->blendPos = 0.0f;
        func_005A3A70(o, layer);
        if (layer == 0) {
            *(Vec3f *)&R(o)[6] = *(Vec3f *)&R(o)[0];
        }
    }
    id = (u16)id;
    func_001925C0(a->anim, id, layer);
    func_00192BD0(a->anim, layer, start);
    if (layer == 0) {
        if (start < speed) {
            R(o)[9] = 0.0f;
            R(o)[10] = 0.0f;
            R(o)[11] = 0.0f;
            func_0055C3D0(o, a);
        } else {
            if (o->type != 1 && o->type != 0) {
                scale = 1.0f;
            } else {
                scale = o->unkBA8;
            }
            func_00191F90(start - speed * scale, start, R(o), a->root);
            *(Vec3f *)&R(o)[9] = a->root->delta;
            a->root->delta.x = 0.0f;
            a->root->delta.y = 0.0f;
            a->root->delta.z = 0.0f;
        }
        *(Vec3f *)&R(o)[3] = *(Vec3f *)&R(o)[0];
    }
    func_0055C190(o, layer);
    l->unk4 = func_00192740(id);
    l->unkC = func_00192760(id, &l->unk8);
    if (l->unk8 < 0.0f) {
        l->unk8 = 0.0f;
    }
    if ((s32)l->unk4 % 2 != 0) {
        l->unk4 -= 1.0f;
    }
    if (l->unkC % 2 != 0) {
        l->unkC -= 1.0f;
    }
    return 0;
}
#endif

f32 func_001A42E0(f32 t, f32 x1, f32 y1, f32 x2, f32 y2, f32 x3, f32 y3);
void func_00192060(void *anim, void *other, u16 layer, f32 w, f32 inv);

/* Advances layer's blend: mixes the motion in (and, on layer 0, the root position). Returns the
   blend frames left (0 when not blending). */
u8 func_0055C190(DrawObjFull *o, u16 layer);
#ifdef NON_MATCHING   /* where the layer pointer is formed; the else branch's multiply order */
u8 func_0055C190(DrawObjFull *o, u16 layer) {
    ObjLayer *l;
    void **m;
    f32 w;
    f32 inv;

    if (!(((ObjLayer *)&o->unk1E4)[layer].flags & 8)) {
        return 0;
    }
    m = (void **)o->model;
    if (((ObjLayer *)&o->unk1E4)[layer].blendFrames == 0) {
        return 0;
    }
    l = &((ObjLayer *)&o->unk1E4)[layer];
    l->blendPos += l->blendStep;
    w = func_001A42E0(l->blendPos, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);
    inv = 1.0 - w;
    if (*(void **)m[1] != NULL) {
        func_00192060(((ObjAnim *)m[0])->anim, *(void **)m[1], layer, w, inv);
    }
    if (!(l->flags & 2) && layer == 0) {
        if (l->flags & 0x10) {
            R(o)[0] = R(o)[3] * l->blendPos + R(o)[6] * (1.0f - l->blendPos);
            R(o)[1] = R(o)[4] * l->blendPos + R(o)[7] * (1.0f - l->blendPos);
            R(o)[2] = R(o)[5] * l->blendPos + R(o)[8] * (1.0f - l->blendPos);
        } else {
            R(o)[0] = R(o)[6] * (1.0f - l->blendPos) + R(o)[0] * l->blendPos;
            R(o)[1] = R(o)[7] * (1.0f - l->blendPos) + R(o)[1] * l->blendPos;
            R(o)[2] = R(o)[8] * (1.0f - l->blendPos) + R(o)[2] * l->blendPos;
        }
    }
    l->blendFrames--;
    if (l->blendFrames == 0) {
        l->flags &= ~8;
        l->flags &= ~0x10;
        l->flags &= ~2;
    }
    return l->blendFrames;
}
#endif

/* Takes the root's motion since the last frame (from where it was, 0x508) into o's 0x4E4, and
   clears it. */
void func_0055C3D0(DrawObjFull *o, ObjAnim *a) {
    R(o)[0] = a->root->delta.x - R(o)[9];
    R(o)[1] = a->root->delta.y - R(o)[10];
    R(o)[2] = a->root->delta.z - R(o)[11];
    *(Vec3f *)&R(o)[9] = a->root->delta;
    a->root->delta.x = 0.0f;
    a->root->delta.y = 0.0f;
    a->root->delta.z = 0.0f;
}

/* Marks every motion layer of o (both copies, 0x1F0 and 0x370) to start over. */
void func_0055C4E0(DrawObjFull *o) {
    u32 i;

    for (i = 0; i < (u8)o->unk524; i++) {
        ((u8 *)o)[0x1F0 + i * 0x20] = 1;
        ((u8 *)o)[0x370 + i * 0x20] = 1;
    }
}

/* Sets bits 1 and 7 of the first motion layer's flags (0x1F1). */
void func_0055C520(DrawObjFull *o) {
    ((s8 *)o)[0x1F1] |= 0x82;
}

void func_005661C0(DrawObjFull *o, f32 speed);
void func_0056A440(DrawObjFull *o, f32 speed);
void func_005BB880(DrawObjFull *o, f32 speed);
void func_0058AA70(DrawObjFull *o, f32 speed);
void func_0058AF00(DrawObjFull *o, f32 speed);
void func_0058B200(DrawObjFull *o, f32 speed);
void func_0058B550(DrawObjFull *o, f32 speed);
void func_005BBC40(DrawObjFull *o, f32 speed);
void func_0058BA20(DrawObjFull *o, f32 speed);
void func_0058BE00(DrawObjFull *o, f32 speed);
void func_00570310(DrawObjFull *o, f32 speed);
void func_005655F0(DrawObjFull *o, f32 speed);

/* A character's own step for its current motion's set (another character's: func_005655F0). */
void func_005654D0(DrawObjFull *o, f32 speed) {
    switch (o->unkCE0) {
    case 0:
        break;
    case 0xFF:
        func_005655F0(o, speed);
        return;
    }
    switch (o->unkCE1) {
    case 0:
        func_005661C0(o, speed);
        break;
    case 1:
        func_0056A440(o, speed);
        break;
    case 2:
        func_005BB880(o, speed);
        break;
    case 3:
        func_0058AA70(o, speed);
        break;
    case 4:
        func_0058AF00(o, speed);
        break;
    case 5:
        func_0058B200(o, speed);
        break;
    case 6:
        func_0058B550(o, speed);
        break;
    case 7:
        func_005BBC40(o, speed);
        break;
    case 8:
        func_0058BA20(o, speed);
        break;
    case 9:
        func_0058BE00(o, speed);
        break;
    case 10:
        func_00570310(o, speed);
        break;
    }
}
void func_00578B30(DrawObjFull *o, f32 speed);

/* Before the motions advance: characters' and things' own step. */
void func_00543410(DrawObjFull *o, f32 speed) {
    if (o->type == 0 || o->type == 1) {
        func_005654D0(o, speed);
    } else if (o->type == 2) {
        func_00578B30(o, speed);
    }
}
/* Advances layer l by speed (backwards when negative). 0 within the motion; 1 when it reached
   an end (0x40), 2 when it wrapped round (looping layers; 0x20 too), 3 for an empty motion. */
s8 func_0055BC80(ObjLayer *l, f32 speed);
#ifdef NON_MATCHING   /* close: branch layout at the two ends and one add's operand order */
s8 func_0055BC80(ObjLayer *l, f32 speed) {
    f32 over;

    l->flags &= ~0x60;
    if (l->flags & 0x10) {
        return 0;
    }
    if (l->unk4 == 0.0f) {
        l->flags |= 0x40;
        if (l->unkC) {
            l->flags |= 0x20;
        }
        return 3;
    }
    if (!(speed < 0.0f)) {
        l->start += speed;
        over = l->start - l->unk4;
        if (!(over <= 0.0f)) {
            if (l->unkC) {
                l->start = l->unk8 + over;
                if (!(l->start <= l->unk4)) {
                    l->start = l->unk4;
                }
                l->flags |= 0x20;
                return 2;
            }
            l->start = l->unk4;
            l->flags |= 0x40;
            return 1;
        }
        if (over == 0.0f) {
            l->flags |= 0x40;
            return 1;
        }
        if (l->start + speed > l->unk4) {
            l->flags |= 0x40;
            return 1;
        }
        return 0;
    }
    l->start += speed;
    if (l->start == 0.0f) {
        l->start = l->unk4;
        l->flags |= 0x40;
        if (l->unkC) {
            l->flags |= 0x20;
        }
        return 1;
    }
    if (l->start < 0.0f) {
        l->start = l->start + l->unk4;
        l->flags |= 0x40;
        if (l->unkC) {
            l->flags |= 0x20;
        }
        return 2;
    }
    return 0;
}
#endif

/* Advances o's motion layer by speed (twice as fast at 30 fps) and poses the model; layer 0
   also moves the root (0x4E4) by the motion's root travel. Returns func_0055BC80's result. */
s32 func_0055BE60(DrawObjFull *o, u16 layer, f32 speed);
#ifdef NON_MATCHING   /* close: two of the root adds load their operands in the other order */
s32 func_0055BE60(DrawObjFull *o, u16 layer, f32 speed) {
    ObjAnim **m;
    ObjLayer *l;
    f32 prev;
    Vec3f tmp;
    s32 r;

    l = &((struct { u8 pad[0x1E4]; ObjLayer l[4]; } *)o)->l[layer];
    m = (ObjAnim **)o->model;

    if (D_00306590.unkC) {
        speed *= 2.0f;
    }
    prev = l->start;
    r = func_0055BC80(l, speed);
    if (r >= 2 && layer == 0) {
        if (l->start != 0.0f) {
            func_00192BD0((*m)->anim, layer, l->start);
            func_00191F90(prev, l->unk4, &tmp.x, (*m)->root);
            func_00191F90(l->unk8, l->start, R(o), (*m)->root);
            R(o)[0] += tmp.x;
            R(o)[1] += tmp.y;
            R(o)[2] += tmp.z;
            *(Vec3f *)&R(o)[9] = (*m)->root->delta;
            (*m)->root->delta.x = 0.0f;
            (*m)->root->delta.y = 0.0f;
            (*m)->root->delta.z = 0.0f;
            func_0055C190(o, layer);
            return r;
        }
        R(o)[9] = 0.0f;
        R(o)[10] = 0.0f;
        R(o)[11] = 0.0f;
    }
    func_00192BD0((*m)->anim, layer, l->start);
    if (layer == 0) {
        func_0055C3D0(o, *m);
    }
    func_0055C190(o, layer);
    return r;
}
#endif

/* Advances o's motion layers by speed (layer 0 last); the result is layer 0's (-1 when it is
   held). For players, 0xCE2 tells whether the motion is past its frame 0xCE3 (0x80) and, for
   ours, whether 0xF80 has any of 0xF3F (0x40). */
s8 func_0055C030(DrawObjFull *o, f32 speed) {
    s8 i;
    s8 r;

    func_00543410(o, speed);
    for (i = 1; i < (u8)o->unk524; i++) {
        if (!(((u8 *)o)[0x1FE + i * 0x20] & 1)) {
            func_0055BE60(o, i, speed);
        }
    }
    if (((u8 *)o)[0x1FE] & 1) {
        r = -1;
    } else {
        r = func_0055BE60(o, 0, speed);
    }
    if (o->type == 0) {
        if (o->unkCE3 != 0) {
            o->unkCE2 = (FIELD(o, f32, 0x1E4) >= (f32)(u32)o->unkCE3) ? 0x80 : 0;
            if (o->kind == D_0048BF60.unk1D) {
                o->unkCE2 |= (u8)((FIELD(o, s32, 0xF80) & 0xF3F) ? 0x40 : 0);
            }
        } else {
            o->unkCE2 = 0;
        }
    }
    return r;
}

/* A motion layer's play state (0x20 bytes from 0x1E4). */
typedef struct {
    f32 frame;          /* 0x00 */
    f32 end;            /* 0x04 */
    f32 loopStart;      /* 0x08 */
    s8 loop;            /* 0x0C */
    s8 flags;           /* 0x0D: 0x10 held */
    u8 unkE[0x20 - 0xE];
} MotLayer;

/* Whether layer's motion passes frame in this step of speed (twice as fast when Options.unkC
   is 1), wrapping at the loop. */
s32 func_00543460(DrawObjFull *o, u8 layer, f32 frame, f32 speed);
#ifdef NON_MATCHING   /* delay slots of the returns, float registers */
s32 func_00543460(DrawObjFull *o, u8 layer, f32 frame, f32 speed) {
    MotLayer *l = (MotLayer *)((u8 *)o + layer * 0x20 + 0x1E4);
    f32 next;

    if (l->flags & 0x10) {
        return 0;
    }
    if (D_00306590.unkC == 1) {
        speed *= 2.0f;
    }
    if (l->loop != 0) {
        next = l->frame + speed;
        if (l->end < next) {
            if (!(frame < l->frame) && frame <= l->end) {
                return 1;
            }
            if (frame < l->loopStart) {
                return 0;
            }
            if (frame < l->loopStart + (next - l->end)) {
                return 1;
            }
            return 0;
        }
        if (!(frame < l->frame) && frame < next) {
            return 1;
        }
        return 0;
    }
    if (!(frame < l->frame) && frame < l->frame + speed) {
        return 1;
    }
    return 0;
}
#endif

/* Item motions' timed events (sounds and effects at frames), by the motion (unk212) and the
   second motion (unk1F2). */
void func_00565790(s32 frame, DrawObjFull *o, s32 se, f32 speed);
void func_005658B0(s32 frame, DrawObjFull *o, s32 n, s32 base, f32 speed);
void func_005659F0(s32 frame, DrawObjFull *o, s32 unused, s32 se, f32 speed);
void func_00565C80(s32 frame, DrawObjFull *o, s32 what, s32 b, f32 speed);
void func_00565DA0(s32 frame, DrawObjFull *o, s32 a, s32 b, s32 c, f32 speed);

void func_0058AA70(DrawObjFull *o, f32 speed) {
    switch (o->unk212) {
    case 0x65:
        func_00565790(0x22, o, 0, speed);
        func_00565790(0x48, o, 0, speed);
        func_00565790(0x6E, o, 0, speed);
        func_00565DA0(0x22, o, 6, 0x81, 0, speed);
        func_00565DA0(0x48, o, 6, 0x82, 0, speed);
        func_00565DA0(0x6E, o, 6, 0x81, 0, speed);
        break;
    case 0x66:
        func_00565790(0x1C, o, 0, speed);
        func_00565790(0x46, o, 0, speed);
        func_00565790(0x72, o, 0, speed);
        func_00565790(0xA0, o, 0, speed);
        func_00565DA0(0x1C, o, 6, 0x41, 0, speed);
        func_00565DA0(0x46, o, 6, 0x42, 0, speed);
        func_00565DA0(0x72, o, 6, 0x41, 0, speed);
        func_00565DA0(0xA0, o, 6, 0x42, 0, speed);
        break;
    case 0x67:
        func_00565790(0x1C, o, 1, speed);
        func_00565790(0x36, o, 1, speed);
        func_005658B0(0x3A, o, 2, 8, speed);
        func_00565DA0(0x1C, o, 6, 0x81, 0, speed);
        func_00565DA0(0x36, o, 6, 0x82, 0, speed);
        break;
    case 0x68:
        func_00565790(8, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        break;
    case 0x6B:
    case 0x122:
    case 0x123:
    case 0x124:
    case 0x173:
    case 0x175:
    case 0x176:
    case 0x177:
    case 0x178:
    case 0x179:
        break;
    case 0x130:
        func_005658B0(0x16, o, 2, 3, speed);
        func_00565C80(0x1E, o, 0, 0x7A, speed);
        func_00565790(0x30, o, 0, speed);
        func_00565790(0x60, o, 0, speed);
        break;
    case 0x133:
        func_00565790(0x26, o, 0, speed);
        func_00565790(0x46, o, 0, speed);
        func_005659F0(0x12, o, 0, 1, speed);
        func_005658B0(0x12, o, 2, 3, speed);
        func_00565C80(0xC, o, 2, 0, speed);
        func_00565C80(0x14, o, 3, 0, speed);
        break;
    case 0x172:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        break;
    case 0x174:
        func_00565790(0x14, o, 0, speed);
        break;
    }
    switch (o->unk1F2) {
    case 0x17A:
        func_00565790(0x12, o, 0, speed);
        func_00565790(0x20, o, 0, speed);
        break;
    case 0x17B:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x16, o, 0, speed);
        break;
    }
}

/* The same for another item's motions. */
void func_0058BE00(DrawObjFull *o, f32 speed) {
    switch (o->unk212) {
    case 0x65:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x46, o, 0, speed);
        func_00565DA0(0x1A, o, 6, 0x81, 0, speed);
        func_00565DA0(0x46, o, 6, 0x82, 0, speed);
        break;
    case 0x66:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x4E, o, 0, speed);
        func_00565790(0x7E, o, 0, speed);
        func_00565790(0xB1, o, 0, speed);
        func_00565DA0(0x1A, o, 6, 0x41, 0, speed);
        func_00565DA0(0x4E, o, 6, 0x42, 0, speed);
        func_00565DA0(0x7E, o, 6, 0x41, 0, speed);
        func_00565DA0(0xB1, o, 6, 0x42, 0, speed);
        break;
    case 0x67:
        func_00565790(0x16, o, 1, speed);
        func_00565790(0x30, o, 1, speed);
        func_005658B0(0x34, o, 2, 8, speed);
        func_00565DA0(0x16, o, 6, 0x81, 0, speed);
        func_00565DA0(0x30, o, 6, 0x82, 0, speed);
        break;
    case 0x68:
        func_00565790(8, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        break;
    case 0x6B:
    case 0x122:
    case 0x123:
    case 0x124:
        break;
    case 0x125:
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x3E, o, 0, speed);
        break;
    case 0x12C:
        func_00565790(0xA, o, 0, speed);
        func_005658B0(0xA, o, 2, 8, speed);
        break;
    }
}

/* (weapon flags, D_006E5940: 0x20 ...) */
typedef struct {
    u32 flags;
    u8 unk4[0x40];
} MotWeapon;

extern MotWeapon D_006E5940[];
void func_00565820(s32 frame, DrawObjFull *o, s32 a, f32 speed);
void func_00565A50(s32 frame, DrawObjFull *o, s32 unused, f32 speed);
void func_00565BC0(s32 frame, DrawObjFull *o, s32 a, f32 speed);
void func_00565C20(s32 frame, DrawObjFull *o, s32 a, f32 speed);
void func_00566090(s32 frame, DrawObjFull *o, s32 a, s32 unused, f32 speed);
void func_0056E7D0(DrawObjFull *o, f32 speed);

#ifdef NON_MATCHING   /* generated from the original's tables; the case order and shared tails differ */
/* Player motions' timed events (sounds and effects at frames), by the second motion (unk1F2)
   and the motion (unk212); then func_0056E7D0's. */
void func_0056A440(DrawObjFull *o, f32 speed) {
    switch (o->unk1F2) {
    case 0x17B:
        func_00565820(0x12, o, 0, speed);
        func_00565820(0x26, o, 0, speed);
        break;
    case 0x17A:
        func_00565820(0x14, o, 0, speed);
        func_00565820(0x20, o, 0, speed);
        break;
    case 0xC0:
        func_00565820(0x2E, o, 0, speed);
        break;
    case 0xBF:
        func_00565820(0x2C, o, 0, speed);
        break;
    case 0xB0:
        func_00565820(0x19, o, 0, speed);
        func_00565820(0x43, o, 0, speed);
        break;
    case 0xAF:
        func_00565820(1, o, 0, speed);
        func_00565820(0x1F, o, 0, speed);
        break;
    case 0xA7:
        func_00565820(0x16, o, 0, speed);
        func_00565820(0x3E, o, 0, speed);
        break;
    case 0xA6:
        func_00565820(0x1E, o, 0, speed);
        func_00565820(0x42, o, 0, speed);
        break;
    case 0xA5:
        func_00565820(0x22, o, 0, speed);
        break;
    case 0xA4:
        func_00565820(0x16, o, 0, speed);
        func_00565820(0x44, o, 0, speed);
        break;
    case 0xA3:
        func_00565820(0x28, o, 0, speed);
        break;
    case 0xA2:
        func_00565820(0x22, o, 0, speed);
        break;
    case 0xA1:
        func_00565820(0x28, o, 0, speed);
        break;
    case 0xA0:
        func_00565820(0x26, o, 0, speed);
        break;
    }
    switch (o->unk212) {
    case 0x193:
        func_005659F0(0xA, o, 0, 0x18, speed);
        func_00566090(0x22, o, 5, 0, speed);
        break;
    case 0x192:
        func_00565A50(0, o, 0, speed);
        if (D_006E5940[(u16)o->unkCA8].flags & 0x20) {
            func_005659F0(0xA, o, 0, 7, speed);
        }
        break;
    case 0x191:
        func_005659F0(0x10, o, 0, 0x19, speed);
        break;
    case 0x190:
        break;
    case 0x18E:
        func_005659F0(0xA, o, 0, 2, speed);
        func_00566090(0, o, 3, 0, speed);
        break;
    case 0x18D:
        func_005659F0(0x1E, o, 0, 4, speed);
        func_005659F0(0x20, o, 0, 4, speed);
        func_00566090(0, o, 2, 0, speed);
        break;
    case 0x39E:
        func_00565790(4, o, 0, speed);
        break;
    case 0x39B:
        func_005658B0(0xA, o, 2, 8, speed);
        func_005658B0(0x26, o, 2, 0xC, speed);
        break;
    case 0x39A:
        func_005658B0(0xA, o, 2, 8, speed);
        func_005658B0(0x26, o, 2, 0xC, speed);
        break;
    case 0x399:
        func_005658B0(0xE, o, 2, 8, speed);
        break;
    case 0xBA:
        func_00565790(0xBC, o, 0, speed);
        func_00565790(0xFC, o, 0, speed);
        break;
    case 0xB2:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x34, o, 0, speed);
        func_00565790(0x96, o, 0, speed);
        func_00565790(0xAC, o, 0, speed);
        break;
    case 0xAA:
        func_00565790(0x19, o, 0, speed);
        func_00565790(0x2E, o, 0, speed);
        func_00565790(0x96, o, 0, speed);
        func_00565790(0xAC, o, 0, speed);
        break;
    case 0x97:
        func_00565790(0x4C, o, 0, speed);
        func_00565790(0x86, o, 0, speed);
        func_00565790(0x96, o, 0, speed);
        break;
    case 0x38E:
        func_00565790(0x1C, o, 0, speed);
        break;
    case 0x386:
        func_00565790(0xC, o, 0, speed);
        func_00565790(0x16, o, 0, speed);
        func_005658B0(0x24, o, 2, 3, speed);
        func_00565790(0x48, o, 0, speed);
        func_00565790(0x4C, o, 0, speed);
        break;
    case 0x385:
        func_00565790(0x26, o, 0, speed);
        break;
    case 0x383:
        func_005658B0(0, o, 0, 5, speed);
        break;
    case 0x382:
        func_00565790(0xD, o, 0, speed);
        func_00565790(0x7F, o, 0, speed);
        func_00565790(0x150, o, 0, speed);
        break;
    case 0x381:
        func_00565790(0xD7, o, 0, speed);
        func_00565790(0xEE, o, 0, speed);
        func_00565790(0x129, o, 0, speed);
        func_00565790(0x139, o, 0, speed);
        break;
    case 0x380:
        func_00565790(0x154, o, 0, speed);
        func_00565790(0x180, o, 0, speed);
        func_005658B0(0x4A, o, 0, 6, speed);
        break;
    case 0x37F:
        func_005658B0(0, o, 2, 1, speed);
        break;
    case 0x37B:
        func_005658B0(0, o, 2, 3, speed);
        break;
    case 0x37A:
        func_00565790(0x1C, o, 0, speed);
        func_00565790(0x26, o, 0, speed);
        func_005658B0(0, o, 2, 3, speed);
        func_00565790(0x86, o, 0, speed);
        func_00565790(0xBC, o, 0, speed);
        break;
    case 0x379:
        func_00565790(0x1E, o, 0, speed);
        func_00565790(0x2E, o, 0, speed);
        func_00565790(0x3C, o, 0, speed);
        func_00565790(0x6C, o, 0, speed);
        func_00565790(0x78, o, 0, speed);
        func_00565790(0x9E, o, 0, speed);
        func_00565790(0xB0, o, 0, speed);
        break;
    case 0x364:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x32, o, 0, speed);
        func_00565790(0xA, o, 0, speed);
        func_00565790(0x96, o, 0, speed);
        func_00565790(0xAA, o, 0, speed);
        func_00565790(0xD5, o, 0, speed);
        func_00565790(0xE4, o, 0, speed);
        func_00565790(0xFD, o, 0, speed);
        func_00565790(0x195, o, 0, speed);
        func_00565790(0x1B5, o, 0, speed);
        break;
    case 0x35A:
        func_00565790(0x54, o, 0, speed);
        break;
    case 0x358:
        func_00565790(0x14, o, 0, speed);
        break;
    case 0x357:
        func_00565790(0x19, o, 0, speed);
        break;
    case 0x355:
        func_00565790(0x19, o, 0, speed);
        break;
    case 0x319:
        func_00565790(0x3C, o, 0, speed);
        func_00565790(0x3F, o, 0, speed);
        break;
    case 0x318:
        func_00565790(0x48, o, 0, speed);
        func_00565790(0x60, o, 0, speed);
        func_00565790(0x7D, o, 0, speed);
        break;
    case 0x30E:
        func_00565790(0x14E, o, 0, speed);
        func_00565790(0x178, o, 0, speed);
        func_005658B0(0x3E, o, 0, 6, speed);
        break;
    case 0x30B:
        func_00565790(0x24, o, 0, speed);
        break;
    case 0x30A:
        func_00565790(0x22, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        func_00565790(0x68, o, 0, speed);
        func_00565790(0x88, o, 0, speed);
        func_00565790(0xAC, o, 0, speed);
        break;
    case 0x309:
        func_00565790(0x22, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        func_00565790(0x68, o, 0, speed);
        func_00565790(0x8A, o, 0, speed);
        func_00565790(0xAC, o, 0, speed);
        break;
    case 0x308:
        func_00565790(0x24, o, 0, speed);
        func_00565790(0x36, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        break;
    case 0x307:
        func_00565790(0x24, o, 0, speed);
        func_00565790(0x36, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        break;
    case 0x306:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x34, o, 0, speed);
        func_00565790(0x50, o, 0, speed);
        break;
    case 0x305:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x34, o, 0, speed);
        func_00565790(0x50, o, 0, speed);
        break;
    case 0x303:
        func_00565790(0xA, o, 0, speed);
        func_00565790(0x20, o, 0, speed);
        func_00565790(0x30, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        break;
    case 0x302:
        func_00565790(0xA, o, 0, speed);
        func_00565790(0x20, o, 0, speed);
        func_00565790(0x30, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        break;
    case 0x2F8:
        func_00565790(0xF, o, 0, speed);
        func_00565790(0x14, o, 0, speed);
        break;
    case 0x2E3:
        func_00565790(0x3A, o, 0, speed);
        break;
    case 0x2DF:
        func_00565790(0x18, o, 4, speed);
        break;
    case 0x2DE:
        func_00565790(2, o, 4, speed);
        func_00565790(0x4C, o, 4, speed);
        break;
    case 0x2DD:
        func_00565790(0x30, o, 0, speed);
        func_00565790(0x4B, o, 0, speed);
        break;
    case 0x2E1:
        func_00565790(0x18, o, 3, speed);
        func_00565790(0x34, o, 0, speed);
        func_00565790(0x4A, o, 0, speed);
        break;
    case 0x2D9:
        func_00565790(0x18, o, 3, speed);
        func_00565790(0x6E, o, 3, speed);
        break;
    case 0x2D8:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x8A, o, 3, speed);
        func_00565790(0xC4, o, 3, speed);
        break;
    case 0x2D7:
        func_00565790(0x32, o, 0, speed);
        func_00565790(0x54, o, 0, speed);
        func_00565790(0x66, o, 0, speed);
        func_00565790(0x7A, o, 0, speed);
        break;
    case 0x2D6:
        func_00565790(0x2E, o, 0, speed);
        break;
    case 0x2D5:
        func_00565790(0x23, o, 0, speed);
        func_00565790(0x3C, o, 0, speed);
        break;
    case 0x26B:
        func_00565790(0x24, o, 0, speed);
        break;
    case 0x269:
        func_00565790(0x12, o, 0, speed);
        func_00565790(0x2B, o, 0, speed);
        break;
    case 0x268:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x22, o, 0, speed);
        break;
    case 0x266:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x3C, o, 0, speed);
        break;
    case 0x263:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x3C, o, 0, speed);
        break;
    case 0x25F:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        break;
    case 0x25E:
        func_00565790(0x23, o, 0, speed);
        func_00565790(0x46, o, 0, speed);
        break;
    case 0x25D:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x2C, o, 0, speed);
        break;
    case 0x25C:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x5E, o, 0, speed);
        func_005659F0(0x22, o, 0, 1, speed);
        func_005658B0(0x22, o, 2, 3, speed);
        func_00565C80(0x24, o, 2, 0, speed);
        func_00565C80(0x2A, o, 3, 0, speed);
        break;
    case 0x25B:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x5E, o, 0, speed);
        func_005659F0(0x24, o, 0, 1, speed);
        func_005658B0(0x24, o, 2, 3, speed);
        func_00565C80(0x26, o, 2, 0, speed);
        func_00565C80(0x2C, o, 3, 0, speed);
        break;
    case 0x25A:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x5E, o, 0, speed);
        func_005659F0(0x22, o, 0, 1, speed);
        func_005658B0(0x22, o, 2, 3, speed);
        func_00565C80(0x24, o, 2, 0, speed);
        func_00565C80(0x2A, o, 3, 0, speed);
        break;
    case 0x259:
        func_00565820(0x14, o, 0, speed);
        func_00565820(0x28, o, 0, speed);
        break;
    case 0x258:
        func_00565820(0x10, o, 0, speed);
        func_00565820(0x26, o, 0, speed);
        break;
    case 0x257:
        func_00565790(0xA, o, 0, speed);
        break;
    case 0x256:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x5E, o, 0, speed);
        func_005659F0(0x18, o, 0, 1, speed);
        func_005658B0(0x18, o, 2, 3, speed);
        func_00565DA0(0x1E, o, 3, 0x12, 0, speed);
        func_00565C80(0x1A, o, 2, 0, speed);
        func_00565C80(0x26, o, 3, 0, speed);
        break;
    case 0x255:
        func_005659F0(0x14, o, 0, 1, speed);
        func_005658B0(0x14, o, 2, 3, speed);
        func_00565790(0x24, o, 0, speed);
        func_00565C80(0x14, o, 2, 0, speed);
        func_00565C80(0x20, o, 3, 0, speed);
        break;
    case 0x254:
        func_00565790(0x10, o, 0, speed);
        func_005659F0(0x14, o, 0, 1, speed);
        func_005658B0(0x14, o, 2, 3, speed);
        func_00565C80(0x12, o, 2, 0, speed);
        func_00565C80(0x22, o, 3, 0, speed);
        break;
    case 0x253:
        func_00565790(8, o, 0, speed);
        func_00565790(0x13, o, 0, speed);
        break;
    case 0x24E:
        func_00565790(2, o, 4, speed);
        func_00565790(0x38, o, 4, speed);
        func_005658B0(0x6E, o, 2, 0xC, speed);
        func_005658B0(0x94, o, 2, 0xC, speed);
        func_00565790(0xCC, o, 0, speed);
        func_00565790(0xF2, o, 0, speed);
        func_00565790(0x12A, o, 0, speed);
        func_00565790(0x146, o, 0, speed);
        break;
    case 0x24D:
        func_00565790(0x36, o, 0, speed);
        func_005658B0(0x72, o, 2, 0xC, speed);
        func_005658B0(0xB2, o, 2, 0xC, speed);
        func_005658B0(0xEA, o, 2, 0xC, speed);
        func_005658B0(0x112, o, 2, 0xC, speed);
        break;
    case 0x24B:
        func_00565790(0x1C, o, 0, speed);
        func_00565790(0x34, o, 0, speed);
        break;
    case 0x24A:
        func_00565790(0x1C, o, 0, speed);
        func_00565790(0x34, o, 0, speed);
        break;
    case 0x249:
        func_00565790(0x20, o, 0, speed);
        func_00565790(0x64, o, 0, speed);
        func_00565790(0x84, o, 0, speed);
        func_00565790(0xAE, o, 0, speed);
        func_00565790(0xCC, o, 0, speed);
        break;
    case 0x248:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x3E, o, 0, speed);
        break;
    case 0x247:
        func_00565790(0x36, o, 0, speed);
        func_00565790(0x54, o, 0, speed);
        func_00565790(0x7C, o, 3, speed);
        func_00565790(0xA2, o, 0, speed);
        break;
    case 0x246:
        func_00565790(0x24, o, 0, speed);
        func_00565790(0x3C, o, 0, speed);
        break;
    case 0x237:
        func_005658B0(0x1E, o, 2, 8, speed);
        func_005658B0(0x94, o, 0, 6, speed);
        func_005658B0(0x94, o, 2, 0xC, speed);
        func_005658B0(0xD8, o, 2, 0xC, speed);
        func_005658B0(0x112, o, 2, 0xC, speed);
        func_00565790(0x1A6, o, 0, speed);
        break;
    case 0x236:
        func_00565790(0x5C, o, 0, speed);
        break;
    case 0x235:
        func_00565790(0x12A, o, 0, speed);
        func_00565790(0xA, o, 4, speed);
        func_00565790(0x3E, o, 4, speed);
        break;
    case 0x234:
        func_005658B0(0x1A, o, 2, 8, speed);
        func_005658B0(0x94, o, 0, 6, speed);
        func_005658B0(0x94, o, 2, 0xC, speed);
        func_005658B0(0xE8, o, 2, 0xC, speed);
        func_005658B0(0x136, o, 2, 0xC, speed);
        func_005658B0(0x162, o, 2, 0xC, speed);
        func_005658B0(0x1A8, o, 2, 0xC, speed);
        func_005658B0(0x1DA, o, 2, 0xC, speed);
        func_00565790(0x1F6, o, 4, speed);
        func_00565790(0x23A, o, 4, speed);
        break;
    case 0x22E:
        func_00565790(0x40, o, 0, speed);
        func_00565790(0x60, o, 0, speed);
        break;
    case 0x22A:
        func_00565790(0xC, o, 0, speed);
        break;
    case 0x229:
        func_00565790(0x64, o, 0, speed);
        break;
    case 0x223:
        func_00565790(0x54, o, 0, speed);
        break;
    case 0x222:
        func_00565790(0x64, o, 0, speed);
        func_005658B0(0x2C, o, 0, 6, speed);
        break;
    case 0x21E:
        func_00565790(0x96, o, 0, speed);
        func_00565790(0xA2, o, 0, speed);
        break;
    case 0x21D:
        func_00565790(0x18, o, 0, speed);
        func_00565790(0x28, o, 0, speed);
        break;
    case 0x21C:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x22, o, 0, speed);
        break;
    case 0x21B:
        func_00565790(0x44, o, 0, speed);
        func_00565790(0x78, o, 0, speed);
        break;
    case 0x21A:
        func_00565790(0x36, o, 0, speed);
        func_00565790(0x4A, o, 0, speed);
        break;
    case 0x216:
        func_00565790(0xE, o, 0, speed);
        func_005658B0(0x20, o, 2, 8, speed);
        func_00565790(0x22, o, 0, speed);
        break;
    case 0x215:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x4A, o, 0, speed);
        break;
    case 0x214:
        func_00565790(0xC, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        break;
    case 0x213:
        func_00565790(0xE, o, 0, speed);
        func_005658B0(0x1E, o, 2, 8, speed);
        func_00565790(0x20, o, 0, speed);
        break;
    case 0x212:
        func_00565790(0x18, o, 0, speed);
        func_00565790(0x5A, o, 0, speed);
        break;
    case 0x211:
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x56, o, 0, speed);
        break;
    case 0x210:
        func_00565790(8, o, 0, speed);
        func_005658B0(0xC, o, 2, 8, speed);
        func_00565790(0x28, o, 0, speed);
        break;
    case 0x20F:
        func_00565790(0xC, o, 0, speed);
        func_005658B0(0x12, o, 2, 0xA, speed);
        func_00565790(0x5A, o, 0, speed);
        break;
    case 0x20E:
        func_00565790(0x10, o, 0, speed);
        func_005658B0(0x20, o, 2, 0xA, speed);
        func_00565790(0x46, o, 0, speed);
        break;
    case 0x20D:
        func_00565790(0x10, o, 0, speed);
        func_005658B0(0x12, o, 2, 8, speed);
        func_00565790(0x2E, o, 0, speed);
        break;
    case 0x20C:
        func_005658B0(0x1E, o, 2, 0xA, speed);
        func_00565790(0x1E, o, 0, speed);
        func_00565790(0x62, o, 0, speed);
        break;
    case 0x20B:
        func_00565790(0x18, o, 0, speed);
        func_005658B0(0x1E, o, 2, 0xA, speed);
        break;
    case 0x20A:
        func_00565790(0xA, o, 0, speed);
        func_005658B0(0x1C, o, 2, 8, speed);
        func_00565790(0x1E, o, 0, speed);
        break;
    case 0x209:
        func_00565790(0x12, o, 0, speed);
        func_00565790(0x4A, o, 0, speed);
        break;
    case 0x208:
        func_00565790(0xC, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        break;
    case 0x207:
        func_00565790(0xC, o, 0, speed);
        func_005658B0(0x1E, o, 2, 8, speed);
        func_00565790(0x20, o, 0, speed);
        break;
    case 0x206:
        func_00565790(0x1E, o, 0, speed);
        func_00565790(0x54, o, 0, speed);
        break;
    case 0x205:
        func_00565790(0x1E, o, 0, speed);
        func_00565790(0x54, o, 0, speed);
        break;
    case 0x201:
        func_00565790(0x130, o, 0, speed);
        break;
    case 0x200:
        func_00565790(4, o, 0, speed);
        func_00565790(0x24, o, 0, speed);
        func_00565790(0x46, o, 0, speed);
        break;
    case 0x1FF:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x56, o, 0, speed);
        break;
    case 0x1FB:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        func_00565790(0x30, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        break;
    case 0x1F9:
        func_00565790(0x1C, o, 0, speed);
        func_00565790(0x26, o, 0, speed);
        break;
    case 0x350:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x32, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        break;
    case 0x349:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        break;
    case 0x348:
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x40, o, 0, speed);
        break;
    case 0x346:
        func_00565790(0x1E, o, 0, speed);
        func_00565790(0x32, o, 0, speed);
        break;
    case 0x11A:
        func_00565790(0x18, o, 0, speed);
        func_00565790(0x32, o, 0, speed);
        break;
    case 0x119:
        func_00565790(0x24, o, 0, speed);
        func_00565790(0x3C, o, 0, speed);
        break;
    case 0x117:
        func_005659F0(0xE, o, 0, 1, speed);
        func_005658B0(0xE, o, 2, 3, speed);
        func_00565C80(0xE, o, 2, 0, speed);
        func_00565C80(0x14, o, 3, 0, speed);
        break;
    case 0x116:
        func_005659F0(0xE, o, 0, 1, speed);
        func_005658B0(0xE, o, 2, 3, speed);
        func_00565C80(0xE, o, 2, 0, speed);
        func_00565C80(0x14, o, 3, 0, speed);
        break;
    case 0x115:
        func_005659F0(0xE, o, 0, 1, speed);
        func_005658B0(0xE, o, 2, 3, speed);
        func_00565C80(0xE, o, 2, 0, speed);
        func_00565C80(0x14, o, 3, 0, speed);
        break;
    case 0x110:
        func_005659F0(0, o, 0, 0x11, speed);
        break;
    case 0x10F:
        func_005659F0(0, o, 0, 0x11, speed);
        break;
    case 0x10E:
        func_005659F0(0, o, 0, 0x11, speed);
        break;
    case 0x10D:
        func_00565790(0x1A, o, 0, speed);
        break;
    case 0x10C:
        func_00565790(0x1A, o, 0, speed);
        break;
    case 0x10A:
        func_005659F0(0x14, o, 0, 1, speed);
        func_005658B0(0x14, o, 2, 3, speed);
        func_00565C80(0x12, o, 2, 0, speed);
        func_00565C80(0x1C, o, 3, 0, speed);
        break;
    case 0x109:
        func_005659F0(0x10, o, 0, 1, speed);
        func_005658B0(0x10, o, 2, 3, speed);
        func_00565C80(0x12, o, 2, 0, speed);
        func_00565C80(0x1C, o, 3, 0, speed);
        break;
    case 0x108:
        func_005659F0(0x12, o, 0, 1, speed);
        func_005658B0(0x12, o, 2, 3, speed);
        func_00565C80(0x12, o, 2, 0, speed);
        func_00565C80(0x1C, o, 3, 0, speed);
        break;
    case 0xEC:
        func_00565790(0xC, o, 0, speed);
        break;
    case 0xEB:
        func_00565790(0x16, o, 0, speed);
        break;
    case 0x3A3:
        func_00565C20(0x6E, o, 0xB7, speed);
        break;
    case 0x1DD:
        func_00565790(0x1C, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        break;
    case 0x1DC:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x2A, o, 0, speed);
        break;
    case 0x1DB:
        func_00566090(0, o, 2, 0, speed);
        func_00565DA0(0, o, 5, 0x19, 0, speed);
        func_005659F0(0x10, o, 0, 2, speed);
        func_005659F0(0x48, o, 0, 3, speed);
        func_005659F0(0x70, o, 0, 0x18, speed);
        break;
    case 0x1D9:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x2A, o, 0, speed);
        break;
    case 0x1D8:
        func_00565790(2, o, 0, speed);
        func_00565790(0x20, o, 0, speed);
        break;
    case 0x1D7:
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x3A, o, 0, speed);
        break;
    case 0x1D5:
        func_00565790(0x26, o, 0, speed);
        func_00565790(0x38, o, 0, speed);
        break;
    case 0x1D3:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x36, o, 0, speed);
        break;
    case 0x1D2:
        func_00565790(0x26, o, 0, speed);
        func_00565790(0x38, o, 0, speed);
        break;
    case 0x1D0:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x28, o, 0, speed);
        break;
    case 0x9C:
    case 0x9F:
    case 0xA8:
    case 0x1B5:
    case 0x1B8:
    case 0x1BD:
    case 0x1BE:
    case 0x36C:
    case 0x371:
        break;
    case 0x1B4:
        func_00565790(0x1E, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        break;
    case 0x1B2:
        func_00565790(0x1D, o, 0, speed);
        func_00565790(0x2E, o, 0, speed);
        break;
    case 0x1B1:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x30, o, 0, speed);
        break;
    case 0x1AC:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x24, o, 0, speed);
        break;
    case 0x1A7:
        func_005659F0(0x14, o, 0, 6, speed);
        break;
    case 0x1A6:
        func_00566090(0, o, 2, 0, speed);
        func_005659F0(0xE, o, 0, 2, speed);
        break;
    case 0x1A5:
        func_00565790(0x1C, o, 0, speed);
        func_00565790(0x46, o, 0, speed);
        break;
    case 0x1A4:
        func_00565790(0x12, o, 0, speed);
        break;
    case 0x1A3:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x2D, o, 0, speed);
        break;
    case 0x1A2:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x2D, o, 0, speed);
        break;
    case 0x19D:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x26, o, 0, speed);
        break;
    case 0x1A1:
        func_00565DA0(0x28, o, 4, 0, 0, speed);
        func_005659F0(0x28, o, 0, 6, speed);
        break;
    case 0x19C:
    case 0x19F:
        func_00565DA0(0x28, o, 4, 0, 0, speed);
        func_005659F0(0x28, o, 0, 6, speed);
        break;
    case 0x19A:
        func_00565790(0x28, o, 0, speed);
        break;
    case 0x174:
        func_00565790(2, o, 0, speed);
        break;
    case 0x172:
        func_00565790(0x10, o, 0, speed);
        break;
    case 0x16E:
        func_00565790(0x26, o, 0, speed);
        func_00565790(0x104, o, 0, speed);
        func_00565790(0x122, o, 0, speed);
        break;
    case 0x16D:
        func_00565790(0x22, o, 0, speed);
        func_00565790(0x106, o, 0, speed);
        func_00565790(0x120, o, 0, speed);
        break;
    case 0x378:
        func_00565790(0x41, o, 0, speed);
        func_00565790(0x5A, o, 0, speed);
        break;
    case 0x377:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        break;
    case 0x376:
        func_00565790(0x40, o, 0, speed);
        func_00565790(0x5C, o, 0, speed);
        break;
    case 0x375:
        func_00565790(0x15, o, 0, speed);
        func_00565790(0x3A, o, 0, speed);
        break;
    case 0x372:
        func_00565790(0x41, o, 0, speed);
        func_00565790(0x57, o, 0, speed);
        break;
    case 0x370:
        func_00565790(0x25, o, 0, speed);
        func_00565790(0x3E, o, 0, speed);
        break;
    case 0x36D:
        func_00565790(0x42, o, 0, speed);
        break;
    case 0x36B:
        func_00565790(0x38, o, 0, speed);
        break;
    case 0x169:
        func_00565790(0xC, o, 0, speed);
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x24, o, 0, speed);
        func_00565790(0x56, o, 0, speed);
        break;
    case 0x165:
        func_00565790(0x30, o, 0, speed);
        break;
    case 0x164:
        func_00565790(0xE, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        func_00565790(0x24, o, 0, speed);
        break;
    case 0x160:
        func_00565790(0x14, o, 0, speed);
        break;
    case 0x15D:
        func_00565BC0(0x24, o, 0, speed);
        break;
    case 0x15C:
        func_00565BC0(0x24, o, 0, speed);
        break;
    case 0x15A:
    case 0x15B:
        if (o->unk54F == 0x39) {
            func_00565C80(1, o, 0, 0xC8, speed);
            func_00565C80(0x64, o, 3, 0, speed);
        }
        break;
    case 0x159:
        func_00565BC0(0x24, o, 0, speed);
        break;
    case 0x158:
        func_00565BC0(0x24, o, 0, speed);
        break;
    case 0x157:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x32, o, 0, speed);
        func_00565790(0x62, o, 0, speed);
        func_00565790(0x80, o, 0, speed);
        func_00565790(0x94, o, 0, speed);
        func_00565790(0xCC, o, 0, speed);
        break;
    case 0x156:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x56, o, 0, speed);
        func_00565790(0x68, o, 0, speed);
        func_00565790(0x88, o, 0, speed);
        func_00565790(0xA2, o, 0, speed);
        break;
    case 0x155:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x34, o, 0, speed);
        func_00565790(0x5A, o, 0, speed);
        func_00565790(0x7E, o, 0, speed);
        func_00565790(0x94, o, 0, speed);
        break;
    case 0x154:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x30, o, 0, speed);
        func_00565790(0x50, o, 0, speed);
        func_00565790(0x6A, o, 0, speed);
        break;
    case 0x11E:
        func_00565790(0x3A, o, 0, speed);
        func_005658B0(0x12, o, 2, 3, speed);
        func_00565C80(0x10, o, 0, 0x7E, speed);
        func_00565C80(0x16, o, 3, 0, speed);
        break;
    case 0xED:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x24, o, 0, speed);
        func_00565790(0x36, o, 0, speed);
        break;
    case 0xE9:
        func_005658B0(0xA, o, 2, 3, speed);
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x26, o, 0, speed);
        func_00565790(0x36, o, 0, speed);
        func_00565C80(0xA, o, 1, 0, speed);
        func_00565C80(0x18, o, 3, 0, speed);
        break;
    case 0xE8:
        func_00565790(2, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        func_00565790(0x4A, o, 0, speed);
        func_00565790(0x6E, o, 0, speed);
        func_00565790(0xA5, o, 0, speed);
        func_00565790(0xC4, o, 0, speed);
        break;
    case 0xE7:
        func_00565790(2, o, 0, speed);
        func_00565790(0x29, o, 0, speed);
        func_00565790(0x49, o, 0, speed);
        func_00565790(0x74, o, 0, speed);
        func_00565790(0x8C, o, 0, speed);
        func_00565790(0xBE, o, 0, speed);
        break;
    case 0xE3:
        func_00565790(0xA, o, 0, speed);
        break;
    case 0xFD:
        func_00565C80(0x14, o, 1, 0, speed);
        func_005659F0(0x14, o, 0, 0, speed);
        break;
    case 0xFC:
        func_00565C80(0x14, o, 1, 0, speed);
        func_005659F0(0x14, o, 0, 0, speed);
        break;
    case 0xD7:
        func_00565C80(0x14, o, 1, 0, speed);
        func_005659F0(0x14, o, 0, 0, speed);
        break;
    case 0xD5:
        func_00565790(0xE, o, 0, speed);
        break;
    case 0xC0:
        func_00565790(0x32, o, 0, speed);
        break;
    case 0xBF:
        func_00565790(0x2E, o, 0, speed);
        break;
    case 0xBE:
        func_00565790(0x3A, o, 0, speed);
        func_00565790(0x56, o, 0, speed);
        func_00565790(0x82, o, 0, speed);
        func_00565790(0xA0, o, 0, speed);
        break;
    case 0xBD:
        func_00565790(2, o, 0, speed);
        func_00565790(0x32, o, 0, speed);
        func_00565790(0x54, o, 0, speed);
        func_00565790(0x87, o, 0, speed);
        break;
    case 0xBC:
        func_00565790(0x17, o, 0, speed);
        func_00565790(0x32, o, 0, speed);
        break;
    case 0xB9:
        func_00565790(0xA, o, 0, speed);
        func_00565790(0x2A, o, 0, speed);
        break;
    case 0xB1:
        func_00565790(0x14, o, 0, speed);
        func_00565790(0x29, o, 0, speed);
        func_00565790(0x45, o, 0, speed);
        func_00565790(0x7A, o, 0, speed);
        func_00565790(0x8C, o, 0, speed);
        break;
    case 0xAE:
        func_00565790(0xC, o, 0, speed);
        func_00565790(0x42, o, 0, speed);
        func_00565790(0x68, o, 0, speed);
        func_00565790(0x8D, o, 0, speed);
        break;
    case 0xAD:
        func_00565790(0x22, o, 0, speed);
        func_00565790(0x48, o, 0, speed);
        func_00565790(0x74, o, 0, speed);
        func_00565790(0x96, o, 0, speed);
        break;
    case 0xAC:
        func_00565790(0x34, o, 0, speed);
        break;
    case 0xA9:
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        func_00565790(0x76, o, 0, speed);
        func_00565790(0x8C, o, 0, speed);
        break;
    case 0x9A:
        func_00565DA0(0x5C, o, 2, 0, 0, speed);
        func_00565790(0x10, o, 2, speed);
        func_00565790(0x4A, o, 2, speed);
        func_00565790(0x5C, o, 0, speed);
        break;
    case 0x99:
        func_00565790(0x56, o, 2, speed);
        func_00565790(0xB0, o, 2, speed);
        break;
    case 0x95:
        func_00565790(2, o, 0, speed);
        break;
    case 0x92:
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x50, o, 0, speed);
        break;
    case 0x8F:
        func_00565790(0x12, o, 3, speed);
        func_00565790(0x44, o, 3, speed);
        func_005658B0(0x3C, o, 2, 0xA, speed);
        break;
    case 0x8E:
        func_00565790(0x4A, o, 3, speed);
        func_005658B0(0x22, o, 2, 0xA, speed);
        break;
    case 0x8D:
        func_00565790(2, o, 0, speed);
        func_00565790(0x12, o, 0, speed);
        break;
    case 0x8B:
        func_005658B0(0x16, o, 2, 0xA, speed);
        func_00565790(0x22, o, 0, speed);
        func_00565790(0x38, o, 0, speed);
        break;
    case 0x8A:
        func_005658B0(0x16, o, 2, 0xA, speed);
        func_00565790(0x22, o, 0, speed);
        func_00565790(0x38, o, 0, speed);
        break;
    case 0x89:
        func_00565790(0x26, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        func_00565790(0x72, o, 0, speed);
        func_005658B0(0x72, o, 2, 8, speed);
        func_00565790(0x5C, o, 3, speed);
        break;
    case 0x88:
        func_005658B0(0x38, o, 2, 0xA, speed);
        func_00565790(0x34, o, 0, speed);
        func_00565790(0x2C, o, 3, speed);
        break;
    case 0x87:
        func_00565790(0x28, o, 0, speed);
        func_00565790(0x3C, o, 3, speed);
        func_005658B0(0x2E, o, 2, 0xA, speed);
        func_00565790(0x68, o, 3, speed);
        break;
    case 0x86:
        func_00565790(8, o, 0, speed);
        break;
    case 0x85:
        func_00565790(0xC, o, 0, speed);
        break;
    case 0x84:
        func_005658B0(0x46, o, 2, 0xA, speed);
        break;
    case 0x83:
        func_005658B0(0x20, o, 2, 0xA, speed);
        break;
    case 0x7F:
        func_00565790(0xC, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        break;
    case 0x7E:
        func_00565790(0x10, o, 0, speed);
        func_00565790(0x44, o, 0, speed);
        func_00565790(0x74, o, 0, speed);
        func_00565790(0xA4, o, 0, speed);
        func_00565DA0(0x10, o, 6, 0x81, 0, speed);
        func_00565DA0(0x44, o, 6, 0x82, 0, speed);
        func_00565DA0(0x74, o, 6, 0x81, 0, speed);
        func_00565DA0(0xA4, o, 6, 0x82, 0, speed);
        break;
    case 0x7D:
        func_00565790(0x1E, o, 1, speed);
        func_00565790(0x36, o, 1, speed);
        func_00565DA0(0x1E, o, 6, 0x81, 0, speed);
        func_00565DA0(0x36, o, 6, 0x82, 0, speed);
        break;
    case 0x7C:
        func_00565790(0x1A, o, 0, speed);
        func_00565790(0x4C, o, 0, speed);
        func_00565DA0(0x1A, o, 6, 0x81, 0, speed);
        func_00565DA0(0x4C, o, 6, 0x82, 0, speed);
        break;
    case 0x68:
        func_00565790(8, o, 0, speed);
        func_00565790(0x18, o, 0, speed);
        break;
    case 0x67:
        func_00565790(0x1C, o, 0, speed);
        func_005658B0(0x32, o, 2, 8, speed);
        func_00565790(0x34, o, 0, speed);
        func_00565DA0(0x1C, o, 6, 0x81, 0, speed);
        func_00565DA0(0x34, o, 6, 0x82, 0, speed);
        break;
    case 0x66:
        func_00565790(0x16, o, 0, speed);
        func_00565790(0x36, o, 0, speed);
        func_00565DA0(0x16, o, 6, 0x41, 0, speed);
        func_00565DA0(0x36, o, 6, 0x42, 0, speed);
        break;
    case 0x65:
        func_00565790(0x18, o, 0, speed);
        func_00565790(0x3A, o, 0, speed);
        func_00565DA0(0x18, o, 6, 0x81, 0, speed);
        func_00565DA0(0x3A, o, 6, 0x82, 0, speed);
        break;
    }
    func_0056E7D0(o, speed);
}

#endif
