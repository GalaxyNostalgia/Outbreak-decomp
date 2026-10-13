#include "common.h"

/* Headings between points. */

f32 func_00191B50(f32 y, f32 x);
u32 func_001918C0(f32 rad);
extern f32 D_00239CE4;      /* (a tiny x, so the heading of straight up/down is defined) */

/* The heading (0x10000 a turn) from a to b, in the x-z plane. */
s16 func_00346C50(const f32 *a, const f32 *b);
#ifdef NON_MATCHING   /* the original keeps d on the stack */
s16 func_00346C50(const f32 *a, const f32 *b) {
    f32 d[3];

    d[0] = b[0] - a[0];
    d[1] = b[1] - a[1];
    d[2] = b[2] - a[2];
    if (d[0] == 0.0f) {
        d[0] = D_00239CE4;
    }
    return func_001918C0(func_00191B50(d[0], d[2]));
}
#endif
