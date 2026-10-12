#include "task.h"
#include "options.h"

/* Boot: the hard disk check (the game can run from an HDD install). */

extern s32 D_00239FE0;      /* HDD found by the IOP set-up: 0 yes, 1 unformatted?, 2/3 none */
extern s32 D_0023BBB4;      /* the game's partition: 0 none, 1 ok, 2 broken */
extern u16 D_003064D0[];    /* player 1's buttons pressed (Ctrl.pressed) */

s32 func_001BBA60(void) {
    switch (D_00239FE0) {
    case 1:
        return -2;
    case 2:
    case 3:
        return -1;
    }
    switch (D_0023BBB4) {
    case 2:
        return -6;
    case 0:
        return -4;
    }
    return 0;
}

extern u32 *D_002180B0[];   /* messages */

u32 *func_001C0220(s32 id) {
    return D_002180B0[id];
}

void func_001B4340(s32 z);
void func_001B4300(s32 size);
void func_001B4370(s32 clut);
void func_001BC600(s32 install);
void func_001C07C0(u32 *msg, s32 x, s32 y);

#ifdef NON_MATCHING   /* first draft; register allocation differs */
/* 0: carry on, 1: still here */
s32 func_001BB210(Task *t) {
    u16 pressed = D_003064D0[0];

    func_001B4340(-2);
    func_001B4300(20);
    func_001B4370(0);
    switch (t->count) {
    case 0:
        switch (func_001BBA60()) {
        case 0:
            t->count++;
            t->unkB[0] = 0;
            break;
        case -1:
            D_00306590.unk10[1] = 0;
            return 0;
        case -4:
            D_00306590.unk10[1] = 1;
            return 0;
        case -2:
            D_00306590.unk10[1] = 0;
            t->count = 0x5A;
            t->unkB[0] = 0x20;
            break;
        case -6:
            D_00306590.unk10[1] = 1;
            t->count = 0x5B;
            t->unkB[0] = 0x20;
            break;
        }
        return 1;
    case 1:
        func_001BC600(1);
        D_00306590.unk10[1] = 2;
        return 0;
    case 0x5A:
        if (t->unkB[0] != 0) {
            t->unkB[0]--;
        } else {
            if (pressed & 0xFFF0) {
                return 0;
            }
            func_001C07C0(func_001C0220(2), 0x30, 0x188);
        }
        func_001C07C0(func_001C0220(7), 0x30, 0x138);
        return 1;
    case 0x5B:
        if (t->unkB[0] != 0) {
            t->unkB[0]--;
        } else {
            if (pressed & 0xFFF0) {
                return 0;
            }
            func_001C07C0(func_001C0220(2), 0x30, 0x188);
        }
        func_001C07C0(func_001C0220(6), 0x30, 0x138);
        return 1;
    }
    return 1;
}
#endif
