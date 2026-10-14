#include "system.h"

/* Drawing-state packets: each setter builds a small GS packet (A+D register writes) in the frame's packet
   buffer and queues it. ctx: 0 context 1, 1 context 2, 2 both. */

s32 func_0018CA60(s32 mode, u32 entry, u32 *tag, DmaQueue *q);
u32 func_0018D850(u32 size, s32 align);

#define PHYS(p) ((u32)(p) & 0x0FFFFFFF)

extern s32 D_0023BA74;      /* alpha test reference */
extern s32 D_0023BA9C;      /* frame buffer height */
extern s32 D_0023BAA0;      /* frame buffer width */

/* Not matching yet: helper for the functions below, which are written but not matching yet. */
#ifdef NON_MATCHING
/* Packet header for n register writes: END tag (with interrupt), FLUSHA, DIRECT, GIF tag (A+D). */
static u64 *state_packet(s32 ctx) {
    u64 *p;
    s32 qwc;

    if (ctx != 2) {
        p = (u64 *)func_0018D850(0x30, 0x10);
        qwc = 2;
    } else {
        p = (u64 *)func_0018D850(0x40, 0x10);
        qwc = 3;
    }
    p[0] = (u32)((qwc + 0x70000000) | 0x80000000);
    ((u32 *)p)[2] = 0x13000000;
    ((u32 *)p)[3] = qwc | 0x50000000;
    p[2] = (u32)(qwc - 1) | 0x8000 | ((u64)1 << 60);
    p[3] = 0xE;
    return p;
}
#endif

/* Not matching yet: helper. */
#ifdef NON_MATCHING
static void state_regs(u64 *p, s32 ctx, u64 value, s32 reg1, s32 reg2) {
    switch (ctx) {
    case 0:
        p[4] = value;
        p[5] = reg1;
        break;
    case 1:
        p[4] = value;
        p[5] = reg2;
        break;
    case 2:
        p[4] = value;
        p[5] = reg1;
        p[6] = value;
        p[7] = reg2;
        break;
    }
}
#endif

/* Not matching yet: first draft. */
#ifdef NON_MATCHING
/* SCISSOR: clipped to the frame buffer. */
s32 func_00196C20(s32 x, s32 y, s32 w, s32 h, s32 ctx) {
    s32 x1;
    s32 y1;
    u64 v;
    u64 *p;

    if (x < 0) {
        x = 0;
    }
    if (D_0023BAA0 < x + w) {
        w = D_0023BAA0 - x;
    }
    x1 = x + w - 1;
    if (y < 0) {
        y = 0;
    }
    if (D_0023BA9C < y + h) {
        h = D_0023BA9C - y;
    }
    y1 = y + h - 1;
    p = state_packet(ctx);
    v = ((u64)y1 << 48) | ((u64)y << 32) | (u64)x | ((u64)x1 << 16);
    if (ctx == 0 || ctx == 2) {
        D_00281DB0.scissor1Rect[0] = x;
        D_00281DB0.scissor1Rect[1] = y;
        D_00281DB0.scissor1Rect[2] = x1;
        D_00281DB0.scissor1Rect[3] = y1;
        D_00281DB0.scissor1 = v;
    }
    if (ctx == 1 || ctx == 2) {
        D_00281DB0.scissor2Rect[0] = x;
        D_00281DB0.scissor2Rect[1] = y;
        D_00281DB0.scissor2Rect[2] = x1;
        D_00281DB0.scissor2Rect[3] = y1;
        D_00281DB0.scissor2 = v;
    }
    state_regs(p, ctx, v, 0x40, 0x41);
    func_0018CA60(0, PHYS(p), (u32 *)p, &D_00281AF0);
    return 1;
}
#endif

/* Not matching yet: first draft. */
#ifdef NON_MATCHING
/* ZBUF: flags bit 15 enables Z writes. */
s32 func_00196F00(s32 flags, s32 ctx) {
    u64 v = ((u64)((flags & 0x8000) != 0x8000) << 32) | ((u64)(D_00281DB0.zbp >> 5) | ((u64)D_00281DB0.zPsm << 24));
    u64 *p = state_packet(ctx);

    if (ctx != 1) {
        D_00281DB0.zbuf[0] = v;
    }
    if (ctx != 0) {
        D_00281DB0.zbuf[1] = v;
    }
    state_regs(p, ctx, v, 0x4E, 0x4F);
    func_0018CA60(0, PHYS(p), (u32 *)p, &D_00281AF0);
    return 1;
}
#endif

