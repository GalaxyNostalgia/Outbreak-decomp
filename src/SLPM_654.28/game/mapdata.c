#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* The map: the rooms seen (per room, its parts {count, count parts of 8 bytes}). */

typedef struct {
    s32 loaded;         /* 0x00 */
    u8 *end;            /* 0x04 */
    s32 *rooms;         /* 0x08 */
    u8 unkC[0x18 - 0xC];
    u8 *seen;           /* 0x18: per room */
} MapData;

extern MapData D_00720990;
void func_006AB4B0(void);
void func_006A7BE0(s32 room, s32 *parts);

/* Marks room (and the map parts it shows) seen. */
void func_006AB300(s32 room);
#ifdef NON_MATCHING   /* first draft */
void func_006AB300(s32 room) {
    s32 parts[16];
    s32 *r;
    u8 *e;
    s32 n = 0;
    s32 i;
    s32 k;

    func_006AB4B0();
    if (D_00720990.loaded == 0) {
        return;
    }
    D_00720990.seen[room] = 1;
    for (i = 15; i >= 0; i--) {
        parts[i] = -1;
    }
    func_006A7BE0(room, parts);
    for (i = 0; i < 16; i++) {
        if (parts[i] != -1) {
            n++;
        }
    }
    for (i = 0; i < 16; i++) {
        if (parts[i] == -1) {
            continue;
        }
        r = D_00720990.rooms;
        for (k = 0; k < parts[i]; k++) {
            r = (s32 *)((u8 *)r + 4 + r[0] * 8);
        }
        if ((u8 *)r >= D_00720990.end) {
            continue;
        }
        e = (u8 *)(r + 1);
        if (e != NULL && *(s16 *)e != -1) {
            for (k = 0; k < r[0]; k++, e += 8) {
                e[6] = 1;
            }
        }
    }
}
#endif

/* The item messages (itmsg, and the English patch's itmsg...e files), the room's, and the
   special items'. */

extern char D_0070D9B0[];   /* "data\\rom\\scene\\item_msg\\" */
extern char D_0070DAB8[];   /* "%sitmsg.dat" */
extern char D_0070DAC8[];   /* "%sitmsge.dat" */
extern char D_0070DAE0[];   /* "%sitmsg%03d.dat" */
extern char D_0070DAF0[];   /* "%sitmsg%03de.dat" */
extern char D_0070DB08[];   /* "itmsg%03d.dat" */
extern char D_0070DB18[];   /* "itmsg%03de.dat" */
extern char D_0070DB30[];   /* "data\\rom\\scene\\item_msg\\spitem.dat" */
extern char D_0070DB58[];   /* "%sspitem.dat" */
extern void *D_0023BFB8;
extern s32 D_0023BFB4;
extern u8 *D_0023BFC0;      /* the common messages */
extern u8 *D_0023BFC4;      /* the room's */
extern u8 *D_0023BFD0;      /* the special items' */
s32 sprintf(char *dst, const char *fmt, ...);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
s32 func_001A96E0(const char *name, void *buf);
s32 func_006A5BA0(s32 which, s32 a);

extern char D_0070DA38[];   /* "%smap%03d.dat" */
extern char D_0070DA48[];   /* "map%03d.dat" */
s32 func_0018D530(const char *name);

/* Loads the room's map file into buf: its size, or 0 if it is missing or over 0x78000 bytes. */
s32 func_006A6620(void *buf) {
    char name[0x100];
    s32 size;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_0070DA38, D_0070D9B0, D_00306590.unk1A);
        size = func_0018D530(name);
        if (size <= 0 || size > 0x78000) {
            return 0;
        }
        func_001CFBB0(name, buf, 1, size);
    } else {
        sprintf(name, D_0070DA48, D_00306590.unk1A);
        size = func_001A96E0(name, buf);
        if (size <= 0 || size > 0x78000) {
            return 0;
        }
    }
    return size;
}

