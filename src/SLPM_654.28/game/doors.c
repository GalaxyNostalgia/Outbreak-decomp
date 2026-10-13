#include "common.h"
#include "options.h"

/* The room's objects (doors and such): types from obj%03d.dat, placements from r%03d.sto. */

typedef struct {
    s8 active;          /* 0x00 */
    s8 unk1;
    s8 unk2;
    s8 index;           /* 0x03 */
    s16 type;           /* 0x04 */
    u8 unk6[0x14 - 6];
    s16 unk14;          /* 0x14 */
    s8 unk16;
    s8 unk17;
    s8 unk18;
    s8 unk19;
    s8 unk1A;           /* 0x1A */
    u8 unk1B;
    s8 unk1C;           /* 0x1C */
    s8 unk1D;
    s8 unk1E;
    u8 unk1F[3];        /* 0x1F: (unk1F[1] 7: ...) */
    u8 unk22;           /* 0x22 */
    u8 unk23;
    s32 flags;          /* 0x24 */
    s32 unk28[5];       /* 0x28 */
} RoomObj;

typedef struct {
    s8 model;           /* 0x00: D_006EF480's entry */
    s8 unk1;
    u8 unk2[2];
    u8 unk4;
    u8 unk5;
    u16 flags;          /* 0x06 */
} RoomObjType;

typedef struct {
    u8 unk0[0x10];
    u8 color[3];        /* 0x10 */
    u8 unk13;
} ObjModelInfo;

typedef struct {
    u8 a;
    u8 b;
    s8 type;            /* 0x02 */
    s8 c;
    s8 d;
    u8 e;
    u8 f;
    u8 color[3];        /* 0x07 */
    u8 pad[2];
    s32 flags;          /* 0x0C: 4: its own color, type 0x1F */
} RoomObjSpot;

extern RoomObj D_00395720[0x20];
extern u8 D_00395EA0[0x930];
extern s8 D_0023BF54;           /* objects placed */
extern RoomObjType D_00716000[0x20];
extern s16 D_00715FC0[0x20];
extern s16 D_0023BF68;          /* object types */
extern s8 D_0023BF50;
extern s8 D_0023BF4C;
extern s8 D_0023BF44;
extern ObjModelInfo D_006EF480[];
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *dst, const void *src, u32 n);

void func_0067ACB0(void) {
    memset(D_00395720, 0, sizeof(D_00395720));
    memset(D_00395EA0, 0, sizeof(D_00395EA0));
    D_0023BF54 = 0;
    memset(D_00716000, 0, sizeof(D_00716000));
    memset(D_00715FC0, -1, sizeof(D_00715FC0));
    D_0023BF68 = 0;
    D_0023BF50 = 0;
    D_0023BF4C = 0;
    D_0023BF44 = 0;
}

RoomObj *func_0067AD30(s8 a, s8 b, s16 type);
#ifdef NON_MATCHING   /* first draft */
RoomObj *func_0067AD30(s8 a, s8 b, s16 type) {
    RoomObj *o;
    s8 i;

    if (D_0023BF54 >= 0x1F) {
        return NULL;
    }
    for (i = 1; i < 0x20; i++) {
        o = &D_00395720[i];
        if (o->active == 0) {
            memset(o, 0, sizeof(RoomObj));
            o->active = 1;
            o->unk1 = a;
            o->unk2 = b;
            o->index = i;
            o->type = type;
            D_0023BF54++;
            return o;
        }
    }
    return NULL;
}
#endif

RoomObj *func_0067AE00(RoomObjSpot *s);
#ifdef NON_MATCHING   /* first draft */
RoomObj *func_0067AE00(RoomObjSpot *s) {
    RoomObj *o;
    u16 type;

    if (s->flags & 4) {
        type = 0x1F;
    } else {
        type = s->type;
    }
    o = func_0067AD30(s->a, s->b, type);
    if (o == NULL) {
        return NULL;
    }
    o->unk18 = o->unk16 = s->c;
    o->unk19 = o->unk17 = s->d;
    o->unk14 = s->e & 3;
    o->unk1A = s->f & 0xF;
    o->flags = s->flags | 1;
    if (s->flags & 4) {
        memcpy(o->unk1F, s->color, 3);
    } else {
        memcpy(o->unk1F, D_006EF480[D_00716000[type].model].color, 3);
    }
    if (D_00716000[type].unk1 == 0) {
        o->unk1F[1] = 7;
    }
    o->flags |= D_00716000[type].flags;
    o->unk22 = D_00716000[type].unk4;
    return o;
}
#endif

