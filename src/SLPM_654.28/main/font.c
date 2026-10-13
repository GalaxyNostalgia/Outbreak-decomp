#include "system.h"
#include "sce.h"
#include "drawlist.h"
#include "options.h"
#include "game.h"

/* Text: a font file (DATA\ROM\FONT\*), glyphs uploaded to GS memory as they're used, one sprite per
   character. Strings are Shift-JIS with two tags: <c n> picks a colour, <p n> centres the line. */

s32 strlen(const char *s);
void func_001B4390();

/* Text attributes of the list text currently goes to */
void func_001B4300(s32 size) {
    D_00239F80->size = size;
    D_00239F80->height = size;
}

void func_001B4320(s32 x, s32 y) {
    D_00239F80->x = x;
    D_00239F80->y = y;
}

void func_001B4340(s32 z) {
    D_00239F80->z = z;
}

void func_001B4350(s32 v) {
    D_00239F80->unkA0 = v;
}

void func_001B4370(s32 clut) {
    D_00239F80->clutIndex = clut;
}

void func_001B4380(u32 color) {
    D_00239F80->color = color;
}

#ifdef NON_MATCHING   /* first draft */
/* GS packet: an image upload of w x h pixels from src to (dx, dy) in the buffer at dbp. */
u32 *func_001B56E0(u32 *p, u32 src, s32 dbp, s32 dbw, s32 psm, s32 dx, s32 dy, s32 w, s32 h) {
    s32 nibbles;
    u32 n;
    u32 qwc;

    nibbles = 8;
    if ((psm & 7) == 1) {
        nibbles = 6;
    }
    if ((psm & 7) == 2) {
        nibbles = 4;
    }
    if ((psm & 7) == 3) {
        nibbles = 2;
    }
    if ((psm & 7) == 4) {
        nibbles = 1;
    }
    n = nibbles * (w * h);
    qwc = n >> 5;
    if (n & 0x1F) {
        qwc++;
    }
    p[0] = 0x10000007;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0x51000007;
    ((u64 *)p)[2] = 5 | ((u64)0x10000000 << 32);
    ((u64 *)p)[3] = 0xE;
    ((u64 *)p)[4] = 0;
    ((u64 *)p)[5] = 0x3F;
    ((u64 *)p)[6] = ((u64)(u32)dbp << 32) | ((u64)(u32)dbw << 48) | ((u64)(u32)psm << 56);
    ((u64 *)p)[7] = 0x50;   /* BITBLTBUF */
    ((u64 *)p)[8] = ((u64)(u32)dx << 32) | ((u64)(u32)dy << 48);
    ((u64 *)p)[9] = 0x51;   /* TRXPOS */
    ((u64 *)p)[10] = (u32)w | ((u64)(u32)h << 32);
    ((u64 *)p)[11] = 0x52;  /* TRXREG */
    ((u64 *)p)[12] = 0;
    ((u64 *)p)[13] = 0x53;  /* TRXDIR */
    ((u64 *)p)[14] = (s64)(s32)qwc | 0x8000 | ((u64)0x08000000 << 32);
    ((u64 *)p)[15] = 0;
    p[32] = qwc | 0x30000000;
    p[33] = src;
    p[34] = 0;
    p[35] = qwc | 0x51000000;
    return p + 36;
}
#endif

/* GS packet: TEXFLUSH and friends at the start of a list */
u32 *func_001B55A0(u32 *p, s32 size) {
    s32 m = size - 1;

    p[0] = 0x10000003;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0x51000003;
    ((u64 *)p)[2] = 2 | ((u64)0x10000000 << 32);
    ((u64 *)p)[3] = 0xE;
    ((u64 *)p)[4] = 0x44;
    ((u64 *)p)[5] = 0x42;
    ((u64 *)p)[6] = ((s64)m << 14) | 0xA | ((s64)m << 34);
    ((u64 *)p)[7] = 8;
    return p + 16;
}

#ifdef NON_MATCHING   /* first draft */
/* Upload the palettes */
u32 *func_001B5860(DrawList *dl, u32 *p) {
    u32 i = 0;

    for (; i < (u32)dl->cluts; i++) {
        p = func_001B56E0(p, (u32)dl->clut + (i << 6), dl->cbp + i, 1, 0, 0, 0, 8, 2);
    }
    return p;
}
#endif

