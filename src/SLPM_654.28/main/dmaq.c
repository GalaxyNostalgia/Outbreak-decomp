#include "system.h"
#include "kernel.h"
#include "sce.h"

extern s32 D_0023BA70;
s32 func_0018C830(s32 cause);

void func_0018C8E0(DmaQueue *q, s32 size, s32 (*handler)(s32)) {
    s32 i;

    q->size = size;
    for (i = 0; i < 2; i++) {
        q->handle[i] = func_0018D6D0(size * 4, 1);
        q->count[i] = 0;
        q->wr[i] = 0;
        q->rd[i] = 0;
    }
    q->handlerId = AddDmacHandler(q->channel, handler, 0);
    q->flags = 0;
    EnableDmac(q->channel);
    D_0023BA70 = AddIntcHandler(0, func_0018C830, 0);
}

/* Not matching yet: register allocation (10 instructions). */
#ifdef NON_MATCHING
/* Queue a packet for the frame being built. */
s32 func_0018C9A0(u32 entry, DmaQueue *q) {
    u32 *list;
    s32 b = D_00281DB0.bufIndex;

    if (q->size - 1 >= q->count[b]) {
        list = (u32 *)func_0018D720(q->handle[b]);
        q->count[b]++;
        list[q->wr[b]++] = entry;
        if (q->wr[b] == q->size) {
            q->wr[b] = 0;
        }
        q->lastTag = 0;
        q->lastPtr = NULL;
        return 0;
    }
    return -1;
}
#endif

/* Queue a packet, or chain it onto the previous one when that one can be extended: an END tag becomes a NEXT
   to the new packet, a REFE becomes a REF. mode 1: the packet can't be extended itself; mode 2: never chain. */
/* Not matching yet: first draft, block order differs. */
#ifdef NON_MATCHING
s32 func_0018CA60(s32 mode, u32 entry, u32 *tag, DmaQueue *q) {
    s32 b;
    u32 *list;
    u32 t;

    if (mode != 2 && q->lastTag != 0 && q->lastPtr != NULL) {
        switch (q->lastTag >> 28) {
        case 0:
        case 3:
        case 4:
        case 5:
            t = q->lastPtr[0];
            if ((t & 0x70000000) != 0x70000000) {
                break;
            }
            q->lastPtr[0] = (t & 0xFFFF) + 0x20000000;
            q->lastPtr[1] = entry & 0x0FFFFFFF;
            goto chained;
        case 1:
            t = q->lastPtr[0];
            if ((t & 0x70000000) != 0) {
                break;
            }
            q->lastPtr[0] = (t & 0xFFFF) + 0x30000000;
        chained:
            if (mode == 1) {
                q->lastTag = 0;
                q->lastPtr = NULL;
            } else {
                q->lastTag = entry;
                q->lastPtr = tag;
            }
            return 0;
        }
    }
    b = D_00281DB0.bufIndex;
    if (q->size - 1 < q->count[b]) {
        q->lastTag = 0;
        q->lastPtr = NULL;
        return -1;
    }
    list = (u32 *)func_0018D720(q->handle[b]);
    q->count[b]++;
    list[q->wr[b]++] = entry;
    if (q->wr[b] == q->size) {
        q->wr[b] = 0;
    }
    q->lastTag = entry;
    q->lastPtr = tag;
    return 0;
}
#endif

/* Sending the queue: on each DMA-end interrupt the next packet of the frame being drawn goes out, until the
   queue is empty. Type 2 entries read an image back from the GS: the GS "finish" interrupt starts that
   transfer (func_0018C830). */

#include "ps2mem.h"

extern u32 D_0023BA00;          /* image read-back: destination */
extern u32 D_0023BA04;          /* image read-back: quadwords */
extern u64 D_0023BA08;          /* GS IMR saved during a read-back */
extern u32 D_00212370[10];      /* each DMA channel's CHCR register address */
extern u128 D_00212360;         /* VIF code MSKPATH3 0 */

#define VIF1_STAT 0x10003C00
#define VIF1_FIFO 0x10005000
#define GS_INTC 0

/* GS finish interrupt: start reading the image back. */
s32 func_0018C830(s32 cause) {
    sceDmaChan *chan;

    if (D_0023BA04 != 0) {
        chan = D_00281DB0.dmaChan[1];
        HW_REG32(VIF1_STAT) = 0x800000;     /* FDR: VIF1 FIFO from GS to memory */
        HW_REG64(GS_BUSDIR) = 1;
        iDisableIntc(GS_INTC);
        chan->chcr.TTE = 1;
        chan->chcr.TIE = 0;
        sceDmaRecvN(chan, (void *)D_0023BA00, D_0023BA04);
        D_0023BA00 = 0;
        D_0023BA04 = 0;
    }
    SYNC_EI();
    return 0;
}

