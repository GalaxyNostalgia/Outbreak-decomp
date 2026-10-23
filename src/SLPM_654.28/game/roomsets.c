#include "common.h"
#include "options.h"
#include "item.h"

/* GAME.BIN: four room data records of 0x34-byte entries (kind unknown yet), and the tables reset
   with them. */

/* A light as the records give it (scene.c's RoomLight from 0x04). */
typedef struct {
    f32 color[4];       /* 0x00 */
    f32 unk10[4];       /* 0x10 */
    f32 pos[3];         /* 0x20 */
    f32 range;          /* 0x2C */
    f32 reach;          /* 0x30 */
} LightDef;

typedef struct {
    u16 unk0;           /* 0x000: (non-zero: present) */
    u16 n;              /* 0x002 */
    u8 unk4[0x34];      /* 0x004 */
    u8 unk38[0x38];     /* 0x038 */
    LightDef entries[32]; /* 0x070 */
    u32 lit;            /* 0x6F0: the entries made lights */
} RoomSet;              /* 0x6F4 */

typedef struct {
    s32 unk0;
    s32 n;              /* 0x04 */
    u8 unk8[0x34];      /* 0x08 */
    u8 unk3C[0x38];     /* 0x3C */
    u8 entries[1][0x34]; /* 0x74 */
} RoomSetDef;

typedef struct {
    s32 version;        /* 0x0: 4 */
    s32 n;              /* 0x4 */
    s32 offs[1];        /* 0x8 */
} RoomSetFile;

extern RoomSet D_003AD380[4];
extern s32 D_0023BF58;
extern s32 D_0023BF5C;
extern u8 D_0023BF60;
extern u8 D_0023BF64;
extern s8 D_003062BD[];
extern s8 D_003062BE[];
extern u8 D_00306290[];
extern u8 D_0030629C[];
extern u8 D_003062A4[];
extern u8 D_003062B0[];
extern s32 D_007122A0[32];
extern void *D_00712320[32];   /* the lights made from the entries */
extern u8 D_007123A0[];
extern u8 D_007131A0[];
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *d, const void *s, u32 n);

/* Resets the tables that go with them. */
void func_0067FA80(void) {
    s32 i;

    D_003062BE[0] = -1;
    D_0023BF5C = 0;
    memset(D_00712320, 0, 0x80);
    memset(D_007131A0, 0, 0xE00);
    D_0023BF64 = 0;
    memset(D_007123A0, 0, 0xE00);
    D_003062BD[0] = -1;
    D_0023BF60 = 0;
    memset(D_00306290, 0, 8);
    memset(D_0030629C, 0, 8);
    memset(D_003062B0, 0, 0xC);
    memset(D_003062A4, 0, 0xC);
    for (i = 31; i >= 0; i--) {
        D_007122A0[i] = 0;
    }
    D_0023BF58 = 0;
}

/* Copies the records in f (version 4 only). */
void func_0067FB70(RoomSetFile *f) {
    s32 i;
    s32 j;
    s32 n;
    s32 *off;
    RoomSetDef *d;
    RoomSet *s;

    s = D_003AD380;
    memset(s, 0, sizeof(D_003AD380));
    n = f->n;
    off = f->offs;
    if (f->version == 4) {
        for (i = 0; i < n; i++, s++, off++) {
            d = (RoomSetDef *)((u8 *)f + *off);
            s->unk0 = d->unk0;
            s->n = d->n;
            memcpy(s->unk4, d->unk8, 0x34);
            memcpy(s->unk38, d->unk3C, 0x38);
            for (j = 0; j < s->n; j++) {
                memcpy(&s->entries[j], d->entries[j], 0x34);
            }
        }
        func_0067FA80();
    }
}

/* Picks record n (when it's there). */
void func_0067FC90(u8 n) {
    if (n < 4 && D_003AD380[n].unk0 != 0) {
        D_003062BE[0] = n;
    }
}

void func_0067FE20(u8 i);
void *func_00680D00(void *def, u8 group, u8 kind);
s32 func_00680CA0(void *l);