/* Not matching yet: first draft. */
#ifdef NON_MATCHING
/* TEST: alpha test from flags bits 19-21, depth test from bits 12-14. */
s32 func_00197080(s32 flags, s32 ctx) {
    s32 aref = D_0023BA74;
    s32 atst;
    s32 ztst;
    u64 v;
    u64 *p;

    if (aref == 0xFF) {
        aref = 0x80;
    } else if (aref != 0) {
        aref >>= 1;
        if (aref == 0) {
            aref = 1;
        }
    }
    switch (flags & 0x380000) {
    case 0x380000: atst = 1; break;    /* ALWAYS */
    case 0x080000: atst = 2; break;    /* LESS */
    case 0x180000: atst = 3; break;    /* LEQUAL */
    case 0x100000: atst = 4; break;    /* EQUAL */
    case 0x300000: atst = 5; break;    /* GEQUAL */
    case 0x200000: atst = 6; break;    /* GREATER */
    case 0x280000: atst = 7; break;    /* NOTEQUAL */
    default: atst = 0; break;          /* NEVER */
    }
    switch (flags & 0x7000) {
    case 0x1000: ztst = 3; break;      /* GREATER */
    case 0x3000: ztst = 2; break;      /* GEQUAL */
    case 0x0000: ztst = 0; break;      /* NEVER */
    default: ztst = 1; break;          /* ALWAYS */
    }
    v = ((u64)ztst << 17) | ((u64)aref << 4) | ((u64)atst << 1) | 1 | 0x10000;
    p = state_packet(ctx);
    if (ctx != 1) {
        D_00281DB0.test[0] = v;
    }
    if (ctx != 0) {
        D_00281DB0.test[1] = v;
    }
    state_regs(p, ctx, v, 0x47, 0x48);
    func_0018CA60(0, PHYS(p), (u32 *)p, &D_00281AF0);
    return 1;
}
#endif

#define ALPHA(a, b, c, d) ((a) | ((b) << 2) | ((c) << 4) | ((d) << 6))
#define ALPHA_FIX(a, b, d) (ALPHA(a, b, 2, d) | ((u64)0x80 << 32))

/* ALPHA: blend mode from flags (bits 0-3 the operation, 4-7 the factor, 10-11 the source of alpha).
   Unknown combinations leave the state alone. */
