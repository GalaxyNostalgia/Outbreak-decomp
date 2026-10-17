#include "common.h"

/* GAME.BIN: the room's route nodes (32 at most, from the room data): each with up to four
   neighbours, and a matrix (n by n bytes) for routing between them. */

typedef struct {
    u8 used;            /* 0x00 */
    u8 nlinks;          /* 0x01 */
    s16 unk2;           /* 0x02 */
    f32 pos[3];         /* 0x04 */
    u8 unk10[4];
    s8 links[4];        /* 0x14: -1: none */
    u8 unk18[0x40 - 0x18];
} NavNode;              /* 0x40 */

typedef struct {
    s32 used;           /* 0x00 */
    f32 pos[3];         /* 0x04 */
    s32 links[4];       /* 0x10: -1: none */
    s32 unk20;
    s32 unk24;          /* 0x24 */
} NavNodeDef;

extern NavNode D_00394F20[32];
extern u8 D_0070F140[32 * 32];
extern s32 D_0023BED8;          /* nodes */
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *d, const void *s, u32 n);

void func_00601F60(void);     /* (pools.c) clears the nodes */

/* Takes the nodes in f into nodes (and the n by n matrix after them into mat); returns how many
   there are. */
s32 func_00602040(s32 *f, s32 unused, NavNode *nodes, s32 n, u8 *mat) {
    s32 cnt;
    s32 *off;
    u8 *base;
    u8 *m;
    u32 i;
    u32 j;
    s32 k;
    NavNodeDef *d;

    m = mat;
    off = f;
    cnt = *off++;
    if (cnt == 0) {
        return 0;
    }
    base = (u8 *)f;
    for (i = 0; i < cnt; i++, nodes++) {
        d = (NavNodeDef *)(base + *off++);
        if (d->used != 0) {
            nodes->used = 1;
            *(Vec3 *)nodes->pos = *(Vec3 *)d->pos;
            nodes->unk2 = d->unk24;
            for (j = 0, k = 0; j < 4; j++) {
                if (d->links[j] >= 0) {
                    nodes->links[k] = d->links[j];
                    k++;
                }
            }
            nodes->nlinks = k;
        }
    }
    memset(m, 0xFF, n * n);
    memcpy(m, base + *off, n * n);
    return cnt;
}

/* Sets the room's nodes up from f. */
void func_00601FE0(s32 *f, s32 x) {
    func_00601F60();
    D_0023BED8 = func_00602040(f, x, D_00394F20, 32, D_0070F140);
}
