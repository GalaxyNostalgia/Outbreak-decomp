#include "common.h"
#include "item.h"

/* Item lookups by slot. */

s32 func_00583EA0(u8 i, s32 mask) {
    ItemSlot *e = &D_003967D0[i];

    if (e->active == 0) {
        return 0;
    }
    if (e->type == 0xFF) {
        return 0;
    }
    return mask & D_00716980[e->type].flags;
}

s8 func_00584790(u8 i) {
    ItemSlot *e = &D_003967D0[i];

    if (e->active == 0) {
        return -1;
    }
    if (e->type == 0xFF) {
        return -1;
    }
    return D_00716980[e->type].weapon;
}

s8 func_00584800(u8 i) {
    ItemSlot *e = &D_003967D0[i];

    if (e->active == 0) {
        return 0;
    }
    if (e->type == 0xFF) {
        return 0;
    }
    return D_00716980[e->type].unkF;
}

extern s8 D_00716510[600];

/* By item id (ids from 10000 are id % 100 + 500). */
s8 func_00583D60(s16 id) {
    if (id >= 10000) {
        id = id % 100 + 500;
    }
    if (id < 0 || id >= 600) {
        return 0;
    }
    return D_00716510[id];
}

extern ItemInfo D_00717580;     /* (for no type) */

/* The type info of the item in slot i. */
ItemInfo *func_00583E30(u8 i) {
    ItemSlot *e = &D_003967D0[i];

    if (e->active == 0 || e->type == 0xFF) {
        return &D_00717580;
    }
    return &D_00716980[e->type];
}

#include "object.h"
#include "options.h"

s32 func_00584440(DrawObjFull *o, u8 slot);
void func_00585370(u8 slot, s32 player, s32 a);

/* (Room 0x29) the timed items (id 0x3717) o has on it count down (0xC84; 0xC88 too for
   character 6); one at 0 that func_00584440 takes goes through func_00585370. 1 when one did. */
s32 func_00585EF0(DrawObjFull *o) {
    s32 i;
    s32 r = 0;
    ItemSlot *it;
    u8 *p;
    u8 s;

    if (o == NULL) {
        return 0;
    }
    if (D_00306590.unk1A != 0x29) {
        return 0;
    }
    for (i = 0; i < 4; i++) {
        p = &o->unkC84[i];
        if (*p != 0) {
            it = &D_003967D0[*p];
            if (it->unk6 == 0x3717) {
                if (it->unk24 >= 0) {
                    it->unk24--;
                }
                if (it->unk24 == 0) {
                    s = *p;
                    if ((s8)func_00584440(o, s)) {
                        func_00585370(s, o->kind, 5);
                        r = 1;
                    }
                }
            }
        }
    }
    if (o->unkB98 == 6) {
        for (i = 0; i < 0x20; i++) {
            p = &o->unkC88[i];
            if (*p != 0) {
                it = &D_003967D0[*p];
                if (it->unk6 == 0x3717) {
                    if (it->unk24 >= 0) {
                        it->unk24--;
                    }
                    if (it->unk24 == 0) {
                        s = *p;
                        if ((s8)func_00584440(o, s)) {
                            func_00585370(s, o->kind, 5);
                            r = 1;
                        }
                    }
                }
            }
        }
    }
    return r;
}

void func_00561160(void *obj, u16 id);
s16 func_00584910(u8 i);

/* Slot e now holds item id: its type, and its thing in the room's look. */
void func_00584740(ItemSlot *e, s16 id) {
    e->unk6 = id;
    e->type = func_00583D60(id);
    if (e->obj != NULL) {
        func_00561160(e->obj, id);
    }
}

/* Turns the carried item in slot i into item id (with its count, func_00584910); 1 if done. */
s32 func_005846A0(u8 i, s16 id) {
    ItemSlot *e = &D_003967D0[i];
    s32 r = 0;

    if (e->active == 0) {
        return 0;
    }
    if (e->unk6 == id) {
        return 0;
    }
    if ((u16)e->owner != 0) {
        func_00584740(e, id);
        e->unk24 = func_00584910(i);
        r = 1;
    }
    return r;
}

/* A full stack of the item in slot i (item 0x3717: 1200, or 600 with option unkC). */
s16 func_00584910(u8 i) {
    ItemSlot *e = &D_003967D0[i];

    if (e->active == 0) {
        return 0;
    }
    if (e->type == 0xFF) {
        return 0;
    }
    if (e->unk6 == 0x3717) {
        return D_00306590.unkC != 0 ? 0x258 : 0x4B0;
    }
    return D_00716980[e->type].max;
}

s32 func_001AF540(DrawObjFull *o);
void func_0034B7D0(s32 a, s32 b, s32 c);

#ifdef NON_MATCHING   /* the original's jump table is indexed with addi (see CLAUDE.md) */
/* After combining (kind, giving item arg), the file notes it unlocks for o's character
   (func_0034B7D0): character 3's (kinds 15/16) and 4's (17-19), anyone's 0x19. */
void func_005861E0(s16 kind, s16 arg, s16 x, DrawObjFull *o) {
    u8 c;

    if (func_001AF540(o) == 0) {
        return;
    }
    c = o->unkB98;
    if (c == 3 && (kind == 15 || kind == 16)) {
        switch (arg) {
        case 0x136:
            func_0034B7D0(0x13, (u16)x, 0);
            return;
        case 0x13A:
            func_0034B7D0(0x14, (u16)x, 0);
            return;
        case 0x137:
            func_0034B7D0(0x15, (u16)x, 0);
            return;
        case 0x13B:
            func_0034B7D0(0x16, (u16)x, 0);
            return;
        case 0x13C:
            func_0034B7D0(0x17, (u16)x, 0);
            return;
        case 0x13D:
            func_0034B7D0(0x18, (u16)x, 0);
            return;
        }
        return;
    }
    if (c == 4 && ((u32)(kind - 0x11) < 2 || kind == 0x13)) {
        switch (arg) {
        case 0x71:
            func_0034B7D0(0x1A, 0, 0);
            return;
        case 0x1D:
            func_0034B7D0(0x1B, 0, 0);
            return;
        case 0x73:
            func_0034B7D0(0x1C, 0, 0);
            return;
        case 0:
        case 1:
            func_0034B7D0(0x1D, 0, 0);
            return;
        case 0xB:
            func_0034B7D0(0x1E, 0, 0);
            return;
        }
        return;
    }
    if ((kind == 9 || kind == 10) && arg == 0x72) {
        func_0034B7D0(0x19, 0, 0);
    }
}
#endif
