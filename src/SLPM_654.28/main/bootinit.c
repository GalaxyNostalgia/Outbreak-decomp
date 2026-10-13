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
TexSlot *func_001CFE20(s32 id, u32 addr);
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

/* The loaded texture with this id, with one more reference; NULL when it isn't loaded. */
static TexSlot *func_001CFDC0(s32 id) {
    TexSlot *t = D_00321590;
    s32 i;

    for (i = 0; i < 0x100; i++, t++) {
        if (t->tex != 0 && t->id == id) {
            t->refs++;
            return t;
        }
    }
    return NULL;
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

/* A movie texture (w x h, from SUBMAIN's func_0034A380) as texture id. */
s32 func_0034A380(s32 a);

TexSlot *func_001CFF80(s32 id, s16 w, s16 h) {
    TexSlot *t = func_001CFD10();

    if (t == NULL) {
        return NULL;
    }
    t->tex = func_0034A380(0);
    if (t->tex != 0) {
        t->id = id;
        t->w = w;
        t->h = h;
        t->bpp = 4;
        t->refs = 1;
        return t;
    }
    func_001CFD60(t);
    return NULL;
}

/* Loads a texture file as texture id (-1: the current base id) and returns its handle; when
   it's loaded already it only gets another reference. mode (low 16 bits): 0 a TIM2 file, 1 the
   same LZ packed (with 0x10000 in mode), 2 a TIM2 already in memory (name is its address), 3
   and 4 a file whose id is its word 2 (these return the id). */
extern u8 *D_0023BC7C;      /* the load buffer */

#ifdef NON_MATCHING   /* the last if's loads are scheduled differently */
s32 func_001D0020(const char *name, s32 id, s32 mode) {
    TexSlot *t;
    s32 *hdr;

    if (mode < 3) {
        if (id == -1) {
            id = D_0023A0C4;
            if (id == -1) {
                return 0;
            }
        }
        if ((t = func_001CFDC0(id)) != NULL) {
            return t->tex;
        }
    }
    switch (mode & 0xFFFF) {
    case 0:
        if (func_001CFBB0(name, D_0023BC7C, mode, 0) > 0) {
            t = func_001CFE20(id, (u32)D_0023BC7C);
        }
        break;
    case 1:
        if (mode & 0x10000) {
            if (func_001CFBB0(name, D_0023BC7C + 0x100000, mode, 0) <= 0) {
                break;
            }
            func_001AE810((u16 *)(D_0023BC7C + 0x100000), (u16 *)D_0023BC7C);
        } else if (func_001CFBB0(name, D_0023BC7C, mode, 0) <= 0) {
            break;
        }
        t = func_001CFE20(id, (u32)D_0023BC7C);
        break;
    case 2:
        t = func_001CFE20(id, (u32)name);
        break;
    case 3:
        t = NULL;
        if (func_001CFBB0(name, D_0023BC7C, mode, 0) > 0) {
            hdr = (s32 *)D_0023BC7C + 2;
            if ((t = func_001CFDC0(*hdr)) == NULL) {
                t = func_001CFE20(*hdr, (u32)D_0023BC7C);
            }
        }
        break;
    case 4:
        if (func_001CFBB0(name, D_0023BC7C, mode, 0) > 0) {
            hdr = (s32 *)D_0023BC7C + 2;
            if ((t = func_001CFDC0(*hdr)) == NULL) {
                t = func_001CFE20(*hdr, (u32)D_0023BC7C);
            }
        }
        break;
    }
    if (t == NULL) {
        return 0;
    }
    if (mode >= 3) {
        return t->id;
    }
    return t->tex;
}
#endif

/* Texture file names, by texture id range (see func_001D0960); kind 0 is a texture (DATA\ROM\TEX\
   ... .TEX), 1 a model (DATA\ROM\MODEL\...). */
extern char *D_0023A0C8[2];
extern char D_00320A50[];   /* the name */
extern char D_00237F50[];   /* "%s%s%02d" */
extern char D_00237F60[];   /* "%s%s%03d" */
extern char D_00237F70[];   /* ".TEX" */
extern char D_00237F78[];   /* "%s%s%02d_%02d" */
extern char D_00237F88[];   /* "%s%s%03d_%02d" */
extern char D_00237FA0[];   /* "%sST\R%03d\R%03d%02d%02d" */
extern char D_00237FC0[];   /* "%sST\R%03d\E%03d%02d%02d" */
extern char D_00237FE0[];   /* "%sEFFECT\ef%02d" */
extern char D_00237FF0[];   /* "%sEFFECT\def%02d" */

char *func_001D05C0(s32 kind, s32 n, const char *prefix) {
    if (n < 100) {
        sprintf(D_00320A50, D_00237F50, D_0023A0C8[kind], prefix, n);
    } else {
        sprintf(D_00320A50, D_00237F60, D_0023A0C8[kind], prefix, n);
    }
    if (kind == 0) {
        strcat(D_00320A50, D_00237F70);
    }
    return D_00320A50;
}

char *func_001D0660(s32 kind, s32 n, s32 sub, const char *prefix) {
    if (n & 0x8000) {
        if ((n & ~0x8000) < 100) {
            sprintf(D_00320A50, D_00237F50, D_0023A0C8[kind], prefix, n & 0x7FFF);
        } else {
            sprintf(D_00320A50, D_00237F60, D_0023A0C8[kind], prefix, n & 0x7FFF);
        }
    } else if (n < 100) {
        sprintf(D_00320A50, D_00237F78, D_0023A0C8[kind], prefix, n, sub);
    } else {
        sprintf(D_00320A50, D_00237F88, D_0023A0C8[kind], prefix, n, sub);
    }
    if (kind == 0) {
        strcat(D_00320A50, D_00237F70);
    }
    return D_00320A50;
}

/* The room's own textures. */
char *func_001D0780(s32 kind) {
    sprintf(D_00320A50, D_00237FA0, D_0023A0C8[kind], D_00306590.unk1A, D_00306590.unk1A,
            D_00306590.unk1C, D_00306590.unk1E);
    if (kind == 0) {
        strcat(D_00320A50, D_00237F70);
    }
    return D_00320A50;
}

char *func_001D0800(s32 room, u32 n, s32 kind) {
    sprintf(D_00320A50, D_00237FC0, D_0023A0C8[kind], room, room, (n >> 8) & 0xFF, n & 0xFF);
    if (kind == 0) {
        strcat(D_00320A50, D_00237F70);
    }
    return D_00320A50;
}

char *func_001D0880(s32 n, s32 kind) {
    sprintf(D_00320A50, D_00237FE0, D_0023A0C8[kind], n);
    if (kind == 0) {
        strcat(D_00320A50, D_00237F70);
    }
    return D_00320A50;
}

char *func_001D08F0(s32 n, s32 kind) {
    sprintf(D_00320A50, D_00237FF0, D_0023A0C8[kind], n);
    if (kind == 0) {
        strcat(D_00320A50, D_00237F70);
    }
    return D_00320A50;
}

extern char D_00238008[];   /* "NPC\LOW\N" */
extern char D_00238018[];   /* "NPC\N" */
extern char D_00238020[];   /* "PL\LOW\P" */
extern char D_00238030[];   /* "PL\P" */
extern char D_00238038[];   /* "PL\EP" */
extern char D_00238040[];   /* "PL\HIGH\HP" */
extern char D_00238050[];   /* "PL\HP" */
extern char D_00238058[];   /* "NPC\HIGH\HN" */
extern char D_00238068[];   /* "NPC\HN" */
extern char *D_0023A080[1];
extern char *D_0023A084[1];
extern char *D_0023A088[1];
extern char *D_0023A08C[1];
extern char *D_0023A090[1];
extern char *D_0023A094[1];
extern char *D_0023A098[1];
extern char *D_0023A09C[1];
extern char *D_0023A0A0[1];
extern char *D_0023A0A4[1];
extern char *D_0023A0A8[1];
extern char *D_0023A0AC[1];
extern char *D_0023A0B0[1];
extern char *D_0023A0B4[1];
extern char *D_0023A0B8[1];
extern char *D_0023A0BC[1];
extern char *D_0023A0C0[1];
extern char *D_002187F0[];
extern char *D_00218A30[];
extern char *D_00218AB0[];
extern char *D_00218D20[];
extern char D_00320B50[][0x40];
extern char D_00320F50[][0x40];

/* The file name for texture id (kind 0) or model (kind 1); arg is the id's argument (low 16
   bits a number, high 16 bits a sub-number). */
#ifdef NON_MATCHING   /* first draft: the range tests are laid out differently */
char *func_001D0960(s32 id, u32 arg, s32 kind, s32 alt) {
    u32 sub = (arg & 0xFFFF0000) >> 16;

    if (id >= 0x100 && id < 0xFF0) {
        if (kind == 0) {
            return D_0023A09C[arg & 0xFFFF];
        }
        return D_0023A0C0[arg & 0xFFFF];
    }
    if (id >= 0x1000 && id < 0x1100) {
        if (arg & 0x8000) {
            if (kind == 0) {
                return func_001D0660(kind, arg & 0xFFFF, sub, D_00238008);
            }
            return func_001D0660(kind, arg & 0xFFFF, sub, D_00238018);
        }
        if (kind == 0) {
            return func_001D0660(kind, arg & 0xFFFF, sub, D_00238020);
        }
        if (alt == 0) {
            return func_001D0660(kind, arg & 0xFFFF, sub, D_00238030);
        }
        return func_001D0660(kind, arg & 0xFFFF, sub, D_00238038);
    }
    if (id >= 0x1100 && id < 0x1200) {
        if (kind == 0) {
            return func_001D05C0(kind, arg & 0xFFFF, D_00238040);
        }
        return func_001D05C0(kind, arg & 0xFFFF, D_00238050);
    }
    if (id >= 0x1200 && id < 0x1300) {
        if (kind == 0) {
            return D_002187F0[arg & 0xFFFF];
        }
        return D_00218AB0[arg & 0xFFFF];
    }
    if (id >= 0x1300 && id < 0x1400) {
        if (kind == 0) {
            return D_00218A30[arg & 0xFFFF];
        }
        return D_00218D20[arg & 0xFFFF];
    }
    if (id >= 0x1400 && id < 0x1600) {
        if (kind == 0) {
            return func_001D05C0(kind, arg & 0xFFFF, D_00238008);
        }
        return func_001D05C0(kind, arg & 0xFFFF, D_00238018);
    }
    if (id >= 0x1600 && id < 0x1800) {
        if (kind == 0) {
            return func_001D05C0(kind, arg & 0xFFFF, D_00238058);
        }
        return func_001D05C0(kind, arg & 0xFFFF, D_00238068);
    }
    if (id >= 0x1800 && id < 0x2000) {
        return func_001D0780(kind);
    }
    if (id >= 0x2100 && id < 0x2400) {
        return func_001D0800(sub, arg & 0xFFFF, kind);
    }
    if (id >= 0x2000 && id < 0x2100) {
        return func_001D0880(arg & 0xFFFF, kind);
    }
    if (id >= 0x2400 && id < 0x2500) {
        return func_001D08F0(arg & 0xFFFF, kind);
    }
    if (id >= 0x2A00 && id < 0x3000) {
        if (kind == 0) {
            switch (sub) {
            case 0:
            case 1:
                return D_0023A080[arg & 0xFFFF];
            case 2:
                return D_0023A084[arg & 0xFFFF];
            case 24:
                return D_0023A088[arg & 0xFFFF];
            case 26:
                return D_0023A08C[arg & 0xFFFF];
            case 28:
                return D_0023A090[arg & 0xFFFF];
            case 35:
                return D_0023A094[arg & 0xFFFF];
            case 41:
                return D_0023A098[arg & 0xFFFF];
            default:
                return D_0023A088[arg & 0xFFFF];
            }
        }
        switch (sub) {
        case 0:
        case 1:
            return D_0023A0A4[arg & 0xFFFF];
        case 2:
            return D_0023A0A8[arg & 0xFFFF];
        case 24:
            return D_0023A0AC[arg & 0xFFFF];
        case 26:
            return D_0023A0B0[arg & 0xFFFF];
        case 28:
            return D_0023A0B4[arg & 0xFFFF];
        case 35:
            return D_0023A0B8[arg & 0xFFFF];
        case 41:
            return D_0023A0BC[arg & 0xFFFF];
        default:
            return D_0023A0AC[arg & 0xFFFF];
        }
    }
    if (id >= 0x3000 && id < 0x3100) {
        if (kind == 0) {
            return D_00320F50[arg & 0xFFFF];
        }
        return D_00320B50[arg & 0xFFFF];
    }
    if (id >= 0x2500 && id < 0x2510) {
        return D_0023A0A0[arg & 0xFFFF];
    }
    return NULL;
}
#endif

/* Movie textures: 0x40 for MOVIE64.TM2, 0x80 for MOVIE128.TM2, else -1. */
extern char D_00238070[];   /* "MOVIE64.TM2" */
extern char D_00238080[];   /* "MOVIE128.TM2" */

s32 func_001D0FA0(const char *path) {
    char name[0x100];

    strcpy(name, strrchr(path, '\\') + 1);
    strupr(name);
    if (strcmp(name, D_00238070) == 0) {
        return 0x40;
    }
    if (strcmp(name, D_00238080) == 0) {
        return 0x80;
    }
    return -1;
}

/* Loads the textures a list file names (one per line, from func_001D0960(id, arg)) as ids
   id, id + 1, ...; movie textures become placeholders. */
extern char D_00238090[];   /* ".tm2" */

#ifdef NON_MATCHING   /* first draft */
void func_001D1010(u32 arg, s32 id) {
    char dir[0x100];
    char path[0x100];
    char line[0x100];
    char *list;
    char *p;
    char *s;
    char *q;
    s32 size;
    s32 pos;
    s32 movie;

    if (id == -1) {
        id = D_0023A0C4;
        if (id == -1) {
            return;
        }
    }
    list = func_001D0960(id, arg, 0, 0);
    if (list == NULL || strlen(list) == 0) {
        return;
    }
    strcpy(dir, list);
    strrchr(dir, '\\')[1] = 0;
    s = (char *)D_0023BC7C;
    size = func_001CFBB0(list, s, 1, 0);
    pos = 0;
    if (size == 0) {
        return;
    }
    for (;;) {
        strcpy(path, dir);
        p = line;
        for (;;) {
            if (*s == '\n' || *s == 0) {
                break;
            }
            pos++;
            *p++ = *s++;
            if (pos >= size) {
                return;
            }
        }
        *p = 0;
        s++;
        pos++;
        q = strrchr(line, '.');
        if (q != NULL || (q = strrchr(line, ',')) != NULL) {
            *q = 0;
        }
        if (strlen(line) == 0) {
            return;
        }
        q = strrchr(line, '/');
        if (q != NULL) {
            strcat(path, q + 1);
        } else {
            strcat(path, line);
        }
        strcat(path, D_00238090);
        movie = func_001D0FA0(path);
        if (movie < 0) {
            func_001D0020(path, id, 1);
        } else {
            func_001CFF80(0xFF0, movie, movie);
        }
        id++;
    }
}
#endif

/* The room's textures (only with loose files; the release has them in its models). */
void func_001D5640(void) {
    if (D_00306590.unk17 == 0) {
        func_001D1010(D_00306590.unk1A, 0x2000);
    }
}

void func_001D56A0(void) {
    func_001D1010(0, 0x2500);
}

/* Registers the TIM2 picture at tim as texture id (-1: the current base), or adds a reference
   when it is loaded. */
s32 func_001D0250(u32 tim, s32 id) {
    TexSlot *t;

    if (id == -1) {
        id = D_0023A0C4;
        if (id == -1) {
            return 0;
        }
    }
    if ((t = func_001CFDC0(id)) != NULL) {
        return t->tex;
    }
    t = func_001CFE20(id, tim);
    return t->tex;
}
