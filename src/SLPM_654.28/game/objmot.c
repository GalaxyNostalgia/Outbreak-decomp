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
    ObjLayer *l = &((ObjLayer *)o->unk1E4)[layer];
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

    if (!(((ObjLayer *)o->unk1E4)[layer].flags & 8)) {
        return 0;
    }
    m = (void **)o->model;
    if (((ObjLayer *)o->unk1E4)[layer].blendFrames == 0) {
        return 0;
    }
    l = &((ObjLayer *)o->unk1E4)[layer];
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

void func_005661C0(DrawObjFull *o);
void func_0056A440(DrawObjFull *o);
void func_005BB880(DrawObjFull *o);
void func_0058AA70(DrawObjFull *o);
void func_0058AF00(DrawObjFull *o);
void func_0058B200(DrawObjFull *o);
void func_0058B550(DrawObjFull *o);
void func_005BBC40(DrawObjFull *o);
void func_0058BA20(DrawObjFull *o);
void func_0058BE00(DrawObjFull *o);
void func_00570310(DrawObjFull *o);
void func_005655F0(DrawObjFull *o);

/* A character's own step for its current motion's set (another character's: func_005655F0). */
void func_005654D0(DrawObjFull *o) {
    switch (o->unkCE0) {
    case 0:
        break;
    case 0xFF:
        func_005655F0(o);
        return;
    }
    switch (o->unkCE1) {
    case 0:
        func_005661C0(o);
        break;
    case 1:
        func_0056A440(o);
        break;
    case 2:
        func_005BB880(o);
        break;
    case 3:
        func_0058AA70(o);
        break;
    case 4:
        func_0058AF00(o);
        break;
    case 5:
        func_0058B200(o);
        break;
    case 6:
        func_0058B550(o);
        break;
    case 7:
        func_005BBC40(o);
        break;
    case 8:
        func_0058BA20(o);
        break;
    case 9:
        func_0058BE00(o);
        break;
    case 10:
        func_00570310(o);
        break;
    }
}
void func_00578B30(DrawObjFull *o);

/* Before the motions advance: characters' and things' own step. */
void func_00543410(DrawObjFull *o) {
    if (o->type == 0 || o->type == 1) {
        func_005654D0(o);
    } else if (o->type == 2) {
        func_00578B30(o);
    }
}
s32 func_0055BE60(DrawObjFull *o, s32 layer, f32 speed);

/* Advances o's motion layers by speed (layer 0 last); the result is layer 0's (-1 when it is
   held). For players, 0xCE2 tells whether the motion is past its frame 0xCE3 (0x80) and, for
   ours, whether 0xF80 has any of 0xF3F (0x40). */
s8 func_0055C030(DrawObjFull *o, f32 speed) {
    s8 i;
    s8 r;

    func_00543410(o);
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