/* The map screen's state (D_0023BFB8). */
typedef struct {
    u8 unk0[0x14];
    u8 floor;           /* 0x14: the floor shown */
    u8 nrooms;          /* 0x15 */
    u8 unk16[4];
    u8 here;            /* 0x1A: the floor we are on */
    u8 unk1B;
    f32 zoom;           /* 0x1C */
    f32 unk20;          /* 0x20: the floor's scale */
    s16 scroll[4];      /* 0x24 */
    u8 unk2C[4];
    u8 *file;           /* 0x30: the map file: a header of 8 words, offsets from here */
    u8 unk34;           /* 0x34 */
    u8 nfloors;         /* 0x35: floor records (D_00720B18, 0x5C bytes) */
    u8 nused;           /* 0x36: floors with rooms */
    u8 unk37;           /* 0x37: the shown floor's record byte 0x40 */
    u8 *unk38;          /* 0x38 */
    u8 *unk3C;          /* 0x3C */
    s16 unk40;          /* 0x40 */
    s16 unk42;          /* 0x42 */
    u8 *unk44;          /* 0x44 */
    u8 unk48[0x58 - 0x48]; /* 0x48: (func_006A8970) */
    u8 level[16];       /* 0x58: per floor record, its level (0xFF: not shown) */
} MapView;

extern DrawObjFull D_00476DD0[8];
/* A floor record of the map file (0x5C bytes). */
typedef struct {
    u8 unk0[0x40];
    u8 level;           /* 0x40: its level (func_00540A10 says whether it is shown) */
    u8 group;           /* 0x41: floors of a group are stacked by height */
    s16 nparts;         /* 0x42 */
    u32 parts;          /* 0x44: in the map file, 6 bytes each (byte 0: its room record) */
    u8 unk48[4];
    s16 height;         /* 0x4C: the floor above starts here */
    u8 unk4E[0x58 - 0x4E];
    f32 scale;          /* 0x58 */
} MapFloor;

/* A room's record in the map file (0x58 bytes). */
typedef struct {
    u8 unk0;
    u8 id;              /* 0x01: the room */
    u8 unk2[0x58 - 2];
} MapRoom;

/* The map module's state (0xD4 bytes, cleared when the map opens). */
typedef struct {
    s32 unk0;
    u32 width;          /* 0x04: the map texture's (0x100-0x400) */
    u32 height;         /* 0x08 */
    u8 unkC[0x14 - 0xC];
    u32 nclut;          /* 0x14: palettes (3: the plain one and two highlighted) */
    u8 *unk18;          /* 0x18 */
    u8 *clut;           /* 0x1C: 16 colours each */
    u8 *pkt[2];         /* 0x20: the two draw packets */
    u8 *pktEnd[2];      /* 0x28 */
    s32 unk30;          /* 0x30 */
    u8 *unk34;          /* 0x34 */
    s32 tbp;            /* 0x38 */
    s32 tbp2;           /* 0x3C */
    s32 used;           /* 0x40: bytes of the map buffer used */
    s32 unk44;          /* 0x44: packet entries */
    u8 unk48[0x58 - 0x48];
    s32 unk58;          /* 0x58 */
    s32 unk5C;          /* 0x5C */
    s32 unk60;          /* 0x60 */
    s32 unk64;          /* 0x64 */
    u32 rgba;           /* 0x68 */
    u32 nfloors;        /* 0x6C */
    u32 unk70;          /* 0x70 */
    MapRoom *rooms;     /* 0x74 */
    MapFloor *floors;   /* 0x78 */
    u8 unk7C[0x84 - 0x7C];
    s32 tw;             /* 0x84: log2 of width (8-10) */
    s32 th;             /* 0x88 */
    u8 unk8C[0xD4 - 0x8C];
} MapState;

extern MapState D_00720AA0;
void func_003478F0(void);
void *memset(void *p, s32 c, u32 n);
s32 func_00540A10(s32 level);

extern char D_0070D970[];   /* "data\\rom\\scene\\item_msg\\cp_back.img" */
extern char D_0070D998[];   /* "%sfile%03d.dat" */
extern char D_0070D9D0[];   /* "%sfile%03de.dat" */
extern u8 *D_0023BFD4;      /* the files screen's background image */
void func_006A6140(s32 a, u8 *img, s32 b, s32 tbp);

/* Loads the files screen's background (once, into VRAM) and the room's file texts after it,
   past the map in the map buffer: where they are, or 0. */
