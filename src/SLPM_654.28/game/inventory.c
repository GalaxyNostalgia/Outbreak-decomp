#include "common.h"
#include "object.h"
#include "item.h"

/* GAME.BIN: the characters' item slots (unkC88). Character 6 has 4 more slots (1-4); character 7
   a pouch of 9 for each of the items 0x12C-0x12E (slots 1, 10, 19). */

void func_005C15E0(DrawObjFull *p);

/* Item slot is added to p's extra slots. */
void func_005C1AC0(DrawObjFull *p, u8 slot) {
    u32 i;
    u16 id;

    if (p->unkB98 == 6) {
        for (i = 0; i < 4; i++) {
            if (p->unkC88[1 + i] == 0) {
                p->unkC88[1 + i] = D_003967D0[slot].index;
                break;
            }
        }
    }
    if (p->unkB98 == 7) {
        id = D_003967D0[slot].unk6;
        if (id >= 0x12C && id < 0x12F) {
            switch (id) {
            case 0x12C:
                for (i = 0; i < 9; i++) {
                    if (p->unkC88[1 + i] == 0) {
                        break;
                    }
                }
                p->unkC88[1 + i] = D_003967D0[slot].index;
                break;
            case 0x12D:
                for (i = 0; i < 9; i++) {
                    if (p->unkC88[10 + i] == 0) {
                        break;
                    }
                }
                p->unkC88[10 + i] = D_003967D0[slot].index;
                break;
            case 0x12E:
                for (i = 0; i < 9; i++) {
                    if (p->unkC88[19 + i] == 0) {
                        break;
                    }
                }
                p->unkC88[19 + i] = D_003967D0[slot].index;
                break;
            }
        }
    }
    func_005C15E0(p);
}

s32 func_00583EA0(u8 slot, s32 mask);
void func_0018D630(void *dst, const void *src, s32 n);
void *memset(void *p, s32 c, u32 n);

/* Tidies p's slots: unkC84 (4: those with flag 0x20000 first, no repeats, packed) and unkC88
   (from 1: flag 0x20000 first, packed; character 7: sorted into her pouches). */
#ifdef NON_MATCHING   /* first draft */
void func_005C15E0(DrawObjFull *p) {
    u8 a[5];
    u8 b[0x21];
    u8 c[0x1D];
    u8 i;
    u8 n;
    u8 j;
    u8 k;
    u8 n1;
    u8 n2;
    u8 n3;

    for (i = 0; i < 5; i++) {
        a[i] = 0;
    }
    n = 0;
    for (i = 0; i < 4; i++) {
        if (p->unkC84[i] != 0 && func_00583EA0(p->unkC84[i], 0x20000) != 0) {
            a[n++] = p->unkC84[i];
        }
    }
    for (i = 0; i < 4; i++) {
        if (p->unkC84[i] != 0 && func_00583EA0(p->unkC84[i], 0x20000) == 0 && n < 4) {
            a[n++] = p->unkC84[i];
        }
    }
    if (n >= 2) {
        for (j = 0; j < n; j++) {
            if (a[j] != 0) {
                for (k = j + 1; k < n; k++) {
                    if (j != k && a[j] == a[k]) {
                        a[k] = 0;
                    }
                }
            }
        }
    }
    k = 0;
    for (j = 0; j < n; j++) {
        if (a[j] != 0) {
            k++;
        } else {
            a[k] = a[j + 1];
            a[j + 1] = 0;
            if (a[k] != 0) {
                k++;
            }
        }
    }
    n = k;
    for (i = 0; i < 4; i++) {
        if (i < n) {
            p->unkC84[i] = a[i];
        } else {
            p->unkC84[i] = 0;
        }
    }

    for (i = 0; i < 0x21; i++) {
        b[i] = 0;
    }
    if (p->unkB98 == 7) {
        memset(c, 0, 0x1D);
        n2 = 0;
        n3 = 0;
        n1 = n2;
        c[0] = p->unkC88[0];
        for (i = 1; i < 0x1D; i++) {
            k = p->unkC88[i];
            if (k != 0) {
                switch (D_003967D0[k].unk6) {
                case 0x12C:
                    c[1 + n1++] = k;
                    break;
                case 0x12D:
                    c[10 + n2++] = k;
                    break;
                case 0x12E:
                    c[19 + n3++] = k;
                    break;
                default:
                    c[0x1C] = k;
                    break;
                }
            }
        }
        func_0018D630(p->unkC88, c, 0x1D);
        return;
    }
    n = 1;
    for (i = 1; i < 0x20; i++) {
        if (p->unkC88[i] != 0 && func_00583EA0(p->unkC88[i], 0x20000) != 0) {
            b[n++] = p->unkC88[i];
        }
    }
    for (i = 1; i < 0x20; i++) {
        if (p->unkC88[i] != 0 && func_00583EA0(p->unkC88[i], 0x20000) == 0 && n < 0x20) {
            b[n++] = p->unkC88[i];
        }
    }
    for (i = 1; i < 0x20; i++) {
        if (i < n) {
            p->unkC88[i] = b[i];
        } else {
            p->unkC88[i] = 0;
        }
    }
}
#endif
