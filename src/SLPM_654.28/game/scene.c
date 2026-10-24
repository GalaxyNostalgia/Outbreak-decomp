#include "common.h"
#include "options.h"
#include "scene.h"
#include "object.h"
#include "model.h"
#include "enemy.h"

void *memset(void *p, s32 c, u32 n);

f32 func_00680D70(f32 *l, f32 *p);

/* GAME.BIN: the scene's lights, fog and camera defaults. */

extern s8 D_0023BF64;
extern s8 D_0023BF60;
extern s32 D_0023BF5C;
/* A light of the room (0x38 bytes). */
typedef struct {
    u8 on;              /* 0x00 */
    u8 group;           /* 0x01: 0: any */
    u8 kind;            /* 0x02: (the second set) 0 point, 2 off, 3/4 ... */
    u8 pad3;
    f32 color[4];       /* 0x04 */
    f32 unk14[4];       /* 0x14 */
    f32 pos[3];         /* 0x24 */
    f32 range;          /* 0x30: its falloff (0: off) */
    f32 reach;          /* 0x34: (the second set) how far it reaches */
} RoomLight;

extern RoomLight D_007131A0[64];
extern RoomLight D_007123A0[64];     /* the second set (the two strongest light the characters) */
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
static void func_00681340(Vec3 *out, Vec3 *v, Vec3 *a);
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
        memcpy(&l, &D_00305FA0.b[0], sizeof(Light));
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


/* out = a + b, each component kept within 0..1 (light colours). */
extern f32 D_00239CA8;          /* the strength of a light right at its source */
f32 func_001916E0(const f32 *a, const f32 *b);

void func_00347830(DrawObjFull *o);
extern s8 D_0048BF88[];
extern u8 D_00343340[];         /* (SUBMAIN.BIN) the blob shadow's draw callback */
f32 func_00341860(f32 x);
s32 func_00346C50(const f32 *a, const f32 *b);

void func_003404F0(Vec3 *out, Vec3 *a, Vec3 *b);

/* Clears n bytes (the game's inline loop). */
#define CLEAR(p, n)                                                                         \
    {                                                                                       \
        u8 *q_ = (u8 *)(p);                                                                 \
        s32 n_ = (n);                                                                       \
        if (q_ != NULL) {                                                                   \
            do {                                                                            \
                *q_++ = 0;                                                                  \
            } while (--n_ != 0);                                                            \
        }                                                                                   \
    }

/* A point of o's: the offset va from bone a, vb from bone b, or halfway between them (moved by
   o's last step); o's position with neither (0xFF). */
void func_006BCAA0(DrawObjFull *o, f32 *out, u8 a, u8 b, f32 *va, f32 *vb);
#ifdef NON_MATCHING   /* two address adds take their operands the other way round */
void func_006BCAA0(DrawObjFull *o, f32 *out, u8 a, u8 b, f32 *va, f32 *vb) {
    Vec3 pa;
    Vec3 pb;
    Vec3 d;

    CLEAR(&pa, sizeof(Vec3));
    CLEAR(&pb, sizeof(Vec3));
    if (a == 0xFF && b == 0xFF) {
        *(Vec3 *)out = *(Vec3 *)o->unk38;
        return;
    }
    if (a != 0xFF) {
        func_001913A0(&pa, (Vec3 *)va, (Mat44 *)((FlBone **)((Slot1468 *)o->model)->unk820)[a]);
        func_003404F0(&pa, &pa, (Vec3 *)((FlBone **)((Slot1468 *)o->model)->unk820)[a]->world[3]);
        if (b == 0xFF) {
            *(Vec3 *)out = *&pa;
            return;
        }
    }
    if (b != 0xFF) {
        func_001913A0(&pb, (Vec3 *)vb, (Mat44 *)((FlBone **)((Slot1468 *)o->model)->unk820)[b]);
        func_003404F0(&pb, &pb, (Vec3 *)((FlBone **)((Slot1468 *)o->model)->unk820)[b]->world[3]);
        if (a == 0xFF) {
            *(Vec3 *)out = *&pb;
            return;
        }
    }
    out[0] = 0.5f * (pa.x + pb.x);
    out[1] = 0.5f * (pa.y + pb.y);
    out[2] = 0.5f * (pa.z + pb.z);
    func_00340560(&d, (Vec3 *)o->unk38, (Vec3 *)o->unk44);
    func_003404F0((Vec3 *)out, (Vec3 *)out, &d);
}
#endif

