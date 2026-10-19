#include "common.h"

/* Headings between points. */

f32 func_00191B50(f32 y, f32 x);
u32 func_001918C0(f32 rad);
extern f32 D_00239CE4;      /* (a tiny x, so the heading of straight up/down is defined) */

/* The heading (0x10000 a turn) from a to b, in the x-z plane. */
s16 func_00346C50(const f32 *a, const f32 *b);
#ifdef NON_MATCHING   /* the original keeps d on the stack */
s16 func_00346C50(const f32 *a, const f32 *b) {
    f32 d[3];

    d[0] = b[0] - a[0];
    d[1] = b[1] - a[1];
    d[2] = b[2] - a[2];
    if (d[0] == 0.0f) {
        d[0] = D_00239CE4;
    }
    return func_001918C0(func_00191B50(d[0], d[2]));
}
#endif

/* The effect models loaded. */
typedef struct {
    u8 unk0[0x820];
    s16 count;          /* 0x820 */
    s16 unk822;
    s16 ids[1];         /* 0x824 */
} EffectModels;

extern EffectModels D_00389750;

/* Which loaded effect model id is (-1: none). */
s16 func_003426E0(s16 id) {
    s32 i;

    for (i = 0; i < D_00389750.count; i++) {
        if (id == D_00389750.ids[i]) {
            return i;
        }
    }
    return -1;
}

#include "model.h"

extern RoomObjModel D_0039A3D0;

/* Which of the room's object models is id (-1: none). */
s16 func_00342450(s16 id) {
    s32 i;

    for (i = 0; i < D_0039A3D0.nroots; i++) {
        if (id == (s16)D_0039A3D0.rootId[i]) {
            return i;
        }
    }
    return -1;
}

/* A sound requested for the next frame (D_00320A42: 1 asked, 2 played). */
extern u8 D_00320A42[];
extern u16 D_00320A44[];
void func_001CF620(u16 id);

void func_00341A70(void) {
    if (D_00320A42[0] == 1) {
        func_001CF620(D_00320A44[0]);
        D_00320A42[0] = 2;
    }
}

void func_001AEC00(void);
void func_001AEC40(void (*fn)(void));
void func_001B3920(void);
void func_00341AF0(void);

void func_001AE290(void);
void func_001B17C0(void);
void func_001B9CC0(void);

/* The per-frame work while a message is up (installed by func_00341B20/func_00341B50). */
void func_00341AF0(void) {
    func_001AE290();
    func_001B17C0();
    func_001B9CC0();
}

void func_00341B20(void) {
    func_001AEC00();
    func_001AEC40(func_00341AF0);
    func_001B3920();
}

/* The same, when a message comes up. */
void func_00341B50(void) {
    func_001AEC00();
    func_001AEC40(func_00341AF0);
    func_001B3920();
}

void func_001918A0(f32 *dst, const f32 *src);
void func_00191750(f32 *v);

/* out = p + s * the direction of dir. */
void func_00340FC0(f32 s, const f32 *dir, const f32 *p, f32 *out) {
    f32 d[4];

    func_001918A0(d, dir);
    func_00191750(d);
    out[0] = p[0] + d[0] * s;
    out[1] = p[1] + d[1] * s;
    out[2] = p[2] + d[2] * s;
}

f32 func_00191B70(f32 x);
extern f32 D_00239CD4;          /* 2 pi / 65536 */

f32 func_00191B60(f32 x);

/* The sine of a game angle (0x10000 a turn). */
f32 func_003400C0(u16 a) {
    return func_00191B60(D_00239CD4 * (u32)a);
}

/* The cosine of a 16-bit angle. */
f32 func_00340100(u16 a) {
    return func_00191B70(D_00239CD4 * (u32)a);
}

/* The point on the line through a and b nearest p. */
void func_00341140(const f32 *a, const f32 *b, const f32 *p, f32 *out) {
    f32 dx = b[0] - a[0];
    f32 dy = b[1] - a[1];
    f32 dz = b[2] - a[2];
    f32 t;

    t = (a[2] * dz + (a[1] * dy + (a[0] * dx - dx * p[0]) - dy * p[1]) - dz * p[2]) /
        (-(dx * dx) - dy * dy - dz * dz);
    out[0] = a[0] + dx * t;
    out[1] = a[1] + dy * t;
    out[2] = a[2] + dz * t;
}

/* Point-in-polygon tests in the x-z plane (points are (x, z) pairs). */

