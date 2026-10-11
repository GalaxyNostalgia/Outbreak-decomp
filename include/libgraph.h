#ifndef LIBGRAPH_H
#define LIBGRAPH_H

#include "common.h"

/* GS (Graphics Synthesizer) register layouts and the double-buffer/upload packets the
   Sony graphics library builds. Layouts follow the GS registers; the offsets match what
   the game's library code reads and writes (see func_00112E38 sceGsSetDefDBuff). */

typedef unsigned long long u64;

typedef struct { u64 FBP:9; u64 FBW:6; u64 PSM:5; u64 pad0:12; u64 DBX:11; u64 DBY:11; u64 pad1:10; } sceGsDispfb;
typedef struct { u64 FBP:9; u64 pad0:7; u64 FBW:6; u64 pad1:2; u64 PSM:6; u64 pad2:2; u64 FBMSK:32; } sceGsFrame;

typedef struct {            /* 0x28 bytes */
    u64 pmode;
    u64 smode2;
    sceGsDispfb dispfb;     /* 0x10 */
    u64 display;
    u64 bgcolor;
} sceGsDispEnv;

typedef struct {            /* 0x80 bytes: 8 registers, each followed by its address (A+D) */
    sceGsFrame frame1;  u64 frame1addr;
    u64 zbuf1;          u64 zbuf1addr;
    u64 xyoffset1;      u64 xyoffset1addr;
    u64 scissor1;       u64 scissor1addr;
    u64 prmodecont;     u64 prmodecontaddr;
    u64 colclamp;       u64 colclampaddr;
    u64 dthe;           u64 dtheaddr;
    u64 test1;          u64 test1addr;
} sceGsDrawEnv1;

typedef struct { u64 data[12]; } sceGsClear;   /* 0x60 bytes */
typedef struct { u64 tag[2]; } sceGifTag;      /* 0x10 bytes */

typedef struct {            /* 0x230 bytes */
    sceGsDispEnv disp[2];   /* 0x000 */
    sceGifTag giftag0;      /* 0x050 */
    sceGsDrawEnv1 draw0;    /* 0x060 */
    sceGsClear clear0;      /* 0x0E0 */
    sceGifTag giftag1;      /* 0x140 */
    sceGsDrawEnv1 draw1;    /* 0x150 */
    sceGsClear clear1;      /* 0x1D0 */
} sceGsDBuff;

typedef struct {            /* 0x60 bytes: GIF packet for a host->VRAM transfer */
    sceGifTag giftag0;
    u64 bitbltbuf; u64 bitbltbufaddr;
    u64 trxpos;    u64 trxposaddr;
    u64 trxreg;    u64 trxregaddr;
    u64 trxdir;    u64 trxdiraddr;
    sceGifTag giftag1;
} sceGsLoadImage;

void sceGsResetGraph(s16 mode, s16 inter, s16 omode, s16 ffmode);
void sceGsResetPath(void);
void sceGsSetDefDBuff(sceGsDBuff *db, s16 psm, s16 w, s16 h, s16 ztest, s16 zpsm, s16 clear);
s32 sceGsSwapDBuff(sceGsDBuff *db, s32 id);
s32 sceGsSyncV(s32 mode);
s32 sceGsSyncPath(s32 mode, u16 timeout);
s32 sceGsSetDefLoadImage(sceGsLoadImage *lp, s16 dbp, s16 dbw, s16 dpsm, s16 x, s16 y, s16 w, s16 h);
s32 sceGsExecLoadImage(sceGsLoadImage *lp, void *src);

void sceDevVif0Reset(void);
void sceDevVif1Reset(void);
void sceDevVu0Reset(void);
void sceDevVu1Reset(void);
void sceDmaReset(s32 mode);
s32 FlushCache(s32 op);
void *memset(void *dst, s32 c, u32 n);

#endif
