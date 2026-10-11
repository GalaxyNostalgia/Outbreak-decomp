#include "system.h"

/* A movable-block allocator. Allocations are handles (index + 1); the blocks are kept in a list sorted by
   address, and when memory runs out the blocks are slid together (their addresses change, the handles
   don't). The game manages a 14 MiB area of its heap with it. */
#define NO_BLOCK 0xFFFF

typedef struct {
    u32 addr;    /* 0x0 */
    u32 size;    /* 0x4: 0 when the slot is free */
    s16 align;   /* 0x8 */
    s16 unkA;    /* 0xA */
    u16 prev;    /* 0xC */
    u16 next;    /* 0xE */
} MemBlock;

typedef struct {
    s32 count;         /* 0x00: number of slots */
    u32 size;          /* 0x04 */
    MemBlock *blocks;  /* 0x08 */
    s32 upwards;       /* 0x0C: 1: blocks grow from base up, 0: from base down */
    u32 cur;           /* 0x10: next free address */
    u32 base;          /* 0x14 */
    s32 align;         /* 0x18 */
    u32 used;          /* 0x1C */
    u32 reserved;      /* 0x20 */
    s32 head;          /* 0x24: lowest (upwards) or highest block */
} BlockMem;

void func_001A5030(void *dst, s32 value, u32 size);
void func_001A5060(u32 dst, u32 src, u32 size);
u32 func_001A0B60(BlockMem *mem);
s32 func_001A0BB0(BlockMem *mem);
void func_001A0C20(BlockMem *mem, s32 i);
void func_001A0D50(BlockMem *mem, s32 i);
s32 func_001A03C0(BlockMem *mem, u32 size);
s32 func_001A03D0(BlockMem *mem, u32 size, s32 align);
u32 func_001A09A0(BlockMem *mem);

void func_001A0350(BlockMem *mem, MemBlock *blocks, s32 count, u32 base, u32 size, s32 align, s32 upwards) {
    mem->count = count;
    mem->blocks = blocks;
    mem->size = size;
    mem->upwards = upwards;
    mem->align = align;
    if (upwards) {
        mem->base = ~(align - 1) & (base + align - 1);
    } else {
        mem->base = ~(align - 1) & (base + size);
    }
    mem->cur = mem->base;
    mem->used = 0;
    mem->reserved = 0;
    mem->head = NO_BLOCK;
    func_001A5030(blocks, 0, count * 16);
}

s32 func_001A03C0(BlockMem *mem, u32 size) {
    return func_001A03D0(mem, size, mem->align);
}

/* Not matching yet: operand order of one add. */
#ifdef NON_MATCHING
/* Allocate at the free end. */
s32 func_001A03D0(BlockMem *mem, u32 size, s32 align) {
    s32 i;

    if (func_001A0B60(mem) <= size + align) {
        return 0;
    }
    if (size == 0) {
        return 0;
    }
    i = func_001A0BB0(mem);
    if (i == NO_BLOCK) {
        return 0;
    }
    mem->blocks[i].unkA = 0;
    mem->blocks[i].size = size;
    mem->blocks[i].align = mem->align;
    if (mem->upwards) {
        mem->blocks[i].addr = mem->cur;
        mem->cur = ~(align - 1) & (align + (size + mem->blocks[i].addr) - 1);
    } else {
        mem->blocks[i].addr = ~(align - 1) & (mem->cur - size);
        mem->cur = mem->blocks[i].addr;
    }
    mem->used += size;
    func_001A0C20(mem, i);
    return i + 1;
}
#endif