s32 func_006A5A30(void);
#ifdef NON_MATCHING   /* memset's argument is reloaded from the global in the original */
s32 func_006A5A30(void) {
    char name[0x100];
    u8 *buf;
    s32 n;

    if (D_0048BF60.unkAE == 0) {
        D_0023BFD4 = buf = (u8 *)(((s32)D_00306590.mapBuf + 0x8003F) / 64 * 64);
        memset(D_0023BFD4, 0, 0x594);
        buf = (u8 *)(((s32)buf + 0x5D3) / 64 * 64);
        n = func_001CFBB0(D_0070D970, buf, 1, 0);
        if (n == 0) {
            D_0023BFD4 = NULL;
        } else {
            func_006A6140(0, buf, 0x20, FIELD(&D_00306590, u16, 0x56) + 0x400);
            D_0048BF60.unkAE = 1;
        }
        if (D_0023BFD4 != NULL) {
            buf += (*(s32 *)(D_0023BFD4 + 0x40) + 0x3F) & ~0x3F;
        }
        if (D_00306590.unk3D == 0) {
            sprintf(name, D_0070D998, D_0070D9B0, D_00306590.unk1A);
        } else {
            sprintf(name, D_0070D9D0, D_0070D9B0, D_00306590.unk1A);
        }
        if (func_001CFBB0(name, buf, 0x10001, 0) == 0) {
            buf = NULL;
        }
    } else {
        buf = (u8 *)D_0023BFB4;
    }
    return (s32)buf;
}
#endif

extern u32 D_006FF640[16];      /* the map's palette */
u32 *func_001B56E0(u32 *p, u32 src, s32 dbp, s32 dbw, s32 psm, s32 dx, s32 dy, s32 w, s32 h);

/* Packets uploading the map's palettes. */
u8 *func_006A9550(MapState *st, u8 *p) {
    u32 i;

    for (i = 0; i < st->nclut; i++) {
        p = (u8 *)func_001B56E0((u32 *)p, (u32)&st->clut[i * 64], st->tbp2 + i, 1, 0, 0, 0, 8, 2);
    }
    return p;
}

/* And its texture. */
u8 *func_006A95D0(MapState *st, u8 *p);
#ifdef NON_MATCHING   /* argument scheduling (the original copies st aside first) */
u8 *func_006A95D0(MapState *st, u8 *p) {
    return (u8 *)func_001B56E0((u32 *)p, (u32)st->unk18, st->tbp, st->width >> 6, 0x14, 0, 0,
                               st->width, st->height);
}
#endif

/* Sets the map module up from the opened map: its tables, its palettes (the second colour
   lit in the two other copies) and its two draw packets after them in p. */
void func_006A69F0(s32 a, u8 *p, u16 tbp, MapView *m);
#ifdef NON_MATCHING   /* register allocation */
void func_006A69F0(s32 a, u8 *p, u16 tbp, MapView *m) {
    MapState *st = &D_00720AA0;
    u8 *c;
    u32 *src;
    u8 *q;
    u32 i;
    s32 j;
    s32 k;
    s32 size;

    D_00720AA0.nfloors = m->nfloors;
    D_00720AA0.unk70 = m->unk34;
    D_00720AA0.rooms = (MapRoom *)m->unk38;
    D_00720AA0.floors = (MapFloor *)m->unk3C;
    D_00720AA0.width = (u16)m->unk40 << 8;
    D_00720AA0.height = (u16)m->unk42 << 8;
    switch (D_00720AA0.width) {
    case 0x100:
        st->tw = 8;
        break;
    case 0x200:
        st->tw = 9;
        break;
    case 0x300:
        st->tw = 10;
        break;
    case 0x400:
        st->tw = 10;
        break;
    default:
        st->tw = 8;
        break;
    }
    switch (st->height) {
    case 0x100:
        st->th = 8;
        break;
    case 0x200:
        st->th = 9;
        break;
    case 0x300:
        st->th = 10;
        break;
    case 0x400:
        st->th = 10;
        break;
    default:
        st->th = 8;
        break;
    }
    st->unk18 = m->unk44;
    st->nclut = 3;
    st->unk44 = 0x80;
    st->clut = p;
    c = st->clut;
    for (i = 0; i < st->nclut; i++) {
        src = D_006FF640;
        for (j = 0; j < 16; j++, src++, c += 4) {
            if (j == 1 && i != 0) {
                if (i == 1) {
                    c[3] = 7;
                    c[0] = 0;
                    c[1] = 0x15;
                    c[2] = 0;
                } else {
                    c[3] = 7;
                    c[0] = 0x15;
                    c[1] = 0;
                    c[2] = 0;
                }
            } else {
                c[3] = (*src & 0xFF000000) >> 24;
                c[2] = (*src & 0xFF0000) >> 16;
                c[1] = (*src & 0xFF00) >> 8;
                c[0] = *src;
            }
        }
    }
    p += st->nclut * 64;
    size = (st->unk44 * 0x90 + (st->nclut * 0x90 + 0x11F)) & ~0x3F;
    st->tbp = tbp;
    st->tbp2 = tbp + 0x600;
    for (k = 0; k < 2; k++) {
        q = p;
        st->pkt[k] = p;
        p += size;
        ((u32 *)q)[0] = 0x10000003;
        ((u32 *)q)[1] = 0;
        ((u32 *)q)[2] = 0;
        ((u32 *)q)[3] = 0x51000003;
        ((u64 *)q)[2] = 0x1000000000000002;
        ((u64 *)q)[3] = 0xE;
        ((u64 *)q)[4] = 0x0000008000000068;
        ((u64 *)q)[5] = 0x42;
        ((u64 *)q)[6] = 5;
        ((u64 *)q)[7] = 8;
        st->pktEnd[k] = func_006A95D0(st, func_006A9550(st, q + 0x40));
    }
    st->unk30 = 0;
    st->unk34 = st->pktEnd[0];
    st->used = p - m->file;
    st->unk58 = 0;
    st->unk5C = 0;
    st->unk60 = -2;
    st->unk64 = 0;
    st->rgba = 0x80808080;
}
#endif

