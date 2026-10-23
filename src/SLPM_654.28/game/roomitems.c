#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"
#include "item.h"
#include "model.h"

/* The room's items: placements (item%03d%02d.tbl, by difficulty and a random pick), the item
   types and spots (item%03d.dat) and what the NPCs carry. */

extern s32 D_0023BE5C;          /* items placed */
extern s16 D_0023BF6C;          /* item types */
extern u8 D_007177A0[0x300];
extern u8 D_007175A0[0x200];
extern u8 D_00716770[0x204];
extern s8 D_00716510[600];
extern u8 D_0048BDE2[0x20];

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} Unk716100;

extern Unk716100 D_00716100[0x81];
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *dst, const void *src, u32 n);

void func_00561640(void) {
    s32 i;

    memset(D_003967D0, 0, sizeof(D_003967D0));
    D_0023BE5C = 0;
    D_0023BF6C = 0;
    memset(D_007177A0, 0, sizeof(D_007177A0));
    memset(D_007175A0, 0, sizeof(D_007175A0));
    memset(D_00716980, 0, 0xC18);
    memset(D_00716770, 0, sizeof(D_00716770));
    memset(D_00716510, 0xFF, sizeof(D_00716510));
    for (i = 0; i < 0x81; i++) {
        D_00716100[i].unk0 = 0x32;
        D_00716100[i].unk4 = 0;
        D_00716100[i].unk2 = 0;
        D_00716100[i].unk6 = 0;
    }
    memset(D_0048BDE2, 0, sizeof(D_0048BDE2));
}

s8 func_00583D60(s16 id);

/* A free item slot (not 0), set up for item id at place. */
ItemSlot *func_00561720(s8 a, s16 place, s16 id);
#ifdef NON_MATCHING   /* first draft */
ItemSlot *func_00561720(s8 a, s16 place, s16 id) {
    ItemSlot *found = NULL;
    ItemSlot *e;
    s8 i;

    if (D_0023BE5C >= 0xFF) {
        return NULL;
    }
    for (i = 1; i < 0x100; i++) {
        e = &D_003967D0[i];
        if (e->active == 0) {
            memset(e, 0, sizeof(ItemSlot));
            e->active = 1;
            e->unk1 = a;
            e->place = place;
            e->index = i;
            e->unk6 = id;
            e->unk3 = 1;
            found = e;
            e->type = func_00583D60(e->unk6);
            D_0023BE5C++;
            break;
        }
    }
    return found;
}
#endif

extern char D_00709810[];   /* "%s/item%03d%02d.tbl" */
extern char D_00709830[];   /* "data/rom/scene/item_set" */
extern char D_00709850[];   /* "item%03d%02d.tbl" */
u16 func_001ADEF0(s32 n);
s32 func_0018D530(const char *name);
s32 func_0018D2A0(const char *name, void *buf, s32 size);
s32 func_001A96E0(const char *name, void *buf);
s32 sprintf(char *dst, const char *fmt, ...);

/* Placements, online: per spot three picks {id, count, weight %}. */
s32 func_00561970(u16 room);
#ifdef NON_MATCHING   /* first draft */
s32 func_00561970(u16 room) {
    char name[0x40];
    s32 table = D_0048BF60.unkBA * 4 + (func_001ADEF0(2) & 3);
    s32 *buf = (s32 *)D_00306590.unk4C;
    s32 r = func_001ADEF0(0) % 100;
    s32 n;
    s32 size;
    s32 i;
    s32 k;
    s32 left;
    u8 *spot;
    u8 *pick;
    ItemSlot *e;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00709810, D_00709830, room);
        size = func_0018D530(name);
        if (size == 0) {
            return 0;
        }
        if (func_0018D2A0(name, buf, size) == 0) {
            return 0;
        }
    } else {
        sprintf(name, D_00709850, room, table);
        if (func_001A96E0(name, buf) == 0) {
            return 0;
        }
    }
    n = buf[1];
    if (buf[0] != 4 || n == 0) {
        return 0;
    }
    spot = (u8 *)(buf + 2);
    for (i = 0; i < n; i++, spot += 0xC) {
        left = r;
        pick = spot;
        for (k = 0; k < 3; k++, pick += 4) {
            left -= pick[3];
            if (left < 0) {
                if (*(u16 *)pick != 0xFFFF) {
                    e = func_00561720(0, i + 1, 0);
                    if (e != NULL) {
                        e->unk6 = *(u16 *)pick;
                        e->unk24 = (s8)pick[2];
                        e->flags = 0x800;
                        if (e->unk6 == 0x3717) {
                            e->unk24 = D_00306590.unkC == 0 ? 1200 : 600;
                        }
                    }
                }
                break;
            }
        }
    }
    return 1;
}
#endif

