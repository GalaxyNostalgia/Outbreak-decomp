#include "common.h"
#include "game.h"
#include "object.h"

/* Two-bone chains (arms) the character's model bends towards a target: a chain remembers the
   bones' rest orientations relative to the plane they lie in, their lengths and the up vector,
   and hooks its three bones' update callbacks (0x178/0x17C). */


void func_0018F830(f32 x, f32 y, f32 z, f32 *m);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
void func_0018FA30(f32 x, f32 y, f32 z, f32 *m);
void func_00190ED0(f32 *dst, const f32 *src);
void func_00190F50(f32 *m, const f32 *a, const f32 *b);
void func_0018F7C0(f32 *m);
void func_0018FAD0(f32 a, f32 *m);
void func_00190C50(f32 *out, const f32 *a, const f32 *b);
void func_00191030(f32 *m, const f32 *a, const f32 *b);
void func_00190E00(f32 *out, const f32 *m);
f32 func_00341860(f32 x);
void func_005A5140(u8 *bone, f32 *parent, f32 *scale, f32 px, f32 py, f32 pz);
void func_005A5250(void *root, f32 *m);
void func_005A5370(IkChain *ik, u8 *bone, f32 *parent);
void func_005A5380(IkChain *ik, u8 *bone, f32 *parent);

/* The world matrix of a bone: scale, then the bone's position moved by the scale's translation. */
#define BONE_MATRIX(out, b)                                                         \
    func_001913A0(p, (f32 *)((b) + 0xF4), m);                                       \
    p[0] += m[12];                                                                  \
    p[1] += m[13];                                                                  \
    p[2] += m[14];                                                                  \
    func_0018FA30(p[0], p[1], p[2], m);                                             \
    func_00190ED0(out, m)

void func_005A5740(IkChain *ik, s32 mode, u8 *b0, u8 *b1, u8 *b2, f32 *scale, f32 angle, f32 x,
                   f32 y, f32 z);