/* DMA-end interrupt of the queue's channel, and the kick. Not matching: written with a shared helper. */
#ifdef NON_MATCHING
static void send_entry(sceDmaChan *chan, u32 entry, s32 inInterrupt) {
    u32 *packet = (u32 *)(entry & 0x0FFFFFFF);

    switch (entry >> 28) {
    case 0:
    case 1:
    case 3:
    case 4:
    case 5:
        chan->chcr.TTE = 1;
        chan->chcr.TIE = 1;
        if (!inInterrupt) {
            FlushCache(0);
        }
        sceDmaSend(chan, packet);
        break;
    case 2:
        D_0023BA00 = packet[0x20];
        D_0023BA04 = packet[0x21];
        HW_REG64(GS_CSR) = 2;               /* clear FINISH */
        D_0023BA08 = HW_REG64(GS_IMR);
        HW_REG64(GS_IMR) = ~(u64)0x200;     /* let FINISH through */
        if (inInterrupt) {
            iEnableIntc(GS_INTC);
        } else {
            EnableIntc(GS_INTC);
        }
        D_00281AF0.flags |= 1;
        chan->chcr.TTE = 0;
        chan->chcr.TIE = 0;
        if (!inInterrupt) {
            FlushCache(0);
        }
        sceDmaSend(chan, packet);
        break;
    }
}

s32 func_0018CC10(s32 channel) {
    DmaQueue *q = &D_00281AF0;
    u32 chcr;
    s32 b;
    u32 *list;
    u32 entry;

    if (channel == 1) {
        chcr = HW_REG32(D_00212370[q->channel]);
        if (chcr & 0x100) {
            goto done;                      /* still running */
        }
        if ((chcr & 0xC) == 4) {            /* chain mode: only the tag that asked for an interrupt */
            if (!(chcr & 0x80) || !(chcr & 0x80000000)) {
                goto done;
            }
        } else if ((chcr & 0xC) != 0 || !(q->flags & 1)) {
            goto done;
        }
        b = D_00281DB0.bufIndex ^ 1;
        list = (u32 *)func_0018D720(q->handle[b]);
        q->count[b]--;
        entry = list[q->rd[b]++];
        if (q->rd[b] == q->size) {
            q->rd[b] = 0;
        }
        if ((entry >> 28) == 2) {           /* a read-back finished: back to normal */
            q->flags = 0;
            HW_REG32(VIF1_STAT) = 0;
            HW_REG64(GS_BUSDIR) = 0;
            HW_REG64(GS_IMR) = D_0023BA08;
#ifndef PLATFORM_PC
            *(volatile u128 *)VIF1_FIFO = D_00212360;
#endif
        }
        if (q->wr[b] != q->rd[b]) {
            entry = list[q->rd[b]];
            q->current = entry;
            if ((entry & 0x0FFFFFFF) != 0) {
                send_entry(D_00281DB0.dmaChan[q->channel], entry, 1);
            }
        }
    }
done:
    SYNC_EI();
    return 0;
}

/* Start sending the frame just built (and switch to filling the other buffer). */
void func_0018CF10(void) {
    DmaQueue *q = &D_00281AF0;
    s32 b;
    u32 *list;

    q->lastTag = 0;
    q->lastPtr = NULL;
    b = D_00281DB0.bufIndex;
    D_00281DB0.bufIndex = b ^ 1;
    if (q->count[b] == 0) {
        return;
    }
    list = (u32 *)func_0018D720(q->handle[b]);
    q->current = list[q->rd[b]];
    send_entry(D_00281DB0.dmaChan[q->channel], q->current, 0);
}
#endif

/* Wait until the frame being drawn has been sent. */
s32 func_0018D0E0(void) {
    volatile u32 *count = &D_00281AF0.count[D_00281DB0.bufIndex ^ 1];

    while (*count != 0) {
    }
    while (sceDmaSync(D_00281DB0.dmaChan[1], 1, 0) != 0) {
    }
    return 1;
}

void func_0018CF10(void);

/* Finish everything queued: wait for the frame being drawn, then send and wait for the one being built. */
s32 func_0018D150(void) {
    if (D_00281AF0.count[0] == 0 && D_00281AF0.count[1] == 0) {
        return 0;
    }
    func_0018D0E0();
    if (D_00281AF0.count[D_00281DB0.bufIndex] == 0) {
        return 0;
    }
    func_0018CF10();
    func_0018D0E0();
    func_0018D7C0();
    return 1;
}
