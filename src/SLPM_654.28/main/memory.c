#include "system.h"
#include "model.h"

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
    s32 size;           /* 0x18: the packets' bytes */
    s32 mem;            /* 0x1C: memory handle */
    u32 base;           /* 0x20: where it was */
    u32 heads;          /* 0x24: offset of the chain headers ({u16 quadwords - 1, ..., u32 start}) */
    s32 unk28;
    u32 starts;         /* 0x2C: offset of the chains' start offsets */
    s32 unk30;
    s32 count;          /* 0x34: materials used, listed at unk60 */
    s32 ntex;           /* 0x38: textures in texList */
    s32 ntris;          /* 0x3C */
    s32 nverts;         /* 0x40 */
    s32 nbatches;       /* 0x44 */
    s32 unk48;          /* 0x48: -1 when the state couldn't be set up */
    u8 *unk4C;          /* 0x4C */
    s32 unk50;
    s32 unk54;
    u8 unk58[8];
    u8 unk60[0x20];     /* 0x60: the materials used (their state slots, D_002767A0) */
    s32 tex[0x20];      /* 0x80: each one's texture */
    s32 texList[0x20];  /* 0x100: the different textures */
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
/* A VU1 program's description (D_00212760... picked by func_001983E0). */
typedef struct {
    u8 unk0[0x14];
    s32 matKind;        /* 0x14: index into D_002122C0 (bytes per matrix) */
    s32 vtxKind;        /* 0x18: index into D_002122E0 ({bytes per vertex, what it has}) */
    s32 primKind;       /* 0x1C: index into D_00212340 ({GS PRIM, GIF REGS, NREG}) */
    u32 batch;          /* 0x20: the most bytes of vertices a batch can take */
    u8 unk24[4];
    s32 *mtx;           /* 0x28: {VU address of the matrices, ..., bytes per matrix} */
} VuProg;

extern s32 D_002122C0[];
extern s32 D_002122E0[][2];
extern s32 D_00212340[][3];
extern f32 D_0023BA1C;          /* frame height / next power of two */
extern f32 D_0023BA20;          /* frame width / next power of two */
extern char D_00223800[];       /* the size mismatch message */
s32 func_001A0080(const char *fmt, ...);
f32 func_0018ADB0(u32 c, s32 unit);
u128 *func_0018C0B0(u128 *p, s32 qwc, s32 irq, s32 unused);
u128 *func_0018C0E0(u128 *p, s32 qwc, u128 *next, s32 irq);
u128 *func_0018C250(u128 *p, s32 qwc, s32 irq, s32 unused);
u128 *func_0018C2B0(u128 *p);
u32 *func_0018C2D0(u128 *p, s32 qwc, s32 addr);

void func_00189A40(void *mesh, FlPacketGroup *g, s32 a);
void func_00189D60(void *mesh, FlPacketGroup *g);
void func_0018BA20(FlPacketGroup *g, s32 a);
void func_0018AEA0(FlPacketGroup *g, s32 count, s32 matKind, u8 *heads, u8 *list);

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
        func_0018AEA0(g, g->count, *(s32 *)(g->unk4C + 0x14), (u8 *)(g->heads + func_0018D720(g->mem)), g->unk60);
        D_0023BA28++;
        return 1;
    }
    return 0;
}
#endif

/* The mesh's primitives batched for the group's VU1 program: the counts, the materials used and the
   packet memory (its size and layout) for them. */
#ifdef NON_MATCHING   /* first draft */
void func_00189A40(void *mesh, FlPacketGroup *g, s32 unused) {
    s8 used[0x20];
    VuProg *vp = (VuProg *)g->unk4C;
    u16 *p;
    s32 count;
    s32 lastIdx;
    s32 idx;
    s32 lastType;
    s32 type;
    s32 cur;
    s32 n;
    s32 total;
    s32 chunk;
    u32 limit;
    u32 flags;
    u16 len;
    s32 vsize;
    s32 i;

    total = 0;
    chunk = 0;
    func_0018D600(used, 0, sizeof(used));
    p = ((u16 **)mesh)[2];
    count = ((s32 *)mesh)[1];
    lastIdx = -1;
    idx = 0;
    lastType = -1;
    type = 0;
    cur = 0;
    n = 0;
    g->nverts = 0;
    g->ntris = 0;
    g->nbatches = 0;
    limit = vp->batch + 0x20;
    for (i = 0; i < count; i++) {
        len = *p & 0x3FFF;
        g->nverts += len;
        g->ntris += len - 2;
        flags = ((u32 *)mesh)[0];
        p++;
        if (flags & 0x100000) {
            idx = *p++;
        }
        if (flags & 0x200000) {
            type = *p++;
        }
        if (flags & 0x1000) {
            p += len;
        }
        if (lastIdx != idx) {
            if (n != 0) {
                chunk += cur;
                n = 0;
                total += chunk;
            }
            lastIdx = idx;
            used[idx] = 1;
            lastType = type;
            g->nbatches++;
            chunk = 0x30;
            cur = 0x20;
        } else if (lastType != type) {
            if (n != 0) {
                chunk += cur;
                n = 0;
                if (chunk + limit >= 0x100000) {
                    total += chunk;
                    chunk = 0x10;
                }
            }
            lastType = type;
            chunk += 0x20;
            cur = 0x20;
        }
        if (len > 0) {
            vsize = D_002122E0[vp->vtxKind][0];
            do {
                if ((u32)(cur + vsize) >= vp->batch) {
                    chunk += cur;
                    if (chunk + limit >= 0x100000) {
                        total += chunk;
                        chunk = 0x10;
                    }
                    chunk += 0x20;
                    cur = 0x20;
                    n = 0;
                }
                len--;
                n++;
                cur += vsize;
            } while (len != 0);
        }
    }
    chunk += cur;
    total += chunk;
    g->size = total;
    g->count = 0;
    for (i = 0; i < 0x20; i++) {
        if (used[i]) {
            g->unk60[g->count++] = i;
        }
    }
    g->unk30 = (g->count * 4 + 0xF) & ~0xF;
    g->unk28 = (D_002122C0[vp->matKind] * g->count + 0xF) & ~0xF;
    g->unk28 += 0x20;
    g->starts = (g->size + 0xF) & ~0xF;
    g->heads = g->starts + ((g->unk30 + 0xF) & ~0xF);
    g->mem = func_0018D6D0(((total + 0xF) & ~0xF) + ((g->unk30 + 0xF) & ~0xF) + ((g->unk28 + 0xF) & ~0xF), 3);
    g->base = func_0018D720(g->mem);
}
#endif