extern char D_0070BE60[];   /* "%s/r%03d.sto" */
extern char D_0070BE70[];   /* "data/rom/scene/obj_set" */
extern char D_0070BE88[];   /* "r%03d.sto" */
extern char D_0070BEA0[];   /* "data/rom/scene/obj_set/obj%03d.dat" */
extern char D_0070BEC8[];   /* "obj%03d.dat" */
s32 func_0018D530(const char *name);
s32 func_0018D2A0(const char *name, void *buf, s32 size);
s32 func_001A96E0(const char *name, void *buf);
s32 sprintf(char *dst, const char *fmt, ...);

/* Placements: {2, n, n spots of 0x10}. */
void func_0067B310(void);
#ifdef NON_MATCHING   /* first draft */
void func_0067B310(void) {
    char name[0x40];
    s32 *buf = (s32 *)D_00306590.unk4C;
    RoomObjSpot *spot;
    s32 size;
    s32 n;
    s32 i;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_0070BE60, D_0070BE70, D_00306590.unk1A);
        size = func_0018D530(name);
        if (size == 0) {
            return;
        }
        if (func_0018D2A0(name, buf, size) == 0) {
            return;
        }
    } else {
        sprintf(name, D_0070BE88, D_00306590.unk1A);
        if (func_001A96E0(name, buf) == 0) {
            return;
        }
    }
    n = buf[1];
    spot = (RoomObjSpot *)(buf + 2);
    if (buf[0] == 2) {
        if (n == 0) {
            return;
        }
        for (i = 0; i < n; i++, spot++) {
            func_0067AE00(spot);
        }
    }
}
#endif

/* Types: {?, n, offset of n types, offset of n s16}. */
void func_0067B420(u16 room);
#ifdef NON_MATCHING   /* first draft */
void func_0067B420(u16 room) {
    char name[0x40];
    s32 *buf = (s32 *)D_00306590.unk4C;
    s32 size;
    s32 n;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_0070BEA0, room);
        size = func_0018D530(name);
        if (size == 0) {
            return;
        }
        if (func_0018D2A0(name, buf, size) == 0) {
            return;
        }
    } else {
        sprintf(name, D_0070BEC8, room);
        if (func_001A96E0(name, buf) == 0) {
            return;
        }
    }
    n = buf[1];
    D_0023BF68 = n;
    if (buf[2] != 0) {
        memcpy(D_00716000, (u8 *)buf + buf[2], n * 8);
    }
    if (buf[3] != 0) {
        memcpy(D_00715FC0, (u8 *)buf + buf[3], n * 2);
    }
}
#endif

extern s8 D_0023BF50;           /* objects are on */
extern s8 D_0023BF4C;           /* objects placed */
void func_005E8600(void);
void func_0067B7A0(RoomObj *o);
void func_0067CB00(RoomObj *o);

/* The room's objects when the room starts. */
void func_0067B1F0(void) {
    s32 i;
    RoomObj *o;

    func_005E8600();
    for (i = 0, o = D_00395720; i < 0x20; i++, o++) {
        if (o->active == 0) {
            continue;
        }
        o->unk28[0] = 0;
        o->unk28[1] = 0;
        o->unk28[2] = 0;
        o->unk28[3] = 0;
        o->unk28[4] = 0;
        o->flags &= ~2;
        o->unk1C = 0;
        o->unk1D = 0;
        o->unk1E = 0;
        if (D_0023BF50 != 0 && (o->flags & 1) && (u8)o->unk1 == D_00306590.unk1C) {
            func_0067B7A0(o);
            func_0067CB00(o);
            D_0023BF4C++;
        }
    }
}
