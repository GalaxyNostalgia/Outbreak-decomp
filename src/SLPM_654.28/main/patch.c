#include "sce.h"
#include "kernel.h"
#include "ps2mem.h"

/* Patches for the overlays: a block of {overlay, address, bytes} records that is written over an
   overlay's code or data right after it loads. The block is checked against this executable's key. */

typedef struct {
    char key[8];
    s32 count;
} PatchHeader;

typedef struct {
    char name[8];
    u8 pad8[0x14 - 8];
    s32 loaded;         /* 0x14 */
} PatchSet;

extern PatchHeader D_0032C580;   /* followed by `count` records */
extern char D_00239B98[];        /* the key patches for this executable carry */
extern char D_002F87F0[8];       /* name of the patch set in use */
extern PatchSet D_00334580;

/* A record: s32 overlay id, u32 address, s32 size, then size bytes (or, when the size's top bit is
   set, a pointer to them), padded to 4 bytes. */
typedef struct {
    s32 ovl;
    u32 addr;
    s32 size;
} PatchRec;

#ifdef NON_MATCHING   /* register allocation: the original keeps both blocks' addresses in saved registers */
void func_001D9810(s32 id) {
    s32 n;
    u8 *p;
    s32 i;
    s32 j;
    u8 *dst;
    u8 *src;
    s32 size;
    s32 ovl;
    PatchSet *set = &D_00334580;
    PatchHeader *h;

    if (set->loaded == 0) {
        return;
    }
    h = &D_0032C580;
    n = h->count;
    p = (u8 *)(h + 1);
    if (strncmp(h->key, D_00239B98, 8) != 0) {
        return;
    }
    if (n != 0) {
        if (strncmp(D_002F87F0, set->name, 8) < 0) {
            strncpy(D_002F87F0, set->name, 8);
        }
    }
    for (i = 0; i < n; i++) {
        ovl = ((PatchRec *)p)->ovl;
        dst = PS2_RAM(((PatchRec *)p)->addr);
        size = ((PatchRec *)p)->size;
        p += sizeof(PatchRec);
        if (id == ovl) {
            if (size & 0x80000000) {
                src = *(u8 **)p;
                p += 4;
                for (j = 0; j < (size & 0x7FFFFFFF); j++) {
                    *dst++ = *src++;
                }
            } else {
                for (j = 0; j < size; j++) {
                    *dst++ = *p++;
                }
            }
        } else if (size & 0x80000000) {
            p += 4;
        } else {
            p += size;
        }
        p = (u8 *)((s32)(p + 3) / 4 * 4);
    }
    FlushCache(0);
}
#endif
