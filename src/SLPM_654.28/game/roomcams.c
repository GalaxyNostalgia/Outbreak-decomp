#include "common.h"
#include "game.h"
#include "room.h"

/* GAME.BIN: the room's cameras (as script ops 0x11-0x16 treat them), set up from the room data.
   Cameras of type 8 chain (prev/next) into paths. */

/* A camera as the room data gives it. */
typedef struct {
    s32 type;           /* 0x000: 1-9 (0: none) */
    s32 unk4;
    s32 unk8;           /* 0x008 */
    s32 unkC[4];        /* 0x00C */
    f32 unk1C;          /* 0x01C */
    f32 unk20;          /* 0x020 */
    s32 unk24[8][3];    /* 0x024 */
    s32 pts[4][2][3];   /* 0x084: four pairs of points */
    u8 unkE4[0x10];     /* 0x0E4 */
    s32 unkF4;          /* 0x0F4 */
    s32 unkF8;          /* 0x0F8 */
    s32 prev;           /* 0x0FC */
    s32 next;           /* 0x100 */
    f32 unk104[3];      /* 0x104 */
    f32 unk110[3];      /* 0x110 */
    u8 unk11C[8];       /* 0x11C */
    s32 unk124;         /* 0x124 */
    s32 unk128;         /* 0x128 */
    s32 unk12C;         /* 0x12C */
} RoomCamDef;

typedef struct RoomCam {
    u8 off;             /* 0x000 */
    u8 type;            /* 0x001 */
    u8 unk2;
    u8 unk3;            /* 0x003 */
    s32 unk4[4];        /* 0x004 */
    f32 unk14;          /* 0x014 */
    f32 unk18;          /* 0x018 */
    f32 unk1C[3];       /* 0x01C */
    s32 unk28[8][3];    /* 0x028 */
    s32 pts[4][2][3];   /* 0x088 */
    f32 len0;           /* 0x0E8: between the first pair */
    f32 len3;           /* 0x0EC: between the last */
    s32 unkF0;          /* 0x0F0: which corners make the two ends (low and high half) */
    f32 ends[2][2][3];  /* 0x0F4: each end's first and last corner */
    f32 dirs[4][3];     /* 0x124: each end's two edge directions */
    u8 unk154[0x10];    /* 0x154 */
    u8 unk164;          /* 0x164 */
    u8 prev;            /* 0x165: 0xFF: none */
    u8 next;            /* 0x166 */
    u8 unk167;          /* 0x167 */
    f32 unk168[3];      /* 0x168 */
    f32 unk174[3];      /* 0x174 */
    f32 pathPos;        /* 0x180 */
    struct RoomCam *head; /* 0x184: its path's first */
    u8 unk188[8];       /* 0x188 */
    s32 unk190;         /* 0x190 */
    s32 unk194;         /* 0x194 */
} RoomCam;              /* 0x198 */

typedef struct {
    u8 unk0;            /* 0x00 */
    s8 count;           /* 0x01 */
    u8 unk2;            /* 0x02 */
    u8 unk3;            /* 0x03 */
    u8 unk4;
    u8 cur;             /* 0x05 */
    s16 unk6;           /* 0x06 */
    s32 unk8;           /* 0x08 */
    s32 unkC;           /* 0x0C */
    s32 unk10;          /* 0x10 */
    s32 unk14;          /* 0x14 */
    s32 unk18;          /* 0x18 */
    u8 unk1C[8];
    u8 unk24;           /* 0x24: our player */
    u8 unk25;           /* 0x25 */
    u8 unk26;           /* 0x26 */
    u8 unk27;
    f32 unk28;          /* 0x28 */
    u8 unk2C[8];
    RoomCam cams[32];   /* 0x34 */
} RoomCams;             /* 0x3334 */

typedef struct {
    u8 unk0[4];
    u16 count;          /* 0x4 */
    u8 unk6[2];
    s32 offs[1];        /* 0x8: each camera's, from here */
} RoomCamFile;

extern RoomCams D_003AEF50;
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *d, const void *s, u32 n);
f32 func_001916E0(const f32 *a, const f32 *b);