/* A character's blob shadow, as its shadow def says: kind 0 under one point between two bones,
   kind 1 stretched between two such points. */
typedef struct {
    s16 kind;           /* 0x00 */
    s16 size;           /* 0x02 */
    u8 b0, b1;          /* 0x04: the first point's bones */
    u8 b2, b3;          /* 0x06: the second's */
    f32 v0[3];          /* 0x08 */
    f32 v1[3];          /* 0x14 */
    f32 v2[3];          /* 0x20 */
    f32 v3[3];          /* 0x2C */
} ShadowDef;

/* A character's hit shape for the frame (from its shadow def): a capsule from a to b. */
typedef struct {
    f32 a[3];           /* 0x00 */
    f32 b[3];           /* 0x0C */
    f32 r;              /* 0x18: its radius */
    f32 d[3];           /* 0x1C: b - a */
    f32 c[3];           /* 0x28: its middle */
    f32 bound;          /* 0x34: the radius of a sphere round it all */
} HitShape;

typedef struct {
    f32 v[3];
} F3s;

typedef struct {
    f32 x, y, z;
} HitPush;
extern HitPush D_00724230[];    /* per hit entry: a push this frame (x, z) */

/* o's hit shape (hit entry i): its shadow def's point(s), moved by the entry's push. */
void func_006BCCC0(DrawObjFull *o, HitShape *w, s16 i);
#ifdef NON_MATCHING   /* kind 1: the original loads the push again for b (ours reuses it) */
void func_006BCCC0(DrawObjFull *o, HitShape *w, s16 i) {
    ShadowDef *d = (ShadowDef *)o->unk1A8;

    if (d == NULL) {
        return;
    }
    if (d->kind == 0) {
        func_006BCAA0(o, w->a, d->b0, d->b1, d->v0, d->v1);
        w->a[0] += D_00724230[i].x;
        w->a[2] += D_00724230[i].z;
        *(F3s *)w->b = *(F3s *)w->a;
        w->d[2] = 0.0f;
        w->d[1] = 0.0f;
        w->d[0] = 0.0f;
        *(F3s *)w->c = *(F3s *)w->a;
        w->bound = w->r = d->size;
    } else if (d->kind == 1) {
        func_006BCAA0(o, w->a, d->b0, d->b1, d->v0, d->v1);
        func_006BCAA0(o, w->b, d->b2, d->b3, d->v2, d->v3);
        w->a[0] += D_00724230[i].x;
        w->a[2] += D_00724230[i].z;
        w->b[0] += D_00724230[i].x;
        w->b[2] += D_00724230[i].z;
        func_00340560((Vec3 *)w->d, (Vec3 *)w->b, (Vec3 *)w->a);
        w->c[0] = 0.5f * (w->a[0] + w->b[0]);
        w->c[1] = 0.5f * (w->a[1] + w->b[1]);
        w->c[2] = 0.5f * (w->a[2] + w->b[2]);
        w->r = d->size;
        w->bound = w->r + func_001916E0(w->a, w->c);
    }
}
#endif

/* The things held to be hit this frame. */
typedef struct {
    u16 mask;           /* 0x0: the kinds it collides with */
    u16 kind;           /* 0x2: 1, 2: pushed away (func_006BC600) */
    DrawObjFull *obj;   /* 0x4 */
} HitEntry;

extern HitEntry D_00386650[];
extern s16 D_0023BD94;          /* entries */
extern f32 D_00239E88;          /* (the least overlap that counts) */
f32 func_001917C0(const f32 *a, const f32 *b);
void func_003405D0(f32 s, f32 *out, const f32 *v);
u8 func_006BCEF0(f32 ra, f32 rb, f32 *ca, f32 *cb, f32 *push);
u8 func_006BCFB0(f32 r, HitShape *w, f32 *c, f32 *push);
u8 func_006BD1F0(HitShape *a, HitShape *b, f32 *push);
void func_006BC600(s8 i, s8 j, f32 *push);

