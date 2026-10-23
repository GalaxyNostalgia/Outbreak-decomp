#include "common.h"
#include "ps2mem.h"

/* The projection: a perspective matrix into one of the scratchpad's matrix slots, then the
   matrices the renderer derives from it. */

void func_0018EFC0(f32 *out, f32 x);   /* VU0: sin and cos of x */

void func_00191B80(f32 *s, f32 *c, f32 x);   /* mathf.c */

void func_0018F7C0(Mat44 *m);           /* VU0: identity */
void func_00190ED0(Mat44 *dst, Mat44 *src);

/* A perspective matrix (fov in radians, aspect = width / height). */
void func_0018F150(Mat44 *m, f32 a, f32 b, f32 fov, f32 aspect) {
    f32 c;
    f32 s;

    fov *= 0.5f;
    func_00191B80(&s, &c, fov);
    func_0018F7C0(m);
    m->m[0][0] = c / s / aspect;
    m->m[1][1] = c / s;
    m->m[2][2] = -(a + b) / (a - b);
    m->m[2][3] = -1.0f;
    m->m[3][2] = -(2.0f * a * b) / (a - b);
    m->m[3][3] = 0;
}

extern Mat44 D_00277160;
extern f32 D_0023BA30;
extern f32 D_0023BA34;
extern f32 D_0023BA40;
extern f32 D_0023BA44;

/* The projection scaled to the screen. */
void func_0018F220(Mat44 *m) {
    func_00190ED0(&D_00277160, m);
    D_00277160.m[0][0] = 2.0f * D_00277160.m[0][0] / (D_0023BA34 / (0.5f * D_0023BA44));
    D_00277160.m[1][1] = 2.0f * D_00277160.m[1][1] / (D_0023BA30 / (0.5f * D_0023BA40));
}

void func_00190C50(void *out, void *a, void *b);
extern Mat44 D_002771A0;
extern Mat44 D_002771E0;
extern Mat44 D_00277220;

void func_0018F0E0(f32 a, f32 b, f32 fov, f32 aspect, s32 slot) {
    Mat44 *m = &((Mat44 *)PS2_SPR(0x70003700))[slot];

    func_0018F150(m, a, b, fov, aspect);
    func_0018F220(m);
    func_00190C50(&D_002771A0, PS2_SPR(0x70003F00), PS2_SPR(0x70003F80));
    func_00190C50(&D_002771E0, PS2_SPR(0x70003F00), &D_00277220);
}

#include "system.h"

void func_0018F6C0(s32 x, s32 y, s32 w, s32 h, f32 zn, f32 zf);
void func_0018F3A0(Mat44 *m, s32 x, s32 y, s32 w, s32 h, f32 zn, f32 zf);

extern Mat44 D_00277120;
extern f32 D_0023BA38;
extern f32 D_0023BA3C;
extern f32 D_0023BA48;
extern f32 D_0023BA4C;
extern s32 D_0023BA9C;          /* frame buffer height */
extern s32 D_0023BAA0;          /* frame buffer width */

#ifdef NON_MATCHING   /* stores scheduled in a different order */
/* The screen matrix for a w x h viewport centred at the GS offset. */
void func_0018F6C0(s32 x, s32 y, s32 w, s32 h, f32 zn, f32 zf) {
    func_0018F7C0(&D_00277120);
    D_00277120.m[0][0] = w / 2;
    D_00277120.m[3][3] = 1.0f;
    D_00277120.m[2][2] = zf - zn;
    D_00277120.m[3][0] = D_0023BA3C;
    D_00277120.m[3][1] = D_0023BA38;
    D_00277120.m[1][1] = -(f32)(h / 2);
    D_00277120.m[2][2] = D_00277120.m[2][2] * (0.5f * -D_00281DB0.zMax);
    D_00277120.m[3][2] = 0.5f * D_00281DB0.zMax;
}
#endif

