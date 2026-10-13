#include "common.h"
#include "scene.h"

/* GAME.BIN: the scene's lights, fog and camera defaults. */

extern s8 D_0023BF64;
extern s8 D_0023BF60;
extern s32 D_0023BF5C;
extern u8 D_007131A0[0xE00];
extern u8 D_007123A0[0xE00];
extern u8 D_00712320[0x80];
void func_001AE9C0(s32 cmd, u32 arg);

#ifdef NON_MATCHING   /* same stores, scheduled in a different order */
void func_0067ED70(void) {
    D_00305FA0.a[0].w[7] = 0;
    D_00305FA0.a[0].w[6] = 0;
    D_00305FA0.a[0].w[3] = 1.0f;
    D_00305FA0.a[0].w[20] = 100.0f;
    D_00305FA0.a[0].w[2] = 1.0f;
    D_00305FA0.a[0].w[1] = 1.0f;
    D_00305FA0.a[0].w[5] = 0;
    D_00305FA0.a[0].w[0] = 0;
    D_00305FA0.a[0].w[18] = 0;
    D_00305FA0.b[1].w[17] = 132.0f;
    D_00305FA0.a[0].w[17] = 0;
    D_00305FA0.b[1].w[18] = 340.0f;
    D_00305FA0.a[0].w[16] = 0;
    D_00305FA0.a[0].w[19] = 0;
    D_00305FA0.a[0].w[21] = 0;
    D_00305FA0.b[1].w[16] = 1785;
    D_00305FA0.a[1].w[7] = 0;
    D_00305FA0.a[1].w[6] = 0;
    D_00305FA0.a[1].w[5] = 0;
    D_00305FA0.a[1].w[13] = 0;
    D_00305FA0.a[1].w[14] = 0;
    D_00305FA0.a[1].w[15] = -1.0f;
    D_00305FA0.a[1].w[0] = 0;
    D_00305FA0.a[2].w[7] = 0;
    D_00305FA0.a[2].w[6] = 0;
    D_00305FA0.a[2].w[5] = 0;
    D_00305FA0.a[2].w[13] = 0;
    D_00305FA0.a[2].w[14] = 0;
    D_00305FA0.a[2].w[15] = -1.0f;
    D_00305FA0.a[2].w[0] = 0;
    D_00305FA0.enable[0] = 2;
    D_00305FA0.enable[2] = 1;
    D_00305FA0.enable[1] = 1;
    D_00305FA0.enable[3] = 1;
    D_00305FA0.b[0].w[7] = 0;
    D_00305FA0.b[0].w[6] = 0;
    D_00305FA0.b[0].w[5] = 0;
    D_00305FA0.b[0].w[13] = 0;
    D_00305FA0.b[0].w[14] = 0;
    D_00305FA0.b[0].w[15] = -1.0f;
    D_00305FA0.b[0].w[0] = 0;
    D_00305FA0.b[1].w[3] = 1.0f;
    D_00305FA0.b[1].w[2] = 1.0f;
    D_00305FA0.b[1].w[1] = 1.0f;
    D_00305FA0.b[1].w[7] = 0;
    D_00305FA0.b[1].w[6] = 0;
    D_00305FA0.b[1].w[5] = 0;
    D_00305FA0.b[1].w[13] = 0;
    D_00305FA0.b[1].w[14] = 0;
    D_00305FA0.b[1].w[15] = -1.0f;
    D_00305FA0.b[2].w[15] = -1.0f;
    D_00305FA0.b[1].w[0] = 0;
    D_00305FA0.a[1].w[19] = 0;
    D_00305FA0.a[1].w[21] = 0;
    D_00305FA0.b[2].w[11] = 0;
    D_00305FA0.b[2].w[10] = 0;
    D_00305FA0.b[2].w[9] = 0;
    D_00305FA0.b[2].w[3] = 1.0f;
    D_00305FA0.b[2].w[2] = 1.0f;
    D_00305FA0.b[2].w[1] = 1.0f;
    D_00305FA0.b[2].w[7] = 0;
    D_00305FA0.b[2].w[6] = 0;
    D_00305FA0.b[2].w[5] = 0;
    D_00305FA0.b[2].w[13] = 0;
    D_00305FA0.b[2].w[14] = 0;
    D_00305FA0.b[2].w[0] = 0;
    D_00305FA0.b[2].w[18] = 0;
    D_00305FA0.b[2].w[17] = 0;
    D_00305FA0.b[2].w[16] = 0;
    D_00305FA0.b[2].w[19] = 0;
    D_00305FA0.b[2].w[21] = 0;
    D_00305FA0.enable[5] = 2;
    D_00305FA0.enable[4] = 2;
    D_00305FA0.a[0].w[11] = 0.2f;
    D_00305FA0.a[0].w[10] = 0.2f;
    D_00305FA0.a[0].w[9] = 0.2f;
    D_00305FA0.a[1].w[11] = 0.01f;
    D_00305FA0.a[1].w[10] = 0.01f;
    D_00305FA0.a[1].w[9] = 0.01f;
    D_00305FA0.a[1].w[3] = 0.2f;
    D_00305FA0.a[1].w[2] = 0.2f;
    D_00305FA0.a[1].w[1] = 0.2f;
    D_00305FA0.a[2].w[11] = 0.01f;
    D_00305FA0.a[2].w[10] = 0.01f;
    D_00305FA0.a[2].w[9] = 0.01f;
    D_00305FA0.a[2].w[3] = 0.2f;
    D_00305FA0.a[2].w[2] = 0.2f;
    D_00305FA0.a[2].w[1] = 0.2f;
    D_00305FA0.b[0].w[11] = 0.01f;
    D_00305FA0.b[0].w[10] = 0.01f;
    D_00305FA0.b[0].w[9] = 0.01f;
    D_00305FA0.b[0].w[3] = 0.2f;
    D_00305FA0.b[0].w[2] = 0.2f;
    D_00305FA0.b[0].w[1] = 0.2f;
    D_00305FA0.b[1].w[11] = 0.1f;
    D_00305FA0.b[1].w[10] = 0.1f;
    D_00305FA0.b[1].w[9] = 0.1f;
    D_00305FA0.b[1].w[20] = 0.01f;
    D_00305FA0.b[2].w[20] = 0.01f;
    func_001AE9C0(0xE, 0xFF808080);
    D_00305FA0.fogColor[0] = 0;
    D_00305FA0.unk31D = -1;
    D_00305FA0.fogNear = 100.0f;
    D_00305FA0.fogColor[1] = 0;
    D_00305FA0.fogColor[2] = 0;
    D_00305FA0.ambient[0] = 7;
    D_00305FA0.ambient[1] = 0xA;
    D_00305FA0.ambient[2] = 0x14;
    D_00305FA0.ambient[3] = 0x80;
    D_00305FA0.color1[0] = 0xFF;
    D_00305FA0.fogFar = 4300;
    D_00305FA0.color1[1] = 0xFF;
    D_00305FA0.color1[2] = 0xFF;
    D_00305FA0.color1[3] = 0xFF;
    D_00305FA0.color2[3] = 0xFF;
    D_00305FA0.color2[0] = 0x80;
    D_00305FA0.color2[1] = 0x80;
    D_00305FA0.color2[2] = 0x80;
    memset(&D_00305FA0.offset1, 0, sizeof(D_00305FA0.offset1));
    memset(&D_00305FA0.offset2, 0, sizeof(D_00305FA0.offset2));
    memset(&D_00305FA0.unk310, 0, sizeof(D_00305FA0.unk310));
    memset(&D_00305FA0.unk304, 0, sizeof(D_00305FA0.unk304));
    memset(&D_00305FA0.offset0, 0, sizeof(D_00305FA0.offset0));
    memset(D_007131A0, 0, sizeof(D_007131A0));
    D_0023BF64 = 0;
    memset(D_007123A0, 0, sizeof(D_007123A0));
    D_0023BF60 = 0;
    D_0023BF5C = 0;
    memset(D_00712320, 0, sizeof(D_00712320));
    D_00305FA0.unk328 = 0;
    D_00305FA0.unk31E = -1;
    D_00305FA0.mode = -1;
    D_00305FA0.unk32C = 1;
    D_00305FA0.unk324 = 0;
    D_00305FA0.unk320 = 0;
    D_00305FA0.flags = 0;
}
#endif

