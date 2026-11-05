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

extern u8 D_0048AE10[];         /* flags, a bit each */

s32 func_005409B0(u32 n) {
    return (D_0048AE10[n >> 3] & (1 << (n & 7))) != 0;
}

extern u16 D_0048BDE0[];        /* the map levels shown, a bit each */

/* Whether the map shows level n. */
s32 func_00540A10(s32 n) {
    return (D_0048BDE0[0] & (1 << (n & 0xF))) != 0;
}

s32 func_001AF510(void);
void func_001AF000(s32 to, void *msg, s32 kind, s32 a3);

/* A flag word change of D_0048AFB0 (row, bit, on, col) told to the others (online). */
void func_0064C0A0(s32 row, s32 bit, s32 on, s32 col) {
    struct {
        u8 kind;
        u8 len;
        u8 unk2[2];
        u8 row;
        u8 bit;
        u8 on;
        u8 col;
        u8 pad[0x28];
    } m;

    if (func_001AF510() != 0) {
        m.kind = 1;
        m.len = 8;
        m.row = row;
        m.bit = bit;
        m.on = on;
        m.col = col;
        func_001AF000(5, &m, 0, 4);
    }
}

s32 func_001AF0A0(void);
void func_00540C00(u8 room, u8 bit);
void func_00540CD0(u8 a, u8 b, u8 c);
void func_00540A30(u8 room, u8 bit, s32 value);
void func_00585490(u16 a, u8 b, u8 c, u8 *data);
void func_0064FDD0(u8 player);
void func_0064E940(s8 a, s8 b, s8 c, s16 d);
void func_0064E600(s32 i, s32 take, s16 value);

extern u8 D_0048BBE0[];         /* per event flag: who set it + 1 */
extern u8 D_0048AE20[];         /* per mark: who holds it (0xFF none) */
extern s16 D_0048AE30[];
extern s16 D_0048AE50[];        /* its value */
extern s16 D_0048AE70[];
extern u8 D_0071A000[][0x60];
extern u8 D_0048BE24[][0x10];
extern u8 D_0048BE28[][0x10];
extern u16 D_0048BE2C[][8];

/* A flag message from another player (online): 0 an event flag, 1 a flag word, 2 / 3 a mark
   taken or given back (the server's word on it when we're it, func_001AF0A0), 4 several flag
   words' values, 5 func_0064FDD0, 6 a room entry's byte, 7 func_00540CD0, 9 func_00585490,
   10 a player's tables. */
void func_0064C130(u8 *m);
#ifdef NON_MATCHING   /* first draft (265 vs 258) */
void func_0064C130(u8 *m) {
    s32 i;
    u8 *b;

    if (func_001AF510() == 0) {
        return;
    }
    switch (m[0]) {
    case 0:
        if (m[6] != 0) {
            D_0048BBE0[*(u16 *)&m[4]] = m[2] + 1;
            func_00540370(*(u16 *)&m[4]);
            return;
        }
        func_005403A0(*(u16 *)&m[4]);
        break;
    case 1:
        if (m[6] != 0) {
            func_00540670(m[4], m[5], m[7]);
            if (m[7] == 2) {
                func_00540C00(m[4], m[5]);
            }
        } else {
            func_00540620(m[4], m[5], m[7]);
        }
        break;
    case 2:
        b = m + 4;
        if (func_001AF0A0() == 0) {
            break;
        }
        if (b[1] != 0) {
            if (D_0048AE20[b[0]] == 0xFF || D_0048AE20[b[0]] == m[2]) {
                D_0048AE20[b[0]] = m[2];
                D_0048AE50[b[0]] = *(s16 *)&b[2];
                func_0064E940(b[0], m[2], 0, *(s16 *)&b[2]);
            } else {
                func_0064E940(b[0], m[2], 1, *(s16 *)&b[2]);
            }
        } else if (D_0048AE20[b[0]] == m[2] && D_0048AE50[b[0]] == *(s16 *)&b[2]) {
            D_0048AE20[b[0]] = 0xFF;
            D_0048AE50[b[0]] = -1;
            func_0064E940(b[0], 0xFF, 0, -1);
        }
        break;
    case 3:
        b = m + 4;
        if (m[6] != 0) {
            if (D_0048BF60.unk1D == b[1] && D_0048AE70[b[0]] == *(s16 *)&b[4]) {
                D_0048AE30[b[0]] = 0;
            }
        } else if (func_001AF0A0() == 0) {
            if (D_0048BF60.unk1D == b[1] && D_0048AE70[b[0]] != *(s16 *)&b[4]) {
                func_0064E600(b[0], 0, 0);
                break;
            }
            D_0048AE20[b[0]] = b[1];
            D_0048AE50[b[0]] = *(s16 *)&b[4];
        }
        break;
    case 4:
        for (i = 0; i < m[4]; i++) {
            func_00540A30(m[8 + i * 4], m[9 + i * 4], *(s16 *)&m[0xA + i * 4]);
        }
        break;
    case 5:
        if (func_001AF0A0() != 0) {
            func_0064FDD0(m[2]);
        }
        break;
    case 6:
        D_0071A000[m[4]][0x5E] = m[5];
        break;
    case 7:
        func_00540CD0(m[4], m[5], m[6]);
        break;
    case 9:
        func_00585490(*(u16 *)&m[4], m[6], m[7], m + 8);
        break;
    case 10:
        D_0048BE24[m[2]][m[5]] = m[4];
        D_0048BE28[m[2]][m[5]] = m[6];
        D_0048BE2C[m[2]][m[5]] = *(u16 *)&m[8];
        break;
    }
}
#endif
