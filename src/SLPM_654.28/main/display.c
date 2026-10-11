#include "system.h"
#include "libgraph.h"
#include "sce.h"
#include "ps2mem.h"
#include "kernel.h"

void func_00189740(void *dst, s32 qwords);   /* asm: clear 16-byte units */
s32 func_0018D710(s32 handle);

/* Clear all of GS memory (16 strips of 1024x64 32-bit pixels) from a zeroed 256 KiB block. */
void func_0019ED40(void) {
    sceGsLoadImage gp;
    s32 handle;
    void *zero;
    u32 i = 0;

    handle = func_0018D6D0(0x40000, 0);
    zero = (void *)func_0018D720(handle);

    func_00189740(zero, 0x4000);
    for (; i < 16; i++) {
        sceGsSetDefLoadImage(&gp, 0, 16, 0, 0, i * 64, 1024, 64);
        FlushCache(0);
        sceGsExecLoadImage(&gp, zero);
        sceGsSyncPath(0, 0);
    }
    func_0018D710(handle);
}

/* Start a DMA packet: an "end" tag for qwc quadwords (with an interrupt when irq is 1). */
u32 *func_0018C280(u32 *tag, s32 qwc, s32 irq, s32 unused) {
    *(u128 *)tag = U128_ZERO;
    tag[0] = qwc + 0x70000000;
    if (irq == 1) {
        tag[0] |= 0x80000000;
    }
    return tag + 4;
}

extern s32 D_0023BA9C;      /* frame buffer height */
extern s32 D_0023BAA0;      /* frame buffer width */
u32 func_0019BF60(u32 size);
void func_001957E0(void);
s32 func_0019EC00(s32 cause);


#define GS_SET_FRAME(fbp, fbw, psm, fbmsk) \
    ((u64)(fbp) | ((u64)(fbw) << 16) | ((u64)(psm) << 24) | ((u64)(fbmsk) << 32))
#define GS_SET_ZBUF(zbp, psm, zmsk) ((u64)(zbp) | ((u64)(psm) << 24) | ((u64)(zmsk) << 32))
#define GS_SET_XYOFFSET(x, y) ((u64)(x) | ((u64)(y) << 32))
#define GIF_TAG_LO(nloop, eop, nreg) ((u64)(nloop) | ((u64)(eop) << 15) | ((u64)(nreg) << 60))

/* Set up video output, the two frame buffers and the Z buffer in GS memory, and the packets that
   select them. fbDepth/zDepth: bytes per pixel; field: interlace mode; videoMode: 0 NTSC, 1-2 PAL;
   narrow: 1 for a 512-pixel-wide frame buffer, else 640. */