/* Placements, offline: one {id, count} per spot (table 0x10 + difficulty). */
s32 func_00561B60(u16 room);
#ifdef NON_MATCHING   /* first draft */
s32 func_00561B60(u16 room) {
    char name[0x20];
    s32 *buf = (s32 *)D_00306590.unk4C;
    s32 n;
    s32 size;
    s32 i;
    u8 *spot;
    ItemSlot *e;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00709810, D_00709830, room);
        size = func_0018D530(name);
        if (size == 0) {
            return 0;
        }
        if (func_0018D2A0(name, buf, size) == 0) {
            return 0;
        }
    } else {
        sprintf(name, D_00709850, room, D_0048BF60.unkBA + 0x10);
        if (func_001A96E0(name, buf) == 0) {
            return 0;
        }
    }
    n = buf[1];
    if (buf[0] != 4 || n == 0) {
        return 0;
    }
    spot = (u8 *)(buf + 2);
    for (i = 0; i < n; i++, spot += 4) {
        if (*(u16 *)spot != 0xFFFF) {
            e = func_00561720(0, i + 1, 0);
            if (e != NULL) {
                e->unk6 = *(u16 *)spot;
                e->unk24 = (s8)spot[2];
                e->flags = 0x800;
                if (e->unk6 == 0x3717) {
                    e->unk24 = D_00306590.unkC == 0 ? 1200 : 600;
                }
            }
        }
    }
    return 1;
}
#endif

s32 func_00561DF0(s32 *p);
s32 func_00561F90(s32 *p);
s32 func_005620C0(s32 *p);
extern char D_00709870[];   /* "%s/item%03d.dat" */
extern char D_00709880[];   /* "item%03d.dat" */

/* The room's item file: {types, spots, D_00716100}, each {offset, count}. */
void func_00561CE0(u16 room);
#ifdef NON_MATCHING   /* first draft */
void func_00561CE0(u16 room) {
    char name[0x40];
    u8 *buf = (u8 *)D_00306590.unk4C;
    s32 *f;
    s32 size;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00709870, D_00709830, room);
        size = func_0018D530(name);
        if (size == 0) {
            return;
        }
        if (func_0018D2A0(name, buf, size) == 0) {
            return;
        }
    } else {
        sprintf(name, D_00709880, room);
        if (func_001A96E0(name, buf) == 0) {
            return;
        }
    }
    f = (s32 *)D_00306590.unk60;
    if (f[0] != 0 && f[1] != 0) {
        func_00561DF0((s32 *)((u8 *)f + f[0]));
    }
    if (f[2] != 0 && f[3] != 0) {
        func_00561F90((s32 *)(D_00306590.unk60 + f[2]));
    }
    if (f[4] != 0 && f[5] != 0) {
        func_005620C0((s32 *)(D_00306590.unk60 + f[4]));
    }
}
#endif

