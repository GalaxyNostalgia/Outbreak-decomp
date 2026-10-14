#include "common.h"
#include "game.h"

/* GAME.BIN: the scene's flags: the event flags (D_0048AD90), the per-player flags (D_0048AC10:
   online, 0x10 words a player) and the 16-bit flag words of D_0048AFB0 (6 a row); changes are
   passed on to the other players (func_0064BFC0, func_0064C0A0). */

extern u32 D_0048AD90[];
extern u32 D_0048AC10[][0x10];
extern u16 D_0048AFB0[][6];

s32 func_001AF4D0(void);
void func_0064BFC0(s32 id, s32 on, s32 player);
void func_0064C0A0(s32 row, s32 bit, s32 on, s32 col);

void func_00540370(u32 n) {
    D_0048AD90[n >> 5] |= 1 << (n & 0x1F);
}

void func_005403A0(u32 n) {
    D_0048AD90[n >> 5] &= ~(1 << (n & 0x1F));
}

/* An event flag set (on) or cleared, told to the others as ours. */
void func_005403E0(s32 id, s32 on) {
    if (on != 0) {
        func_00540370(id);
    } else {
        func_005403A0(id);
    }
    func_0064BFC0(id, on, (u8)(D_0048BF60.unk1D + 1));
}

/* The same, told as the scene's. */
void func_00540450(s32 id, s32 on) {
    if (on != 0) {
        func_00540370(id);
    } else {
        func_005403A0(id);
    }
    func_0064BFC0(id, on, 0);
}

/* A player's flag (online; offline the one set). */
void func_005404B0(u32 n, u8 player) {
    if (func_001AF4D0() == 1) {
        D_0048AC10[player][n >> 5] |= 1 << (n & 0x1F);
    } else {
        D_0048AC10[0][n >> 5] |= 1 << (n & 0x1F);
    }
}

void func_00540550(u32 n, u8 player) {
    if (func_001AF4D0() == 1) {
        D_0048AC10[player][n >> 5] &= ~(1 << (n & 0x1F));
    } else {
        D_0048AC10[0][n >> 5] &= ~(1 << (n & 0x1F));
    }
}

void func_005405F0(u32 n, s32 on, u8 player) {
    if (on != 0) {
        func_005404B0(n, player);
    } else {
        func_00540550(n, player);
    }
}

void func_00540620(s32 row, s32 bit, s32 col) {
    D_0048AFB0[row][(u8)col] &= ~(1 << bit);
}

void func_00540670(s32 row, s32 bit, s32 col) {
    D_0048AFB0[row][(u8)col] |= 1 << bit;
}

void func_005406B0(s32 row, s32 bit, s32 col) {
    func_00540670(row, bit, col);
    func_0064C0A0(row, bit, 1, col);
}

void func_00540700(s32 row, s32 bit, s32 col) {
    func_00540620(row, bit, col);
    func_0064C0A0(row, bit, 0, col);
}