/* Not matching yet: first draft, instruction scheduling and switch layout differ. */
#ifdef NON_MATCHING
void func_0019EE00(s32 fbDepth, s32 zDepth, s32 field, s32 videoMode, s32 narrow) {
    s32 width;
    s32 height;
    s32 dispHeight;
    u32 size;
    u64 fbw;
    GsEnvPacket *env0;
    GsEnvPacket *env1;

    D_00281DB0.videoMode = videoMode;
    D_00281DB0.field = field;
    D_00281DB0.fbDepth = fbDepth;
    D_00281DB0.zDepth = zDepth;
    D_00281DB0.dispAdjX = 0;
    D_00281DB0.dispAdjY = 0;
    if (narrow == 1) {
        D_00281DB0.magH = 4;
        width = 0x200;
    } else {
        D_00281DB0.magH = 3;
        width = 0x280;
    }
    if (field == 0) {
        if (videoMode == 2) {
            sceGsResetGraph(0, 1, 3, 1);
            D_00281DB0.dispX = 0x290;
            D_00281DB0.dispY = 0x48;
            height = 0x100;
            dispHeight = 0x200;
        } else if (videoMode == 1) {
            sceGsResetGraph(0, 1, 3, 1);
            D_00281DB0.dispX = 0x290;
            D_00281DB0.dispY = 0x62;
            height = 0xE0;
            dispHeight = 0x1C0;
        } else {
            sceGsResetGraph(0, 1, 2, 1);
            D_00281DB0.dispX = 0x27C;
            D_00281DB0.dispY = 0x32;
            height = 0xE0;
            dispHeight = 0x1C0;
        }
    } else if (videoMode == 2) {
        sceGsResetGraph(0, 1, 3, 0);
        D_00281DB0.dispX = 0x290;
        height = 0x200;
        D_00281DB0.dispY = 0x48;
        dispHeight = height;
    } else if (videoMode == 1) {
        sceGsResetGraph(0, 1, 3, 0);
        D_00281DB0.dispX = 0x290;
        height = 0x1C0;
        D_00281DB0.dispY = 0x62;
        dispHeight = height;
    } else {
        sceGsResetGraph(0, 1, 2, 0);
        D_00281DB0.dispX = 0x27C;
        height = 0x1C0;
        D_00281DB0.dispY = 0x32;
        dispHeight = height;
    }
    D_00281DB0.dispHeight = dispHeight;
    D_0023BAA0 = width;
    D_00281DB0.width = width;
    D_0023BA9C = height;

    switch (D_00281DB0.fbDepth) {
    case 2:
        D_00281DB0.fbPsm = 2;          /* PSMCT16 */
        D_00281DB0.fbPageW = 0x40;
        D_00281DB0.fbPageH = 0x40;
        break;
    case 3:
        D_00281DB0.fbPsm = 1;          /* PSMCT24 */
        D_00281DB0.fbPageW = 0x40;
        D_00281DB0.fbPageH = 0x20;
        break;
    case 4:
    default:
        D_00281DB0.fbPsm = 0;          /* PSMCT32 */
        D_00281DB0.fbPageW = 0x40;
        D_00281DB0.fbPageH = 0x20;
        break;
    }
    size = fbDepth * (D_00281DB0.fbPageH * ((height + D_00281DB0.fbPageH - 1) / (u32)D_00281DB0.fbPageH)
                      * (D_00281DB0.fbPageW * ((width + D_00281DB0.fbPageW - 1) / (u32)D_00281DB0.fbPageW)));
    D_00281DB0.fbp[0] = func_0019BF60(size) & 0xFFFF;
    D_00281DB0.fbp[1] = func_0019BF60(size) & 0xFFFF;

    switch (D_00281DB0.zDepth) {
    case 2:
        D_00281DB0.zPsm = D_00281DB0.fbDepth == 2 ? 0x32 : 0x3A;   /* PSMZ16 / PSMZ16S */
        D_00281DB0.zPageW = 0x40;
        D_00281DB0.zPageH = 0x40;
        D_00281DB0.zMax = 0xFFFF;
        break;
    case 3:
        D_00281DB0.zPsm = 0x31;        /* PSMZ24 */
        D_00281DB0.zPageW = 0x40;
        D_00281DB0.zPageH = 0x20;
        D_00281DB0.zMax = 0xFFFF28;
        break;
    case 4:
    default:
        if (D_00281DB0.fbDepth == 2) {
            D_00281DB0.fbPsm = 0xA;    /* PSMCT16S */
        }
        D_00281DB0.zPsm = 0x30;        /* PSMZ32 */
        D_00281DB0.zPageW = 0x40;
        D_00281DB0.zPageH = 0x20;
        D_00281DB0.zMax = 0xFFFF28;
        break;
    }
    D_00281DB0.zbp = func_0019BF60(zDepth * (D_00281DB0.zPageW * ((width + D_00281DB0.zPageW - 1) / (u32)D_00281DB0.zPageW)
                                             * (D_00281DB0.zPageH * ((height + D_00281DB0.zPageH - 1) / (u32)D_00281DB0.zPageH))))
                     & 0xFFFF;
    D_00281DB0.unk3AC = 0;
    D_00281DB0.unk3B0 = 0;
    D_00282240.width = width;
    D_00281DB0.offsetX = 0x800 - (width >> 1);
    D_00282240.height = height;
    D_00281DB0.offsetY = 0x800 - (height >> 1);
    D_00282240.unk10 = 0;
    D_00282240.unk0 = 0;
    D_00282240.depth = D_00281DB0.fbDepth;
    D_00282240.stride = width * D_00281DB0.fbDepth;
    switch (D_00281DB0.fbDepth) {
    case 2:
        D_00282240.r.shift = 0;
        D_00282240.r.bits = 5;
        D_00282240.b.shift = 10;
        D_00282240.r.mask = 0x1F;
        D_00282240.g.shift = 5;
        D_00282240.a.shift = 15;
        D_00282240.b.bits = 5;
        D_00282240.g.bits = 5;
        D_00282240.b.mask = 0x1F;
        D_00282240.g.mask = 0x1F;
        D_00282240.a.bits = 1;
        D_00282240.a.mask = 1;
        break;
    case 3:
        D_00282240.a.bits = 0;
        D_00282240.r.bits = 8;
        D_00282240.b.shift = 16;
        D_00282240.r.mask = 0xFF;
        D_00282240.g.bits = 8;
        D_00282240.g.shift = 8;
        D_00282240.b.bits = 8;
        D_00282240.g.mask = 0xFF;
        D_00282240.b.mask = 0xFF;
        D_00282240.a.shift = 0;
        D_00282240.a.mask = 0;
        D_00282240.r.shift = 0;
        break;
    case 4:
        D_00282240.r.shift = 0;
        D_00282240.r.bits = 8;
        D_00282240.b.shift = 16;
        D_00282240.r.mask = 0xFF;
        D_00282240.a.shift = 24;
        D_00282240.g.bits = 8;
        D_00282240.g.shift = 8;
        D_00282240.g.mask = 0xFF;
        D_00282240.b.bits = 8;
        D_00282240.a.bits = 8;
        D_00282240.b.mask = 0xFF;
        D_00282240.a.mask = 0xFF;
        break;
    }
    func_001957E0();

    D_00281DB0.dispBuffer = 0;
    env0 = &D_00281C70[0];
    D_00281DB0.vsync = 1;
    env1 = &D_00281C70[1];
    func_0018C280((u32 *)env0, 9, 0, 0);
    func_0018C280((u32 *)env1, 9, 0, 0);
    env0->gifTag[1] = 0xE;
    env0->gifTag[0] = GIF_TAG_LO(8, 1, 1);
    env1->gifTag[0] = GIF_TAG_LO(8, 1, 1);
    env1->gifTag[1] = 0xE;
    fbw = (u64)(D_0023BAA0 / 64) << 16;
    env0->reg[0].addr = 0x4C;    /* FRAME_1 */
    env0->reg[1].addr = 0x4D;    /* FRAME_2 */
    env0->reg[0].data = env0->reg[1].data =
        ((u64)D_00281DB0.fbPsm << 24) | ((u64)(D_00281DB0.fbp[1] >> 5) | fbw) | ((u64)1 << 32);
    env1->reg[0].data = ((u64)D_00281DB0.fbPsm << 24) | ((u64)(D_00281DB0.fbp[0] >> 5) | fbw) | ((u64)1 << 32);
    env1->reg[1].data = ((u64)D_00281DB0.fbPsm << 24) | ((u64)(D_00281DB0.fbp[0] >> 5) | fbw) | ((u64)1 << 32);
    env1->reg[0].addr = 0x4C;
    env1->reg[1].addr = 0x4D;
    env0->reg[2].addr = 0x4E;    /* ZBUF_1 */
    env0->reg[3].addr = 0x4F;    /* ZBUF_2 */
    env0->reg[2].data = env0->reg[3].data = (u64)(D_00281DB0.zbp >> 5) | ((u64)D_00281DB0.zPsm << 24);
    env1->reg[2].data = (u64)(D_00281DB0.zbp >> 5) | ((u64)D_00281DB0.zPsm << 24);
    env1->reg[3].data = (u64)(D_00281DB0.zbp >> 5) | ((u64)D_00281DB0.zPsm << 24);
    env1->reg[2].addr = 0x4E;
    env1->reg[3].addr = 0x4F;
    env0->reg[4].addr = 0x18;    /* XYOFFSET_1 */
    env0->reg[5].addr = 0x19;    /* XYOFFSET_2 */
    env0->reg[4].data = env0->reg[5].data =
        (u64)(D_00281DB0.offsetX << 4) | ((u64)(D_00281DB0.offsetY << 4) << 32);
    env1->reg[4].data = (u64)(D_00281DB0.offsetX << 4) | ((u64)(D_00281DB0.offsetY << 4) << 32);
    env1->reg[5].data = (u64)(D_00281DB0.offsetX << 4) | ((u64)(D_00281DB0.offsetY << 4) << 32);
    env1->reg[4].addr = 0x18;
    env1->reg[5].addr = 0x19;
    if (D_00281DB0.fbDepth == 2) {
        env0->reg[6].data = 1;   /* DTHE: dither 16-bit output */
        env1->reg[6].data = 1;
    } else {
        env0->reg[6].data = 0;
        env1->reg[6].data = 0;
    }
    env0->reg[6].addr = 0x45;
    env1->reg[6].addr = 0x45;
    env0->reg[7].data = 1;       /* COLCLAMP */
    env1->reg[7].data = 1;
    env0->reg[7].addr = 0x46;
    env1->reg[7].addr = 0x46;

    /* full-screen clear: a sprite drawn with Z test "always", then the drawing state restored */
    func_0018C280((u32 *)&D_00281B30, 0x13, 1, 0);
    D_00281B30.vif[0] = 0x13000000;           /* FLUSHA */
    D_00281B30.vif[1] = 0x51000013;           /* DIRECTHL 0x13 quadwords */
    D_00281B30.gifTag[0] = GIF_TAG_LO(18, 1, 1);
    D_00281B30.gifTag[1] = 0xE;
    D_00281B30.reg[0].data = 0;
    D_00281B30.reg[0].addr = 0x4A;            /* FBA_1 */
    D_00281B30.reg[1].data = 0;
    D_00281B30.reg[1].addr = 0x40;            /* SCISSOR_1 */
    D_00281B30.reg[2].data = 0x30003;
    D_00281B30.reg[2].addr = 0x47;            /* TEST_1 */
    D_00281B30.reg[3].data = 6;
    D_00281B30.reg[3].addr = 0;               /* PRIM: sprite */
    D_00281B30.reg[4].data = 0;
    D_00281B30.reg[4].addr = 1;               /* RGBAQ */
    D_00281B30.reg[5].data = 0;
    D_00281B30.reg[5].addr = 5;               /* XYZ2 */
    D_00281B30.reg[6].data = 0;
    D_00281B30.reg[6].addr = 5;               /* XYZ2 */
    D_00281B30.reg[7].data = 0;
    D_00281B30.reg[7].addr = 0x40;            /* SCISSOR_1 */
    D_00281B30.reg[8].data = 0;
    D_00281B30.reg[8].addr = 0x41;            /* SCISSOR_2 */
    D_00281B30.reg[9].data = 0;
    D_00281B30.reg[9].addr = 0x47;            /* TEST_1 */
    D_00281B30.reg[10].data = 0;
    D_00281B30.reg[10].addr = 0x48;           /* TEST_2 */
    D_00281B30.reg[11].data = 0;
    D_00281B30.reg[11].addr = 0x4E;           /* ZBUF_1 */
    D_00281B30.reg[12].data = 0;
    D_00281B30.reg[12].addr = 0x4F;           /* ZBUF_2 */
    D_00281B30.reg[13].data = 0;
    D_00281B30.reg[13].addr = 0x4A;           /* FBA_1 */
    D_00281B30.reg[14].data = 0;
    D_00281B30.reg[14].addr = 0x4B;           /* FBA_2 */
    D_00281B30.reg[15].data = 0;
    D_00281B30.reg[15].addr = 0x49;           /* PABE */
    D_00281B30.reg[16].data = 1;
    D_00281B30.reg[16].addr = 0x1A;           /* PRMODECONT */
    D_00281B30.reg[17].data = 0x6071243571603524;
    D_00281B30.reg[17].addr = 0x44;           /* DIMX: the standard dither matrix */
    sceGsSyncVCallback(func_0019EC00);
}
#endif

