#include "common.h"
#include "game.h"
#include "object.h"

/* Using an item from the menu (D_003AEF50: the mode, its step, a timer, and the effect entries
   of 0x198 bytes). */

typedef struct {
    u8 unk0[2];
    s8 step;            /* 0x02 */
    u8 unk3;            /* 0x03 */
    u8 unk4;
    u8 cur;             /* 0x05: the entry running (0xFF: none) */
    u16 timer;          /* 0x06: 0xFFFF, 0xFFFE: no count-down */
    u8 unk8[0x24 - 8];
    u8 player;          /* 0x24 */
    s8 mode;            /* 0x25 */
    u8 unk26;
    u8 unk27;           /* 0x27 */
    u8 unk28[4];
    s16 unk2C;          /* 0x2C */
    s16 unk2E;          /* 0x2E */
    f32 unk30;          /* 0x30 */
    struct {
        s8 active;      /* 0x00 */
        u8 unk1[0x198 - 1];
    } entries[1];       /* 0x34 */
} ItemUse;

extern ItemUse D_003AEF50;
extern DrawObjFull D_00476DD0[];
void *func_00543800(void *owner, void *fn);
void func_005A9080(void);
void func_005A8EB0(void);
void func_005A94E0(void);
void func_00345740(void);
void func_0058E7B0(void);

/* Once a frame. */
void func_0058E420(void);
#ifdef NON_MATCHING   /* first draft */
void func_0058E420(void) {
    ItemUse *f = &D_003AEF50;
    DrawObjFull *p = &D_00476DD0[D_003AEF50.player];
    u8 *o;

    switch (D_003AEF50.mode) {
    case 1:
        switch (f->step) {
        case 0:
            if (p->unk54F != 0xA && p->unk54F != 0x3A) {
                break;
            }
            f->step++;
        case 1:
            if (p->unk54F != 0xA && p->unk54F != 0x3A) {
                f->mode = 0;
                break;
            }
            o = func_00543800(0, func_005A9080);
            if (o != NULL) {
                o[4] = 1;
                func_00543800(0, func_005A8EB0);
                f->unk3 = 0xFF;
                f->step++;
            } else {
                f->mode = 0;
            }
            func_00345740();
            return;
        case 2:
            if (p->unk54F == 0xA || p->unk54F == 0x3A) {
                func_00345740();
                return;
            }
            f->mode = 0;
            break;
        default:
            func_00345740();
            return;
        }
        break;
    case 2:
        switch (f->step) {
        case 0:
            f->step++;
            func_0058E7B0();
        case 1:
            if (p->unk54F == 0xA) {
                o = func_00543800(0, func_005A9080);
                if (o != NULL) {
                    o[4] = 2;
                    func_00543800(0, func_005A8EB0);
                    f->unk3 = 0xFF;
                    f->step++;
                } else {
                    f->mode = 0;
                }
            }
            func_00345740();
            return;
        case 2:
            if (p->unk54F == 0xA) {
                func_00345740();
                return;
            }
            f->mode = 0;
            break;
        default:
            func_00345740();
            return;
        }
        break;
    case 3:
        if (f->timer != 0) {
            f->entries[f->cur].active = 0;
            f->cur = 0xFF;
            f->timer = 0;
            f->mode = 0;
            break;
        }
        func_00345740();
        f->mode = 0;
        return;
    case 4:
        if (f->timer != 0) {
            FIELD(&D_0048BF60, u8, 0xB9) = 1;
            if (f->timer != 0xFFFF && f->timer != 0xFFFE) {
                D_003AEF50.timer--;
            }
            if (f->timer != 0xFFFE && (p->unkBA0 & 0x20000)) {
                f->timer = 0;
            }
            if (f->timer == 0) {
                f->entries[f->cur].active = 0;
                f->cur = 0xFF;
                f->mode = 0;
            }
        } else {
            f->mode = 0;
            f->unk3 = 0xFF;
        }
        break;
    case 5:
        switch (f->step) {
        case 1:
            break;
        case 0:
            o = func_00543800(0, func_005A94E0);
            if (o != NULL) {
                o[4] = f->unk27;
                *(s16 *)(o + 0x10) = f->unk2C;
                *(s16 *)(o + 0x12) = f->unk2E;
                *(f32 *)(o + 0x70) = f->unk30;
                f->step++;
            } else {
                f->mode = 0;
            }
            break;
        }
        break;
    }
    func_0058E7B0();
}
#endif
