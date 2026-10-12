#include "tim2.h"

/* TIM2 pictures: finding a picture's header, image (per mipmap level) and palette, and
   describing their pixel formats. Only single-picture files are accepted. */

/* The pixel formats of image types 1-5 (images) and clut types 1-3 (palettes). R and B are
   set the other way round first and then swapped. */
#define SET_16(f) \
    (f)->r.bits = 5; (f)->r.shift = 10; (f)->r.mask = 0x1F; \
    (f)->g.bits = 5; (f)->g.shift = 5; (f)->g.mask = 0x1F; \
    (f)->b.bits = 5; (f)->b.shift = 0; (f)->b.mask = 0x1F; \
    (f)->a.bits = 1; (f)->a.shift = 15; (f)->a.mask = 1; \
    (f)->r.shift = 0; (f)->b.shift = 10
#define SET_24(f) \
    (f)->r.bits = 8; (f)->r.shift = 16; (f)->r.mask = 0xFF; \
    (f)->g.bits = 8; (f)->g.shift = 8; (f)->g.mask = 0xFF; \
    (f)->b.bits = 8; (f)->b.shift = 0; (f)->b.mask = 0xFF; \
    (f)->a.bits = 0; (f)->a.shift = 0; (f)->a.mask = 0; \
    (f)->r.shift = 0; (f)->b.shift = 16
#define SET_32(f) \
    (f)->r.bits = 8; (f)->r.shift = 16; (f)->r.mask = 0xFF; \
    (f)->g.bits = 8; (f)->g.shift = 8; (f)->g.mask = 0xFF; \
    (f)->b.bits = 8; (f)->b.shift = 0; (f)->b.mask = 0xFF; \
    (f)->a.bits = 8; (f)->a.shift = 24; (f)->a.mask = 0xFF; \
    (f)->r.shift = 0; (f)->b.shift = 16
#define SET_NONE(f) \
    (f)->r.shift = 0; (f)->r.bits = 0; (f)->r.mask = 0; \
    (f)->g.shift = 0; (f)->g.bits = 0; (f)->g.mask = 0; \
    (f)->b.shift = 0; (f)->b.bits = 0; (f)->b.mask = 0; \
    (f)->a.shift = 0; (f)->a.bits = 0; (f)->a.mask = 0

/* Mipmap levels after the first. */
s32 func_001A47A0(Tim2File *tim, s32 unused) {
    if (func_001A4CD0(tim) == 0) {
        return 0;
    }
    return (u8)(func_001A4DE0(tim, 0)->mipmaps - 1);
}

/* Describes each mipmap level's pixels (fmt[0..mipmaps-1]). */
s32 func_001A47F0(PixFormat *fmt, Tim2File *tim) {
    Tim2Picture *ph;
    s32 i;
    s32 w, h;

    if (func_001A4CD0(tim) == 0) {
        return 0;
    }
    ph = func_001A4DE0(tim, 0);
    if (ph->mipmaps > 7) {
        return 0;
    }
    w = ph->width;
    h = ph->height;
    for (i = 0; i < ph->mipmaps; i++) {
        fmt->flags = 0;
        fmt->width = w;
        fmt->height = h;
        switch (ph->imageType) {
        case 4:
            fmt->flags |= 0x14;
            fmt->bpp = 0;
            fmt->pitch = fmt->width >> 1;
            SET_NONE(fmt);
            break;
        case 5:
            fmt->flags |= 4;
            fmt->bpp = 1;
            fmt->pitch = fmt->bpp * fmt->width;
            SET_NONE(fmt);
            break;
        case 1:
            fmt->bpp = 2;
            fmt->pitch = fmt->bpp * fmt->width;
            SET_16(fmt);
            break;
        case 2:
            fmt->bpp = 3;
            fmt->pitch = fmt->bpp * fmt->width;
            SET_24(fmt);
            break;
        case 3:
            fmt->bpp = 4;
            fmt->pitch = fmt->bpp * fmt->width;
            SET_32(fmt);
            break;
        }
        fmt++;
        w >>= 1;
        h >>= 1;
    }
    return 1;
}