typedef struct {
    s32 encoding;
    s32 glyphSize;
    s32 packed;
    s32 unk14;
    s32 uploadBytes;
    u8 *clutData;       /* 0x14: 2 bytes per entry */
    s32 cluts;
    s32 glyphs;
    s32 glyphBytes;
    s32 fontSize;       /* 0x24 */
    s32 pad[3];
} FontInfo;             /* 0x34 */

extern FontInfo D_00216A80[];
extern u32 D_00239F78[2];       /* DMA addresses of the two packet buffers */
extern u8 D_002C0FC0[0x24170];
extern u8 D_0029CE40[0x24170];

#ifdef NON_MATCHING   /* first draft */
/* Set up the main list for font `mode` loaded at `font`. */
void func_001B3B20(s32 mode, u8 *font, s32 maxGlyphs, s32 tbp) {
    FontInfo *f;
    u32 next;
    u32 *c;
    u8 *src;
    u32 i;
    s32 j;
    s32 b;

    memset(&D_0029CD80, 0, sizeof(DrawList) - 8);
    memset(D_002C0FC0, 0, sizeof(D_002C0FC0));
    memset(D_0029CE40, 0, sizeof(D_0029CE40));
    D_0029CD80.mode = mode;
    D_0029CD80.unk9C = 0x24170;
    f = &D_00216A80[D_0029CD80.mode];
    D_0029CD80.encoding = f->encoding;
    D_0029CD80.glyphSize = f->glyphSize;
    D_0029CD80.packed = f->packed;
    D_0029CD80.unk14 = f->unk14;
    D_0029CD80.uploadBytes = f->uploadBytes;
    D_0029CD80.glyphs = f->glyphs;
    D_0029CD80.glyphBytes = f->glyphBytes;
    D_0029CD80.cluts = f->cluts;
    D_0029CD80.maxGlyphs = maxGlyphs;
    D_0029CD80.font = font;
    next = ((u32)font + f->fontSize + 0x3F) & ~0x3F;
    for (b = 0; b < 2; b++) {
        D_0029CD80.glyphBuf[b] = (u8 *)next;
        next = (next + D_0029CD80.uploadBytes * D_0029CD80.maxGlyphs + 0x3F) & ~0x3F;
    }
    D_0029CD80.clut = (u32 *)next;
    src = f->clutData;
    c = D_0029CD80.clut;
    for (i = 0; i < (u32)D_0029CD80.cluts; i++) {
        for (j = 0; j < 16; j++) {
            ((u8 *)c)[3] = src[0];
            ((u8 *)c)[2] = src[1];
            ((u8 *)c)[1] = src[1];
            ((u8 *)c)[0] = src[1];
            src += 2;
            c++;
        }
    }
    D_0029CD80.tbp = tbp;
    D_0029CD80.cbp = tbp + 2;
    for (b = 0; b < 2; b++) {
        D_0029CD80.head[b] = D_00239F78[b];
        D_0029CD80.buf[b] = func_001B5860(&D_0029CD80, func_001B55A0((u32 *)D_00239F78[b], D_0029CD80.glyphSize));
    }
    D_0029CD80.cur = 0;
    D_0029CD80.ptr = D_0029CD80.buf[0];
    D_0029CD80.unk5C = (next + (D_0029CD80.cluts << 6)) - (u32)D_0029CD80.font;
    D_0029CD80.size = D_0029CD80.glyphSize;
    D_0029CD80.x = 0;
    D_0029CD80.y = 0;
    D_0029CD80.z = -2;
    D_0029CD80.clutIndex = 0;
    D_0029CD80.color = 0x80808080;
    D_0029CD80.unk88 = 0;
    func_001B4390(0, D_0029CD80.glyphSize);
}
#endif

s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
extern char *D_00216C20[];      /* font file names */

#ifdef NON_MATCHING   /* first draft */
/* Load font `mode` (top bit: already loaded) and start the text list. */
void func_001B39B0(u32 mode) {
    u8 *buf;
    u32 m;

    D_00306590.unk16 = 0;
    buf = D_00306590.fontBuf;
    m = mode & 0x7FFFFFFF;
    if (D_00306590.unk2B == 1 && m == D_0029CD80.mode) {
        goto ready;
    }
    if (mode & 0x80000000) {
        return;
    }
    if (func_001CFBB0(D_00216C20[m], buf, 1, 0) > 0) {
        D_00306590.unk2B = 1;
    }
ready:
    func_001B3B20(m, buf, 0x200, D_00306590.fontTbp);
    D_00306590.unk16 = 1;
    func_001B4350(0);
}
#endif

