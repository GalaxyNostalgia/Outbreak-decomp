#include "task.h"

/* The first task: started by the frame loop, it loads SUBMAIN.BIN and starts the game. */

void func_001BACB0(Task *t);
void func_001BAE10(Task *t);

void func_001BAC60(Task *t) {
    switch (t->step) {
    case 0:
        func_001BACB0(t);
        break;
    case 1:
        func_001BAE10(t);
        break;
    }
}

void func_001A8990(u32 ovl);
void func_001B67A0(s16 slot, void (*onKill)(void));
void func_001B65D0(void (*fn)(Task *), s16 slot);
void func_001BB200(void);
void func_001BD8F0(void);
void func_001B8D00(Task *t);
void func_001B2C90(void);
void func_001D6CC0(void);
extern s8 D_0023BB90;

void func_001BACB0(Task *t) {
    t->step++;
    t->sub = 0;
    func_001A8990(0);
    func_001B67A0(0, func_001BB200);
    func_001BD8F0();
    func_001B65D0(func_001B8D00, 14);
    D_0023BB90 = 1;
    func_001B2C90();
    func_001D6CC0();
}

s32 func_001D99C0(void);
void func_001A8B40(void);
void func_001D99A0(void);
void func_001BB190(void);
void func_001D5320(void);
extern s32 D_0023BBB0;      /* the update check's answer */

void func_001BAE50(void) {
    if (D_0023BBB0 == 0) {
        D_0023BBB0 = func_001D99C0();
    }
}

void func_001BAE20(void) {
    func_001A8B40();
    func_001D99A0();
}

void func_001BAD20(void) {
    func_001BB190();
    func_001D5320();
}

s32 func_001AFB10(void);
s32 func_001BB210(void);
void func_001C0940(s32 a, s32 b);
s32 func_001C0C20(void);
void func_001BA7A0(s16 id);
s32 func_001D7DD0(void);
void func_001AE9C0(s32 cmd, u32 arg);
void func_001BAD50(s32 logo);
void func_001B9CC0(void);
void func_001B9240(void);
void func_001BA870(void);
void func_001B6850(void);
void func_001B66A0(s16 slot);

#ifdef NON_MATCHING   /* the original reads the step into a2 */
/* The boot sequence: controllers, memory card, hard disk, then the logos, then the title (DEMO.BIN). */
void func_001BAE80(Task *t) {
    switch (t->sub) {
    case 0:
        if ((s8)func_001AFB10() != -1) {
            t->sub++;
        }
        break;
    case 1:
        if (func_001BB210() == 0) {
            t->sub++;
        }
        break;
    case 2:
        func_001C0940(0, 0);
        t->sub = 4;
        break;
    case 3:
        t->sub++;
        break;
    case 4:
        if (func_001C0C20() == 0) {
            t->count = 0x80;
            func_001BA7A0(8);
            t->sub++;
        }
        break;
    case 5:
        func_001BAD20();
        func_001BAE20();
        D_0023BBB0 = 0;
        t->sub++;
        func_001BA7A0(8);
        break;
    case 6:
        if (func_001D7DD0() == 0) {
            t->sub++;
        }
        break;
    case 7:
        if (--t->count != 0) {
            if (t->count == 20) {
                func_001BA7A0(7);
            }
            func_001AE9C0(0x14, 0xFF000000);
            func_001BAD50(3);
            func_001B9CC0();
        } else {
            t->sub++;
            t->count = 0x80;
            func_001BA7A0(8);
        }
        break;
    case 8:
        if (--t->count != 0) {
            if (t->count == 20) {
                func_001BA7A0(7);
            }
            func_001AE9C0(0x14, 0xFF000000);
            func_001BAD50(1);
            func_001B9CC0();
        } else {
            t->sub++;
            t->count = 0x80;
            func_001AE9C0(0x14, -1);
            func_001BA7A0(8);
        }
        break;
    case 9:
        if (--t->count != 0) {
            if (t->count == 20) {
                func_001BA7A0(7);
            }
            func_001AE9C0(0x14, -1);
            func_001BAD50(2);
            func_001B9CC0();
        } else {
            func_001AE9C0(0x14, 0xFF000000);
            t->sub++;
        }
        break;
    case 10:
        if (D_0023BBB0 == 0) {
            func_001BAE50();
            return;
        }
        func_001B9240();
        func_001AE9C0(0x14, 0xFF000000);
        func_001BA870();
        func_001B6850();
        func_001B66A0(0);
        break;
    }
    if (t->sub >= 7 && t->sub < 10) {
        func_001BAE50();
    }
}
#endif

void func_001BAE10(Task *t) {
    func_001BAE80(t);
}