extern s32 D_0023BA18;
void func_0019F930(s32 buffer, s32 inInterrupt);
s32 func_001A8470(void);

/* V-blank handler: flips to the finished frame buffer once the frame's wait has passed. */
s32 func_0019EC00(s32 cause) {
    D_00281DB0.vcount++;
    D_0023BA18++;
    if (D_00281DB0.vcount > D_00281DB0.swapWait && D_00281DB0.swapRequest == 1) {
        D_00281DB0.swapRequest = 0;
        D_00281DB0.swapWait = D_00281DB0.nextSwapWait;
        D_00281DB0.dispBuffer ^= 1;
        func_0019F930(D_00281DB0.dispBuffer, 0);
    }
    func_001A8470();
    SYNC_EI();
}

s32 func_0018D0E0(void);

#define DISPFB(fbp, fbw, psm) ((u64)((fbp) >> 5) | ((u64)(fbw) << 9) | ((u64)(psm) << 15))
#define DISPLAY(dx, dy, magh, dw, dh) \
    ((u64)(dx) | ((u64)(dy) << 12) | ((u64)(magh) << 23) | ((u64)(dw) << 32) | ((u64)(dh) << 44))

/* Not matching yet: first draft. */
#ifdef NON_MATCHING
/* Show frame buffer `buffer`: send its drawing environment and point the video output at it. */
void func_0019F930(s32 buffer, s32 inInterrupt) {
    GsEnvPacket *env = &D_00281C70[buffer];
    s32 dx;
    s32 dy;

    if (D_00281DB0.field != 1) {
        if (D_00281DB0.vsync == 1) {
            env->reg[4].data = (u64)(D_00281DB0.offsetX << 4) | ((u64)(D_00281DB0.offsetY << 4) << 32);
            env->reg[5].data = (u64)(D_00281DB0.offsetX << 4) | ((u64)(D_00281DB0.offsetY << 4) << 32);
        } else {
            /* odd field: half a line down */
            env->reg[4].data = (u64)(D_00281DB0.offsetX << 4) | ((u64)((D_00281DB0.offsetY << 4) + 8) << 32);
            env->reg[5].data = (u64)(D_00281DB0.offsetX << 4) | ((u64)((D_00281DB0.offsetY << 4) + 8) << 32);
        }
    }
    if (inInterrupt == 0) {
        iFlushCache(0);
    } else {
        FlushCache(0);
    }
    func_0018D0E0();
    sceDmaSend(D_00281DB0.dmaChan[2], env);
    while (sceDmaSync(D_00281DB0.dmaChan[2], 1, 0) != 0) {
    }
    dx = D_00281DB0.dispX + D_00281DB0.dispAdjX * D_00281DB0.magH;
    dy = D_00281DB0.dispY + D_00281DB0.dispAdjY;
    if (D_00281DB0.field == 0) {
        HW_REG64(GS_SMODE2) = 3;
        HW_REG64(GS_PMODE) = 0x66;
        HW_REG64(GS_DISPFB2) = DISPFB(buffer == 0 ? D_00281DB0.fbp[0] : D_00281DB0.fbp[1], D_0023BAA0 / 64,
                                      D_00281DB0.fbPsm);
        HW_REG64(GS_DISPLAY2) = DISPLAY(dx, dy, D_00281DB0.magH, 0x9FF, D_00281DB0.dispHeight - 1);
    } else if (D_00281DB0.blendFields == 0) {
        HW_REG64(GS_SMODE2) = 1;
        HW_REG64(GS_PMODE) = 0x7F27;
        HW_REG64(GS_DISPFB1) = DISPFB(buffer == 0 ? D_00281DB0.fbp[0] : D_00281DB0.fbp[1], D_0023BAA0 / 64,
                                      D_00281DB0.fbPsm);
        HW_REG64(GS_DISPFB2) = DISPFB(buffer == 0 ? D_00281DB0.fbp[0] : D_00281DB0.fbp[1], D_0023BAA0 / 64,
                                      D_00281DB0.fbPsm);
        HW_REG64(GS_DISPLAY1) = DISPLAY(dx, dy + 1, D_00281DB0.magH, 0x9FF, D_00281DB0.dispHeight - 1);
        HW_REG64(GS_DISPLAY2) = DISPLAY(dx, dy, D_00281DB0.magH, 0x9FF, D_00281DB0.dispHeight - 1);
    } else {
        HW_REG64(GS_SMODE2) = 1;
        HW_REG64(GS_PMODE) = 0x66;
        HW_REG64(GS_DISPFB2) = DISPFB(buffer == 0 ? D_00281DB0.fbp[0] : D_00281DB0.fbp[1], D_0023BAA0 / 64,
                                      D_00281DB0.fbPsm);
        HW_REG64(GS_DISPLAY2) = DISPLAY(dx, dy, D_00281DB0.magH, 0x9FF, D_00281DB0.dispHeight - 1);
    }
    HW_REG64(GS_EXTWRITE) = 0;
}
#endif
