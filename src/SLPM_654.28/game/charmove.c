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

/* The head's turn (from the skeleton's head bone) as angles. */
void func_0062F930(DrawObjFull *o) {
    u8 *m = (u8 *)o->model;

    FIELD(o, s16, 0xBD8) = 0.5f + 65536.0f * FIELD(FIELD(m, u8 *, 0x874), f32, 0x110) / D_00239E18;
    FIELD(o, s16, 0xBDA) = 0.5f + 65536.0f * FIELD(FIELD(m, u8 *, 0x874), f32, 0x114) / D_00239E18;
    FIELD(o, s16, 0xBD6) = 0;
    FIELD(o, s16, 0xBD4) = 0;
    o->unkBD1[0] = 0;
    FIELD(o, s16, 0xBD2) = 0;
    FIELD(o, s16, 0xC30) = 0;
    FIELD(o, s32, 0xBE0) = 0;
}

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
void func_00340310(s32 rx, s32 ry, s32 rz, Mat44 *m);

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
extern u8 D_0023ABA0[];         /* per character kind */
extern u8 D_0023ABA8[];         /* (the zombie forms) */
extern u8 D_006DB4FF[];         /* per NPC model (from 1) */
extern u8 D_006DB500[];
extern u8 D_006DB5E0[];
extern void *D_006DB880[];
void func_005E9CF0(void *bones, void *face, u8 n);

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

#include "zone.h"

extern u8 D_00470090[0x6D40];
s32 func_00340940(Zone *z, s32 *xz, s32 n);
void func_005910F0(Zone *z, s32 hit);

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
            func_005910F0(z, hit);
        }
    }
}
#endif

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

void func_0062E950(void);
void func_006359D0(DrawObjFull *o);
void func_00636CF0(DrawObjFull *o);
void func_00636D50(DrawObjFull *o);

/* (Offline) a player not in our scenario: kept still. */
void func_00637B40(DrawObjFull *o) {
    o->unkBA4 |= 0x100;
    func_0062E950();
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
f32 func_00340100(u16 a);

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

typedef struct {
    s16 v[4];
} Rot4;

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
    u8 unk4[0x5C];
    u8 *unk60;
    u8 unk64[4];
    u8 *unk68;
} ModelBones;

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
void func_006AC7D0(DrawObjFull *o);
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
        func_006AC7D0(o);
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