extern u8 D_002767A0[][0x4C];     /* the material state slots (set by gfxstate commands 0x3A on) */
void func_0018AF90(u32 chain, u8 *p, u32 matKind, s32 mat);

/* A colour byte for the VU programs: unit 0: PS2 scale (0x80 is full; 0xFF gives 128.0), else
   0-1. */
f32 func_0018ADB0(u32 c, s32 unit) {
    f32 f = 0.0f;

    if (unit == 0) {
        if (c == 0xFF) {
            f = 128.0f;
        } else if (c != 0) {
            c >>= 1;
            if (c == 0) {
                c = 1;
            }
            f = c;
        }
    } else {
        f = c / 255.0f;
        if (f > 1.0f) {
            f = 1.0f;
        }
        if (f < 0.0f) {
            f = 0.0f;
        }
    }
    return f;
}

/* Close the open batch: its UNPACK and header ({kind, size} and the GIF tag for n vertices), then
   an MSCNT. */
#define END_BATCH()                                                                                 \
    {                                                                                               \
        u32 *h_ = func_0018C2D0((u128 *)unpack, size >> 4, 0);                                      \
        u32 qw_;                                                                                    \
        switch (type) {                                                                             \
        case 0:                                                                                     \
            h_[0] = 1;                                                                              \
            break;                                                                                  \
        case 1:                                                                                     \
            h_[0] = 2;                                                                              \
            break;                                                                                  \
        }                                                                                           \
        qw_ = stride * (n + 2);                                                                     \
        h_[1] = (qw_ >> 4) + 1;                                                                     \
        *(u64 *)&h_[2] = 0;                                                                         \
        *(u64 *)&h_[4] = nreg | ((s64)n | (0x8000 | ((u64)0x4000 << 32)) | prim);                   \
        h_[6] = regs;                                                                               \
        h_[7] = 0;                                                                                  \
        q = (u8 *)func_0018C2B0((u128 *)q);                                                         \
    }

/* Close the chain for material idx at q: the material's first chain ends in a RET; a later one is
   linked from the end of the ones before (their RET made a NEXT). */
#define END_CHAIN(idx)                                                                              \
    {                                                                                               \
        u32 *t_ = (u32 *)first[idx];                                                                \
        if ((u8 *)t_ == chain) {                                                                    \
            func_0018C250((u128 *)chain, ((u32)(q - chain) >> 4) - 1, 0, 0);                        \
            starts[idx] = chain - base;                                                             \
        } else {                                                                                    \
            do {                                                                                    \
                u32 tag_ = *t_;                                                                     \
                switch (tag_ & 0x70000000) {                                                        \
                case 0x10000000:                                                                    \
                case 0x20000000:                                                                    \
                    t_ = (u32 *)((u8 *)t_ + ((tag_ & 0xFFFF) + 1) * 16);                            \
                    break;                                                                          \
                case 0x60000000:                                                                    \
                    func_0018C0E0((u128 *)t_, tag_ & 0xFFFF, (u128 *)chain, 0);                     \
                    t_ = NULL;                                                                      \
                    break;                                                                          \
                }                                                                                   \
            } while (t_ != NULL);                                                                   \
            func_0018C250((u128 *)chain, ((u32)(q - chain) >> 4) - 1, 0, 0);                        \
        }                                                                                           \
    }

/* A colour channel as the VU program wants it. */
#define CHANNEL(c, raw) ((raw) ? (f32)(u32)(c) : func_0018ADB0((c), 1))

/* Build the group's packets: per material, a DMA chain of batches (UNPACK of a header and up to
   vp->batch bytes of vertices, then MSCNT), each vertex as the VU program wants it (position with
   the ADC bit on a strip's first two, normal with an alternating sign, colour, UV, matrices and
   weights). */