/* Item types: {3, n, m, k, offsets of n * 0x18 (D_00716980), n * 4, k * 4, m * 2}. */
#ifdef NON_MATCHING   /* first draft */
s32 func_00561DF0(s32 *p) {
    s32 n = p[1];
    s32 m = p[2];
    s32 k = p[3];
    s32 *offs = p + 4;
    s16 id;
    s8 i;

    if (p[0] != 3 || n == 0) {
        return 0;
    }
    D_0023BF6C = n;
    if (offs[0] != 0 && n > 0) {
        memcpy(D_00716980, (u8 *)p + offs[0], n * 0x18);
    }
    if (offs[1] != 0 && n > 0) {
        memcpy(D_00716770, (u8 *)p + offs[1], n * 4);
    }
    if (offs[2] != 0 && k > 0) {
        memcpy(D_007175A0, (u8 *)p + offs[2], k * 4);
    }
    if (offs[3] != 0 && m > 0) {
        memcpy(D_007177A0, (u8 *)p + offs[3], m * 2);
    }
    for (i = 0; i < n; i++) {
        id = D_00716980[i].id;
        if (D_00716980[i].id >= 10000) {
            id = D_00716980[i].id % 100 + 500;
        }
        if (id >= 0 && id < 600) {
            D_00716510[id] = i;
        }
    }
    return 1;
}
#endif

/* Item spots: {3, n, n * 0x24 byte spots}, matched to the placed items by place. */
typedef struct {
    u16 unk0;
    u16 place;          /* 0x02 */
    f32 pos[3];         /* 0x04 */
    u16 unk10;
    u16 unk12;
    u8 unk14;
    u8 unk15;
    u16 unk16;
    f32 unk18[3];       /* 0x18 */
} ItemSpot;

#ifdef NON_MATCHING   /* first draft */
s32 func_00561F90(s32 *p) {
    s32 n = p[1];
    ItemSpot *s = (ItemSpot *)(p + 2);
    ItemSlot *e;
    s32 i;
    s32 k;

    if (p[0] != 3 || n == 0) {
        return 0;
    }
    for (i = 0; i < n; i++, s++) {
        for (k = 0, e = D_003967D0; k < 0x100; k++, e++) {
            if (e->active != 0 && e->place == s->place) {
                e->unk1 = s->unk0;
                e->pos[0] = s->pos[0];
                e->pos[1] = s->pos[1];
                e->pos[2] = s->pos[2];
                e->unk14 = s->unk10;
                e->unk16 = s->unk12;
                e->unk2C = s->unk14;
                e->unk2D = s->unk15;
                e->unk2E = s->unk16;
                e->unk30[0] = s->unk18[0];
                e->unk30[1] = s->unk18[1];
                e->unk30[2] = s->unk18[2];
                e->flags |= 1;
                e->type = func_00583D60(e->unk6);
                break;
            }
        }
    }
    return 1;
}
#endif

s32 func_005620C0(s32 *p) {
    s32 n = p[0];

    if (n <= 0) {
        return 0;
    }
    memcpy(D_00716100, p + 1, n * 8);
    return 1;
}

/* What the NPCs carry (D_006EF550 per NPC model: {item, count}). */
typedef struct {
    u16 item;
    u16 count;
} NpcItem;

extern NpcItem D_006EF550[];
extern u32 D_0023BE14;
extern DrawObjFull D_00476DD0[];
s32 func_005C1C50(DrawObjFull *obj);
void func_005C1A60(DrawObjFull *obj, s8 slot);

void func_00562100(void);
#ifdef NON_MATCHING   /* first draft */
void func_00562100(void) {
    DrawObjFull *obj;
    NpcItem *ni;
    ItemSlot *e;
    s32 i;
    u16 item;
    u16 count;

    for (i = 0, obj = D_00476DD0; i < (s32)D_0023BE14; i++, obj++) {
        if (obj->unkBB4 == 0 || obj->unkBB4 >= 0x99) {
            continue;
        }
        if (func_005C1C50(obj) == 0) {
            continue;
        }
        ni = &D_006EF550[(u16)(obj->unkBB4 - 1)];
        item = ni->item;
        count = ni->count;
        if (item == 0xFFFF || count == 0) {
            continue;
        }
        e = func_00561720(0, 0, item);
        if (e != NULL) {
            e->flags = 1;
            e->unk3 = 0;
            e->unk24 = count;
            e->owner = i + 1;
            func_005C1A60(obj, e->index);
        }
    }
}
#endif

