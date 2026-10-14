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

/* DMA packet groups kept in movable memory: after the memory is compacted their chains'
   NEXT addresses are moved along. */
typedef struct {
    u32 layout;         /* 0x00 */
    s32 used;           /* 0x04: the mesh's flags */
    u32 format;         /* 0x08 */
    s32 drawFlags;      /* 0x0C */
    s32 copySize;       /* 0x10 */
    s32 copy;           /* 0x14: memory handle of the mesh's copy */
    u8 unk18[4];
    s32 mem;            /* 0x1C: memory handle */
    u32 base;           /* 0x20: where it was */
    u32 heads;          /* 0x24: offset of the chain headers ({u16 quadwords - 1, ..., u32 start}) */
    s32 unk28;
    u32 starts;         /* 0x2C: offset of the chains' start offsets */
    s32 unk30;
    u32 count;          /* 0x34 */
    u8 unk38[0x48 - 0x38];
    s32 unk48;          /* 0x48: -1 when the state couldn't be set up */
    u8 *unk4C;          /* 0x4C */
    s32 unk50;
    s32 unk54;
    u8 unk58[8];
    u8 unk60[0x180 - 0x60];
} FlPacketGroup;

extern FlPacketGroup *D_0023BA2C;
extern s32 D_0023BA28;          /* groups in use */

#ifdef NON_MATCHING   /* first draft */
void func_0018BB50(FlPacketGroup *g) {
    u32 old = g->base;
    u32 base = func_0018D720(g->mem);
    u32 *start;
    u8 *head;
    u32 i;
    u32 chain;
    u32 *tag;
    u32 t;

    g->base = base;
    start = (u32 *)(base + g->starts);
    head = (u8 *)(base + g->heads);
    for (i = 0; i < g->count; i++) {
        chain = base + *start;
        ((u32 *)head)[1] = chain;
        tag = (u32 *)chain;
        head += (*(u16 *)head + 1) * 16;
        do {
            t = *tag;
            switch (t & 0x70000000) {
            case 0x10000000:            /* cnt */
                tag = (u32 *)((u8 *)tag + ((t & 0xFFFF) + 1) * 16);
                break;
            case 0x20000000:            /* next */
                tag[1] = tag[1] - old + base;
                tag = (u32 *)tag[1];
                break;
            case 0x60000000:            /* ret */
                chain = 0;
                break;
            }
        } while (chain != 0);
        start++;
    }
}
#endif

void func_0018BAE0(void) {
    s32 i;

    for (i = 0; i < 0x280; i++) {
        FlPacketGroup *g = &D_0023BA2C[i];

        if (g->used != 0 && g->mem != 0) {
            func_0018BB50(g);
        }
    }
}

extern s32 D_0023BA90;          /* drawing flags (gfxstate cmd 2 and others) */
s32 func_0018BF70(u32 n);
void func_001983E0(FlPacketGroup *g);
void func_00189A40(void *mesh, FlPacketGroup *g, s32 a);
void func_00189D60(void *mesh, FlPacketGroup *g);
void func_0018BA20(FlPacketGroup *g, s32 a);
void func_0018AEA0(FlPacketGroup *g, u32 count, s32 a, u32 heads, s32 *out);

extern u8 D_00212760[], D_00212790[], D_002127D0[], D_00212800[], D_00212830[], D_00212860[], D_00212890[], D_002128C0[], D_002128F0[], D_00212920[], D_00212950[], D_00212980[], D_002129C0[], D_002129F0[], D_00212A20[], D_00212A50[], D_00212A90[], D_00212AD0[], D_00212B00[], D_00212B40[], D_00212B70[], D_00212BB0[], D_00212BE0[], D_00212C20[], D_00212C50[], D_00212C80[], D_00212CB0[];

#define PICK(n, d)              \
    {                           \
        g->unk48 = (n);         \
        g->unk4C = (d);         \
        return;                 \
    }

/* The VU1 program for the group: by its drawing flags (the low 24 bits; none above) and its mesh
   (flags | format: 0x21035/0x121035 plain, 0x221135/0x321135 with bone weights). unk48 stays -1
   when there's none. */
