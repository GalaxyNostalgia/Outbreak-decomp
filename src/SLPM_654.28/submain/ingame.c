#include "common.h"
#include "options.h"
#include "camera.h"
#include "room.h"
#include "object.h"
#include "system.h"

/* SUBMAIN.BIN: setting up the in-game screen. */

extern u32 D_0023BCD4;           /* effects queued this frame (D_0035C8D0) */

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
/* out = a + b */
void func_003404F0(Vec3 *out, Vec3 *a, Vec3 *b) {
    Vec3 d;

    d.x = a->x + b->x;
    d.y = a->y + b->y;
    d.z = a->z + b->z;
    *out = d;
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
extern u8 D_0023BCC0;            /* the glow pass (0-7) */
extern DrawObj *D_0023BDF4;     /* (GAME.BIN) objects of 0xD90 bytes in use */
extern DrawObj *D_0023BE04;     /* (GAME.BIN) objects of 0x1170 bytes in use */
extern u8 D_00476DD0[8][0x10E0];
extern s8 D_0048BF88[];
extern u8 *D_00360104[];        /* [0]: 16 objects of 0x11A0 bytes */

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
        p = D_00360104[0];
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

/* Drawing one object by its type (the player, enemies, the room...). */


#include "model.h"

/* A part's own placement: its bone and the offset from it. */
typedef struct {
    s32 bone;           /* 0x0: index into the model's bones (Slot1468.unk0) */
    f32 v[3];           /* 0x4 */
} PartXf;

/* Which of a model's parts need special drawing (by character / enemy variant). */
typedef struct {
    s16 alpha;          /* 0x00: these are drawn sorted, with their own matrix (alphaXf) */
    s16 nalpha;         /* 0x02 */
    s16 env;            /* 0x04: these are environment mapped */
    s16 nenv;           /* 0x06 */
    s16 glow;           /* 0x08: these are drawn sorted, glowing (glowXf) */
    s16 nglow;          /* 0x0A */
    PartXf *alphaXf;    /* 0x0C: per alpha part */
    PartXf *glowXf;     /* 0x10: per glow part */
} AnimEntry;            /* 0x14 */

/* A part queued for sorted drawing (0x60 bytes, D_0023BCC4, D_0023BCC8 of them). */
typedef struct {
    void *obj;          /* 0x00 */
    Slot1468 *model;    /* 0x04 */
    s32 part;           /* 0x08 */
    u8 env;             /* 0x0C: environment mapped */
    u8 glow;            /* 0x0D: the glow pass (D_0023BCC0) */
    u8 padE[2];
    Mat44 m;            /* 0x10 */
    f32 envU;           /* 0x50 */
    f32 envV;           /* 0x54 */
    s32 unk58;          /* 0x58 */
    u8 pad5C[4];
} SortedPart;

#define SORTED (((SortedPart *)D_0023BCC4)[D_0023BCC8])

extern AnimEntry D_003559A0[];
extern AnimEntry D_00355A40[];
extern AnimEntry *D_00356850[];
extern AnimEntry *D_0034F680[];
extern u8 D_005E4800[];         /* (GAME.BIN) */
extern u8 D_005A04C0[];         /* (GAME.BIN) */

void func_0018FAB0(f32 *v, u32 a);
void func_00680EF0(f32 *p, s32 group);
void func_001B2590(void *obj);
void func_003435F0(void *obj, u8 *shown, AnimEntry *anim, void (*extra)(void *, s32), u32 color);
void func_00663350(void *obj, u16 i, AnimEntry *anim);
void func_00663DC0(void *obj, u16 i, AnimEntry *anim);
void func_003421D0(void *obj);
void func_003445C0(void *obj);
void func_003424B0(void *obj);
#include "item.h"
extern RoomObjModel D_0039A3D0;
extern u8 D_004BEEF0[][0x4C];
void func_00190D50(f32 *out, const f32 *a, const f32 *b);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
s32 func_0018BC50(u32 n, s32 unused);
void func_00344FF0(void);

/* Draws an item thing: its bones (into world matrices from the room's object model's poses),
   then its model part. */
#ifdef NON_MATCHING   /* the original keeps the stores of an unused point (p); stack layout */
void func_003424B0(void *obj) {
    ItemObj *o = obj;
    s16 model = o->model;
    s16 n;
    s16 i;
    s32 k;
    Mat44 m;
    Mat44 b;
    f32 v[4];
    f32 w[4];
    f32 p[4];
    ModelPart *part;

    if (model == -1) {
        return;
    }
    n = D_0039A3D0.count[model];
    func_0018F7C0(&m);
    func_00344FF0();
    for (i = 0; i < n; i++) {
        func_00190D50((f32 *)&m, (f32 *)&o->parts[i], o->mat);
        func_0018FAB0(v, (u32)&o->parts[i]);
        v[0] -= o->unk218;
        func_001913A0(w, v, o->mat);
        p[0] = o->pos[0] + w[0];
        p[1] = o->pos[1] + w[1];
        p[2] = o->pos[2] + w[2];
        func_00190C50(&b, &D_0039A3D0.pose[model][i], &m);
        func_001AE9C0((u8)(i + 0x1A), &b);
    }
    part = &((ModelPart *)((Slot1468 *)&D_0039A3D0)->buf)[model];
    if (part->group == -1 || part->group == 0) {
        return;
    }
    for (k = 0; k < part->nmats; k++) {
        func_001AE9C0((u8)(k + 0x3A), D_004BEEF0[((Slot1468 *)&D_0039A3D0)->ids[part->mats[k]]]);
    }
    func_0018BC50(part->group, 0);
}
#endif

void func_00344960(RoomPart *p, s32 *a);
/* The glow pass n's 8 KiB of the work area (0xFF: the current pass); 0 past 8. */
s32 func_00344AB0(u8 n);
#ifdef NON_MATCHING   /* the original leaves a nop in the first branch's delay slot */
s32 func_00344AB0(u8 n) {
    if (n == 0xFF) {
        n = D_0023BCC0;
    }
    if (n >= 8) {
        return 0;
    }
    return D_00306590.unk4C + 0x2BF000 + (n << 13);
}
#endif

void func_00344A40(void);
void func_00344B00(f32 *pos, s16 *rot, f32 *m, f32 *u, f32 *v);
void func_00344D50(f32 *pos, s16 *rot, f32 *m);
void func_00344FF0(void);
void func_001922B0(s32 h, u32 a);
void func_00192210(u32 a);
void func_00192350(void *p, s32 h);
void func_0018F8E0(f32 x, f32 y, f32 z, f32 *m);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
s32 func_001B1460(void *fn, void *arg, f32 *pos);
s32 func_0018BC50(u32 n, s32 unused);
extern u8 D_004BEEF0[][0x4C];
void func_00344190(void *cb);
void func_003443A0(void *cb);

/* Draws a model: each shown part with its materials and render state, or queued for sorted
   drawing (anim's alpha and glow parts), the env-mapped ones with their matrix; extra(obj, part)
   after each. */
#ifdef NON_MATCHING   /* register allocation; logic follows the asm */
void func_003435F0(void *obj, u8 *shown, AnimEntry *anim, void (*extra)(void *, s32), u32 color) {
    DrawObjFull *o = obj;
    Slot1468 *model = (Slot1468 *)o->model;
    Mat44 m;
    f32 pos[4];
    s32 h;
    u8 glowed;
    s32 i;
    s32 k;
    ModelPart *part;
    FlBone *b;

    func_0018F8E0(0.0f, 0.0f, 0.0f, (f32 *)&m);
    func_0018FCA0(0.0f, 0.0f, 0.0f, &m);
    func_001AE9C0(0x19, &m);
    func_001AE9C0(0x5E, (void *)0x32);
    glowed = 0;
    h = func_00344AB0(0xFF);
    if (model->unk818 != NULL) {
        func_001922B0(h, *model->unk0);
    } else {
        func_00192210(*model->unk0);
    }
    for (i = 0; i < model->nparts; i++) {
        part = &((ModelPart *)model->buf)[i];
        if (part->group == -1 || part->group == 0 || shown[i] == 0) {
            continue;
        }
        if (anim != NULL && i >= anim->alpha && i < anim->alpha + anim->nalpha) {
            if (D_0023BCC8 >= 0x100) {
                continue;
            }
            SORTED.obj = o;
            SORTED.model = model;
            SORTED.part = i;
            SORTED.m = *(Mat44 *)((FlBone **)model->unk0)[anim->alphaXf[i - anim->alpha].bone];
            func_001913A0(pos, anim->alphaXf[i - anim->alpha].v, (f32 *)&SORTED.m);
            pos[0] += SORTED.m.m[3][0];
            pos[1] += SORTED.m.m[3][1];
            pos[2] += SORTED.m.m[3][2];
            SORTED.m.m[3][0] = pos[0];
            SORTED.m.m[3][1] = pos[1];
            SORTED.m.m[3][2] = pos[2];
            pos[3] = 1.0f;
            if (i >= anim->env && i < anim->env + anim->nenv) {
                SORTED.env = 1;
                func_00344B00(o->unk38, &o->unk90, ((FlBone **)model->unk0)[3]->local[0], &SORTED.envU,
                              &SORTED.envV);
            } else {
                SORTED.env = 0;
                SORTED.unk58 = 0;
                SORTED.envV = 0;
                SORTED.envU = 0;
            }
            func_001B1460(func_00344190, &SORTED, pos);
            D_0023BCC8++;
            continue;
        }
        if (anim != NULL && i >= anim->glow && i < anim->glow + anim->nglow) {
            if (D_0023BCC8 >= 0x100) {
                continue;
            }
            SORTED.obj = o;
            SORTED.model = model;
            SORTED.part = i;
            b = ((FlBone **)model->unk0)[anim->glowXf[i - anim->glow].bone];
            func_001913A0(pos, anim->glowXf[i - anim->glow].v, b->world[0]);
            glowed = 1;
            pos[0] += b->world[3][0];
            pos[1] += b->world[3][1];
            pos[2] += b->world[3][2];
            pos[3] = 1.0f;
            SORTED.glow = D_0023BCC0;
            if (i >= anim->env && i < anim->env + anim->nenv) {
                SORTED.env = 1;
                func_00344B00(o->unk38, &o->unk90, ((FlBone **)model->unk0)[3]->local[0], &SORTED.envU,
                              &SORTED.envV);
            } else {
                SORTED.env = 0;
                SORTED.unk58 = 0;
                SORTED.envV = 0;
                SORTED.envU = 0;
            }
            func_001B1460(func_003443A0, &SORTED, pos);
            D_0023BCC8++;
            continue;
        }
        for (k = 0; k < part->nmats; k++) {
            func_001AE9C0((u8)(k + 0x3A), D_004BEEF0[model->ids[part->mats[k]]]);
        }
        if (part->lit) {
            func_00344960((RoomPart *)part, (s32 *)&color);
        } else {
            func_00344A40();
        }
        func_001AE9C0(0x67, (void *)color);
        if (anim != NULL && anim->nenv != 0) {
            if (anim->env == i) {
                func_00344D50(o->unk38, &o->unk90, ((FlBone **)model->unk0)[3]->local[0]);
                func_001AE9C0(0x5E, (void *)0x12);
            }
            if (i == anim->env + anim->nenv) {
                func_00344FF0();
                func_001AE9C0(0x5E, (void *)0x32);
            }
        }
        if (extra != NULL) {
            extra(o, i);
        }
        if (model->unk818 != NULL) {
            func_00192350(model->unk818 + i * 0x42, h);
        }
        func_0018BC50(((ModelPart *)model->buf)[i].group, 0);
    }
    if (glowed) {
        D_0023BCC0++;
    }
}
#endif

#define OBJ_COLOR(o) ((o)->color[2] | ((o)->color[1] << 8 | ((o)->color[3] << 24 | (o)->color[0] << 16)))

#ifdef NON_MATCHING   /* delay slots filled differently */
void func_00341EB0(void *obj) {
    DrawObjFull *o = obj;
    f32 m[4];
    AnimEntry *anim = NULL;
    AnimEntry *tbl;

    switch (o->type) {
    case 0:
        func_0018FAB0(m, o->model->unk82C);
        func_00680EF0(m, o->unk175);
        if (o->unk54D == 1) {
            anim = &D_003559A0[o->unkB98];
        }
        func_003435F0(o, o->unkF4, anim, NULL, o->unkCE8);
        break;
    case 1:
        func_0018FAB0(m, o->model->unk82C);
        func_00680EF0(m, o->unk175);
        if (o->unk54D == 1) {
            anim = &D_00355A40[o->unkBB4];
        }
        func_003435F0(o, o->unkF4, anim, NULL, -1);
        break;
    case 2:
        func_0018FAB0(m, o->model->unk828);
        func_00680EF0(m, o->unk175);
        if (func_001B6980(0x38) != 0) {
            func_001B2590(o);
        }
        if (o->unk54D == 1) {
            tbl = D_00356850[o->kind];
        } else {
            tbl = D_0034F680[o->kind];
        }
        anim = &tbl[o->unkBBA];
        switch (o->kind) {
        case 0x11:
            func_00663350(o, o->unkBBA, anim);
            break;
        case 0x16:
            func_003435F0(o, o->unkF4, anim, (void (*)(void *, s32))D_005E4800, OBJ_COLOR(o));
            break;
        case 0x1B:
            func_003435F0(o, o->unkF4, anim, (void (*)(void *, s32))D_005A04C0, OBJ_COLOR(o));
            break;
        case 0x25:
            func_00663DC0(o, o->unkBBA, anim);
            break;
        default:
            func_003435F0(o, o->unkF4, anim, NULL, OBJ_COLOR(o));
            break;
        }
        break;
    case 3:
        break;
    case 4:
        func_003421D0(o);
        break;
    case 5:
        break;
    case 6:
        break;
    case 10:
        func_003445C0(o);
        break;
    case 8:
        func_00680EF0(o->unk200, o->unk241);
        func_003424B0(o);
        break;
    case 13:
        break;
    }
}
#endif

/* Drawing the room: every shown part of every group with its matrix and textures. */

extern Mat44 D_0035C7D0;

/* Resets the world matrix (renderer command 0x19) to the identity. */
void func_00344FF0(void) {
    func_0018F7C0(&D_0035C7D0);
    func_001AE9C0(0x19, &D_0035C7D0);
}
void func_00680AB0(f32 *v, s32 group);
void func_0018FA70(Mat44 *m, f32 *v);
/* The renderer state a room part is drawn with (and the texture word a, -1 without). */
void func_00344960(RoomPart *p, s32 *a) {
    func_001AE9C0(0x5E, (void *)(p->unk89 | (p->unk8A << 4)));
    func_001AE9C0(0xD, (void *)(p->unk8B << 10));
    if (p->unk8D) {
        func_001AE9C0(0x5F, (void *)2);
        func_001AE9C0(0x60, (void *)0xFF);
    } else {
        func_001AE9C0(0x5F, (void *)5);
        func_001AE9C0(0x60, (void *)0);
    }
    if (a != NULL) {
        func_001AE9C0(0x67, (void *)*a);
    } else {
        func_001AE9C0(0x67, (void *)-1);
    }
    func_001AE9C0(0x63, (void *)(p->unk8C << 16));
    func_001AE9C0(0x6C, (void *)p->unk8F);
}


#ifdef NON_MATCHING   /* first draft */
void func_003421D0(void *obj) {
    Room *r = obj;
    s32 g;
    s32 j;
    s32 k;
    s32 part;
    u8 f;
    RoomPart *p;
    Mat44 m;

    if (r->visible == 0) {
        return;
    }
    func_00344FF0();
    for (g = 0; g < r->nGroups; g++) {
        func_00680AB0((f32 *)&D_00470090.groupPos[g], D_00470090.groupArg[g]);
        for (j = 0; j < r->groupCount[g]; j++) {
            part = r->groupFirst[g] + j;
            f = r->partFlags[part];
            if (!(f & 1) || (f & 0x80)) {
                continue;
            }
            p = &r->parts[part];
            if (p->model == -1 || p->model == 0) {
                continue;
            }
            if ((r->mtxOverride[part >> 5] & (1 << (part & 0x1F))) && r->partMtxPtr[part] != NULL) {
                m = *r->partMtxPtr[part];
            } else {
                m = r->partMtx[part];
            }
            func_0018FA70(&m, D_00470090.offset);
            func_001AE9C0(0x1A, &m);
            p = &r->parts[part];
            if (p->unk88 != 0) {
                func_00344960(p, &r->unk920[part]);
            }
            for (k = 0; k < r->parts[part].nTex; k++) {
                func_001AE9C0(0x3A + k, D_004BEEF0[r->texId[r->parts[part].tex[k]]]);
            }
            func_0018BC50(r->parts[part].model, 0);
        }
    }
}
#endif

extern u8 D_003AEF74;           /* (GAME.BIN) */
extern s8 D_0048AEA0;
extern s8 D_0048AEA1;
extern s8 D_0048AEA3;
#ifdef PLATFORM_PC
s8 func_005902F0(f32 *pos, s8 range);
#else
s32 func_005902F0(f32 *pos, s8 range);  /* as the original declared it here (s8 in GAME.BIN) */
#endif
void func_00680590(DrawObjFull *o);

#ifdef NON_MATCHING   /* first draft */
/* An object's shadow, unless a debug switch or the camera says otherwise. */
void func_00345020(void *obj) {
    DrawObjFull *o = obj;

    if (!(o->active & o->visible) || o->unk553 == 0) {
        return;
    }
    if (func_001B6980(0x14) != 0 && o->kind == 0) {
        return;
    }
    if (func_001B6980(0x15) != 0 && o->kind == 1) {
        return;
    }
    if (func_001B6980(0x19) != 0 && o->kind == 2) {
        return;
    }
    if (func_001B6980(0x1A) != 0 && o->kind == 3) {
        return;
    }
    if (o->type == 0) {
        if (o->kind == D_003AEF74) {
            if (D_0048AEA0 != 0) {
                return;
            }
        } else {
            if (D_0048AEA1 != 0) {
                return;
            }
            if (func_005902F0(o->unk38, 1) != 0) {
                return;
            }
        }
    } else if (D_0048AEA3 != 0 || func_005902F0(o->unk38, 1) != 0) {
        return;
    }
    func_00680590(o);
}
#endif

#include "enemy.h"
extern u8 D_0048AEA2[];         /* the enemies' shadows are off */

/* An enemy's shadow, unless switched off or out of its draw distance. */
void func_00345170(void *obj) {
    DrawObjFull *o = obj;

    if (o != NULL && o->active != 0 && o->visible != 0 && o->unk553 != 0 && D_0048AEA2[0] == 0 &&
        func_005902F0(o->unk38, D_00357500[o->kind].defs[o->unk54D].unkC) == 0) {
        func_00680590(o);
    }
}

/* Screen effects: a GS packet built in the frame's scratch area and sent straight to the GS. */

typedef struct {
    s32 obj;
    f32 x;
    f32 y;
    f32 z;
} EffectReq;

extern s16 D_0023BCE0;
extern s16 D_0023BCE4;
extern s16 D_0023BCE8;
extern s16 D_0023BCEC;
extern u32 D_0023BCF0;
extern u32 D_0023BCF4;          /* the packet's start */
extern u32 D_0023BCF8;          /* where the packet ends so far */
extern EffectReq D_0035C8D0[];
extern f32 D_0035C8C0[4];
extern u32 D_0023BA8C;

void func_00348680(void);
void func_00347BB0(s32 obj);
void func_00348880(u16 a);
s32 func_0018C9A0(u32 entry, DmaQueue *q);
s32 func_00197330(s32 flags, s32 ctx);
extern u16 D_003062CC;          /* (Scene) */

#ifdef NON_MATCHING   /* first draft */
void func_003479E0(void) {
    u32 start;
    s16 i;
    EffectReq *e;
    s32 obj;
    s32 n;

    if (D_00306590.unk45 != 1) {
        return;
    }
    func_001AE9C0(0x63, (void *)0x10000);
    D_0023BCE8 = 0x1C0;
    D_0023BCE4 = 0;
    D_0023BCE0 = 0;
    D_0023BCEC = D_00281DB0.width;
    D_0023BCF4 = D_00306590.unk4C + (D_00281DB0.dispBuffer << 18);
    start = D_0023BCF4;
    D_0023BCF0 = D_0023BCF4;
    D_0023BCF8 = D_0023BCF4;
    if (D_0023BCD4 != 0) {
        func_00348680();
        for (i = 0; (u32)i < D_0023BCD4; i++) {
            e = &D_0035C8D0[i];
            obj = e->obj;
            if (obj != 0) {
                e->obj = 0;
                D_0035C8C0[0] = e->x;
                D_0035C8C0[1] = e->y;
                D_0035C8C0[2] = e->z;
                D_0035C8C0[3] = 1.0f;
                func_00347BB0(obj);
            }
        }
        D_0023BCD4 = 0;
        func_00348880(D_003062CC);
    }
    if (D_0023BCF4 < D_0023BCF8) {
        n = (s32)(D_0023BCF8 - D_0023BCF4) / 16 - 1;
        ((u64 *)D_0023BCF4)[0] = (u32)((n + 0x70000000) | 0x80000000);
        ((u32 *)D_0023BCF4)[2] = 0x13000000;
        ((u32 *)D_0023BCF4)[3] = n | 0x50000000;
        ((u64 *)D_0023BCF4)[2] = (u32)(n - 1) | (0x8000 | ((u64)0x10000000 << 32));
        ((u64 *)D_0023BCF4)[3] = 0xE;
        func_0018C9A0(start & 0x0FFFFFFF, &D_00281AF0);
        func_00197330(D_0023BA8C, 2);
    }
}
#endif

/* The renderer's defaults for models. */
void func_00344A40(void) {
    func_001AE9C0(0x5E, (void *)0x32);
    func_001AE9C0(0xD, (void *)0);
    func_001AE9C0(0x5F, (void *)5);
    func_001AE9C0(0x60, (void *)0);
    func_001AE9C0(0x67, (void *)-1);
    func_001AE9C0(0x63, (void *)0);
    func_001AE9C0(0x6C, (void *)1);
}

extern u8 D_0023BCBC;
extern u8 *D_00360100[];        /* [0]: 8 objects of 0x10E0 bytes (online: the other players) */

/* Online: the other players' shadows and effects. */
void func_00345390(void) {
    s32 i;
    u8 *p;
    DrawObj *o;

    if (D_0048BF88[0] != 0 && D_0023BCBC == 0) {
    p = D_00360100[0];
    if (p != NULL) {
        for (i = 0; i < 8; i++, p += 0x10E0) {
            func_00345020(p);
        }
    }
    for (o = D_0023BE04; o != NULL; o = o->next) {
        func_00345020(o);
    }
    for (o = D_0023BDF4; o != NULL; o = o->next) {
        func_00345170(o);
    }
    func_003479E0();
    p = D_00360100[0];
    if (p != NULL) {
        for (i = 0; i < 8; i++, p += 0x10E0) {
            func_00345210(p);
        }
    }
    }
}

extern s8 D_003BB520[][0x824];  /* (GAME.BIN) per character: what they hold */
void func_00343E10(s8 *held, AnimEntry *anim, void *obj);

#ifdef NON_MATCHING   /* first draft */
/* Draws a character (its own draw function), then what it holds. */
void func_00345210(void *obj) {
    DrawObjFull *o = obj;
    s8 *held;

    func_001AE9C0(0x63, (void *)0);
    o->drawn = 0;
    if (!(o->active & o->visible)) {
        return;
    }
    if (func_001B6980(0x14) != 0 && o->kind == 0) {
        return;
    }
    if (func_001B6980(0x15) != 0 && o->kind == 1) {
        return;
    }
    if (func_001B6980(0x19) != 0 && o->kind == 2) {
        return;
    }
    if (func_001B6980(0x1A) != 0 && o->kind == 3) {
        return;
    }
    if (o->kind == D_003AEF74) {
        if (D_0048AEA0 != 0) {
            return;
        }
    } else if (D_0048AEA1 != 0 || func_005902F0(o->unk38, 1) != 0) {
        return;
    }
    o->draw(o);
    o->drawn = 1;
    held = D_003BB520[o->kind];
    if (held[0] != 0) {
        func_00343E10(held, &D_003559A0[o->unkB98], o);
    }
}
#endif

void func_001CF590(void);

void func_003419D0(void) {
    func_001CF590();
}

f32 func_00191BF0(f32 x);

/* sqrt that's 0 for 0. */
f32 func_00341860(f32 x) {
    if (x == 0.0f) {
        return 0.0f;
    }
    return func_00191BF0(x);
}
