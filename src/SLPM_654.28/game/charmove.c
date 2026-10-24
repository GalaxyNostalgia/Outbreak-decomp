#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* Character odds and ends run each frame. */

/* The point at 0x1AC: last frame's kept, then moved towards its target over unkCB2 frames. */
void func_0062CD00(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_0062CD00(DrawObjFull *o) {
    f32 *t;

    o->unk1BC[0] = o->unk1AC[0];
    o->unk1BC[1] = o->unk1AC[1];
    o->unk1C4[0] = o->unk1B4[0];
    o->unk1C4[1] = o->unk1B4[1];
    if (FIELD(o, u8, 0xCB2) == 0) {
        return;
    }
    if (--FIELD(o, u8, 0xCB2) != 0) {
        o->unk1AC[0] += FIELD(o, f32, 0xCC8);
        o->unk1AC[1] += FIELD(o, f32, 0xCCC);
        o->unk1B4[0] += FIELD(o, f32, 0xCD0);
        return;
    }
    t = (f32 *)o->unkCB8;
    o->unk1AC[0] = t[0];
    o->unk1AC[1] = t[1];
    o->unk1B4[0] = t[2];
    o->unk1B4[1] = t[3];
}
#endif

s32 func_001AF4D0(void);
void func_005A9CC0(DrawObjFull *o, s32 a, s32 b);

void func_0062CC50(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_0062CC50(DrawObjFull *o) {
    if (D_0048BF60.unk28 == 1 || (o->unk54A & 4)) {
        return;
    }
    if (func_001AF4D0() == 0 && o->type == 0 && o->kind != D_0048BF60.unk1D) {
        func_005A9CC0(o, 1, 0);
        return;
    }
    if (o->type == 1) {
        func_005A9CC0(o, 1, 0);
        return;
    }
    func_005A9CC0(o, 1, 3);
}
#endif

extern f32 D_00239E18;          /* 2 pi */

#include "model.h"
#include "vu0.h"
void func_00340310(s16 rx, s16 ry, s16 rz, Mat44 *m);
f32 func_00191A70(f32 x);

#include "model.h"
#include "thing.h"
#include "relation.h"

/* A character's skeleton: its bones by number (0x14 the neck, 0x15 the head); none when [0] is. */
#define OBJ_BONES(o) ((FlBone **)((u8 *)(o)->model + 0x820))

extern Thing D_003FFCA0[50];
f32 func_00191A70(f32 x);
s32 func_00346C50(const f32 *a, const f32 *b);

/* The nth thing (enemy) in o's view: active, posed, not hidden (0xC94 bit 0), within range, at
   most 300 above and 100 below, in sight, and within fov/2 of o's heading (by the heading kept
   in the relation tables, or with mode 1 by the line from o's bone 11 to its watched bone). Goes
   by o's list of things, nearest first. */
Thing *func_0062D700(DrawObjFull *o, s32 mode, s16 fov, u8 nth, f32 range);
#ifdef NON_MATCHING   /* register allocation; the original places the loop's setup after the loop */
Thing *func_0062D700(DrawObjFull *o, s32 mode, s16 fov, u8 nth, f32 range) {
    Vec3 eye;
    Vec3 to;
    s32 half;
    u8 n;
    u8 i;
    Relation *rel;
    Thing *t;
    u8 *list;

    i = 0;
    n = 0;
    if (o->type == 0) {
        list = D_003840E0[o->kind];
        rel = D_00385490[o->kind];
    } else {
        list = D_00383D50[o->kind];
        rel = D_003844F0[o->kind];
    }
    for (half = fov / 2; list[i] != 0xFF && i < 50; i++) {
        t = &D_003FFCA0[list[i]];
        if (t == NULL || t->active == 0 || t->model == NULL || (t->unkC94 & 1)) {
            continue;
        }
        if (!(rel[list[i]].dist <= range) || !(t->pos[1] - o->unk38[1] <= 300.0f) ||
            !(o->unk38[1] - t->pos[1] <= 100.0f) || (u8)rel[list[i]].hidden != 0) {
            continue;
        }
        if (mode != 1) {
            if (!(func_00191A70((s16)(rel[list[i]].angle - o->unk92)) <= func_00191A70(half))) {
                continue;
            }
        } else {
            eye = *(Vec3 *)OBJ_BONES(o)[0xB]->world[3];
            if (((FlBone **)t->model->unk820)[0] != NULL) {
                to = *(Vec3 *)((FlBone **)t->model->unk820)[t->unk552]->world[3];
            } else {
                to = *(Vec3 *)t->pos;
            }
            if (!(func_00191A70((s16)((s16)func_00346C50(&eye.x, &to.x) - o->unk92)) <=
                  func_00191A70(half))) {
                continue;
            }
        }
        if (n == nth) {
            return t;
        }
        n++;
    }
    return NULL;
}
#endif


/* An angle in radians as the game's (0x10000 a turn), rounded. */
#define ANG(r) ((s16)(0.5f + 65536.0f * (r) / D_00239E18) & 0xFFFF)

/* Where o's bone b is (its position when it has no skeleton). */
void func_0062F740(DrawObjFull *o, u8 b, f32 *out) {
    FlBone **bones = OBJ_BONES(o);
    Vec3 *p;

    if (bones[0] != NULL) {
        p = (Vec3 *)bones[b]->world[3];
    } else {
        p = (Vec3 *)o->unk38;
    }
    *(Vec3 *)out = *p;
}

void func_00192C00(FlBone *b, f32 *parent, f32 sx, f32 sy, f32 sz);

/* Turns the head toward ang (x, y; limited to +-0x17FF and +-0x1FFF) over the frames left in
   unkBD2, a step a call, and poses the head bone (its matrix kept at 0xBF0). */
void func_0062F790(DrawObjFull *o, s16 *ang);
#ifdef NON_MATCHING   /* the head bone is loaded once where the original reloads it */
void func_0062F790(DrawObjFull *o, s16 *ang) {
    FlBone **bones = OBJ_BONES(o);
    s16 x;
    s16 y = ang[1];

    x = ang[0];

    if (y >= 0x2000) {
        y = 0x1FFF;
    }
    if (y < -0x1FFF) {
        y = -0x1FFF;
    }
    if (x >= 0x1800) {
        x = 0x17FF;
    }
    if (x < -0x17FF) {
        x = -0x17FF;
    }
    if (o->unkBD2 == 0) {
        o->unkBD2 = 1;
    }
    o->unkBD4[0] = (x - o->unkBD8[0]) / o->unkBD2;
    o->unkBD4[1] = (y - o->unkBD8[1]) / o->unkBD2;
    o->unkBD2--;
    o->unkBD8[0] += o->unkBD4[0];
    o->unkBD8[1] += o->unkBD4[1];
    func_00340310(o->unkBDC[0] = o->unkBD8[0], o->unkBDC[1] = o->unkBD8[1],
                  ANG(bones[0x15]->cur[1][2]),
                  (Mat44 *)bones[0x15]->local);
    func_00192C00(bones[0x15], bones[0x14]->world[0], bones[0x14]->cur[0][0],
                  bones[0x14]->cur[0][1], bones[0x14]->cur[0][2]);
    func_00190ED0(o->unkBF0, bones[0x15]->local[0]);
}
#endif

/* The head's turn (from the skeleton's head bone) as angles. */
void func_0062F930(DrawObjFull *o) {
    u8 *m = (u8 *)o->model;

    FIELD(o, s16, 0xBD8) = 0.5f + 65536.0f * FIELD(FIELD(m, u8 *, 0x874), f32, 0x110) / D_00239E18;
    FIELD(o, s16, 0xBDA) = 0.5f + 65536.0f * FIELD(FIELD(m, u8 *, 0x874), f32, 0x114) / D_00239E18;
    FIELD(o, s16, 0xBD6) = 0;
    FIELD(o, s16, 0xBD4) = 0;
    o->unkBD1 = 0;
    FIELD(o, s16, 0xBD2) = 0;
    FIELD(o, s16, 0xC30) = 0;
    FIELD(o, s32, 0xBE0) = 0;
}

extern s16 D_0023BB98;
extern u32 D_0023ACC0;          /* how long it keeps looking at someone: the random part */
extern u32 D_0023ACC4;          /* and the fixed part */
extern f32 D_00477EE8[3];      /* a point in the room (mode 2 looks 170 above it) */
extern s8 D_0048BF88[];
void func_0055C480(DrawObjFull *o, s32 bone, s16 *ang);
s32 func_00346C50(const f32 *a, const f32 *b);
void func_00346CD0(s16 *ang, const f32 *from, const f32 *to);
u16 func_001ADEF0(s32 n);

/* Where the head looks (mode, from the caller; forced to 0 while hurt (0xBA0 0x8038), and on
   motions with 0x211 bits 0x18 it is taken as already done): 0 back to the motion's own turn,
   1 at the nearest character in view within 1000 (0xBE0; another one after unkC30 frames), 2
   along the room's point D_00477EE8 (170 up), 4 at 0xB48's target for 0xB60 frames, 5 by the
   left stick, 6 at the room's origin (170 up). */
void func_0062F9B0(DrawObjFull *o, u8 mode);
#ifdef NON_MATCHING   /* register allocation; ang[1] through a pointer in the original */
void func_0062F9B0(DrawObjFull *o, u8 mode) {
    f32 m[4][4];
    s16 ang[3];
    Vec3 from;
    Vec3 tgt;
    Vec3 tmp;
    Vec3 d;
    Vec3 eye;
    Vec3 to;
    s16 dir;
    FlBone **bones;
    DrawObjFull *t;
    DrawObjFull *near;
    f32 dist;
    f32 a;
    s16 hx;
    s16 hy;
    u32 div;
    f32 rate = D_00306590.unkC ? 6 : 12;

    if (D_0023BB98 == 1 && D_0048BF88[0] == 0) {
        return;
    }
    bones = OBJ_BONES(o);
    if (o->unkBA0 & 0x8000) {
        mode = 0;
    }
    if (o->unkBA0 & 0x38) {
        mode = 0;
    }
    if ((((s8 *)o->unk204)[0xD] & 0x18) && mode != 0) {
        mode = 0;
        o->unkBD1 = 1;
    }
    t = o->unkBE0;
    hx = ANG(bones[0x15]->cur[1][0]);
    hy = ANG(bones[0x15]->cur[1][1]);
    if (t != NULL && (t->active == 0 || t->model == NULL)) {
        o->unkBE0 = NULL;
    }
    switch (mode) {
    case 0:
        if (o->unkBD1 != 0) {
            o->unkBD1 = 0;
            if (D_0048BF88[0] != 0) {
                if (o->unkB60 > 0) {
                    o->unkBD2 = o->unkB60;
                    o->unkBD2 = (D_00306590.unkC ? o->unkBD2 / 2 : o->unkBD2) + 1;
                } else {
                    o->unkBD2 = 10;
                }
            } else {
                o->unkBD2 = 10;
            }
        }
        if (o->unkBD1 == 0 && o->unkBD2 != 0) {
            ang[0] = 0.5f + 65536.0f * bones[0x15]->cur[1][0] / D_00239E18;
            ang[1] = 0.5f + 65536.0f * bones[0x15]->cur[1][1] / D_00239E18;
            ang[2] = 0.5f + 65536.0f * bones[0x15]->cur[1][2] / D_00239E18;
            func_0062F790(o, ang);
            if (o->unkBD2 == 0) {
                func_0062F930(o);
            }
        }
        break;
    case 1:
        near = (DrawObjFull *)func_0062D700(o, 1, -0x8000, 0, 1000.0f);
        if (o->unkBD1 == 0) {
            if (o->unkBD2 != 0) {
                func_0062F9B0(o, 0);
                break;
            }
            if (o->unkC30 == 0) {
                if (o->unkBE0 == NULL && near == NULL) {
                    break;
                }
                o->unkBE0 = near;
                o->unkBD2 = 1;
                div = D_00306590.unkC ? 2 : 1;
                o->unkC30 = (D_0023ACC4 + (func_001ADEF0(1) & 0xFFFF) % D_0023ACC0) / div;
            } else {
                o->unkC30--;
                break;
            }
        }
        t = o->unkBE0;
        if (t != NULL) {
            eye = *(Vec3 *)OBJ_BONES(o)[0x14]->world[3];
            if (OBJ_BONES(t)[0] != NULL) {
                to = *(Vec3 *)OBJ_BONES(t)[t->unk552]->world[3];
            } else {
                to = *(Vec3 *)t->unk38;
            }
            d.x = to.x - eye.x;
            d.y = to.y - eye.y;
            d.z = to.z - eye.z;
            dir = func_00346C50(&eye.x, &to.x);
            dist = func_00191690(&d.x);
            a = func_00191A70((s16)(dir - o->unk92));
            if (a > func_00191A70(16384.0f)) {
                o->unkBD2 = 0;
                o->unkC30 = 0;
            }
            if (dist > 1000.0f) {
                o->unkBD2 = 0;
                o->unkC30 = 0;
            }
        }
        if (o->unkBD2 == 0) {
            if (o->unkC30 == 0) {
                near = (DrawObjFull *)func_0062D700(o, 1, -0x8000, 1, 1000.0f);
                if (near == o->unkBE0) {
                    near = (DrawObjFull *)func_0062D700(o, 1, -0x8000, 0, 1000.0f);
                } else if (near == NULL) {
                    near = (DrawObjFull *)func_0062D700(o, 1, -0x8000, 0, 1000.0f);
                }
                o->unkBD1 = 0;
                o->unkBE0 = near;
                div = D_00306590.unkC ? 2 : 1;
                o->unkC30 = (D_0023ACC4 + (func_001ADEF0(1) & 0xFFFF) % D_0023ACC0) / div;
            } else {
                o->unkC30--;
            }
        }
        t = o->unkBE0;
        if (t == NULL) {
            o->unkBD1 = 1;
            func_0062F9B0(o, 0);
            break;
        }
        func_0062F740(t, t->unk552, &tgt.x);
        func_00190E00(m[0], bones[0x14]->world[0]);
        from.x = bones[0x15]->cur[2][0];
        from.y = bones[0x15]->cur[2][1];
        from.z = bones[0x15]->cur[2][2];
        func_00191410(&tgt.x, m[0]);
        tgt.x += m[3][0];
        tgt.y += m[3][1];
        tgt.z += m[3][2];
        func_00346CD0(ang, &from.x, &tgt.x);
        ang[0] = -ang[0];
        ang[1] -= ANG(bones[0x15]->cur[1][1]);
        ang[2] = 0.5f + 65536.0f * bones[0x15]->cur[1][2] / D_00239E18;
        if (o->unkBD1 != 1) {
            if (func_00191A70(ang[0] - o->unkBD8[0]) > func_00191A70(ang[1] - o->unkBD8[1])) {
                o->unkBD2 = 1.0f + func_00191A70(ang[0] - o->unkBD8[0]) / (6144.0f / rate);
            } else {
                o->unkBD2 = 1.0f + func_00191A70(ang[1] - o->unkBD8[1]) / (8192.0f / rate);
            }
        }
        o->unkBD1 = 1;
        func_0062F790(o, ang);
        break;
    case 2:
        if (o->unkBD1 == 2 && o->unkBD2 != 0) {
            o->unkBD2--;
            o->unkBD8[0] += o->unkBD4[0];
            o->unkBD8[1] += o->unkBD4[1];
            ang[0] = o->unkBD8[0];
            ang[1] = o->unkBD8[1];
            ang[2] = 0.5f + 65536.0f * bones[0x15]->bind[1][2] / D_00239E18;
            func_0055C480(o, 0x15, ang);
            break;
        }
        tgt = *(Vec3 *)D_00477EE8;
        tgt.y += 170.0f;
        func_0062F740(o, 0x15, &from.x);
        func_00346CD0(ang, &from.x, &tgt.x);
        ang[0] = -ang[0];
        ang[1] -= o->unk92;
        o->unkBD1 = 2;
        break;
    case 4:
        if (o->unkB60 > 0) {
            o->unkBD2 = o->unkB60;
            o->unkBD2 = (D_00306590.unkC ? o->unkBD2 / 2 : o->unkBD2) + 1;
            o->unkBD1 = 4;
        }
        if (o->unkB60 != 0) {
            switch (o->unkB48) {
            case 0:
                tgt = *(Vec3 *)o->unkB54;
                break;
            case 1:
                func_0062F740(o->unkB4C, 0, &tmp.x);
                func_0062F740(o->unkB4C, 2, &tgt.x);
                tgt.y = tmp.y + o->unkB50;
                break;
            }
            func_0062F740(o, 0x15, &from.x);
            func_00190E00(m[0], bones[0x14]->world[0]);
            func_00191410(&from.x, m[0]);
            from.x += m[3][0];
            from.y += m[3][1];
            from.z += m[3][2];
            func_00191410(&tgt.x, m[0]);
            tgt.x += m[3][0];
            tgt.y += m[3][1];
            tgt.z += m[3][2];
            func_00346CD0(ang, &from.x, &tgt.x);
            ang[0] = -ang[0];
            ang[1] -= ANG(bones[0x15]->cur[1][1]);
            ang[2] = 0.5f + 65536.0f * bones[0x15]->cur[1][2] / D_00239E18;
            func_0062F790(o, ang);
            break;
        }
        func_0062F9B0(o, 0);
        break;
    case 5:
        if (o->unkBD1 != 5) {
            o->unkBD1 = 5;
            o->unkBD2 = 1;
            o->unkBD8[0] = hx;
            o->unkBD8[1] = hy;
        }
        ang[0] = hx + (s16)((f32)(o->ctrl.ly * 0x1800) / 127.0f);
        ang[1] = hy - (s16)((f32)(o->ctrl.lx << 13) / 127.0f);
        ang[2] = 0.5f + 65536.0f * bones[0x15]->bind[1][2] / D_00239E18;
        func_0062F790(o, ang);
        break;
    case 6:
        if (o->unkBD1 == 6 && o->unkBD2 != 0) {
            o->unkBD2--;
            o->unkBD8[0] += o->unkBD4[0];
            o->unkBD8[1] += o->unkBD4[1];
            ang[0] = o->unkBD8[0];
            ang[1] = o->unkBD8[1];
            ang[2] = 0.5f + 65536.0f * bones[0x15]->bind[1][2] / D_00239E18;
            func_0062F790(o, ang);
            break;
        }
        tgt.y = 0.0f;
        tgt.x = 0.0f;
        tgt.z = 0.0f;
        tgt.y += 170.0f;
        func_0062F740(o, 0x15, &from.x);
        func_00346CD0(ang, &from.x, &tgt.x);
        ang[0] = -ang[0];
        ang[1] -= o->unk92;
        if (o->unkBD1 != 6) {
            if (func_00191A70(ang[0]) > func_00191A70(ang[1])) {
                o->unkBD2 = 1.0f + func_00191A70(ang[0] - o->unkBD8[0]) / (6144.0f / rate);
            } else {
                o->unkBD2 = 1.0f + func_00191A70(ang[1] - o->unkBD8[1]) / (8192.0f / rate);
            }
        }
        o->unkBD1 = 6;
        func_0062F790(o, ang);
        break;
    }
}
#endif

void func_00633C10(DrawObjFull *o, s32 flags);
void func_00633C60(DrawObjFull *o, s32 flags);

void func_00633BC0(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_00633BC0(DrawObjFull *o) {
    s32 f = o->unkBA4;

    if (f & 0x4000) {
        func_00633C10(o, f);
        return;
    }
    if (f & 0x8000) {
        func_00633C60(o, f);
    }
}
#endif

#include "vu0.h"
void func_00340310(s16 rx, s16 ry, s16 rz, Mat44 *m);

/* Moves a character by its velocity (0x4E4, in its own frame: through its rotation and its
   x/z scale, times its speed factor). */
void func_0062CA30(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_0062CA30(DrawObjFull *o) {
    f32 s[16];
    f32 m[16];
    f32 v[4];

    func_0018F7C0(m);
    func_00340310(o->unk90, o->unk92, o->unk94, (Mat44 *)m);
    if (!(o->unk54A & 8)) {
        func_0018F830(o->unkB4[0], 1.0f, o->unkB4[2], s);
        func_00190CD0(m, s);
    }
    o->unk54A &= ~8;
    func_001913A0(v, (f32 *)o->unk4E4, m);
    v[0] *= o->unkF0;
    v[1] *= o->unkF0;
    v[2] *= o->unkF0;
    o->unk38[0] += v[0];
    o->unk38[1] += v[1];
    o->unk38[2] += v[2];
}
#endif

void func_005C5A90(DrawObjFull *o, s32 a);
void func_00635920(DrawObjFull *o);

void func_00637C60(DrawObjFull *o) {
    s32 f;

    if ((o->unkBA4 & 0x80) && (o->unkBA0 & 0x1A0)) {
        o->unkBA0 |= 0x1160;
    }
    if (o->unk1E2 != D_00306590.unk1C) {
        return;
    }
    f = o->unkBA0;
    if (!(f & 0x40)) {
        return;
    }
    if (f & 0x1000) {
        o->visible = 0;
        return;
    }
    o->visible = 1;
    if (o->act[0] != 4 || o->act[1] != 1) {
        func_005C5A90(o, 0);
        o->unk54F = 0xA0;
        o->act[0] = 4;
        o->act[1] = 1;
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
    }
}

/* The character's expression (its face's motion table, by kind or NPC model). */
extern u8 D_0023ABA0[8];        /* per character kind */
extern u8 D_0023ABA8[8];        /* (the zombie forms) */
extern u8 D_006DB4FF[];         /* per NPC model (from 1) */
extern u8 D_006DB500[];
extern u8 D_006DB5E0[];
extern void *D_006DB880[];

/* A bone link: bone's link kind (0 0xC8, 1 0xD0, 2 0xCC) to parent (0xFF none). */
typedef struct {
    u8 bone;
    u8 kind;
    u8 parent;
} BoneLink;

/* Links the face's bones (a list ended by 0xFF) to their parents. */
void func_005E9CF0(void *bones, void *face, u8 n) {
    u8 **b = bones;
    BoneLink *l = face;
    u8 *p;

    for (; l->bone != 0xFF; l++) {
        if (l->parent == 0xFF) {
            p = NULL;
        } else {
            p = b[l->parent];
        }
        switch (l->kind) {
        case 0:
            *(u8 **)(b[l->bone] + 0xC8) = p;
            break;
        case 1:
            *(u8 **)(b[l->bone] + 0xD0) = p;
            break;
        case 2:
            *(u8 **)(b[l->bone] + 0xCC) = p;
            break;
        }
    }
}

void func_005E9220(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_005E9220(DrawObjFull *o) {
    u8 n;

    if (o->active == 0 || o->visible == 0 || (o->unk54A & 0x40)) {
        return;
    }
    switch (o->type) {
    case 0:
        if (o->unkBB4 == 0) {
            if (o->unk54D == 1) {
                n = D_0023ABA8[o->unkB98];
            } else if (o->costume == 0) {
                n = D_0023ABA0[o->unkB98];
            } else {
                return;
            }
        } else {
            n = D_006DB4FF[o->unkBB4];
        }
        break;
    case 1:
        if (o->unk54D == 1) {
            n = D_006DB5E0[o->unkBB4];
        } else {
            n = D_006DB500[o->unkBB4];
        }
        break;
    default:
        return;
    }
    if (n != 0xFF) {
        func_005E9CF0((u8 *)o->model + 0x820, D_006DB880[n], n);
    }
}
#endif

/* An eye (or a pair of them) of a face: a chain of bones turned to look at a point ahead. */
typedef struct {
    u16 bones;          /* 0x0: D_006DB730's (first bone, end) */
    u16 flags;          /* 0x2: 1: clamp every bone, not just the first */
    s16 aim;            /* 0x4: D_006DB780: how far ahead */
    s16 pos;            /* 0x6: D_006DB790: the point, in the head's frame */
    s16 reach;          /* 0x8: D_006DB850: the head sphere's radius per bone */
    s16 ofs;            /* 0xA: D_006DB740 (-1: none) */
    s16 lim;            /* 0xC: D_006DB7D0: the turn's limits (-1: none) */
    s16 decay;          /* 0xE: D_0023ABB0 */
    s16 pull;           /* 0x10: D_006DB7C0 */
} EyeDef;

extern s16 D_006DB6C0[][4];     /* per face: its eyes, -1 ended */
extern EyeDef D_006DB6E0[];
extern u16 D_006DB730[][2];
extern f32 D_006DB740[][3];
extern f32 D_006DB780[];
extern f32 D_006DB790[][3];
extern f32 D_006DB7C0[];
extern f32 D_006DB7D0[][6];
extern f32 D_006DB850[][2];
void func_001918A0(f32 *dst, const f32 *src);
extern f32 D_0023ABB0[2];
void func_00340410(f32 *out, f32 *a, f32 *b);
void func_003404F0(f32 *out, const f32 *a, const f32 *b);
void func_003405D0(f32 s, f32 *out, const f32 *v);
void func_0018FAB0(f32 *out, const f32 *m);
f32 func_00191B50(f32 y, f32 x);
f32 func_00341860(f32 x);
void func_00192BE0(FlBone *b, f32 *parent);
void func_00340560(f32 *out, const f32 *a, const f32 *b);

/* How far p lies inside a sphere of radius r about c along the direction dir: when it is (or
   is behind it), out is the push along dir (dot + r) and 1. */
s32 func_006BDDF0(f32 *p, f32 *c, f32 *dir, f32 *out, f32 r) {
    f32 t[3];
    f32 n[3];
    f32 dot;

    func_00340560(t, c, p);
    func_001918A0(n, dir);
    func_00191750(n);
    dot = func_001917C0(n, t);
    if (dot <= 0.0f && !(-dot <= r)) {
        return 0;
    }
    dot += r;
    func_003405D0(dot, out, n);
    return 1;
}

/* Turns face n's eyes: each bone of each eye toward a point ahead of the head, pulled by its
   motion and drifting (0xE58), within its limits. */
void func_005E9600(DrawObjFull *o, u8 n);
#ifdef NON_MATCHING   /* stack layout and register allocation; logic follows the asm */
void func_005E9600(DrawObjFull *o, u8 n) {
    f32 m[16];
    f32 pm[16];
    f32 hm[16];
    EyeDef *defs[4];
    f32 wpos[3];
    f32 v[3];
    f32 y[3];
    f32 x[3];
    f32 out[3];
    f32 c[3];
    f32 d[3];
    f32 a[3];
    f32 tgt[3];
    f32 pos[3];
    f32 dir[3];
    s16 *l;
    s16 i;
    s16 cnt;
    s16 k;
    s16 j;
    s16 b;
    u16 first;
    s32 end;
    FlBone **bones;
    FlBone **bp;
    EyeDef **dp;
    f32 *e;
    s32 count;
    f32 rx;
    f32 ry;
    u8 *eyes;

    l = D_006DB6C0[n];
    cnt = 0;
    for (i = 0; i < 4; i++) {
        if (*l == -1) {
            break;
        }
        defs[i] = &D_006DB6E0[*l++];
        cnt++;
    }
    count = cnt;
    if (count == 0) {
        return;
    }
    bones = OBJ_BONES(o);
    func_00190ED0(hm, bones[0x15]->world[0]);
    for (k = 0; k < count; k++) {
        if (defs[k]->ofs != -1) {
            func_001918A0(c, D_006DB740[defs[k]->ofs]);
            func_00191410(c, OBJ_BONES(o)[0xA]->world[0]);
        }
        dp = &defs[k];
        a[0] = 0.0f;
        a[1] = D_006DB780[(*dp)->aim];
        a[2] = 0.0f;
        v[0] = D_006DB790[(*dp)->pos][0];
        v[1] = D_006DB790[(*dp)->pos][1];
        v[2] = D_006DB790[(*dp)->pos][2];
        func_00191410(v, hm);
        func_003404F0(a, a, v);
        eyes = (u8 *)o + k * 0x48;
        first = D_006DB730[(*dp)->bones][0];
        end = D_006DB730[(*dp)->bones][1];
        for (j = 0, b = first; b < end; b++, j++) {
            bp = &bones[b];
            func_0018FAB0(pos, (*bp)->world[0]);
            func_0018FAB0(v, (*bp)->local[0]);
            if (j == 0) {
                func_00190ED0(pm, hm);
            } else {
                func_00190C50(m, (*bp)->parent->local[0], pm);
                func_00190ED0(pm, m);
            }
            func_00191410(v, pm);
            func_003404F0(wpos, &pm[12], v);
            v[1] = -10.0f;
            v[2] = 0.0f;
            v[0] = 0.0f;
            func_001913A0(dir, v, (*bp)->world[0]);
            func_00340410(d, pos, wpos);
            func_003405D0(D_006DB7C0[(*dp)->pull], d, d);
            func_003404F0(dir, dir, d);
            func_00191820(x, dir, a);
            func_00191820(y, x, dir);
            func_00191750(y);
            func_003405D0(func_001917C0(y, a), y, y);
            e = (f32 *)(eyes + 0xE58 + j * 0x24);
            func_003404F0(e, e, y);
            func_003404F0(dir, dir, e);
            func_003404F0(tgt, wpos, dir);
            func_00190E00(m, pm);
            if ((*dp)->ofs == -1) {
                d[0] = -dir[0];
                d[1] = -dir[1];
                d[2] = -dir[2];
                func_00191410(d, m);
                func_00191750(d);
            } else {
                if (func_006BDDF0(tgt, bones[0x15]->world[3], c, out,
                                  D_006DB850[(*dp)->reach][j])) {
                    func_003404F0(tgt, tgt, out);
                }
                func_00340410(d, wpos, tgt);
                func_00191410(d, m);
                func_00191750(d);
            }
            if ((j == 0 || ((*dp)->flags & 1)) && (*dp)->lim != -1) {
                f32 *lim = D_006DB7D0[(*dp)->lim];

                if (d[0] < lim[0]) {
                    d[0] = lim[0];
                } else if (!(d[0] <= lim[1])) {
                    d[0] = lim[1];
                }
                if (d[1] < lim[2]) {
                    d[1] = lim[2];
                } else if (!(d[1] <= lim[3])) {
                    d[1] = lim[3];
                }
                if (d[2] < lim[4]) {
                    d[2] = lim[4];
                } else if (!(d[2] <= lim[5])) {
                    d[2] = lim[5];
                }
            }
            ry = func_00191B50(d[2], d[1]);
            rx = func_00191B50(-d[0], func_00341860(d[1] * d[1] + d[2] * d[2]));
            func_0018F7C0(m);
            func_0018FC10(rx, m);
            func_0018FAD0(ry, m);
            func_001918A0((*bp)->local[0], &m[0]);
            func_001918A0((*bp)->local[1], &m[4]);
            func_001918A0((*bp)->local[2], &m[8]);
            func_003405D0(D_0023ABB0[(*dp)->decay], e, e);
        }
    }
    func_00192BE0(bones[first], hm);
}
#endif

/* A capsule (a segment and its radius), as func_005E9DA0 builds a matrix for. */
typedef struct {
    f32 p0[3];
    f32 p1[3];
    f32 r;
} Capsule;

extern s8 D_0023ABE8[8];        /* Cindy's: the four bones the pairs hang from, then the four */
extern s8 D_0023ABF0[8];        /* (her zombie form) */
extern f32 D_006DB8A0[2][3];    /* where the pairs hang, from the bones */
extern f32 D_00239E98;          /* how far the first of a pair hangs */
extern f32 D_00239E9C;          /* and the second */
/* A capsule ready for tests: the segment, its radius, direction and middle, and a sphere about
   the middle that holds it all. */
typedef struct {
    f32 p0[3];          /* 0x00 */
    f32 p1[3];          /* 0x0C */
    f32 r;              /* 0x18 */
    f32 dir[3];         /* 0x1C: p1 - p0 */
    f32 mid[3];         /* 0x28 */
    f32 bound;          /* 0x34: the sphere's radius */
} CapsuleInfo;

extern f32 D_00239E88;          /* how close to the axis counts as on it */
void func_001918A0(f32 *dst, const f32 *src);

void func_005E9DA0(Capsule *c, CapsuleInfo *out) {
    func_001918A0(out->p0, c->p0);
    func_001918A0(out->p1, c->p1);
    out->r = c->r;
    out->dir[0] = c->p1[0] - c->p0[0];
    out->dir[1] = c->p1[1] - c->p0[1];
    out->dir[2] = c->p1[2] - c->p0[2];
    out->mid[0] = 0.5f * (c->p0[0] + c->p1[0]);
    out->mid[1] = 0.5f * (c->p0[1] + c->p1[1]);
    out->mid[2] = 0.5f * (c->p0[2] + c->p1[2]);
    out->bound = func_001916E0(c->p0, out->mid);
    out->bound += c->r;
}

/* Whether spheres (a, ra) and (b, rb) touch. */
u8 func_005E9E80(f32 *a, f32 *b, f32 ra, f32 rb) {
    f32 r = ra + rb;
    f32 dx = a[0] - b[0];
    f32 dy = a[1] - b[1];
    f32 dz = a[2] - b[2];

    if (dx * dx + dy * dy + dz * dz <= r * r) {
        return 1;
    }
    return 0;
}

/* Whether a sphere at p of radius r touches capsule c; if so out is where p is pushed out to
   (c's radius from its axis, or from the end it is past). */
s32 func_005E9EE0(f32 *p, CapsuleInfo *c, f32 *out, f32 r);
#ifdef NON_MATCHING   /* register allocation (radius and distance swap f20/f21) */
s32 func_005E9EE0(f32 *p, CapsuleInfo *c, f32 *out, f32 r) {
    f32 dist;
    f32 n[3];
    f32 t[3];
    f32 u[3];
    f32 w[3];
    f32 q[3];
    f32 e[3];
    u8 hit;

    hit = func_005E9E80(p, c->mid, r, c->bound);
    if (!hit) {
        return 0;
    }
    func_001918A0(n, c->dir);
    t[0] = c->p0[0] - p[0];
    t[1] = c->p0[1] - p[1];
    t[2] = c->p0[2] - p[2];
    func_00191820(u, n, t);
    func_00191820(w, n, u);
    func_00191750(w);
    dist = func_001917C0(w, t);
    r += c->r;
    if (!(func_00191A70(dist) <= r)) {
        return 0;
    }
    u[0] = dist * w[0];
    u[1] = dist * w[1];
    u[2] = dist * w[2];
    q[0] = p[0] + u[0];
    q[1] = p[1] + u[1];
    q[2] = p[2] + u[2];
    t[0] = q[0] - c->p0[0];
    t[1] = q[1] - c->p0[1];
    t[2] = q[2] - c->p0[2];
    if (func_001917C0(t, n) < 0.0f) {
        if (!(func_001916E0(c->p0, p) <= r)) {
            return 0;
        }
        t[0] = p[0] - c->p0[0];
        t[1] = p[1] - c->p0[1];
        t[2] = p[2] - c->p0[2];
        func_00191750(t);
        t[0] *= c->r;
        t[1] *= c->r;
        t[2] *= c->r;
        out[0] = c->p0[0] + t[0];
        out[1] = c->p0[1] + t[1];
        out[2] = c->p0[2] + t[2];
        return 1;
    }
    t[0] = q[0] - c->p1[0];
    t[1] = q[1] - c->p1[1];
    t[2] = q[2] - c->p1[2];
    n[0] = -n[0];
    n[1] = -n[1];
    n[2] = -n[2];
    if (func_001917C0(t, n) < 0.0f) {
        if (!(func_001916E0(c->p1, p) <= r)) {
            return 0;
        }
        t[0] = p[0] - c->p1[0];
        t[1] = p[1] - c->p1[1];
        t[2] = p[2] - c->p1[2];
        func_00191750(t);
        t[0] *= c->r;
        t[1] *= c->r;
        t[2] *= c->r;
        out[0] = c->p1[0] + t[0];
        out[1] = c->p1[1] + t[1];
        out[2] = c->p1[2] + t[2];
        return 1;
    }
    if (func_00191A70(dist) < D_00239E88) {
        /* On the axis: any direction across it will do. */
        func_001918A0(e, q);
        e[0] += 5.0f;
        t[0] = e[0] - c->p1[0];
        t[1] = e[1] - c->p1[1];
        t[2] = e[2] - c->p1[2];
        func_00191820(u, t, n);
        if (func_001917C0(u, u) < D_00239E88) {
            q[1] += 5.0f;
            t[0] = e[0] - c->p1[0];
            t[1] = e[1] - c->p1[1];
            t[2] = e[2] - c->p1[2];
            func_00191820(u, t, n);
        }
        func_00191820(t, u, n);
        func_00191750(t);
    } else {
        t[0] = p[0] - q[0];
        t[1] = p[1] - q[1];
        t[2] = p[2] - q[2];
        func_00191750(t);
    }
    t[0] *= c->r;
    t[1] *= c->r;
    t[2] *= c->r;
    out[0] = q[0] + t[0];
    out[1] = q[1] + t[1];
    out[2] = q[2] + t[2];
    return 1;
}
#endif

/* Cindy's hanging bones (kind 7 in her first costume): two pairs, each hung from two points on
   bones 0 and 2 toward the matching points on the other one, and kept out of the capsules
   between bones 0-1 and 2-3. */
void func_005EA470(DrawObjFull *o);
#ifdef NON_MATCHING   /* register allocation (the original spills bone 2 to the stack) */
void func_005EA470(DrawObjFull *o) {
    f32 d[3];
    f32 v[3];
    FlBone *a[4];
    FlBone *b[4];
    f32 p[4][3];
    Capsule cap;
    CapsuleInfo m0;
    CapsuleInfo m1;
    s32 i;
    s8 *ids;
    s32 j;

    if (o->unkB98 == 7 && o->costume == 0 && o->unkBB4 == 0) {
        ids = D_0023ABE8;
        if (o->unk54D == 1) {
            ids = D_0023ABF0;
        }
        for (i = 0; i < 4; i++) {
            a[i] = ((FlBone **)((Slot1468 *)o->model)->unk820)[ids[i]];
            b[i] = ((FlBone **)((Slot1468 *)o->model)->unk820)[ids[i + 4]];
        }
        for (i = 0; i < 2; i++) {
            for (j = 0; j < 2; j++) {
                func_001913A0(p[j + i * 2], D_006DB8A0[j], a[i * 2]->world[0]);
                p[j + i * 2][0] += a[i * 2]->world[3][0];
                p[j + i * 2][1] += a[i * 2]->world[3][1];
                p[j + i * 2][2] += a[i * 2]->world[3][2];
            }
        }
        v[0] = 0.0f;
        cap.r = 10.5f;
        v[1] = -100.0f;
        v[2] = 0.0f;
        func_001918A0(cap.p0, a[0]->world[3]);
        func_001918A0(cap.p1, a[1]->world[3]);
        func_005E9DA0(&cap, &m0);
        func_001918A0(cap.p0, a[2]->world[3]);
        func_001918A0(cap.p1, a[3]->world[3]);
        func_005E9DA0(&cap, &m1);
        for (i = 0; i < 2; i++) {
            d[0] = p[i + 2][0] - p[i][0];
            d[1] = p[i + 2][1] - p[i][1];
            d[2] = p[i + 2][2] - p[i][2];
            func_00191750(d);
            v[0] = D_00239E98 * d[0];
            v[1] = D_00239E98 * d[1];
            v[2] = D_00239E98 * d[2];
            func_00190ED0(b[i]->world[0], a[0]->world[0]);
            b[i]->world[3][0] = v[0] + p[i][0];
            b[i]->world[3][1] = v[1] + p[i][1];
            b[i]->world[3][2] = v[2] + p[i][2];
            if (func_005E9EE0(b[i]->world[3], &m0, v, 1.0f)) {
                b[i]->world[3][0] = v[0];
                b[i]->world[3][1] = v[1];
                b[i]->world[3][2] = v[2];
            }
            v[0] = D_00239E9C * d[0];
            v[1] = D_00239E9C * d[1];
            v[2] = D_00239E9C * d[2];
            func_00190ED0(b[i + 2]->world[0], a[2]->world[0]);
            b[i + 2]->world[3][0] = v[0] + p[i + 2][0];
            b[i + 2]->world[3][1] = v[1] + p[i + 2][1];
            b[i + 2]->world[3][2] = v[2] + p[i + 2][2];
            if (func_005E9EE0(b[i + 2]->world[3], &m1, v, 1.0f)) {
                b[i + 2]->world[3][0] = v[0];
                b[i + 2]->world[3][1] = v[1];
                b[i + 2]->world[3][2] = v[2];
            }
        }
    }
}
#endif

extern void *D_006DB890[];      /* the faces' other links (by the same number) */

/* Sets up the character's face for its expression table (as func_005E9220, with the other
   links; online players by 0xFAC, the zombie forms by 0x54D). */
void func_005E9390(DrawObjFull *o);
#ifdef NON_MATCHING   /* the early returns' branch layout */
void func_005E9390(DrawObjFull *o) {
    u8 n;
    u8 k;

    if (o->active != 0 && o->visible != 0 && !(o->unk54A & 0x40)) {
        if (D_0023BB98 == 1 && D_0048BF88[0] == 0) {
            k = 0;
            switch (o->unkFAC) {
            case 0:
            case 1:
            case 3:
                k = 0;
                break;
            case 4:
                break;
            default:
                k = 0xFF;
                break;
            }
        } else {
            k = o->type;
        }
        switch (k) {
        case 0:
            k = 1;
            if (o->unkBB4 == 0) {
                k = (o->unk54D == 1) ? 3 : 0;
            }
            switch (k) {
            case 0:
                func_005EA470(o);
                n = D_0023ABA0[o->unkB98];
                if (n == 0xFF || o->costume != 0) {
                    return;
                }
                break;
            case 1:
                n = D_006DB500[o->unkB98];
                if (n == 0xFF) {
                    return;
                }
                break;
            case 3:
                func_005EA470(o);
                n = D_0023ABA8[o->unkB98];
                if (n == 0xFF) {
                    return;
                }
                break;
            case 4:
                n = D_006DB5E0[o->unkB98];
                if (n == 0xFF) {
                    return;
                }
                break;
            }
            break;
        case 1:
            if (o->unk54D == 1) {
                n = D_006DB5E0[o->unkBB4];
            } else {
                n = D_006DB500[o->unkBB4];
            }
            if (n == 0xFF) {
                return;
            }
            break;
        default:
            return;
        }
        func_005E9600(o, n);
        func_005E9CF0((u8 *)o->model + 0x820, D_006DB890[n], n);
    }
}
#endif

#include "zone.h"

extern u8 D_00470090[0x6D40];
s32 func_00340940(Zone *z, s32 *xz, s32 n);
void func_005910F0(Zone *z);

/* The floor switches (zones of kind 0x85 whose switch is armed) a character steps on. */
void func_005516D0(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_005516D0(DrawObjFull *o) {
    s32 xz[2];
    Zone *z;
    f32 top;
    f32 y;
    s32 hit;

    if (D_0023BD88 == 0x80) {
        return;
    }
    z = D_0023BD90;
    xz[0] = o->unk38[0];
    xz[1] = o->unk38[2];
    for (; z != NULL; z = z->next) {
        if (z->kind != 0x85) {
            continue;
        }
        if (D_00470090[0x4731 + z->u.def[0x10] * 0x60] != 2 || func_00340940(z, xz, 4) != 1) {
            continue;
        }
        top = z->top;
        y = o->unk38[1];
        hit = 0;
        if (!(top <= y)) {
            if (top <= y + 200.0f) {
                hit = 1;
            }
        } else if (top < y) {
            if (!(z->bottom < y)) {
                hit = 1;
            }
        } else {
            hit = 1;
        }
        if (hit) {
            z->who = o;
            func_005910F0(z);
        }
    }
}
#endif

#include "item.h"

s32 func_00340DB0(Zone *z, s32 *xz, s32 n, f32 r);
s32 func_005924E0(Zone *z, DrawObjFull *o);
s32 func_0067E9C0(DrawObjFull *o, u8 item);

/* (Item zones, kind 0x81) whether o sets the zone off: anyone but a player does; a player
   when func_0067E9C0 says so for the zone's item. */
s32 func_005953D0(Zone *z, DrawObjFull *o) {
    if (o->type != 0) {
        return 1;
    }
    return func_0067E9C0(o, ((ItemObj *)z->u.def)->index) != 0;
}

/* Sets off the trigger zone (the lowest unk32 of those it is in, within 200 below to its
   bottom) that who of the given kind (0 player, 2 ...) at pos sets off; r is the reach for
   the round ones (0x80, 0x81). */
void func_005ABBE0(f32 r, f32 *pos, DrawObjFull *o, s32 kind);
#ifdef NON_MATCHING   /* close: s-register choices; one store through a pointer */
void func_005ABBE0(f32 r, f32 *pos, DrawObjFull *o, s32 kind) {
    u8 k = kind;
    s32 xz[2];
    s32 oxz[2];
    Zone *best = NULL;
    Zone *z = D_0023BD90;
    u8 *slot = &o->unkC82;
    f32 *ox = &o->unk38[0];
    f32 *oz = &o->unk38[2];
    s32 hit;
    f32 top;
    f32 y;

    xz[0] = pos[0];
    xz[1] = pos[2];
    while (z != NULL) {
        if (!z->on) {
            z = z->next;
            continue;
        }
        if (z->unk3D) {
            z = z->next;
            continue;
        }
        switch (z->unk31) {
        case 4:
            if (k == 0 || k == 2 || k == 5 || k == 6 || k == 4) {
                break;
            }
            z = z->next;
            continue;
        case 2:
            if (z->unk31 != k) {
                z = z->next;
                continue;
            }
            if (z->unk48 != D_003967D0[*slot].unk6) {
                z = z->next;
                continue;
            }
            break;
        case 6:
            if (k == 2 || k == 6) {
                if (D_003967D0[*slot].unk6 >= 0x19E && D_003967D0[*slot].unk6 < 0x1A2) {
                    break;
                }
            }
            z = z->next;
            continue;
        default:
            if (z->unk31 != k) {
                z = z->next;
                continue;
            }
            break;
        }
        switch (z->kind) {
        case 2:
            if (FIELD(z, s32, 0x58) != 2 && D_00470090[0x4731 + z->u.id * 0x60] != 0) {
                z = z->next;
                continue;
            }
            if (!func_00340940(z, xz, 4)) {
                z = z->next;
                continue;
            }
            break;
        case 1:
            if (!func_005924E0(z, o)) {
                z = z->next;
                continue;
            }
            if (!func_00340940(z, xz, 4)) {
                z = z->next;
                continue;
            }
            break;
        case 0x80:
            if (!func_00340DB0(z, xz, 4, r)) {
                z = z->next;
                continue;
            }
            break;
        case 0x81:
            if (!func_005953D0(z, o)) {
                z = z->next;
                continue;
            }
            if (!func_00340DB0(z, xz, 4, r)) {
                z = z->next;
                continue;
            }
            break;
        case 0x85:
            oxz[0] = *ox;
            oxz[1] = *oz;
            if (!func_00340940(z, oxz, 4)) {
                z = z->next;
                continue;
            }
            break;
        default:
            if (!func_00340940(z, xz, 4)) {
                z = z->next;
                continue;
            }
            break;
        }
        top = z->top;
        y = pos[1];
        hit = 0;
        if (!(top <= y)) {
            if (top <= y + 200.0f) {
                hit = 1;
            }
        } else if (top < y) {
            if (!(z->bottom < y)) {
                hit = 1;
            }
        } else {
            hit = 1;
        }
        if (hit) {
            if (best != NULL) {
                if (z->unk32 < best->unk32) {
                    best = z;
                }
            } else {
                best = z;
            }
        }
        z = z->next;
    }
    if (best != NULL) {
        best->who = o;
        best->unk3E = kind;
        func_005910F0(best);
    }
}
#endif

f32 func_00341860(f32 x);
s16 func_00346F20(s32 from, s32 to, s32 n);

/* The step that moves o to to in frames frames (none when it's further than 500: it is put
   there). */
void func_00631F60(DrawObjFull *o, f32 *to, f32 *step, u32 frames);
#ifdef NON_MATCHING   /* close: the length's two loads go through the pointers */
void func_00631F60(DrawObjFull *o, f32 *to, f32 *step, u32 frames) {
    f32 d[3];
    f32 *dy;
    f32 *dz;

    if (frames == 0) {
        step[0] = 0.0f;
        step[1] = 0.0f;
        step[2] = 0.0f;
        return;
    }
    dy = &d[1];
    dz = &d[2];
    d[0] = to[0] - o->unk38[0];
    *dy = to[1] - o->unk38[1];
    *dz = to[2] - o->unk38[2];
    if (!(func_00341860(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]) <= 500.0f)) {
        o->unk38[0] = to[0];
        o->unk38[1] = to[1];
        o->unk38[2] = to[2];
        d[0] = 0.0f;
        *dy = 0.0f;
        *dz = 0.0f;
    }
    d[0] /= (f32)frames;
    *dy /= (f32)frames;
    *dz /= (f32)frames;
    *(Vec3 *)step = *(Vec3 *)d;
}
#endif

/* The turn per frame from o's heading to to over frames frames. */
void func_00632140(DrawObjFull *o, s16 *to, s16 *step, u32 frames);
#ifdef NON_MATCHING   /* close: the two argument loads in the other order */
void func_00632140(DrawObjFull *o, s16 *to, s16 *step, u32 frames) {
    s16 t;

    if (frames == 0) {
        *step = 0;
        return;
    }
    t = *to;
    *step = func_00346F20(o->unk92, t, frames);
}
#endif

/* Sets up o's push (0xD74 and 0xD8C over 0xD90 frames). */
void func_00632180(DrawObjFull *o) {
    func_00631F60(o, o->unkD74, o->unkD80, o->unkD90);
    func_00632140(o, &o->unkD8C, &o->unkD8E, o->unkD90);
}

void func_00340210(s32 rx, s32 ry, s32 rz, f32 *m);
s32 func_00633D80(DrawObjFull *o);

/* The character's world matrix (at 0x50) from its angles and position, its facing (0xA8), and
   the frame's pushes (0xD8 in its own frame, 0xE4 in the world's) applied. */
void func_0062CB30(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_0062CB30(DrawObjFull *o) {
    f32 r[16];
    f32 z[4];
    f32 d[4];
    f32 *m = (f32 *)((u8 *)o + 0x50);

    func_0018F7C0(m);
    func_0018F7C0(r);
    func_00340310(0, o->unk92, 0, (Mat44 *)r);
    z[2] = 1.0f;
    z[0] = 0.0f;
    z[1] = 0.0f;
    func_001913A0((f32 *)&o->unkA8, z, r);
    func_00340210(o->unk90, o->unk92, o->unk94, m);
    func_001913A0(d, o->unkD8, m);
    o->unk38[0] += d[0] + o->unkE4[0];
    o->unk38[1] += d[1] + o->unkE4[1];
    o->unk38[2] += d[2] + o->unkE4[2];
    o->unkD8[2] = 0.0f;
    o->unkD8[1] = 0.0f;
    o->unkD8[0] = 0.0f;
    o->unkE4[2] = 0.0f;
    o->unkE4[1] = 0.0f;
    o->unkE4[0] = 0.0f;
    func_00633D80(o);
    m[12] = o->unk38[0];
    m[13] = o->unk38[1];
    m[14] = o->unk38[2];
}
#endif

u16 func_001ADEF0(s32 n);
void func_00634260(DrawObjFull *o, s32 a);
s32 func_0062E5B0(DrawObjFull *o, u16 dmg, s32 a);
extern s8 D_0048BF88[];
s32 func_0062E880(DrawObjFull *o);
void func_0062EBF0(DrawObjFull *o, s32 cond);
extern u8 D_0048BF92[];
extern u8 D_0048BF93[];

/* Bleeding (0xBA0 & 1: 1 damage every 20 frames) and poison (& 2: 5 every 60, for a random
   40-150 seconds, then it wears off), while above 1 health; 0x18 cleared while alive. Counts
   are halved at 30 fps (Options.unkC). */
void func_0062E950(DrawObjFull *o) {
    u16 bleed = D_00306590.unkC ? 10 : 20;
    u16 hurt = D_00306590.unkC ? 30 : 60;
    s32 lo = D_00306590.unkC ? 1200 : 2400;
    s32 hi = D_00306590.unkC ? 4500 : 9000;
    u16 last = (D_00306590.unkC ? 1200 : 2400) + (u16)((func_001ADEF0(1) & 0xFFFF) % (u16)(hi - lo));

    if (D_0048BF88[0] != 0) {
        return;
    }
    if (o->unkBA0 & 0x400000) {
        return;
    }
    if ((o->unkBA0 & 0x18) && o->unk544 != 0) {
        o->unkBA0 &= ~0x18;
    }
    if ((o->unkBA0 & 1) && o->unk544 > 1) {
        if (D_0048BF92[0] % 4 == 0 && D_0048BF93[0] == 0) {
            func_00634260(o, 6);
        }
        if (o->unkC60 > bleed) {
            o->unkC60 = bleed;
        }
        if (o->unkC60 != 0) {
            if (--o->unkC60 == 0) {
                func_0062E5B0(o, 1, 0);
            }
        } else {
            o->unkC60 = bleed;
        }
    }
    if (o->unkBA0 & 2) {
        if (o->unkC5A > last) {
            o->unkC5A = last;
        }
        if (o->unkC5A == 0) {
            return;
        }
        if (o->unk544 > 1) {
            if (o->unkC58 > hurt) {
                o->unkC58 = hurt;
            }
            if (o->unkC58 != 0) {
                if (--o->unkC58 == 0 && func_0062E880(o)) {
                    func_0062E5B0(o, 5, 0);
                    if (o->unk544 == 0) {
                        o->unk544 = 1;
                    }
                    func_00634260(o, 6);
                }
            } else {
                o->unkC58 = hurt;
            }
        }
        if (--o->unkC5A == 0 || o->unk544 < 2) {
            o->unkC5A = 0;
            o->unkC58 = 0;
            func_0062EBF0(o, 3);
        }
    }
}
s32 func_001AF540(DrawObjFull *o);
s32 func_006AC4F0(DrawObjFull *o);
void func_005C4260(void);
void func_0062E2D0(DrawObjFull *o);
void func_0062E2F0(DrawObjFull *o, s32 a);
void func_00636440(DrawObjFull *o, u32 from, u32 to);
void func_0034B7D0(s32 a, s32 b, s32 c);
void func_006AC7D0(void);
void func_0082C640(DrawObjFull *o);     /* SINGLE.BIN */
extern u32 D_006E6C70[];        /* per character: the virus count that is 100% */
extern u32 D_006E6C90[];        /* per character: its virus per second */
extern f32 D_006E6BF0[];        /* (NPC models, by unkBBE & 7) its rate */
extern f32 D_00239DC8;          /* the % at which the virus is full */
extern s8 D_0048BF88[];
extern u8 D_0048BF7D[];
extern u8 D_0048BF93[];

/* Our player's virus (0xBAC) for this frame: it grows by the character's rate once a second
   (every frame, faster, while 0x10 or the character's grapple holds it), less what the three
   anti-virus timers (0xBB6-0xBBA, set by 0xBA4 8, 0x10, 0x20) hold back. At 100% it turns:
   1 when it did. */
s32 func_006359D0(DrawObjFull *o);
#ifdef NON_MATCHING   /* close: the rate's live ranges get other registers; daddiu for the step */
s32 func_006359D0(DrawObjFull *o) {
    f32 cur = (f32)(u32)o->unkBAC;
    f32 hold1 = 100.0f;
    f32 rate = 0.0f;
    u32 step = 60;
    f32 hold2 = hold1;
    u32 add = D_006E6C90[o->unkB98];
    f32 hold3 = hold1;
    f32 t;
    u32 max;

    if (D_0048BF88[0] != 0) {
        return 0;
    }
    if (o->type != 0) {
        return 0;
    }
    if (o->kind != D_0048BF7D[0]) {
        return 0;
    }
    if (o->unkBA0 & 0x400000) {
        return 0;
    }
    if (o->unkBA0 & 0x10) {
        rate = 60.0f;
    }
    if (o->unkB98 == 2 && o->unk54F == 0x41 && o->act[2] >= 3) {
        rate = 10.0f;
    }
    if (o->unkBB4 != 0) {
        rate *= D_006E6BF0[o->unkBBE & 7];
    }
    if (rate != 0.0f) {
        if (func_001AF540(o) == 1) {
            func_006AC7D0();
        }
        add = (f32)add * rate / (f32)(D_00306590.unkC ? 30 : 60);
        step = 2;
    } else if (D_0048BF93[0] != 0) {
        return 0;
    }
    if (o->unkBA0 & 0x1E0) {
        o->unkBB6 = 0;
        o->unkBB8 = 0;
        o->unkBBA = 0;
        return 0;
    }
    if (o->unkBA0 & 0x400) {
        o->unkBB6 = 0;
        o->unkBB8 = 0;
        o->unkBBA = 0;
    }
    if (o->unkBA4 & 0x80000) {
        o->unkBA4 &= ~0x80000;
        return 0;
    }
    if (o->unkBA4 & 8) {
        o->unkBA4 &= ~8;
        o->unkBB6 = 5400;
    }
    if (o->unkBB6 != 0) {
        t = (f32)o->unkBB6 - (f32)step;
        hold1 = 0.0f;
        if (t <= hold1) {
            t = hold1;
        }
        o->unkBB6 = t;
    }
    add = (f32)add * (hold1 / 100.0f);
    if (o->unkBA4 & 0x10) {
        o->unkBA4 &= ~0x10;
        o->unkBB8 = 10800;
    }
    if (o->unkBB8 != 0) {
        t = (f32)o->unkBB8 - (f32)step;
        hold2 = 0.0f;
        if (t <= hold2) {
            t = hold2;
        }
        o->unkBB8 = t;
    }
    add = (f32)add * (hold2 / 100.0f);
    if (o->unkBA4 & 0x20) {
        o->unkBA4 &= ~0x20;
        o->unkBBA = 1800;
    }
    if (o->unkBBA != 0) {
        t = (f32)o->unkBBA - (f32)step;
        hold3 = 0.0f;
        if (t <= hold3) {
            t = hold3;
        }
        o->unkBBA = t;
    }
    add = (f32)add * (hold3 / 100.0f);
    cur += (f32)add;
    max = D_006E6C70[o->unkB98];
    if (!(100.0f * (cur / (f32)max) < D_00239DC8)) {
        cur = (f32)max;
    }
    if (!(cur < (f32)max)) {
        if (func_001AF540(o) == 1 && !func_006AC4F0(o)) {
            func_005C4260();
        }
        func_0062E2D0(o);
        o->unkBAC = D_006E6C70[o->unkB98];
        if (!(o->unkBA4 & 0x100)) {
            return 0;
        }
        o->unk544 = 0;
        if (func_001AF540(o) == 1 && !(o->unkBA4 & 0x10000000)) {
            o->unkBA4 |= 0x10000000;
            func_0034B7D0(0x24, 0, 0);
        }
        if (o->unkBA0 & 0x18) {
            o->unkBA0 &= ~0x18;
            switch (o->type) {
            case 0:
                if (o->isNpc && o->unk1E2 != D_00306590.unk1C) {
                    o->unkBA0 |= 0x60;
                    func_005C5A90(o, 0);
                    o->unk54F = 0xA0;
                    o->act[0] = 4;
                    o->act[1] = 1;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                    func_0082C640(o);
                } else {
                    o->unkBA0 |= 0x20;
                    if (func_001AF4D0() == 1) {
                        o->unkBA0 |= 0x40000000;
                    }
                    func_005C5A90(o, 0);
                    o->unk54F = 0xA0;
                    o->act[0] = 4;
                    o->act[1] = 0;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                }
                break;
            case 1:
                o->unkBA0 |= 0x40;
                func_005C5A90(o, 0);
                o->unk54F = 0xA0;
                o->act[0] = 4;
                o->act[1] = 1;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                break;
            }
        } else {
            func_0062E2F0(o, 2);
            if (o->type == 0 && o->isNpc && o->unk1E2 != D_00306590.unk1C) {
                func_0082C640(o);
            }
        }
        return 1;
    }
    add = cur;
    func_00636440(o, o->unkBAC, add);
    o->unkBAC = add;
    return 0;
}
#endif
s32 func_006AC490(void);
s32 func_00585EF0(DrawObjFull *o);

/* func_00585EF0 for o, unless it's ours and the map's request is on, or in motion 0x15. */
void func_00636CF0(DrawObjFull *o) {
    if ((func_001AF540(o) != 1 || !func_006AC490()) && o->unk54F != 0x15) {
        func_00585EF0(o);
    }
}

/* Infected for good (0xBA4 & 0x1000): no virus gauge, and once dying (0xBA0 & 0x18) it turns
   after 3 (6 at 60 fps) minutes. */
void func_00636D50(DrawObjFull *o) {
    if ((o->unkBA4 & 0x1000) && o->act[0] != 4) {
        o->unkBAC = 0;
        if (o->unkBA0 & 0x18) {
            if (o->unkC62 != 0) {
                if (--o->unkC62 == 0) {
                    o->unkBA0 |= 0x40000000;
                    func_005C5A90(o, 0);
                    o->unk54F = 0xA0;
                    o->act[0] = 4;
                    o->act[1] = 0;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                }
            } else {
                o->unkC62 = D_00306590.unkC ? 10800 : 21600;
            }
        }
    }
}

/* (Offline) a player not in our scenario: kept still. */
void func_00637B40(DrawObjFull *o) {
    o->unkBA4 |= 0x100;
    func_0062E950(o);
    func_006359D0(o);
    func_00636CF0(o);
    func_00636D50(o);
    func_00637C60(o);
}

s32 func_0062CDE0(u8 item);
void func_0062CF10(DrawObjFull *o, u8 item);
s8 func_00582220(u8 item, DrawObjFull *o);
void func_001B7310(DrawObjFull *o, s32 a);

/* The item in hand's per-frame work (bit 31 of unkBA0 asked for it). */
void func_0062D2C0(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_0062D2C0(DrawObjFull *o) {
    u8 item = o->unkC7C;

    o->unkBA0 &= 0x7FFFFFFF;
    if (item == 0) {
        return;
    }
    if (o->type == 0 && o->kind == D_0048BF60.unk1D) {
        if (func_0062CDE0(item) != 0) {
            func_0062CF10(o, item);
        }
        return;
    }
    if (o->type == 1) {
        switch (func_00582220(item, o)) {
        case 0:
            func_001B7310(o, 1);
            return;
        case 1:
            if (func_0062CDE0(item) != 0) {
                func_0062CF10(o, item);
            }
            break;
        }
        return;
    }
    func_0062CF10(o, item);
}
#endif

/* Requests for this frame (32 at most): a character and what to do for it. */
typedef struct {
    s16 b;              /* 0x0 */
    s16 a;              /* 0x2 */
    DrawObjFull *o;     /* 0x4 */
} CharRequest;

extern CharRequest D_00386650[32];
extern s16 D_0023BD94;          /* how many */

/* Queues request (a, b) for o. */
void func_005A9CC0(DrawObjFull *o, s32 a, s32 b) {
    if (D_0023BD94 < 32) {
        D_00386650[D_0023BD94].o = o;
        D_00386650[D_0023BD94].b = b;
        D_00386650[D_0023BD94].a = a;
        D_0023BD94++;
    }
}

extern Vec3 D_006DEC28;         /* the ground samples: ahead */
extern Vec3 D_006DEC38;         /* ahead to the left */
extern Vec3 D_006DEC48;         /* ahead to the right */
extern Vec3 D_006DEC58;         /* up */
extern f32 D_00239CD0;          /* 0.1 */
void func_00191590(f32 *v, f32 angle);
f32 func_005E5B40(const f32 *p, u8 *kind, s32 want);
f32 func_00340100(s16 a);

/* Tilts o to the ground under it (when 0x548 bit 0 is on; players and NPCs start it when 0xBA0
   bit 3 is set): three floor heights around it give the slope, and the matrix is blended toward
   it by unkB90 (which runs 0-1 by unkB94). Gives up on steps over 80 or slopes too steep. */
#ifdef NON_MATCHING   /* first draft: stack layout and the type branch */
s32 func_00633D80(DrawObjFull *o) {
    f32 m2[16];
    f32 m[16];
    Vec3 up;
    Vec3 n;
    Vec3 a = D_006DEC28;
    Vec3 b = D_006DEC38;
    Vec3 c = D_006DEC48;
    Vec3 pc;
    Vec3 pb;
    Vec3 pa;
    u8 kind;

    if (o->type == 1 || o->type == 0) {
        if ((o->unkBA0 & 8) && !(o->unk548 & 1)) {
            o->unk548 |= 1;
            o->unkB94 = D_00239CD0;
        }
        if (!(o->unk548 & 1)) {
            return 1;
        }
    } else if (!(o->unk548 & 1)) {
        return 1;
    }
    o->unkB90 += o->unkB94;
    if (o->unkB94 > 0.0f) {
        if (o->unkB90 > 1.0f) {
            o->unkB90 = 1.0f;
        }
    } else if (o->unkB90 < 0.0f) {
        o->unkB90 = 0.0f;
    }
    func_00191590(&a.x, D_00239E18 * (u16)o->unk92 / 65536.0f);
    func_00191590(&b.x, D_00239E18 * (u16)(o->unk92 - 0x71C) / 65536.0f);
    func_00191590(&c.x, D_00239E18 * (u16)(o->unk92 + 0x71C) / 65536.0f);
    pa.x = a.x + o->unk38[0];
    pa.y = a.y + o->unk38[1];
    pa.z = a.z + o->unk38[2];
    pb.x = b.x + o->unk38[0];
    pb.y = b.y + o->unk38[1];
    pb.z = b.z + o->unk38[2];
    pc.x = c.x + o->unk38[0];
    pc.y = c.y + o->unk38[1];
    pc.z = c.z + o->unk38[2];
    pa.y = func_005E5B40(&pa.x, &kind, 0xFF);
    pb.y = func_005E5B40(&pb.x, &kind, 0xFF);
    pc.y = func_005E5B40(&pc.x, &kind, 0xFF);
    b.x = pb.x - pa.x;
    b.y = pb.y - pa.y;
    b.z = pb.z - pa.z;
    c.x = pc.x - pa.x;
    c.y = pc.y - pa.y;
    c.z = pc.z - pa.z;
    func_00191820(&n.x, &b.x, &c.x);
    if (func_00191A70(pa.y - pc.y) <= 80.0f && func_00191A70(pa.y - pb.y) <= 80.0f) {
        up = D_006DEC58;
        if (func_001917C0((f32 *)((u8 *)o + 0x178), &up.x) <= func_00340100(0x100)) {
            func_00191110(m, &n.x, (f32 *)&o->unkA8);
            func_00190ED0(m2, (f32 *)((u8 *)o + 0x50));
            m[14] = 0.0f;
            m[13] = 0.0f;
            m[12] = 0.0f;
            m2[14] = 0.0f;
            m2[13] = 0.0f;
            m2[12] = 0.0f;
            func_001912D0(1.0f - o->unkB90, o->unkB90, (f32 *)((u8 *)o + 0x50), m2, m);
            return 0;
        }
    }
    o->unk548 &= ~1;
    o->unkB90 = 0.0f;
    return 0;
}
#endif

/* Action 1's step tables (0x20 bytes per set, by func_0061C490's a). */
typedef struct {
    u16 s[16];          /* 1, 2 turning (left, right); 3 forward; 4, 5 turning on the spot (or
                           walking: 8 and 4); 6 running; 9 backing away (5: while aiming); 7, 8,
                           10, 11, 13, 14 turning in the other motions */
} StepSet;

typedef struct {
    s16 v[4];
} Rot4;

extern StepSet D_006DD930[];   /* [0] on ordinary floors, [1] on kind 3 */

void func_00635920(DrawObjFull *o);

/* Walking from one kind of floor onto the other: a character in action 1 (and without 0x54A
   bit 1) whose step is in the other floor's table takes the same step from this one's (set 1 on
   kind 3). Stepping onto kind 3 with no such step starts step 0's; when hit (0xBA0 0x88) it goes
   back to where it was instead. */
void func_00634670(DrawObjFull *o);
#ifdef NON_MATCHING   /* delay slots and the loops' index masking differ */
void func_00634670(DrawObjFull *o) {
    Vec3 p;
    u8 kind;
    u8 was;
    u8 i;

    if (!(o->unk54A & 2) && o->act[0] == 1) {
        p = *(Vec3 *)o->unk38;
        p.y += 50.0f;
        was = o->unk174;
        func_005E5B40(&p.x, &kind, 0xFF);
        switch (was) {
        case 2:
        case 0:
        case 1:
            switch (kind) {
            case 2:
            case 0:
            case 1:
                for (i = 0; i < 16; i++) {
                    if (D_006DD930[1].s[i] == o->act[1]) {
                        break;
                    }
                }
                if (i == 16) {
                    return;
                }
                o->act[0] = 1;
                o->act[1] = D_006DD930[0].s[i];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                o->unkCE2 |= 0x20;
                return;
            case 3:
                if (o->unkBA0 & 0x88) {
                    *(Vec3 *)o->unk38 = *(Vec3 *)o->unk44;
                    *(Rot4 *)&o->unk90 = *(Rot4 *)o->unkA0;
                    return;
                }
                for (i = 0; i < 16; i++) {
                    if (o->act[1] == D_006DD930[0].s[i]) {
                        break;
                    }
                }
                if (i == 16) {
                    i = 0;
                }
                o->act[0] = 1;
                o->act[1] = D_006DD930[1].s[i];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                o->unkCE2 |= 0x20;
                return;
            }
            return;
        case 3:
            switch (kind) {
            case 2:
            case 0:
            case 1:
                for (i = 0; i < 16; i++) {
                    if (D_006DD930[1].s[i] == o->act[1]) {
                        break;
                    }
                }
                if (i == 16) {
                    i = 0;
                }
                o->act[0] = 1;
                o->act[1] = D_006DD930[0].s[i];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                o->unkCE2 |= 0x20;
                return;
            case 3:
                for (i = 0; i < 16; i++) {
                    if (o->act[1] == D_006DD930[0].s[i]) {
                        break;
                    }
                }
                if (i == 16) {
                    return;
                }
                o->act[0] = 1;
                o->act[1] = D_006DD930[1].s[i];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                o->unkCE2 |= 0x20;
                return;
            }
        }
    }
}
#endif

/* Remembers o's position and heading as where it started. */
/* Whether the character is out of the game (gone, or status 0x5E0); with a != 0 always 0. */
u8 func_00634640(DrawObjFull *p, s32 a) {
    u8 r;

    if (a != 0) {
        return 0;
    }
    r = (p->unkBA0 & 0x5E0) != 0;
    if (p->active == 0) {
        r = 1;
    }
    return r;
}

void func_00635920(DrawObjFull *o) {
    *(Vec3 *)o->unkD94 = *(Vec3 *)o->unk38;
    o->unkDA0 = o->unk92;
}

/* Clears o's turn request (0xD94-0xDA0). */
void func_00635950(DrawObjFull *o) {
    FIELD(o, s32, 0xD94) = 0;
    FIELD(o, s32, 0xD98) = 0;
    FIELD(o, s32, 0xD9C) = 0;
    FIELD(o, s16, 0xDA0) = 0;
}

/* Takes o's pending motion change (unkBA4 4): 0 none (or the step is 0xFF), 2 with flag 0x80,
   else 1. */
s32 func_00635970(DrawObjFull *o) {
    if (o->unkBA4 & 4) {
        o->unkBA4 &= ~4;
        o->unk520 = 0;
        if (o->act[2] == 0xFF) {
            return 0;
        }
        return (o->unkBA4 & 0x80) ? 2 : 1;
    }
    return 0;
}

/* The room script's second table (at its 0x8004): an entry per room object model (< 0x20). */
u8 func_00551690(u8 model) {
    if (model >= 0x20) {
        return 0;
    }
    return (*(u8 **)(D_00306590.script + 0x8004))[model];
}

f32 func_005E5C20(const f32 *p, f32 *normal, f32 *m, u8 *kind, s32 want, u8 *sub, u16 *id,
                  u8 doMtx);

/* The floor under p for character o (its height, normal, kind, sub-kind and id). */
void func_005E5AF0(DrawObjFull *o, f32 *p, s32 want) {
    f32 m[16];

    o->unk170 = func_005E5C20(p, o->unk178, m, &o->unk174, want, &o->unk175, &o->unk176, 0);
}

/* The floor under a character (50 above its feet), and its fall: on kind-3 ground the height it
   falls from (0x170) is reset; when it is hit (status 0x88) there, it goes back to where it was
   last frame (0x44, heading 0xA0). */
void func_00634A10(DrawObjFull *o) {
    Vec3 p;

    p = *(Vec3 *)o->unk38;
    p.y += 50.0f;
    func_005E5AF0(o, &p.x, 0xFF);
    switch (o->unk174) {
    case 3:
        o->unk174 = 3;
        FIELD(o, u8, 0x553) = 0;
        if (o->unk188 != 0) {
            o->unk170 = o->unk184;
        } else {
            o->unk170 = 160.0f + o->unk38[1];
        }
        break;
    default:
        if (o->unk170 == p.y) {
            o->unk170 = o->unk38[1];
        }
        break;
    }
    if ((o->unkBA0 & 0x88) && o->unk174 == 3) {
        *(Vec3 *)o->unk38 = *(Vec3 *)o->unk44;
        *(Rot4 *)&o->unk90 = *(Rot4 *)o->unkA0;
    }
}

s32 func_0061E020(DrawObjFull *o);
void func_0058C0C0(DrawObjFull *o);
/* The breathing curve: (value, time) keys, ended by a time of -1. */
typedef struct {
    f32 val;
    f32 t;
} BreathKey;

extern BreathKey D_006D75A0[];  /* the own models' */
extern BreathKey D_006D75D0[];  /* the shared models' */
extern s8 D_0048BF88[];

/* The model's animation and two of its bones (0x60, 0x68). */
typedef struct {
    void *anim;
    u8 *unk4;           /* 0x04: the root bone */
    u8 unk8[0x54 - 8];
    u8 *unk54;          /* 0x54: the head bone */
    u8 unk58[8];
    u8 *unk60;
    u8 unk64[4];
    u8 *unk68;
} ModelBones;

typedef struct {
    u8 unk0[8];
    f32 scale;          /* 0x08 */
    u8 unkC[8];
} NpcModelScale;        /* 0x14 */

extern NpcModelScale D_006E2490[];  /* per NPC model (from 1) */
extern f32 D_006E6CB0[][3];         /* per character: its size */
extern f32 D_00239E8C;
extern f32 D_00239E90;

/* Squashes the model when its head is under something low (0x874's height): the root's 0x74
   by how far, scaled to the character. */
void func_005A6720(DrawObjFull *o);
#ifdef NON_MATCHING   /* close: one load hoisted into a delay slot the original leaves empty */
void func_005A6720(DrawObjFull *o) {
    ModelBones *b;
    f32 h;
    f32 s;
    f32 v;

    if (D_0048BF88[0] && o->unkB40 == 1) {
        return;
    }
    b = (ModelBones *)&o->model->unk0[0x820];
    s = 0.0f;
    h = FIELD(b->unk54, f32, 0x34) - o->unk38[1] - D_00239E8C;
    if (h <= 0.0f) {
        return;
    }
    switch (o->type) {
    case 0:
    case 1:
        s = o->unkAA8 * (o->unkB4[1] / (o->unkBB4 ? D_006E2490[o->unkBB4 - 1].scale : D_006E6CB0[o->unkB98][0]));
        break;
    case 2:
        s = o->unkAA8;
        break;
    }
    v = h * s / D_00239E90;
    if (v < s) {
        v = s;
    }
    FIELD(b->unk4, f32, 0x74) = v;
}
#endif

/* Breathing: while alive and not busy, sets two bones' 0x74 from the curve at the timer, then
   advances it by s (looping). */
void func_005A61C0(DrawObjFull *o, f32 s);
#ifdef NON_MATCHING   /* close: the loop-around keeps a different address in a register */
void func_005A61C0(DrawObjFull *o, f32 s) {
    BreathKey *k;
    ModelBones *b;

    if (o->unk54D == 0 && D_0048BF88[0] == 0 && FIELD(o, u16, 0x232) == 0 && o->unk544 > 0) {
        if ((u8)o->unkB99) {
            k = D_006D75D0;
        } else {
            k = D_006D75A0;
        }
        b = (ModelBones *)&o->model->unk0[0x820];
        FIELD(b->unk60, f32, 0x74) = ((f32)o->unkC50 - k[o->unkC52].t) *
                                     (k[o->unkC52 + 1].val - k[o->unkC52].val) /
                                     (k[o->unkC52 + 1].t - k[o->unkC52].t);
        FIELD(b->unk60, f32, 0x74) += k[o->unkC52].val;
        FIELD(b->unk68, f32, 0x74) = FIELD(b->unk60, f32, 0x74);
        s *= D_00306590.unkC ? 2.0f : 1.0f;
        o->unkC50 = (f32)o->unkC50 + s;
        if (!((f32)o->unkC50 < k[o->unkC52 + 1].t)) {
            o->unkC52++;
            if (k[o->unkC52 + 1].t == -1.0f) {
                o->unkC50 = (f32)o->unkC50 - k[o->unkC52].t;
                o->unkC52 = 0;
            }
        }
    }
}
#endif

/* The kind 0x81 surface at o (looked for from 1000 above it): 0x188 whether there is one, 0x184
   its height. */
void func_00634B40(DrawObjFull *o) {
    f32 p[3];
    f32 *y;
    u8 kind;

    y = &p[1];
    *(Vec3 *)p = *(Vec3 *)o->unk38;
    *y += 1000.0f;
    *y = func_005E5B40(p, &kind, 0x81);
    if (kind == 0x81) {
        o->unk184 = *y;
        o->unk188 = 1;
    } else {
        o->unk184 = 0.0f;
        o->unk188 = 0;
    }
}

void func_0064C620(DrawObjFull *o, s32 kind);
void func_0062F130(DrawObjFull *o);
s32 func_001AF510(void);
s32 func_001AF540(DrawObjFull *o);
/* A player's frame counters (0xD2C, 0xD28): counting unless stopped (-1). */
void func_00635310(DrawObjFull *o) {
    if (o != NULL && o->type == 0 && o->unkD2C != -1) {
        o->unkD2C++;
    }
}

void func_00635360(DrawObjFull *o) {
    if (o != NULL && o->type == 0 && o->unkD28 != -1) {
        o->unkD28++;
    }
}
extern u8 D_0048BF7E[];
s32 func_006AC490(void);
void func_006AE930(void);
extern s16 D_0023BB98;

/* (Ours, when 0x48BF7E holds our player number + 1, 1-4) the map's request: done once
   func_006AC490 says so. */
void func_005C41A0(DrawObjFull *o);
#ifdef NON_MATCHING   /* close: register pair swapped; the outer default's branch */
void func_005C41A0(DrawObjFull *o) {
    s32 k;
    u8 w;

    if (o->type != 0) {
        return;
    }
    w = D_0048BF7E[0];
    k = o->kind + 1;
    if (k != w) {
        return;
    }
    switch (w) {
    case 1:
    case 2:
    case 3:
    case 4:
        switch (D_0023BB98) {
        default:
            if (k == w && !func_006AC490()) {
                D_0048BF7E[0] = 0;
                return;
            }
            break;
        case 1:
            if (w == 0) {
                func_006AE930();
            }
            break;
        }
        break;
    default:
        break;
    }
}
#endif
void func_0062E1E0(DrawObjFull *o, s32 a);

/* Out of health, standing or walking (action 1, 2): NPCs die at once (action 4/0); players
   die (4/3, 0xBA0 |= 0x120) unless something holds them (0x1F0: 0x80 waits 0xC5E frames for
   0xBA4 & 0x100, 0x10 stops it). */
void func_0062EE10(DrawObjFull *o) {
    s32 f;

    if (D_0048BF88[0] == 0 && D_0023BB98 != 1 && o->unk544 == 0) {
        switch (o->act[0]) {
        case 1:
        case 2:
            f = o->unkBA0;
            if (!(f & 0x1F0)) {
                if (o->type == 1) {
                    func_005C5A90(o, 0);
                    o->unk54F = 0xA0;
                    o->act[0] = 4;
                    o->act[1] = 0;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                } else if (o->unkBA4 & 0x100) {
                    func_0062E1E0(o, 0);
                }
            } else if (f & 0x80) {
                if (o->unkC5E != 0) {
                    o->unkC5E--;
                }
                if (o->unkC5E == 0 && (o->unkBA4 & 0x100)) {
                    o->unkBA0 |= 0x120;
                    func_005C5A90(o, 0);
                    o->unk54F = 0xA0;
                    o->act[0] = 4;
                    o->act[1] = 3;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                }
            } else if (!(f & 0x10)) {
                o->unkBA0 = f | 0x120;
                func_005C5A90(o, 0);
                o->unk54F = 0xA0;
                o->act[0] = 4;
                o->act[1] = 3;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
            }
            break;
        }
    }
}
extern u8 D_0048AEA8[];

/* Drops what the player was doing for a reaction: 0x200000 and the motion's hold cleared, and
   (when 0xBD0 held the controls) the buttons let go. */
void func_005F4C20(DrawObjFull *o) {
    D_0048AEA8[0] = 0;
    o->unkBA0 &= ~0x200000;
    if (o->unkBD0) {
        o->unkBD0 = 0;
        o->ctrl.prev[0] = o->ctrl.cur;
        o->ctrl.repeat = 0;
        o->ctrl.released = 0;
        o->ctrl.pressed = 0;
    }
}

extern void (*D_006DDA80[])(DrawObjFull *o);    /* the reactions */
extern void (*D_006DDB30[])(DrawObjFull *o);    /* action 1's steps */

s32 func_0061D9A0(DrawObjFull *o);
s32 func_0061D090(DrawObjFull *o);
s32 func_0061E810(DrawObjFull *o);
s32 func_0061D0F0(DrawObjFull *o);
s32 func_0061DEF0(DrawObjFull *o);
s32 func_0061D1E0(DrawObjFull *o);
s32 func_0061D1F0(DrawObjFull *o);
s32 func_0061DE60(DrawObjFull *o);
s32 func_0061E3E0(DrawObjFull *o);
s32 func_0061E3F0(DrawObjFull *o);
s32 func_0061CE50(DrawObjFull *o);
s32 func_0061CF40(DrawObjFull *o);
s32 func_0061D000(DrawObjFull *o);
s32 func_0061C580(DrawObjFull *o, s32 a);

/* Per weapon (0x44 bytes): what it allows, and how it sits in the hand. */
typedef struct {
    u32 flags;          /* 0x00: 1 aiming, 4 its run */
    u8 unk4;
    u8 hold;            /* 0x05: how it is held (0xC: two-handed, its own way in motion 0x80) */
    u8 unk6[0xA];
    f32 ofs[2][3];      /* 0x10: in the hand (own, shared motions) */
    u16 rot[2][4];      /* 0x28: its turn in the hand (0x10000 a turn) */
    u8 unk38[0xC];
} WeaponInfo;

extern WeaponInfo D_006E5940[];
s32 func_0062D6A0(DrawObjFull *o);
s32 func_005561A0(DrawObjFull *o, u8 item);
s32 func_0061DB40(DrawObjFull *o);

/* The standing step's checks: each starts its action (1 when it did). */

/* Walking forward (up). */
s32 func_0061C490(DrawObjFull *o, u8 set) {
    if (o->ctrl.cur & 1) {
        o->act[0] = 1;
        o->act[1] = D_006DD930[set].s[3];
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        return 1;
    }
    return 0;
}

/* Running (up with run held). */
s32 func_0061C4F0(DrawObjFull *o, u8 set);
#ifdef NON_MATCHING   /* close: which instruction fills one branch's delay slot */
s32 func_0061C4F0(DrawObjFull *o, u8 set) {
    u32 c;

    if (o->unkBA0 & 8) {
        return 0;
    }
    c = o->ctrl.cur;
    if (c & 0x10) {
        if (c & 1) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[6];
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif

s32 func_0061C580(DrawObjFull *o, s32 a) {
    return 0;
}

/* Backing away (down). */
s32 func_0061C590(DrawObjFull *o, u8 set) {
    if (o->unkBA0 & 0x80) {
        if (o->ctrl.cur & 2) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[5];
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        return 0;
    }
    if (o->ctrl.cur & 2) {
        o->act[0] = 1;
        o->act[1] = D_006DD930[set].s[9];
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        return 1;
    }
    return 0;
}

/* Aiming (0x400 at the nearest target, 0x40 at nothing): action 2. */
s32 func_0061CE50(DrawObjFull *o) {
    if (o->unkBA0 & 0x98) {
        return 0;
    }
    if ((u16)o->unkCA8 != 0 && !(D_006E5940[(u16)o->unkCA8].flags & 1)) {
        return 0;
    }
    if (o->ctrl.cur & 0x400) {
        o->unk24 = func_0062D6A0(o);
        o->act[0] = 2;
        o->act[1] = 0;
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        return 1;
    }
    if (o->ctrl.cur & 0x40) {
        o->unkCDC = 0xFF;
        o->unk24 = 0;
        o->act[0] = 2;
        o->act[1] = 0;
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        return 1;
    }
    return 0;
}

/* The quick turn (run pressed while backing away). */
#ifdef NON_MATCHING   /* close: which instruction fills one branch's delay slot */
s32 func_0061CF40(DrawObjFull *o) {
    if (o->unkBA0 & 0x98) {
        return 0;
    }
    if (o->unkB9B == 3) {
        return 0;
    }
    if ((u16)o->unkCA8 != 0 && !(D_006E5940[(u16)o->unkCA8].flags & 4)) {
        return 0;
    }
    if (o->ctrl.pressed & 0x10) {
        if (o->ctrl.cur & 2) {
            o->act[0] = 1;
            o->act[1] = 0x15;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif

/* Using the item in hand (0x800). */
#ifdef NON_MATCHING   /* close: which instruction fills one branch's delay slot */
s32 func_0061D000(DrawObjFull *o) {
    if (o->unkBA0 & 0x98) {
        return 0;
    }
    if (o->unkC7C != 0) {
        if (func_005561A0(o, o->unkC7C) && (o->ctrl.pressed & 0x800)) {
            o->act[0] = 1;
            o->act[1] = 0x38;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif

/* (Busy, 0xBA0 & 0x80) 0x20 pressed: step 0xB7. */
s32 func_0061D090(DrawObjFull *o) {
    if (!(o->unkBA0 & 0x80)) {
        return 0;
    }
    if (!(o->ctrl.pressed & 0x20)) {
        return 0;
    }
    o->act[0] = 1;
    o->act[1] = 0xB7;
    o->act[2] = 0;
    o->act[3] = 0;
    func_00635920(o);
    return 1;
}

/* The right stick flicked (0xF00000): up for step 0x11, down for 0x3C. */
#ifdef NON_MATCHING   /* close: which instruction fills one branch's delay slot */
s32 func_0061D0F0(DrawObjFull *o) {
    s32 q;

    if (o->unkBA0 & 0x98) {
        return 0;
    }
    if (o->ctrl.cur & 0x80) {
        return 0;
    }
    if (!(o->ctrl.pressed & 0xF00000)) {
        return 0;
    }
    q = ((o->ctrl.langle + 0x2000) & 0xC000) >> 14;
    if (q == 0 && o->unk54F != 0x1E) {
        o->act[0] = 1;
        o->act[1] = 0x11;
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        return 1;
    }
    if (q == 2) {
        if (o->unk54F != 0x42) {
            o->act[0] = 1;
            o->act[1] = 0x3C;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif

s32 func_0061D1E0(DrawObjFull *o) {
    return 0;
}

/* 0x20 pressed (or a partner): func_0061DB40's, unless ours with 0x8000 and no map request. */
s32 func_0061DE60(DrawObjFull *o) {
    if (o->type == 0 && !(o->ctrl.pressed & 0x20)) {
        return 0;
    }
    if (func_001AF540(o) == 1 && (o->ctrl.pressed & 0x8000) && D_0048BF7E[0] == 0) {
        return 0;
    }
    return func_0061DB40(o) != 0;
}

s32 func_0061DEF0(DrawObjFull *o) {
    return 0;
}

s32 func_0061E3E0(DrawObjFull *o) {
    return 0;
}

s32 func_0061E3F0(DrawObjFull *o) {
    return 0;
}

/* (Character 1, ours) 0x10 with 0x440 held: action 2/0x15/10. */
#ifdef NON_MATCHING   /* close: which instruction fills one branch's delay slot */
s32 func_0061E810(DrawObjFull *o) {
    u32 c;

    if (o->unkBA0 & 0x98) {
        return 0;
    }
    if (o->unkB98 == 1) {
        if (o->kind == D_0048BF7D[0] && ((c = o->ctrl.cur) & 0x10) && (c & 0x440)) {
            o->act[0] = 2;
            o->act[1] = 0x15;
            o->act[2] = 10;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif

f32 func_003400C0(s16 a);
f32 func_00340100(s16 a);
u8 func_005494C0(const f32 *from, const f32 *to, f32 *hit, f32 *normal, s32 mask, s32 b, s32 *kind);

/* Probes dist ahead of o (10 up) for what is there (mask, 0: all but walls): its kind (0x528 the
   point, 0x534 the surface), 0 for nothing. */
u16 func_0062DAE0(DrawObjFull *o, s32 mask, s32 b, f32 dist) {
    s32 kind;
    f32 p[3];

    if (mask == 0) {
        mask = 0xFFFF003E;
    }
    p[0] = o->unk38[0] + dist * func_003400C0(o->unk92);
    p[2] = o->unk38[2] + dist * func_00340100(o->unk92);
    p[1] = 10.0f + o->unk38[1];
    o->unk540 = func_005494C0(o->unk38, p, o->unk528, o->unk534, mask, b, &kind);
    if (o->unk540) {
        return kind;
    }
    return 0;
}
s32 func_00635500(DrawObjFull *o, s32 angle, f32 dist);
s32 func_0067D6C0(DrawObjFull *o);
s32 func_0061D7A0(DrawObjFull *o, u8 b);
f32 func_001917C0(const f32 *a, const f32 *b);
f32 func_00340100(s16 a);
void func_005AC0F0(f32 *pos, DrawObjFull *o, u8 a);

/* Sets off the trigger zones at the player (what 0), just ahead of it (1, 3) or what is in
   front of it (2, func_005AC0F0), for zone kind kind. */
void func_00631760(DrawObjFull *o, u8 what, s32 kind) {
    f32 p[3];

    switch (what) {
    case 0:
        p[0] = o->unk38[0];
        p[1] = o->unk38[1];
        p[2] = o->unk38[2];
        func_005ABBE0(50.0f, p, o, kind);
        break;
    case 1:
        p[0] = o->unk38[0] + (46.0f + o->unk1B4[0]) * func_003400C0(o->unk92);
        p[2] = o->unk38[2] + (46.0f + o->unk1B4[0]) * func_00340100(o->unk92);
        p[1] = o->unk38[1];
        func_005ABBE0(50.0f, p, o, kind);
        break;
    case 2:
        p[0] = o->unk38[0] + 46.0f * func_003400C0(o->unk92);
        p[2] = o->unk38[2] + 46.0f * func_00340100(o->unk92);
        p[1] = o->unk38[1];
        func_005AC0F0(p, o, o->unkCB0);
        break;
    case 3:
        p[0] = o->unk38[0] + o->unk1B4[0] * func_003400C0(o->unk92);
        p[2] = o->unk38[2] + o->unk1B4[0] * func_00340100(o->unk92);
        p[1] = o->unk38[1];
        func_005ABBE0(60.0f, p, o, kind);
        break;
    }
}

/* What is ahead (func_0062DAE0's kinds, 55 ahead, 75 in danger, behind when backing): 0x20 at
   a step, ledge or the like starts climbing it (1/0x13, 0x47, 0x14, 0x3B, 0x56-0x58); then a
   wall 96 ahead pushed against for a second (ours) func_00631760 and func_0061D7A0. 1 when an
   action started. */
#ifdef NON_MATCHING   /* close: the push-against-a-wall tail's branches and one delay slot */
s32 func_0061D1F0(DrawObjFull *o) {
    f32 v[3];
    f32 d;
    f32 a;
    s32 k;
    u16 w;

    if (o->unkBA0 & 8) {
        return 0;
    }
    if (o->unk174 == 1) {
        return 0;
    }
    if (o->unkB9B == 2) {
        d = 75.0f;
    } else {
        d = 55.0f;
    }
    if (o->unk54F == 2) {
        d = -d;
    }
    k = func_0062DAE0(o, -1, 0, d);
    switch ((u16)k) {
    case 4:
    case 0x10:
        if ((o->ctrl.pressed & 0x20) && func_00635500(o, 0x2000, 50.0f)) {
            if (k == 4) {
                o->act[0] = 1;
                o->act[1] = 0x13;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
            } else {
                o->act[0] = 1;
                o->act[1] = 0x47;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
            }
            func_0064C620(o, 6);
            return 1;
        }
        break;
    case 8:
    case 0x20:
        if (o->ctrl.pressed & 0x20) {
            if ((o->unkBA0 & 0x4000) && !(s8)func_0067D6C0(o)) {
                break;
            }
            if (func_00635500(o, 0x2000, 50.0f)) {
                if (k == 8) {
                    o->act[0] = 1;
                    o->act[1] = 0x14;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                } else {
                    o->act[0] = 1;
                    o->act[1] = 0x3B;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                }
                func_0064C620(o, 6);
                return 1;
            }
        }
        break;
    case 0x40:
        if (o->ctrl.pressed & 0x20) {
            if (!(u8)func_00635500(o, 0x2000, 50.0f)) {
                return 0;
            }
            o->act[0] = 1;
            o->act[1] = 0x56;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            func_0064C620(o, 6);
            return 1;
        }
        break;
    case 0x100:
        if (o->ctrl.pressed & 0x20) {
            if (!(u8)func_00635500(o, 0x2000, 50.0f)) {
                return 0;
            }
            o->act[0] = 1;
            o->act[1] = 0x58;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            func_0064C620(o, 6);
            return 1;
        }
        break;
    case 0x80:
        if (o->ctrl.pressed & 0x20) {
            if (!(u8)func_00635500(o, 0x2000, 50.0f)) {
                return 0;
            }
            o->act[0] = 1;
            o->act[1] = 0x57;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            func_0064C620(o, 6);
            return 1;
        }
        break;
    case 0x800:
        if (o->ctrl.pressed & 0x20) {
            if (!(u8)func_00635500(o, 0x2000, 50.0f)) {
                return 0;
            }
            o->act[0] = 1;
            o->act[1] = 0x56;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            func_0064C620(o, 6);
            return 1;
        }
        break;
    case 0x200:
        if (o->ctrl.pressed & 0x20) {
            if (!(u8)func_00635500(o, 0x2000, 50.0f)) {
                return 0;
            }
            o->act[0] = 1;
            o->act[1] = 0x57;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            func_0064C620(o, 6);
            return 1;
        }
        break;
    }
    w = func_0062DAE0(o, -1, 0, o->unk54F == 2 ? -96.0f : 96.0f);
    if (w != 2) {
        o->unkCDE = 0;
    }
    switch (w) {
    case 2:
        if (func_001AF540(o) == 1 && !(o->unkBA0 & 0x90)) {
            if (o->ctrl.cur & 1) {
                v[0] = -1.0f * o->unk534[0];
                v[1] = -1.0f * o->unk534[1];
                v[2] = -1.0f * o->unk534[2];
                a = func_001917C0((f32 *)&o->unkA8, v);
                if (a < func_00340100(0x2000)) {
                    o->unkCDE = 0;
                } else {
                    o->unkCDE++;
                    if (o->unkCDE >= (D_00306590.unkC ? 30 : 60)) {
                        func_00631760(o, 1, 5);
                        o->unkCDE = 0;
                        if (func_0061D7A0(o, 0)) {
                            func_0064C620(o, 3);
                            return 1;
                        }
                    }
                }
            } else {
                o->unkCDE = 0;
            }
        }
        break;
    case 0:
        return 0;
    }
    return 0;
}
#endif

void func_00637B90(DrawObjFull *o);
void func_006373F0(DrawObjFull *o);
void func_001AF240(u8 player);
extern u16 D_006DD9B0[][2];     /* per zone action: its step (alone, and from 0x61D9A0's b) */

/* A zone's action for the player (0xAB8 asked, 0xAB9 which): the step D_006DD9B0 gives (while
   busy only some), or with b 0 and not walking, func_00637B90/func_006373F0 and func_001AF240.
   1 when it acted. */
#ifdef NON_MATCHING   /* close: an extra mask of the zone action before the compares; one branch order */
s32 func_0061D7A0(DrawObjFull *o, u8 b) {
    if (!o->unkAB8) {
        return 0;
    }
    o->unkAB8 = 0;
    if (!o->unkAB9) {
        return 0;
    }
    if (o->unkBA0 & 0x80) {
        if (o->unkAB9 != 0x2F && o->unkAB9 != 0x30 && o->unkAB9 != 0x20 && o->unkAB9 != 0x1F &&
            o->unkAB9 != 7 && o->unkAB9 != 0x29 && o->unkAB9 != 0x1A && o->unkAB9 != 2 &&
            o->unkAB9 != 1) {
            return 0;
        }
    }
    if (b == 0 && o->act[0] != 1 && o->type == 0) {
        if (func_001AF540(o) == 1) {
            func_00637B90(o);
        } else if (func_001AF4D0() == 1) {
            func_006373F0(o);
            o->unkBD0 = 0;
        }
        func_001AF240(o->kind);
        return 1;
    }
    o->act[0] = 1;
    o->act[1] = D_006DD9B0[o->unkAB9][b];
    o->act[2] = 0;
    o->act[3] = 0;
    func_00635920(o);
    switch (o->unkAB9) {
    case 0:
        return 1;
    case 1:
    case 2:
        if (b == 0) {
            o->act[0] = 1;
            o->act[1] = o->unkABA ? 0x17 : 0x18;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
        }
        break;
    }
    return 1;
}
#endif

void func_0064E360(DrawObjFull *o, s32 a, u8 b);

/* (Also with the map up) the item and pick-up checks (func_0061D7A0), held 0xAB8 (in motion
   0x16: step 0xDD), and standing idle (0xBBD frames; 20/40 in motions 1, 3 starts step 0xAE
   for 900/1800 frames). 1 when an action started. */
#ifdef NON_MATCHING   /* close: one branch's delay slot (the original leaves a nop before the shared return 0) */
s32 func_0061D9A0(DrawObjFull *o) {
    u8 old = o->unkBBD;

    func_00631760(o, 3, 7);
    func_00631760(o, 3, 1);
    if (o->unkAD4) {
        o->unkAB8 = 0;
        return 1;
    }
    if (o->unkAB8) {
        switch (o->unk54F) {
        case 0x16:
            o->unkAB8 = 0;
            if (!o->unkAB9) {
                return 0;
            }
            o->act[0] = 1;
            o->act[1] = 0xDD;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
    }
    if (func_0061D7A0(o, 0)) {
        func_0064C620(o, 3);
        return 1;
    }
    if (o->unkBBD == old) {
        o->unkBBD = 0;
    }
    if (!(o->unkBA0 & 0x90)) {
        if (o->unkBBD >= (D_00306590.unkC ? 20 : 40)) {
            switch (o->unk54F) {
            case 1:
            case 3:
                o->unkC = D_00306590.unkC ? 900 : 1800;
                o->act[0] = 1;
                o->act[1] = 0xAE;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                func_0064E360(o, 3, o->unkCB0);
                return 1;
            }
        }
        return 0;
    }
    return 0;
}
#endif

/* Turning (left 8, right 4 in 0xF80): standing turns at once; walking and the other motions
   turn their way, pressed (0xF78) for a quick turn, held (act[2] 1) to keep turning. */
s32 func_0061C650(DrawObjFull *o, u8 set) {
    u32 c;
    u32 p;

    switch (o->unk54F) {
    case 0:
    case 0x21:
    case 0x2D:
        c = o->ctrl.cur;
        if (c & 8) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[1];
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        if (c & 4) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[2];
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        break;
    case 1:
    case 0x22:
    case 0x2E:
        if (!(o->unkBA0 & 0x2000)) {
            p = o->ctrl.pressed;
            if (p & 8) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[4];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
            if (p & 4) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[5];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
        }
        c = o->ctrl.cur;
        if (c & 8) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[4];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        if (c & 4) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[5];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        break;
    case 2:
    case 0x23:
    case 0x2F:
        if (o->unkBA0 & 0x80) {
            c = o->ctrl.cur;
            if (c & 8) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[4];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
            if (c & 4) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[5];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
            break;
        }
        if (!(o->unkBA0 & 0x2000)) {
            p = o->ctrl.pressed;
            if (p & 8) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[11];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
            if (p & 4) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[10];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
        }
        c = o->ctrl.cur;
        if (c & 8) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[11];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        if (c & 4) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[10];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        break;
    case 3:
    case 0x24:
        if (!(o->unkBA0 & 0x2000)) {
            p = o->ctrl.pressed;
            if (p & 8) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[7];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
            if (p & 4) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[8];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
        }
        c = o->ctrl.cur;
        if (c & 8) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[7];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        if (c & 4) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[8];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        break;
    case 4:
        if (!(o->unkBA0 & 0x2000)) {
            p = o->ctrl.pressed;
            if (p & 8) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[13];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
            if (p & 4) {
                o->act[0] = 1;
                o->act[1] = D_006DD930[set].s[14];
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                return 1;
            }
        }
        c = o->ctrl.cur;
        if (c & 8) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[13];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        if (c & 4) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[14];
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        break;
    default:
        c = o->ctrl.cur;
        if (c & 8) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[1];
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        if (c & 4) {
            o->act[0] = 1;
            o->act[1] = D_006DD930[set].s[2];
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            return 1;
        }
        break;
    }
    return 0;
}

s32 func_006332D0(DrawObjFull *o);
void func_006321D0(DrawObjFull *o);

/* (Online) another player's push (0xD80 a frame for 0xD90 frames, ending at 0xD74 facing
   0xD8C), while standing or walking: 2 while it lasts, 1 on its last frame, 0 when there's
   none (out of those steps it is put at the end at once). 0x54A & 0x4000: func_006332D0 says it
   can't be pushed (any push is dropped). */
s32 func_00632DC0(DrawObjFull *o);
#ifdef NON_MATCHING   /* same code; ours merges the two copies of the put-at-the-end tail and fills the case delay slots */
s32 func_00632DC0(DrawObjFull *o) {
    f32 *vy;
    f32 *vz;

    if (!func_001AF510()) {
        return 0;
    }
    if (func_001AF4D0() == 1) {
        return 0;
    }
    if (o->type != 0) {
        return 0;
    }
    if (o->kind == D_0048BF7D[0]) {
        return 0;
    }
    if (!(o->unk54A & 0x4000)) {
        if (func_006332D0(o)) {
            o->unk54A &= ~0x4000;
        } else {
            o->unk54A |= 0x4000;
        }
    }
    if (o->unk54A & 0x4000) {
        o->unkD90 = 0;
        o->unkD80[0] = 0.0f;
        o->unkD80[1] = 0.0f;
        o->unkD80[2] = 0.0f;
        o->unkD8E = 0;
        return 0;
    }
    switch (o->act[0]) {
    case 1:
        switch (o->act[1]) {
        case 0:
        case 1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0x31:
        case 0x32:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
        case 0xA0:
        case 0x24:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0xCD:
        case 0xD0:
        case 0xD1:
        case 0x81:
        case 0x82:
        case 0x83:
            func_00632180(o);
            vy = &o->unkD80[1];
            vz = &o->unkD80[2];
            o->unk38[0] += o->unkD80[0];
            o->unk38[1] += *vy;
            o->unk38[2] += *vz;
            o->unk92 += o->unkD8E;
            func_006321D0(o);
            if (o->unkD90 == 0) {
                return 0;
            }
            if (--o->unkD90 != 0) {
                o->unk54A |= 1;
                return 2;
            }
            *(Vec3 *)o->unk38 = *(Vec3 *)o->unkD74;
            o->unk92 = o->unkD8C;
            if (o->unkD90 == 0) {
                o->unkD80[0] = 0.0f;
                *vy = 0.0f;
                *vz = 0.0f;
                o->unkD8E = 0;
            }
            return 1;
        default:
            if (o->unkD90 != 0) {
                *(Vec3 *)o->unk38 = *(Vec3 *)o->unkD74;
                o->unk92 = o->unkD8C;
                o->unkD90 = 0;
                o->unkD80[0] = 0.0f;
                o->unkD80[1] = 0.0f;
                o->unkD80[2] = 0.0f;
                o->unkD8E = 0;
            }
            return 0;
        }
    default:
        if (o->unkD90 != 0) {
            *(Vec3 *)o->unk38 = *(Vec3 *)o->unkD74;
            o->unk92 = o->unkD8C;
            o->unkD90 = 0;
            o->unkD80[0] = 0.0f;
            o->unkD80[1] = 0.0f;
            o->unkD80[2] = 0.0f;
            o->unkD8E = 0;
        }
        return 0;
    }
}
#endif

u16 func_005C1210(u8 item);
void func_0018F7C0(f32 *m);
void func_00191750(f32 *v);
void func_00190C50(f32 *out, const f32 *a, const f32 *b);
void func_0018FAB0(f32 *out, const f32 *m);
extern f32 D_00239D30;          /* 2 pi */
extern f32 D_00239D84;          /* (two-handed in motion 0x80) its turn */
extern f32 D_00239D88;
extern f32 D_00239D8C;
extern f32 D_00239D90;          /* and place */
extern f32 D_00239D94;
extern f32 D_00239D98;

/* Puts the item in o's hand on bone bone (15: the left hand, mirrored): its matrix from the
   bone's, turned and moved as the weapon table says for the character. */
void func_006313A0(DrawObjFull *o, u8 item, s32 bone);
#ifdef NON_MATCHING   /* close: the item slot's address operand order; the three angles' evaluation order */
void func_006313A0(DrawObjFull *o, u8 item, s32 bone) {
    f32 m[16];
    f32 p[3];
    ItemObj *it;
    u8 **b;
    WeaponInfo *e;
    u16 *r;
    f32 *py;
    f32 *pz;

    if (item == 0) {
        return;
    }
    it = D_003967D0[item].obj;
    if (it == NULL) {
        return;
    }
    if (o->model == NULL) {
        return;
    }
    b = (u8 **)((u8 *)o->model + bone * 4 + 0x820);
    if (*b == NULL) {
        return;
    }
    func_0018F7C0(m);
    *(Mat44 *)it->mat = *(Mat44 *)*b;
    func_00191750(&it->mat[0]);
    func_00191750(&it->mat[4]);
    func_00191750(&it->mat[8]);
    e = &D_006E5940[func_005C1210(item)];
    if (e == NULL) {
        return;
    }
    py = &p[1];
    pz = &p[2];
    r = e->rot[(u8)o->unkB99];
    *(Vec3 *)p = *(Vec3 *)e->ofs[(u8)o->unkB99];
    p[0] *= o->unkB4[0];
    *py *= o->unkB4[1];
    *pz *= o->unkB4[2];
    if (bone == 15) {
        p[0] *= -1.0f;
    }
    if (o->unk54F == 0x80) {
        switch (o->act[1]) {
        case 0:
        case 7:
            if (D_006E5940[(u16)o->unkCA8].hold == 0xC) {
                func_00190C50(it->mat, it->parts[5].m[0], it->mat);
                func_0018FAB0(it->pos, it->mat);
                return;
            }
            break;
        default:
            if (D_006E5940[(u16)o->unkCA8].hold == 0xC) {
                func_0018FCA0(D_00239D84, D_00239D88, D_00239D8C, m);
                m[12] = D_00239D90;
                m[13] = D_00239D94;
                m[14] = D_00239D98;
                if (bone == 15) {
                    m[0] *= -1.0f;
                    m[4] *= -1.0f;
                    m[8] *= -1.0f;
                }
                func_00190C50(it->mat, m, it->mat);
                func_0018FAB0(it->pos, it->mat);
                return;
            }
            break;
        }
    }
    func_0018FCA0(D_00239D30 * (s32)r[0] / 65536.0f, D_00239D30 * (s32)r[1] / 65536.0f,
                  D_00239D30 * (s32)r[2] / 65536.0f, m);
    m[12] = p[0];
    m[13] = *py;
    m[14] = *pz;
    if (bone == 15) {
        m[0] *= -1.0f;
        m[4] *= -1.0f;
        m[8] *= -1.0f;
    }
    func_00190C50(it->mat, m, it->mat);
    func_0018FAB0(it->pos, it->mat);
}
#endif

/* Action 1's standing step: the first of the player's possible actions that starts (each
   func_0061Cxxx-func_0061Exxx checks its buttons and starts it); 1 when one did. With the map
   screen up only func_0061D9A0's. */
s32 func_006198F0(DrawObjFull *o) {
    if (o->type != 0) {
        return 0;
    }
    if (func_001AF540(o) == 1 && func_006AC490()) {
        if (o->type == 0 && func_0061D9A0(o)) {
            return 1;
        }
        return 0;
    }
    o->unkBBD = 0;
    if (o->type == 0 && func_0061D090(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061E810(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061D0F0(o)) {
        return 1;
    }
    if (func_0061DEF0(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061D1E0(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061D1F0(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061DE60(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061D9A0(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061E3E0(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061E3F0(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061CE50(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061CF40(o)) {
        return 1;
    }
    if (o->type == 0 && func_0061C580(o, 0)) {
        return 1;
    }
    if (o->type == 0 && func_0061C4F0(o, 0)) {
        return 1;
    }
    if (o->type == 0 && func_0061C490(o, 0)) {
        return 1;
    }
    if (o->type == 0 && func_0061C590(o, 0)) {
        return 1;
    }
    if (o->type == 0 && func_0061C650(o, 0)) {
        return 1;
    }
    if (o->type == 0 && func_0061D000(o)) {
        return 1;
    }
    return 0;
}

/* Action 1 (standing, walking): its step, unless held (0xBA4 & 4). */
void func_006197E0(DrawObjFull *o) {
    if (!(o->unkBA4 & 4)) {
        D_006DDB30[o->act[1]](o);
    }
}

/* A pending reaction (0xAD4, which 0xAD5) starts, unless the player is busy (0xBA0 & 0x80). */
void func_006196E0(DrawObjFull *o) {
    if (o->unkAD4) {
        o->unkAD4 = 0;
        if (!(o->unkBA0 & 0x80)) {
            func_005F4C20(o);
            o->unkBA0 &= ~0x80000;
            D_006DDA80[o->unkAD5](o);
        }
    }
}
void *memset(void *p, s32 c, u32 n);
s32 func_00346C50(const f32 *a, const f32 *b);
extern u8 D_0048C01D[];
extern u8 D_003AEF54[];         /* the camera cut this frame */
extern f32 D_00306338[3];       /* the camera */
extern f32 D_00306344[3];       /* what it looks at */
extern u16 D_006DEC10[5][2];    /* (motion 0x16) the stick's sectors against the heading */
extern u16 D_006DEBF0[5][2];    /* (the others) */

/* Turns the player's left stick into direction buttons (0xF80's low 4 bits: 1 forward, 2 back,
   4 and 8 turning; 0x10 running), by the stick's angle against the camera and the heading:
   8 ways for the motions that use them, otherwise by sectors (D_006DEBF0). */
void func_006307E0(DrawObjFull *o);
#ifdef NON_MATCHING   /* same code; the s registers are assigned differently */
void func_006307E0(DrawObjFull *o) {
    s32 dir;
    u8 eight;
    u16 d;
    u16 sect;
    u8 hold;
    u8 walk;
    u16 i;
    s32 a;
    s32 r;

    dir = 0;
    hold = 1;
    walk = hold;
    eight = 0;
    if (o->unkBD0 || ((o->unkBA0 & 0x80) && D_0048C01D[0])) {
        memset(&o->ctrl, 0, sizeof(Ctrl));
    }
    if (func_001AF540(o) == 1 && (D_0048BF7E[0] || func_006AC490())) {
        walk = 1;
    }
    if (o->ctrl.rmag < 0x10) {
        o->unkFAA = 0;
        o->unkBA0 &= ~0x2000;
        if ((o->unkBA0 & 0x80) && (o->ctrl.cur & 2)) {
            dir &= ~2;
            r = o->ctrl.cur & 0xC;
            if (r == 0) {
                r = 4;
            }
            dir |= r;
            o->ctrl.cur &= ~0xF;
            o->ctrl.cur |= dir;
            o->ctrl.pressed = o->ctrl.cur & ~o->ctrl.prev[0];
            o->ctrl.released = o->ctrl.prev[0] & ~o->ctrl.cur;
        }
        return;
    }
    o->unkBA0 |= 0x2000;
    switch (o->unk54F) {
    case 0x1A:
    case 0xB0:
    case 0x80:
    case 0x27:
    case 0x31:
        eight = 1;
        break;
    case 0x15:
        hold = 1;
    case 0x16:
        walk = 1;
        break;
    }
    if (o->unkB98 == 1 && o->unk54F == 0x41) {
        eight = 1;
    }
    if (eight) {
        switch ((o->ctrl.rangle + 0x2000) & 0xC000) {
        case 0:
            dir = 1;
            break;
        case 0x2000:
            dir = 5;
            break;
        case 0x4000:
            dir = 4;
            break;
        case 0x6000:
            dir = 6;
            break;
        case 0x8000:
            dir = 2;
            break;
        case 0xA000:
            dir = 10;
            break;
        case 0xC000:
            dir = 8;
            break;
        case 0xE000:
            dir = 9;
            break;
        }
        o->ctrl.cur &= ~0xF;
        o->ctrl.cur |= dir;
        if (o->unkBD0) {
            o->ctrl.cur = 0;
        }
        o->ctrl.pressed = o->ctrl.cur & ~o->ctrl.prev[0];
        o->ctrl.released = o->ctrl.prev[0] & ~o->ctrl.cur;
        return;
    }
    if (D_003AEF54[0] && !o->isNpc) {
        o->unkFAA = 1;
        o->unkFA8 = o->ctrl.rangle;
    }
    D_003AEF54[0] = 0;
    a = (s16)((s16)func_00346C50(D_00306338, D_00306344) & 0xFFF0) + (s16)(o->ctrl.rangle & 0xFFF0);
    o->unkBCE = a;
    d = a - o->unk92;
    for (i = 0; i < 5; i++) {
        if (o->unk54F == 0x16) {
            if (d >= D_006DEC10[i][0] || d <= D_006DEC10[i][1]) {
                sect = i;
                break;
            }
        } else {
            if (d >= D_006DEBF0[i][0] || d <= D_006DEBF0[i][1]) {
                sect = i;
                break;
            }
        }
    }
    if (o->unkFAA) {
        if ((u16)(o->unkFA8 - o->ctrl.rangle + 0x71C) < 0xE39) {
            if (o->unk54F == 2) {
                sect = 4;
            } else {
                sect = 0;
            }
        }
    }
    switch (sect) {
    case 0:
        dir |= 1;
        break;
    case 1:
        r = 8;
        if (!(d & 0x8000)) {
            r = 4;
        }
        dir |= 1;
        dir |= r;
        break;
    case 2:
        r = 8;
        if (!(d & 0x8000)) {
            r = 4;
        }
        dir |= 1;
        dir |= r;
        break;
    case 3:
    case 4:
        if (o->isNpc) {
            r = 8;
            if (!(d & 0x8000)) {
                r = 4;
            }
            dir |= 1;
            dir |= r;
            break;
        }
        if (o->unkBA0 & 0x80) {
            r = 8;
            if (!(d & 0x8000)) {
                r = 4;
            }
            dir |= 1;
            dir |= r;
            break;
        }
        if (o->unkFAA) {
            hold = 1;
        }
        r = (s32)func_0062D700(o, 0, -0x8000, 0, o->unk54F == 2 ? 320.0f : 300.0f);
        if (r && !(o->ctrl.cur & 0x10)) {
            walk = 1;
            hold = 0;
        }
        if (o->unk54F == 2 && o->unkFAA && ((o->ctrl.pressed & 0x10) || !r)) {
            walk = 0;
            hold = 1;
        }
        if (hold && walk) {
            r = 8;
            if (!(d & 0x8000)) {
                r = 4;
            }
            dir |= 1;
            dir |= r;
        } else {
            dir |= 2;
        }
        if (!walk) {
            dir |= 0x10;
            o->ctrl.prev[0] &= ~0x10;
        }
        break;
    }
    o->ctrl.cur &= ~0xF;
    o->ctrl.cur |= dir;
    if (o->unkBD0) {
        o->ctrl.cur = 0;
    }
    o->ctrl.pressed = o->ctrl.cur & ~o->ctrl.prev[0];
    o->ctrl.released = o->ctrl.prev[0] & ~o->ctrl.cur;
}
#endif
s32 func_0061D9A0(DrawObjFull *o);
s32 func_006337C0(DrawObjFull *o, u8 act, u8 step);
extern s16 D_003967F4[][0x1E];  /* per item: (+0) ... */

/* Whether the player may be interrupted (by act/step) in its current action. */
s32 func_006337C0(DrawObjFull *o, u8 act, u8 step) {
    switch (o->act[0]) {
    case 1:
        switch (o->act[1]) {
        case 0x1:
        case 0x2:
        case 0x3:
        case 0x4:
        case 0x5:
        case 0x6:
        case 0x7:
        case 0x8:
        case 0x31:
        case 0x9:
        case 0xA:
        case 0xB:
        case 0xC:
        case 0xD:
        case 0xE:
        case 0x32:
        case 0x98:
        case 0x99:
        case 0x9A:
        case 0x9B:
        case 0x9C:
        case 0x9D:
        case 0x9E:
        case 0x9F:
        case 0xA0:
        case 0xA3:
        case 0xA4:
        case 0xA5:
        case 0xA6:
        case 0xA7:
        case 0xA8:
        case 0xA9:
        case 0xAA:
        case 0xAB:
        case 0x74:
        case 0x75:
        case 0x76:
        case 0x77:
        case 0x78:
        case 0x79:
        case 0x7A:
        case 0x7B:
        case 0x7C:
        case 0x4A:
        case 0x4B:
        case 0x4C:
        case 0x4D:
        case 0x4E:
        case 0x4F:
        case 0x50:
        case 0x51:
        case 0x52:
        case 0x53:
        case 0x54:
        case 0x55:
        case 0xCD:
        case 0xD0:
        case 0xD1:
        case 0x81:
        case 0x82:
        case 0x83:
            return 1;
        }
        break;
    case 2:
        if (act == 2) {
            switch (step) {
            case 1:
                switch (o->act[1]) {
                case 2:
                    if (D_003967F4[o->unkC7C][0] == 0) {
                        return 1;
                    }
                    break;
                }
                break;
            case 5:
            case 6:
                switch (o->act[1]) {
                case 9:
                    return 1;
                case 2:
                    if (D_003967F4[o->unkC7C][0] == 0) {
                        return 1;
                    }
                    break;
                }
                break;
            }
        }
        switch (o->act[1]) {
        case 2:
            if (D_003967F4[o->unkC7C][0] == 0) {
                return 1;
            }
            break;
        case 0:
        case 1:
        case 7:
        case 5:
        case 6:
        case 10:
            return 1;
        }
        break;
    }
    return 0;
}
extern u8 D_0048BF93[];

/* Our player, every 30 ticks while standing or walking (action 1 and these steps): the
   func_0064C620 2 message. */
void func_00633530(DrawObjFull *o) {
    if (D_0048BF88[0] == 0 && D_0048BF93[0] % 30 == 0 && o->kind == D_0048BF7D[0]) {
        switch (o->act[0]) {
        case 1:
            switch (o->act[1]) {
            case 0:
            case 1:
            case 0x2:
            case 0x3:
            case 0x4:
            case 0x5:
            case 0x6:
            case 0x7:
            case 0x8:
            case 0x9:
            case 0xA:
            case 0xB:
            case 0xC:
            case 0xD:
            case 0xE:
            case 0x99:
            case 0x9A:
            case 0x9B:
            case 0x9C:
            case 0x9D:
            case 0x9E:
            case 0x9F:
            case 0xA0:
            case 0x24:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0xCD:
            case 0xD0:
            case 0xD1:
            case 0x81:
            case 0x82:
            case 0x83:
                func_0064C620(o, 2);
                break;
            }
            break;
        }
    }
}
void func_005A6630(DrawObjFull *o, s32 a);
extern void (*D_006DEC90[])(DrawObjFull *o);    /* per action: its step */
extern void (*D_006DECD0[])(DrawObjFull *o);    /* per action: after the step */
extern u8 D_0023BF14;           /* the action before the step */
extern u8 D_0023BF10;           /* its sub-step */
extern s8 D_0048BF88[];
extern u8 D_0048BF7D[];         /* our player number */
extern u8 D_0048BF7E[];

/* A player's frame: the position and angles before the move kept, controls and the action's
   step (for those we run), then the after-step, breathing and 0xBA0 & 0x80's func_005A6630. */
void func_006399A0(DrawObjFull *o) {
    func_00634B40(o);
    func_00634A10(o);
    *(Vec3 *)o->unk44 = *(Vec3 *)o->unk38;
    *(Rot4 *)o->unkA0 = *(Rot4 *)&o->unk90;
    func_00635310(o);
    func_00635360(o);
    if (o->type != 0 || (D_0048BF88[0] == 0 && (func_001AF510() != 1 || o->kind == D_0048BF7D[0]))) {
        D_0023BF14 = o->act[0];
        D_0023BF10 = o->act[1];
        func_005C41A0(o);
        func_0062E950(o);
        func_0062EE10(o);
        func_0062F130(o);
        func_006359D0(o);
        func_00636CF0(o);
        func_00636D50(o);
        if (o->type == 0) {
            func_006196E0(o);
            func_006307E0(o);
            func_0061E020(o);
            if (func_001AF540(o) == 1) {
                if (D_0048BF7E[0] == 0) {
                    D_006DEC90[o->act[0]](o);
                } else {
                    func_0061D9A0(o);
                }
            } else {
                D_006DEC90[o->act[0]](o);
            }
            if (o->act[0] != D_0023BF14 || o->act[1] != D_0023BF10 || o->act[2] == 0xFF) {
                if (o->act[2] == 0xFF) {
                    func_00635920(o);
                }
                switch (o->act[0]) {
                case 3:
                case 4:
                case 5:
                    func_0064C620(o, 5);
                    break;
                default:
                    if (!func_006337C0(o, D_0023BF14, D_0023BF10)) {
                        func_0064C620(o, 1);
                    }
                    break;
                }
            }
            func_00633530(o);
        }
    }
    o->unkBA0 &= ~0x800000;
    o->unkBA4 &= ~0x10100;
    if (func_001AF510() == 1) {
        if (o->unk1E2 == D_00306590.unk1C || D_0048BF88[0] != 0) {
            D_006DECD0[o->act[0]](o);
        }
    } else {
        D_006DECD0[o->act[0]](o);
    }
    o->unkBA4 &= ~0x400;
    if (o->unkBD0) {
        o->unkBA4 &= ~0x100;
    }
    func_005A61C0(o, 1.0f);
    if (o->unkBA0 & 0x80) {
        func_005A6630(o, 3);
    }
}

/* A character's per-frame movement, collision and animation steps. */
void func_00639CE0(DrawObjFull *o) {
    func_00634A10(o);
    func_0061E020(o);
    func_0058C0C0(o);
    func_005A61C0(o, 1.0f);
}

void func_0062E0F0(DrawObjFull *o);

/* Whether o's condition changed since its motion started (then action 1/0x62 plays the
   change; not while bleeding). */
#ifdef NON_MATCHING   /* register of the loaded unkB9C */
s32 func_0061DF00(DrawObjFull *o) {
    u8 cond;
    s32 r;

    if (o->unkBA0 & 0x80) {
        return 0;
    }
    func_0062E0F0(o);
    cond = o->unkB9B;
    if (o->unkBA0 & 8) {
        cond = 3;
    }
    r = 0;
    if (o->unkB9C != cond) {
        o->unk54A |= 0x10;
        o->act[0] = 1;
        o->act[1] = 0x62;
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        r = 1;
    }
    return r;
}
#endif

s32 func_006350A0(DrawObjFull *o);
void func_0061DF90(DrawObjFull *o, s32 k);
s32 func_001AF510(void);
s16 func_00635120(s32 k);
void func_0064C620(DrawObjFull *o, s32 kind);
s32 func_001AF540(DrawObjFull *o);
void func_005C4260(void);
void func_005A4490(DrawObjFull *o, s16 id, f32 a, f32 b);
void func_0055C4E0(DrawObjFull *o);

/* A character's pending hit (unkC64 2): what hit it (unkC65) decides the reaction. */
s32 func_0061E020(DrawObjFull *o) {
    s32 k;

    if (D_0048BF60.unk28 != 0) {
        return 0;
    }
    if (o->unkC64 == 2) {
        o->unkC64 = 0;
        switch (o->unkC65) {
        case 1:
            k = func_006350A0(o);
            if ((D_0048BF60.unkB8[1] & 2) || (o->unkB9B != 2 && !(o->unkBA0 & 8))
                || (o->unkBA4 & 0x10000) || o->unkBD0 != 0) {
                func_0061DF90(o, k);
                return 1;
            }
            switch (o->act[0]) {
            case 1:
                switch (o->act[1]) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 0xA:
                case 0xB:
                case 0xC:
                case 0xD:
                case 0xE:
                case 0x31:
                case 0x32:
                    if (func_001AF510() == 1) {
                        switch (o->type) {
                        case 0:
                            o->unkC64 = 2;
                            o->unkC65 = 3;
                            o->unkC66 = func_00635120(k);
                            func_0064C620(o, 8);
                            break;
                        case 1:
                            break;
                        }
                    }
                    o->unkC64 = 0;
                    o->unkC65 = 0;
                    o->unkC66 = func_00635120(k);
                    o->unkBA0 |= 0x80000;
                    o->act[0] = 1;
                    o->act[1] = 0xAC;
                    o->act[2] = 0;
                    o->act[3] = 0;
                    func_00635920(o);
                    break;
                default:
                    func_0061DF90(o, k);
                    return 1;
                }
                break;
            default:
                func_0061DF90(o, k);
                return 1;
            }
            break;
        case 2:
            o->unkBA0 &= ~0x80000;
            o->act[0] = 1;
            o->act[1] = 0;
            o->act[2] = 0;
            o->act[3] = 0;
            func_00635920(o);
            func_0064C620(o, 1);
            o->unk54F = 0;
            o->unk548 &= ~1;
            o->act[0] = 1;
            o->act[1] = 0;
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            func_005A4490(o, 0x2050, 0.0f, 10.0f);
            func_0055C4E0(o);
            return 1;
        case 3:
            if (o->unk54F == 0x16) {
                o->act[0] = 1;
                o->act[1] = 0xA1;
                o->act[2] = 2;
                o->act[3] = 0;
                func_00635920(o);
                func_0064C620(o, 1);
            }
            break;
        case 4:
            if (o->unk54F == 0x16 && o->act[1] != 0xA2) {
                o->act[0] = 1;
                o->act[1] = 0xA2;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
                func_0064C620(o, 1);
            }
            break;
        default:
            return 0;
        }
        if (func_001AF540(o) == 1) {
            func_005C4260();
        }
        return 1;
    }
    return 0;
}

void func_005C5A90(DrawObjFull *o, s32 a);
void func_0062F130(DrawObjFull *o);
void func_00639D50(DrawObjFull *o);
void func_0058C240(DrawObjFull *o);

/* Death (health 0: falls, action 4), then the action's step: actions 3 and up (dying, down)
   on their own, else the usual movement. */
#ifdef NON_MATCHING   /* branch to the epilogue (delay slot) */
void func_0058C0C0(DrawObjFull *o) {
    if (o->unk544 == 0) {
        switch (o->act[0]) {
        case 3:
        case 4:
        case 5:
            break;
        default:
            if (o->unkBA4 & 2) {
                o->unkBA0 |= 0x60;
                func_005C5A90(o, 0);
                o->unk54F = 0xA0;
                o->act[0] = 4;
                o->act[1] = 1;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
            } else {
                func_005C5A90(o, 0);
                o->unk54F = 0xA0;
                o->act[0] = 4;
                o->act[1] = 0;
                o->act[2] = 0;
                o->act[3] = 0;
                func_00635920(o);
            }
            break;
        }
    }
    func_00637C60(o);
    func_0062F130(o);
    if (o->act[0] >= 3) {
        o->unkBA0 |= 0x100000;
        func_00639D50(o);
        if (o->act[0] < 3) {
            o->wayStep = 0;
            o->unkFB9[0] = 0;
            o->unkFB9[1] = 0;
            o->unkFB9[2] = 0;
        }
    } else {
        o->unkBA0 &= ~0x100000;
        if (o->unkBA0 & 0x80000) {
            func_00639D50(o);
        } else {
            func_0058C240(o);
        }
    }
}
#endif

#include "item.h"

s32 func_001AF340(u8 player);
void func_0034B7D0(s32 a, s32 b, s32 c);
void func_0062ED10(DrawObjFull *o, s32 a);
void func_0064E360(DrawObjFull *o, s32 a, u8 b);
void func_005C4EE0(DrawObjFull *o, s32 a, u8 b);
void func_0062CDA0(DrawObjFull *o);
void func_00634BE0(DrawObjFull *o);
void func_00635340(DrawObjFull *o);
void func_0062E6A0(DrawObjFull *o, u16 a, s32 b);
void func_006AC7D0(void);
void func_00636F90(DrawObjFull *o);
s32 func_0062EFC0(DrawObjFull *o);
void func_00634260(DrawObjFull *o, s32 a);
s32 func_0062E5B0(DrawObjFull *o, u16 dmg, s32 a);
extern s16 D_0023AE70[];
extern s8 D_00389694;
extern DrawObjFull D_00476DD0[];
void *memset(void *p, s32 c, u32 n);

/* A hit taken this frame (0x13F set): falls in a pit (kind-2 floor, 9), bleeds (status 0x80),
   or takes the damage (0x138) and reacts with action 3 (or 5 for strong hits, 0x13D >= 0x80). */
void func_0062F130(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft */
void func_0062F130(DrawObjFull *o) {
    s16 hp;
    s32 id;

    if (D_0048BF60.unk28 != 0) {
        FIELD(o, u8, 0x13F) = 0;
        return;
    }
    if (FIELD(o, u8, 0x13F) == 0) {
        return;
    }
    FIELD(o, u8, 0x13F) = 0;
    FIELD(o, u8, 0x115) |= 1;
    if (o->type == 0 && FIELD(o, u8, 0x150) == 2 && FIELD(o, u8, 0x13D) == 9) {
        o->unkBA0 |= 0x40000800;
        o->unk544 = 0;
        o->unkBA0 |= 0x20;
        func_005C5A90(o, 0);
        o->unk54F = 0xA0;
        o->act[0] = 4;
        o->act[1] = 0;
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        FIELD(o, u8, 0x115) |= 1;
        func_0064C620(o, 5);
        return;
    }
    if (o->unkBA0 & 0x200008) {
        return;
    }
    if (o->type == 1 && func_001AF340(D_0048BF60.unk1D) == 0) {
        return;
    }
    if (o->unkBD0 != 0 && FIELD(o, u8, 0x148) == 0) {
        return;
    }
    if (o->unkBA0 & 0x80) {
        if ((s16)FIELD(o, u16, 0xC5E) - D_0023AE70[FIELD(o, u8, 0x140)] <= 0) {
            FIELD(o, u16, 0xC5E) = 1;
        } else {
            FIELD(o, u16, 0xC5E) -= D_0023AE70[FIELD(o, u8, 0x140)];
        }
        if (FIELD(o, u8, 0x13D) >= 0x10) {
            FIELD(o, u8, 0x13D) = 1;
        }
        o->act[0] = 3;
        o->act[1] = FIELD(o, u8, 0x13D);
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        FIELD(o, u8, 0x115) |= 1;
        return;
    }
    if (FIELD(o, u8, 0x148) == 0 && func_001AF540(o) == 1
        && (D_00476DD0[FIELD(o, u8, 0x149) & 3].unkBA0 & 0x80)) {
        func_0034B7D0(0x25, D_00476DD0[FIELD(o, u8, 0x149)].unkB98, 0);
    }
    if ((FIELD(o, u8, 0x143) & 2) && !(o->unkBA0 & 0x400000)) {
        func_0062ED10(o, 1);
    }
    if ((FIELD(o, u8, 0x143) & 4) && !(o->unkBA0 & 0x400000)) {
        func_0062ED10(o, 2);
    }
    o->unkC64 = 0;
    o->unkAB8 = 0;
    o->unkAD4 = 0;
    if (func_001AF540(o) == 1) {
        func_005C4260();
    }
    if (o->type == 0 && o->act[0] == 1 && o->act[1] == 0x16) {
        if (o->type == 0) {
            id = o->kind + 1;
        } else {
            id = o->unkBB4 + 5;
        }
        if ((u16)D_003967D0[FIELD(o, u8, 0xC7A)].owner != id) {
            func_0064E360(o, 2, FIELD(o, u8, 0xC7A));
        }
    }
    if (o->type == 0 && o->act[0] == 1 && o->act[1] == 0x24) {
        func_0064E360(o, 1, FIELD(o, u8, 0xC7F));
        func_005C4EE0(o, 0, FIELD(o, u8, 0xC7F));
    }
    if (o->type == 0 && o->act[0] == 2 && o->act[1] == 0xB) {
        o->unkBA0 |= 0x80000000;
        func_0062CDA0(o);
    }
    func_00634BE0(o);
    func_00635340(o);
    if (FIELD(o, u16, 0x14E) != 0) {
        func_0062E6A0(o, FIELD(o, u16, 0x14E), 0);
    }
    if (func_001AF540(o) == 1) {
        func_006AC7D0();
    }
    func_00636F90(o);
    if (func_001AF540(o) == 1 && !(FIELD(o, u8, 0x143) & 1)) {
        D_00389694 = 1;
    }
    if (FIELD(o, u8, 0x13D) >= 0x80) {
        if (o->unkBA0 & 0x800000) {
            return;
        }
        if (o->unkBA0 & 0xB8) {
            return;
        }
        if (func_0062EFC0(o) != 0) {
            return;
        }
        o->act[0] = 5;
        o->act[1] = FIELD(o, s8, 0x13D) - 0x80;
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        memset(&FIELD(o, u8, 0xB1C), 0, 0x18);
    } else {
        hp = o->unk544;
        if (FIELD(o, s32, 0x138) > 0) {
            func_00634260(o, 3);
        }
        if (func_0062E5B0(o, FIELD(o, s32, 0x138), 0) != 0) {
            o->unk544 = 0;
            if (func_0062EFC0(o) != 0) {
                return;
            }
            if (hp >= 2) {
                o->unk544 = 1;
            }
        } else if (func_0062EFC0(o) != 0) {
            return;
        }
        o->act[0] = 3;
        o->act[1] = FIELD(o, u8, 0x13D);
        o->act[2] = 0;
        o->act[3] = 0;
        func_00635920(o);
        func_0064C620(o, 5);
    }
    FIELD(o, u8, 0x115) |= 1;
    if (func_001AF510() == 1 && o->type == 1) {
        o->unkBA0 |= 0x10000000;
        func_0064C620(o, 5);
        o->unkBA0 &= ~0x10000000;
    }
}
#endif

void func_0058C380(DrawObjFull *o);
void func_0058C6B0(DrawObjFull *o);
void func_0058C880(DrawObjFull *o);
void func_0058CA60(DrawObjFull *o);
void func_0058CCB0(DrawObjFull *o);
void func_0058CD90(DrawObjFull *o);
void func_0058CE70(DrawObjFull *o);
void func_0058CF40(DrawObjFull *o);
void func_0058D170(DrawObjFull *o);
void func_0058D250(DrawObjFull *o);
void func_0058D300(DrawObjFull *o);
void func_0058D420(DrawObjFull *o);
void func_0058D500(DrawObjFull *o);
void func_0058D740(DrawObjFull *o);
void func_0058D8F0(DrawObjFull *o);

/* The movement step of a character's current way of moving (0xFB7). */
void func_0058C240(DrawObjFull *o) {
    switch (o->way) {
    case 0:
        func_0058C380(o);
        break;
    case 1:
        func_0058C6B0(o);
        break;
    case 2:
        func_0058C880(o);
        break;
    case 3:
        func_0058CA60(o);
        break;
    case 4:
        func_0058CCB0(o);
        break;
    case 5:
        func_0058CD90(o);
        break;
    case 6:
        func_0058CE70(o);
        break;
    case 7:
        func_0058CF40(o);
        break;
    case 8:
        func_0058D170(o);
        break;
    case 9:
        func_0058D250(o);
        break;
    case 10:
        func_0058D300(o);
        break;
    case 11:
        func_0058D420(o);
        break;
    case 12:
        func_0058D500(o);
        break;
    case 13:
        func_0058D740(o);
        break;
    case 14:
        func_0058D500(o);
        break;
    case 15:
        func_0058D8F0(o);
        break;
    case 16:
        break;
    }
}

/* A character's step, by its action (act[0]) and then the action's kind (act[1]): tables of
   handlers, each reached by a tail call. */
typedef void (*CharStep)(DrawObjFull *o);

extern CharStep D_006DECD0[];
extern CharStep D_006DED10[];
void func_005E91A0(DrawObjFull *o);
void func_001AF1C0(u8 player);
s32 func_00635970(DrawObjFull *o);
s32 func_0061DF00(DrawObjFull *o);
void func_00635950(DrawObjFull *o);
s8 func_0055C030(DrawObjFull *o, f32 speed);
void func_00639DF0(DrawObjFull *o);

void func_00639D50(DrawObjFull *o) {
    D_006DECD0[o->act[0]](o);
}

/* Action 0: the character appears (then the action's kind runs at once). */
void func_00639D70(DrawObjFull *o) {
    o->act[0]++;
    o->visible = 1;
    func_005E91A0(o);
    o->unkBA4 |= 0x8000000;
    if (o->type == 0) {
        o->unkBD0 = 0;
    }
    func_001AF1C0(o->kind);
    func_00639DF0(o);
}

void func_00639DF0(DrawObjFull *o) {
    o->unkBA0 &= ~0x200;
    D_006DED10[o->act[1]](o);
}

/* Kind 0: standing (its motion started, then looped). */
void func_00639E20(DrawObjFull *o);
#ifdef NON_MATCHING   /* first draft: delay slots in the case tests */
void func_00639E20(DrawObjFull *o) {
    o->unkBA4 |= 0x100;
    o->unk548 &= ~1;
    if (func_00635970(o)) {
        o->act[2] = 0;
    }
    if (func_0061DF00(o)) {
        return;
    }
    switch (o->act[2]) {
    case 0:
        o->act[2]++;
        o->unk54F = 0;
        o->unkD10[0] = 0;
        func_00635950(o);
        o->unkC0[0] = 0.0f;
        o->unkC0[1] = 0.0f;
        o->unkC0[2] = 0.0f;
        o->unkBA8 = 1.0f;
        func_005A4490(o, 0x2050, 0.0f, 0.0f);
        func_0055C4E0(o);
        break;
    case 1:
        o->act[2] = 10;
        o->unk54F = 0;
        o->unkD10[0] = 0;
        func_00635950(o);
        o->unkBA8 = 1.0f;
    case 10:
        if (func_0055C030(o, 1.0f) == 1) {
            o->act[2]++;
            if (o->unk520 != 0x2001) {
                func_005A4490(o, 0x2051, 0.0f, 8.0f);
                func_0055C4E0(o);
            }
        }
        break;
    case 11:
        func_0055C030(o, 1.0f);
        break;
    }
}
#endif

/* Way 0 (standing about): after two frames, a character not busy with something (actions 3-5,
   7) goes back to standing. */
#ifdef NON_MATCHING   /* branch layout of the case-1 test and delay slots */
void func_0058C380(DrawObjFull *o) {
    switch (o->wayStep) {
    case 0:
        o->wayStep++;
    case 1:
        o->wayStep++;
        switch (o->act[0]) {
        case 1:
            if (o->act[1] == 0) {
                break;
            }
        default:
            o->unk54F = 0;
            o->unk548 &= ~1;
            o->act[0] = 1;
            o->act[1] = 0;
            o->act[2] = 1;
            o->act[3] = 0;
            func_00635920(o);
            func_005A4490(o, 0x2050, 0.0f, 10.0f);
            func_0055C4E0(o);
            func_00639DF0(o);
            break;
        case 3:
        case 4:
        case 5:
        case 7:
            break;
        }
        break;
    case 2:
        func_00639D50(o);
        break;
    case 3:
        o->unkFCC = 0;
        break;
    }
}
#endif

typedef struct {
    u8 heavy;           /* 0x00 */
    u8 unk1;            /* 0x01: its turn-speed column (action 0x80) */
    u8 unk2[0x42];
} WeaponTurn;

extern WeaponTurn D_006E5944[];
extern s16 D_006E7210[][13];    /* (action 0x80) by condition */
extern s16 *D_006E72E0[];       /* by the action's kind: [condition * 5 + heavy] */
extern s16 *D_006E7690[];       /* (the same while 0xBA0 & 0x2000) */
extern s16 D_006E76C0[];        /* (0xBA0 & 0x80) by the action's kind */
extern s16 D_006E7280[][8];     /* 0x8000 | n: per character */
extern s16 D_006E7630[][8];
f32 func_00191A70(f32 x);

/* How fast the character turns this frame (0x10000 a turn): by its action, condition and
   weapon, doubled at 30 fps; while aiming (0xBA0 & 0x2000) scaled by how far it has to turn. */
s16 func_0062DBC0(DrawObjFull *o);
#ifdef NON_MATCHING   /* close: delay slots in the action switch and the tail's returns */
s16 func_0062DBC0(DrawObjFull *o) {
    s16 sp;
    u8 cond;
    s16 t;
    f32 f;
    s16 d;
    s16 r;
    u8 w;

    cond = o->unkB9B;
    if (o->unkD90 != 0) {
        return 0;
    }
    if (o->unkBA0 & 0x18) {
        cond = 3;
    }
    if (o->unkBA0 & 0x20) {
        cond = 0;
    }
    switch (o->unk54F) {
    case 0xA:
    case 0:
        f = o->unkBA8;
        t = 0;
        break;
    case 1:
        f = 1.0f;
        t = 1;
        break;
    case 2:
        f = 1.0f;
        t = 2;
        break;
    case 3:
        f = 1.0f;
        t = 3;
        break;
    case 4:
        f = 1.0f;
        t = 4;
        break;
    case 7:
    case 0x2D:
    case 0x2F:
    case 0x2E:
        f = 1.0f;
        t = 5;
        break;
    case 0x80:
        f = o->unkBA8;
        t = 0;
        break;
    case 0x21:
        f = o->unkBA8;
        t = 6;
        break;
    case 0x22:
        f = 1.0f;
        t = 7;
        break;
    case 0x23:
        f = 1.0f;
        t = 8;
        break;
    case 0x24:
        f = 1.0f;
        t = 9;
        break;
    default:
        t = 0;
        f = 1.0f;
        break;
    }
    switch (o->unk54F) {
    default:
        w = D_006E5944[(u16)o->unkCA8].heavy;
        if (o->unkBA0 & 0x2000) {
            sp = D_006E7690[t][cond * 10 + w];
        } else {
            sp = D_006E72E0[t][cond * 10 + w];
        }
        if (o->unkBA0 & 0x80) {
            sp = D_006E76C0[t];
        }
        break;
    case 0x80:
        sp = D_006E7210[cond][D_006E5944[(u16)o->unkCA8].unk1];
        break;
    }
    if (sp & 0x8000) {
        if (o->unkBA0 & 0x2000) {
            sp = D_006E7630[sp & 0x7FFF][o->unkB98];
        } else {
            sp = D_006E7280[sp & 0x7FFF][o->unkB98];
        }
    }
    sp *= (s16)(D_00306590.unkC ? 2 : 1);
    if (!(f <= 0.0f)) {
        sp = sp * f;
    }
    if (!(o->unkBA0 & 0x2000)) {
        return sp;
    }
    if (o->unk54F == 0x80) {
        return sp;
    }
    switch (o->act[1]) {
    case 4:
    case 5:
    case 7:
    case 8:
        if (FIELD(o, f32, 0x1E4) < FIELD(o, f32, 0x1EC)) {
            sp *= 2;
        }
        break;
    }
    switch (o->act[1]) {
    case 9:
    case 10:
    case 11:
        if (func_00191A70(o->unkBCE - 0x8000 - o->unk92) <= sp) {
            sp = func_00191A70(o->unkBCE - 0x8000 - o->unk92);
        }
        return sp;
    }
    d = o->unkBCE - o->unk92;
    if (d == -0x8000) {
        d = 0x7FFF;
    }
    r = func_00191A70(d);
    if (r < sp) {
        sp = r;
    } else if (r > 0x6000) {
        sp *= 4;
    } else if (r > 0x4000) {
        sp *= 2;
    }
    return sp;
}
#endif
f32 func_005A41B0(DrawObjFull *o, u16 motion, s32 a);
void func_0055C520(DrawObjFull *o);
void func_00635920(DrawObjFull *o);
extern f32 D_00239DFC;

/* Kind 1/2 of action 1: turning on the spot (act[1] 1 one way, else the other). */
void func_00639FA0(DrawObjFull *o) {
    s16 m;

    o->unkBA4 |= 0x100;
    if (func_00635970(o)) {
        o->unk54F = 0;
        o->unk548 &= ~1;
        o->act[0] = 1;
        o->act[1] = 0;
        o->act[2] = 1;
        o->act[3] = 0;
        func_00635920(o);
        func_005A4490(o, 0x2050, 0.0f, 10.0f);
        func_0055C4E0(o);
        return;
    }
    if (o->act[1] == 1) {
        o->unk92 -= func_0062DBC0(o);
    } else {
        o->unk92 += func_0062DBC0(o);
    }
    switch (o->act[2]) {
    case 0:
        o->act[2]++;
        o->unk54F = 0;
        o->unkD10[0] = 1;
        func_00635950(o);
        o->unkBA8 = 1.0f;
        if (o->unkBA0 & 8) {
            o->unkBA8 = 1.0f;
        }
        if (o->unkBA0 & 0x80) {
            o->unkBA8 = D_00239DFC;
        }
    case 1:
        o->act[2]++;
        if (!(o->unkBA0 & 0x2000)) {
            func_0055C520(o);
            switch (o->unk520) {
            case 0x2003:
                m = 0x2003;
                break;
            case 0x2004:
                m = 0x2004;
                break;
            default:
                m = 0x2003;
                break;
            }
            if (o->unk520 != m) {
                func_005A4490(o, m, func_005A41B0(o, m, 0), 14.0f);
                func_0055C4E0(o);
            } else {
                func_0055C030(o, o->unkBA8);
                func_0055C4E0(o);
            }
        } else {
            func_0055C030(o, o->unkBA8);
        }
        o->unk4E4[0] = 0;
        o->unk4E4[1] = 0;
        o->unk4E4[2] = 0;
        break;
    case 2:
        func_0055C030(o, o->unkBA8);
        o->unk4E4[0] = 0;
        o->unk4E4[1] = 0;
        o->unk4E4[2] = 0;
        break;
    case 0xFF:
    default:
        o->unk54F = 0;
        o->unk548 &= ~1;
        o->act[0] = 1;
        o->act[1] = 0;
        o->act[2] = 1;
        o->act[3] = 0;
        func_00635920(o);
        func_005A4490(o, 0x2050, 0.0f, 12.0f);
        func_0055C4E0(o);
        break;
    }
}