/* Not matching yet: first draft. */
#ifdef NON_MATCHING
s32 func_00197330(s32 flags, s32 ctx) {
    u64 a;
    u64 *p = state_packet(ctx);
    s32 f = flags & 0xF0;

#ifdef PLATFORM_PC
    a = 0;
#endif
    switch (flags & 0xC00) {
    case 0:
        switch (flags & 0xF) {
        case 0:
            switch (f) {
            case 0x50: a = 0x56; break;
            case 0x40: a = 0x99; break;
            case 0x30: a = 0x46; break;
            case 0x20: a = 0x89; break;
            case 0x10: a = 0x6A | ((u64)0x80 << 32); break;
            case 0x00: a = 0xAA | ((u64)0x80 << 32); break;
            default: return 0;
            }
            break;
        case 1:
            switch (f) {
            case 0x50: a = 0x16; break;
            case 0x40: a = 0x19; break;
            case 0x30: a = 0x06; break;
            case 0x20: a = 0x09; break;
            case 0x10: a = 0x68 | ((u64)0x80 << 32); break;
            case 0x00: a = 0xA8 | ((u64)0x80 << 32); break;
            default: return 0;
            }
            break;
        case 2:
            switch (f) {
            case 0x30: a = 0x44; break;
            case 0x00: a = 0x88; break;
            case 0x10: a = 0x48; break;
            default: return 0;
            }
            break;
        case 3:
            switch (f) {
            case 0x20: a = 0x01; break;
            case 0x00: a = 0x02; break;
            default: return 0;
            }
            break;
        case 4:
            switch (f) {
            case 0x50: a = 0x54; break;
            case 0x00: a = 0x98; break;
            case 0x10: a = 0x58; break;
            default: return 0;
            }
            break;
        case 5:
            switch (f) {
            case 0x40: a = 0x11; break;
            case 0x00: a = 0x12; break;
            default: return 0;
            }
            break;
        default:
            return 0;
        }
        break;
    case 0x400:
        switch (flags & 0xF) {
        case 0:
            switch (f) {
            case 0x40: a = 0x96; break;
            case 0x20: a = 0x86; break;
            case 0x10: a = 0xA6 | ((u64)0x80 << 32); break;
            case 0x00: a = 0xAA | ((u64)0x80 << 32); break;
            default: return 0;
            }
            break;
        case 1:
            switch (f) {
            case 0x10: a = 0xA4 | ((u64)0x80 << 32); break;
            case 0x00: a = 0x2A | ((u64)0x80 << 32); break;
            default: return 0;
            }
            break;
        case 2:
            switch (f) {
            case 0x20: a = 0x84; break;
            case 0x00: a = 0x88; break;
            default: return 0;
            }
            break;
        case 3:
            if (f != 0) {
                return 0;
            }
            a = 0x02;
            break;
        case 4:
            switch (f) {
            case 0x40: a = 0x94; break;
            case 0x00: a = 0x98; break;
            default: return 0;
            }
            break;
        case 5:
            if (f != 0) {
                return 0;
            }
            a = 0x56;
            break;
        default:
            return 0;
        }
        break;
    case 0x800:
        switch (flags & 0xF) {
        case 0:
            switch (f) {
            case 0x50: a = 0x56; break;
            case 0x40: a = 0x99; break;
            case 0x30: a = 0x46; break;
            case 0x20: a = 0x89; break;
            case 0x10: a = 0xA9 | ((u64)0x80 << 32); break;
            case 0x00: a = 0xAA | ((u64)0x80 << 32); break;
            default: return 0;
            }
            break;
        case 1:
            switch (f) {
            case 0x10: a = 0xA1 | ((u64)0x80 << 32); break;
            case 0x00: a = 0xA2 | ((u64)0x80 << 32); break;
            default: return 0;
            }
            break;
        case 2:
            switch (f) {
            case 0x20: a = 0x81; break;
            case 0x00: a = 0x82; break;
            default: return 0;
            }
            break;
        case 4:
            switch (f) {
            case 0x40: a = 0x91; break;
            case 0x00: a = 0x92; break;
            default: return 0;
            }
            break;
        default:
            return 0;
        }
        break;
    }
    if (!(flags & 0x01000000)) {
        a = 0xA8 | ((u64)0x80 << 32);   /* blending off: Cs */
    }
    state_regs(p, ctx, a, 0x42, 0x43);
    func_0018CA60(0, PHYS(p), (u32 *)p, &D_00281AF0);
    return 1;
}
#endif

/* Not matching yet: first draft. */
#ifdef NON_MATCHING
/* FOGCOL from 0x00BBGGRR... colour as the engine keeps it (0x00RRGGBB). */
s32 func_00197A50(u32 color) {
    u64 *p = (u64 *)func_0018D850(0x30, 0x10);

    p[0] = (u32)(0xF000 << 16) | 2;
    ((u32 *)p)[2] = 0x13000000;
    ((u32 *)p)[3] = 0x51000002;
    p[2] = 1 | 0x8000 | ((u64)1 << 60);
    p[3] = 0xE;
    p[4] = ((color & 0xFF) << 16) | ((color >> 16) & 0xFF) | (((color >> 8) & 0xFF) << 8);
    p[5] = 0x3D;
    func_0018CA60(0, PHYS(p), (u32 *)p, &D_00281AF0);
    return 1;
}
#endif

/* TEX1: bilinear filtering unless flags bit 16. */
s32 func_00197B10(s32 flags, s32 ctx) {
    u64 *p = (u64 *)func_0018D850(0x30, 0x10);

    p[0] = (u32)(0xF000 << 16) | 2;
    ((u32 *)p)[2] = 0x13000000;
    ((u32 *)p)[3] = 0x50000002;
    p[2] = 1 | 0x8000 | ((u64)1 << 60);
    p[3] = 0xE;
    if ((flags & 0x10000) == 0x10000) {
        p[4] = 0;
    } else {
        p[4] = 0x60;
    }
    if (ctx == 0) {
        p[5] = 0x14;
    } else {
        p[5] = 0x15;
    }
    func_0018CA60(0, PHYS(p), (u32 *)p, &D_00281AF0);
    return 1;
}