#ifdef NON_MATCHING   /* first draft */
void func_00189D60(void *mesh, FlPacketGroup *g) {
    s32 bone[4];
    s32 weight[4];
    u8 *first[0x20];
    VuProg *vp = (VuProg *)g->unk4C;
    u32 has = D_002122E0[vp->vtxKind][1];
    u32 *mo = (u32 *)mesh;
    u8 *base;
    u32 *starts;
    u8 *q;
    u8 *chain = NULL;
    u8 *unpack = NULL;
    u8 *v0 = NULL;
    u8 *v1 = NULL;
    u8 *v2 = NULL;
    u16 *p;
    u32 primBits;
    u64 prim;
    u64 nreg;
    u32 regs;
    s32 stride;
    u32 limit;
    s32 lastIdx;
    s32 idx;
    s32 type;
    s32 newType;
    s32 n;
    u32 size;
    s32 vsize;
    s32 sign;
    s32 count;
    s32 i;
    s32 k;
    s32 b;
    u32 len;
    u32 c;

    for (i = 0; i < 4; i++) {
        bone[i] = 0;
        weight[i] = 0;
    }
    for (i = 0x1F; i >= 0; i--) {
        first[i] = NULL;
    }
    primBits = D_00212340[vp->primKind][0];
    regs = D_00212340[vp->primKind][1];
    nreg = D_00212340[vp->primKind][2];
    if ((g->drawFlags & 0x18000) == 0x8000) {
        primBits |= 0x20;
    }
    base = (u8 *)func_0018D720(g->mem);
    q = base;
    prim = (u64)primBits << 47;
    n = 0;
    p = ((u16 **)mesh)[2];
    lastIdx = -1;
    count = mo[1];
    idx = 0;
    newType = 0;
    type = -1;
    stride = nreg * 16;
    limit = vp->batch + 0x20;
    starts = (u32 *)(base + g->starts);
    nreg <<= 60;
    size = 0;
    for (i = 0; i < count; i++) {
        len = *p++;
        sign = 0;
        if (mo[0] & 0x100000) {
            idx = *p++;
        }
        if (mo[0] & 0x200000) {
            newType = *p++;
        }
        if (lastIdx != idx) {
            if (n != 0) {
                END_BATCH();
                END_CHAIN(lastIdx);
                n = 0;
            }
            lastIdx = idx;
            type = newType;
            chain = q;
            if (first[idx] == NULL) {
                first[idx] = q;
            }
            unpack = q + 0x10;
            size = 0x20;
            q += 0x40;
        } else if (type != newType) {
            if (n != 0) {
                END_BATCH();
                n = 0;
                if (limit + (u32)(q - chain) >= 0x100000) {
                    func_0018C0B0((u128 *)chain, ((u32)(q - chain) >> 4) - 1, 0, 0);
                    chain = q;
                    q += 0x10;
                    n = 0;
                }
            }
            unpack = q;
            q += 0x30;
            type = newType;
            size = 0x20;
        }
        len &= 0x3FFF;
        for (k = 0; (u32)k < len; k++) {
            vsize = D_002122E0[vp->vtxKind][0];
            if (size + vsize >= vp->batch) {
                END_BATCH();
                unpack = q;
                if (limit + (u32)(q - chain) >= 0x100000) {
                    func_0018C0B0((u128 *)chain, ((u32)(q - chain) >> 4) - 1, 0, 0);
                    chain = q;
                    q += 0x10;
                    unpack = q;
                }
                size = 0x20;
                q += 0x30;
                n = 0;
            }
            size += vsize;
            n++;
            switch (mo[3]) {
            case 0x25:
                if (mo[0] & 0x1000) {
                    v0 = (u8 *)mo[4] + *p++ * 0x1C;
                } else {
                    v0 += 0x1C;
                }
                if (has & 1) {
                    ((f32 *)q)[0] = ((f32 *)v0)[0];
                    ((f32 *)q)[1] = ((f32 *)v0)[1];
                    ((f32 *)q)[2] = ((f32 *)v0)[2];
                    ((u32 *)q)[3] = k < 2 ? 0x8000 : 0;
                    q += 0x10;
                }
                if (has & 2) {
                    ((f32 *)q)[0] = ((f32 *)v0)[3];
                    ((f32 *)q)[1] = ((f32 *)v0)[4];
                    ((f32 *)q)[2] = ((f32 *)v0)[5];
                    ((u32 *)q)[3] = sign == 0 ? 0x3F800000 : 0xBF800000;
                    q += 0x10;
                }
                if (has & 4) {
                    c = ((u32 *)v0)[6];
                    ((f32 *)q)[0] = CHANNEL((c >> 16) & 0xFF, !(g->drawFlags & 0xF00));
                    ((f32 *)q)[1] = CHANNEL((c >> 8) & 0xFF, !(g->drawFlags & 0xF00));
                    ((f32 *)q)[2] = CHANNEL(c & 0xFF, !(g->drawFlags & 0xF00));
                    ((f32 *)q)[3] = func_0018ADB0((c >> 24) & 0xFF, 0);
                    q += 0x10;
                }
                break;
            case 0x35:
                if (mo[0] & 0x1000) {
                    v1 = (u8 *)mo[4] + *p++ * 0x24;
                } else {
                    v1 += 0x24;
                }
                if (has & 1) {
                    ((f32 *)q)[0] = ((f32 *)v1)[0];
                    ((f32 *)q)[1] = ((f32 *)v1)[1];
                    ((f32 *)q)[2] = ((f32 *)v1)[2];
                    ((u32 *)q)[3] = k < 2 ? 0x8000 : 0;
                    q += 0x10;
                }
                if (has & 2) {
                    ((f32 *)q)[0] = ((f32 *)v1)[3];
                    ((f32 *)q)[1] = ((f32 *)v1)[4];
                    ((f32 *)q)[2] = ((f32 *)v1)[5];
                    ((u32 *)q)[3] = sign == 0 ? 0x3F800000 : 0xBF800000;
                    q += 0x10;
                }
                if (has & 4) {
                    c = ((u32 *)v1)[6];
                    ((f32 *)q)[0] = func_0018ADB0((c >> 16) & 0xFF, (g->drawFlags & 0xF00) != 0);
                    ((f32 *)q)[1] = func_0018ADB0((c >> 8) & 0xFF, (g->drawFlags & 0xF00) != 0);
                    ((f32 *)q)[2] = func_0018ADB0(c & 0xFF, (g->drawFlags & 0xF00) != 0);
                    ((f32 *)q)[3] = func_0018ADB0((c >> 24) & 0xFF, 0);
                    q += 0x10;
                }
                if (has & 8) {
                    ((f32 *)q)[0] = ((f32 *)v1)[7];
                    ((f32 *)q)[1] = ((f32 *)v1)[8];
                    ((u32 *)q)[2] = 0x3F800000;
                    ((u32 *)q)[3] = 0;
                    if (g->layout & 0x20) {
                        ((f32 *)q)[0] *= D_0023BA20;
                        ((f32 *)q)[1] *= D_0023BA1C;
                    }
                    q += 0x10;
                }
                break;
            case 0x135:
                if (mo[0] & 0x1000) {
                    v2 = (u8 *)mo[4] + *p++ * 0x2C;
                } else {
                    v2 += 0x2C;
                }
                if (has & 1) {
                    ((f32 *)q)[0] = ((f32 *)v2)[0];
                    ((f32 *)q)[1] = ((f32 *)v2)[1];
                    ((f32 *)q)[2] = ((f32 *)v2)[2];
                    ((u32 *)q)[3] = k < 2 ? 0x8000 : 0;
                    q += 0x10;
                }
                if (has & 2) {
                    ((f32 *)q)[0] = ((f32 *)v2)[3];
                    ((f32 *)q)[1] = ((f32 *)v2)[4];
                    ((f32 *)q)[2] = ((f32 *)v2)[5];
                    ((u32 *)q)[3] = sign == 0 ? 0x3F800000 : 0xBF800000;
                    q += 0x10;
                }
                if (has & 4) {
                    c = ((u32 *)v2)[6];
                    ((f32 *)q)[0] = func_0018ADB0((c >> 16) & 0xFF, (g->drawFlags & 0xF00) != 0);
                    ((f32 *)q)[1] = func_0018ADB0((c >> 8) & 0xFF, (g->drawFlags & 0xF00) != 0);
                    ((f32 *)q)[2] = func_0018ADB0(c & 0xFF, (g->drawFlags & 0xF00) != 0);
                    ((f32 *)q)[3] = func_0018ADB0((c >> 24) & 0xFF, 0);
                    q += 0x10;
                }
                if (has & 8) {
                    ((f32 *)q)[0] = ((f32 *)v2)[7];
                    ((f32 *)q)[1] = ((f32 *)v2)[8];
                    ((u32 *)q)[2] = 0x3F800000;
                    ((u32 *)q)[3] = 0;
                    if (g->layout & 0x20) {
                        ((f32 *)q)[0] *= D_0023BA20;
                        ((f32 *)q)[1] *= D_0023BA1C;
                    }
                    q += 0x10;
                }
                if ((has & 0x10) && (has & 0x20)) {
                    u32 *mq = (u32 *)q;
                    f32 *wq = (f32 *)(q + 0x10);

                    c = ((u32 *)v2)[9];
                    bone[0] = (c >> 24) & 0xFF;
                    bone[1] = (c >> 16) & 0xFF;
                    bone[2] = (c >> 8) & 0xFF;
                    bone[3] = c & 0xFF;
                    c = ((u32 *)v2)[10];
                    weight[0] = (c >> 24) & 0xFF;
                    weight[1] = (c >> 16) & 0xFF;
                    weight[2] = (c >> 8) & 0xFF;
                    weight[3] = c & 0xFF;
                    *(u128 *)q = U128_ZERO;
                    *(u128 *)(q + 0x10) = U128_ZERO;
                    q += 0x20;
                    switch (type) {
                    case 0:
                        mq[0] = vp->mtx[0] + bone[0] * 4;
                        g->unk54 |= 1 << bone[0];
                        mq[1] = vp->mtx[0] + vp->mtx[1] * 4 + bone[0] * vp->mtx[2];
                        ((u32 *)wq)[0] = 0x42C80000;    /* 100.0f */
                        break;
                    case 1:
                        for (b = 0; b < 4; b++) {
                            mq[b] = vp->mtx[0] + bone[b] * 4;
                            g->unk54 |= 1 << bone[b];
                            wq[b] = weight[b] / 255.0f;
                        }
                        break;
                    }
                }
                break;
            }
            sign ^= 1;
        }
    }
    if (n != 0) {
        END_BATCH();
    }
    END_CHAIN(lastIdx);
    if (g->size != q - base) {
        func_001A0080(D_00223800, base, q, g->size);
    }
}
#endif