/* Per-frame callbacks */
extern s32 D_0023BB30;
extern void (*D_0029B0D0[8])(void);

void func_001AEC40(void (*fn)(void)) {
    s32 i;

    for (i = 0; i < D_0023BB30; i++) {
        if (D_0029B0D0[i] == fn) {
            return;
        }
    }
    if (D_0023BB30 != 8) {
        D_0029B0D0[D_0023BB30] = fn;
        D_0023BB30++;
    }
}

/* Removes a per-frame callback. */
void func_001AECB0(void (*fn)(void)) {
    s32 i;

    for (i = 0; i < D_0023BB30; i++) {
        if (D_0029B0D0[i] == fn) {
            D_0029B0D0[i] = NULL;
            break;
        }
    }
    if (i >= D_0023BB30) {
        return;
    }
    for (; i < D_0023BB30 - 1; i++) {
        D_0029B0D0[i] = D_0029B0D0[i + 1];
    }
    D_0029B0D0[i] = NULL;
    D_0023BB30--;
}

/* Debug text lines */
typedef struct {
    s16 used;           /* 0x00 */
    s16 x;
    s16 y;
    s16 timer;          /* 0x06: frames left */
    u8 size;            /* 0x08 */
    s8 fade;            /* 0x09: 1 fading in, 2 fading out at the end */
    u8 color;           /* 0x0A: alpha */
    u8 prio;            /* 0x0B */
    u8 text[0x80];      /* 0x0C */
} TextLine;             /* 0x8C */

typedef struct {
    u8 unk0[0x10];
    TextLine line[32];  /* 0x10 */
} TextLines;

extern TextLines D_002F7440;
void func_001B3400(void);

void func_001B3920(void) {
    memset(&D_002F7440, 0, sizeof(TextLines));
    func_001AEC40(func_001B3400);
}

void func_001B2C90(void) {
    func_001B39B0(6);
    func_001B3920();
}

#ifdef NON_MATCHING   /* first draft */
/* Hide lines covered by a higher-priority line */
void func_001B3350(void) {
    TextLine *a = D_002F7440.line;
    TextLine *b;
    s32 i;
    s32 j;

    for (i = 0; i < 32; i++, a++) {
        if (a->used == 0) {
            continue;
        }
        for (j = 0, b = D_002F7440.line; j < 32; j++, b++) {
            if (b->used != 0 && b->prio < a->prio && b->y < a->y + a->size && a->y < b->y + b->size) {
                a->used = 0;
                break;
            }
        }
    }
}
#endif

void func_001B3000(s32 x, s32 y, s32 size, u8 *text, u8 color);
extern u8 D_48C024[];   /* (SUBMAIN.BIN) */

void func_001B3400(void) {
    TextLine *l;
    s32 i;

    if (D_00306590.unk16 != 0 && D_48C024[0] == 0) {
        func_001B3350();
        l = D_002F7440.line;
        func_001B4340(-2);
        i = 0;
        do {
            if (l->used != 0) {
                func_001B3000(l->x, l->y, l->size, l->text, l->color);
            }
            i++;
            l++;
        } while (i < 32);
    }
}

s32 func_001A8AD0(s32 id, s32 slot);
void func_001B3960(s32 a);
void func_006AE930(void);           /* GAME.BIN */