/* Makes entry i of the picked record a light (the second set, kind 1). */
void func_0067FCD0(u8 i);
#ifdef NON_MATCHING   /* the original's early return jumps to ld ra with the index masked in its delay slot */
void func_0067FCD0(u8 i) {
    RoomSet *s = &D_003AD380[D_003062BE[0]];
    LightDef l;

    if (D_003062BE[0] < 0) {
        return;
    }
    if (s->unk0 == 0) {
        return;
    }
    if (s->entries[i].range <= 0.0f) {
        return;
    }
    func_0067FE20(i);
    memcpy(l.color, s->entries[i].color, 0x10);
    memcpy(l.unk10, s->entries[i].unk10, 0x10);
    memcpy(l.pos, s->entries[i].pos, 0xC);
    l.range = s->entries[i].range;
    l.reach = s->entries[i].reach;
    D_00712320[i] = func_00680D00(&l, 0, 1);
    s->lit |= 1 << i;
}
#endif

/* Takes entry i's light away again. */
void func_0067FE20(u8 i) {
    RoomSet *s = &D_003AD380[D_003062BE[0]];

    if (D_00712320[i] != NULL) {
        func_00680CA0(D_00712320[i]);
    }
    s->lit &= ~(1 << i);
}

/* Another room data block (kind unknown yet): a position, two 0x480-byte tables and eight
   switchable entries. */
typedef struct {
    f32 pos[3];         /* 0x000 */
    s32 unkC;           /* 0x00C */
    s32 unk10;          /* 0x010 */
    u8 a[0x480];        /* 0x014 */
    u8 b[0x480];        /* 0x494 */
    u32 flags[8];       /* 0x914: bit 0 on, bits 16-22 and 24-30 */
} RoomExtraDef;

typedef struct {
    f32 pos[3];         /* 0x000 */
    s16 unkC;           /* 0x00C */
    s16 unkE;           /* 0x00E */
    u8 a[0x480];        /* 0x010 */
    u8 b[0x480];        /* 0x490 */
    struct {
        u8 on;
        u8 x;
        u8 y;
        s8 unk3;
    } e[8];             /* 0x910 */
} RoomExtra;

/* Something placed in a room that takes up cells of its grid. */
typedef struct {
    s8 used;
    u8 room;            /* 0x01 */
    u8 unk2[2];
    u16 kind;           /* 0x04: D_00716000 entry */
    u8 unk6[2];
    f32 pos[3];         /* 0x08 */
    u16 dir;            /* 0x14: its turn (& 3) */
    u8 unk16[2];
    s8 x;               /* 0x18 */
    s8 y;               /* 0x19 */
    u8 sides;           /* 0x1A: which sides it is reached from (by turn) */
    u8 unk1B[4];
    s8 w;               /* 0x1F */
    s8 d;               /* 0x20: (own size) its height */
    s8 h;               /* 0x21 */
    u8 unk22[2];
    s32 flags;          /* 0x24: 1 placed, 4 own size (w, h) */
    ItemObj *obj;       /* 0x28: its thing */
    s32 unk2C;          /* 0x2C: its area (an Aot) */
    s32 unk30;          /* 0x30: its side areas */
    s32 unk34;
    s32 unk38;          /* 0x38: the area on top of it */
} RoomExtraSlot;        /* 0x3C */

typedef struct {
    s8 shape;
    s8 unk1;            /* 0x1: (> 0) it has side areas (func_0067BA30) */
    s8 unk2;            /* 0x2: (> 0) it has an area of its own */
    u8 unk3[5];
} PlaceKind;

typedef struct {
    s8 rect[4][4];      /* per turn: x0, y0, x1, y1 */
    u8 unk10;
    s8 height;          /* 0x11: in cells (50 units) */
    u8 unk12[2];
} PlaceShape;           /* 0x14 */

extern PlaceKind D_00716000[];
extern PlaceShape D_006EF480[];
extern u32 D_00711080[96][3];   /* the room grid's taken cells */

extern s8 D_0023BF54;
extern u8 D_0023BF50;
extern RoomExtraSlot D_00395720[32];
extern RoomExtra D_00395EA0;
void func_0067C6E0(void);