/* The material state slots (0x4C bytes, set by gfxstate commands 0x3A on). */
/* Each used material's texture, and the different ones. */
#ifdef NON_MATCHING   /* registers (two temporaries swapped) */
void func_0018BA20(FlPacketGroup *g, s32 unused) {
    s32 i;
    s32 k;
    u8 *m;

    g->ntex = 0;
    for (i = 0; i < g->count; i++) {
        m = D_002767A0[g->unk60[i]];
        g->tex[i] = 0;
        if (*(s32 *)(m + 0x44) != 0) {
            g->tex[i] = *(s32 *)(m + 0x44);
            for (k = 0; k < g->ntex; k++) {
                if (g->texList[k] == *(s32 *)(m + 0x44)) {
                    goto next;
                }
            }
            g->texList[g->ntex++] = *(s32 *)(m + 0x44);
        }
    next:;
    }
}
#endif

s32 func_00197C50(u32 h, u64 *texa, u64 *tex1, u64 *tex0, u64 *clamp, u64 *mip1, u64 *mip2, u32 flags);
extern u32 D_0023BA8C;          /* default drawing flags */
extern s32 D_0023BA54;          /* the second texture (gfxstate cmd 5) */
u128 *func_0018C1F0(u128 *p, s32 qwc, u32 addr, s32 irq);
u32 *func_0018C280(u32 *tag, s32 qwc, s32 irq, s32 unused);

