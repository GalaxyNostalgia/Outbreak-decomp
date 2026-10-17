#include "common.h"
#include "options.h"
#include "game.h"
#include "motion.h"
#include "object.h"
#include "model.h"

/* Loading motion files: a list of 20 groups {count, offset of count motion offsets}; each motion
   is decoded (func_001A1540) into a memory handle the first time and its offset slot is replaced
   by {-1, handle}. */

typedef struct {
    u32 count;
    s32 offs;           /* 0x04: offset of count motion offsets (-1: none) */
} MotionGroup;

typedef struct {
    u8 unk0[0x14];
    void *data;         /* 0x14 */
    u8 unk18[0x20 - 0x18];
} MotionTmp;

extern u8 *D_003065DC;
typedef struct {
    u16 index;
    u16 anim;
} MotionBase;
extern MotionBase D_00218E40[];
s32 func_00191ED0(MotionTmp *tmp);
void func_001D6680(MotionTmp *tmp, u8 *work, s32 *src);
s32 func_00191F70(s32 handle);
void func_001A1540(MotionTmp *tmp, u8 *work, s32 *src);

void func_001D4880(u8 *base, MotionSet *set, MotionGroup *g, s32 size, u8 mode);
#ifdef NON_MATCHING   /* first draft (callers depend on func_001D4880's registers) */
void func_001D4880(u8 *base, MotionSet *set, MotionGroup *g, s32 size, u8 mode) {
    MotionTmp tmp;
    u16 h;
    u16 *slot;
    u32 count;
    s32 *offs;
    s32 *m;
    u8 *work;
    u8 *end;
    u8 used;
    s32 index;
    s32 row;
    u16 anim;
    u16 grp;
    u16 k;
    u16 j;

    grp = 0;
    set->loaded = 1;
    index = D_00218E40[mode].index;
    anim = D_00218E40[mode].anim;
    end = base + g->offs;
    work = (u8 *)(((u32)D_003065DC + size + 0x3F) & ~0x3F);
    for (; grp < 20; g++, grp++) {
        if ((u8 *)g >= end) {
            return;
        }
        count = g->count;
        if (count == 0) {
            continue;
        }
        row = grp * 100;
        offs = (s32 *)(base + g->offs);
        for (k = 0; k < count; k++) {
            j = k % set->per;
            if (j == 0) {
                used = 0;
            }
            if (offs[k] != -1) {
                m = (s32 *)(base + offs[k]);
                used = 1;
                if (m[0] == -1) {
                    set->anims[anim + j] = m[1];
                } else {
                    func_001D6680(&tmp, work, m);
                    h = func_00191ED0(&tmp);
                    slot = &set->anims[anim + j];
                    if (*slot != 0) {
                        func_00191F70(*slot);
                    }
                    *slot = h;
                    m[0] = -1;
                    m[1] = h;
                }
                set->index[index + row + k / set->per] = anim;
            }
            if (j == set->per - 1) {
                if (used) {
                }
                anim += set->per;
            }
        }
    }
}
#endif

/* Motion file: {u16 count, u16 per, ...} then the groups at 8. */
void func_001D4B00(u16 *file, MotionSet *set, s32 size, u8 mode);
#ifdef NON_MATCHING   /* first draft (callers depend on func_001D4880's registers) */
void func_001D4B00(u16 *file, MotionSet *set, s32 size, u8 mode) {
    set->count = file[0];
    set->per = file[2];
    func_001D4880((u8 *)file, set, (MotionGroup *)(file + 4), size, mode);
}
#endif

extern MotionSet D_004B9AF0;
extern MotionSet D_004B7BA0;

