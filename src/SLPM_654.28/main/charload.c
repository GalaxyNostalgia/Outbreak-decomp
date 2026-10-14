#include "common.h"
#include "options.h"
#include "object.h"
#include "model.h"

/* Loading a character's model and textures: textures 0x1000 + 8 * kind (0x1100 for the high
   detail ones, unk54D == 1), models in D_0047F4D0[kind], what they hold in D_003BB520[kind]. */

void func_001AE9C0(s32 cmd, u32 arg);
void func_001D0440(s32 base);
void func_001D1010(u32 arg, s32 id);
s32 func_001D1310(u32 arg, s32 id, Slot1468 *m, u32 flags);
u8 *func_001B1AD0(s32 count, s32 owner);

/* The render state models are loaded under. */
void func_001D3560(void) {
    func_001AE9C0(0x15, 2);
    func_001AE9C0(1, 0x800);
    func_001AE9C0(0x12, 0x8000);
    func_001AE9C0(0xC, 0);
    func_001AE9C0(0x66, 1);
    func_001AE9C0(0x62, 1);
    func_001AE9C0(0, 0);
    func_001AE9C0(2, 0);
}

/* The character's textures. */
void func_001D5030(DrawObjFull *obj, u8 kind, u8 costume) {
    u32 arg;

    if (obj->unkBB4 != 0) {
        arg = (obj->unkBB4 - 1) | 0x8000;
        if (obj->unk54D == 1) {
            func_001D1010(kind, obj->kind * 8 + 0x1100);
            return;
        }
        func_001D1010(arg, obj->kind * 8 + 0x1000);
        return;
    }
    if (obj->unk54D == 1) {
        func_001D1010(kind, obj->kind * 8 + 0x1100);
        return;
    }
    func_001D1010((costume << 16) | kind, obj->kind * 8 + 0x1000);
}

extern u8 D_003BB520[][0x824];      /* per character: what they hold (byte 0: loaded) */
extern Slot1468 D_0047F4D0[];
typedef struct {
    u8 unk0[0xC];
    s32 unkC;
    u8 unk10[4];
} Unk3559A0;
extern Unk3559A0 D_003559A0[];

/* The held item's model (only for characters whose kind has one). */
void func_001D3770(DrawObjFull *obj, u8 kind) {
    Slot1468 *m;

    D_003BB520[obj->kind][0] = 0;
    if (((s32 *)&D_003559A0[kind])[3] != 0) {
        func_001AE9C0(0x15, 2);
        func_001AE9C0(1, 0x800);
        func_001AE9C0(0x12, 0x8000);
        func_001AE9C0(0x62, 1);
        func_001AE9C0(0xC, 0);
        func_001AE9C0(0x66, 1);
        func_001AE9C0(2, 0);
        func_001AE9C0(0, 0);
        D_003BB520[obj->kind][0] = 1;
        m = (Slot1468 *)&D_003BB520[obj->kind][4];
        m->buf = func_001B1AD0(0x20, 1);
        m->unk818 = NULL;
    }
}

/* The character's model. */
void func_001D35E0(DrawObjFull *obj, u8 kind, u8 costume) {
    s32 n;
    Slot1468 *m;
    s32 high = obj->unk54D;
    u32 arg = kind;

    if (D_00306590.unk17 == 0) {
        if (obj->unkBB4 != 0) {
            n = 0;
            if (high == 1) {
                func_001D0440(obj->kind * 8 + 0x1100);
            } else {
                func_001D0440(obj->kind * 8 + 0x1000);
                arg = (obj->unkBB4 - 1) | 0x8000;
            }
        } else {
            n = 0;
            if (high == 1) {
                func_001D0440(obj->kind * 8 + 0x1100);
            } else {
                func_001D0440(obj->kind * 8 + 0x1000);
            }
        }
    }
    func_001D3560();
    if (high == 1) {
        n++;
    }
    n |= kind << 16;
    m = &D_0047F4D0[obj->kind];
    m->buf = func_001B1AD0(0x20, n);
    m->unk0 = (u32 *)m->unk820;
    m->unk4 = (u32 *)m->unkA20;
    m->unk818 = m->unkC20;
    if (D_00306590.unk17 == 0) {
        func_001D1310((costume << 16) | arg, -1, m, ((high & 7) << 29) | 2);
        if (high == 1) {
            func_001D3770(obj, kind);
        }
    }
}