/* The texture-state packet in .data: a header and six A+D writes (TEXA, TEX1, TEX0, CLAMP,
   MIPTBP1, MIPTBP2) that func_00197BE0 fills in for one texture and sends. */
extern u32 D_002126E0[];
extern u64 D_00212700[];
extern u64 D_00212710[];
extern u64 D_00212720[];
extern u64 D_00212730[];
extern u64 D_00212740[];
extern u64 D_00212750[];
extern s32 D_0023BA58;      /* mipmap K (cmd 6) */
extern u32 D_0023BA5C;      /* mipmap L (cmd 7) */
extern s32 D_0023BA8C;
s32 func_001937B0(u32 *pkt);
s16 func_0019C6A0(FlTexture *t, u32 mip);
s32 func_00197C50(u32 h, u64 *texa, u64 *tex1, u64 *tex0, u64 *clamp, u64 *mip1, u64 *mip2, u32 flags);

/* Use texture handle h for drawing (0 if it isn't in GS memory). */
s32 func_00197BE0(u32 h) {
    if (func_00197C50(h, D_00212700, D_00212710, D_00212720, D_00212730, D_00212740, D_00212750, D_0023BA8C) == 0) {
        return 0;
    }
    func_001937B0(D_002126E0);
    return 1;
}

/* The GS registers for texture handle h: flags bit 16 point sampling, bits 17-18 clamp to the
   texture's size, bits 22-23 mipmapping (1 nearest level, 2/3 blend levels). */
#ifdef NON_MATCHING   /* first draft: the TEX0 and MIPTBP expressions' order */
s32 func_00197C50(u32 h, u64 *texa, u64 *tex1, u64 *tex0, u64 *clamp, u64 *mip1, u64 *mip2, u32 flags) {
    FlTexture *t = &D_0027AAC0[(h & 0xFFFF) - 1];
    FlTexture *c;
    s32 tbp[6];
    s32 tbw[6];
    s32 w;
    s32 i;
    s16 bw;

    if ((h & 0xFFFF) == 0) {
        return 0;
    }
    if (t->vram == 0) {
        return 0;
    }
    if (t->inVram == 0) {
        return 0;
    }
    if (t->flags & 4) {
        if ((h & 0xFFFF0000) >> 16 == 0) {
            return 0;
        }
        c = &D_002772C0[((h & 0xFFFF0000) >> 16) - 1];
        if (c->vram == 0) {
            return 0;
        }
        if (c->inVram == 0) {
            return 0;
        }
    }

    if (!(flags & 0xC00000) || t->mips == 1) {
        if ((flags & 0x10000) == 0x10000) {
            *tex1 = 0;
        } else {
            *tex1 = 0x60;
        }
    } else if ((flags & 0xC00000) == 0x400000) {
        if ((flags & 0x10000) == 0x10000) {
            *tex1 = ((u64)(t->mips - 1) << 2 | 0x80) | (u64)D_0023BA5C << 19 | (u64)(u32)((D_0023BA58 << 4) & 0xFFF) << 32;
        } else {
            *tex1 = ((u64)(t->mips - 1) << 2 | 0x120) | (u64)D_0023BA5C << 19 | (u64)(u32)((D_0023BA58 << 4) & 0xFFF) << 32;
        }
    } else {
        if ((flags & 0x10000) == 0x10000) {
            *tex1 = ((u64)(t->mips - 1) << 2 | 0xC0) | (u64)D_0023BA5C << 19 | (u64)(u32)((D_0023BA58 << 4) & 0xFFF) << 32;
        } else {
            *tex1 = ((u64)(t->mips - 1) << 2 | 0x160) | (u64)D_0023BA5C << 19 | (u64)(u32)((D_0023BA58 << 4) & 0xFFF) << 32;
        }
    }

    bw = t->w / 64;
    if (bw == 0) {
        bw = 1;
    }
    if (t->flags & 4) {
        c = &D_002772C0[((h & 0xFFFF0000) >> 16) - 1];
        if (c->bpp == 3) {
            *texa = 0x80 | (u64)0x80 << 32;
        } else {
            *texa = (u64)0x80 << 32;
        }
        *tex0 = (u64)t->vram | (u64)bw << 14 | (u64)(u32)t->psm << 20 | (u64)t->tw << 26 | (u64)t->th << 30
              | (u64)4 << 32 | (u64)c->vram << 37 | (u64)(u32)c->psm << 51 | (u64)0x40000000 << 32;
    } else {
        if (t->bpp == 3) {
            *texa = 0x80 | (u64)0x80 << 32;
        } else {
            *texa = (u64)0x80 << 32;
        }
        *tex0 = (u64)t->vram | (u64)bw << 14 | (u64)(u32)t->psm << 20 | (u64)t->tw << 26 | (u64)t->th << 30
              | (u64)4 << 32;
    }

    switch (flags & 0x60000) {
    case 0:
        *clamp = 0;
        break;
    default:
        *clamp = ((u64)t->w << 14 | 0xA) | (u64)t->h << 34;
        break;
    }

    *mip1 = 0;
    *mip2 = 0;
    if (t->mips < 2) {
        return 1;
    }
    for (i = 5; i >= 0; i--) {
        tbp[i] = 0;
        tbw[i] = 0;
    }
    w = t->w >> 1;
    for (i = 0; i < t->mips - 1; i++) {
        tbp[i] = func_0019C6A0(t, i + 1);
        tbw[i] = w / 64;
        if (tbw[i] == 0) {
            tbw[i] = 1;
        }
        w >>= 1;
    }
    *mip1 = (u64)tbp[0] | (u64)tbw[0] << 14 | (u64)tbp[1] << 20 | (u64)tbw[1] << 34 | (u64)tbp[2] << 40 | (u64)tbw[2] << 54;
    *mip2 = (u64)tbp[3] | (u64)tbw[3] << 14 | (u64)tbp[4] << 20 | (u64)tbw[4] << 34 | (u64)tbp[5] << 40 | (u64)tbw[5] << 54;
    return 1;
}
#endif

