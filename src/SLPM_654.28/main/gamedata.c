#include "common.h"
#include "game.h"
#include "options.h"

/* The game's object pools: cleared for each game. Names are what they hold so far. */

void *memset(void *dst, s32 c, u32 n);

extern s32 D_0053AA10[0x2BC];
extern s32 D_0053A910[0x40];

void func_001B0090(void) {
    s32 *p = D_0053AA10;
    s32 i;

    for (i = 0; i < 0x2BC; i++) {
        *p++ = -1;
    }
}

void func_001B0160(void) {
    s32 *p = D_0053A910;
    s32 i;

    for (i = 0; i < 0x40; i++) {
        *p++ = -1;
    }
}

void func_001B11C0(void *p) {
    memset(p, 0, 0xC84);
}

void func_001B11D0(void *p) {
    memset(p, 0, 0x1F44);
}

void func_001B11E0(void *p) {
    memset(p, 0, 0x3EC);
}

extern u8 D_004BBA40[0x1F44];
extern u8 D_004B9AF0[0x1F44];
extern u8 D_004B7BA0[0x1F44];
extern u8 D_004A8180[8][0x1F44];
extern u8 D_004948D0[10][0x1F44];
extern u8 D_0048FDB0[6][0xC84];
extern u8 D_0048E620[6][0x3EC];
extern u8 D_0048C030[0x3E];

void func_001B11F0(void) {
    s32 i;

    func_001B11D0(D_004BBA40);
    func_001B11D0(D_004B9AF0);
    func_001B11D0(D_004B7BA0);
    for (i = 0; i < 8; i++) {
        func_001B11D0(D_004A8180[i]);
    }
    for (i = 0; i < 10; i++) {
        func_001B11D0(D_004948D0[i]);
    }
    for (i = 0; i < 6; i++) {
        func_001B11C0(D_0048FDB0[i]);
    }
    for (i = 0; i < 6; i++) {
        func_001B11E0(D_0048E620[i]);
    }
    for (i = 0x3D; i >= 0; i--) {
        D_0048C030[i] = 0xFF;
    }
}

/* 0x480 blocks of 0x190 bytes, handed out from a stack of pointers. */
extern u8 D_004CA110[0x480][0x190];
extern u8 *D_004BEEF0[];        /* the stack's end (it grows down) */
extern u8 **D_0023BE20;         /* top */
extern s16 D_0023BE24;          /* free */

void func_001B0230(void) {
    u8 *p;
    s16 i;

    memset(D_004CA110, 0, sizeof(D_004CA110));
    p = D_004CA110[0x47F];
    D_0023BE20 = D_004BEEF0;
    for (i = 0; i < 0x480; i++) {
        *--D_0023BE20 = p;
        p -= 0x190;
    }
    D_0023BE24 = 0x480;
}

extern u8 D_004BDA90[0x258];

void func_001B02B0(void) {
    s16 i;

    for (i = 0; i < 0x258; i++) {
        memset((u8 *)D_004BEEF0 + i * 0x4C, 0, 0x4C);
        D_004BDA90[i] = 0;
    }
}

extern u8 D_0048AC10[0x1254];

void func_001AE6D0(void) {
    memset(&D_0048BF60, 0, sizeof(D_0048BF60));
    memset(D_0048AC10, 0, sizeof(D_0048AC10));
}

/* Left empty in the release (it took arguments). */
void func_001B69A0(void) {
}

void func_001AE7F0(void) {
    D_00306590.unkD = 1;
}

/* Left empty in the release. */
void func_001CF590(void) {
}

extern s16 D_0048B9BC[];
extern s16 D_0048B9BE[];
extern u8 D_0048B9C0[];
extern u8 D_0048A810[0x400];

void func_001B1DE0(void) {
    D_0048B9BC[0] = 0;
    D_0048B9BE[0] = 0;
    memset(D_0048B9C0, 0, 4);
    memset(D_0048A810, 0xFF, sizeof(D_0048A810));
}

/* Freeing a model. */
#include "model.h"

extern s32 D_0053AA10[0x2BC];   /* packet groups in use (-1: free) */
extern s32 D_0053A910[0x40];    /* memory handles in use (-1: free) */
extern s32 D_004BDCF0[0x480];   /* blocks that aren't the pool's */

