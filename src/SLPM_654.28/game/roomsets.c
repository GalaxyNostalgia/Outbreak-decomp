#include "common.h"
#include "options.h"

/* GAME.BIN: four room data records of 0x34-byte entries (kind unknown yet), and the tables reset
   with them. */

typedef struct {
    u16 unk0;           /* 0x000 */
    u16 n;              /* 0x002 */
    u8 unk4[0x34];      /* 0x004 */
    u8 unk38[0x38];     /* 0x038 */
    u8 entries[32][0x34]; /* 0x070 */
    u8 pad[4];
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
extern u8 D_00712320[];
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
                memcpy(s->entries[j], d->entries[j], 0x34);
            }
        }
        func_0067FA80();
    }
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
    u8 unk6[0x14 - 6];
    u16 dir;            /* 0x14: its turn (& 3) */
    u8 unk16[2];
    s8 x;               /* 0x18 */
    s8 y;               /* 0x19 */
    u8 unk1A[5];
    s8 w;               /* 0x1F */
    u8 unk20;
    s8 h;               /* 0x21 */
    u8 unk22[2];
    s32 flags;          /* 0x24: 1 placed, 4 own size (w, h) */
    u8 unk28[0x3C - 0x28];
} RoomExtraSlot;        /* 0x3C */

typedef struct {
    s8 shape;
    u8 unk1[7];
} PlaceKind;

typedef struct {
    s8 rect[4][4];      /* per turn: x0, y0, x1, y1 */
    u8 unk10[4];
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

/* The cells (x0, y0, x1, y1) slot s takes for its turn. (Probably static in the original: its
   callers, all near it, keep values in a/t registers across it, which only a static callee
   gives here; it can be made static once they are all in C.) */
s8 *func_0067B020(RoomExtraSlot *s) {
    return D_006EF480[D_00716000[s->kind].shape].rect[s->dir & 3];
}

/* The room grid: the room's own cells (from the room data) plus what is placed. */
void func_0067C1A0(void);
#ifdef NON_MATCHING   /* the original keeps values in a/t registers across func_0067B020 */
void func_0067C1A0(void) {
    RoomExtraSlot *s;
    s32 i;
    s32 x;
    s32 x1;
    s32 y;
    s32 y0;
    s32 y1;
    s8 *r;

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
#ifdef NON_MATCHING   /* callers of func_0067B020 keep values across it in the original (it is probably static there) */
s8 func_0067D8B0(s8 x, s8 y) {
    RoomExtraSlot *s;
    s32 i;
    s8 x0;
    s8 x1;
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
#endif
