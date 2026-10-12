#include "system.h"
#include "ps2mem.h"
#include "libgraph.h"
#include "task.h"
extern f32 D_0023BB60;
extern s32 D_0023BB64;
extern u16 D_0023BBA4;
extern u16 D_0023BBA8;
extern u16 D_0023BBAC;
s32 func_0019E9A0(s32 w, s32 h);
double func_001A8570(void);
void func_001AE710(void);
s32 func_001AE800(void);
void func_001B63C0(void);
void func_001B65D0(void (*fn)(Task *), s16 slot);
void func_001B9140(s32 frames);
void func_001B94E0(void);
void func_001B97D0(void);
void func_001BAC60(Task *t);

/* The per-frame system step: on the first call it sets the system up, after that it runs once a frame. */
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
        func_001B9140(1);
        D_0023BBA8 = D_0023BBAC;
        D_0023BBA4++;
        D_0023BB60 = func_001A8570();
        if (func_001AE800() != 0) {
            func_001AE710();
        }
        func_001B63C0();
        break;
    }
    return 0;
}

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

s32 func_001AE800(void) {
    return 0;
}

/* Seconds the last frame took (always 0 in this build) */
double func_001A8570(void) {
    return 0.0;
}

s32 func_0019ECB0(s32 unused);
s32 func_001AB8E0(void);
extern s32 D_003065C4[];   /* (Options) */

/* Run `frames` frames of system work. */
void func_001B9140(s32 frames) {
    s32 i;

    for (i = 0; i < frames; i++) {
        func_0019ECB0(0);
    }
    D_003065C4[0] = func_001AB8E0();
}

void func_001AE290(void);
void func_001B97C0(void);
void func_001B4C50(void);
void func_001B28C0(void);
void func_001BA6C0(void);
void func_001AE2D0(void);

/* Before the frame is sent */
void func_001B91F0(void) {
    if (D_0023BB64 != 0) {
        func_001AE290();
        func_001B97C0();
        func_001B4C50();
        func_001B28C0();
        func_001BA6C0();
        func_001AE2D0();
    }
}

void func_001AD740(void);
void func_001B28A0(void);
void func_001A10F0(void);
void func_001B9760(void);
void func_001AC240(void);
void func_001CF550(void);

/* After the frame is sent */
void func_001B91A0(void) {
    if (D_0023BB64 != 0) {
        func_001AD740();
        func_001B28A0();
        func_001A10F0();
        func_001B9760();
        func_001AC240();
        func_001CF550();
    }
}

s32 func_0018D0E0(void);
void func_0018CF10(void);
void func_0018D7C0(void);
void func_0019FEE0(void);
extern s32 D_0023BA98;       /* frames sent */

#define T0_COUNT 0x10000000
#define T0_MODE 0x10000010

/* One frame: finish the game's work on it, wait for the last one to be drawn and flipped, then send it. */
s32 func_0019ECB0(s32 unused) {
    func_001B91F0();
    func_0018D0E0();
    func_001A84A0();
    HW_REG32(T0_MODE) = 0x83;     /* timer 0: bus clock / 256, counting */
    HW_REG32(T0_COUNT) = 0;
    D_00281DB0.vcount = 0;
    D_0023BA98++;
    func_0018CF10();
    func_0018D7C0();
    func_0019FEE0();
    D_00281DB0.unk3F0 = 0;
    D_00281DB0.unk3EC = -1;
    func_001B91A0();
    return 1;
}

extern s32 D_0023BA18;

s32 func_001AB8E0(void) {
    return D_0023BA18;
}

void func_001B97C0(void) {
}

void func_001B28C0(void) {
}

extern s32 D_0023BA9C;      /* screen height */
extern s32 D_0023BAA0;      /* screen width */
extern u32 D_0023BA24;
s32 func_0018CA60(s32 mode, u32 entry, u32 *tag, DmaQueue *q);
u32 func_0018D850(u32 size, s32 align);
void func_00189618(void *src, void *dst, s32 n);

#ifdef NON_MATCHING   /* instruction scheduling of the loads */
/* Start the next frame: clear it (a full-screen sprite in the clear colour) and set the GS state. */
void func_0019FEE0(void) {
    u32 p;

    D_00281B30.reg[1].data = ((s64)(D_0023BAA0 - 1) << 16) | ((s64)(D_0023BA9C - 1) << 48);
    D_00281B30.reg[7].data = D_00281DB0.scissor1;
    D_00281B30.reg[8].data = D_00281DB0.scissor2;
    D_00281B30.reg[5].data = (D_00281DB0.offsetX << 4) | ((u64)(D_00281DB0.offsetY << 4) << 16);
    D_00281B30.reg[6].data = ((D_00281DB0.offsetX + D_0023BAA0) << 4) |
                             ((u64)((D_00281DB0.offsetY + D_0023BA9C) << 4) << 16);
    D_00281B30.reg[4].data = ((u64)((D_00281DB0.clearColor >> 24) & 0xFF) << 24) |
                             ((u64)((D_00281DB0.clearColor >> 16) & 0xFF) | ((u64)((D_00281DB0.clearColor >> 8) & 0xFF) << 8) |
                              ((u64)(D_00281DB0.clearColor & 0xFF) << 16)) |
                             ((u64)1 << 32);
    D_00281B30.reg[9].data = D_00281DB0.test[0];
    D_00281B30.reg[10].data = D_00281DB0.test[1];
    D_00281B30.reg[11].data = D_00281DB0.zbuf[0];
    D_00281B30.reg[12].data = D_00281DB0.zbuf[1];
    D_00281B30.reg[13].data = D_0023BA24;
    D_00281B30.reg[14].data = D_0023BA24;
    p = func_0018D850(sizeof(GsClearPacket), 16);
    func_00189618(&D_00281B30, (void *)p, sizeof(GsClearPacket) / 16);
    func_0018CA60(0, p & 0x0FFFFFFF, (u32 *)p, &D_00281AF0);
}
#endif