void func_001B0120(s32 group) {
    s32 *p = D_0053AA10;
    s32 i;

    for (i = 0; i < 0x2BC; i++, p++) {
        if (*p == group) {
            *p = -1;
        }
    }
}

void func_001B01F0(s32 handle) {
    s32 *p = D_0053A910;
    s32 i;

    for (i = 0; i < 0x40; i++, p++) {
        if (*p == handle) {
            *p = -1;
        }
    }
}

static s32 func_001B0370(u8 *block) {
    s32 *p = D_004BDCF0;
    s32 i;

    for (i = 0; i < 0x480; i++, p++) {
        if (*p == (s32)block) {
            return 1;
        }
    }
    return 0;
}

/* Gives a block back to the pool. */
void func_001B03B0(u8 *block) {
    if (func_001B0370(block) != 1) {
        D_0023BE24++;
        *--D_0023BE20 = block;
        memset(block, 0, 0x190);
    }
}

void func_001B0A70(Slot1468 *m) {
    u32 *p;
    s32 i;
    u32 *q;
    s32 k;

    for (i = 0, p = m->unk0; i < m->nblocks; i++, p++) {
        func_001B03B0((u8 *)*p);
        *p = 0;
    }
    m->unk0 = NULL;
    q = m->unk4;
    if (q != NULL && m->unk81C != 1) {
        for (k = 0; k < m->nblocks; k++, q++) {
            if (*q != 0) {
                func_001B03B0((u8 *)*q);
            }
            *q = 0;
        }
    }
    m->unk4 = NULL;
    m->nblocks = 0;
}

void func_001B0B30(Slot1468 *m) {
    s32 i;

    for (i = 0; i < m->nids; i++) {
        D_004BDA90[m->ids[i]] = 0;
        memset((u8 *)D_004BEEF0 + m->ids[i] * 0x4C, 0, 0x4C);
        m->ids[i] = 0;
    }
    m->nids = 0;
}


void func_001B1BF0(u8 *buf) {
    GameWork *w = &D_0029B140;
    s32 i;

    for (i = 0; i < 100; i++) {
        if (w->bufs[i] == buf) {
            break;
        }
    }
    if (i != 100) {
        if (w->counts[i] != 0) {
            memset(w->bufs[i], 0, w->counts[i] * 0x98);
            w->owner[i] = 0;
        }
    }
}

s32 func_0018BF70(u32 n);
s32 func_00191F50(s32 handle);

void func_001B0450(Slot1468 *m) {
    s32 i;
    s32 g;
    s32 off;

    if (m->buf == NULL) {
        return;
    }
    for (i = 0; i < m->nparts; i++) {
        off = i * 0x98;
        g = *(s32 *)(m->buf + off);
        if (g != -1 && g != 0) {
            func_0018BF70(g);
            func_001B0120(*(s32 *)(m->buf + off));
            *(s32 *)(m->buf + off) = 0;
        }
    }
    func_001B0B30(m);
    func_001B0A70(m);
    if (m->unk80C != 0) {
        func_00191F50(m->unk80C);
        func_001B01F0(m->unk80C);
        m->unk80C = 0;
    }
    func_001B1BF0(m->buf);
    m->buf = NULL;
    m->nparts = 0;
}

void func_001B08C0(Slot1468 *m) {
    s32 i;
    s32 g;
    s32 off;

    if (m->buf == NULL) {
        return;
    }
    for (i = 0; i < m->nparts; i++) {
        off = i * 0x98;
        g = *(s32 *)(m->buf + off);
        if (g != -1 && g != 0) {
            func_0018BF70(g);
            func_001B0120(*(s32 *)(m->buf + off));
            *(s32 *)(m->buf + off) = 0;
        }
    }
    func_001B0B30(m);
    func_001B1BF0(m->buf);
    m->buf = NULL;
    m->nparts = 0;
}

/* What a character holds: frees its model when there's one. */
void func_001B1320(s8 *held) {
    if (*held != 0) {
        func_001B08C0((Slot1468 *)(held + 4));
    }
    *held = 0;
}