/* The player motion file: the player's groups at 8, the shared ones at 0xA8. */
void func_001D4B90(s32 *file, s32 size, u8 mode);
void func_001D4C40(void);
#ifdef NON_MATCHING   /* first draft (callers depend on func_001D4880's registers) */
void func_001D4B90(s32 *file, s32 size, u8 mode) {
    s32 count = file[0];
    s32 per = file[1];

    D_004B9AF0.count = count & 0xFFFF;
    D_004B9AF0.per = per & 0xFFFF;
    func_001D4880((u8 *)file, &D_004B9AF0, (MotionGroup *)(file + 2), size, mode);
    D_004B7BA0.count = count & 0xFFFF;
    D_004B7BA0.per = per & 0xFFFF;
    func_001D4880((u8 *)file, &D_004B7BA0, (MotionGroup *)((u8 *)file + 0xA8), size, mode);
}
#endif

extern char D_00238350[];   /* "data\\rom\\motion\\player\\pc_000.bin" */
void func_001B0E90(void);
void func_001B1D40(void);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
s32 sprintf(char *dst, const char *fmt, ...);

/* The player motions (once). */
void func_001D4C40(void);
void func_001D4E10(u8 room);
#ifdef NON_MATCHING   /* first draft (callers depend on func_001D4880's registers) */
void func_001D4C40(void) {
    char name[0x80];
    u8 *buf;

    if (D_0048BF60.unk3F == 0) {
        func_001B0E90();
        buf = D_003065DC;
        sprintf(name, D_00238350);
        func_001D4B90((s32 *)buf, func_001CFBB0(name, buf, 0x20001, 0), 0);
        func_001B1D40();
        D_0048BF60.unk3F = 1;
    }
}
#endif

extern char D_00238380[];   /* "data\\rom\\motion\\player\\pc_%03d.bin" */
extern char D_002383B0[];   /* "data\\rom\\motion\\player\\pl%02d_pc_%03d.bin" */
extern DrawObjFull D_00476DD0[];
extern MotionSet D_004A8180[8];

/* A player motion set: the shared one, then each character's own. */
void func_001D4CD0(u16 id) {
    char name[0x80];
    s32 size;
    u8 *buf = (u8 *)D_00306590.unk4C;
    s32 i;
    char *n;
    u8 *kind;
    DrawObjFull *obj;

    sprintf(name, D_00238380, id);
    n = name;
    size = func_001CFBB0(n, buf, 0x20001, 0);
    func_001D4B90((s32 *)buf, size, 1);
    for (i = 0; i < 8; i++) {
        obj = &D_00476DD0[i];
        if (obj->active != 0) {
            kind = &obj->unkB98;
            sprintf(name, D_002383B0, *kind);
            size = func_001CFBB0(n, buf, 0x20001, size);
            func_001D4B00((u16 *)buf, &D_004A8180[*kind], size, 1);
        }
    }
}

extern char D_002383E0[];   /* "data\\rom\\motion\\rn%02d_tbl.bin" */
extern MotionSet D_004948D0;

/* The room's motions. */
void func_001D4E10(u8 room);
#ifdef NON_MATCHING   /* first draft (callers depend on func_001D4880's registers) */
void func_001D4E10(u8 room) {
    char name[0x80];
    u8 *buf = D_003065DC;

    sprintf(name, D_002383E0, room);
    func_001D4B00((u16 *)buf, &D_004948D0, func_001CFBB0(name, buf, 0x20001, 0), 2);
}
#endif

void func_001D6680(MotionTmp *tmp, u8 *work, s32 *src) {
    func_001A1540(tmp, work, src);
}

void func_001AC150(s32 a);
void func_001B6A40(void);
s32 func_001B6A50(u8 *types);
void func_001AC3F0(void *a, void *b, s32 c);

/* Loads the code of the room's enemy types. */
void func_001D6E70(u8 *types);
#ifdef NON_MATCHING   /* first draft */
void func_001D6E70(u8 *types) {
    u8 *buf = (u8 *)D_00306590.unk4C;
    s32 r;

    func_001AC150(0xC);
    func_001B6A40();
    do {
        r = func_001B6A50(types);
    } while (r == 0);
    if (r >= 0) {
        func_001AC3F0(buf, (u8 *)D_00306590.unk4C + 0x4000, 0xC);
    }
}
#endif