/* A+D writes of a texture's registers (TEXA, TEX1_1, TEX0_1, CLAMP_1, MIPTBP1_1, MIPTBP2_1);
   texture 0 writes zeros. */
void func_0018B750(s32 tex, u64 *texa, u64 *tex1, u64 *tex0, u64 *clamp, u64 *mip1, u64 *mip2) {
    if (tex != 0) {
        func_00197C50(tex, texa, tex1, tex0, clamp, mip1, mip2, D_0023BA8C);
        texa[1] = 0x3B;
        tex1[1] = 0x14;
        tex0[1] = 6;
        clamp[1] = 8;
        mip1[1] = 0x34;
        mip2[1] = 0x36;
    } else {
        texa[0] = 0;
        texa[1] = 0x3B;
        tex1[0] = 0;
        tex1[1] = 0x14;
        tex0[0] = 0;
        tex0[1] = 6;
        clamp[0] = 0;
        clamp[1] = 8;
        mip1[0] = 0;
        mip1[1] = 0x34;
        mip2[0] = 0;
        mip2[1] = 0x36;
    }
}

/* The same for two textures, the second to context 2's registers (TEX1_2, TEX0_2, CLAMP_2,
   MIPTBP1_2, MIPTBP2_2). */
void func_0018B840(s32 tex, u64 *texa, u64 *tex1, u64 *tex0, u64 *clamp, u64 *mip1, u64 *mip2, s32 tex2,
                   u64 *tex1b, u64 *tex0b, u64 *clampb, u64 *mip1b, u64 *mip2b) {
    if (tex != 0) {
        func_00197C50(tex, texa, tex1, tex0, clamp, mip1, mip2, D_0023BA8C);
        texa[1] = 0x3B;
        tex1[1] = 0x14;
        tex0[1] = 6;
        clamp[1] = 8;
        mip1[1] = 0x34;
        mip2[1] = 0x36;
    } else {
        texa[0] = 0;
        texa[1] = 0x3B;
        tex1[0] = 0;
        tex1[1] = 0x14;
        tex0[0] = 0;
        tex0[1] = 6;
        clamp[0] = 0;
        clamp[1] = 8;
        mip1[0] = 0;
        mip1[1] = 0x34;
        mip2[0] = 0;
        mip2[1] = 0x36;
    }
    if (tex2 != 0) {
        func_00197C50(tex2, texa, tex1b, tex0b, clampb, mip1b, mip2b, D_0023BA8C);
        texa[1] = 0x3B;
        tex1b[1] = 0x15;
        tex0b[1] = 7;
        clampb[1] = 9;
        mip1b[1] = 0x34;
        mip2b[1] = 0x36;
    } else {
        texa[0] = 0;
        texa[1] = 0x3B;
        tex1b[0] = 0;
        tex1b[1] = 0x15;
        tex0b[0] = 0;
        tex0b[1] = 7;
        clampb[0] = 0;
        clampb[1] = 9;
        mip1b[0] = 0;
        mip1b[1] = 0x35;
        mip2b[0] = 0;
        mip2b[1] = 0x37;
    }
}

/* The header for A+D writes: an UNPACK of n + 1 + (qwords) and a GIF tag of n A+D writes. */
#define AD_HEADER(p, n)                                                                             \
    {                                                                                               \
        ((u128 *)(p))[2] = U128_ZERO;                                                               \
        ((u64 *)(p))[6] = (n) | 0x8000 | ((u64)0x10000000 << 32);                                   \
        ((u32 *)(p))[14] = 0xE;                                                                     \
        ((u32 *)(p))[15] = 0;                                                                       \
    }

/* A+D writes with no data (reg ids only). */
#define AD_ZERO(q, reg)                                                                             \
    {                                                                                               \
        ((u64 *)(q))[0] = 0;                                                                        \
        ((u64 *)(q))[1] = (reg);                                                                    \
    }

/* A material's colours as the VU programs want them (the alpha scaled to 0-128). */
#define COLOURS(q, v)                                                                               \
    {                                                                                               \
        ((f32 *)(q))[0] = ((f32 *)(v))[0];                                                          \
        ((f32 *)(q))[1] = ((f32 *)(v))[1];                                                          \
        ((f32 *)(q))[2] = ((f32 *)(v))[2];                                                          \
        ((f32 *)(q))[3] = 128.0f * ((f32 *)(v))[3];                                                 \
    }

#define COLOURS4(q, v)                                                                              \
    {                                                                                               \
        ((f32 *)(q))[0] = ((f32 *)(v))[0];                                                          \
        ((f32 *)(q))[1] = ((f32 *)(v))[1];                                                          \
        ((f32 *)(q))[2] = ((f32 *)(v))[2];                                                          \
        ((f32 *)(q))[3] = ((f32 *)(v))[3];                                                          \
    }