void func_00560880(void);
extern ItemObj D_003C51B0[0x40];
extern ItemObj *D_00386950[0x40];   /* the free ones */
extern s16 D_0023BDB0;              /* how many */
extern ItemObj *D_0023BDB4;         /* the list's first */
extern ItemObj *D_0023BDB8;         /* its last */

/* A new (cleared) item thing, put last in the list (first when at != 0); NULL when none is free. */
ItemObj *func_00560900(s32 at) {
    ItemObj *o;

    if (D_0023BDB0 == 0) {
        return NULL;
    }
    D_0023BDB0--;
    o = D_00386950[D_0023BDB0];
    memset(o, 0, sizeof(ItemObj));
    o->slot = (u32)((u8 *)o - (u8 *)D_003C51B0) / sizeof(ItemObj);
    if (D_0023BDB8 == NULL) {
        D_0023BDB4 = o;
        D_0023BDB8 = o;
        o->next = NULL;
        o->prev = NULL;
    } else if (at == 0) {
        o->prev = D_0023BDB8;
        o->next = NULL;
        D_0023BDB8->next = o;
        D_0023BDB8 = o;
    } else {
        o->prev = NULL;
        o->next = D_0023BDB4;
        D_0023BDB4->prev = o;
        D_0023BDB4 = o;
    }
    return o;
}

/* Gives an item thing back (0 when the pool is full). */
s32 func_005609F0(ItemObj *o) {
    if (D_0023BDB0 == 0x40) {
        return 0;
    }
    D_00386950[D_0023BDB0] = o;
    D_0023BDB0++;
    if (D_0023BDB8 == o) {
        if (o->prev == NULL) {
            D_0023BDB4 = NULL;
            D_0023BDB8 = NULL;
        } else {
            o->prev->next = NULL;
            D_0023BDB8 = o->prev;
        }
        memset(o, 0, sizeof(ItemObj));
        return 1;
    }
    if (D_0023BDB4 == o) {
        if (o->next == NULL) {
            D_0023BDB4 = NULL;
            D_0023BDB8 = NULL;
        } else {
            o->next->prev = NULL;
            D_0023BDB4 = o->next;
        }
        memset(o, 0, sizeof(ItemObj));
        return 1;
    }
    o->next->prev = o->prev;
    o->prev->next = o->next;
    memset(o, 0, sizeof(ItemObj));
    return 1;
}

extern RoomObjModel D_0039A3D0;
extern void *D_006D5C30[];
extern u8 D_00341EB0[];
s16 func_00342450(s16 id);
void func_0018F7C0(f32 *m);
void func_005AEA90(ItemObj *o, s32 a);

/* A new thing for item id. */
ItemObj *func_00560E80(s32 id, s32 a) {
    u8 kind = 0;
    ItemObj *o;
    u8 type;
    s32 i;
    s16 sid;

    if ((o = func_00560900(1)) == NULL) {
        return NULL;
    }
    sid = id;
    type = func_00583D60(sid);
    if (type != 0xFF) {
        kind = D_00716980[type].unk14;
    }
    o->active = 1;
    o->unk2 = 8;
    o->id = id;
    o->unk4 = 0;
    o->draw = D_00341EB0;
    o->unk238 = 0xFF;
    o->update = D_006D5C30[kind];
    o->model = func_00342450(sid);
    if (o->model >= 0) {
        o->unk1 = 1;
        for (i = 0; i < D_0039A3D0.count[o->model]; i++) {
            o->partOn[i] = 1;
            o->parts[i] = D_0039A3D0.world[o->model][i];
        }
    } else {
        o->unk1 = 0;
    }
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
    func_005AEA90(o, a);
    return o;
}

void *func_00561270(ItemSlot *e);