#ifdef NON_MATCHING   /* stores scheduled in a different order */
/* The viewport: screen matrices for (x, y, w, h) and depths zn..zf. */
void func_0018F3A0(Mat44 *m, s32 x, s32 y, s32 w, s32 h, f32 zn, f32 zf) {
    s32 hw = w / 2;
    f32 fw;
    f32 fh;
    f32 dz;
    f32 fx;
    f32 fy;

    fw = hw;
    fh = h / 2;
    func_0018F7C0(&D_00277220);
    D_00277220.m[0][0] = fw;
    D_00277220.m[1][1] = -fh;
    D_00277220.m[3][3] = 1.0f;
    D_00277220.m[3][2] = zn;
    dz = zf - zn;
    D_00277220.m[2][2] = dz;
    D_00277220.m[3][0] = (f32)x + fw;
    D_00277220.m[3][1] = (f32)y + fh;
    if (D_00281DB0.field == 0) {
        y /= 2;
        h /= 2;
    }
    fw = hw;
    fh = h / 2;
    func_0018F7C0(m);
    fx = x;
    fy = y;
    m->m[0][0] = fw;
    m->m[1][1] = -fh;
    m->m[2][2] = dz;
    m->m[3][0] = fx + fw;
    m->m[3][1] = fy + fh;
    m->m[3][3] = 1.0f;
    m->m[2][2] *= 0.5f * -D_00281DB0.zMax;
    m->m[3][0] += 2048 - (D_0023BAA0 >> 1);
    m->m[3][1] += 2048 - (D_0023BA9C >> 1);
    m->m[3][2] = 0.5f * D_00281DB0.zMax;
    D_0023BA3C = m->m[3][0];
    D_0023BA38 = m->m[3][1];
    D_0023BA4C = fx;
    D_0023BA48 = fy;
    D_0023BA44 = w;
    D_0023BA40 = h;
    D_0023BA34 = 2048.0f - D_0023BA3C;
    if (D_0023BA34 < 0.0f) {
        D_0023BA34 *= -1.0f;
    }
    D_0023BA34 = 2048.0f - D_0023BA34;
    D_0023BA30 = 2048.0f - D_0023BA38;
    if (D_0023BA30 < 0.0f) {
        D_0023BA30 *= -1.0f;
    }
    D_0023BA30 = 2048.0f - D_0023BA30;
    func_0018F6C0(0, 0, D_0023BA34, D_0023BA30, zn, zf);
}
#endif

s32 func_00196C20(s32 x, s32 y, s32 w, s32 h, s32 ctx);

/* A viewport in matrix slot `slot` (and the GS scissor for it). */
void func_0018F2B0(s32 slot, s32 x, s32 y, s32 w, s32 h, f32 zn, f32 zf) {
    func_0018F3A0(&((Mat44 *)PS2_SPR(0x70003700))[slot], x, y, w, h, zn, zf);
    func_0018F220(PS2_SPR(0x70003F00));
    func_00190C50(&D_002771A0, PS2_SPR(0x70003F00), PS2_SPR(0x70003F80));
    func_00190C50(&D_002771E0, PS2_SPR(0x70003F00), &D_00277220);
    if (D_00281DB0.field == 0) {
        y /= 2;
        h /= 2;
    }
    func_00196C20(x, y, w, h, 2);
}

void func_00190BA0(Mat44 *m);                       /* VU0: transpose the 3x3 part */
void func_001913A0(f32 *out, Vec3 *v, Mat44 *m);    /* VU0: v times the 3x3 part */

/* Inverts a rotation + translation matrix in place. */
void func_00190BF0(Mat44 *m) {
    Vec3 t;

    t.x = -m->m[3][0];
    t.y = -m->m[3][1];
    t.z = -m->m[3][2];
    func_00190BA0(m);
    func_001913A0(m->m[3], &t, m);
}

/* Moves a matrix by (x, y, z). */
void func_0018FA40(Mat44 *m, f32 x, f32 y, f32 z) {
    m->m[3][0] += x;
    m->m[3][1] += y;
    m->m[3][2] += z;
}

/* Moves a matrix by v. */
void func_0018FA70(Mat44 *m, f32 *v) {
    m->m[3][0] += v[0];
    m->m[3][1] += v[1];
    m->m[3][2] += v[2];
}

/* Sets the translation row. */
void func_0018FA30(f32 x, f32 y, f32 z, f32 *m) {
    m[12] = x;
    m[13] = y;
    m[14] = z;
}

/* A matrix's translation row. */
void func_0018FAB0(f32 *out, const Mat44 *m) {
    out[0] = m->m[3][0];
    out[1] = m->m[3][1];
    out[2] = m->m[3][2];
}

/* Copies a matrix's 3x3 part. */
void func_00190F00(Mat44 *dst, Mat44 *src) {
    dst->m[0][0] = src->m[0][0];
    dst->m[0][1] = src->m[0][1];
    dst->m[0][2] = src->m[0][2];
    dst->m[1][0] = src->m[1][0];
    dst->m[1][1] = src->m[1][1];
    dst->m[1][2] = src->m[1][2];
    dst->m[2][0] = src->m[2][0];
    dst->m[2][1] = src->m[2][1];
    dst->m[2][2] = src->m[2][2];
}