/* A material's part of the group's packet: a DMA call of its chain (chain) after the material's
   registers and colours for the VU program (by matKind), ending in an MSCNT. */
#ifdef NON_MATCHING   /* first draft */
void func_0018AF90(u32 chain, u8 *p, u32 matKind, s32 mat) {
    u8 *m = D_002767A0[mat];
    s32 power;
    s32 shift;
    s32 exact;

    switch (matKind) {
    case 0:
        func_0018C1F0((u128 *)p, 0xC, chain, 0);
        func_0018C2D0((u128 *)(p + 0x10), 0xA, 0);
        AD_HEADER(p, 6);
        func_0018B750(*(s32 *)(m + 0x44), (u64 *)(p + 0x40), (u64 *)(p + 0x50), (u64 *)(p + 0x60),
                      (u64 *)(p + 0x70), (u64 *)(p + 0x80), (u64 *)(p + 0x90));
        COLOURS(p + 0xA0, m + 4);
        COLOURS4(p + 0xB0, m + 0x24);
        func_0018C2B0((u128 *)(p + 0xC0));
        break;
    case 1:
        func_0018C1F0((u128 *)p, 0xC, chain, 0);
        func_0018C2D0((u128 *)(p + 0x10), 0xA, 0);
        AD_HEADER(p, 6);
        AD_ZERO(p + 0x40, 0x3B);
        AD_ZERO(p + 0x50, 0x14);
        AD_ZERO(p + 0x60, 6);
        AD_ZERO(p + 0x70, 8);
        AD_ZERO(p + 0x80, 0x34);
        AD_ZERO(p + 0x90, 0x36);
        COLOURS(p + 0xA0, m + 4);
        COLOURS4(p + 0xB0, m + 0x24);
        func_0018C2B0((u128 *)(p + 0xC0));
        break;
    case 2:
        func_0018C1F0((u128 *)p, 0xE, chain, 0);
        func_0018C2D0((u128 *)(p + 0x10), 0xC, 0);
        AD_HEADER(p, 6);
        func_0018B750(*(s32 *)(m + 0x44), (u64 *)(p + 0x40), (u64 *)(p + 0x50), (u64 *)(p + 0x60),
                      (u64 *)(p + 0x70), (u64 *)(p + 0x80), (u64 *)(p + 0x90));
        COLOURS(p + 0xA0, m + 4);
        COLOURS4(p + 0xB0, m + 0x24);
        COLOURS4(p + 0xC0, m + 0x14);
        power = *(s32 *)(m + 0x48);
        switch (power) {
        case 0:
        case 1:
        case 2:
            shift = 1;
            exact = 1;
            break;
        case 4:
            shift = 2;
            exact = 1;
            break;
        case 8:
            shift = 3;
            exact = 1;
            break;
        case 0x10:
            shift = 4;
            exact = 1;
            break;
        case 0x20:
            shift = 5;
            exact = 1;
            break;
        case 0x40:
            shift = 6;
            exact = 1;
            break;
        case 0x80:
            shift = 7;
            exact = 1;
            break;
        case 0x100:
            shift = 8;
            exact = 1;
            break;
        default:
            shift = power - 1;
            exact = 0;
            break;
        }
        ((s32 *)p)[0x34] = shift;
        ((s32 *)p)[0x35] = exact;
        ((s32 *)p)[0x36] = 0;
        ((s32 *)p)[0x37] = 0;
        func_0018C2B0((u128 *)(p + 0xE0));
        break;
    case 3:
        func_0018C1F0((u128 *)p, 0xA, chain, 0);
        func_0018C2D0((u128 *)(p + 0x10), 8, 0);
        AD_HEADER(p, 6);
        func_0018B750(*(s32 *)(m + 0x44), (u64 *)(p + 0x40), (u64 *)(p + 0x50), (u64 *)(p + 0x60),
                      (u64 *)(p + 0x70), (u64 *)(p + 0x80), (u64 *)(p + 0x90));
        func_0018C2B0((u128 *)(p + 0xA0));
        break;
    case 4:
        func_0018C1F0((u128 *)p, 0xB, chain, 0);
        func_0018C2D0((u128 *)(p + 0x10), 9, 0);
        AD_HEADER(p, 6);
        AD_ZERO(p + 0x40, 0x3B);
        AD_ZERO(p + 0x50, 0x14);
        AD_ZERO(p + 0x60, 6);
        AD_ZERO(p + 0x70, 8);
        AD_ZERO(p + 0x80, 0x34);
        AD_ZERO(p + 0x90, 0x36);
        ((u128 *)p)[0xA] = U128_ZERO;
        func_0018C2B0((u128 *)(p + 0xB0));
        break;
    case 5:
        func_0018C1F0((u128 *)p, 0x10, chain, 0);
        func_0018C2D0((u128 *)(p + 0x10), 0xE, 0);
        AD_HEADER(p, 0xB);
        func_0018B840(*(s32 *)(m + 0x44), (u64 *)(p + 0x40), (u64 *)(p + 0x50), (u64 *)(p + 0x60),
                      (u64 *)(p + 0x70), (u64 *)(p + 0x80), (u64 *)(p + 0x90), D_0023BA54, (u64 *)(p + 0xA0),
                      (u64 *)(p + 0xB0), (u64 *)(p + 0xC0), (u64 *)(p + 0xD0), (u64 *)(p + 0xE0));
        COLOURS(p + 0xF0, m + 4);
        func_0018C2B0((u128 *)(p + 0x100));
        break;
    }
}
#endif