/* Hit entry i (o, shape w) against the others of the kinds in mask: the one overlapping most
   (ahead of o, for kind 1) is pushed off; 1 when there was one. */
u8 func_006BC150(DrawObjFull *o, HitShape *w, u16 mask, s8 i);
#ifdef NON_MATCHING   /* first draft: registers and the stack (the original keeps &w->c[1], &w->c[2] on the stack) */
u8 func_006BC150(DrawObjFull *o, HitShape *w, u16 mask, s8 i) {
    f32 c[3];
    f32 b[3];
    f32 a[3];
    f32 push[3];
    f32 v[3];
    HitShape w2;
    ShadowDef *d;
    DrawObjFull *p;
    s32 k;
    s32 best;
    s32 how;
    s32 bestHow;
    u8 r = 0;
    f32 most;
    f32 gap;
    f32 reach;
    f32 dist;

    best = -1;
    bestHow = best;
    most = D_00239E88;
    for (k = 0; k < D_0023BD94; k++) {
        if (!(mask & D_00386650[k].kind)) {
            continue;
        }
        p = D_00386650[k].obj;
        if (p == o) {
            continue;
        }
        d = (ShadowDef *)p->unk1A8;
        if (d == NULL || d->size <= 0) {
            continue;
        }
        how = 1;
        if (((ShadowDef *)o->unk1A8)->kind == 0) {
            how = 0;
        }
        if (d->kind == 0) {
            func_006BCAA0(p, c, d->b0, d->b1, d->v0, d->v1);
            reach = w->bound + d->size;
            dist = func_001916E0(c, w->c);
        } else if (d->kind == 1) {
            func_006BCAA0(p, a, d->b0, d->b1, d->v0, d->v1);
            func_006BCAA0(p, b, d->b2, d->b3, d->v2, d->v3);
            c[0] = 0.5f * (a[0] + b[0]);
            c[1] = 0.5f * (a[1] + b[1]);
            c[2] = 0.5f * (a[2] + b[2]);
            reach = w->bound + (d->size + func_001916E0(a, c));
            dist = func_001916E0(c, w->c);
            how |= 2;
        }
        gap = reach - dist;
        if (gap <= most) {
            continue;
        }
        if (D_00386650[i].kind == 1) {
            v[0] = c[0] - w->c[0];
            v[1] = c[1] - w->c[1];
            v[2] = c[2] - w->c[2];
            if (func_001917C0((f32 *)&o->unkA8, v) < 0.0f) {
                continue;
            }
        }
        most = gap;
        bestHow = how;
        best = k;
    }
    if (best < 0) {
        return 0;
    }
    p = D_00386650[best].obj;
    func_006BCCC0(p, &w2, best);
    memset(push, 0, sizeof(push));
    switch (bestHow) {
    case 0:
        r = func_006BCEF0(w->r, w2.r, w->c, w2.c, push);
        break;
    case 1:
        r = func_006BCFB0(w2.r, w, w2.c, push);
        break;
    case 2:
        r = func_006BCFB0(w->r, &w2, w->c, push);
        func_003405D0(-1.0f, push, push);
        break;
    case 3:
        r = func_006BD1F0(&w2, w, push);
        break;
    }
    if (r) {
        if (o->type == 0) {
            FIELD(o, s8, 0xC53) = 1;
            FIELD(o, DrawObjFull *, 0xC54) = p;
        }
        if (p->type == 0) {
            FIELD(p, s8, 0xC53) = 1;
            FIELD(p, DrawObjFull *, 0xC54) = o;
        }
    }
    if (D_00386650[i].kind == 1 || D_00386650[i].kind == 2) {
        if (r) {
            func_006BC600(i, best, push);
            return r;
        }
        o->unk1E0[1] = 0;
    } else if (r) {
        o->unk38[0] = o->unk44[0];
        o->unk38[2] = o->unk44[2];
    }
    return r;
}
#endif