/* The room's item things again (from the slots), and the items lying in this scenario. */
void func_00561570(void) {
    s32 i;
    ItemSlot *e;
    u8 *o;

    func_00560880();
    for (i = 0, e = D_003967D0; i < 0x100; i++, e++) {
        if (e->active == 0) {
            continue;
        }
        e->obj = NULL;
        if (e->flags & 0x20000) {
            e->obj = func_00560E80(e->unk6, 0);
            if ((o = e->obj) != NULL) {
                *(s32 *)(o + 0x23C) = e->flags;
                ((u8 *)e->obj)[0x240] = (u8)e->index;
            }
        }
        e->unk20 = 0;
        if ((e->flags & 1) && (u16)e->owner == 0 && D_00306590.unk1C == (u8)e->unk1) {
            func_00561270(e);
        }
    }
}


typedef struct {
    f32 v[3];
} ItemVec3;

typedef struct {
    u16 v;
    u8 unk2[2];
} Unk6EF3A0;

extern Unk6EF3A0 D_006EF3A0[][7];
ItemInfo *func_00583E30(u8 i);
f32 func_005E5B80(const f32 *p, u8 *sub, s32 want);
u16 func_001ADEF0(s32 n);
void func_005614B0(ItemSlot *e);

/* Makes the thing of the item lying in slot e (again): placed, raised by its type or with its
   own pose (D_006EF3A0), and its collision. */
void *func_00561270(ItemSlot *e) {
    s8 posed = 0;
    ItemObj *o;
    s8 col;
    s8 k;
    u16 v;

    if (e->obj != NULL) {
        func_005609F0(e->obj);
        e->obj = NULL;
    }
    o = func_00560E80(e->unk6, 0);
    if (o == NULL) {
        return NULL;
    }
    *(ItemVec3 *)o->pos = *(ItemVec3 *)e->pos;
    o->rot[2] = e->unk14;
    o->rot[1] = e->unk16;
    o->flags = e->flags;
    o->index = e->index;
    e->obj = o;
    if (e->unk6 == 0x3200) {
        func_005E5B80(o->pos, o->unk241, 0xFF);
    } else if ((u16)e->owner == 0) {
        o->flags |= 0x80000;
        col = func_00583E30(e->index)->unkC;
        k = func_00583E30(e->index)->unkD;
        if (e->unk3 == 0) {
            v = D_006EF3A0[(s8)e->unk2D][col].v;
            if (v != 0) {
                o->rot[2] = v;
                posed = 1;
                o->unk218 = ((f32 *)D_007175A0)[k];
            }
        }
        if (posed == 0) {
            o->pos[1] += ((f32 *)D_00716770)[e->type];
            o->unk218 = 0.0f;
        }
        if (e->unk3 == 0) {
            e->flags &= ~0x200000;
            o->flags &= ~0x200000;
        }
        if (!(e->flags & 0x200000)) {
            func_005614B0(e);
        }
        if (o->flags & 0x800) {
            o->unkE = (s16)(func_001ADEF0(1) & 0x1F) + 0x3C;
            o->unk12 = 0;
        }
        func_005E5B80(o->pos, o->unk241, 0xFF);
    }
    return o;
}

/* Per item shape (D_006EF080, 0x28 bytes): its footprint's corners (x, z) and its top and
   bottom. */
typedef struct {
    s32 corner[4][2];
    s32 top;            /* 0x20 */
    s32 bottom;         /* 0x24 */
} ItemShape;

extern ItemShape D_006EF080[];
void func_00340310(s32 rx, s32 ry, s32 rz, f32 *m);
void func_00191410(f32 *v, const f32 *m);
void func_00590CC0(s32 h);
s32 func_00595CD0(s32 index, s32 (*corners)[2], s32 top, s32 bottom);

