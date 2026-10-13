#include "common.h"
#include "sce.h"
#include "options.h"
#include "task.h"
#include "tim2.h"

/* Boot, after the save-data check: reset the scene state, start the system tasks and load the
   textures the title needs. */

void func_001AE9B0(void);
void func_001AE9C0(s32 cmd, u32 arg);
void func_001B1360(void);
void func_001B13D0(void);
void func_001B2C90(void);
void func_001B65D0(void (*fn)(Task *), s16 slot);
void func_001D6CC0(void);
void func_001B9A60(Task *t);
void func_001B9ED0(Task *t);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 byNumber);
typedef struct TexSlot TexSlot;


extern u8 D_003065D0[];     /* (Options) */
extern u8 D_003065D1[];
extern u8 D_003065D2[];
extern u8 D_003065D3[];

/* Default render state: fog from 500 to 10000 etc. With keep set only the last few. */
void func_001AE310(s32 keep) {
    f32 fogNear;
    f32 fogFar;
    s32 n = 10000;

    fogNear = 500.0f;
    fogFar = n;
    func_001AE9B0();
    if (keep == 0) {
        func_001AE9C0(0xE, 0);
        func_001AE9C0(1, 0x200);
        func_001AE9C0(0x15, 2);
        func_001AE9C0(0x12, 0);
        func_001AE9C0(0xF, 0x80FFFFFF);
        func_001AE9C0(0x10, *(u32 *)&fogNear);
        func_001AE9C0(0x11, *(u32 *)&fogFar);
        func_001AE9C0(0xC, 0);
        func_001AE9C0(0x66, 0);
        func_001AE9C0(0x62, 0);
        func_001AE9C0(2, 0);
        func_001AE9C0(0, 0);
    }
    D_003065D2[0] = 0;
    D_003065D0[0] = 4;
    D_003065D1[0] = 5;
    D_003065D3[0] = 0xFF;
    func_001AE9C0(0x5E, 0x32);
    func_001AE9C0(0x63, 0x10000);
}

/* Background colour presets (r, g, b). */
extern u8 D_00216850[][3];
extern u8 D_0030627C[];

void func_001AE8E0(s32 i) {
    u8 *c = D_00216850[i];

    D_0030627C[0] = c[0];
    D_0030627C[1] = c[1];
    D_0030627C[2] = c[2];
    func_001AE9C0(0x14, (D_0030627C[0] << 16) | 0xFF000000 | (D_0030627C[1] << 8) | D_0030627C[2]);
}

/* Decompresses 16-bit LZ data: a flag word per 16 items, set = copy (offset 11 bits, length
   5 bits or the next word; offset 0 = run of zeros), clear = literal. */
void func_001AE810(u16 *src, u16 *dst) {
    s32 flags = 0;
    s32 bit = 0;
    u32 n;
    u32 off;
    u16 *from;

    for (;;) {
        if (bit == 0) {
            flags = *(s16 *)src++;
            bit = 0x8000;
        }
        if (flags & bit) {
            off = *src++;
            n = off >> 11;
            if (n != 0) {
                off &= 0x7FF;
            } else {
                n = *src++;
            }
            if (off == 0) {
                if (n == 0) {
                    return;
                }
                do {
                    *dst++ = 0;
                    n--;
                } while (n != 0);
            } else {
                from = dst - off;
                do {
                    n--;
                    *dst++ = *from++;
                } while (n != 0);
            }
        } else {
            *dst++ = *src++;
        }
        bit >>= 1;
    }
}

/* Loaded textures: 256 slots of {texture, id, ...}. */
struct TexSlot {
    s32 tex;            /* texture handle */
    s32 id;
    s16 w, h;
    s16 bpp;
    s16 refs;
};

extern TexSlot D_00321590[0x100];
extern u8 D_0023BE2C;

/* Texture ids 0x2100-0x23FF are per player (0x30 each) and map to 0x1800 + 0x80 per player. */
s32 func_001D02B0(s32 id) {
    if (id >= 0x2100 && id < 0x2400) {
        id -= D_0023BE2C * 0x30 + 0x2100;
        id += (D_0023BE2C << 7) + 0x1800;
    }
    return id;
}

/* The loaded texture with this id, or NULL. */
s32 func_001D04D0(s32 id) {
    TexSlot *t;
    s32 i;

    id = func_001D02B0(id);
    t = D_00321590;
    for (i = 0; i < 0x100; i++, t++) {
        if (t->tex != 0 && t->id == id) {
            return t->tex;
        }
    }
    return 0;
}

