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
