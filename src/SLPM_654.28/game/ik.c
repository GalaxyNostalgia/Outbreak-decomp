#include "common.h"
#include "game.h"
#include "object.h"

/* Two-bone chains (arms) the character's model bends towards a target: a chain remembers the
   bones' rest orientations relative to the plane they lie in, their lengths and the up vector,
   and hooks its three bones' update callbacks (0x178/0x17C). */

typedef struct {
    f32 m0[16];         /* 0x00: bone 0 relative to its aim */
    f32 m1[16];         /* 0x40: bone 1 relative to its aim */
    s16 mode;           /* 0x80 */
    s16 on;             /* 0x82 */
    f32 angle;          /* 0x84 */
    u8 *b0;             /* 0x88 */
    u8 *b1;             /* 0x8C */
    f32 len0;           /* 0x90 */
    f32 len1;           /* 0x94 */
    u8 unk98[8];
    f32 up[3];          /* 0xA0 */
    u8 unkAC[0xBC - 0xAC];
    s32 unkBC;          /* 0xBC */
} IkChain;              /* 0xC0 */

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
void func_005A5140(void);
void func_005A5370(void);
void func_005A5380(void);

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
    ik->unkBC = 0;
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