/* A free slot. */
static TexSlot *func_001CFD10(void) {
    TexSlot *t = D_00321590;
    s32 i;

    for (i = 0; i < 0x100; i++, t++) {
        if (t->tex == 0) {
            return t;
        }
    }
    return NULL;
}

void func_0019CAA0(s32 tex);
void func_0019CC00(s32 pal);

/* Frees a texture handle's texture and palette. */
void func_001CFCC0(s32 handle) {
    s32 tex = handle & 0xFFFF;
    u32 pal;

    if (tex != 0) {
        func_0019CAA0(tex);
    }
    pal = (u32)(handle & 0xFFFF0000) >> 16;
    if (pal != 0) {
        func_0019CC00(pal);
    }
}

/* Drops a reference; frees the texture with the last. */
void func_001CFD60(TexSlot *t) {
    if (t->tex != 0) {
        t->refs--;
        if (t->refs <= 0) {
            func_001CFCC0(t->tex);
            memset(t, 0, sizeof(TexSlot));
        }
    }
}

s32 func_0018D8C0(void *tim, u32 owner);
s32 func_0019CF50(PixFormat *fmt, s32 handle);

#ifdef NON_MATCHING   /* the original stores the id after the call */
/* Loads the TIM2 picture at addr as texture id. */
TexSlot *func_001CFE20(s32 id, u32 addr) {
    PixFormat fmt;
    s32 ok;
    TexSlot *t = func_001CFD10();

    if (t == NULL) {
        return NULL;
    }
    t->tex = func_0018D8C0((void *)addr, 4);
    if ((ok = t->tex) != 0) {
        t->id = id;
        if (func_0019CF50(&fmt, ok & 0xFFFF) == 1) {
            t->w = fmt.width;
            t->h = fmt.height;
            t->bpp = fmt.bpp;
            t->refs = 1;
        }
        return t;
    }
    func_001CFD60(t);
    return NULL;
}
#endif

extern u8 *D_003065DC[];    /* (Options) work memory */

/* Loads the textures shared by the demo screens (texture 0) unless they're loaded. */
void func_001D5170(void) {
    char name[0x100];
    u32 out;
    u32 buf;

    if (func_001D04D0(0) == 0) {
        buf = (u32)D_003065DC[0];
        out = buf + 0x100000;
        strcpy(name, "data\\rom\\scene\\pit_tex\\demo_com.tex");
        if (func_001CFBB0(name, D_003065DC[0], 1, 0) != 0) {
            func_001AE810((u16 *)buf, (u16 *)out);
            func_001CFE20(0, out + 0x1E2740);
        }
    }
}

/* System start after the save check. */
void func_001BB190(void) {
    func_001B1360();
    func_001B13D0();
    func_001AE8E0(0);
    func_001AE310(0);
    func_001B65D0(func_001B9ED0, 0xA);
    func_001B65D0(func_001B9A60, 0xB);
    D_00306590.unk0 = 0;
    func_001D6CC0();
    func_001D5170();
    func_001B2C90();
}

/* The demo screens' shared textures: three pictures in demo_com.tex. */
extern s32 D_00218E60[3];   /* offsets in the file */
extern s32 D_00218E70[3];   /* texture ids */

void func_001D5320(void) {
    char name[0x100];
    u32 out;
    u32 buf;
    s32 i;

    buf = (u32)D_003065DC[0];
    out = buf + 0x100000;
    strcpy(name, "data\\rom\\scene\\pit_tex\\demo_com.tex");
    if (func_001CFBB0(name, D_003065DC[0], 1, 0) != 0) {
        func_001AE810((u16 *)buf, (u16 *)out);
        for (i = 0; i < 3; i++) {
            func_001CFE20(D_00218E70[i], out + D_00218E60[i]);
        }
    }
}

extern s8 D_0023BCA8, D_0023BCAC, D_0023BCB0, D_0023BCB4;

void func_001D99A0(void) {
    D_0023BCB4 = 0;
    D_0023BCB0 = 0;
    D_0023BCAC = 0;
    D_0023BCA8 = 0;
}

/* The DNAS check (in NETDNAS.BIN), run a step per frame during the logos: 1 nothing to check,
   2 passed, -1 failed, 0 still running. */
typedef struct {
    u8 unk0[0x14];
    s32 size;           /* 0x14: bytes to check (0: none) */
} DnasData;

extern DnasData D_00334580;
extern u8 D_00324580[0x8000];
extern u8 D_0032C580[0x8000];
s32 func_743F50(s8 *a, s8 *b, u8 *data, s32 size, s32 bufSize);
void func_001D9790(s32 a);
void func_001D9810(s32 a);
void *memcpy(void *dst, const void *src, u32 n);

