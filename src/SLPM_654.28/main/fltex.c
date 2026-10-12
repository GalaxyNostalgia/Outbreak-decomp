#include "fltex.h"
#include "sce.h"

/* Capcom's flPS2 texture manager (the error messages name it): loading TIM2 pictures into
   textures and palettes, VRAM allocation and the GS upload packets. */

typedef struct DmaQueue DmaQueue;
extern DmaQueue D_00281AF0;
extern s32 D_0023BAA4;      /* textures registered */
extern s32 D_0023BA64;      /* palettes registered */
extern s32 D_0023BA68;      /* VRAM nodes in use */

s32 func_0018D6D0(u32 size, s32 unused);
u32 func_0018D720(s32 handle);
u32 func_0018D850(u32 size, s32 align);
s32 func_0018CA60(s32 mode, u32 entry, u32 *tag, DmaQueue *q);
s32 func_001A0080(const char *fmt, ...);
void func_0019DA80(s16 w, s16 h, u8 *src, u8 *dst, s32 is8bit);

u128 *func_0018C0B0(u128 *p, s32 qwc, s32 irq, s32 unused);
u128 *func_0018C140(u128 *p, s32 qwc, u32 addr, s32 irq);
u128 *func_0018C1A0(u128 *p, s32 qwc, u32 addr, s32 irq);
u32 *func_0018C280(u32 *tag, s32 qwc, s32 irq, s32 unused);
u128 *func_0018C310(u128 *p, s32 n);
s32 func_0018C330(u32 size);
s32 func_0018C370(u32 size);
u128 *func_0018C380(u128 *p, s32 irq, u32 src, u32 size, s16 dbp, s16 dbw, s16 psm, s16 dsax,
                    s16 dsay, s16 w, s16 h);
void func_0018C7C0(u128 *p, s32 irq);
void func_0018D630(s8 *dst, s8 *src, s32 size);
void func_0018DC30(u8 *p, s32 w, s32 h);
s32 func_0019C780(void);
s32 func_0019CA50(void);
s16 func_0019D100(s16 n);
s32 func_0019D150(s32 psm, s16 w, s16 h, s32 mips);
s16 func_0019D1F0(FlTexture *t);
s16 func_0019D310(FlTexture *t);
s32 func_0019C190(PixFormat *fmt, s32 mips, s32 handle, u32 owner);
s32 func_0019C7D0(PixFormat *fmt, s32 handle, u32 owner);
s16 func_0019C6A0(FlTexture *t, u32 mip);
void func_0019C4A0(FlTexture *t);
FlVramNode *func_0019E140(void);
void func_0019E220(FlVramNode *after, FlVramNode *node);
s32 func_0019E2B0(FlVramNode *after, FlTexture *t);
FlVramNode *func_0019E660(s16 size, s16 align);
s32 func_0019E520(s32 *handles, s32 n);
s32 func_0019C3A0(s32 handle, u32 owner);
s32 func_0019C940(s32 handle, u32 owner);

/* --- DMA/VIF/GIF packet pieces --- */

/* DMA cnt tag: qwc quadwords follow. */
u128 *func_0018C0B0(u128 *p, s32 qwc, s32 irq, s32 unused) {
    *p = U128_ZERO;
    *(u32 *)p = qwc + 0x10000000;
    if (irq == 1) {
        *(u32 *)p |= 0x80000000;
    }
    return p + 1;
}

