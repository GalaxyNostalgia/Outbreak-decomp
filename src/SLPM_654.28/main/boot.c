#include "libgraph.h"
#include "ps2mem.h"

/* Boot-time graphics setup, called first thing by main(): resets the vector units and
   DMA, clears video memory, sets up 640x448 double buffering and shows a small boot
   image (256x32, 16 colours) while the game loads. */

extern u32 D_00213720[];   /* 16-colour palette of the boot image */
extern u8 D_00213760[];    /* boot image: 256x32, 4 bits per pixel */

void func_001A85C0(void) {
    sceGsDBuff db;
    sceGsLoadImage gp;
    s32 i;
    u8 *src;
    u32 *pal;
    u32 *dst;

    sceDevVif0Reset();
    sceDevVif1Reset();
    sceDevVu0Reset();
    sceDevVu1Reset();
    sceGsResetPath();
    sceDmaReset(1);

    /* clear all 4 MiB of video memory, 1024x64 at a time, from a zeroed buffer */
    memset(PS2_RAM(0x01000000), 0, 0x40000);
    for (i = 0; i < 16; i++) {
        sceGsSetDefLoadImage(&gp, 0, 16, 0, 0, i * 64, 1024, 64);
        FlushCache(0);
        sceGsExecLoadImage(&gp, PS2_RAM(0x01000000));
        sceGsSyncPath(0, 0);
    }

    sceGsResetGraph(0, 1, 2, 0);
    for (i = 0; i < 20; i++) {
        sceGsSyncV(0);
    }
    sceGsSetDefDBuff(&db, 0, 640, 448, 0, 0, 1);
    db.disp[1].dispfb.FBP = db.draw0.frame1.FBP = 140;
    while ((sceGsSyncV(0) != 0) ^ 1) {
    }
    sceGsSyncPath(0, 0);

    /* expand the boot image to 32-bit colour and show it */
    pal = D_00213720;
    src = D_00213760;
    dst = PS2_RAM(0x01000000);
    for (i = 0; i < 0x1000; i++) {
        u8 c = *src++;
        dst[0] = pal[(u8)(c >> 4)];
        dst[1] = pal[c & 0xF];
        dst += 2;
    }
    sceGsSetDefLoadImage(&gp, 0, 10, 0, 16, 400, 256, 32);
    FlushCache(0);
    sceGsSwapDBuff(&db, 1);
    sceGsExecLoadImage(&gp, PS2_RAM(0x01000000));
    sceGsSyncV(0);
    FlushCache(0);
    sceGsSyncPath(0, 0);
    sceGsSwapDBuff(&db, 0);
}