extern s32 D_0023A0C4;
void func_001D5700(Slot1468 *m, void *model, s16 *tex);
void func_001B1D30(Slot1468 *m);
void func_001D5920(Slot1468 *m, void *model, u8 mode);
void func_001D11E0(s32 id, Slot1468 *m, void *motion);

/* Sets a model up from its data (and its motion, when there is one). */
s32 func_001D2720(Slot1468 *m, s32 mode, void *model, void *motion, s16 *tex) {
    func_001D5700(m, model, tex);
    func_001B1D30(m);
    func_001D5920(m, model, mode);
    if (motion == NULL) {
        return 1;
    }
    func_001D11E0(D_0023A0C4, m, motion);
    return 3;
}

/* Left empty in the release. */
void func_001D4E00(u8 kind) {
}

/* The room's effects model (ef%02d, textures 0x2000) and its effect table (efdata%02d.bin). */
typedef struct {
    u8 unk0[0x18];
    Slot1468 *model;    /* 0x18 */
    u8 *unk1C;          /* 0x1C: -> unk460 */
    s32 unk20;
    s32 unk24;
    u8 unk28[0x60 - 0x28];
    u8 data[0x400];     /* 0x60 */
    u8 unk460[4];
} EffectData;

extern EffectData D_00363950;
extern Slot1468 D_00389750;
extern Slot1468 D_0039A3D0;
extern char D_002382D0[];   /* "ef%02d" */
extern char D_002382E0[];   /* "data\\rom\\etc\\efdata%02d.bin" */
extern char D_00238300[];   /* "efdata%02d.bin" */
extern char D_00238218[];   /* "objS%02d" */
extern u8 *D_003065DC;
void func_001A9DA0(const char *name, Slot1468 *m, s32 id, u32 flags);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
s32 sprintf(char *dst, const char *fmt, ...);
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *dst, const void *src, u32 n);

void func_001D41F0(void);
void func_001D3D30(u8 room);
#ifdef NON_MATCHING   /* first draft */
void func_001D41F0(void) {
    char name[0x100];
    u8 *buf;
    s32 n;

    func_001D0440(0x2000);
    func_001AE9C0(0x15, 2);
    func_001AE9C0(1, 0);
    func_001AE9C0(0x12, 0);
    func_001AE9C0(0x62, 1);
    func_001AE9C0(0xC, 0);
    func_001AE9C0(0x66, 0);
    func_001AE9C0(2, 0);
    func_001AE9C0(0x12, 0);
    func_001AE9C0(0, 0);
    D_00389750.buf = func_001B1AD0(0x400, 4);
    D_00389750.unk818 = NULL;
    if (D_00306590.unk17 == 0) {
        func_001D1310(D_00306590.unk1A, -1, &D_00389750, 2);
    } else {
        memset(name, 0, 0x20);
        sprintf(name, D_002382D0, D_00306590.unk1A);
        func_001A9DA0(name, &D_00389750, 0x2000, 0x10002);
    }
    D_00363950.unk20 = 0;
    D_00363950.unk24 = 0;
    D_00363950.model = &D_00389750;
    buf = D_003065DC + 0x2FF000;
    memset(name, 0, 0x20);
    sprintf(name, D_002382E0, D_00306590.unk1A);
    if (D_00306590.unk17 != 0) {
        sprintf(name, D_00238300, D_00306590.unk1A);
    }
    n = func_001CFBB0(name, buf, 0x10001, 0);
    if (n != 0) {
        memcpy(D_00363950.data, buf, n);
        D_00363950.unk1C = D_00363950.unk460;
    }
}
#endif

/* The room's "ST" textures (loose files only). */
void func_001D5600(u16 room) {
    if (D_00306590.unk17 == 0) {
        func_001D1010(room << 16, 0x2A00);
    }
}