void func_001B2D00(void);
#ifdef NON_MATCHING   /* an argument register is set one slot later */
/* Once a frame: the lines' timers and fades. */
void func_001B2D00(void) {
    TextLine *l;
    s32 i;
    s32 busy;
    s32 a;

    if (D_0048BF60.unkC4 != 0) {
        return;
    }
    if (D_00306590.unk0 != 0) {
        D_0048BF60.unkBC = 1;
    }
    if (D_00306590.unk2B == 0) {
        func_001B3960(2);
        func_001AECB0(func_001B3400);
        return;
    }
    busy = func_001A8AD0(2, 0);
    l = D_002F7440.line;
    for (i = 0; i < 32; i++, l++) {
        if (l->used == 0) {
            continue;
        }
        l->timer--;
        if (l->timer == 0) {
            l->used = 0;
            continue;
        }
        if (busy != 0 && D_00306590.unk0 != 0 && l->prio == 0) {
            func_006AE930();
            D_0048BF60.unkBC = 0;
        }
        switch (l->fade) {
        case 0:
            break;
        case 1:
            if (l->color < 100) {
                l->color += 5;
            } else {
                l->color += 3;
            }
            if (l->color >= 0x80) {
                l->color = 0x80;
                l->fade = 2;
            }
            break;
        case 2:
            if (l->timer < 0x1F && l->color != 0) {
                a = l->color - 4;
                if (a < 0) {
                    a = 0;
                }
                l->color = a;
            }
            break;
        }
    }
}
#endif

#define SJIS_LEAD(c) (((c) >= 0x80 && (c) < 0xA0) || ((c) >= 0xE0 && (c) < 0x100))

extern char D_00231CB8[];   /* "<p >" */
extern char D_00231CC0[];   /* "<c >" */

#ifdef NON_MATCHING   /* first draft */
/* Width of a line in half-width units (tags don't count) */
s32 func_001B2EA0(u8 *s) {
    u8 tag[8];
    s32 n = 0;
    s32 i;
    u8 c;

    while ((c = *s++) != 0) {
        if (c == '\n') {
            break;
        }
        if (SJIS_LEAD(c)) {
            n++;
            if (*s++ == 0) {
                break;
            }
            n++;
            continue;
        }
        if (c == '<') {
            tag[0] = c;
            for (i = 1; i < 4 && s[i - 1] != 0; i++) {
                if (SJIS_LEAD(c)) {
                    break;
                }
                tag[i] = s[i - 1];
            }
            tag[i] = 0;
            if (tag[2] != 0) {
                tag[2] = ' ';
            }
            if (strcmp((char *)tag, D_00231CB8) == 0) {
                s += 3;
                continue;
            }
            if (strcmp((char *)tag, D_00231CC0) == 0) {
                s += 3;
                continue;
            }
        }
        n++;
    }
    return n;
}
#endif

extern u32 D_00216980[];    /* colours for <c n> */
extern u32 D_0021699C;      /* default colour */
void func_001B43C0(u8 *s);

#ifdef NON_MATCHING   /* first draft */
/* Draw a string at (x, y); \\n starts a new line at x. */
void func_001B3000(s32 x, s32 y, s32 size, u8 *text, u8 alpha) {
    u8 tag[8];
    u8 buf[0x200];
    u8 *o;
    u32 a;
    s32 colorIndex;
    s32 centre;
    s32 half;
    s32 done;
    s32 cx;
    s32 w;
    s32 i;
    u8 c;
    u8 c2;
    u8 arg;

    if (text == NULL) {
        return;
    }
    func_001B4300(size);
    func_001B4370(0);
    colorIndex = 7;
    cx = x;
    buf[0] = 0;
    o = buf;
    centre = 0;
    func_001B4300(size);
    func_001B4370(0);
    a = (alpha & 0xFF) << 24;
    func_001B4380(a | (D_0021699C & 0xFFFFFF));
    func_001B4320(x, y);
    done = 0;
    half = size / 2;
    for (;;) {
        if (done == 0 && centre != 0) {
            w = func_001B2EA0(text);
            if (w != 0) {
                cx = (640 - w * half) / 2;
                func_001B4320(cx, y);
                done = 1;
            }
        }
        c = *text++;
        if (c == 0 || c == '\r') {
            break;
        }
        if (c == '\n') {
            cx = x;
            y += size;
            done = 0;
            goto flush;
        }
        if (SJIS_LEAD(c)) {
            *o++ = c;
            c2 = *text++;
            if (c2 == 0) {
                break;
            }
            *o++ = c2;
            cx += size >> 0;
            continue;
        }
        if (c == '<') {
            tag[0] = c;
            for (i = 1; i < 4 && text[i - 1] != 0; i++) {
                arg = text[i - 1];
                if (SJIS_LEAD(c)) {
                    break;
                }
                tag[i] = arg;
            }
            tag[i] = 0;
            if (tag[2] != 0) {
                arg = tag[2];
                tag[2] = ' ';
            }
            if (strcmp((char *)tag, D_00231CB8) == 0) {
                text += 3;
                centre = arg - '0';
                goto flush;
            }
            if (strcmp((char *)tag, D_00231CC0) == 0) {
                text += 3;
                colorIndex = arg - '0';
                goto flush;
            }
        }
        *o++ = c;
        cx += size >> 1;
        continue;
    flush:
        *o = 0;
        if (strlen((char *)buf) != 0) {
            func_001B43C0(buf);
            buf[0] = 0;
            o = buf;
        }
        func_001B4380(a | (D_00216980[colorIndex] & 0xFFFFFF));
        func_001B4320(cx, y);
    }
    *o = 0;
    if (strlen((char *)buf) != 0) {
        func_001B43C0(buf);
    }
}
#endif