#include "camera.h"
#include "game.h"

static inline s16 clamp_color(s16 c, s16 add) {
    s16 v;

    v = c + add;
    return v < 0 ? 0 : v > 0xFF ? 0xFF : v;
}

void func_00340310(s32 x, s32 y, s32 z, Mat44 *m);     /* SUBMAIN.BIN: rotation from angles */
void func_00340560(Vec3 *out, Vec3 *to, Vec3 *from);    /* SUBMAIN.BIN */
void func_001913A0(Vec3 *out, Vec3 *v, Mat44 *m);
void func_00191750(Vec3 *v);
void func_00107B50(void *dst, void *src, s32 n);
void func_00681340(Vec3 *out, Vec3 *v, Vec3 *a);
extern Vec3 D_006EC0C8;         /* the direction lights point at angle 0 */

#ifdef NON_MATCHING   /* the light-direction copies go through pointers in the original */
/* Mode 0: three lights from angles (the third may follow the camera), ambient and fog. */
void func_0067F320(void) {
    Mat44 m;
    f32 v[3];
    s32 i;
    s16 r;
    s16 g;
    s16 b;
    s16 a;
    f32 far;

    for (i = 0; i < 3; i++) {
        if (D_00305FA0.enable[i] == 1) {
            func_00340310(D_00305FA0.angA[i].x, D_00305FA0.angA[i].y, D_00305FA0.angA[i].z, &m);
            func_001913A0((Vec3 *)v, &D_006EC0C8, &m);
            D_00305FA0.a[i].w[13] = v[0];
            D_00305FA0.a[i].w[14] = v[1];
            D_00305FA0.a[i].w[15] = v[2];
        }
        if (D_00305FA0.enable[i + 3] == 1) {
            if ((D_00305FA0.flags & 1) && D_0048BF60.unk28 == 0) {
                func_00340560((Vec3 *)v, &D_00306320.target, &D_00306320.pos);
                func_00191750((Vec3 *)v);
            } else {
                func_00340310(D_00305FA0.angB[i].x, D_00305FA0.angB[i].y, D_00305FA0.angB[i].z, &m);
                func_001913A0((Vec3 *)v, &D_006EC0C8, &m);
            }
            D_00305FA0.b[i].w[13] = v[0];
            D_00305FA0.b[i].w[14] = v[1];
            D_00305FA0.b[i].w[15] = v[2];
        }
    }
    r = clamp_color(D_00305FA0.ambient[0], D_00305FA0.offset0[0]);
    g = clamp_color(D_00305FA0.ambient[1], D_00305FA0.offset0[1]);
    b = clamp_color(D_00305FA0.ambient[2], D_00305FA0.offset0[2]);
    a = clamp_color(D_00305FA0.ambient[3], D_00305FA0.offset0[3]);
    func_001AE9C0(0xF, b | (g << 8 | (a << 24 | r << 16)));
    func_001AE9C0(0x10, *(u32 *)&D_00305FA0.fogNear);
    far = D_00305FA0.fogFar;
    if (D_00305FA0.fogNear == far) {
        far += 1.0f;
    }
    func_001AE9C0(0x11, *(u32 *)&far);
    func_001AE9C0(0x67, -1);
    func_001AE9C0(0x14, D_00305FA0.fogColor[2] | ((D_00305FA0.fogColor[0] << 16) | 0xFF000000 |
                                                 (D_00305FA0.fogColor[1] << 8)));
}
#endif