/* The room's object model (objS%02d, textures 0x2A00). */
void func_001D3D30(u8 room);
#ifdef NON_MATCHING   /* first draft */
void func_001D3D30(u8 room) {
    char name[0x100];
    Slot1468 *m = &D_0039A3D0;

    func_001D0440(0x2A00);
    func_001AE9C0(0x15, 2);
    func_001AE9C0(1, 0x800);
    func_001AE9C0(0x12, 0x8000);
    func_001AE9C0(0xC, 0);
    func_001AE9C0(0x66, 1);
    func_001AE9C0(0x62, 1);
    func_001AE9C0(0, 0x20);
    func_001AE9C0(2, 0);
    m->buf = func_001B1AD0(0x240, 8);
    m->unk818 = NULL;
    if (D_00306590.unk17 == 0) {
        func_001D1310(room << 16, -1, m, 3);
        return;
    }
    sprintf(name, D_00238218, room);
    func_001A9DA0(name, m, 0x2A00, 0x10003);
}
#endif


/* The room's geometry: loose files (textures first, buffered 0/1). */
extern u8 D_0023BE2C;           /* the room texture buffer in use (0/1) */
extern Slot1468 D_004700F0;     /* (Room + 0x60) */
extern u8 D_00470090[];         /* Room */
extern Slot1468 D_0038E774;
void func_001A97A0(u8 room, u8 a, u8 b);

void func_001D5110(u8 room, u8 a, u8 b) {
    u8 old;

    if (D_00306590.unk17 == 0) {
        old = D_0023BE2C;
        D_0023BE2C = old ^ 1;
        func_001D1010((room << 16) | (a << 8) | b, (D_0023BE2C << 7) + 0x1800);
    }
}

void func_001D3C30(u8 room, u8 a, u8 b);
#ifdef NON_MATCHING   /* first draft */
void func_001D3C30(u8 room, u8 a, u8 b) {
    s32 i;

    if (D_00306590.unk17 != 0) {
        func_001A97A0(D_00306590.unk1A, D_00306590.unk1C, D_00306590.unk1E);
        return;
    }
    func_001D0440((D_0023BE2C << 7) + 0x1800);
    D_004700F0.buf = func_001B1AD0(0x80, 6);
    D_004700F0.unk818 = NULL;
    func_001D1310((room << 16) | (a << 8) | b, -1, &D_004700F0, 2);
    for (i = 0; i < D_004700F0.nparts; i++) {
        D_00470090[0x8A0 + i] = 1;
    }
}
#endif

/* The room's effects (e%03d%02d%02d.bin): {n1, n1 * 8 bytes} {n2, n2 * 12} {n3, n3 * 0x30} and
   the rest for func_0055FB60; the object model (D_0038E774) draws them. */
typedef struct {
    u8 unk0[0x18];
    Slot1468 *model;    /* 0x18 */
    u8 unk1C[4];
    s32 n1;             /* 0x20 */
    s32 n3;             /* 0x24 */
    s32 n2;             /* 0x28 */
} RoomEffects;

extern RoomEffects D_0036397C;
extern Slot1468 *D_00363994;    /* (D_0036397C.model) */
extern u8 D_00363910[];
extern u8 D_003632F0[];
extern u8 D_00363310[];
extern u8 D_0037B9B0[];
extern char *D_0023A0F8;
extern char D_00238230[];   /* "%sr%03d\\e%03d%02d%02d.bin" */
extern char D_00238250[];   /* "e%03d%02d%02d.bin" */
extern char D_00238270[];   /* "ROOM EFFECT OVER!! (num %d)" */
extern char D_00238290[];   /* "ROOM EFFECT OVER!! (size %x)" */
extern char D_002382B0[];   /* "    ARE U OK?" */
extern char D_002382C0[];   /* "   PUSH START" */
extern s16 D_0023BB98;          /* debug checks */
extern u32 D_003064D0[];        /* (pad 1: buttons) */
s32 func_001A96E0(const char *name, void *buf);
void func_001AF820(void);
void func_001AE8E0(s32 a);
void func_001B26D0(s32 x, s32 y, s32 color, s32 a, const char *fmt, ...);
void func_001B9140(s32 a);
void func_0055FB60(void *data, RoomEffects *fx, u8 *out);