#ifdef NON_MATCHING   /* first draft */
s32 func_001D99C0(void) {
    s32 r;

    if (D_0023BCA8 != 0) {
        return D_0023BCA8;
    }
    switch (D_0023BCB4) {
    case 0:
        if (D_00334580.size == 0) {
            D_0023BCA8 = 1;
            break;
        }
        D_0023BCB4++;
        D_0023BCB0 = 0;
        D_0023BCAC = 0;
        memcpy(D_0032C580, D_00324580, 0x8000);
    case 1:
        D_0023BCB4++;
        break;
    case 2:
        D_0023BCB4++;
    case 3:
        r = func_743F50(&D_0023BCB0, &D_0023BCAC, D_0032C580, D_00334580.size, 0x8000);
        if (r > 0) {
            func_001D9810(0);
            D_0023BCB4 = 0;
            D_0023BCA8 = 2;
            D_0023BCB0 = 0;
            D_0023BCAC = 0;
        } else if (r < 0) {
            func_001D9790(0);
            D_0023BCB4 = 0;
            D_0023BCA8 = -1;
            D_0023BCB0 = 0;
            D_0023BCAC = 0;
        }
        break;
    }
    return D_0023BCA8;
}
#endif

#ifdef NON_MATCHING   /* the id is computed later in the original */
/* Releases textures first..first+n-1. */
void func_001D0300(s32 first, s32 n) {
    TexSlot *t;
    s32 i, j;
    s32 id;

    for (i = 0; i < n; i++) {
        id = first + i;
        for (j = 0, t = D_00321590; j < 0x100; j++, t++) {
            if (t->tex != 0 && t->id == id) {
                func_001CFD60(t);
                break;
            }
        }
    }
}
#endif

void func_0018D730(void);

/* The logos' textures. */
void func_001D53D0(void) {
    func_001D0300(1, 3);
    func_0018D730();
}

/* Boot task end. */
void func_001BB200(void) {
    func_001D53D0();
}

/* The title's textures (six pictures in demo_com.tex), their handles to tex[]. */
extern s32 D_00218E80[6];   /* offsets in the file */
extern s32 D_00218EA0[6];   /* texture ids */

void func_001D5400(s32 *tex) {
    char name[0x100];
    u32 out;
    u32 buf;
    s32 i;

    buf = (u32)D_003065DC[0];
    out = buf + 0x100000;
    strcpy(name, "data\\rom\\scene\\pit_tex\\demo_com.tex");
    if (func_001CFBB0(name, (void *)buf, 1, 0) != 0) {
        func_001AE810((u16 *)buf, (u16 *)out);
        for (i = 0; i < 6; i++) {
            func_001CFE20(D_00218EA0[i], out + D_00218E80[i]);
            *tex++ = func_001D04D0(D_00218EA0[i]);
        }
    }
}

void func_001D54D0(void) {
    func_001D0300(1, 6);
    func_0018D730();
}

/* Frees the n textures loaded from select.tex. */
void func_001D55D0(s32 n) {
    func_001D0300(0x4200, n);
    func_0018D730();
}

extern s32 D_0023A0C4;      /* texture id base for the next loads */

void func_001D0440(s32 base) {
    D_0023A0C4 = base;
}

extern char D_00238560[];   /* "data\rom\scene\pit_tex\select.tex" */

/* Loads select.tex (LZ packed) and registers n of its pictures, at the given offsets, as
   textures 0x4200 + i; their handles go to tex. */
void func_001D5500(s32 *offsets, s32 *tex, s32 n) {
    u32 out;
    u32 buf;
    s32 i;

    func_001D0440(0x4200);
    buf = (u32)D_003065DC[0];
    out = buf + 0x100000;
    if (func_001CFBB0(D_00238560, (void *)buf, 1, 0) != 0) {
        func_001AE810((u16 *)buf, (u16 *)out);
        for (i = 0; i < n; i++) {
            func_001CFE20(i + 0x4200, out + *offsets);
            *tex = func_001D04D0(i + 0x4200);
            offsets++;
            tex++;
        }
    }
}

extern u32 D_00306650[];    /* (Options) the event list */
extern char D_00238190[];   /* "data\rom\scene\item_msg\evelist.dat" */

/* Loads the event list to addr. */
s32 func_001D3500(u32 addr) {
    D_00306650[0] = addr;
    return func_001CFBB0(D_00238190, (void *)addr, 1, 0);
}