/* With D_0023BF54: when a slot is for this room, takes d (and sets D_0023BF50). */
void func_0067B550(RoomExtraDef *d);
#ifdef NON_MATCHING   /* loop registers; the flags address is kept */
void func_0067B550(RoomExtraDef *d) {
    s32 i;
    RoomExtraSlot *s;
    s32 j;
    u16 room;

    if (D_0023BF54 != 0) {
        room = D_00306590.unk1C;
        for (i = 0, s = D_00395720; i < 32; i++, s++) {
            if (s->used != 0 && s->room == room) {
                *(Vec3 *)D_00395EA0.pos = *(Vec3 *)d->pos;
                D_00395EA0.unkC = d->unkC;
                D_00395EA0.unkE = d->unk10;
                memcpy(D_00395EA0.a, d->a, sizeof(d->a));
                memcpy(D_00395EA0.b, d->b, sizeof(d->b));
                for (j = 0; j < 8; j++) {
                    if (d->flags[j] & 1) {
                        D_00395EA0.e[j].on = 1;
                        D_00395EA0.e[j].x = (d->flags[j] >> 16) & 0x7F;
                        D_00395EA0.e[j].y = (d->flags[j] >> 24) & 0x7F;
                    }
                    D_00395EA0.e[j].unk3 = -1;
                }
                D_0023BF50 = 1;
                break;
            }
        }
        func_0067C6E0();
    }
}
#endif

/* The cells (x0, y0, x1, y1) slot s takes for its turn. (Static: its callers keep values in
   a/t registers across it, which only a static callee gives. doors.c's callers, from the same
   original file, use their own copy.) */
static s8 *func_0067B020(RoomExtraSlot *s) {
    return D_006EF480[D_00716000[s->kind].shape].rect[s->dir & 3];
}

typedef struct {
    f32 v[3];
} SlotVec3;

/* Where slot s's thing stands: the room's origin plus its cells (50 units each), centred on
   what it takes up. */
void func_0067B070(f32 *out, RoomExtraSlot *s) {
    s8 *r;

    if (s == NULL || s->used == 0) {
        return;
    }
    if (out == NULL) {
        return;
    }
    *(SlotVec3 *)out = *(SlotVec3 *)D_00395EA0.pos;
    out[0] += 50.0f * s->x;
    out[2] += 50.0f * s->y;
    if (s->flags & 4) {
        out[0] += 0.5f * (50.0f * s->w);
        out[2] += 0.5f * (50.0f * s->h);
    } else {
        r = func_0067B020(s);
        out[0] += 0.5f * (50.0f * (r[0] + r[2]));
        out[2] += 0.5f * (50.0f * (r[1] + r[3]));
    }
}

f32 func_005E5B80(const f32 *p, u8 *sub, s32 want);

typedef struct {
    f32 v[4][2];
} SlotDirs;

extern SlotDirs D_006E5800;     /* the corners' directions */
void func_00340310(s32 rx, s32 ry, s32 rz, f32 *m);
void func_00191410(f32 *v, const f32 *m);

/* Slot s's own area: its size (w x h cells) turned by its heading, 10 units larger. */
void func_0067B890(RoomExtraSlot *s, s32 (*out)[2]);
#ifdef NON_MATCHING   /* element addresses (&c[2], &v[1], &v[2]) kept in s registers */
void func_0067B890(RoomExtraSlot *s, s32 (*out)[2]) {
    SlotDirs dirs = D_006E5800;
    f32 m[16];
    f32 c[3];
    f32 v[3];
    f32 w;
    f32 d;
    s32 i;

    if (s == NULL || s->used == 0) {
        return;
    }
    w = 50.0f * s->w;
    d = 50.0f * s->h;
    func_0067B070(c, s);
    for (i = 0; i < 4; i++) {
        func_00340310(0, s->dir << 14, 0, m);
        v[0] = w * dirs.v[i][0];
        v[1] = 0.0f;
        v[2] = d * dirs.v[i][1];
        func_00191410(v, m);
        out[i][0] = c[0] + v[0] + 10.0f * dirs.v[i][0];
        out[i][1] = c[2] + v[2] + 10.0f * dirs.v[i][1];
    }
}
#endif

