#include "common.h"
#include "options.h"
#include "game.h"
#include "motion.h"
#include "object.h"

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