#ifdef NON_MATCHING   /* first draft */
void func_001983E0(FlPacketGroup *g) {
    u32 flags;
    u32 fmt;

    g->unk48 = -1;
    g->unk4C = NULL;
    flags = g->drawFlags;
    fmt = g->used | g->format;
    if (flags & 0xFF000000) {
        return;
    }
    switch (flags & 0xFFFFFF) {
    case 0x20080:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0, D_00212760);
        }
        return;
    case 0x8802:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(1, D_00212790);
        }
        if (fmt == 0x221135 || fmt == 0x321135) {
            PICK(2, D_002127D0);
        }
        return;
    case 0x2:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(3, D_00212800);
        }
        if (fmt == 0x121025 || fmt == 0x21025) {
            PICK(4, D_00212830);
        }
        return;
    case 0x20000:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(5, D_00212860);
        }
        if (fmt == 0x121025 || fmt == 0x21025) {
            PICK(6, D_00212890);
        }
        return;
    case 0x4202:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(7, D_002128C0);
        }
        return;
    case 0x4002:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(8, D_002128F0);
        }
        return;
    case 0x4000:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(9, D_00212920);
        }
        return;
    case 0x24000:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0xA, D_00212950);
        }
        return;
    case 0xC202:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0xB, D_00212980);
        }
        if (fmt == 0x221135 || fmt == 0x321135) {
            PICK(0xC, D_002129C0);
        }
        return;
    case 0x2C000:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0xD, D_002129F0);
        }
        return;
    case 0x2C080:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0xE, D_00212A20);
        }
        return;
    case 0x8882:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0xF, D_00212A50);
        }
        if (fmt == 0x221135 || fmt == 0x321135) {
            PICK(0x10, D_00212A90);
        }
        return;
    case 0xC802:
        if (fmt == 0x221135 || fmt == 0x321135) {
            PICK(0x11, D_00212AD0);
        }
        return;
    case 0x2C822:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0x12, D_00212B00);
        }
        if (fmt == 0x221135 || fmt == 0x321135) {
            PICK(0x13, D_00212B40);
        }
        return;
    case 0x2C802:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0x14, D_00212B70);
        }
        if (fmt == 0x221135 || fmt == 0x321135) {
            PICK(0x15, D_00212BB0);
        }
        return;
    case 0x28802:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0x16, D_00212BE0);
        }
        if (fmt == 0x221135 || fmt == 0x321135) {
            PICK(0x17, D_00212C20);
        }
        return;
    case 0x2C380:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0x18, D_00212C50);
        }
        return;
    case 0x24380:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0x19, D_00212C80);
        }
        return;
    case 0x24080:
        if (fmt == 0x121035 || fmt == 0x21035) {
            PICK(0x1A, D_00212CB0);
        }
        return;
    }
}
#endif
#undef PICK

/* Build a group's packets from a converted mesh (1 done, 0 failed). */
s32 func_00189920(FlPacketGroup *g, void *mesh);
#ifdef NON_MATCHING   /* first draft: branch layout */
s32 func_00189920(FlPacketGroup *g, void *mesh) {
    func_001983E0(g);
    if (g->unk48 != -1) {
        func_00189A40(mesh, g, 0);
        func_00189D60(mesh, g);
        func_0018BA20(g, 0);
        func_0018AEA0(g, g->count, *(s32 *)(g->unk4C + 0x14), g->heads + func_0018D720(g->mem), (s32 *)g->unk60);
        D_0023BA28++;
        return 1;
    }
    return 0;
}
#endif

/* A packet group from a converted mesh (MeshOut, chunk.c) in this layout: its number (1-based),
   0 when it couldn't be built. Unless layout bit 2, a copy of the mesh is kept with it. */
s32 func_00189790(u32 *mesh, u32 layout);
#ifdef NON_MATCHING   /* first draft: registers and the copy's pointer */
s32 func_00189790(u32 *mesh, u32 layout) {
    u32 i;
    FlPacketGroup *g;
    u8 *p;

    for (i = 0; i < 0x280; i++) {
        if (D_0023BA2C[i].used == 0) {
            break;
        }
    }
    g = &D_0023BA2C[i];
    func_0018D600(g, 0, sizeof(FlPacketGroup));
    g->used = mesh[0];
    g->format = mesh[3];
    g->layout = layout;
    g->unk50 = 0;
    g->unk54 = 0;
    if (!(layout & 4)) {
        g->copySize = 0x30;
        g->copySize += (mesh[7] + 0xF) & ~0xF;
        g->copySize += (mesh[6] + 0xF) & ~0xF;
        g->copy = func_0018D6D0(g->copySize, 3);
        p = (u8 *)func_0018D720(g->copy);
        func_0018D630((s8 *)p, (s8 *)mesh, 0x24);
        p += 0x30;
        func_0018D630((s8 *)p, (s8 *)mesh[2], mesh[7]);
        func_0018D630((s8 *)(p + ((mesh[7] + 0xF) & ~0xF)), (s8 *)mesh[4], mesh[6]);
    }
    g->drawFlags = D_0023BA90;
    if (func_00189920(g, mesh) == 0) {
        func_0018BF70(i + 1);
        return 0;
    }
    return i + 1;
}
#endif

/* Frees packet group n (1-based). */
s32 func_0018BF70(u32 n) {
    FlPacketGroup *g = &D_0023BA2C[n - 1];

    if (n == 0) {
        return 0;
    }
    if (n > 0x280) {
        return 0;
    }
    if (g->used == 0) {
        return 0;
    }
    func_0018D150();
    g->used = 0;
    if (g->copy != 0) {
        func_0018D710(g->copy);
    }
    if (g->mem != 0) {
        func_0018D710(g->mem);
    }
    func_0018D600(g, 0, sizeof(FlPacketGroup));
    D_0023BA28--;
    return 1;
}

s32 func_00191F50(s32 handle) {
    func_0018D710(handle);
    return 1;
}

/* A copy of a decoded motion (tmp->data, data->size bytes) in movable memory; its handle. */
typedef struct {
    u8 unk0[0x18];
    u32 size;           /* 0x18 */
} MotionData;

typedef struct {
    u8 unk0[0x14];
    MotionData *data;   /* 0x14 */
} MotionTmp;

s32 func_0018D6D0(u32 size, s32 unused);

s32 func_00191ED0(MotionTmp *tmp) {
    MotionData *d = tmp->data;
    s32 h = func_0018D6D0(d->size, 4);
    u32 *dst = (u32 *)func_0018D720(h);
    u32 i;
    u32 *src = (u32 *)d;

    for (i = 0; i < d->size >> 2; i++) {
        *dst++ = *src++;
    }
    return h;
}

s32 func_00191F70(s32 handle) {
    func_0018D710(handle);
    return 1;
}