#ifdef NON_MATCHING   /* first draft */
/* DMA ref tag: qwc quadwords from addr. */
u128 *func_0018C140(u128 *p, s32 qwc, u32 addr, s32 irq) {
    u32 spr = 0;

    if ((addr & 0x70000000) == 0x70000000) {
        spr = 0x80000000;
    }
    *p = U128_ZERO;
    *(u32 *)p = qwc + 0x30000000;
    ((u32 *)p)[1] = (addr & 0x0FFFFFFF) | spr;
    if (irq == 1) {
        *(u32 *)p |= 0x80000000;
    }
    return p + 1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* DMA refe tag: qwc quadwords from addr, then the chain ends. */
u128 *func_0018C1A0(u128 *p, s32 qwc, u32 addr, s32 irq) {
    u32 spr = 0;

    if ((addr & 0x70000000) == 0x70000000) {
        spr = 0x80000000;
    }
    *p = U128_ZERO;
    *(u32 *)p = qwc;
    ((u32 *)p)[1] = (addr & 0x0FFFFFFF) | spr;
    if (irq == 1) {
        *(u32 *)p |= 0x80000000;
    }
    return p + 1;
}
#endif

/* VIF DIRECT of n quadwords (after three NOPs). */
u128 *func_0018C310(u128 *p, s32 n) {
    ((u32 *)p)[0] = 0;
    ((u32 *)p)[1] = 0;
    ((u32 *)p)[2] = 0;
    ((u32 *)p)[3] = n | 0x51000000;
    return p + 1;
}

/* Bytes of upload packet for an image of size bytes (0x70000 bytes a transfer). */
s32 func_0018C330(u32 size) {
    u32 n = size / 0x70000;

    if (size % 0x70000 != 0) {
        n++;
    }
    return n << 7;
}

s32 func_0018C370(u32 size) {
    return 0x40;
}

/* GS image upload: src (size bytes, w x h pixels of psm) to VRAM block dbp. Split into
   transfers of up to 0x70000 bytes. Returns the last DMA tag. */
#ifdef NON_MATCHING   /* first draft */
u128 *func_0018C380(u128 *p, s32 irq, u32 src, u32 size, s16 dbp, s16 dbw, s16 psm, s16 dsax,
                    s16 dsay, s16 w, s16 h) {
    u32 n = size / 0x70000;
    u128 *last = NULL;
    s16 rows;
    s32 bytes;
    u64 bitbltbuf;
    u64 trxpos;
    u32 i;
    s32 qwc;

    if (size % 0x70000 != 0) {
        n++;
    }
    if (n == 1) {
        rows = h;
    } else {
        switch (psm) {
        case PSMCT32:
        case PSMCT24:
            rows = 0x70000 / (w * 4);
            break;
        case PSMCT16:
        case PSMCT16S:
            rows = 0x70000 / (w * 2);
            break;
        case PSMT8:
            rows = 0x70000 / w;
            break;
        case PSMT4:
            rows = 0x70000 / (w >> 1);
            break;
        }
    }
    if (dbw == 0) {
        dbw = 1;
    }
    trxpos = (u64)dsax << 32;
    bitbltbuf = ((u64)psm << 56) | ((u64)dbp << 32) | ((u64)dbw << 48);
    for (i = 0; i < n; i++) {
        switch (psm) {
        case PSMCT32:
        case PSMCT24:
            bytes = w * rows * 4;
            break;
        case PSMCT16:
        case PSMCT16S:
            bytes = w * rows * 2;
            break;
        case PSMT8:
            bytes = w * rows;
            break;
        case PSMT4:
            bytes = w * rows / 2;
            break;
        }
        func_0018C0B0(p, 6, 0, 0);
        ((u32 *)p)[2] = 0x13000000;
        ((u32 *)p)[3] = 0x51000006;
        ((u64 *)p)[2] = 4 | ((u64)0x10000000 << 32);
        ((u64 *)p)[3] = 0xE;
        ((u64 *)p)[4] = bitbltbuf;
        ((u64 *)p)[5] = 0x50;
        ((u64 *)p)[6] = trxpos | ((u64)dsay << 48);
        ((u64 *)p)[7] = 0x51;
        ((u64 *)p)[8] = w | ((u64)rows << 32);
        ((u64 *)p)[9] = 0x52;
        ((u64 *)p)[10] = 0;
        ((u64 *)p)[11] = 0x53;
        if (i >= n - 1) {
            qwc = size >> 4;
            last = p + 7;
            ((u64 *)p)[12] = qwc | (0x8000 | ((u64)0x08000000 << 32));
            ((u64 *)p)[13] = 0;
            func_0018C1A0(last, qwc, src, irq);
            ((u32 *)p)[0x1E] = 0;
            ((u32 *)p)[0x1F] = qwc | 0x51000000;
        } else {
            ((u64 *)p)[12] = 0xF000 | ((u64)0x08000000 << 32);
            ((u64 *)p)[13] = 0;
            func_0018C140(p + 7, 0x7000, src, 0);
            ((u32 *)p)[0x1E] = 0;
            ((u32 *)p)[0x1F] = 0x51007000;
            src += bytes;
            dsay += rows;
            p += 8;
            if (h < dsay + rows) {
                rows = h - dsay;
            }
            size -= bytes;
        }
    }
    return last;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* TEXFLUSH packet. */
void func_0018C7C0(u128 *p, s32 irq) {
    func_0018C280((u32 *)p, 3, irq, 0);
    func_0018C310(p + 1, 2);
    ((u64 *)p)[4] = 0x8001 | ((u64)0x10000000 << 32);
    ((u64 *)p)[5] = 0xE;
    ((u64 *)p)[6] = 0;
    ((u64 *)p)[7] = 0x3F;
}
#endif

/* --- textures --- */

/* PS2 alpha is 0-128: halves 32-bit pixels' alpha (255 becomes 128, others at least 1). */
void func_0018DC30(u8 *p, s32 w, s32 h) {
    s32 x, y;
    u8 a;

    for (y = 0; y < h; y++) {
        for (x = 0; x < w; x++) {
            a = p[3];
            if (a == 0xFF) {
                a = 0x80;
            } else if (a != 0) {
                a >>= 1;
                if (a == 0) {
                    a = 1;
                }
            }
            p[3] = a;
            p += 4;
        }
    }
}

/* A free texture slot (+1; 0x101 when full). */
s32 func_0019C780(void) {
    s32 i;

    for (i = 0; i < 0x100; i++) {
        if (!D_0027AAC0[i].used) {
            break;
        }
    }
    return i + 1;
}

/* A free palette slot, as a handle's high half. */
s32 func_0019CA50(void) {
    s32 i;

    for (i = 0; i < 0x100; i++) {
        if (!D_002772C0[i].used) {
            break;
        }
    }
    return (i + 1) << 16;
}

#ifdef NON_MATCHING   /* first draft */
/* log2 of a power of two from 2 to 1024 (1 otherwise). */
s16 func_0019D100(s16 n) {
    s32 i;

    for (i = 1; i < 11; i++) {
        if (n <= 1 << i) {
            return i;
        }
    }
    return 1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Bytes of pixels, every mipmap level. */
s32 func_0019D150(s32 psm, s16 w, s16 h, s32 mips) {
    s32 size = 0;
    s32 i;

    for (i = 0; i < mips; i++) {
        switch (psm) {
        case PSMCT32:
        case PSMCT24:
            size += w * h * 4;
            break;
        case PSMCT16:
        case PSMCT16S:
            size += w * h * 2;
            break;
        case PSMT8:
            size += w * h;
            break;
        case PSMT4:
            size += w * h >> 1;
            break;
        }
        w >>= 1;
        h >>= 1;
    }
    return size;
}
#endif

/* VRAM blocks a texture needs: whole pages per mipmap level. */
s16 func_0019D1F0(FlTexture *t) {
    s16 pw, ph;
    s16 w, h;
    s16 n;
    u32 i;

    switch (t->psm) {
    case PSMCT32:
    case PSMCT24:
        pw = 0x40;
        ph = 0x20;
        break;
    case PSMCT16:
    case PSMCT16S:
        pw = 0x40;
        ph = 0x40;
        break;
    case PSMT8:
        pw = 0x80;
        ph = 0x40;
        break;
    case PSMT4:
        pw = 0x80;
        ph = 0x80;
        break;
    }
    w = t->w;
    h = t->h;
    n = 0;
    for (i = 0; i < t->mips; i++) {
        n += (((w + pw - 1) / pw) * ((h + ph - 1) / ph)) << 5;
        w >>= 1;
        h >>= 1;
    }
    return n;
}

/* VRAM blocks a palette needs. */
s16 func_0019D310(FlTexture *t) {
    if (t->h == 1) {
        return 2;
    }
    switch (t->psm) {
    case PSMCT32:
    case PSMCT24:
        return 4;
    case PSMCT16:
        return 4;
    }
}

#ifdef NON_MATCHING   /* first draft */
/* flPS2GetTextureInfoFromContext: fills texture slot (handle) from a pixel format. */
s32 func_0019C190(PixFormat *fmt, s32 mips, s32 handle, u32 owner) {
    FlTexture *t = &D_0027AAC0[(handle & 0xFFFF) - 1];
    s32 bpp;

    t->used = 1;
    t->owner = owner;
    t->flags = fmt->flags;
    t->w = fmt->width;
    t->h = fmt->height;
    t->swizzled = 0;
    t->mem = 0;
    t->unk2C = 0;
    t->unk37 = 0;
    t->mips = mips;
    switch (fmt->bpp) {
    case 4:
        t->psm = PSMCT32;
        t->bpp = 4;
        break;
    case 3:
        t->psm = PSMCT24;
        t->bpp = 3;
        break;
    case 2:
        t->psm = PSMCT16;
        t->bpp = 2;
        break;
    case 1:
        t->psm = PSMT8;
        t->bpp = 1;
        break;
    case 0:
        t->psm = PSMT4;
        t->bpp = 0;
        break;
    }
    switch (fmt->width) {
    case 0x20:
    case 0x40:
    case 0x80:
    case 0x100:
    case 0x200:
    case 0x400:
        break;
    default:
        func_001A0080("Not supported width...%d @flPS2GetTextureInfoFromContext", fmt->width);
        return 0;
    }
    t->tw = func_0019D100(t->w);
    switch (fmt->height) {
    case 0x20:
    case 0x40:
    case 0x80:
    case 0x100:
    case 0x200:
    case 0x400:
        break;
    default:
        func_001A0080("Not supported height...%d @flPS2GetTextureInfoFromContext", fmt->height);
        return 0;
    }
    t->th = func_0019D100(t->h);
    t->blocks = func_0019D1F0(t);
    t->align = 0x20;
    t->size = func_0019D150(t->psm, t->w, t->h, t->mips);
    return 1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* flCreatePaletteHandle: fills palette slot (handle's high half). */
s32 func_0019C7D0(PixFormat *fmt, s32 handle, u32 owner) {
    FlTexture *t = &D_002772C0[((u32)(handle & 0xFFFF0000) >> 16) - 1];

    if (fmt->height != 1) {
        func_001A0080("Supported only 1 palette. Unallocatable. @flCreatePaletteHandle");
        return 0;
    }
    switch (fmt->bpp) {
    case 4:
        t->psm = PSMCT32;
        t->bpp = 4;
        break;
    case 3:
        t->psm = PSMCT24;
        t->bpp = 3;
        break;
    case 2:
        t->psm = PSMCT16;
        t->bpp = 2;
        break;
    default:
        func_001A0080("Not supported texture bit depth @flCreatePaletteHandle");
        return 0;
    }
    if (fmt->width == 0x100) {
        t->w = 0x10;
        t->h = 0x10;
    } else {
        t->w = 8;
        t->h = 2;
    }
    t->flags = fmt->flags;
    t->owner = owner;
    t->used = 1;
    t->tw = func_0019D100(t->w);
    t->th = func_0019D100(t->h);
    t->swizzled = 0;
    t->mem = 0;
    t->unk2C = 0;
    t->unk37 = 0;
    t->mips = 1;
    t->blocks = func_0019D310(t);
    t->align = 1;
    t->size = func_0019D150(t->psm, t->w, t->h, t->mips);
    return 1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* VRAM block address of a mipmap level (-1 if there's no such level). */
s16 func_0019C6A0(FlTexture *t, u32 mip) {
    s16 w, h;
    s16 addr;
    s32 i;
    s32 bytes;

    if (mip >= t->mips) {
        return -1;
    }
    w = t->w;
    h = t->h;
    addr = t->vram;
    for (i = 0; i < (s32)mip; i++) {
        switch (t->psm) {
        case PSMT4:
            bytes = w * h >> 1;
            break;
        case PSMT8:
            bytes = w * h;
            break;
        case PSMCT16:
            bytes = w * h * 2;
            break;
        case PSMCT24:
            bytes = w * h * 4;
            break;
        case PSMCT32:
            bytes = w * h * 4;
            break;
        default:
            return -1;
        }
        addr += (bytes + 0xFF) / 0x100;
        w >>= 1;
        h >>= 1;
    }
    return addr;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Queues the GS uploads of every mipmap level of a texture already given VRAM. */
void func_0019C4A0(FlTexture *t) {
    s16 w, h;
    u32 src;
    u32 i;
    s32 bytes;
    u128 *pkt;
    u128 *last;
    s16 dbp;

    src = func_0018D720(t->mem);
    w = t->w;
    h = t->h;
    for (i = 0; i < t->mips; i++) {
        switch (t->psm) {
        case PSMCT32:
            bytes = w * h * 4;
            break;
        case PSMCT24:
            bytes = w * h * 4;
            break;
        case PSMCT16:
            bytes = w * h * 2;
            break;
        case PSMT8:
            bytes = w * h;
            break;
        case PSMT4:
            bytes = w * h >> 1;
            break;
        }
        pkt = (u128 *)func_0018D850(func_0018C330(bytes), 0x10);
        dbp = func_0019C6A0(t, i);
        if (!t->swizzled) {
            last = func_0018C380(pkt, 1, src, bytes, dbp, w / 64, t->psm, 0, 0, w, h);
        } else {
            last = func_0018C380(pkt, 1, src, bytes, dbp, t->sw / 64, PSMCT32, 0, 0, t->sw, t->sh);
        }
        func_0018CA60(0, ((u32)pkt & 0x0FFFFFFF) | 0x10000000, (u32 *)last, &D_00281AF0);
        w >>= 1;
        h >>= 1;
        src += bytes;
    }
}
#endif

/* --- VRAM --- */

#ifdef NON_MATCHING   /* first draft */
/* Links node in after `after` (at the front for NULL). */
void func_0019E220(FlVramNode *after, FlVramNode *node) {
    FlVramNode *next = NULL;

    if (after != NULL) {
        next = after->next;
        after->next = node;
    } else {
        if (D_0023BA6C != NULL) {
            next = D_0023BA6C;
        }
        D_0023BA6C = node;
    }
    node->prev = after;
    node->next = next;
    if (next != NULL) {
        next->prev = node;
    }
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Gives texture t VRAM after node `after`. */
s32 func_0019E2B0(FlVramNode *after, FlTexture *t) {
    FlVramNode *node;
    s16 addr;
    s32 a;

    if (after == NULL) {
        node = func_0019E140();
        if (node == NULL) {
            return 0;
        }
        func_0019E220(after, node);
        addr = D_00281DF8;
    } else {
        node = func_0019E140();
        if (node == NULL) {
            return 0;
        }
        func_0019E220(after, node);
        a = t->align - 1;
        addr = ~a & (after->addr + after->size + a);
    }
    t->vram = addr;
    t->inVram = 1;
    node->tex = t;
    node->flags = t->flags;
    node->owner = t->owner;
    node->addr = addr;
    node->size = t->blocks;
    node->align = t->align;
    return 1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Where size blocks (aligned) fit: the node to allocate after, NULL for the front, -1 none. */
FlVramNode *func_0019E660(s16 size, s16 align) {
    FlVramNode *n = D_0023BA6C;
    FlVramNode *next;
    s32 end;
    u32 start;
    s32 a;

    if (n == NULL) {
        return NULL;
    }
    if (n->addr != D_00281DF8 && (u32)(n->addr - D_00281DF8) >= (u32)size) {
        return NULL;
    }
    a = align - 1;
    for (;;) {
        next = n->next;
        end = n->addr + n->size;
        start = ~a & (end + a);
        if (next == NULL) {
            if (end >= 0x4000) {
                return (FlVramNode *)-1;
            }
            if (start + size >= 0x4000) {
                return (FlVramNode *)-1;
            }
            return n;
        }
        if (start < (u32)next->addr && (u32)(next->addr - start) >= (u32)size) {
            return n;
        }
        n = next;
    }
}
#endif

/* Gives each handle's texture and palette VRAM if they haven't any. */
s32 func_0019E520(s32 *handles, s32 n) {
    s32 h;
    s32 i;
    FlTexture *t;
    FlVramNode *at;
    u32 c;

    for (i = 0; i < n; i++) {
        h = *handles++;
        if ((h & 0xFFFF) != 0) {
            t = &D_0027AAC0[(h & 0xFFFF) - 1];
            if (!t->inVram) {
                at = func_0019E660(t->blocks, t->align);
                if (at == (FlVramNode *)-1) {
                    return 0;
                }
                if (func_0019E2B0(at, t) == 0) {
                    return 0;
                }
            }
        }
        c = (u32)(h & 0xFFFF0000) >> 16;
        if (c != 0) {
            t = &D_002772C0[c - 1];
            if (!t->inVram) {
                at = func_0019E660(t->blocks, t->align);
                if (at == (FlVramNode *)-1) {
                    return 0;
                }
                if (func_0019E2B0(at, t) == 0) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

/* Registers texture (handle); unless it's kept in main memory (owner 1 or 4), uploads it now
   (if VRAM is full it's kept in main memory after all). */
#ifdef NON_MATCHING   /* first draft */
s32 func_0019C3A0(s32 handle, u32 owner) {
    FlTexture *t = &D_0027AAC0[(u16)handle - 1];
    u128 *p;

    for (;;) {
        switch (owner) {
        case 1:
        case 4:
            break;
        default:
            if (func_0019E520(&handle, 1) == 0) {
                owner = 4;
                t->owner = 4;
                continue;
            }
            func_0019C4A0(t);
            p = (u128 *)func_0018D850(func_0018C370(t->size), 0x10);
            func_0018C7C0(p, 1);
            func_0018CA60(0, (u32)p & 0x0FFFFFFF, (u32 *)p, &D_00281AF0);
            break;
        }
        break;
    }
    D_0023BAA4++;
    return 1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* The same for a palette. */
s32 func_0019C940(s32 handle, u32 owner) {
    FlTexture *t = &D_002772C0[((u32)(handle & 0xFFFF0000) >> 16) - 1];
    u128 *p;

    for (;;) {
        switch (owner) {
        case 1:
        case 4:
            break;
        default:
            if (func_0019E520(&handle, 1) == 0) {
                owner = 4;
                t->owner = 4;
                continue;
            }
            func_0019C4A0(t);
            p = (u128 *)func_0018D850(func_0018C370(t->size), 0x10);
            func_0018C7C0(p, 1);
            func_0018CA60(0, (u32)p & 0x0FFFFFFF, (u32 *)p, &D_00281AF0);
            break;
        }
        break;
    }
    D_0023BA64++;
    return 1;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Loads a TIM2 picture into a new texture (and palette). Returns the handle, 0 on failure. */
s32 func_0018D8C0(Tim2File *tim, u32 owner) {
    PixFormat fmt[8];
    PixFormat cfmt;
    s32 handle;
    s32 mips;
    FlTexture *t;
    FlTexture *c;
    s16 w, h;
    u8 *dst;
    s32 i;
    s32 bytes;
    s32 pal = 0;

    handle = func_0019C780();
    t = &D_0027AAC0[(handle & 0xFFFF) - 1];
    mips = func_001A47A0(tim, (handle & 0xFFFF) - 1);
    if (func_001A47F0(fmt, tim) == 0) {
        return 0;
    }
    func_0019C190(fmt, mips + 1, handle, owner);
    t->mem = func_0018D6D0(t->size, 2);
    dst = (u8 *)func_0018D720(t->mem);
    w = t->w;
    h = t->h;
    for (i = 0; i <= mips; i++) {
        switch (fmt[i].bpp) {
        case 0:
            bytes = w * h >> 1;
            if (!t->swizzled) {
                func_0018D630((s8 *)dst, (s8 *)func_001A4CB0(tim, i), bytes);
            } else {
                func_0019DA80(w, h, func_001A4CB0(tim, i), dst, 0);
            }
            break;
        case 1:
            bytes = w * h;
            if (!t->swizzled) {
                func_0018D630((s8 *)dst, (s8 *)func_001A4CB0(tim, i), bytes);
            } else {
                func_0019DA80(w, h, func_001A4CB0(tim, i), dst, 1);
            }
            break;
        case 2:
            bytes = w * h * 2;
            func_0018D630((s8 *)dst, (s8 *)func_001A4CB0(tim, i), bytes);
            break;
        case 3:
            bytes = w * h * 4;
            func_0018D630((s8 *)dst, (s8 *)func_001A4CB0(tim, i), bytes);
            break;
        case 4:
            bytes = w * h * 4;
            func_0018D630((s8 *)dst, (s8 *)func_001A4CB0(tim, i), bytes);
            func_0018DC30(dst, w, h);
            break;
        }
        w >>= 1;
        h >>= 1;
        dst += bytes;
        if (0) {
        }
    }
    func_0019C3A0(handle, owner);
    if (t->psm == PSMT4 || t->psm == PSMT8) {
        pal = func_0019CA50();
        c = &D_002772C0[((u32)(pal & 0xFFFF0000) >> 16) - 1];
        func_001A4A90(&cfmt, tim, ((u32)(pal & 0xFFFF0000) >> 16) - 1);
        func_0019C7D0(&cfmt, pal, owner);
        c->mem = func_0018D6D0(c->size, 2);
        dst = (u8 *)func_0018D720(c->mem);
        func_0018D630((s8 *)dst, (s8 *)func_001A4CC0(tim), c->size);
        if (cfmt.bpp == 4) {
            func_0018DC30(dst, c->w, c->h);
        }
        func_0019C940(pal, owner);
    }
    return handle | pal;
}
#endif

/* Describes a texture's pixels. */
s32 func_0019CF50(PixFormat *fmt, s32 handle) {
    FlTexture *t = &D_0027AAC0[(handle & 0xFFFF) - 1];

    fmt->width = t->w;
    fmt->height = t->h;
    fmt->bpp = t->bpp;
    fmt->pitch = fmt->width * fmt->bpp;
    switch (t->psm) {
    case PSMCT32:
        fmt->r.bits = 8; fmt->r.shift = 16; fmt->r.mask = 0xFF;
        fmt->g.bits = 8; fmt->g.shift = 8; fmt->g.mask = 0xFF;
        fmt->b.bits = 8; fmt->b.shift = 0; fmt->b.mask = 0xFF;
        fmt->a.bits = 8; fmt->a.shift = 24; fmt->a.mask = 0xFF;
        fmt->r.shift = 0; fmt->b.shift = 16;
        break;
    case PSMCT24:
        fmt->r.bits = 8; fmt->r.shift = 16; fmt->r.mask = 0xFF;
        fmt->g.bits = 8; fmt->g.shift = 8; fmt->g.mask = 0xFF;
        fmt->b.bits = 8; fmt->b.shift = 0; fmt->b.mask = 0xFF;
        fmt->a.bits = 0; fmt->a.shift = 0; fmt->a.mask = 0;
        fmt->r.shift = 0; fmt->b.shift = 16;
        break;
    case PSMCT16:
        fmt->r.bits = 5; fmt->r.shift = 10; fmt->r.mask = 0x1F;
        fmt->g.bits = 5; fmt->g.shift = 5; fmt->g.mask = 0x1F;
        fmt->b.bits = 5; fmt->b.shift = 0; fmt->b.mask = 0x1F;
        fmt->a.bits = 1; fmt->a.shift = 15; fmt->a.mask = 1;
        fmt->r.shift = 0; fmt->b.shift = 10;
        fmt->g.bits = 5; fmt->g.mask = 0x1F;
        break;
    case PSMT8:
    case PSMT4:
        fmt->r.mask = 0; fmt->r.shift = 0; fmt->r.bits = 0;
        fmt->g.mask = 0; fmt->g.shift = 0; fmt->g.bits = 0;
        fmt->b.mask = 0; fmt->b.shift = 0; fmt->b.bits = 0;
        fmt->a.mask = 0; fmt->a.shift = 0; fmt->a.bits = 0;
        break;
    }
    return 1;
}

void func_0019E1A0(FlVramNode *e);

#ifdef NON_MATCHING   /* only used by the non-matching func_0019E730 */
static s32 in_handles(FlVramNode *node, s32 *handles, s32 n) {
    s32 i;
    s32 h;
    u32 c;

    for (i = 0; i < n; i++) {
        h = handles[i];
        if ((h & 0xFFFF) != 0 && &D_0027AAC0[(h & 0xFFFF) - 1] == node->tex) {
            return 1;
        }
        c = (u32)(h & 0xFFFF0000) >> 16;
        if (c != 0 && &D_002772C0[c - 1] == node->tex) {
            return 1;
        }
    }
    return 0;
}
#endif

#define EVICTABLE(pass, node) ((pass) != 0 || (node)->owner == 1 || (node)->owner == 4)

/* Where t can go by throwing out other textures (none of handles): first only ones kept in
   main memory anyway, then any. Returns the node to take over (1: room at the end, -1: none). */
#ifdef NON_MATCHING   /* first draft */
FlVramNode *func_0019E730(FlTexture *t, s32 *handles, s32 n) {
    FlVramNode *node;
    FlVramNode *prev;
    FlVramNode *q;
    s32 pass = 0;
    s32 a;
    s32 start;

again:
    node = D_0023BA6C;
    if (node == NULL) {
        return NULL;
    }
    for (;;) {
        prev = node;
        if (EVICTABLE(pass, node) && !in_handles(node, handles, n)) {
            a = t->align - 1;
            start = ~a & (node->addr + a);
            if (node->addr == start) {
                q = node->next;
                for (;;) {
                    if (q == NULL) {
                        if ((u32)(0x4000 - start) >= (u32)t->blocks) {
                            return node;
                        }
                        break;
                    }
                    if ((u32)(q->addr - start) >= (u32)t->blocks) {
                        return node;
                    }
                    prev = q;
                    if (!EVICTABLE(pass, q) || in_handles(q, handles, n)) {
                        break;
                    }
                    q = q->next;
                }
            }
        }
        node = prev->next;
        if (node == NULL) {
            if (0x4000 - (prev->addr + prev->size) >= t->blocks) {
                return (FlVramNode *)1;
            }
            if (pass == 0) {
                pass++;
                goto again;
            }
            return (FlVramNode *)-1;
        }
    }
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Gives t the VRAM at node, throwing out what's there and after it as far as needed. */
s32 func_0019E3B0(FlVramNode *node, FlTexture *t) {
    if (node == NULL) {
        node = func_0019E140();
        if (node == NULL) {
            return 0;
        }
        func_0019E220(NULL, node);
        node->addr = D_00281DF8;
    } else {
        while (node->next != NULL && node->next->addr - node->addr < t->blocks) {
            func_0019E1A0(node->next);
        }
    }
    if (node->tex != NULL) {
        node->tex->inVram = 0;
        node->tex->owner = 4;
    }
    t->vram = node->addr;
    t->inVram = 1;
    node->tex = t;
    node->flags = t->flags;
    node->owner = t->owner;
    node->size = t->blocks;
    node->align = t->align;
    return 1;
}
#endif

/* Makes the textures and palettes of n handles resident in VRAM (uploading what isn't).
   Returns how many were uploaded. */
#ifdef NON_MATCHING   /* first draft */
s32 func_0019CD70(s32 n, s32 *handles) {
    FlTexture *last = NULL;
    FlTexture *t;
    FlTexture *pal;
    FlVramNode *at;
    u32 tex, c;
    s32 count = 0;
    s32 i, k;
    u128 *p;

    for (i = 0; i < n; i++) {
        tex = handles[i] & 0xFFFF;
        c = (u32)(handles[i] & 0xFFFF0000) >> 16;
        pal = &D_002772C0[c - 1];
        for (k = 0; k < 2; k++) {
            if (k == 0) {
                if (tex == 0 || tex >= 0x100) {
                    continue;
                }
                t = &D_0027AAC0[tex - 1];
            } else {
                if (c == 0 || c >= 0x100) {
                    continue;
                }
                t = pal;
            }
            if (!t->inVram) {
                at = func_0019E660(t->blocks, t->align);
                if (at != (FlVramNode *)-1) {
                    func_0019E2B0(at, t);
                } else {
                    do {
                        at = func_0019E730(t, handles, n);
                    } while (at == (FlVramNode *)-1);
                    func_0019E3B0(at, t);
                }
                func_0019C4A0(t);
                last = t;
                count++;
            }
        }
    }
    if (last != NULL) {
        p = (u128 *)func_0018D850(func_0018C370(0), 0x10);
        func_0018C7C0(p, 1);
        func_0018CA60(0, (u32)p & 0x0FFFFFFF, (u32 *)p, &D_00281AF0);
    }
    return count;
}
#endif

s32 func_0019CD60(s32 n, s32 *handles) {
    return func_0019CD70(n, handles);
}
