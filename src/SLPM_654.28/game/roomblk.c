#include "common.h"

/* GAME.BIN: a room data block of three tables: {n0, n1, n2} then n0 and n1 8-byte entries and n2
   words. */

typedef struct {
    s32 n0;
    s32 n1;
    s32 n2;
    u8 data[1];         /* 0x0C */
} RoomBlk;

typedef struct {
    u8 unk0[8];
    u8 *t0;             /* 0x08 */
    u8 *t1;             /* 0x0C */
    u8 *t2;             /* 0x10 */
} RoomBlkRef;

void func_00107B50(void *dst, const void *src, s32 n);

/* Copies block src's tables to dst and points out at them. */
void func_0055FB60(RoomBlk *src, RoomBlkRef *out, u8 *dst);
#ifdef NON_MATCHING   /* a reload of out->t0 */
void func_0055FB60(RoomBlk *src, RoomBlkRef *out, u8 *dst) {
    s32 n0 = src->n0;
    s32 n1 = src->n1;

    func_00107B50(dst, src->data, n0 * 8 + n1 * 8 + src->n2 * 4);
    out->t0 = dst;
    out->t1 = out->t0 + n0 * 8;
    out->t2 = out->t1 + n1 * 8;
}
#endif