/* The group's head packet: each used material's part, then an END tag. */
#ifdef NON_MATCHING   /* registers */
void func_0018AEA0(FlPacketGroup *g, s32 count, s32 matKind, u8 *heads, u8 *list) {
    u32 *starts;
    s32 i;
    s32 size;
    u32 base;

    size = D_002122C0[matKind];
    base = g->base;
    starts = (u32 *)(base + g->starts);
    for (i = 0; i < count; i++) {
        func_0018AF90(base + starts[list[i]], heads, matKind, list[i]);
        heads += size;
    }
    func_0018C280((u32 *)heads, 1, 1, 0);
    ((u32 *)heads)[2] = 0;
    ((u32 *)heads)[3] = 0;
    ((u32 *)heads)[4] = 0;
    ((u32 *)heads)[5] = 0;
    ((u32 *)heads)[6] = 0;
    ((u32 *)heads)[7] = 0;
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

/* A skeleton (see chunk.c: hdr's word 2 points at it, word 3 is its size) copied to memory of its
   own: the handle. */
s32 func_00191E50(u32 *hdr) {
    u32 *skel = (u32 *)hdr[2];
    s32 h = func_0018D6D0(skel[3], 4);
    u8 *dst = (u8 *)func_0018D720(h);
    u32 i;
    u8 *src = (u8 *)skel;

    for (i = 0; i < skel[3]; i++) {
        *dst++ = *src++;
    }
    return h;
}

/* A skeleton handle's bone count. */
s32 func_001929B0(s32 h) {
    s16 *s = (s16 *)func_0018D720(h);

    if (s != NULL) {
        return s[1];
    }
    return 0;
}

s32 func_00192F30(FlBone **bones, s32 skel, s32 a, s32 mode);

/* Set the skeleton's bones up in blocks. */
s32 func_00192F20(FlBone **bones, s32 skel) {
    return func_00192F30(bones, skel, 0, 1);
}

void func_0018F7C0(f32 *m);
extern f32 D_00277260[4][4];

/* A skeleton as built by func_001A2350 (chunk.c). */
typedef struct {
    s16 id;
    u16 unk2;
    s16 unk4;
    s16 child;          /* 0x06 */
    s16 next;           /* 0x08 */
    s16 unkA;
    u8 unkC[4];
    f32 v[3][4];        /* 0x10 */
} SkelFileBone;         /* 0x40 */

typedef struct {
    s16 unk0;
    s16 n;              /* 0x02 */
    s32 unk4;
    s32 unk8;
    s32 size;           /* 0x0C */
    SkelFileBone bones[1];  /* 0x10 */
} SkelFile;
s32 func_00193130(FlBone *b, f32 sx, f32 sy, f32 sz, f32 *parent);
s32 func_00193240(FlBone *b, f32 sx, f32 sy, f32 sz, f32 *parent);
s32 func_00193570(FlBone *b, u8 *skel, s32 i);
void func_00193070(FlBone **bones, FlBone *b, FlBone *parent);
s32 func_00193110(FlBone *b);
s32 func_00193220(FlBone *b);

/* The bones from a skeleton handle: cleared, read from the skeleton, linked, and posed (mode 0 or
   1) from the root. */
s32 func_00192F30(FlBone **bones, s32 skel, s32 unused, s32 mode) {
    s16 n;
    u8 *s;
    s32 i;

    s = (u8 *)func_0018D720(skel);
    n = ((s16 *)s)[1];
    for (i = 0; i < n; i++) {
        bones[i]->nbones = n;
        bones[i]->nextIdx = -1;
        bones[i]->childIdx = -1;
        bones[i]->next = NULL;
        bones[i]->child = NULL;
        bones[i]->unkD4 = 0;
        bones[i]->unkD8 = 0;
        bones[i]->unk178 = 0;
        bones[i]->unk17C = 0;
        func_0018F7C0((f32 *)bones[i]->local);
    }
    for (i = 0; i < n; i++) {
        func_00193570(bones[i], s, i);
    }
    func_00193070(bones, bones[0], NULL);
    switch (mode) {
    case 0:
        func_00193110(bones[0]);
        break;
    case 1:
        func_00193220(bones[0]);
        break;
    }
    return 1;
}

/* Link bone b (and, through it, its children and siblings) to its parent. */
void func_00193070(FlBone **bones, FlBone *b, FlBone *parent) {
    if (parent != NULL) {
        b->parent = parent;
    }
    if (b->childIdx != -1) {
        b->child = bones[b->childIdx];
        func_00193070(bones, b->child, b);
    }
    if (b->nextIdx != -1) {
        b->next = bones[b->nextIdx];
        func_00193070(bones, b->next, parent);
    }
}

/* Pose the tree from b at scale 1 (the two ways). */
s32 func_00193110(FlBone *b) {
    return func_00193130(b, 1.0f, 1.0f, 1.0f, (f32 *)D_00277260);
}

void func_0018F830(f32 x, f32 y, f32 z, f32 *m);
void func_001903E0(f32 rx, f32 ry, f32 rz, f32 *m);
void func_00190C50(f32 *out, const f32 *a, const f32 *b);
void func_00190E00(f32 *out, const f32 *m);
void func_00190CD0(f32 *a, const f32 *b);
s32 func_001933A0(f32 *m, f32 *v, f32 sx, f32 sy, f32 sz);
s32 func_00193340(f32 *m, f32 *v);
s32 func_00193490(f32 *m, f32 *v, f32 sx, f32 sy, f32 sz);

/* The bind pose of the tree from b: each bone's matrix (scale from the parents' scales) times its
   parent's. */
s32 func_00193130(FlBone *b, f32 sx, f32 sy, f32 sz, f32 *parent) {
    f32 m[4][4];
    s32 i;

    for (i = 0; i < 10; i++) {
        (&b->cur[0][0])[i] = (&b->bind[0][0])[i];
    }
    func_001933A0((f32 *)b->local, (f32 *)b->cur, sx, sy, sz);
    func_00190C50((f32 *)m, (f32 *)b->local, parent);
    func_00190E00((f32 *)b->pose, (f32 *)m);
    if (b->child != NULL) {
        func_00193130(b->child, sx * b->cur[0][0], sy * b->cur[0][1], sz * b->cur[0][2], (f32 *)m);
    }
    if (b->next != NULL) {
        func_00193130(b->next, sx, sy, sz, parent);
    }
    return 1;
}

s32 func_00193220(FlBone *b) {
    return func_00193240(b, 1.0f, 1.0f, 1.0f, (f32 *)D_00277260);
}

/* The same with the local matrix only rotation and translation (the scale applied separately). */
s32 func_00193240(FlBone *b, f32 sx, f32 sy, f32 sz, f32 *parent) {
    f32 m[4][4];
    f32 t[4][4];
    s32 i;

    for (i = 0; i < 10; i++) {
        (&b->cur[0][0])[i] = (&b->bind[0][0])[i];
    }
    func_00193340((f32 *)b->local, (f32 *)b->cur);
    func_00193490((f32 *)t, (f32 *)b->cur, sx, sy, sz);
    func_00190C50((f32 *)m, (f32 *)t, parent);
    func_00190E00((f32 *)b->pose, (f32 *)m);
    if (b->child != NULL) {
        func_00193240(b->child, sx * b->cur[0][0], sy * b->cur[0][1], sz * b->cur[0][2], (f32 *)m);
    }
    if (b->next != NULL) {
        func_00193240(b->next, sx, sy, sz, parent);
    }
    return 1;
}

/* Rotation (v[3..5]) and translation (v[6..8]). */
s32 func_00193340(f32 *m, f32 *v) {
    func_0018F7C0(m);
    func_001903E0(v[3], v[4], v[5], m);
    m[12] = v[6];
    m[13] = v[7];
    m[14] = v[8];
    return 1;
}

/* Scale (v[0..2] times s), rotation, then translation, the parents' scale taken back out. */
s32 func_001933A0(f32 *m, f32 *v, f32 sx, f32 sy, f32 sz) {
    f32 r[4][4];
    f32 t[4][4];

    func_0018F830(v[0] * sx, v[1] * sy, v[2] * sz, m);
    func_0018F7C0((f32 *)r);
    func_001903E0(v[3], v[4], v[5], (f32 *)r);
    func_0018F7C0((f32 *)t);
    t[0][0] = 1.0f / sx;
    t[1][1] = 1.0f / sy;
    t[2][2] = 1.0f / sz;
    t[3][0] = v[6];
    t[3][1] = v[7];
    t[3][2] = v[8];
    func_00190CD0(m, (f32 *)r);
    func_00190CD0(m, (f32 *)t);
    return 1;
}

/* The same without the parents' scale on this bone's own. */
s32 func_00193490(f32 *m, f32 *v, f32 sx, f32 sy, f32 sz) {
    f32 r[4][4];
    f32 t[4][4];

    func_0018F830(v[0], v[1], v[2], m);
    func_0018F7C0((f32 *)r);
    func_001903E0(v[3], v[4], v[5], (f32 *)r);
    func_0018F7C0((f32 *)t);
    t[0][0] = 1.0f / sx;
    t[1][1] = 1.0f / sy;
    t[2][2] = 1.0f / sz;
    t[3][0] = v[6];
    t[3][1] = v[7];
    t[3][2] = v[8];
    func_00190CD0(m, (f32 *)r);
    func_00190CD0(m, (f32 *)t);
    return 1;
}

/* Bone i of the skeleton: its ids, links and vectors. */
s32 func_00193570(FlBone *b, u8 *skel, s32 i) {
    SkelFileBone *e = ((SkelFile *)skel)->bones;

    e += i;

    b->unk174 = e->unkA;
    b->id = e->id;
    b->unkC6 = e->unk2;
    b->childIdx = e->child;
    b->nextIdx = e->next;
    b->cur[0][0] = b->bind[0][0] = e->v[0][0];
    b->cur[0][1] = b->bind[0][1] = e->v[0][1];
    b->cur[0][2] = b->bind[0][2] = e->v[0][2];
    b->cur[1][0] = b->bind[1][0] = e->v[1][0];
    b->cur[1][1] = b->bind[1][1] = e->v[1][1];
    b->cur[1][2] = b->bind[1][2] = e->v[1][2];
    b->cur[2][0] = b->bind[2][0] = e->v[2][0];
    b->cur[2][1] = b->bind[2][1] = e->v[2][1];
    b->cur[2][2] = b->bind[2][2] = e->v[2][2];
    b->unkD8 = 0;
    b->unkD4 = 0;
    return 1;
}