/* Not matching yet: register allocation. */
#ifdef NON_MATCHING
/* Allocate in the first gap that fits, else at the free end. */
s32 func_001A0520(BlockMem *mem, u32 size) {
    u32 need = ~(mem->align - 1) & (size + mem->align - 1);
    s32 i = func_001A0BB0(mem);
    MemBlock *b;
    MemBlock *n;
    u32 addr;

    if (i == NO_BLOCK) {
        return 0;
    }
    if (mem->head == NO_BLOCK) {
        return func_001A03C0(mem, size);
    }
    b = &mem->blocks[mem->head];
    if (mem->upwards) {
        while (b->next != NO_BLOCK) {
            n = &mem->blocks[b->next];
            addr = ~(mem->align - 1) & (mem->align + (b->addr + b->size) - 1);
            if (n->addr - addr >= need) {
                mem->blocks[i].unkA = 0;
                mem->blocks[i].size = size;
                mem->blocks[i].align = mem->align;
                mem->blocks[i].addr = addr;
                mem->used += size;
                func_001A0C20(mem, i);
                return i + 1;
            }
            b = n;
        }
        addr = ~(mem->align - 1) & (mem->align + (b->addr + b->size) - 1);
        if (mem->cur - addr >= need) {
            mem->blocks[i].unkA = 0;
            mem->blocks[i].size = size;
            mem->blocks[i].align = mem->align;
            mem->blocks[i].addr = addr;
            mem->used += size;
            func_001A0C20(mem, i);
            return i + 1;
        }
    } else {
        while (b->next != NO_BLOCK) {
            n = &mem->blocks[b->next];
            if (b->addr - (~(mem->align - 1) & (mem->align + (n->addr + n->size) - 1)) >= need) {
                mem->blocks[i].unkA = 0;
                mem->blocks[i].size = size;
                mem->blocks[i].align = mem->align;
                mem->blocks[i].addr = b->addr - need;
                mem->used += size;
                func_001A0C20(mem, i);
                return i + 1;
            }
            b = n;
        }
        if (b->addr - mem->cur >= need) {
            mem->blocks[i].unkA = 0;
            mem->blocks[i].size = size;
            mem->blocks[i].align = mem->align;
            mem->blocks[i].addr = b->addr - need;
            mem->used += size;
            func_001A0C20(mem, i);
            return i + 1;
        }
    }
    return func_001A03C0(mem, size);
}
#endif

/* Where a block of `size` would go, compacting first if it doesn't fit; 0 if it can't. */
u32 func_001A0820(BlockMem *mem, u32 size) {
    u32 need = ~(mem->align - 1) & (size + mem->align - 1);

    if (func_001A0B60(mem) < need) {
        func_001A09A0(mem);
        need = ~(mem->align - 1) & (need + mem->align - 1);
        if (func_001A0B60(mem) < need) {
            return 0;
        }
    }
    if (mem->upwards) {
        return mem->base + mem->size - need;
    }
    return mem->base - mem->size;
}

u32 func_001A08E0(BlockMem *mem, s32 handle) {
    s32 i = handle - 1;
    if (i >= mem->count || handle == 0) {
        return 0;
    }
    return mem->blocks[i].addr;
}

s32 func_001A0920(BlockMem *mem, s32 handle) {
    u32 size;

    if (--handle >= mem->count) {
        return 0;
    }
    size = mem->blocks[handle].size;
    if (size == 0) {
        return 0;
    }
    mem->used -= size;
    mem->blocks[handle].size = 0;
    mem->blocks[handle].addr = 0;
    func_001A0D50(mem, handle);
    return 1;
}

/* Not matching yet: loop shape. */
#ifdef NON_MATCHING
/* Slide every block towards the base. */
u32 func_001A09A0(BlockMem *mem) {
    MemBlock *b;
    u32 addr;

    if (mem->head == NO_BLOCK) {
        mem->cur = mem->base;
        return mem->cur;
    }
    b = &mem->blocks[mem->head];
    if (mem->upwards) {
        addr = ~(mem->align - 1) & (mem->base + mem->align - 1);
        if (addr != b->addr) {
            func_001A5060(addr, b->addr, b->size);
            b->addr = addr;
        }
        while (b->next != NO_BLOCK) {
            u32 end = b->addr + b->size;
            b = &mem->blocks[b->next];
            addr = ~(mem->align - 1) & (mem->align + end - 1);
            if (addr != b->addr) {
                func_001A5060(addr, b->addr, b->size);
                b->addr = addr;
            }
        }
        mem->cur = ~(mem->align - 1) & (mem->align + (b->addr + b->size) - 1);
    } else {
        addr = ~(mem->align - 1) & (mem->base - b->size);
        if (addr != b->addr) {
            func_001A5060(addr, b->addr, b->size);
            b->addr = addr;
        }
        while (b->next != NO_BLOCK) {
            u32 prev = b->addr;
            b = &mem->blocks[b->next];
            addr = ~(mem->align - 1) & (prev - b->size);
            if (addr != b->addr) {
                func_001A5060(addr, b->addr, b->size);
                b->addr = addr;
            }
        }
        mem->cur = b->addr;
    }
    return mem->cur;
}
#endif

