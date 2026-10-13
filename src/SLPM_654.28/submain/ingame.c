#include "common.h"
#include "options.h"
#include "camera.h"

/* SUBMAIN.BIN: setting up the in-game screen. */

extern s32 D_0023BCD4;

void func_003478E0(void) {
    D_0023BCD4 = 0;
}

extern f32 D_00239D9C;          /* 0.001 */
extern f32 D_00239D40;          /* 0.001 */
extern f32 D_00239D44;          /* pi / 180 */
extern f32 D_00239DA0;          /* 4 / 3 */
extern f32 D_00239D48;          /* 4 / 3 */

void func_0018F0E0(f32 far, f32 near, f32 fov, f32 aspect, s32 n);
void func_001AE9C0(s32 cmd, void *arg);
void func_0018F7C0(Mat44 *m);
void func_001911F0(Mat44 *m, Vec3 *dir, Vec3 *up);
void func_00190C50(Mat44 *out, Mat44 *a, Mat44 *b);
void func_00190BF0(Mat44 *m);

f32 func_00191950(u32 a);
void func_001903E0(f32 rx, f32 ry, f32 rz, Mat44 *m);

#ifdef NON_MATCHING   /* a float move is scheduled one slot earlier */
/* Rotates m by three 16-bit angles (x, then y, then z). */
void func_00340210(s32 rx, s32 ry, s32 rz, Mat44 *m) {
    f32 x = func_00191950((u16)rx);
    f32 y = func_00191950((u16)ry);

    func_001903E0(x, y, func_00191950((u16)rz), m);
}
#endif

void func_0018FCA0(f32 rx, f32 ry, f32 rz, Mat44 *m);

void func_00340310(s32 rx, s32 ry, s32 rz, Mat44 *m);
#ifdef NON_MATCHING   /* a float move is scheduled one slot earlier */
/* A rotation matrix from three 16-bit angles. */
void func_00340310(s32 rx, s32 ry, s32 rz, Mat44 *m) {
    f32 x = func_00191950((u16)rx);
    f32 y = func_00191950((u16)ry);

    func_0018FCA0(x, y, func_00191950((u16)rz), m);
}
#endif

#ifdef NON_MATCHING   /* VU0 in the original */
/* out = a - b */
void func_00340560(Vec3 *out, Vec3 *a, Vec3 *b) {
    Vec3 d;

    d.x = a->x - b->x;
    d.y = a->y - b->y;
    d.z = a->z - b->z;
    *out = d;
}
#endif

/* The view matrix from the camera. */
void func_00345870(void) {
    Vec3 dir;
    Mat44 look;
    Mat44 rot;

    func_0018F7C0(&rot);
    func_00340210(0, 0, D_00306320.roll, &rot);
    dir.x = -(D_00306320.target.x - D_00306320.pos.x);
    dir.y = -(D_00306320.target.y - D_00306320.pos.y);
    dir.z = -(D_00306320.target.z - D_00306320.pos.z);
    func_001911F0(&look, &dir, &D_00306320.up);
    look.m[3][3] = 1.0f;
    look.m[3][0] = D_00306320.pos.x;
    look.m[3][1] = D_00306320.pos.y;
    look.m[3][2] = D_00306320.pos.z;
    func_00190C50(&D_003062E0, &rot, &look);
    func_00190BF0(&D_003062E0);
}

/* Hands the camera to the renderer. */
void func_00345740(void) {
    if (D_00306320.fov == 0.0f) {
        D_00306320.fov = D_00239D40;
    }
    if (D_00306590.unkF != 0) {
        D_00306320.width = 0x200;
    } else {
        D_00306320.width = 0x280;
    }
    func_0018F0E0(D_00306320.far, D_00306320.near, D_00239D44 * D_00306320.fov, D_00239D48, 0x20);
    func_001AE9C0(0x18, &D_00306320);
    func_00345870();
    func_001AE9C0(0x16, &D_003062E0);
}

#ifdef NON_MATCHING   /* the constants are loaded in a different order */
/* The camera's defaults. */
void func_00345480(void) {
    D_00306320.target.z = 0;
    D_00306320.target.y = 0;
    D_00306320.target.x = 0;
    D_00306320.unk54 = 500.0f;
    D_00306320.pos.z = 500.0f;
    D_00306320.fov = 27.0f;
    D_00306320.pos.y = 0;
    D_00306320.height = 0x1C0;
    D_00306320.pos.x = 0;
    D_00306320.unk30.z = 0;
    D_00306320.unk30.y = 0;
    D_00306320.unk30.x = 0;
    D_00306320.roll = 0;
    D_00306320.far = 50000;
    D_00306320.unk68 = 0;
    D_00306320.unk64 = 0;
    D_00306320.unk48.x = 0;
    D_00306320.unk48.y = 1.0f;
    D_00306320.unk48.z = 0;
    D_00306320.up.x = 0;
    D_00306320.up.y = 1.0f;
    D_00306320.up.z = 0;
    D_00306320.near = 1.0f;
    D_00306320.unk14 = 1.0f;
    D_00306320.unk0 = 0;
    D_00306320.unk4 = 0;
    D_00306320.unk10 = 0;
    if (D_00306590.unkF != 0) {
        D_00306320.width = 0x200;
    } else {
        D_00306320.width = 0x280;
    }
    if (D_00306320.fov == 0.0f) {
        D_00306320.fov = D_00239D9C;
    }
    func_0018F0E0(D_00306320.far, D_00306320.near, D_00239D44 * D_00306320.fov, D_00239DA0, 0x20);
    func_001AE9C0(0x18, &D_00306320);
    func_00345740();
}
#endif