extern Mat44 D_00470BB0[];
extern Mat44 D_00474DC0[];
s32 func_001A2350(void *hdr, void *out, void *motion, s32 root);
s32 func_00191E50(u32 *hdr);
void func_001B01A0(s32 handle);
s32 func_001929B0(s32 h);
u8 *func_001B0330(void);
s32 func_00192F20(FlBone **bones, s32 skel);
void func_001D5DE0(Slot1468 *m, void *motion, Mat44 *world, Mat44 *pose);
void func_001D6110(Slot1468 *m, void *motion);
void func_001D6230(Slot1468 *m, void *motion);
void func_001D6480(Slot1468 *m, void *motion, s32 kind);

/* The skeleton of a motion file's root 0. */
s32 func_001D6670(void *hdr, void *out, void *motion, s32 root) {
    return func_001A2350(hdr, out, motion, root);
}

/* The model's skeleton from its motion file (kept in memory of its own), and a 0x190-byte block
   per bone for it. */
void func_001D5D30(Slot1468 *m, void *motion) {
    u32 hdr[4];
    s32 i;
    u32 *blocks = m->unk0;

    func_001D6670(hdr, (void *)(D_00306590.unk4C + 0x200000), motion, 0);
    m->unk80C = func_00191E50(hdr);
    func_001B01A0(m->unk80C);
    m->nblocks = func_001929B0(m->unk80C);
    for (i = 0; i < m->nblocks; i++) {
        *blocks++ = (u32)func_001B0330();
    }
    func_00192F20((FlBone **)m->unk0, m->unk80C);
}

/* Blocks for the model's second set too. */
void func_001D6690(Slot1468 *m) {
    s32 i;
    u32 *blocks = m->unk4;

    if (blocks != NULL) {
        for (i = 0; i < m->nblocks; i++) {
            *blocks++ = (u32)func_001B0330();
        }
        func_00192F20((FlBone **)m->unk4, m->unk80C);
    }
}

/* Set a model's motion up by its texture id range: characters (0x1800-, 0x2000-, 0x2100-,
   0x2400-), things 0x2A00-, others the plain way. */
void func_001D11E0(s32 id, Slot1468 *m, void *motion) {
    if (id >= 0x2000 && id < 0x2100) {
        func_001D6480(m, motion, 0);
        return;
    }
    if (id >= 0x2400 && id < 0x2500) {
        func_001D6480(m, motion, 1);
        return;
    }
    if (id >= 0x2A00 && id < 0x3000) {
        func_001D6230(m, motion);
        return;
    }
    if (id >= 0x1800 && id < 0x2000) {
        func_001D5DE0(m, motion, D_00470BB0, D_00474DC0);
        return;
    }
    if (id >= 0x2100 && id < 0x2400) {
        func_001D6110(m, motion);
        return;
    }
    func_001D5D30(m, motion);
    if (m->unk4 != NULL && m->unk81C != 1) {
        func_001D6690(m);
    }
}

extern RoomObjModel D_0039A3D0;
s32 func_001A2050(void *p);
s32 func_001A2090(void *f);
s32 func_001A20C0(void *f, u32 n);
s32 func_00191F50(s32 handle);
void func_001B01F0(s32 handle);
s32 func_00192D20(FlBone *bones, s32 skel);
void func_0018F7C0(f32 *m);
void func_00192BE0(FlBone *b, f32 *parent);

/* The room objects' bones (a skeleton root each): for each, its root bone's id and the world and
   bind-pose matrices of the bones under it. */
