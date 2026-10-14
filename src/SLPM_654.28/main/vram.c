#include "system.h"


TexEntry *func_0019E140(void) {
    s32 i;

    for (i = 0; i < 0x200; i++) {
        if (D_0027E2F0[i].addr == 0) {
            D_0023BA68++;
            return &D_0027E2F0[i];
        }
    }
    return NULL;
}

void func_0019E1A0(TexEntry *e) {
    TexEntry *prev = e->prev;
    TexEntry *next = e->next;

    if (prev != NULL) {
        prev->next = next;
    } else if (next != NULL) {
        D_0023BA6C = next;
    } else {
        D_0023BA6C = NULL;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    if (e->tex != NULL) {
        e->tex->inVram = 0;
        e->tex->owner = 4;
    }
    func_0018D600(e, 0, sizeof(TexEntry));
    D_0023BA68--;
}

/* The VRAM allocation holding texture t, if it's in VRAM. */
TexEntry *func_0019E270(FlTexture *t) {
    TexEntry *e;

    for (e = D_0023BA6C; e != NULL; e = e->next) {
        if (e->tex == t) {
            return e;
        }
    }
    return NULL;
}

/* Take texture t out of VRAM. */
s32 func_0019E4F0(FlTexture *t) {
    TexEntry *e = func_0019E270(t);

    if (e != NULL) {
        func_0019E1A0(e);
    }
    return 1;
}

s32 func_0019E4B0(void) {
    while (D_0023BA6C != NULL) {
        func_0019E1A0(D_0023BA6C);
    }
    return 1;
}

u32 func_0019BF60(u32 size) {
    s32 i;
    u32 blocks;
    VramBlock *b;

    if (D_0023BA60 >= 6) {
        return 0;
    }
    func_0019E4B0();
    blocks = (((size + 0xFF) >> 8) + 0x1F) & ~0x1F;
    for (b = D_0027E2C0, i = 0; i < 6; b++, i++) {
        if (b->used == 0) {
            break;
        }
    }
    b->used = 1;
    b->addr = D_00281DB0.vramNext;
    b->size = blocks;
    D_0023BA60++;
    D_00281DB0.vramNext += blocks;
    return b->addr;
}

/* Frees the block at addr: blocks are a stack, so the allocator goes back to it. */
void func_0019C020(u32 addr) {
    s32 i;
    VramBlock *b;

    for (b = D_0027E2C0, i = 0; i < 0x200; b++, i++) {
        if (*(s16 *)&b->addr == (u16)addr) {
            break;
        }
    }
    if (i != 6) {
        func_0019E4B0();
        func_0018D600(b, 0, sizeof(VramBlock));
        D_0023BA60--;
        D_00281DB0.vramNext = (u16)addr;
    }
}