/* Puts slot s (and its thing) where it stands. */
void func_0067AF70(RoomExtraSlot *s) {
    f32 p[3];

    if (s == NULL || s->used == 0) {
        return;
    }
    func_0067B070(p, s);
    *(SlotVec3 *)s->pos = *(SlotVec3 *)p;
    if (s->obj != NULL) {
        *(SlotVec3 *)s->obj->pos = *(SlotVec3 *)p;
        func_005E5B80(s->pos, s->obj->unk241, 0xFF);
    }
    if (s->unk2C != 0) {
        func_0067B890(s, (s32 (*)[2])s->unk2C);
    }
}

void func_00590CC0(s32 h);
s32 func_00595D90(ItemObj *o, s32 (*c)[2], s32 bottom, s32 top);
s32 func_00595E60(ItemObj *o, s32 (*c)[2], s32 bottom, s32 top, s32 side);
s32 func_00595F30(ItemObj *o, s32 (*c)[2], s32 bottom, s32 top);
void func_0067BA30(RoomExtraSlot *s, s32 (*a)[2], s32 (*b)[2]);
void func_0067BC50(RoomExtraSlot *s, s32 (*c)[2]);

/* (Re)makes slot s's areas: its own, its sides and its top, by its kind. */
void func_0067BDD0(RoomExtraSlot *s) {
    s32 c[4][2];
    s32 b[4][2];
    s32 y;
    s32 top;
    s8 shape;

    if (s == NULL || s->obj == NULL) {
        return;
    }
    shape = D_00716000[s->kind].shape;
    if (shape == 7) {
        return;
    }
    y = s->obj->pos[1];
    top = y + D_006EF480[shape].height * 50 - 1;
    if (D_00716000[s->kind].unk2 > 0 && (s->sides & 0xF)) {
        func_0067B890(s, c);
        if (s->unk2C != 0) {
            func_00590CC0(s->unk2C);
        }
        s->unk2C = func_00595D90(s->obj, c, y, top);
    }
    if (D_00716000[s->kind].unk1 > 0) {
        func_0067BA30(s, c, b);
        if (s->unk30 != 0) {
            func_00590CC0(s->unk30);
        }
        if (s->unk34 != 0) {
            func_00590CC0(s->unk34);
        }
        s->unk30 = func_00595E60(s->obj, c, y, top, 0);
        s->unk34 = func_00595E60(s->obj, b, y, top, 1);
        func_0067BC50(s, c);
        if (s->unk38 != 0) {
            func_00590CC0(s->unk38);
        }
        s->unk38 = func_00595F30(s->obj, c, top, top + 50);
    }
}

typedef struct {
    f32 v[4][3];
} SlotNormals;

typedef struct {
    s8 v[8][3];
} SlotTris;

extern SlotNormals D_006E58C0;  /* the sides' normals */
extern SlotTris D_006E58F0;     /* the sides' triangles (corner numbers) */
extern SlotNormals D_006E5910;  /* the sides' outward directions */
void *memset(void *p, s32 c, u32 n);
void func_003405D0(f32 s, f32 *out, const f32 *v);
void func_003404F0(f32 *out, const f32 *a, const f32 *b);
s8 func_00549860(const f32 *from, const f32 *to, s32 mask, s32 a, s32 *hit);
void func_005E84F0(f32 *a, f32 *b, f32 *c, f32 *n);

/* Slot s's box (its cells, at least 300 units high unless it has side areas) as collision
   walls; a side with something within 90 units in front of it is pushed out by that much. */