/* Describes the palette. */
s32 func_001A4A90(PixFormat *fmt, Tim2File *tim, s32 unused) {
    Tim2Picture *ph;

    if (func_001A4CD0(tim) == 0) {
        return 0;
    }
    ph = func_001A4DE0(tim, 0);
    if (ph->mipmaps > 7) {
        return 0;
    }
    fmt->flags = 0;
    switch (ph->clutType) {
    case 1:
        fmt->bpp = 2;
        SET_16(fmt);
        break;
    case 2:
        fmt->bpp = 3;
        SET_24(fmt);
        break;
    case 3:
        fmt->bpp = 4;
        SET_32(fmt);
        break;
    default:
        return 0;
    }
    switch (ph->imageType) {
    case 4:
        if (ph->clutColors != 0x10) {
            return 0;
        }
        fmt->width = 0x10;
        fmt->height = 1;
        break;
    case 5:
        if (ph->clutColors != 0x100) {
            return 0;
        }
        fmt->width = 0x100;
        fmt->height = 1;
        break;
    default:
        return 0;
    }
    fmt->pitch = fmt->bpp * fmt->width;
    return 1;
}

u8 *func_001A4CB0(Tim2File *tim, s32 mip) {
    return func_001A4E60(tim, 0, mip);
}

u8 *func_001A4CC0(Tim2File *tim) {
    return func_001A4F40(tim, 0);
}

/* A TIM2 file with one picture? */
s32 func_001A4CD0(Tim2File *tim) {
    u32 fmt;

    if (tim->magic[0] != 'T' || tim->magic[1] != 'I' || tim->magic[2] != 'M' || tim->magic[3] != '2') {
        return 0;
    }
    fmt = tim->format;
    if (tim->version != 3) {
        if (tim->version != 4 || (fmt & 0xFF) > 1) {
            return 0;
        }
    }
    if (tim->pictures != 1) {
        return 0;
    }
    return 1;
}

/* n rounded up to the file's alignment. */
static u32 func_001A4D70(Tim2File *tim, u32 n) {
    u32 align = tim->format == 0 ? 0x10 : 0x80;

    if (n % align != 0) {
        n += align - (n - align * (n / align)) % align;
    }
    return n;
}

#ifdef NON_MATCHING   /* register allocation around the static alignment helper differs */
Tim2Picture *func_001A4DE0(Tim2File *tim, s32 index) {
    Tim2File *f = tim;
    u8 *p = (u8 *)tim;
    Tim2Picture *ph;
    s32 i;

    p += func_001A4D70(f, 0x10);
    ph = (Tim2Picture *)p;
    for (i = 0; i < index; i++) {
        if (i >= tim->pictures) {
            break;
        }
        p += func_001A4D70(tim, 0x30);
        p += ph->totalSize;
        ph = (Tim2Picture *)p;
    }
    return ph;
}
#endif

#ifdef NON_MATCHING   /* register allocation around the static alignment helper differs */
/* The image of a mipmap level. */
u8 *func_001A4E60(Tim2File *tim, s32 index, s32 mip) {
    u32 *sizes[8];
    Tim2Picture *ph = func_001A4DE0(tim, 0);
    u8 *p = (u8 *)ph;
    s32 n = ph->mipmaps;
    s32 hsize = 0x30;
    s32 i;
    s32 off;

    if (n >= 2) {
        for (i = 0; i < n - 1; i++) {
            sizes[i] = (u32 *)((u8 *)ph + 0x30 + i * 4 + 0x10);
        }
        hsize += n < 5 ? 0x20 : 0x30;
    }
    p += func_001A4D70(tim, hsize);
    if (ph->mipmaps >= 2) {
        off = 0;
        for (i = 0; i < mip; i++) {
            off += func_001A4D70(tim, *sizes[i]);
        }
        p += off;
    }
    return p;
}
#endif

#ifdef NON_MATCHING   /* register allocation around the static alignment helper differs */
/* The palette, or NULL. */
u8 *func_001A4F40(Tim2File *tim, s32 index) {
    Tim2Picture *ph = func_001A4DE0(tim, 0);
    u8 *p = (u8 *)ph;
    s32 hsize = 0x30;

    if (ph->mipmaps >= 2) {
        hsize += ph->mipmaps < 5 ? 0x20 : 0x30;
    }
    p += func_001A4D70(tim, hsize);
    if (ph->clutSize != 0) {
        return p + ph->imageSize;
    }
    return NULL;
}
#endif
