#include "common.h"

/* The game's float maths: wrappers around the C library's and around VU0 routines (vu0math). */

f32 acosf(f32 x);
f32 atanf(f32 x);
f32 atan2f(f32 y, f32 x);
f32 floorf(f32 x);
f32 modff(f32 x, f32 *ip);
f32 func_0018EDB0(f32 x);                   /* sin on VU0 */
f32 func_0018EED0(f32 x);                   /* cos on VU0 */
void func_0018EFC0(f32 *out, f32 x);        /* sin and cos on VU0 */
f32 func_00191BF0(f32 x);                   /* sqrt on VU0 */

/* A vector's copy. */
typedef struct {
    f32 x, y, z;
} Vec3f;

void func_001918A0(f32 *dst, const f32 *src) {
    *(Vec3f *)dst = *(const Vec3f *)src;
}

void func_00191B80(f32 *s, f32 *c, f32 x);

/* Turns v by angle about y. */
void func_00191590(f32 *v, f32 angle);
#ifdef NON_MATCHING   /* the copy's z is read through an address register */
void func_00191590(f32 *v, f32 angle) {
    Vec3f t;
    f32 sn;
    f32 c;

    t = *(Vec3f *)v;
    func_00191B80(&sn, &c, angle);
    v[0] = c * t.x + sn * t.z;
    v[2] = -sn * t.x + c * t.z;
}
#endif

f32 func_00191AD0(f32 x) {
    return acosf(x);
}

f32 func_00191B40(f32 x);

/* asin */
f32 func_00191AE0(f32 x) {
    return func_00191B40(x / func_00191BF0(1.0f + -x * x));
}

f32 func_00191B40(f32 x) {
    return atanf(x);
}

f32 func_00191B50(f32 y, f32 x) {
    return atan2f(y, x);
}

f32 func_00191B60(f32 x) {
    return func_0018EDB0(x);
}

f32 func_00191B70(f32 x) {
    return func_0018EED0(x);
}

void func_00191B80(f32 *s, f32 *c, f32 x);
#ifdef NON_MATCHING   /* the original keeps s and c in a2/t1 across the VU0 call */
void func_00191B80(f32 *s, f32 *c, f32 x) {
    f32 v[4];

    func_0018EFC0(v, x);
    *s = v[0];
    *c = v[1];
}
#endif

f32 func_00191BC0(f32 x) {
    return floorf(x);
}

/* the fractional part */
f32 func_00191BD0(f32 x) {
    f32 ip;

    return modff(x, &ip);
}

extern f32 D_00239C94;          /* pi */
extern f32 D_00239C98;          /* -2 pi */

f32 func_00191950(u32 a);
#ifdef NON_MATCHING   /* float registers differ */
/* A 16-bit angle (0x10000 = a full turn) in radians, -pi..pi */
f32 func_00191950(u32 a) {
    f32 r = D_00239C94 * (0.0054931640625f * a) / 180.0f;

    if (!(r <= D_00239C94)) {
        r += D_00239C98;
    }
    return r;
}
#endif