void func_0067CB00(RoomExtraSlot *s);
#ifdef NON_MATCHING   /* first draft: stack layout, &c[1]/&c[2] kept in s registers */
void func_0067CB00(RoomExtraSlot *s) {
    SlotNormals nrm = D_006E58C0;
    SlotTris tris = D_006E58F0;
    SlotNormals dirs = D_006E5910;
    f32 offs[8][3];
    f32 box[8][3];
    f32 dir[3];
    f32 to[3];
    f32 c[3];
    s32 hit;
    f32 x0;
    f32 x1;
    f32 y0;
    f32 y1;
    f32 z0;
    f32 z1;
    s8 *r;
    s32 i;

    if (s->flags & 4) {
        x0 = D_00395EA0.pos[0] + 50.0f * s->x;
        x1 = D_00395EA0.pos[0] + 50.0f * (s->x + s->w);
        y0 = D_00395EA0.pos[1];
        y1 = D_00395EA0.pos[1] + 50.0f * s->d;
        z0 = D_00395EA0.pos[2] + 50.0f * s->y;
        z1 = D_00395EA0.pos[2] + 50.0f * (s->y + s->h);
    } else {
        r = func_0067B020(s);
        x0 = D_00395EA0.pos[0] + 50.0f * (s->x + r[0]);
        x1 = D_00395EA0.pos[0] + 50.0f * (s->x + r[2]);
        y0 = D_00395EA0.pos[1];
        y1 = D_00395EA0.pos[1] + 50.0f * D_006EF480[D_00716000[s->kind].shape].height;
        z0 = D_00395EA0.pos[2] + 50.0f * (s->y + r[1]);
        z1 = D_00395EA0.pos[2] + 50.0f * (s->y + r[3]);
    }
    if (D_00716000[s->kind].unk1 <= 0 && y1 - y0 < 300.0f) {
        y1 = 300.0f + y0;
    }
    box[0][0] = x0;
    box[0][1] = y1;
    box[0][2] = z0;
    box[1][0] = x0;
    box[1][1] = y0;
    box[1][2] = z0;
    box[2][0] = x0;
    box[2][1] = y1;
    box[2][2] = z1;
    box[3][0] = x0;
    box[3][1] = y0;
    box[3][2] = z1;
    box[4][0] = x1;
    box[4][1] = y1;
    box[4][2] = z1;
    box[5][0] = x1;
    box[5][1] = y0;
    box[5][2] = z1;
    box[6][0] = x1;
    box[6][1] = y1;
    box[6][2] = z0;
    box[7][0] = x1;
    box[7][1] = y0;
    box[7][2] = z0;
    memset(offs, 0, sizeof(offs));
    for (i = 0; i < 8; i += 2) {
        c[0] = (box[(i + 3) & 7][0] + (box[(i + 2) & 7][0] + (box[i][0] + box[i + 1][0]))) / 4.0f;
        c[1] = (box[(i + 3) & 7][1] + (box[(i + 2) & 7][1] + (box[i][1] + box[i + 1][1]))) / 4.0f;
        c[2] = (box[(i + 3) & 7][2] + (box[(i + 2) & 7][2] + (box[i][2] + box[i + 1][2]))) / 4.0f;
        func_003405D0(90.0f, dir, dirs.v[i / 2]);
        func_003404F0(to, c, dir);
        if (func_00549860(c, to, 0x4FFFF, 0, &hit)) {
            func_003404F0(offs[i], offs[i], dir);
            func_003404F0(offs[i + 1], offs[i + 1], dir);
            func_003404F0(offs[(i + 2) & 7], offs[(i + 2) & 7], dir);
            func_003404F0(offs[(i + 3) & 7], offs[(i + 3) & 7], dir);
        }
    }
    for (i = 0; i < 8; i++) {
        func_003404F0(box[i], box[i], offs[i]);
    }
    for (i = 0; i < 8; i++) {
        func_005E84F0(box[tris.v[i][0]], box[tris.v[i][1]], box[tris.v[i][2]], nrm.v[i / 2]);
    }
}
#endif

/* The room grid: the room's own cells (from the room data) plus what is placed. */
void func_0067C1A0(void);
#ifdef NON_MATCHING   /* loop registers (the row pointer and y counter) */
void func_0067C1A0(void) {
    RoomExtraSlot *s;
    s32 i;
    s32 x;
    s32 y1;
    s32 x1;
    s8 *r;
    s32 y0;
    s32 y;

    memcpy(D_00711080, D_00395EA0.a, sizeof(D_00395EA0.a));
    for (i = 0, s = D_00395720; i < 32; i++, s++) {
        if (s->used != 0 && (s->flags & 1) && s->room == D_00306590.unk1C) {
            if (s->flags & 4) {
                x = s->x;
                x1 = s->x + s->w;
                y0 = s->y;
                y1 = s->y + s->h;
            } else {
                r = func_0067B020(s);
                x = s->x + r[0];
                x1 = s->x + r[2];
                y0 = s->y + r[1];
                y1 = s->y + r[3];
            }
            for (; x < x1; x++) {
                for (y = y0; y < y1; y++) {
                    D_00711080[x][y >> 5] |= 1 << (y & 0x1F);
                }
            }
        }
    }
}
#endif

