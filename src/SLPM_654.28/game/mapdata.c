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
extern u8 D_00720AA0[0xD4];
/* A floor record of the map file (0x5C bytes). */
typedef struct {
    u8 unk0[0x40];
    u8 level;           /* 0x40: its level (func_00540A10 says whether it is shown) */
    u8 unk41[0x5C - 0x41];
} MapFloor;

extern MapFloor *D_00720B18[];  /* the floor records */
void func_003478F0(void);
void *memset(void *p, s32 c, u32 n);
void func_006A69F0(s32 a, u8 *p, u16 b, MapView *m);
s32 func_00540A10(u8 level);
s32 func_006A8860(u8 room, s32 y);
void func_006A8970(u8 *p);
f32 func_006A89C0(u8 floor);
s32 func_006A5A30(void);

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
    memset(D_00720AA0, 0, 0xD4);
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
    r = D_00720B18[0];
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
    m->unk37 = D_00720B18[0][m->floor].level;
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