/* The floor of room at height y: the first whose parts include the room, among those of its
   group y is below the next one of; -1 if none. */
s32 func_006A8860(u8 room, s16 y);
#ifdef NON_MATCHING   /* the next floor is recomputed from its index in the original */
s32 func_006A8860(u8 room, s16 y) {
    u8 *buf = D_00306590.mapBuf;
    MapState *st = &D_00720AA0;
    u32 n = D_00720AA0.nfloors;
    MapFloor *f;
    u8 *p;
    u32 i;
    s32 k;

    for (i = 0; i < n; i++) {
        f = &st->floors[i];
        if (i == n - 1 || st->floors[i + 1].group != f->group || y < st->floors[i + 1].height) {
            p = f->parts + buf;
            for (k = 0; k < f->nparts; p += 6, k++) {
                if (st->rooms[p[0]].id == room) {
                    return i;
                }
            }
        }
    }
    return -1;
}
#endif

/* The floors' levels, at most 16. */
void func_006A8970(u8 *out) {
    u32 i;
    MapFloor *f = D_00720AA0.floors;
    MapState *st = &D_00720AA0;

    for (i = 0; i < st->nfloors; i++, f++) {
        *out++ = f->level;
        if (i == 15) {
            break;
        }
    }
}

/* Floor n's scale. */
f32 func_006A89C0(s32 n) {
    s32 i;
    MapFloor *f = D_00720AA0.floors;

    for (i = 0; i < n; i++, f++) {
    }
    return f->scale;
}

/* Opens the map: loads its file (once per room), reads its header, and sets up the floors
   with the one we are on shown. */