/* Mode 1: ambient and the first light as it is. */

void func_0067F6C0(void) {
    s16 r;
    s16 g;
    s16 b;
    s16 a;

    r = clamp_color(D_00305FA0.color1[0], D_00305FA0.offset1[0]);
    g = clamp_color(D_00305FA0.color1[1], D_00305FA0.offset1[1]);
    b = clamp_color(D_00305FA0.color1[2], D_00305FA0.offset1[2]);
    a = clamp_color(D_00305FA0.color1[3], D_00305FA0.offset1[3]);
    func_001AE9C0(0xE, b | (g << 8 | (a << 24 | r << 16)));
    if (D_00305FA0.enable[0] != 0) {
        func_001AE9C0(0x5A, (u32)&D_00305FA0.a[0]);
    }
}

/* Mode 2: ambient and the second set of lights, the first moved by unk304. */
void func_0067F860(void) {
    Light l;
    s16 r;
    s16 g;
    s16 b;
    s16 a;

    r = clamp_color(D_00305FA0.color2[0], D_00305FA0.offset2[0]);
    g = clamp_color(D_00305FA0.color2[1], D_00305FA0.offset2[1]);
    b = clamp_color(D_00305FA0.color2[2], D_00305FA0.offset2[2]);
    a = clamp_color(D_00305FA0.color2[3], D_00305FA0.offset2[3]);
    func_001AE9C0(0xE, b | (g << 8 | (a << 24 | r << 16)));
    if (D_00305FA0.enable[3] != 0) {
        func_00107B50(&l, &D_00305FA0.b[0], sizeof(Light));
        func_00681340((Vec3 *)&l.w[1], (Vec3 *)&D_00305FA0.b[0].w[1], &D_00305FA0.unk304);
        func_00681340((Vec3 *)&l.w[9], (Vec3 *)&D_00305FA0.b[0].w[9], &D_00305FA0.unk304);
        func_001AE9C0(0x5A, (u32)&l);
    }
    if (D_00305FA0.enable[4] != 0) {
        func_001AE9C0(0x5B, (u32)&D_00305FA0.b[1]);
    }
    if (D_00305FA0.enable[5] != 0) {
        func_001AE9C0(0x5C, (u32)&D_00305FA0.b[2]);
    }
}

/* Sets the lighting mode (0-2) unless it is already set. */
void func_0067F290(s32 mode) {
    if (D_00305FA0.mode != mode) {
        switch (mode) {
        case 0:
            func_0067F320();
            break;
        case 1:
            func_0067F6C0();
            break;
        case 2:
            func_0067F860();
            break;
        }
        D_00305FA0.mode = mode;
    }
}