/* Sets the room's cameras up from f. */
void func_0058DE60(RoomCamFile *f);
#ifdef NON_MATCHING   /* first draft */
void func_0058DE60(RoomCamFile *f) {
    RoomCams *t = &D_003AEF50;
    u16 n;
    u16 i;
    s32 *off;
    RoomCamDef *d;
    RoomCam *c;
    f32 v[2][3];
    s8 j;
    s8 k;
    u8 at;
    RoomCam *head;
    RoomCam *p;

    memset(t, 0, sizeof(RoomCams));
    n = f->count;
    D_003AEF50.unk0 = 1;
    D_003AEF50.unk24 = D_0048BF60.unk1D;
    D_003AEF50.unk3 = 0xFF;
    D_003AEF50.unk28 = 80.0f;
    D_003AEF50.unk2 = 0;
    D_003AEF50.count = n;
    D_003AEF50.unk26 = 0;
    D_003AEF50.unk18 = 0;
    D_003AEF50.unk10 = 0;
    D_003AEF50.unk8 = 0;
    D_003AEF50.unk14 = 0;
    D_003AEF50.unkC = 0;
    D_003AEF50.unk6 = 0;
    D_003AEF50.unk25 = 0;
    for (i = 0, off = f->offs; i < n; i++, off++) {
        d = (RoomCamDef *)((u8 *)f + *off);
        switch (d->type) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            c = &t->cams[i];
            c->off = 0;
            c->type = d->type;
            c->unk4[0] = d->unkC[0];
            c->unk4[1] = d->unkC[1];
            c->unk4[2] = d->unkC[2];
            c->unk4[3] = d->unkC[3];
            c->unk14 = d->unk1C;
            c->unk18 = d->unk20;
            c->unkF0 = d->unk8;
            c->unk3 = d->unkF4;
            c->unk167 = d->unk12C;
            memcpy(c->unk154, d->unkE4, 0x10);
            memcpy(c->unk188, d->unk11C, 8);
            c->unk190 = d->unk124;
            c->unk194 = d->unk128;
            for (j = 0; j < 8; j++) {
                for (k = 0; k < 3; k++) {
                    c->unk28[j][k] = d->unk24[j][k];
                }
            }
            for (j = 0; j < 4; j++) {
                for (k = 0; k < 3; k++) {
                    c->pts[j][0][k] = d->pts[j][0][k];
                    c->pts[j][1][k] = d->pts[j][1][k];
                }
            }
            c->unk1C[0] = d->pts[1][0][0];
            c->unk1C[1] = d->pts[1][0][1];
            c->unk1C[2] = d->pts[1][0][2];
            for (j = 0; j < 2; j++) {
                v[j][0] = d->pts[0][j][0];
                v[j][1] = d->pts[0][j][1];
                v[j][2] = d->pts[0][j][2];
            }
            c->len0 = func_001916E0(v[0], v[1]);
            func_0058FDB0(c, 0);
            for (j = 0; j < 2; j++) {
                v[j][0] = d->pts[3][j][0];
                v[j][1] = d->pts[3][j][1];
                v[j][2] = d->pts[3][j][2];
            }
            c->len3 = func_001916E0(v[0], v[1]);
            func_0058FDB0(c, 1);
            c->unk164 = d->unkF8;
            c->prev = d->prev;
            c->next = d->next;
            for (j = 0; j < 3; j++) {
                c->unk168[j] = d->unk104[j];
                c->unk174[j] = d->unk110[j];
            }
            break;
        }
    }
    /* Link the paths: each type 8 camera with no prev starts one. */
    for (i = 0; i < t->count; i++) {
        head = &t->cams[i];
        if (head->type != 8 || head->prev != 0xFF) {
            continue;
        }
        head->pathPos = 0.0f;
        head->head = head;
        at = i;
        p = head;
        while (p->next != 0xFF) {
            c = &t->cams[p->next];
            if (c->type != 8 || c->prev != at) {
                break;
            }
            at = t->cams[c->prev].next;
            c->head = head;
            c->pathPos = c->unk164;
            p = c;
        }
    }
}
#endif

extern u8 D_006D6CD0[4][4];     /* the corners of end 0, by unkF0 & 3 */
extern u8 D_006D6CE0[4][4];     /* end 1, by (unkF0 >> 16) & 3 */
void func_00341140(f32 *a, f32 *b, f32 *c, f32 *out);
void func_00340410(f32 *out, const f32 *a, const f32 *b);
void func_00191750(f32 *v);