s32 func_006A6710(MapView *m);
#ifdef NON_MATCHING   /* the last floor lookup's registers */
s32 func_006A6710(MapView *m) {
    u8 cnt[16];
    u8 *buf;
    MapFloor *r;
    s32 *hdr;
    u8 *q;
    s32 i;
    s32 j;
    s32 f;

    func_003478F0();
    if (D_00306590.unk45 != 1) {
        return 0;
    }
    buf = D_00306590.mapBuf;
    memset(&D_00720AA0, 0, 0xD4);
    if (D_0048BF60.unkAD == 0) {
        if (func_006A6620(buf) == 0) {
            return 0;
        }
        D_0048BF60.unkAD = 1;
    }
    m->file = buf;
    hdr = (s32 *)buf;
    q = m->file + hdr[0];
    m->unk34 = hdr[1];
    m->unk38 = m->file + hdr[2];
    m->nfloors = hdr[3];
    m->unk3C = m->file + hdr[4];
    m->unk40 = hdr[5];
    m->unk42 = hdr[6];
    m->unk44 = m->file + hdr[7];
    func_006A69F0(0, q, FIELD(&D_00306590, u16, 0x56), m);
    r = D_00720AA0.floors;
    m->nused = 0;
    for (i = 0; i < m->nfloors; r++, i++) {
        if (func_00540A10(r->level) == 1) {
            m->level[i] = r->level;
        } else {
            m->level[i] = 0xFF;
        }
    }
    for (; i < 16; i++) {
        m->level[i] = 0xFF;
    }
    for (i = 0; i < 16; i++) {
        cnt[i] = 0xFF;
        for (j = 0; j < 16; j++) {
            if (m->level[j] == i) {
                cnt[i]++;
            }
        }
    }
    m->nused = 0;
    for (i = 0; i < 16; i++) {
        if (cnt[i] != 0xFF) {
            m->nused++;
        }
    }
    f = func_006A8860(D_00476DD0[D_0048BF60.unk1D].unk1E2, D_00476DD0[D_0048BF60.unk1D].unk38[1]);
    if (f < 0) {
        f = 0;
    }
    m->here = f;
    m->floor = m->here;
    m->nrooms = m->nfloors;
    m->zoom = 1.0f;
    m->scroll[0] = 0;
    m->scroll[1] = 0;
    m->scroll[2] = 0;
    m->scroll[3] = 0;
    func_006A8970(m->unk48);
    m->unk20 = func_006A89C0(m->floor);
    m->unk37 = D_00720AA0.floors[m->floor].level;
    return func_006A5A30();
}
#endif

/* Reloads the map, when there is one. */
void func_006AB4B0(void) {
    if (D_0023BFB8 != NULL) {
        func_006A6620(D_00306590.mapBuf);
        func_006A6710(D_0023BFB8);
    }
}

/* Loads the item messages (which -1: as the state says). */
void func_006AB4E0(s32 which);
#ifdef NON_MATCHING   /* first draft */
void func_006AB4E0(s32 which) {
    char name[0x100];
    u8 *buf;
    s32 size;

    if (D_0048BF60.unk28 != 0) {
        return;
    }
    if (which == -1) {
        if (FIELD(&D_0048BF60, u8, 0xAC) == 0 && FIELD(&D_0048BF60, u8, 0xAD) == 0) {
            which = 0;
        } else if (FIELD(&D_0048BF60, u8, 0xAC) != 0) {
            which = 0;
        } else {
            which = 1;
        }
        FIELD(&D_0048BF60, u8, 0xAC) = 0;
        FIELD(&D_0048BF60, u8, 0xAD) = 0;
        FIELD(&D_0048BF60, s8, 0xAE) = 0;
    }
    switch (which) {
    case 0:
    case 2:
    case 3:
        FIELD(&D_0048BF60, u8, 0xAD) = 0;
        break;
    case 1:
        FIELD(&D_0048BF60, u8, 0xAC) = 0;
        break;
    }
    if (which == 1) {
        if (D_0023BFB8 != NULL) {
            D_0023BFB4 = func_006A6710(D_0023BFB8);
        }
        FIELD(D_0023BFB8, s16, 0x2C) = 0;
        FIELD(D_0023BFB8, s16, 0x2E) = 0;
        if (D_0023BFB4 == 0) {
            FIELD(&D_0048BF60, u8, 0xAD) = 0;
            return;
        }
        D_0023BFC0 = NULL;
        buf = (u8 *)FIELD(&D_00306590, s32, 0x80);
        D_0023BFC4 = NULL;
        if (D_00306590.unk3D == 0) {
            sprintf(name, D_0070DAB8, D_0070D9B0);
        } else {
            sprintf(name, D_0070DAC8, D_0070D9B0);
        }
        size = func_001CFBB0(name, buf, 1, 0);
        if (size != 0) {
            D_0023BFC0 = buf;
        }
        buf += size;
        if (D_00306590.unk17 == 0) {
            if (D_00306590.unk3D == 0) {
                sprintf(name, D_0070DAE0, D_0070D9B0, D_00306590.unk1A);
            } else {
                sprintf(name, D_0070DAF0, D_0070D9B0, D_00306590.unk1A);
            }
            size = func_001CFBB0(name, buf, 1, 0);
        } else {
            if (D_00306590.unk3D == 0) {
                sprintf(name, D_0070DB08, D_00306590.unk1A);
            } else {
                sprintf(name, D_0070DB18, D_00306590.unk1A);
            }
            size = func_001A96E0(name, buf);
        }
        if (size != 0) {
            D_0023BFC4 = buf;
        }
        buf += size;
        if (func_001CFBB0(D_0070DB30, buf, 1, 0) != 0) {
            D_0023BFD0 = buf;
        }
        FIELD(&D_0048BF60, u8, 0xAD) = 1;
        return;
    }
    if (FIELD(&D_0048BF60, u8, 0xAC) != 0) {
        D_0023BFB4 = func_006A5BA0(which, -1);
        return;
    }
    D_0023BFC0 = NULL;
    buf = (u8 *)FIELD(&D_00306590, s32, 0x80);
    D_0023BFC4 = NULL;
    if (D_00306590.unk3D == 0) {
        sprintf(name, D_0070DAB8, D_0070D9B0);
    } else {
        sprintf(name, D_0070DAC8, D_0070D9B0);
    }
    size = func_001CFBB0(name, buf, 1, 0);
    if (size != 0) {
        D_0023BFC0 = buf;
    }
    buf += size;
    if (D_00306590.unk17 == 0) {
        if (D_00306590.unk3D == 0) {
            sprintf(name, D_0070DAE0, D_0070D9B0, D_00306590.unk1A);
        } else {
            sprintf(name, D_0070DAF0, D_0070D9B0, D_00306590.unk1A);
        }
        size = func_001CFBB0(name, buf, 1, 0);
    } else {
        if (D_00306590.unk3D == 0) {
            sprintf(name, D_0070DB08, D_00306590.unk1A);
        } else {
            sprintf(name, D_0070DB18, D_00306590.unk1A);
        }
        size = func_001A96E0(name, buf);
    }
    if (size != 0) {
        D_0023BFC4 = buf;
    }
    buf += size;
    sprintf(name, D_0070DB58, D_0070D9B0);
    if (func_001CFBB0(name, buf, 1, 0) != 0) {
        D_0023BFD0 = buf;
    }
    D_0023BFB4 = func_006A5BA0(which, -1);
}
#endif