extern s32 D_0023BA80;      /* fog colour */
extern s32 D_0023BA8C;      /* default drawing flags */
extern f32 D_0023BA1C;      /* frame height / next power of two */
extern f32 D_0023BA20;      /* frame width / next power of two */
extern s32 D_0023BA24;
extern f32 D_00277260[4][4];
void func_001A0280(void);
void func_001A0290(void);

/* Not matching yet: register allocation in the power-of-two loops. */
#ifdef NON_MATCHING
void func_001957E0(void) {
    s32 i;
    s32 n;

    func_001A0280();
    func_00197330(D_0023BA8C, 2);
    func_00197080(D_0023BA8C, 2);
    func_00196F00(D_0023BA8C, 2);
    func_00196C20(0, 0, D_0023BAA0, D_0023BA9C, 2);
    func_00197A50(D_0023BA80);
    func_00197B10(D_0023BA8C, 0);
    D_0023BA24 = 0;
    func_001A0290();
    for (i = 0; (n = 1 << i) < D_0023BAA0; i++) {
    }
    D_0023BA20 = (f32)D_0023BAA0 / (f32)n;
    for (i = 0; (n = 1 << i) < D_0023BA9C; i++) {
    }
    D_00277260[0][1] = 0.0f;
    D_00277260[0][2] = 0.0f;
    D_00277260[0][3] = 0.0f;
    D_00277260[1][0] = 0.0f;
    D_00277260[0][0] = 1.0f;
    D_00277260[1][1] = 1.0f;
    D_00277260[1][2] = 0.0f;
    D_00277260[1][3] = 0.0f;
    D_00277260[2][0] = 0.0f;
    D_00277260[2][1] = 0.0f;
    D_00277260[2][2] = 1.0f;
    D_00277260[3][3] = 1.0f;
    D_00277260[2][3] = 0.0f;
    D_00277260[3][0] = 0.0f;
    D_00277260[3][1] = 0.0f;
    D_00277260[3][2] = 0.0f;
    D_0023BA1C = (f32)D_0023BA9C / (f32)n;
}
#endif