/* Item e's footprint corners in the room (its shape turned by its heading, at its place). */
void func_00562410(ItemSlot *e, s8 shape, s32 (*out)[2]);
#ifdef NON_MATCHING   /* v[1]/v[2] addresses kept in s registers (the usual unsolved pattern) */
void func_00562410(ItemSlot *e, s8 shape, s32 (*out)[2]) {
    f32 m[16];
    f32 v[3];
    s32 i;

    for (i = 0; i < 4; i++) {
        func_00340310(0, e->unk16, 0, m);
        v[0] = D_006EF080[shape].corner[i][0];
        v[1] = 0.0f;
        v[2] = D_006EF080[shape].corner[i][1];
        func_00191410(v, m);
        out[i][0] = e->pos[0] + v[0];
        out[i][1] = e->pos[2] + v[2];
    }
}
#endif

/* (Re)makes item e's pick-up area. */
void func_005614B0(ItemSlot *e) {
    s32 c[4][2];
    s32 top;
    s32 bottom;
    s32 y;

    if (e == NULL) {
        return;
    }
    func_00562410(e, e->unk2C, c);
    y = e->pos[1];
    top = y + D_006EF080[(s8)e->unk2C].top;
    bottom = y + D_006EF080[(s8)e->unk2C].bottom;
    if (e->unk20 != 0) {
        func_00590CC0(e->unk20);
    }
    e->unk20 = func_00595CD0(e->index, c, top, bottom);
}

/* The slot of the item at place (> 0). */
static ItemSlot *func_00562520(s16 place) {
    ItemSlot *e;
    s32 i;

    if (place <= 0) {
        return NULL;
    }
    for (i = 0, e = D_003967D0; i < 0x100; i++, e++) {
        if (e->place == place) {
            return e;
        }
    }
    return NULL;
}

void func_00581320(u8 i);

/* The functions on the item at a place return the slot only when it's inactive (they fall off
   the end otherwise, as the original does; callers don't use it). */

/* The item at place is taken: no longer placed, its thing and its (unk20) gone. */
ItemSlot *func_00562630(s16 place) {
    ItemSlot *e = func_00562520(place);

    if (e != NULL) {
        if (e->active == 0) {
            return e;
        }
        if (e->unk3 != 0) {
            e->flags &= ~0x200001;
            if (e->obj != NULL || e->unk20 != 0) {
                func_00581320(e->index);
            }
        }
    }
}


void func_00581320(u8 i) {
    ItemSlot *e = &D_003967D0[i];

    if (e->active != 0) {
        if (e->obj != NULL) {
            func_005609F0(e->obj);
            e->obj = NULL;
        }
        if (e->unk20 != 0) {
            func_00590CC0(e->unk20);
            e->unk20 = 0;
        }
    }
}

ItemSlot *func_00561720(s8 a, s16 place, s16 id);

/* A new item id (count of it) that character who holds. */
ItemSlot *func_005626B0(u16 who, s16 id, s16 count) {
    ItemSlot *e = func_00561720(0, 0, id);

    if (e == NULL) {
        return NULL;
    }
    e->flags = 1;
    e->unk3 = 0;
    e->unk24 = count;
    e->owner = who + 1;
    return e;
}

void func_005614B0(ItemSlot *e);

/* The item at place: shown (lying here: its thing made). */
ItemSlot *func_00562720(s16 place) {
    ItemSlot *e = func_00562520(place);

    if (e != NULL) {
        if (e->active == 0) {
            return e;
        }
        if (e->unk3 != 0) {
            e->flags |= 0x200001;
            if (D_00306590.unk1C == (u8)e->unk1 && (u16)e->owner == 0) {
                func_00561270(e);
            }
        }
    }
}

ItemSlot *func_005627B0(s16 place) {
    ItemSlot *e = func_00562520(place);

    if (e != NULL) {
        if (e->active == 0) {
            return e;
        }
        if (e->unk3 != 0) {
            e->flags |= 1;
            e->flags &= ~0x200000;
            if (D_00306590.unk1C == (u8)e->unk1 && (u16)e->owner == 0) {
                func_005614B0(e);
            }
        }
    }
}