/* The map screen. */
void func_001AECB0(void (*fn)(void));
void func_006B7020(void);

void func_006AC8A0(void) {
    func_001AECB0(func_006B7020);
    FIELD(&D_0048BF60, s8, 0xB0) = 0;
    FIELD(&D_0048BF60, s8, 0xB1) = 0;
}

typedef struct {
    u8 unk0[4];
    u8 open;            /* 0x04: 0 closed, 1 the menu, 2 ..., 4, 5 */
    u8 unk5[0x32C0 - 5];
    u16 pad;            /* 0x32C0: buttons pressed */
    u8 unk32C2[0x33C0 - 0x32C2];
    s8 unk33C0;         /* 0x33C0 */
} MapScreen;

extern MapScreen D_00720BE0;
extern s8 D_00723F94;
extern u8 *D_0023BFBC;
void *memset(void *p, s32 c, u32 n);
void func_0064E360(void *p, s32 a, u8 b);
void func_006B9910(void);
void func_001B6EB0(void);
void func_006B9610(void);
void func_006A5A10(s32 a);

/* Whether the map screen is up (with 0x63 on; open 2 only when unk1E is set). */
s32 func_006AC490(void);
#ifdef NON_MATCHING   /* the compiler folds the last two returns into set-if-not-zero */
s32 func_006AC490(void) {
    u8 open;

    if ((u8)D_0048BF60.unk63 == 0) {
        return 0;
    }
    open = D_00720BE0.open;
    if (open == 0) {
        return 0;
    }
    if (open == 2) {
        if (D_0048BF60.unk1E[0] != 0) {
            return 1;
        }
        return 0;
    }
    return 1;
}
#endif

