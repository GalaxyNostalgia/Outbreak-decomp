#include "common.h"

void func_001A5030(void *p, u8 value, s32 size) {
    u8 *dst = p;
    s32 i;

    for (i = 0; i < size; i++) {
        *dst++ = value;
    }
}

/* Not matching yet: the copy loops keep a copy of the source pointer (22 instructions). */
#ifdef NON_MATCHING
/* memmove */
void func_001A5060(void *d, void *s, s32 size) {
    s8 *dst = d;
    s8 *src = s;
    s32 i = 0;

    if (src < dst && dst < src + size) {
        dst += size - 1;
        src += size - 1;
        for (i = 0; i < size; i++) {
            *dst-- = *src--;
        }
    } else {
        for (; i < size; i++) {
            *dst++ = *src++;
        }
    }
}
#endif

/* Free lists: a singly linked list threaded through the free items; bit 0 marks the last one. */
void func_001A50F0(s32 count, u32 *items, s32 unused) {
    s32 i;

    for (i = 0; i < count - 1; i++) {
        items[i] = (u32)&items[i + 1];
    }
    items[i] = 0;
}

void func_001A5130(u32 *item, u32 *list) {
    u32 head = *list;
    *list = (u32)item | 1;
    *item = head;
}

u32 func_001A5150(u32 *list) {
    u32 *item;
    u32 next;

    while (1) {
        item = (u32 *)*list;
        if (item == NULL) {
            return 0;
        }
        next = *item;
        if (next & 1) {
            return *list = next & ~1;
        }
        *list = next;
    }
}