void func_001D6230(Slot1468 *m, void *motion) {
    u16 first[0x61];
    u32 hdr[4];
    f32 ident[4][4];
    u16 k;
    FlBone *b;
    FlBone *buf;
    u16 r;

    m->nblocks = func_001A2050(motion);
    D_0039A3D0.nroots = func_001A2090(motion);
    for (r = 0; r < D_0039A3D0.nroots; r++) {
        first[r] = func_001A20C0(motion, r);
    }
    first[r] = m->nblocks;
    buf = (FlBone *)(D_00306590.unk4C + 0x200000);
    func_0018F7C0((f32 *)ident);
    for (r = 0; r < D_0039A3D0.nroots; r++) {
        func_001D6670(hdr, buf, motion, r);
        m->unk80C = func_00191E50(hdr);
        func_001B01A0(m->unk80C);
        func_00192D20(buf, m->unk80C);
        func_00192BE0(buf, (f32 *)ident);
        D_0039A3D0.count[r] = 0;
        b = buf;
        for (k = first[r]; k < first[r + 1]; k++, b++) {
            if (k == first[r]) {
                D_0039A3D0.rootId[r] = b->unkC6;
            } else {
                D_0039A3D0.world[r][D_0039A3D0.count[r]] = *(Mat44 *)b->world;
                D_0039A3D0.pose[r][D_0039A3D0.count[r]] = *(Mat44 *)b->pose;
                D_0039A3D0.count[r]++;
            }
        }
        func_00191F50(m->unk80C);
        func_001B01F0(m->unk80C);
        m->unk80C = 0;
    }
}

/* A character's body parts (one skeleton root each): for each, its root bone's id and the ids
   (unk174) of the bones it holds; kind 0 into D_00389750, else work memory. */
typedef struct {
    u8 unk0[0x820];
    s16 nroots;         /* 0x820 */
    u8 unk822[2];
    u16 rootId[0x200];  /* 0x824 */
    s16 count[0x200];   /* 0xC24 */
    s16 bones[1][16];   /* 0x1024 */
} BodyParts;

extern u8 D_00389750[];

#ifdef NON_MATCHING   /* first draft */
void func_001D6480(Slot1468 *m, void *motion, s32 kind) {
    u32 hdr[4];
    u16 first[0x208];
    BodyParts *bp;
    FlBone *b;
    FlBone *buf;
    u16 r;
    u16 k;

    if (kind == 0) {
        bp = (BodyParts *)D_00389750;
    } else {
        bp = (BodyParts *)(D_00306590.unk4C + 0x2FF000);
    }
    m->nblocks = func_001A2050(motion);
    bp->nroots = func_001A2090(motion);
    for (r = 0; r < bp->nroots; r++) {
        first[r] = func_001A20C0(motion, r);
    }
    first[r] = m->nblocks;
    buf = (FlBone *)(D_00306590.unk4C + 0x200000);
    for (r = 0; r < bp->nroots; r++) {
        func_001D6670(hdr, buf, motion, r);
        m->unk80C = func_00191E50(hdr);
        func_001B01A0(m->unk80C);
        func_00192D20(buf, m->unk80C);
        bp->count[r] = 0;
        b = buf;
        for (k = first[r]; k < first[r + 1]; k++, b++) {
            if (k == first[r]) {
                bp->rootId[r] = b->unkC6;
            } else if (b->unk174 >= 0) {
                bp->bones[r][bp->count[r]++] = b->unk174;
            }
        }
        func_00191F50(m->unk80C);
        func_001B01F0(m->unk80C);
        m->unk80C = 0;
    }
}
#endif

/* The room's door models (textures 0x1800-0x1FFF): for each skeleton root (door), its first
   bone, a number from the first unnumbered bone's scale, its position, and the world and pose
   matrices of its numbered bones (unk174) into world/pose. */
extern u8 D_00470090[];
void func_0018FAB0(f32 *out, FlBone *b);