/* o's shadow from its hit shape: under the middle, as long and turned as the shape. */
void func_006BE2F0(DrawObjFull *o, HitShape *w) {
    f32 dx;
    f32 dz;

    o->unk19C = w->c[0];
    o->unk1A0 = w->c[2];
    dx = w->a[0] - w->c[0];
    dz = w->a[2] - w->c[2];
    o->unk1A4 = func_00341860(dx * dx + dz * dz);
    o->unk1A6 = func_00346C50(w->a, w->b);
}

/* A wall the players can't pass this frame (a door's or a shutter's: z <= its own - 200 while
   they are below y -870). */
typedef struct {
    u8 on;              /* 0x0 */
    u8 off;             /* 0x1 */
    u8 unk2[2];
    DrawObjFull *obj;   /* 0x4 */
} PushWall;

extern PushWall D_00386610[];
extern DrawObjFull D_00476DD0[];
extern u32 D_0023BE14;          /* players */
s32 func_001AF4D0(void);

/* Keeps the players (online, ours; offline, the one played) behind wall w. */
void func_006BE610(PushWall *w);
#ifdef NON_MATCHING   /* registers; the original keeps &p->unk38[2] for the store */
void func_006BE610(PushWall *w) {
    DrawObjFull *o = w->obj;
    DrawObjFull *p;
    u32 i;
    f32 z;
    u16 id;
    u32 n;

    if (func_001AF4D0() == 1) {
        id = D_00306590.unk1C;
        n = D_0023BE14;
        for (i = 0; i < n; i++) {
            p = &D_00476DD0[i];
            z = o->unk38[2] - 200.0f;
            if (z < p->unk38[2] && p->unk38[1] < -870.0f && id == p->unk1E2) {
                p->unk38[2] = z;
                p->unk10A9 = 1;
            }
        }
    } else {
        z = o->unk38[2] - 200.0f;
        if (z < D_00476DD0[D_0048BF60.unk1D].unk38[2] && D_00476DD0[D_0048BF60.unk1D].unk38[1] < -870.0f) {
            D_00476DD0[D_0048BF60.unk1D].unk38[2] = z;
            D_00476DD0[D_0048BF60.unk1D].unk10A9 = 1;
        }
    }
}
#endif

void func_006BE5C0(void) {
    PushWall *w;

    if ((w = D_00386610)->on != 0) {
        switch (w->off) {
        case 0:
            func_006BE610(w);
            break;
        }
    }
}

void func_006BE3B0(DrawObjFull *o);
#ifdef NON_MATCHING   /* the missing-def return's branch layout */
void func_006BE3B0(DrawObjFull *o) {
    f32 p[4];
    f32 a[4];
    f32 b[4];
    ShadowDef *d;
    f32 dx;
    f32 dz;

    if (o != NULL) {
        d = (ShadowDef *)o->unk1A8;
        if (d == NULL) {
            return;
        }
        if (d->kind == 0) {
            func_006BCAA0(o, p, d->b0, d->b1, d->v0, d->v1);
            o->unk19C = p[0];
            o->unk1A0 = p[2];
            o->unk1A4 = 0;
            o->unk1A6 = 0;
        } else if (d->kind == 1) {
            func_006BCAA0(o, a, d->b0, d->b1, d->v0, d->v1);
            func_006BCAA0(o, b, d->b2, d->b3, d->v2, d->v3);
            o->unk19C = 0.5f * (a[0] + b[0]);
            o->unk1A0 = 0.5f * (a[2] + b[2]);
            dx = a[0] - o->unk19C;
            dz = a[2] - o->unk1A0;
            o->unk1A4 = func_00341860(dx * dx + dz * dz);
            o->unk1A6 = func_00346C50(a, b);
        }
    }
}
#endif
f32 func_005E5B40(const f32 *p, u8 *kind, s32 want);
void func_001B1460(void *fn, void *a, void *b);