/* Half-width (ASCII, katakana) to the full-width Shift-JIS character */
extern u8 D_00216B9F[];
extern u8 D_00216BAA[];

#ifdef NON_MATCHING   /* first draft */
s32 func_001B5190(u32 c) {
    c &= 0xFF;
    if (c < 0x21) {
        return 0x8140;
    }
    if (c < 0x7F) {
        return c + 0x7FFF + 0x6D7F;
    }
    if (c < 0xA1) {
        return 0x8140;
    }
    if (c < 0xA6) {
        return D_00216B9F[c] + 0x7FFF + 0x6E01;
    }
    switch (c) {
    case 0xB0:
        return 0xEEBA;
    case 0xDE:
        return 0xEEAA;
    case 0xDF:
        return 0xEEA9;
    }
    return D_00216BAA[c] + 0x7FFF + 0x6F01;
}
#endif

s32 func_001B5250(s32 c) {
    c &= 0xFF;
    if (c < 0x60) {
        return c + 0x7FFF + 0x520;
    }
    if (c < 0x80) {
        return c + 0x7FFF + 0x521;
    }
    if (c == 0xA5) {
        return 0x85A3;
    }
    return c + 0x7FFF + 0x4FF;
}

s32 func_001B5160(u32 c, s32 encoding) {
    if (encoding == 0) {
        return func_001B5190(c);
    }
    return func_001B5250(c);
}