/* Twice the signed area of the triangle a, b, c. */
f32 func_00340AA0(const f32 *a, const f32 *b, const f32 *c) {
    return (b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1]);
}

/* Whether p is in the triangle a, b, c: 1 when it is and the triangle turns clockwise, 2 when
   it is and it turns the other way, 0 when not (or it is flat). */
u8 func_00340AE0(const f32 *a, const f32 *b, const f32 *c, const f32 *p) {
    f32 t = func_00340AA0(a, b, c);

    if (t == 0.0f) {
        return 0;
    }
    if (t < 0.0f) {
        if (func_00340AA0(a, b, p) > 0.0f) {
            return 0;
        }
        if (func_00340AA0(b, c, p) > 0.0f) {
            return 0;
        }
        return func_00340AA0(c, a, p) > 0.0f ? 0 : 1;
    }
    if (func_00340AA0(a, b, p) < 0.0f) {
        return 0;
    }
    if (func_00340AA0(b, c, p) < 0.0f) {
        return 0;
    }
    if (func_00340AA0(c, a, p) < 0.0f) {
        return 0;
    }
    return 2;
}

/* Whether p is inside the n-sided polygon poly (counting the fan triangles it falls in). */
s32 func_00340C50(const f32 (*poly)[2], const f32 *p, s32 n) {
    s32 area = 0;
    s32 i;
    s8 in;
    s32 cw;
    s32 j;
    u8 r;

    for (i = 0; i < n - 2; i++) {
        area += (s32)func_00340AA0(poly[i], poly[i + 1], poly[i + 2]);
    }
    cw = area <= 0;
    in = 0;
    for (i = 0, j = 1; i < n - 2; i++, j++) {
        r = func_00340AE0(poly[0], poly[j], poly[j + 1], p);
        if (r != 0) {
            if (cw) {
                if (r == 1) {
                    in++;
                } else {
                    in--;
                }
            } else {
                if (r == 2) {
                    in++;
                } else {
                    in--;
                }
            }
        }
    }
    return in > 0;
}

/* Point-in-polygon tests on x-z integer points (a zone's corners). */

/* The cross product of b - a and c - a: > 0 when c is to the left of a to b. */
s32 func_00340750(const s32 *a, const s32 *b, const s32 *c) {
    s32 az = a[1];
    s32 ax = a[0];
    s32 bz = b[1];
    s32 bx = b[0];
    s32 cx = c[0];
    s32 cz = c[1];

    return (bx - ax) * (cz - az) - (cx - ax) * (bz - az);
}

/* Whether p is in the triangle a b c: 1 inside a clockwise one, 2 an anticlockwise one, 0 not
   (or a degenerate one). */
s32 func_00340790(const s32 *a, const s32 *b, const s32 *c, const s32 *p) {
    s32 s = func_00340750(a, b, c);

    if (s == 0) {
        return 0;
    }
    if (s < 0) {
        if (func_00340750(a, b, p) > 0) {
            return 0;
        }
        if (func_00340750(b, c, p) > 0) {
            return 0;
        }
        if (func_00340750(c, a, p) > 0) {
            return 0;
        }
        return 1;
    }
    if (func_00340750(a, b, p) < 0) {
        return 0;
    }
    if (func_00340750(b, c, p) < 0) {
        return 0;
    }
    if (func_00340750(c, a, p) < 0) {
        return 0;
    }
    return 2;
}

/* Whether p is in the polygon of n points pts (x, z pairs): fans it into triangles from the
   first point and counts the ones p is in, by their winding against the polygon's. */
s32 func_00340940(s32 (*pts)[2], const s32 *p, s32 n) {
    s32 j = 0;
    s32 i;
    s8 c;
    s32 cw;
    s32 r;

    for (i = 0; i < n - 2; i++) {
        j += func_00340750(pts[i], pts[i + 1], pts[i + 2]);
    }
    cw = j <= 0;
    c = 0;
    for (i = 0, j = 1; i < n - 2; i++, j++) {
        r = (u8)func_00340790(pts[0], pts[j], pts[j + 1], p);
        if (r) {
            if (cw) {
                if (r == 1) {
                    c++;
                } else {
                    c--;
                }
            } else if (r == 2) {
                c++;
            } else {
                c--;
            }
        }
    }
    return c > 0;
}

/* The per-frame turn from heading from to heading to over n frames (the short way round). */
s16 func_00346F20(u16 from, u16 to, s16 n) {
    return (s16)(to - from) / n;
}