/* Whether the item at place has been taken. */
s32 func_00562840(s16 place) {
    ItemSlot *e = func_00562520(place);

    if (e == NULL) {
        return 0;
    }
    if (e->active != 0 && e->unk3 == 0) {
        return 1;
    }
    if (e->active == 0 && (e->flags & 0x20000000)) {
        return 1;
    }
    return 0;
}

ItemSlot *func_00584500(s16 id, u16 *who);
DrawObjFull *func_00651300(s32 i);
void func_005C1430(DrawObjFull *o, u8 slot);
void func_005C15E0(DrawObjFull *o);
void func_00561810(u8 slot);
extern DrawObjFull D_00476DD0[];

/* Item id is taken from whoever holds it (players 0-3, then NPCs). */
void func_005628C0(s16 id) {
    u16 who;
    ItemSlot *e = func_00584500(id, &who);
    DrawObjFull *o;

    if (e != NULL) {
        if (e->obj != NULL) {
            func_00581320(e->index);
        }
        if (who < 4) {
            o = &D_00476DD0[who];
        } else {
            o = func_00651300(who - 4);
        }
        func_005C1430(o, e->index);
        func_005C15E0(o);
        func_00561810(e->index);
    }
}

/* Flag 0x800 (0x400) of the item at place, and of its thing. */
ItemSlot *func_00562970(s16 place, s32 on) {
    ItemSlot *e = func_00562520(place);

    if (e != NULL) {
        if (e->active == 0) {
            return e;
        }
        if (e->unk3 != 0) {
            if (on != 0) {
                e->flags |= 0x800;
                if (e->obj != NULL) {
                    *(s32 *)((u8 *)e->obj + 0x23C) |= 0x800;
                }
            } else {
                e->flags &= ~0x800;
                if (e->obj != NULL) {
                    *(s32 *)((u8 *)e->obj + 0x23C) &= ~0x800;
                }
            }
        }
    }
    return e;
}

ItemSlot *func_00562A10(s16 place, s32 on) {
    ItemSlot *e = func_00562520(place);

    if (e != NULL) {
        if (e->active == 0) {
            return e;
        }
        if (e->unk3 != 0) {
            if (on != 0) {
                e->flags |= 0x400;
                if (e->obj != NULL) {
                    *(s32 *)((u8 *)e->obj + 0x23C) |= 0x400;
                }
            } else {
                e->flags &= ~0x400;
                if (e->obj != NULL) {
                    *(s32 *)((u8 *)e->obj + 0x23C) &= ~0x400;
                }
            }
        }
    }
    return e;
}

void *func_00561050(u16 id, s32 a);

/* The item at place is put (back) here: its thing made if it lies in this room. */
ItemSlot *func_00562AB0(s16 place, s32 a);
#ifdef NON_MATCHING   /* a delay slot left empty */
ItemSlot *func_00562AB0(s16 place, s32 a) {
    ItemSlot *e = func_00562520(place);
    u8 *o;

    if (e != NULL) {
        if (e->active == 0) {
            return e;
        }
        if (e->unk3 != 0) {
            e->flags |= 1;
            if (e->obj == NULL) {
                if (D_00306590.unk1C == (u8)e->unk1 && (u16)e->owner == 0) {
                    e->flags &= ~0x200000;
                    o = func_00561050(e->unk6, a);
                    if (o != NULL) {
                        o[0x240] = e->index;
                        e->obj = o;
                    }
                }
            }
        }
    }
}
#endif

/* Clears every room item's unk2A. */
void func_005811A0(void) {
    s32 i;
    ItemSlot *e;

    for (i = 0, e = D_003967D0; i < 0x100; i++, e++) {
        if (e->active != 0) {
            e->unk2A = 0;
        }
    }
}

/* The item type flags of mask that item thing o's type has. */
u32 func_00562B70(ItemObj *o, u32 mask) {
    u8 t;

    if (o == NULL) {
        return 0;
    }
    t = D_00716510[func_00583D60(o->id)];
    if (t == 0xFF) {
        return 0;
    }
    return mask & D_00716980[t].flags;
}
