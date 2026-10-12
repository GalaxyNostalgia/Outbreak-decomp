#include "system.h"
#include "drawlist.h"

extern s32 D_0023BB80;      /* drawing open */
s32 func_001957B0(void);
s32 func_001957C0(void);

void func_001AE290(void) {
    if (D_0023BB80 == 0) {
        func_001957B0();
        D_0023BB80++;
    }
}

void func_001AE2D0(void) {
    if (D_0023BB80 != 0) {
        func_001957C0();
        D_0023BB80--;
    }
}

s32 func_001957B0(void) {
    D_00281DB0.bufState = 1;
    return 1;
}

s32 func_001957C0(void) {
    D_00281DB0.bufState = 2;
    return 1;
}


void func_001B4390(s32 alt) {
    if (alt == 0) {
        D_00239F80 = &D_0029CD80;
    } else {
        D_00239F80 = &D_0029CCD0;
    }
}

s32 func_001B6980(void) {
    return 0;
}

void func_001ACA80(void);

void func_001AC240(void) {
    func_001ACA80();
}

void func_00195970(u8 cmd, ...);

void func_001AE9C0(s32 cmd, u32 arg) {
    func_00195970(cmd, arg);
}

extern u8 D_003065D0[];     /* (Options) */
extern u8 D_003065D1[];
extern u32 D_002167F0[];
extern u32 D_00216820[];

void func_001AE430(s32 a, s32 b) {
    D_003065D0[0] = a;
    D_003065D1[0] = b;
    func_001AE9C0(0x5E, D_002167F0[a] | D_00216820[b]);
}

extern u8 D_003065A5[];     /* (Options) */
void func_349EC0(void);     /* SUBMAIN.BIN */
void func_001CF840(void);

void func_001CF550(void) {
    if (D_003065A5[0] == 1) {
        func_349EC0();
    } else {
        func_001CF840();
    }
}

void func_001AF820(void);
void func_001AFA90(void *pad);
void func_001AFE90(void);
extern u8 D_003064D0[], D_00306500[], D_00306530[], D_00306560[];

void func_001B9760(void) {
    func_001AF820();
    func_001AFA90(D_003064D0);
    func_001AFA90(D_00306500);
    func_001AFA90(D_00306530);
    func_001AFA90(D_00306560);
    func_001AFE90();
}