#ifdef NON_MATCHING   /* first draft */
void func_005A5740(IkChain *ik, s32 mode, u8 *b0, u8 *b1, u8 *b2, f32 *scale, f32 angle, f32 x,
                   f32 y, f32 z) {
    f32 m[16];
    f32 r[16];
    f32 w0[16];
    f32 w1[16];
    f32 w2[16];
    f32 t[16];
    f32 inv[16];
    f32 sc[3];
    f32 up[3];
    f32 d[3];
    f32 p[3];
    f32 n[3];
    f32 len0;
    f32 len1;

    sc[0] = scale[0];
    sc[1] = scale[1];
    sc[2] = scale[2];
    up[0] = x;
    up[1] = y;
    up[2] = z;
    func_0018F830(sc[0], sc[1], sc[2], m);
    BONE_MATRIX(w0, b0);
    BONE_MATRIX(w1, b1);
    BONE_MATRIX(w2, b2);
    d[0] = w2[12] - w0[12];
    d[1] = w2[13] - w0[13];
    d[2] = w2[14] - w0[14];
    func_00190F50(m, d, up);
    func_0018F7C0(r);
    func_0018FAD0(-angle, r);
    func_00190C50(m, r, m);
    n[0] = m[8];
    n[1] = m[9];
    n[2] = m[10];
    d[0] = w1[12] - w0[12];
    d[1] = w1[13] - w0[13];
    d[2] = w1[14] - w0[14];
    len0 = func_00341860(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
    func_00190ED0(t, w0);
    func_00191030(t, d, n);
    func_00190E00(inv, t);
    func_00190C50(ik->m0, w0, inv);
    d[0] = w2[12] - w1[12];
    d[1] = w2[13] - w1[13];
    d[2] = w2[14] - w1[14];
    len1 = func_00341860(d[0] * d[0] + d[1] * d[1] + d[2] * d[2]);
    func_00190ED0(t, w1);
    func_00191030(t, d, n);
    func_00190E00(inv, t);
    func_00190C50(ik->m1, w1, inv);
    ik->mode = mode;
    ik->on = 1;
    ik->up[0] = x;
    ik->up[1] = y;
    ik->up[2] = z;
    ik->angle = angle;
    ik->b0 = b0;
    ik->b1 = b1;
    ik->len0 = len0;
    ik->len1 = len1;
    ik->unkBC = 0.0f;
    *(void **)(b0 + 0x178) = func_005A5380;
    *(void **)(b0 + 0x17C) = ik;
    *(void **)(b1 + 0x178) = func_005A5370;
    *(void **)(b1 + 0x17C) = ik;
    *(void **)(b2 + 0x178) = func_005A5140;
    *(void **)(b2 + 0x17C) = ik;
}
#endif

/* The character's two arm chains (obj + 0x560 and + 0x620), when its skeleton is loaded. */
extern f32 D_00239E00;
extern f32 D_00239E04;

void func_005A5FC0(DrawObjFull *obj);
#ifdef NON_MATCHING   /* first draft */
void func_005A5FC0(DrawObjFull *obj) {
    u8 *bones;

    if (D_0048BF60.unk28 != 0) {
        return;
    }
    bones = (u8 *)obj->model + 0x820;
    if (*(s32 *)bones == 0 || bones == NULL) {
        return;
    }
    func_005A5740((IkChain *)((u8 *)obj + 0x560), 2, *(u8 **)(bones + 0x1C), *(u8 **)(bones + 0x20),
                  *(u8 **)(bones + 0x24), obj->unkB4, D_00239E00, 1.0f, 0.0f, 0.0f);
    func_005A5740((IkChain *)((u8 *)obj + 0x620), 2, *(u8 **)(bones + 0x10), *(u8 **)(bones + 0x14),
                  *(u8 **)(bones + 0x18), obj->unkB4, D_00239E04, -1.0f, 0.0f, 0.0f);
}
#endif

void func_00346B30(DrawObjFull *o);
void func_0018FAB0(f32 *out, const f32 *m);
f32 func_005E5B40(const f32 *p, u8 *kind, s32 want);
extern f32 D_006D7560[];        /* per character: its feet's height above the floor */
extern f32 D_006D7580[];        /* (in danger) */
extern f32 D_0023BE78;          /* the feet's height this frame */
extern f32 D_0023BE74;
extern s8 D_0048BF88[];

typedef struct {
    u8 unk0[8];
    f32 scale;          /* 0x08 */
    u8 unkC[8];
} IkNpcModel;           /* 0x14 */

extern IkNpcModel D_006E2490[];     /* per NPC model (from 1) */
extern f32 D_006E6CB0[][3];         /* per character: its size */

/* The feet on the floor: with any chain on, each leg's chain (0x560, 0x620) aims at its foot
   and lifts it to the floor under it (0xAA0, 0xAA4), then the skeleton is posed again when
   one moved. */
void func_005A5BA0(DrawObjFull *o);
#ifdef NON_MATCHING   /* close: the original keeps two branches for the same floor kind (0xFF) that ours folds */
void func_005A5BA0(DrawObjFull *o) {
    f32 m110[16];
    f32 mD0[16];
    f32 m90[16];
    f32 m50[16];
    f32 p[3];
    u8 k;
    u8 *bones = (u8 *)o->model + 0x820;
    u32 i;
    u8 n;
    s32 want;
    u8 r;
    f32 *py;

    if (D_0048BF88[0]) {
        func_00346B30(o);
        return;
    }
    n = 0;
    for (i = 0; i < 7; i++) {
        n += o->ik[i].on != 0;
    }
    if (!n) {
        func_00346B30(o);
        return;
    }
    switch (o->type) {
    case 0:
    case 1:
        D_0023BE78 = o->unkB9B == 3 ? D_006D7580[o->unkB98] : D_006D7560[o->unkB98];
        D_0023BE78 += o->unkAA8 * (o->unkB4[1] / (o->unkBB4 ? D_006E2490[o->unkBB4 - 1].scale : D_006E6CB0[o->unkB98][0]));
        D_0023BE74 = 0.0f;
        break;
    default:
        D_0023BE78 = 10.0f;
        D_0023BE78 += o->unkAA8;
        D_0023BE74 = 0.0f;
        break;
    }
    func_00346B30(o);
    if (!(o->ik[0].on | o->ik[1].on)) {
        return;
    }
    func_00190ED0(m90, *(f32 **)(bones + 0x1C));
    func_00190ED0(m110, *(f32 **)(bones + 0x24));
    func_00190ED0(m50, *(f32 **)(bones + 0x10));
    func_00190ED0(mD0, *(f32 **)(bones + 0x18));
    func_0018FAB0(o->ik[0].tgt, m110);
    o->ik[0].up[0] = m90[12] - 50.0f * m90[0];
    o->ik[0].up[1] = m90[13] - 50.0f * m90[1];
    o->ik[0].up[2] = m90[14] - 50.0f * m90[2];
    func_0018FAB0(o->ik[1].tgt, mD0);
    o->ik[1].up[0] = m50[12] + 50.0f * m50[0];
    o->ik[1].up[1] = m50[13] + 50.0f * m50[1];
    o->ik[1].up[2] = m50[14] + 50.0f * m50[2];
    switch (o->type) {
    case 0:
        if (o->unk54F == 0) {
            want = 0xFF;
        } else {
            want = 0xFF;
        }
        break;
    case 1:
        want = 0xFF;
        break;
    case 2:
        want = 0xFF;
        break;
    }
    py = &p[1];
    *(Vec3 *)p = *(Vec3 *)o->ik[0].tgt;
    *py = o->unk38[1];
    if (o->unk174 == 1) {
        o->unkAA0 = func_005E5B40(p, &k, want);
        o->unkAA0 += D_0023BE78 + D_0023BE74;
    } else {
        o->unkAA0 = func_005E5B40(p, &k, 0xFF);
        o->unkAA0 += D_0023BE78;
    }
    *(Vec3 *)p = *(Vec3 *)o->ik[1].tgt;
    *py = o->unk38[1];
    if (o->unk174 == 1) {
        o->unkAA4 = func_005E5B40(p, &k, want);
        o->unkAA4 += D_0023BE78 + D_0023BE74;
    } else {
        o->unkAA4 = func_005E5B40(p, &k, 0xFF);
        o->unkAA4 += D_0023BE78;
    }
    r = 0;
    if (o->ik[0].tgt[1] < o->unkAA0) {
        o->ik[0].tgt[1] = o->unkAA0;
        r = 1;
    }
    if (o->ik[1].tgt[1] < o->unkAA4) {
        r |= 2;
        o->ik[1].tgt[1] = o->unkAA4;
    }
    if (r) {
        func_005A5250(*(void **)bones, (f32 *)((u8 *)o + 0x50));
    }
}
#endif

/* Updates bone and its children and siblings from parent: each with an IK chain on it runs its
   callback (0x178; func_005A5140 also gets the scale and may change it for the children). */
void func_005A5270(u8 *bone, f32 *parent, f32 sx, f32 sy, f32 sz) {
    IkChain *ik;
    void (*cb)();
    f32 *pz;
    f32 *py;
    f32 s[3];

    py = &s[1];
    pz = &s[2];
    s[0] = sx;
    *py = sy;
    *pz = sz;
    ik = *(IkChain **)(bone + 0x17C);
    if (ik != NULL && ik->on) {
        cb = *(void (**)())(bone + 0x178);
        if (cb == (void (*)())func_005A5140) {
            func_005A5140(bone, parent, s, sx, sy, sz);
        } else {
            cb(ik, bone, parent);
        }
    }
    if (*(u8 **)(bone + 0xD0) != NULL) {
        func_005A5270(*(u8 **)(bone + 0xD0), (f32 *)bone, s[0], *py, *pz);
    }
    if (*(u8 **)(bone + 0xCC) != NULL) {
        func_005A5270(*(u8 **)(bone + 0xCC), parent, sx, sy, sz);
    }
}

/* Updates the skeleton from its root bone under the world matrix m. */
void func_005A5250(void *root, f32 *m) {
    func_005A5270(root, m, 1.0f, 1.0f, 1.0f);
}

void func_00190D50(f32 *out, const f32 *a, const f32 *b);
void func_00190CD0(f32 *a, const f32 *b);
extern Vec3 D_006D7550;         /* (1, 1, 1) */

/* A bone's world matrix from its parent's (whose scale px, py, pz it takes back out): its own
   scale (0x104, also into scale for its children), turn (0x40) and place (0x70). */
#ifdef NON_MATCHING   /* stack slot order of the four matrices; trailing nop */
void func_005A5140(u8 *bone, f32 *parent, f32 *scale, f32 px, f32 py, f32 pz) {
    f32 m50[16];
    f32 m90[16];
    f32 mD0[16];
    f32 m110[16];

    func_0018F7C0(m110);
    scale[0] = *(f32 *)(bone + 0x104);
    scale[1] = *(f32 *)(bone + 0x108);
    scale[2] = *(f32 *)(bone + 0x10C);
    func_0018F830(scale[0], scale[1], scale[2], m110);
    func_0018F7C0(mD0);
    mD0[0] = 1.0f / px;
    mD0[5] = 1.0f / py;
    mD0[10] = 1.0f / pz;
    mD0[12] = *(f32 *)(bone + 0x70);
    mD0[13] = *(f32 *)(bone + 0x74);
    mD0[14] = *(f32 *)(bone + 0x78);
    func_0018F7C0(m90);
    func_00190D50(m90, m110, (f32 *)(bone + 0x40));
    func_00190C50(m50, m90, mD0);
    func_00190C50((f32 *)bone, m50, parent);
}
#endif

/* The chain's middle bone: posed with its first (func_005A5380). */
void func_005A5370(IkChain *ik, u8 *bone, f32 *parent) {
}

/* The chain's first bone: aims the two bones at the target (0xAC), bending at the middle by
   the cosine rule, or straight when it is out of reach; the up vector by mode (0 none, 1 in
   the parent's frame, 2 a point). */
#ifdef NON_MATCHING   /* register allocation and scheduling differ; logic follows the asm */
void func_005A5380(IkChain *ik, u8 *bone, f32 *parent) {
    f32 m70[16];
    f32 mB0[16];
    f32 mF0[16];
    f32 m130[16];
    f32 m170[16];
    f32 m1B0[16];
    f32 m1F0[16];
    f32 m230[16];
    Vec3 sc;
    f32 d[3];
    f32 u[3];
    f32 *dy;
    f32 *dz;
    f32 *r1;
    f32 *r4;
    f32 *r5;
    f32 len0;
    f32 len1;
    f32 l2;
    f32 a2;
    f32 b2;
    f32 c;
    f32 sn;

    func_0018F7C0(m70);
    func_0018FAD0(ik->unkBC, m70);
    sc = D_006D7550;
    func_005A5140(bone, parent, &sc.x, *(f32 *)(*(u8 **)(bone + 0xC8) + 0x104),
                  *(f32 *)(*(u8 **)(bone + 0xC8) + 0x108), *(f32 *)(*(u8 **)(bone + 0xC8) + 0x10C));
    func_00190ED0(m1F0, (f32 *)bone);
    dy = &d[1];
    dz = &d[2];
    d[0] = ik->tgt[0] - m1F0[12];
    *dy = ik->tgt[1] - m1F0[13];
    *dz = ik->tgt[2] - m1F0[14];
    switch (ik->mode) {
    case 0:
        break;
    case 1:
        func_001913A0(u, ik->up, parent);
        break;
    case 2:
        u[0] = ik->up[0] - m1F0[12];
        u[1] = ik->up[1] - m1F0[13];
        u[2] = ik->up[2] - m1F0[14];
        break;
    }
    func_00190F50(m1F0, d, u);
    func_0018F7C0(m1B0);
    func_0018FAD0(ik->angle, m1B0);
    func_00190CD0(m1B0, m1F0);
    func_00190ED0(m1F0, m1B0);
    len0 = ik->len0;
    len1 = ik->len1;
    l2 = d[0] * d[0] + *dy * *dy + *dz * *dz;
    a2 = len0 * len0;
    b2 = len1 * len1;
    c = func_00341860(l2);
    if (!(c < len0 + len1)) {
        func_00190C50(m1B0, m70, m1F0);
        func_00190ED0(m1F0, m1B0);
        func_00190C50((f32 *)ik->b0, ik->m0, m1F0);
        func_00190C50((f32 *)ik->b1, ik->m1, m1F0);
        *(f32 *)(ik->b1 + 0x30) += m1F0[0] * len0;
        *(f32 *)(ik->b1 + 0x34) += m1F0[1] * len0;
        *(f32 *)(ik->b1 + 0x38) += m1F0[2] * len0;
        return;
    }
    func_0018F7C0(m230);
    c = (a2 + l2 - b2) / (2.0f * c * len0);
    if (c == 1.0f) {
        sn = 0.0f;
    } else {
        sn = func_00341860(1.0f - c * c);
    }
    m230[0] = c;
    r1 = &m230[1];
    *r1 = sn;
    r4 = &m230[4];
    *r4 = -sn;
    r5 = &m230[5];
    *r5 = c;
    func_00190C50(m170, m230, m1F0);
    func_00190C50(mF0, m70, m170);
    func_00190C50((f32 *)ik->b0, ik->m0, mF0);
    c = (b2 + a2 - l2) / (2.0f * len0 * len1);
    if (c == 1.0f) {
        sn = 0.0f;
    } else {
        sn = func_00341860(1.0f - c * c);
    }
    m230[0] = -c;
    *r1 = -sn;
    *r4 = sn;
    *r5 = -c;
    m230[12] = ik->len0;
    m230[13] = 0.0f;
    m230[14] = 0.0f;
    func_00190C50(m130, m230, m170);
    func_00190C50(mB0, m70, m130);
    func_00190C50((f32 *)ik->b1, ik->m1, mB0);
}
#endif
