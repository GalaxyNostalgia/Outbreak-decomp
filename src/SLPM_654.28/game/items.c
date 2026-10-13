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