/* A blob shadow under o, on the floor below its shadow point when that is close under it. */
void func_00680790(DrawObjFull *o) {
    f32 p[3];
    u8 kind;

    if (o->unk1A8 != 0 && (f32)((s16 *)o->unk1A8)[1] > 0.0f && D_0048BF88[0] == 0) {
        func_006BE3B0(o);
        p[0] = o->unk19C;
        p[2] = o->unk1A0;
        p[1] = 50.0f + o->unk38[1];
        p[1] = func_005E5B40(p, &kind, 0xFF);
        if (p[1] < 10.0f + o->unk38[1]) {
            p[1] += 3.0f;
            func_001B1460(D_00343340, o, p);
        }
    }
}

/* Draws o's shadow as the scene says for its kind: the model's own (func_00347830, needing its
   character data) or a blob (func_00680790). */
void func_00680590(DrawObjFull *o) {
    s32 m;

    switch (o->type) {
    case 0:
        if (o->unkBB4 != 0) {
            m = D_00305FA0.shadows & 3;
            if (m == 0 || m == 2) {
                func_00680790(o);
            }
        } else if (o->kind == D_0048BF60.unk1D) {
            m = D_00305FA0.shadows & 3;
            if (m == 0) {
                if (((Slot1468 *)o->model)->unk1460 != NULL) {
                    func_00347830(o);
                }
            } else if (m == 2) {
                func_00680790(o);
            }
        } else {
            m = (D_00305FA0.shadows & 0xC) >> 2;
            if (m == 0) {
                if (((Slot1468 *)o->model)->unk1460 != NULL) {
                    func_00347830(o);
                }
            } else if (m == 2) {
                func_00680790(o);
            }
        }
        break;
    case 1:
        if (((Slot1468 *)o->model)->unk1460 != NULL) {
            m = (D_00305FA0.shadows & 0x30) >> 4;
            if (m == 0) {
                func_00347830(o);
            } else if (m == 2) {
                func_00680790(o);
            }
        }
        break;
    case 2:
        m = (D_00305FA0.shadows & 0xC0) >> 6;
        if (m == 0) {
            if (D_00357500[o->kind].defs[o->unkBBA].hasData == 0 ||
                ((Slot1468 *)o->model)->unk1460 == NULL) {
                func_00680790(o);
            } else {
                func_00347830(o);
            }
        } else if (m == 2) {
            func_00680790(o);
        }
        break;
    }
}

/* Makes room light i (out of range or off: none) the characters' light. */
void func_006809B0(s32 i) {
    RoomLight *l;

    if (i >= 0 && i < 64) {
        l = &D_007131A0[i];
        if (l->on) {
            memcpy(&D_00305FA0.a[0].w[1], l->color, 0x10);
            memcpy(&D_00305FA0.a[0].w[9], l->unk14, 0x10);
            memcpy(&D_00305FA0.a[0].w[16], l->pos, 0xC);
            D_00305FA0.a[0].w[20] = l->range;
        } else {
            memset(&D_00305FA0.a[0], 0, 0x68);
        }
    } else {
        memset(&D_00305FA0.a[0], 0, 0x68);
    }
    if (D_00305FA0.enable[0] != 0) {
        func_001AE9C0(0x5A, (u32)&D_00305FA0.a[0]);
    }
    D_00305FA0.mode = -1;
}

/* The brightest room light at p (of the group, or of any with 0) becomes the characters'. */
void func_00680AB0(f32 *p, s32 group) {
    RoomLight *l;
    f32 max;
    f32 v;
    s16 best;
    s16 i;

    best = -1;
    i = 0;
    max = 0.0f;
    for (; i < 64; i++) {
        l = &D_007131A0[i];
        if (l->on == 0 || l->range == 0.0f) {
            continue;
        }
        if (group != 0 && l->group != 0 && (group & 0xFF) != (l->group & 0xFF)) {
            continue;
        }
        v = func_00680D70(l->color, p);
        if (max < v) {
            max = v;
            best = i;
        }
    }
    func_006809B0(best);
}