/* The slot that takes cell (x, y) of the room grid, or -1. */
s8 func_0067D8B0(s8 x, s8 y);
s8 func_0067D8B0(s8 x, s8 y) {
    s32 i;
    RoomExtraSlot *s;
    s8 x1;
    s8 x0;
    s8 y0;
    s8 y1;
    s8 *r;

    if (x < 0 || x >= D_00395EA0.unkC || y < 0 || y >= D_00395EA0.unkE) {
        return -1;
    }
    for (i = 0, s = D_00395720; i < 32; i++, s++) {
        if (s->used != 0 && (s->flags & 1) && s->room == D_00306590.unk1C) {
            if (s->flags & 4) {
                x0 = s->x;
                x1 = s->x + s->w - 1;
                y0 = s->y;
                y1 = s->y + s->h - 1;
            } else {
                r = func_0067B020(s);
                x0 = s->x + r[0];
                x1 = s->x + r[2] - 1;
                y0 = s->y + r[1];
                y1 = s->y + r[3] - 1;
            }
            if (x >= x0 && x <= x1 && y >= y0 && y <= y1) {
                return i;
            }
        }
    }
    return -1;
}

#include "object.h"

void func_001913A0(f32 *out, const f32 *v, const f32 *m);
void func_003404F0(f32 *out, const f32 *a, const f32 *b);
/* The room grid's cell (50 units each from its origin) under p, into cell when given: whether
   it is inside the grid. */
s32 func_0067E490(s32 *cell, const f32 *p);
#ifdef NON_MATCHING   /* close: the last test's branch sense */
s32 func_0067E490(s32 *cell, const f32 *p) {
    f32 x;
    f32 z;

    if (p == NULL) {
        return 0;
    }
    x = (p[0] - D_00395EA0.pos[0]) / 50.0f;
    z = (p[2] - D_00395EA0.pos[2]) / 50.0f;
    if (cell != NULL) {
        cell[0] = x;
        cell[1] = z;
    }
    if (!(x < 0.0f) && x < D_00395EA0.unkC && !(z < 0.0f)) {
        if (z < D_00395EA0.unkE) {
            return 1;
        }
    }
    return 0;
}
#endif

/* Whether o, at the point it reaches (0x1AC through its matrix), is at a side of slot item's
   thing it faces from (by its heading, the slot's sides) and not on a cell of the slot. */
s32 func_0067E9C0(DrawObjFull *o, u8 item);
#ifdef NON_MATCHING   /* close: delay slots in the side switch (the original leaves nops) */
s32 func_0067E9C0(DrawObjFull *o, u8 item) {
    s32 c[2];
    Vec3 p;
    s32 r;
    RoomExtraSlot *s;
    s32 lo;
    s32 hi;

    s = &D_00395720[item & 0x1F];
    r = 0;

    if (s == NULL || s->used == 0) {
        return 0;
    }
    func_001913A0(&p.x, o->unk1AC, (f32 *)((u8 *)o + 0x50));
    func_003404F0(&p.x, &p.x, o->unk38);
    if (func_0067E490(c, &p.x) && item == (s8)func_0067D8B0((s8)c[0], (s8)c[1])) {
        return 0;
    }
    switch ((1 << ((((u16)o->unk92 + 0x2000) / 0x4000) & 3)) & s->sides & 0xFF) {
    case 2:
    case 8:
        lo = (s32)D_00395EA0.pos[2] + s->y * 50;
        hi = lo + s->h * 50;
        if (!(p.z < (f32)lo) && p.z <= (f32)hi) {
            r = 1;
        }
        break;
    case 1:
    case 4:
        lo = (s32)D_00395EA0.pos[0] + s->x * 50;
        hi = lo + s->w * 50;
        if (!(p.x < (f32)lo) && p.x <= (f32)hi) {
            r = 1;
        }
        break;
    }
    return r;
}
#endif