#ifdef NON_MATCHING   /* first draft */
/* Shift-JIS to JIS X 0208 */
u32 func_001B4F60(u32 sj) {
    s32 hi = (sj >> 8) & 0xFF;
    s32 lo = sj & 0xFF;

    if (hi >= 0x81 && hi < 0xA0) {
        hi -= 0x81;
    } else if (hi >= 0xE0 && hi < 0xF0) {
        hi -= 0xC1;
    }
    hi *= 2;
    if (lo >= 0x40 && lo < 0x7F) {
        lo -= 0x40;
    } else if (lo >= 0x80 && lo < 0x9F) {
        lo -= 0x41;
    } else if (lo >= 0x9F && lo < 0xFD) {
        lo -= 0x9F;
        hi++;
    }
    return ((hi + 1) << 8) + lo + 0x2021;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Shift-JIS to JIS (the other mapping) */
u32 func_001B5040(u32 sj) {
    u16 hi = (sj >> 8) & 0xFF;
    u16 lo = sj & 0xFF;
    u16 h;
    u16 l;

    h = (u16)((u16)(hi - (hi < 0xA0 ? 0x71 : 0xB1)) * 2 + 1);
    if (lo >= 0x80) {
        lo--;
    }
    if (lo >= 0x9E) {
        l = lo - 0x7D;
        h++;
    } else {
        l = lo - 0x1F;
    }
    return (h << 8) | l;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Glyph number of a character */
u32 func_001B50D0(u32 c, s32 encoding) {
    u32 j;

    if (encoding == 0) {
        j = func_001B4F60(c);
        return ((j >> 8) - 0x21) * 94 + ((j & 0xFF) - 0x21);
    }
    j = func_001B5040(c);
    return (j & 0xFF) - 0x21 + (((j >> 8) & 0xFF) - 0x21) * 94;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* The glyph's pixels, expanded into this frame's upload buffer when the font is packed */
u8 *func_001B4960(DrawList *dl, s32 glyph) {
    u8 *src = dl->font + glyph * dl->glyphBytes;
    u8 *dst;
    u8 *start;
    s32 i;
    u8 b;

    if (dl->packed == 0 && dl->unk14 == 1) {
        return src;
    }
    dst = dl->glyphBuf[dl->cur] + dl->uploadBytes * dl->count;
    if (dl->packed == 0) {
        memcpy(dst, src, dl->glyphBytes);
        return dst;
    }
    start = dst;
    for (i = 0; i < (s32)((u32)(dl->glyphSize * ((dl->glyphSize + 3) & ~3)) >> 2); i++) {
        b = *src++;
        *(u16 *)dst = ((b & 3) << 12) | ((b & 0xC) << 6) | ((b & 0xC0) >> 6) | (b & 0x30);
        dst += 2;
    }
    return start;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* GS packet: upload the glyph and draw it as a sprite at the list's position */
u32 *func_001B58E0(DrawList *dl, u32 *p, u8 *glyph, s32 half) {
    s32 ox;
    s32 oy;
    s32 x;
    s32 y;
    s32 h;
    f32 sx;
    u32 adv;
    u32 w;
    u32 tw;

    p = func_001B56E0(p, (u32)glyph, dl->tbp, 1, 20, 0, 0, dl->glyphSize, dl->glyphSize);
    ox = (0x1000 - D_00281DB0.width) / 2;
    oy = (0x1000 - D_00281DB0.dispHeight) / 2;
    adv = dl->size >> half;
    y = dl->y * 16;
    sx = D_00281DB0.width / 640.0f;
    h = dl->height * 16;
    w = (u32)(16.0f * (adv * sx));
    x = ox * 16 + (s32)(16.0f * (dl->x * sx));
    if (D_00281DB0.field == 0) {
        y >>= 1;
        h >>= 1;
    }
    y = oy * 16 + y;
    tw = dl->glyphSize * 16;
    p[0] = 0x10000008;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0x51000008;
    ((u64 *)p)[2] = 3 | ((u64)0x10000000 << 32);
    ((u64 *)p)[3] = 0xE;
    ((u64 *)p)[4] = 0;
    ((u64 *)p)[5] = 0x3F;   /* TEXFLUSH */
    ((u64 *)p)[6] = (u32)dl->tbp | 0x55404000 | ((u64)5 << 32) | ((u64)(u32)(dl->cbp + dl->clutIndex) << 37) |
                    ((u64)0x20000000 << 32);
    ((u64 *)p)[7] = 6;      /* TEX0_1 */
    ((u64 *)p)[8] = 0x60;
    ((u64 *)p)[9] = 0x14;   /* TEX1_1 */
    ((u64 *)p)[10] = 0x8001 | ((u64)0x64000000 << 32);
    ((u64 *)p)[11] = 0x535310;
    ((u64 *)p)[12] = 0x156; /* PRIM: sprite, textured, blended, UV */
    ((u64 *)p)[13] = dl->color;
    ((u64 *)p)[14] = 0x80008;
    ((u64 *)p)[15] = ((u64)(u32)dl->z << 32) | ((s64)x | ((s64)y << 16));
    ((u64 *)p)[16] = (u32)((tw >> half) + 8) | ((u64)(u32)(tw + 8) << 16);
    ((u64 *)p)[17] = ((u64)(u32)dl->z << 32) | ((s64)(x + w) | ((s64)(y + h) << 16));
    return p + 36;
}
#endif

s32 func_001B6980(s32 id);

#ifdef NON_MATCHING   /* first draft */
/* Add a string's characters to the list (with a black shadow unless turned off). */
void func_001B43C0(u8 *s) {
    DrawList *dl = &D_0029CD80;
    u32 *p;
    u8 *glyph;
    s32 x0;
    s32 half;
    u32 g;
    s32 ch;
    u8 c;
    u8 c2;
    s32 sx;
    s32 sy;
    s32 sz;
    u32 scol;

    if (D_00306590.unk16 == 0 && D_00306590.unk2A == 0) {
        return;
    }
    x0 = dl->x;
    p = dl->ptr;
    while (dl->count < dl->maxGlyphs) {
        c = *s++;
        if (c == 0) {
            break;
        }
        if (c == '\n') {
            dl->x = x0;
            dl->y += dl->size;
            continue;
        }
        if (SJIS_LEAD(c)) {
            c2 = *s++;
            if (c2 == 0) {
                break;
            }
            half = 0;
            ch = (c << 8) | c2;
        } else {
            ch = func_001B5160(c, dl->encoding);
            half = 1;
        }
        g = func_001B50D0(ch, dl->encoding);
        if (g < (u32)dl->glyphs) {
            glyph = func_001B4960(dl, g);
            if (dl->unkA4 == 0 && D_00306590.online != 3 && func_001B6980(0x50) == 0) {
                sx = dl->x;
                scol = dl->color;
                sy = dl->y;
                sz = dl->z;
                dl->color = scol & 0xFF000000;
                dl->x += 2;
                dl->y += 2;
                dl->z--;
                p = func_001B58E0(dl, p, glyph, half);
                dl->color = scol;
                dl->x = sx;
                dl->y = sy;
                dl->z = sz;
                if (++dl->count >= dl->maxGlyphs) {
                    goto next;
                }
            }
            p = func_001B58E0(dl, p, glyph, half);
            dl->count++;
        }
    next:
        dl->x += dl->size >> half;
    }
    dl->ptr = p;
}
#endif

#ifdef NON_MATCHING   /* close: operand order / scheduling differs */
/* UTF-8: decodes the character at *s and advances *s; -1 for a bad sequence. */
u32 func_001B52A0(u8 **s) {
    u32 c = *(*s)++;
    u32 c1, c2;

    if ((s32)c < 0x80) {
        return c;
    }
    if ((s32)c >= 0xC0 && (s32)c < 0xE0) {
        c1 = **s;
        if (c1 == 0) {
            return -1;
        }
        (*s)++;
        if ((s32)c1 < 0x80 || (s32)c1 >= 0xC0) {
            return -1;
        }
        return ((c & 0x1F) << 6) | (c1 & 0x3F);
    }
    if ((s32)c >= 0xE0 && (s32)c < 0xF0) {
        c1 = **s;
        if (c1 == 0) {
            return -1;
        }
        (*s)++;
        if ((s32)c1 < 0x80 || (s32)c1 >= 0xC0) {
            return -1;
        }
        c2 = **s;
        if (c2 == 0) {
            return -1;
        }
        (*s)++;
        if ((s32)c2 < 0x80 || (s32)c2 >= 0xC0) {
            return -1;
        }
        return ((c & 0xF) << 12) | ((c1 & 0x3F) << 6) | (c2 & 0x3F);
    }
    return -1;
}
#endif

/* The glyph for a Unicode character: a 256-entry page table at the start of the font, then
   pages of 256 u16 glyph numbers (+1, 0 = none). Missing ones become '?' when the font has
   all of Unicode. */
#ifdef NON_MATCHING   /* close: operand order / scheduling differs */
s32 func_001B53D0(DrawList *dl, u32 c) {
    u8 *font = dl->font;
    u8 page = font[(c >> 8) & 0xFF];
    u16 g;

    if (page != 0) {
        g = *(u16 *)((page << 9) + font - 0x100 + (c & 0xFF) * 2);
        if (g != 0) {
            return g - 1;
        }
    }
    if (dl->unicode == 0) {
        return 0;
    }
    return func_001B53D0(dl, '?');
}
#endif

s32 func_001B5440(DrawList *dl, u32 glyph) {
    return glyph < dl->glyphCount;
}

#ifdef NON_MATCHING   /* the Shift-JIS lead-byte test is laid out differently */
/* Bytes in the character at s, | 0x10 when the font has no glyph for it (0 at the end). */
s32 func_001B5450(u8 *s) {
    s32 n;
    DrawList *dl = &D_0029CD80;
    u32 c = *s;
    u32 u;

    if (c == 0) {
        return 0;
    }
    n = 2;
    if (dl->encoding != 2) {
        if ((c >= 0x80 && c < 0xC0) || (c >= 0xE0 && c < 0x100)) {
            return 0x12;
        }
        return 1;
    }
    if (c < 0x80) {
        n = 1;
    } else if (c >= 0xC0 && c < 0xE0) {
    } else if (c >= 0xE0 && c < 0xF0) {
        n = 3;
    } else {
        return 1;
    }
    u = func_001B52A0(&s);
    if (dl->unicode == 0) {
        if (u < 0x10) {
            return n | 0x10;
        }
        if (u < 0x80) {
            return n;
        }
    }
    if (!func_001B5440(dl, func_001B53D0(dl, u))) {
        n |= 0x10;
    }
    return n;
}
#endif