/* A free light of the second set (marked on), or NULL. */
RoomLight *func_00680C30(void) {
    s32 i;
    RoomLight *l;
    RoomLight *r = NULL;

    if (D_0023BF60 >= 0x40) {
        return r;
    }
    for (i = 0; i < 0x40; i++) {
        l = &D_007123A0[i];
        if (l->on == 0) {
            r = l;
            l->on = 1;
            D_0023BF60++;
            break;
        }
    }
    return r;
}

/* Frees a light of the second set; 1 when it was on. */
s32 func_00680CA0(RoomLight *l);
#ifdef NON_MATCHING   /* the original sets the not-on return 0 after the branch, not in its delay slot */
s32 func_00680CA0(RoomLight *l) {
    if (D_0023BF60 <= 0) {
        return 0;
    }
    if (l != NULL) {
        if (l->on != 0) {
            memset(l, 0, sizeof(RoomLight));
            D_0023BF60--;
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif

/* Adds a light of the second set from def (0x34 bytes: color on), of the group and kind. */
RoomLight *func_00680D00(void *def, u8 group, u8 kind);
#ifdef NON_MATCHING   /* the original tests the saved result (s0) with a nop in the delay slot */
RoomLight *func_00680D00(void *def, u8 group, u8 kind) {
    RoomLight *l = func_00680C30();

    if (l != NULL) {
        if (def != NULL) {
            memcpy(l->color, def, 0x34);
        }
        l->group = group;
        l->kind = kind;
    }
    return l;
}
#endif

/* How bright light l (from its colour) is at p: its brightest channel, falling off with the
   distance; -1 without either. */
f32 func_00680D70(f32 *l, f32 *p) {
    f32 k;
    f32 d;
    f32 m;

    if (l == NULL || p == NULL) {
        return -1.0f;
    }
    d = func_001916E0(l + 8, p);
    if (d == 0.0f) {
        k = D_00239CA8;
    } else {
        k = 1.0f / (l[11] * d);
    }
    m = l[0];
    if (m < l[1]) {
        m = l[1];
    }
    if (m < l[2]) {
        m = l[2];
    }
    return k * m;
}

extern f32 D_00239CDC;          /* pi / 2 */
extern f32 D_00239CE0;
extern s16 D_0023AE8C;          /* -1, -1 */
f32 func_00191B70(f32 x);

/* How bright light l (from its colour) is at p: falling off to 0 at l[12] by a quarter cosine,
   times its brightest channel; -1 without either. */
f32 func_00680E20(f32 *l, f32 *p) {
    f32 r = 0.0f;
    f32 d;
    f32 m;

    if (l == NULL || p == NULL) {
        return -1.0f;
    }
    if (l[12] == 0.0f) {
        return r;
    }
    d = func_001916E0(l + 8, p);
    if (d <= l[12]) {
        r = func_00191B70(D_00239CDC * (d / l[12]));
        m = l[0];
        if (m < l[1]) {
            m = l[1];
        }
        if (m < l[2]) {
            m = l[2];
        }
        r *= m;
    }
    return r;
}

/* The two strongest lights of the second set at p (of the group, or any with 0) become the
   characters' other two lights (Scene b[1], b[2]); kind 0 lights over D_00239CE0 count 0.5,
   kind 4 1, kind 2 none; the others' colours are scaled to their strength and added to the
   ambient (unk310). */
void func_00680EF0(f32 *p, s32 group);
#ifdef NON_MATCHING   /* register allocation and the lights' address arithmetic */
void func_00680EF0(f32 *p, s32 group) {
    s8 idx[2];
    f32 best[2];
    f32 c2[4];
    f32 c1[4];
    RoomLight *l;
    f32 *c;
    f32 v;
    f32 m;
    s32 i;

    CLEAR(best, 8);
    *(s16 *)idx = D_0023AE8C;
    if (p == NULL) {
        return;
    }
    for (i = 0; i < 64; i++) {
        l = &D_007123A0[i];
        if (l->on == 0 || l->range <= 0.0f) {
            continue;
        }
        if (group != 0 && l->group != 0 && (group & 0xFF) != (l->group & 0xFF)) {
            continue;
        }
        c = l->color;
        if (l->kind == 2) {
            continue;
        }
        if (l->kind == 0) {
            v = func_00680E20(c, p);
            if (v > 0.0f && c[11] > D_00239CE0) {
                v = 0.5f;
            }
        } else if (l->kind == 4) {
            v = 1.0f;
        } else {
            v = func_00680E20(c, p);
        }
        if (!(v <= best[0])) {
            best[1] = best[0];
            idx[1] = idx[0];
            best[0] = v;
            idx[0] = i;
        } else if (!(v <= best[1])) {
            best[1] = v;
            idx[1] = i;
        }
    }
    for (i = 0; i < 2; i++) {
        if (idx[i] >= 0) {
            l = &D_007123A0[idx[i]];
            c = l->color;
            if (l->kind == 0 || l->kind == 3 || l->kind == 4) {
                memcpy(&D_00305FA0.b[i + 1].w[1], c, 0x10);
                memcpy(&D_00305FA0.b[i + 1].w[9], c + 4, 0x10);
                memcpy(&D_00305FA0.b[i + 1].w[16], c + 8, 0xC);
                D_00305FA0.b[i + 1].w[20] = c[11];
            } else {
                m = c[0];
                if (m < c[1]) {
                    m = c[1];
                }
                if (m < c[2]) {
                    m = c[2];
                }
                best[i] = best[i] / m;
                c1[0] = c[0] * best[i];
                c1[1] = c[1] * best[i];
                c1[2] = c[2] * best[i];
                c2[0] = c[4] * best[i];
                c2[1] = c[5] * best[i];
                c2[2] = c[6] * best[i];
                func_00681340((Vec3 *)&D_00305FA0.b[i + 1].w[1], (Vec3 *)c1, &D_00305FA0.unk310);
                func_00681340((Vec3 *)&D_00305FA0.b[i + 1].w[9], (Vec3 *)c2, &D_00305FA0.unk310);
                memcpy(&D_00305FA0.b[i + 1].w[16], c + 8, 0xC);
                D_00305FA0.b[i + 1].w[20] = c[11];
            }
        } else {
            memset(&D_00305FA0.b[i + 1], 0, sizeof(Light));
        }
    }
    if (D_00305FA0.enable[4] != 0) {
        func_001AE9C0(0x5B, (u32)&D_00305FA0.b[1]);
    }
    if (D_00305FA0.enable[5] != 0) {
        func_001AE9C0(0x5C, (u32)&D_00305FA0.b[2]);
    }
    D_00305FA0.mode = -1;
}
#endif

static void func_00681340(Vec3 *out, Vec3 *a, Vec3 *b) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
    out->x = out->x < 0.0f ? 0.0f : out->x > 1.0f ? 1.0f : out->x;
    out->y = out->y < 0.0f ? 0.0f : out->y > 1.0f ? 1.0f : out->y;
    out->z = out->z < 0.0f ? 0.0f : out->z > 1.0f ? 1.0f : out->z;
}

void func_00681440(void);
/* Restores the renderer's first light (mode 2's, moved by unk304). */
void func_00681440(void) {
    Light l;

    memcpy(&l, &D_00305FA0.b[0], sizeof(Light));
    func_00681340((Vec3 *)&l.w[1], (Vec3 *)&D_00305FA0.b[0].w[1], &D_00305FA0.unk304);
    func_00681340((Vec3 *)&l.w[9], (Vec3 *)&D_00305FA0.b[0].w[9], &D_00305FA0.unk304);
    func_001AE9C0(0x5A, (u32)&l);
}

extern s8 D_007160F9[];
extern s8 D_007160FA[];
extern s8 D_007160FC[];
void func_0067ACB0(void);
void func_0067B310(void);
void func_0067B420(u16 room);

void func_0067B2C0(void) {
    func_0067ACB0();
    func_0067B420(D_00306590.unk1A);
    D_007160FA[0] = 0;
    D_007160F9[0] = 4;
    D_007160FC[0] = 0;
    func_0067B310();
}
