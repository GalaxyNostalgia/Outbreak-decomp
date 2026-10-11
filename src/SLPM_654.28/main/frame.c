#include "common.h"
extern f32 D_0023BB60;
extern s32 D_0023BB64;
extern u16 D_0023BBA4;
extern u16 D_0023BBA8;
extern u16 D_0023BBAC;
f32 func_001121B0(s32);
s32 func_0019E9A0(s32 w, s32 h);
s32 func_001A8570(void);
void func_001AE710(void);
s32 func_001AE800(void);
void func_001B63C0(void);
void func_001B65D0(void (*fn)(void), s32 arg);
void func_001B9140(void);
void func_001B94E0(void);
void func_001B97D0(void);
void func_001BAC60(void);

/* The per-frame system step: on the first call it sets the system up, after that it runs once a frame.
   Not matching yet: register allocation (4 instructions). */
#ifdef NON_MATCHING
s32 func_001B9070(void) {
    switch (D_0023BB64) {
    case 0:
        while (func_0019E9A0(640, 448) == 0) {
        }
        func_001B94E0();
        func_001B97D0();
        func_001B65D0(func_001BAC60, 0);
        D_0023BB64++;
        break;
    case 1:
        func_001B9140();
        D_0023BBA4++;
        D_0023BBA8 = D_0023BBAC;
        D_0023BB60 = func_001121B0(func_001A8570());
        if (func_001AE800() != 0) {
            func_001AE710();
        }
        func_001B63C0();
        break;
    }
    return 0;
}
#endif
