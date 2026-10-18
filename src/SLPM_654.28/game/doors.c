#include "common.h"
#include "options.h"
#include "item.h"

/* The room's objects (doors and such): types from obj%03d.dat, placements from r%03d.sto. */

typedef struct {
    s8 active;          /* 0x00 */
    s8 unk1;
    s8 unk2;
    u8 index;           /* 0x03 */
    s16 type;           /* 0x04 */
    u8 unk6[0x14 - 6];
    s16 unk14;          /* 0x14 */
    s8 unk16;
    s8 unk17;
    s8 unk18;
    s8 unk19;
    s8 unk1A;           /* 0x1A */
    s8 unk1B;           /* 0x1B: characters on it */
    s8 unk1C;           /* 0x1C */
    s8 unk1D;
    s8 unk1E;
    s8 unk1F[3];        /* 0x1F: width, the floor it is on (7: any), depth */
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
    s8 size[3];         /* 0x10: width, floor, depth */
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
    s8 size[3];         /* 0x07 */
    u8 pad[2];
    s32 flags;          /* 0x0C: 4: its own size (type 0x1F) */
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
        memcpy(o->unk1F, s->size, 3);
    } else {
        memcpy(o->unk1F, D_006EF480[D_00716000[type].model].size, 3);
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
ItemObj *func_0067B7A0(RoomObj *o);
void func_0067CB00(RoomObj *o);

/* The room objects' updates, by their type's kind. */
typedef struct {
    void *fn[4];
} RoomObjFns;

extern RoomObjFns D_006E57F0;
extern u8 D_00341EB0[];
ItemObj *func_00560900(s32 at);
void func_0018F7C0(f32 *m);

/* A new thing for a room object of type. */
ItemObj *func_0067B6A0(u16 type) {
    RoomObjFns fns = D_006E57F0;
    ItemObj *o;

    if ((o = func_00560900(1)) == NULL) {
        return NULL;
    }
    o->active = 1;
    o->unk1 = 0;
    o->unk2 = 8;
    o->id = type;
    o->unk4 = 2;
    o->update = fns.fn[D_00716000[type].unk2[1]];
    o->draw = D_00341EB0;
    o->model = (u8)D_00715FC0[type];
    o->flags = 0;
    o->pos[2] = 0.0f;
    o->pos[1] = 0.0f;
    o->pos[0] = 0.0f;
    o->rot[2] = 0;
    o->rot[1] = 0;
    o->rot[0] = 0;
    o->scale[2] = 1.0f;
    o->scale[1] = 1.0f;
    o->scale[0] = 1.0f;
    func_0018F7C0(o->mat);
    return o;
}

extern u8 D_00470930[];
u8 func_00551690(u8 model);
void func_005A79E0(u8 k, Mat44 *m);
void func_00340310(s32 rx, s32 ry, s32 rz, f32 *m);
void func_0067AF70(RoomObj *o);
void func_0067BDD0(RoomObj *o);

/* Makes room object o's thing, turned and set up. */
ItemObj *func_0067B7A0(RoomObj *r) {
    ItemObj *o;
    u8 k;

    if (r->flags & 4) {
        return NULL;
    }
    o = func_0067B6A0(r->type);
    if (o == NULL) {
        return NULL;
    }
    r->unk28[0] = (s32)o;
    o->rot[1] = (u16)r->unk14 << 14;
    o->flags = r->flags;
    o->index = r->index;
    if (o->flags & 8) {
        o->unk1 = 0;
        k = func_00551690(o->model);
        D_00470930[k] = 1;
        if (k != 0xFF) {
            func_005A79E0(k, (Mat44 *)o->mat);
        }
    }
    func_00340310(0, o->rot[1], 0, o->mat);
    func_0067AF70(r);
    func_0067BDD0(r);
    return o;
}

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

extern u8 D_00711080[4][0x480]; /* per floor: the walkable grid, 96 x 96 bits (0: blocked) */
extern s8 D_0023AE60[4];        /* the floors */
extern u8 D_003967B0[8][4];
void func_0067C1A0(void);
#ifdef NON_MATCHING
/* (roomsets.c's func_0067B020 is static there; this copy serves the drafts here) */
static s8 *func_0067B020(RoomObj *o) {
    return (s8 *)&D_006EF480[D_00716000[o->type].model] + (o->unk14 & 3) * 4;
}
#endif
s8 func_0067D8B0(s8 x, s8 y);
void *memset(void *p, s32 c, u32 n);

/* Once a frame: the grid the room's solid objects block, per floor. */
void func_0067C6E0(void);
#ifdef NON_MATCHING   /* first draft */
void func_0067C6E0(void) {
    RoomObj *o;
    u32 *grid;
    s8 *r;
    s32 k;
    s32 i;
    s32 x;
    s32 y;
    s32 x1;
    s32 y1;
    s32 j;
    u8 *e;

    if (D_0023BF50 == 0) {
        return;
    }
    func_0067C1A0();
    for (k = 1; k < 4; k++) {
        grid = (u32 *)D_00711080[k];
        memset(grid, 0xFF, 0x480);
        for (i = 0, o = D_00395720; i < 0x20; i++, o++) {
            if (o->active == 0 || !(o->flags & 1) || (u8)o->unk1 != D_00306590.unk1C ||
                D_00716000[o->type].unk1 <= 0 || o->unk1F[1] != D_0023AE60[k]) {
                continue;
            }
            if (o->flags & 4) {
                x = o->unk18;
                y = o->unk19;
                x1 = x + o->unk1F[0];
                y1 = y + o->unk1F[2];
            } else {
                r = func_0067B020(o);
                x = o->unk18 + r[0];
                x1 = o->unk18 + r[2];
                y = o->unk19 + r[1];
                y1 = o->unk19 + r[3];
            }
            for (; x < x1; x++) {
                for (j = y; j < y1; j++) {
                    grid[x * 3 + (j >> 5)] &= ~(1 << (j & 0x1F));
                }
            }
        }
    }
    for (i = 0, e = D_003967B0[0]; i < 8; i++, e += 4) {
        if (e[0] != 0) {
            e[3] = func_0067D8B0(e[1], e[2]);
        }
    }
}
#endif

/* A character moved from the object (area) prev onto now: their counts. */
void func_0067D7C0(s8 now, s8 prev);
#ifdef NON_MATCHING   /* first draft */
void func_0067D7C0(s8 now, s8 prev) {
    RoomObj *o;

    if (now == prev) {
        return;
    }
    if (now >= 0) {
        o = &D_00395720[now & 0x1F];
        if (o != NULL) {
            if (o->active == 0) {
                return;
            }
            if (o->unk1B < D_00716000[o->type].unk1) {
                o->unk1B++;
            }
        }
    }
    if (prev >= 0) {
        o = &D_00395720[prev & 0x1F];
        if (o != NULL) {
            if (o->active == 0) {
                return;
            }
            if (o->unk1B > 0) {
                o->unk1B--;
            }
        }
    }
}
#endif

s32 func_0067E490(s32 *xy);
s8 func_0067D8B0(s8 x, s8 y);

/* The object (area) under pos, when pos is on top of one. */
s32 func_0067DA50(s8 *out, f32 *pos);
#ifdef NON_MATCHING   /* first draft */
s32 func_0067DA50(s8 *out, f32 *pos) {
    s32 xy[2];
    s32 r = 0;
    s8 i;

    if (pos == NULL) {
        return 0;
    }
    if (pos[1] - FIELD(D_00395EA0, f32, 4) < 50.0f) {
        return 0;
    }
    if (func_0067E490(xy) != 0) {
        i = func_0067D8B0(xy[0], xy[1]);
        *out = i;
        if (i >= 0) {
            r = 1;
        }
    }
    return r;
}
#endif

/* How high the object (area) i lifts who stands on it. */
f32 func_0067DAF0(u8 i) {
    RoomObj *o = &D_00395720[i & 0x1F];

    if (o->active == 0) {
        return 0.0f;
    }
    if (o->flags & 4) {
        return 0.0f;
    }
    return 50.0f * o->unk1F[1];
}
