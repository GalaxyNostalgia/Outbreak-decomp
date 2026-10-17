#include "common.h"
#include "object.h"

/* GAME.BIN: each player's motion data (slots 0-3; 4: shared), and which slot each character
   kind uses. */

extern u8 *D_00719EB8[5];
extern u8 *D_00719ECC[5];
extern u8 *D_00719EE0[5];
extern s8 D_00719EF4[];
extern DrawObjFull D_00476DD0[];

/* Player slot's motion file p (kind 0: its two parts by the offsets at 0x10 and 0x14). */
void func_00684FC0(u8 *p, s32 kind, s32 slot) {
    if (kind == 0) {
        D_00719ECC[slot] = p + *(s32 *)(p + 0x10);
        D_00719EB8[slot] = p + *(s32 *)(p + 0x14);
    } else {
        D_00719EE0[slot] = p;
    }
    if (slot != 4) {
        D_00719EF4[D_00476DD0[slot].unkB98] = slot;
    }
}
