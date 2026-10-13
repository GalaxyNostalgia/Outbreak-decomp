#include "common.h"

/* SUBMAIN: lookups for the select screen. */

extern u8 D_00358EE8[];         /* pairs, 0xFF-terminated */

s32 func_0034B080(s32 x) {
    u8 *p = D_00358EE8;
    s32 v;
    s32 c;

    while ((v = p[0]) != 0xFF) {
        c = p[1];
        p += 2;
        if (c == x) {
            return v;
        }
    }
    return -1;
}

s32 func_0034B0C0(s32 x) {
    u8 *p = D_00358EE8;
    s32 v;
    s32 c;

    while ((v = p[0]) != 0xFF) {
        c = p[1];
        p += 2;
        if ((u8)v == x) {
            return (u8)c;
        }
    }
    return -1;
}

extern u8 D_00301D60[];         /* the system save */
extern u8 D_003023FC[];         /* items there are, per file */

u8 func_0034D610(u8 n);
#ifdef NON_MATCHING   /* first draft */
/* How much of save file n's item list is found, in percent. */
u8 func_0034D610(u8 n) {
    s32 found;
    s32 i;
    s32 total;
    s32 pct;

    total = D_003023FC[n];
    if (total == 0) {
        return 0;
    }
    found = 0;
    for (i = 0; i < D_00301D60[n + 0x69C]; i++) {
        if (((s32 *)(D_00301D60 + n * 8 + 0x51C))[i >> 5] & (1 << (i & 0x1F))) {
            found = (u8)(found + 1);
        }
    }
    pct = (u8)(((u8)found * 100) / total);
    if (pct < 0) {
        pct = 0;
    } else if (pct > 100) {
        pct = 100;
    }
    return pct;
}
#endif
