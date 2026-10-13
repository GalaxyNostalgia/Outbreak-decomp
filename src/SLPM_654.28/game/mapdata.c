#include "common.h"
#include "options.h"
#include "game.h"

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
s32 func_006A6710(void *p, s32 a);
s32 func_006A5BA0(s32 which, s32 a);

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
            D_0023BFB4 = func_006A6710(D_0023BFB8, -1);
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
    u8 open;            /* 0x04: 1 the map, 2 ... */
} MapScreen;

extern MapScreen D_00720BE0;
extern s8 D_00723F94;
extern u8 *D_0023BFBC;
extern u8 D_00476DD0[][0x10E0];
void func_0064E360(void *p, s32 a, u8 b);
void func_006B9910(void);
void func_001B6EB0(void);
void func_006B9610(void);
void func_006A5A10(s32 a);

/* Closes the map screen. */
void func_006AE930(void);
#ifdef NON_MATCHING   /* first draft */
void func_006AE930(void) {
    u8 *p = D_00476DD0[D_0048BF60.unk1D];

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