/* Closes the map screen. */
void func_006AE930(void);
#ifdef NON_MATCHING   /* first draft */
void func_006AE930(void) {
    u8 *p = (u8 *)&D_00476DD0[D_0048BF60.unk1D];

    FIELD(p, u32, 0xBA0) &= ~0x8000;
    switch (D_00720BE0.open) {
    case 0:
        return;
    case 1:
        if (D_0023BFBC[0x310D] != 0) {
            func_0064E360(p, 2, p[0xC7A]);
        }
        D_00723F94 = 0;
        func_006B9910();
        func_001B6EB0();
        FIELD(&D_0048BF60, s8, 0xB0) = 0;
        break;
    case 2:
        func_001B6EB0();
        break;
    }
    D_00720BE0.open = 0;
    func_006B9610();
    FIELD(&D_0048BF60, s8, 0x1E) = 0;
    func_006A5A10(0);
    func_006AC8A0();
}
#endif

extern u8 D_00389740[0x10];
extern u8 D_00723F9E;
void func_006AC230(void);
s32 func_001AF510(void);
s32 func_001AF500(void);
void func_006AC8D0(u8 mode);
u8 func_005C1060(DrawObjFull *p);
void func_006AE420(s32 mode, u8 player);
void func_005C15E0(DrawObjFull *p);
void func_006AC3C0(void);
void func_006AA8A0(void);
void func_006AF010(void);
void func_006B3600(void);
void func_006B32D0(void);
void func_006B38E0(void);
void func_006B9D20(void);
void func_006BA610(void);

/* Once a frame: the in-game menu (opened with its buttons when it is closed). */
void func_006AC510(void);
#ifdef NON_MATCHING   /* first draft */
void func_006AC510(void) {
    MapScreen *m = &D_00720BE0;
    DrawObjFull *p;
    u8 r;

    if (FIELD(&D_0048BF60, u8, 0x63) == 0) {
        return;
    }
    func_006AC230();
    if (func_001AF510() == 1) {
        p = &D_00476DD0[D_0048BF60.unk1D];
    } else {
        p = &D_00476DD0[0];
    }
    if (m->open != 1 && m->open < 4) {
        func_006AC8D0(m->open);
    }
    switch (m->open) {
    case 0:
        if (FIELD(&D_0048BF60, u8, 0x1E) != 0) {
            break;
        }
        r = func_005C1060(p);
        if (func_001AF500() == 1 && (m->pad & 0x4000)) {
            if (r != 0) {
                func_006AE420(4, D_0048BF60.unk1D);
            }
            break;
        }
        if (r != 1) {
            break;
        }
        if (m->pad & 0x200) {
            FIELD(&D_0048BF60, s8, 0x38) = 0;
            FIELD(&D_0048BF60, u8, 0x1E) = p->kind + 1;
            memset(D_00389740, 0, sizeof(D_00389740));
            func_006AE420(1, p->kind);
        } else if (m->pad & 0x8000) {
            m->pad &= 0x7FFF;
            FIELD(&D_0048BF60, s8, 0x38) = 0;
            FIELD(&D_0048BF60, u8, 0x1E) = p->kind + 1;
            memset(D_00389740, 0, sizeof(D_00389740));
            func_005C15E0(p);
            func_006AE420(0, p->kind);
            D_00389740[0] = 0;
        }
        break;
    case 1:
        func_006AC3C0();
        func_006AA8A0();
        switch (D_00723F9E) {
        case 0:
            func_006AF010();
            break;
        case 1:
            func_006B3600();
            break;
        case 2:
            func_006B32D0();
            break;
        default:
            if (m->pad & 0x10) {
                func_006AE930();
            } else if (D_00720BE0.pad & 0x400) {
                m->unk33C0 = 1;
            } else if (m->pad & 0x800) {
                m->unk33C0 = 2;
            }
            break;
        }
        break;
    case 2:
        func_006B38E0();
        break;
    case 4:
        func_006B9D20();
        break;
    case 5:
        func_006BA610();
        break;
    }
}
#endif

extern s32 D_0023BFC8;
extern s32 D_0023BFCC;
extern u8 D_007209B0[0xE8];     /* the text window's state (func_006ABD70 shows a text) */
/* (empty) */
void func_006A5A00(void) {
}

/* Clears the text window. */
void func_006ABBA0(void) {
    D_0023BFCC = 0;
    D_0023BFC8 = 0;
    memset(D_007209B0, 0, sizeof(D_007209B0));
    D_00306590.unk2A = 0;
    func_006A5A00();
    D_00306590.unk3E = 0;
}
