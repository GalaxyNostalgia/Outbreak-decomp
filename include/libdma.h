#ifndef LIBDMA_H
#define LIBDMA_H

#include "common.h"

/* Sony DMA library (libdma). */

typedef struct {
    u32 DIR : 1;
    u32 pad0 : 1;
    u32 MOD : 2;
    u32 ASP : 2;
    u32 TTE : 1;
    u32 TIE : 1;
    u32 STR : 1;
    u32 pad1 : 7;
    u32 TAG : 16;
} tD_CHCR;

typedef struct {
    tD_CHCR chcr;
    u32 pad0[3];
    void *madr;
    u32 pad1[3];
    u32 qwc;
    u32 pad2[3];
    void *tadr;
    u32 pad3[3];
    void *as0;
    u32 pad4[3];
    void *as1;
    u32 pad5[3];
    u32 pad6[8];
    void *sadr;
    u32 pad7[3];
} sceDmaChan;

void sceDmaReset(s32 mode);
sceDmaChan *sceDmaGetChan(s32 id);
void sceDmaSend(sceDmaChan *chan, void *tag);
void sceDmaRecvN(sceDmaChan *chan, void *addr, s32 qwc);
s32 sceDmaSync(sceDmaChan *chan, s32 mode, s32 timeout);

#endif
