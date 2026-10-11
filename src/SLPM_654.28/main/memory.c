#include "system.h"

#define ALIGN_UP(x, a) (~((a) - 1) & ((x) + (a) - 1))

/* Not matching yet: instruction order around the alignments (8 instructions). */
#ifdef NON_MATCHING
s32 func_00189480(LinearHeap *heap, u32 mem, u32 size, s32 align) {
    size = ALIGN_UP(size, align);
    heap->mem = mem;
    heap->align = align;
    heap->start = ALIGN_UP(heap->mem, heap->align);
    heap->end = ALIGN_UP(heap->mem + size, heap->align);
    heap->lo = heap->start;
    heap->hi = heap->end;
    return 1;
}
#endif

u32 func_00189500(LinearHeap *heap, u32 size, s32 fromTop) {
    u32 p;

    size = ALIGN_UP(size, heap->align);
    p = heap->lo;
    if (heap->hi < p + size) {
        return 0;
    }
    if (fromTop) {
        heap->hi -= size;
        return heap->hi;
    }
    heap->lo = p + size;
    return p;
}

/* byte fill and copy */
void func_0018D600(void *p, u8 value, s32 size) {
    u8 *dst = p;
    s32 i;

    for (i = 0; i < size; i++) {
        *dst++ = value;
    }
}

/* Not matching yet: the original copies the source pointer before incrementing it (5 instructions). */
#ifdef NON_MATCHING
void func_0018D630(s8 *dst, s8 *src, s32 size) {
    s32 i;

    for (i = 0; i < size; i++) {
        *dst++ = *src++;
    }
}
#endif


u32 func_0018D660(u32 size) {
    return func_00189500(&D_00282220, size, 0);
}

u32 func_0018D6B0(u32 size) {
    return func_00189500(&D_00282220, size, 1);
}

/* Heap save/restore: remember the bottom pointer in a slot and go back to it later. */
s32 func_00189560(LinearHeap *heap, s32 slot, u32 *mark) {
    mark[0] = (&heap->lo)[slot];
    mark[1] = slot;
    return 1;
}

void func_00189580(LinearHeap *heap, u32 *mark) {
    (&heap->lo)[mark[1]] = mark[0];
}

s32 func_0018D680(u32 *mark) {
    return func_00189560(&D_00282220, 0, mark);
}

void func_0018D6A0(u32 *mark) {
    func_00189580(&D_00282220, mark);
}

s32 func_001A02F0(u32 size);
s32 func_001A0300(u32 size);
u32 func_001A0320(s32 handle);
s32 func_001A0330(s32 handle);
u32 func_001A0340(void);
void func_0018D150(void);
void func_0018BAE0(void);

void func_0018D730(void);

/* Block memory with a retry: when it's full, wait for the GPU to let go of everything, compact, try again. */
s32 func_0018D6D0(u32 size, s32 unused) {
    s32 handle = func_001A02F0(size);
    if (handle == 0) {
        func_0018D730();
        handle = func_001A0300(size);
    }
    return handle;
}

s32 func_0018D710(s32 handle) {
    return func_001A0330(handle);
}

u32 func_0018D720(s32 handle) {
    return func_001A0320(handle);
}

void func_0018D730(void) {
    func_0018D150();
    func_001A0340();
    func_0018BAE0();
}