u32 func_001A0B60(BlockMem *mem) {
    if (mem->upwards) {
        return mem->base + mem->size - mem->cur - mem->reserved;
    }
    return mem->cur - (mem->base - mem->size) - mem->reserved;
}

s32 func_001A0BB0(BlockMem *mem) {
    s32 i;

    for (i = 0; i < mem->count; i++) {
        if (mem->blocks[i].size == 0) {
            func_001A5030(&mem->blocks[i], 0, 16);
            return i;
        }
    }
    return NO_BLOCK;
}

/* Not matching yet: loop shape. */
#ifdef NON_MATCHING
/* Insert block i into the address-sorted list. */
void func_001A0C20(BlockMem *mem, s32 i) {
    MemBlock *blocks = mem->blocks;
    MemBlock *b = &blocks[i];
    MemBlock *p;
    u32 prev;
    u32 cur;

    if (mem->head == NO_BLOCK) {
        mem->head = i;
        b->prev = NO_BLOCK;
        b->next = NO_BLOCK;
        return;
    }
    cur = mem->head;
    p = &blocks[cur];
    prev = NO_BLOCK;
    if (mem->upwards) {
        while (p->addr < b->addr) {
            prev = cur;
            cur = p->next;
            if (cur == NO_BLOCK) {
                break;
            }
            p = &blocks[cur];
        }
    } else {
        while (b->addr < p->addr) {
            prev = cur;
            cur = p->next;
            if (cur == NO_BLOCK) {
                break;
            }
            p = &blocks[cur];
        }
    }
    b->prev = prev;
    b->next = cur;
    if (prev != NO_BLOCK) {
        mem->blocks[prev].next = i;
    }
    if (cur != NO_BLOCK) {
        mem->blocks[cur].prev = i;
    }
}
#endif

/* Not matching yet: register allocation. */
#ifdef NON_MATCHING
/* Take block i out of the list. */
void func_001A0D50(BlockMem *mem, s32 i) {
    MemBlock *blocks = mem->blocks;
    MemBlock *b = &blocks[i];

    if (b->prev != NO_BLOCK) {
        blocks[b->prev].next = b->next;
    } else {
        mem->head = b->next;
        if (mem->head == NO_BLOCK) {
            mem->cur = mem->base;
        }
    }
    if (b->next != NO_BLOCK) {
        mem->blocks[b->next].prev = b->prev;
    }
}
#endif

void func_001A0DD0(u32 *dst, s32 size) {
    s32 i;
    s32 rest = size & 3;
    u8 *b;

    for (i = 0; i < size / 4; i++) {
        *dst++ = 0;
    }
    if (rest != 0) {
        b = (u8 *)dst;
        for (i = 0; i < rest; i++) {
            *b++ = 0;
        }
    }
}

/* The game's block memory */
extern BlockMem D_00292290;
extern MemBlock D_00282290[0x1000];

void func_001A0280(void);

void func_001A02C0(u32 base, u32 size, s32 align) {
    func_001A0350(&D_00292290, D_00282290, 0x1000, base, size, align, 1);
}

s32 func_001A02F0(u32 size) {
    return func_001A0520(&D_00292290, size);
}

s32 func_001A0300(u32 size) {
    return func_001A03C0(&D_00292290, size);
}

u32 func_001A0310(u32 size) {
    return func_001A0820(&D_00292290, size);
}

u32 func_001A0320(s32 handle) {
    return func_001A08E0(&D_00292290, handle);
}

s32 func_001A0330(s32 handle) {
    return func_001A0920(&D_00292290, handle);
}

u32 func_001A0340(void) {
    return func_001A09A0(&D_00292290);
}
