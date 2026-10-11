#include "system.h"
#include "ps2mem.h"
#include "libgraph.h"
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

extern s32 D_0023BAF4;          /* a movie is playing (CRI Sofdec runs the frame) */
void func_0012BC20(s32 unused);  /* CRI ADX v-blank server */
void func_0015F790(void);        /* CRI Sofdec: wait for the next frame */
s32 func_001AB9C0(s32 nowait);

void func_001A8470(void) {
    if (D_0023BAF4 != 0) {
        func_0012BC20(0);
    }
}

/* End of frame: ask the v-blank handler to flip, and wait until it has (when the frame took no longer than
   its v-blank budget). */
void func_001A84A0(void) {
    func_001AB9C0(1);
    if (D_00281DB0.vcount <= D_00281DB0.swapWait) {
        u64 csr = HW_REG64(GS_CSR);
        D_00281DB0.swapRequest = 1;
        D_00281DB0.vsync = (csr >> 13) & 1;
        do {
            if (D_0023BAF4 != 0) {
                func_0015F790();
            } else {
                sceGsSyncV(0);
            }
        } while (D_00281DB0.swapRequest != 0);
    } else {
        D_00281DB0.swapRequest = 1;
        if (D_0023BAF4 != 0) {
            func_0015F790();
        } else {
            sceGsSyncV(0);
        }
    }
}
