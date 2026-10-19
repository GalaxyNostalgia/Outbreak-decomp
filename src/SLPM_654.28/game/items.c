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