#ifdef NON_MATCHING   /* first draft */
void func_001D5DE0(Slot1468 *m, void *motion, Mat44 *world, Mat44 *pose) {
    f32 ident[4][4];
    u32 hdr[4];
    u16 first[0x20];
    u16 r;
    u16 k;
    FlBone *buf;
    FlBone *b;

    m->nblocks = func_001A2050(motion);
    D_00470090[3] = func_001A2090(motion);
    for (r = 0; r < D_00470090[3]; r++) {
        first[r] = func_001A20C0(motion, r);
        D_00470090[0x2B20 + r] = first[r] - r * 2;
    }
    first[r] = m->nblocks;
    buf = (FlBone *)(D_00306590.unk4C + 0x200000);
    func_0018F7C0((f32 *)ident);
    for (r = 0; r < D_00470090[3]; r++) {
        func_001D6670(hdr, buf, motion, r);
        m->unk80C = func_00191E50(hdr);
        func_001B01A0(m->unk80C);
        func_00192D20(buf, m->unk80C);
        for (k = first[r] + 1, b = buf + 1; k < first[r + 1]; k++, b++) {
            if (b->unk174 < 0) {
                D_00470090[0x2B38 + r] = (u32)b->bind[0][0];
                break;
            }
        }
        func_00192BE0(buf, (f32 *)ident);
        D_00470090[4 + r] = 0;
        for (k = first[r], b = buf; k < first[r + 1]; k++, b++) {
            if (k == first[r]) {
                func_0018FAB0((f32 *)&D_00470090[0x2B50 + r * 0xC], b);
            }
            if (b->unk174 >= 0) {
                world[b->unk174] = *(Mat44 *)b->world;
                pose[b->unk174] = *(Mat44 *)b->pose;
                D_00470090[4 + r]++;
            }
        }
        func_00191F50(m->unk80C);
        func_001B01F0(m->unk80C);
        m->unk80C = 0;
    }
}
#endif

/* Models 0x2100-0x23FF: the world matrices of root 0's numbered bones (unk174) into
   Options.unk68. */
#ifdef NON_MATCHING   /* registers */
void func_001D6110(Slot1468 *m, void *motion) {
    f32 ident[4][4];
    u32 hdr[4];
    Mat44 *out = (Mat44 *)D_00306590.unk68;
    s32 n = func_001A2050(motion);
    FlBone *b = (FlBone *)(D_00306590.unk4C + 0x200000);
    s32 i;

    func_0018F7C0((f32 *)ident);
    func_001D6670(hdr, b, motion, 0);
    m->unk80C = func_00191E50(hdr);
    func_001B01A0(m->unk80C);
    func_00192D20(b, m->unk80C);
    func_00192BE0(b, (f32 *)ident);
    for (i = 0; i < n; i++, b++) {
        if (b->unk174 >= 0) {
            out[b->unk174] = *(Mat44 *)b->world;
        }
    }
    func_00191F50(m->unk80C);
    func_001B01F0(m->unk80C);
    m->unk80C = 0;
}
#endif

/* Another motion file layout: up to 8 groups {count, offset of count motion offsets}; group k's
   motions go to anims[base + k * stride + i]. */
#ifdef NON_MATCHING   /* first draft */
void func_001D46C0(u8 *file, MotionSet *set, s32 stride, MotionGroup *g, s32 size, s32 base) {
    MotionTmp tmp;
    u8 *end = file + g->offs;
    u8 *work = (u8 *)(((u32)D_003065DC + size + 0x3F) & ~0x3F);
    u16 k;
    u16 i;
    u32 count;
    s32 *offs;
    s32 *m;
    u16 *slot;
    u16 h;

    for (k = 0; k < 8 && (u8 *)g < end; k++, g++) {
        count = g->count;
        if (count == 0) {
            continue;
        }
        offs = (s32 *)(file + g->offs);
        for (i = 0; i < count; i++) {
            if (offs[i] == -1) {
                continue;
            }
            m = (s32 *)(file + offs[i]);
            if (m[0] == -1) {
                set->anims[base + i + k * stride] = m[1];
            } else {
                func_001D6680(&tmp, work, m);
                h = func_00191ED0(&tmp);
                slot = &set->anims[base + i + k * stride];
                if (*slot != 0) {
                    func_00191F70(*slot);
                }
                *slot = h;
                m[0] = -1;
                m[1] = h;
            }
        }
    }
}
#endif

/* That file: {u16 count, u16 per, ...}, the groups at 8. */
void func_001D4B20(u16 *file, MotionSet *set, s32 size, s32 stride, s32 base) {
    u8 per = file[2];
    u8 count = file[0];

    set->count = count;
    set->per = per;
    set->loaded = 0;
    func_001D46C0((u8 *)file, set, stride, (MotionGroup *)(file + 4), size, base);
}

void func_001D5100(void) {
}

void func_001D50F0(void) {
}