/* Works out end which of c: its four corners (on the ground), and from them the edges. */
void func_0058FDB0(RoomCam *c, s32 which);
#ifdef NON_MATCHING   /* registers and frame size */
void func_0058FDB0(RoomCam *c, s32 which) {
    f32 p[4][3];
    f32 n[4];

    if (which == 0) {
        p[0][0] = c->unk28[D_006D6CD0[c->unkF0 & 3][0]][0];
        p[0][2] = c->unk28[D_006D6CD0[c->unkF0 & 3][0]][2];
        p[1][0] = c->unk28[D_006D6CD0[c->unkF0 & 3][1] - 4][0];
        p[1][2] = c->unk28[D_006D6CD0[c->unkF0 & 3][1] - 4][2];
        p[2][0] = c->unk28[D_006D6CD0[c->unkF0 & 3][2]][0];
        p[2][2] = c->unk28[D_006D6CD0[c->unkF0 & 3][2]][2];
        p[3][0] = c->unk28[D_006D6CD0[c->unkF0 & 3][3] - 4][0];
        p[3][2] = c->unk28[D_006D6CD0[c->unkF0 & 3][3] - 4][2];
    } else {
        p[0][0] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][0]][0];
        p[0][2] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][0]][2];
        p[1][0] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][1] - 4][0];
        p[1][2] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][1] - 4][2];
        p[2][0] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][2]][0];
        p[2][2] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][2]][2];
        p[3][0] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][3] - 4][0];
        p[3][2] = c->unk28[D_006D6CE0[(c->unkF0 >> 16) & 3][3] - 4][2];
    }
    p[0][1] = 0.0f;
    p[1][1] = 0.0f;
    p[2][1] = 0.0f;
    p[3][1] = 0.0f;
    *(Vec3 *)c->ends[which][0] = *(Vec3 *)p[0];
    *(Vec3 *)c->ends[which][1] = *(Vec3 *)p[3];
    func_00341140(p[0], p[1], p[2], n);
    func_00340410(c->dirs[which * 2], p[1], n);
    func_00191750(c->dirs[which * 2]);
    func_00341140(p[2], p[3], p[0], n);
    func_00340410(c->dirs[which * 2 + 1], p[0], n);
    func_00191750(c->dirs[which * 2 + 1]);
}
#endif

extern u8 D_0048AC10[];         /* (0x290-0x293: the camera's four switches) */
f32 func_001917C0(const f32 *a, const f32 *b);
void func_00191820(f32 *out, const f32 *a, const f32 *b);
f32 func_00191690(const f32 *v);

typedef struct {
    f32 v[3];
} CamVec;

typedef struct {
    u8 unk0[0x38];
    f32 pos[3];         /* 0x38 */
    u8 unk44[0x10E0 - 0x44];
} CamPlayer;

extern CamPlayer D_00476DD0[];

/* The unit normal of triangle b a p (as SUBMAIN's func_00340630). */
s32 func_0058F630(const f32 *a, const f32 *b, const f32 *p, f32 *out) {
    f32 u[4];
    f32 w[4];

    func_00340410(u, a, b);
    func_00340410(w, p, b);
    func_00191820(out, w, u);
    func_00191750(out);
    return func_00191690(out) != 0.0f;
}

/* Whether p is in front of the plane through q with normal n. */
s32 func_0058F6C0(f32 *p, f32 *q, f32 *n) {
    f32 w[4];

    func_00340410(w, p, q);
    return func_001917C0(w, n) > 0.0f;
}

/* One face (corners a, b, c of camera c's zone) of func_0058F720's test. */
#define CAM_FACE(k, a, b, c_)                                                               \
    t[0][0] = box[a][0];                                                                    \
    t[0][1] = box[a][1];                                                                    \
    t[0][2] = box[a][2];                                                                    \
    t[1][0] = box[b][0];                                                                    \
    t[1][1] = box[b][1];                                                                    \
    t[1][2] = box[b][2];                                                                    \
    t[2][0] = box[c_][0];                                                                   \
    t[2][1] = box[c_][1];                                                                   \
    t[2][2] = box[c_][2];                                                                   \
    if (func_0058F630(t[0], t[1], t[2], n[k]) == 0) {                                       \
        return 0;                                                                           \
    }                                                                                       \
    q[k] = *(CamVec *)t[0];                                                                 \
    pt = *(CamVec *)p->pos;                                                                 \
    pt.v[1] += 10.0f