void func_001D3E40(u8 room, u8 a, u8 b);
#ifdef NON_MATCHING   /* first draft */
void func_001D3E40(u8 room, u8 a, u8 b) {
    char name[0x100];
    s32 *p;
    u32 size;
    s32 n1;
    s32 n3;

    func_001D0440(D_0023BE2C * 0x30 + 0x2100);
    func_001AE9C0(0x15, 0);
    func_001AE9C0(1, 0);
    func_001AE9C0(0x12, 0);
    func_001AE9C0(0x62, 0);
    func_001AE9C0(0xC, 0);
    func_001AE9C0(0x66, 1);
    func_001AE9C0(2, 0);
    if (D_00306590.unk17 == 0) {
        D_0038E774.buf = func_001B1AD0(0x400, 7);
        D_0038E774.unk818 = NULL;
        func_001D1310((room << 16) | (a << 8) | b, -1, &D_0038E774, 2);
    }
    D_00363994 = &D_0038E774;
    D_0036397C.model = &D_0038E774;
    D_0036397C.n1 = 0;
    p = (s32 *)(D_003065DC + 0x200000);
    D_0036397C.n3 = 0;
    D_0036397C.n2 = 0;
    memset(name, 0, sizeof(name));
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238230, D_0023A0F8, D_00306590.unk1A, D_00306590.unk1A, D_00306590.unk1C,
                D_00306590.unk1E);
        size = func_001CFBB0(name, p, 1, 0);
    } else {
        sprintf(name, D_00238250, D_00306590.unk1A, D_00306590.unk1C, D_00306590.unk1E);
        size = func_001A96E0(name, p);
    }
    if (size == 0) {
        return;
    }
    n1 = *p++;
    memcpy(D_00363910, p, n1 * 8);
    p = (s32 *)((u8 *)p + ((n1 * 8) & ~3));
    D_0036397C.n2 = *p++;
    memcpy(D_003632F0, p, D_0036397C.n2 * 12);
    p = (s32 *)((u8 *)p + ((D_0036397C.n2 * 12) & ~3));
    n3 = *p++;
    if (D_0023BB98 != 0) {
        if (n3 > 0x20 || size > 0x8000) {
            while (!(D_003064D0[0] & 0x8000)) {
                func_001AF820();
                func_001AE8E0(1);
                if (n3 > 0x20) {
                    func_001B26D0(0xD, 0xA, 2, -1, D_00238270, n3);
                } else {
                    func_001B26D0(0xD, 0xA, 2, -1, D_00238290, size);
                }
                func_001B26D0(0xD, 0xB, 2, -1, D_002382B0);
                func_001B26D0(0xD, 0xD, 2, -1, D_002382C0);
                func_001B9140(1);
            }
        }
        func_001AE8E0(0);
    }
    memcpy(D_00363310, p, n3 * 0x30);
    func_0055FB60((u8 *)p + ((n3 * 0x30) & ~3), &D_0036397C, D_0037B9B0);
    D_0036397C.n1 = n1;
    D_0036397C.n3 = n3;
}
#endif

/* The room's hit data (w%03d%02d%02d.bin walls, g... ground); 1 and 2 say which loaded. */
extern char D_002380C0[];   /* "%03d%02d%02d.bin" */
extern char D_002380E0[];   /* "DATA\\ROM\\MODEL\\ST_HIT\\r%03d\\w%s" */
extern char D_00238100[];   /* "w%s" */
extern char D_00238110[];   /* "DATA\\ROM\\MODEL\\ST_HIT\\r%03d\\g%s" */
extern char D_00238130[];   /* "g%s" */
extern u8 *D_003065FC;          /* (Options) */
void func_005E4E00(u8 *walls);
void func_005E4F30(u8 *ground);

s32 func_001D15D0(u8 room, u8 a, u8 b);
#ifdef NON_MATCHING   /* first draft */
s32 func_001D15D0(u8 room, u8 a, u8 b) {
    char id[0x100];
    char name[0x100];
    s32 r = 0;
    u8 *buf = D_003065FC;
    u32 off = 0;
    s32 n;

    sprintf(id, D_002380C0, room, a, b);
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_002380E0, room, id);
        n = func_001CFBB0(name, buf, 1, 0);
    } else {
        sprintf(name, D_00238100, id);
        n = func_001A96E0(name, buf);
    }
    if (n > 0) {
        off = (n + 0x3F) & ~0x3F;
        func_005E4E00(buf);
        r = 1;
    }
    buf += off;
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238110, room, id);
        n = func_001CFBB0(name, buf, 1, 0);
    } else {
        sprintf(name, D_00238130, id);
        n = func_001A96E0(name, buf);
    }
    if (n > 0) {
        func_005E4F30(buf);
        r += 2;
    }
    return r;
}
#endif

void func_001D56F0(u8 room, u8 a, u8 b) {
    func_001D15D0(room, a, b);
}