void func_001AEC00(void);
void func_001AEC40(void (*fn)(void));
void func_001B3920(void);
void func_00341B80(void);

/* Starts the in-game screen's per-frame work. */
void func_00341AB0(void) {
    func_001AEC00();
    func_003478E0();
    func_001AEC40(func_00341B80);
    func_001B3920();
    func_00345480();
}

/* Once a frame while in game: everything is drawn from here. */

typedef struct DrawObj {
    s8 active;          /* 0x00 */
    s8 visible;         /* 0x01 */
    u8 unk2[0x1C - 2];
    struct DrawObj *next; /* 0x1C */
} DrawObj;

extern u32 D_0023BCC4;          /* this frame's scratch area */
extern s32 D_0023BCC8;
extern s8 D_0023BCC0;
extern DrawObj *D_0023BDF4;     /* (GAME.BIN) objects of 0xD90 bytes in use */
extern DrawObj *D_0023BE04;     /* (GAME.BIN) objects of 0x1170 bytes in use */
extern u8 D_00476DD0[8][0x10E0];
extern DrawObj D_00470090;
extern s8 D_0048BF88[];
extern u8 *D_00360104;          /* 16 objects of 0x11A0 bytes */

void func_001AE290(void);
void func_0067F290(s32 a);
void func_001966F0(s32 a);
s32 func_001B6980(s32 slot);
void func_00341EB0(void *obj);
void func_00345020(void *obj);
void func_00345170(void *obj);
void func_003479E0(void);
void func_00344A40(void);
void func_00345390(void);
void func_00345210(void *obj);
void func_00651720(void);
void func_00688010(void);
void func_00560B60(void);
void func_00552030(void);
void func_00543860(void);
void func_00620740(void);
void func_001B17C0(void);
void func_003419D0(void);
void func_001BAB90(void);
void func_001B9CC0(void);

#ifdef NON_MATCHING   /* a constant add is folded and registers differ */
void func_00341B80(void) {
    DrawObj *a;
    DrawObj *b;
    u8 *p;
    s8 i;

    func_001AE290();
    D_0023BCC4 = D_00306590.unk4C + 0x2BF000 + 0x10000;
    D_0023BCC8 = 0;
    memset((void *)D_0023BCC4, 0, 0x6000);
    D_0023BCC0 = 0;
    func_0067F290(0);
    func_00345740();
    func_001966F0(0xE);
    func_0067F290(1);
    func_001AE9C0(0x6C, (void *)1);
    if (D_00470090.active != 0 && D_00470090.visible != 0 && func_001B6980(0x1C) == 0) {
        func_00341EB0(&D_00470090);
    }
    func_001AE9C0(0x5E, (void *)0x32);
    func_001AE9C0(0xD, (void *)0);
    func_0067F290(2);
    func_001AE9C0(0x6C, (void *)1);
    if (D_0048BF88[0] == 0) {
        b = D_0023BDF4;
        a = D_0023BE04;
        p = D_00476DD0[0];
        for (i = 0; i < 8; i++, p += 0x10E0) {
            func_00345020(p);
        }
        while (a != NULL) {
            func_00345020(a);
            a = a->next;
        }
        while (b != NULL) {
            func_00345170(b);
            b = b->next;
        }
    }
    func_003479E0();
    func_00344A40();
    func_00345390();
    for (i = 0; i < 8; i++) {
        func_00345210(D_00476DD0[i]);
    }
    func_001AE9C0(0x6C, (void *)1);
    func_00651720();
    func_001AE9C0(0x6C, (void *)1);
    func_00688010();
    func_001AE9C0(0x5F, (void *)5);
    func_001AE9C0(0x60, (void *)0);
    func_00344A40();
    func_00560B60();
    func_00345740();
    func_00552030();
    func_00543860();
    func_001AE9C0(0x6C, (void *)1);
    if (D_0048BF88[0] != 0) {
        p = D_00360104;
        if (p != NULL) {
            for (i = 0; i < 0x10; i++, p += 0x11A0) {
                if (((DrawObj *)p)->active & ((DrawObj *)p)->visible) {
                    func_00341EB0(p);
                }
            }
        }
    }
    func_001AE9C0(0x6C, (void *)1);
    func_001AE9C0(0x5E, (void *)0x32);
    func_001AE9C0(0x5F, (void *)5);
    func_001AE9C0(0x60, (void *)0);
    func_0067F290(1);
    func_00620740();
    func_001B17C0();
    func_003419D0();
    if (D_00306590.unk15 == 0) {
        func_001BAB90();
    }
    func_001B9CC0();
    func_001AE9C0(0x5E, (void *)1);
}
#endif