/* Whether our player (10 up) is inside camera c's zone (eight corners). */
s8 func_0058F720(u16 i, RoomCam *c);
#ifdef NON_MATCHING   /* as pools.c func_00543F60 */
s8 func_0058F720(u16 i, RoomCam *c) {
    CamVec pt;
    f32 n[6][3];
    CamVec q[6];
    f32 t[3][3];
    s32 (*box)[3] = c->unk28;
    CamPlayer *p = &D_00476DD0[D_003AEF50.unk24];

    CAM_FACE(0, 0, 3, 1);
    if (func_0058F6C0(pt.v, q[0].v, n[0])) {
        return 0;
    }
    CAM_FACE(1, 0, 1, 4);
    if (func_0058F6C0(pt.v, q[1].v, n[1])) {
        return 0;
    }
    CAM_FACE(2, 0, 4, 3);
    if (func_0058F6C0(pt.v, q[2].v, n[2])) {
        return 0;
    }
    CAM_FACE(3, 6, 7, 5);
    if (func_0058F6C0(pt.v, q[3].v, n[3])) {
        return 0;
    }
    CAM_FACE(4, 6, 2, 7);
    if (func_0058F6C0(pt.v, q[4].v, n[4])) {
        return 0;
    }
    CAM_FACE(5, 6, 5, 2);
    return func_0058F6C0(pt.v, q[5].v, n[5]) == 0;
}
#endif
void func_0067FEB0(u8 n);
void func_0058EC70(RoomCam *c);
void func_0058ED80(RoomCam *c);
void func_0058F230(RoomCam *c);
void func_00345740(void);

/* Once a frame: picks the camera (the one held by unk6, else the highest-priority one whose
   zone our player is in, keeping the current one among equals); a new camera waits up to 8
   frames when it ranks below the old one, then gets its lights, the room parts it shows and its
   switches, and starts by its type. */
void func_0058E7B0(void);
#ifdef NON_MATCHING   /* first draft */
void func_0058E7B0(void) {
    RoomCams *t = &D_003AEF50;
    RoomCam *c;
    u16 i;
    u16 k;
    u8 sel;
    u8 best;

    D_003AEF50.unk4 = 0;
    if (D_003AEF50.unk0 == 0) {
        return;
    }
    sel = 0xFF;
    if (D_003AEF50.unk6 != 0) {
        sel = D_003AEF50.cur;
    } else {
        for (i = 0, best = sel; i < t->count; i++) {
            c = &t->cams[i];
            if (c->off == 0 || func_0058F720(i, c) != 1) {
                continue;
            }
            if (best == 0xFF) {
                best = c->unk167;
                sel = i;
            } else if (best == c->unk167) {
                if (t->unk3 == i) {
                    sel = i;
                }
            } else if (best < c->unk167) {
                best = c->unk167;
                sel = i;
            }
        }
    }
    if (sel == 0xFF) {
        sel = 0;
        if (t->unk3 != 0xFF) {
            func_00345740();
            return;
        }
    }
    if (t->unk3 == sel) {
        if (t->unk26 < 0xFF) {
            t->unk26++;
        }
    } else {
        if (t->unk3 != 0xFF) {
            if (t->cams[sel].unk167 < t->cams[t->unk3].unk167 && t->unk26 < 8) {
                t->unk26++;
                func_00345740();
                return;
            }
            t->unk4 = 1;
        }
        t->unk3 = sel;
        t->unk26 = 0;
        func_0067FEB0(t->cams[sel].unk3);
        for (k = 0; k < D_00470090.nParts; k++) {
            D_00470090.partFlags[k] &= ~1;
            if (((s32 *)t->cams[sel].unk154)[k >> 5] & (1 << (k & 0x1F))) {
                D_00470090.partFlags[k] |= 1;
            }
        }
        memcpy(t->unk1C, t->cams[sel].unk188, 8);
    }
    c = &t->cams[sel];
    for (k = 0; k < 4; k++) {
        if (c->unk190 & (1 << k)) {
            D_0048AC10[0x290 + k] = 1;
        } else {
            D_0048AC10[0x290 + k] = 0;
        }
    }
    switch (c->type) {
    case 1:
        t->unk18 = (s32)c;
        func_0058EC70(c);
        break;
    case 8:
        func_0058F230(c);
        break;
    default:
        func_0058ED80(c);
        break;
    }
    func_00345740();
}
#endif
