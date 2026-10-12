#include "system.h"
#include "ps2mem.h"
#include "drawlist.h"
#include "options.h"

/* Sending the frame's packet lists. */

void func_00195970(u8 cmd, ...);
s32 func_0018C9A0(u32 entry, DmaQueue *q);
void func_001B4390();    /* (s32 alt); called once with a stray second argument */
void func_6AC010(void);          /* GAME.BIN */
s32 func_0018D0E0(void);
void func_001AE9B0(void);

#define D2_CHCR 0x1000A000       /* DMA channel 2 (GIF) */

/* Close the main list and queue it, then flip its buffers. */
void func_001B6210(void) {
    DrawList *dl;
    u32 *p;

    if (D_00306590.unk16 != 0 || D_00306590.unk2A != 0) {
        dl = &D_0029CD80;
        if (dl->count != 0) {
            func_00195970(0x5F, 5);
            func_00195970(0x60, 0);
            if (dl->unkA0 != 0) {
                func_00195970(0x6C, 1);
            } else {
                func_00195970(0x6C, 0);
            }
            p = dl->ptr;
            p[0] = 0xF0000000;
            p[1] = 0;
            p[2] = 0;
            p[3] = 0;
            func_0018C9A0(dl->head[dl->cur] & 0x0FFFFFFF, &D_00281AF0);
        }
        dl->cur ^= 1;
        dl->ptr = dl->buf[dl->cur];
        dl->lastCount = dl->count;
        dl->count = 0;
    }
}

#ifdef NON_MATCHING   /* the end tag's constant goes in v0, the original's in v1 */
/* Build the second list (GAME.BIN fills it) and close it. */
void func_001B4730(void) {
    DrawList *gl = &D_0029CCD0;
    u32 *p;

    if (D_0029CD80.unk88 == 0) {
        D_0029CD80.unk88++;
    }
    func_001B4390(1);
    func_6AC010();
    p = gl->ptr;
    p[0] = 0xF0000000;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    func_001B4390(0, p);
}
#endif

/* Send the frame's lists. When the second list is on, it goes straight to the GIF now and the main
   list waits. */
void func_001B4C50(void) {
    DrawList *l;
    u32 *p;

    l = &D_0029CD80;
    if (D_0029CD80.unk0 != 0) {
        l->unk50 = l->unk9C;
        goto flip;
    }
    if (l->mode == 7) {
        func_001B6210();
        return;
    }
    func_00195970(0x5F, 5);
    func_00195970(0x60, 0);
    func_00195970(0x6C, 0);
    if (D_00306590.unk16 == 0 && D_00306590.unk2A == 0) {
        return;
    }
    if (l->unk88 > 0) {
        if (D_00306590.unk2A != 0) {
            l = &D_0029CCD0;
            if (D_00281AF0.count[0] != 0 || D_00281AF0.count[1] != 0) {
                func_0018D0E0();
            }
            func_001B4730();
            D_00281DB0.dmaChan[2]->chcr.TTE = 1;
            D_00281DB0.dmaChan[2]->chcr.TIE = 1;
            FlushCache(0);
            while (HW_REG32(D2_CHCR) & 0x100) {
            }
            sceDmaSend(D_00281DB0.dmaChan[2], (void *)l->head[l->cur]);
            l->cur ^= 1;
            l->ptr = l->buf[l->cur];
            l->lastCount = l->count;
            l->count = 0;
            return;
        }
        if (l->count != 0) {
            p = l->ptr;
            p[0] = 0xF0000000;
            p[1] = 0;
            p[2] = 0;
            p[3] = 0;
            func_0018C9A0(l->head[l->cur] & 0x0FFFFFFF, &D_00281AF0);
            func_00195970(0x6C, 1);
        }
    flip:
        l->cur ^= 1;
        l->ptr = l->buf[l->cur];
        l->lastCount = l->count;
        l->count = 0;
        func_001AE9B0();
        return;
    }
    if (D_00306590.unk2A != 0) {
        l = &D_0029CCD0;
        func_001B4730();
        D_00281DB0.dmaChan[2]->chcr.TTE = 1;
        D_00281DB0.dmaChan[2]->chcr.TIE = 1;
        FlushCache(0);
        while (HW_REG32(D2_CHCR) & 0x100) {
        }
        sceDmaSend(D_00281DB0.dmaChan[2], (void *)l->head[l->cur]);
        l->cur ^= 1;
        l->ptr = l->buf[l->cur];
        l->lastCount = l->count;
        l->count = 0;
    } else if (D_0029CD80.unk88 == 0) {
        D_0029CD80.unk88++;
    }
    func_001AE9B0();
}
