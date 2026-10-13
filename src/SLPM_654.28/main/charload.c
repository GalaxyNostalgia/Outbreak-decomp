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
void func_001D5700(Slot1468 *m, void *model, s32 a);
void func_001B1D30(Slot1468 *m);
void func_001D5920(Slot1468 *m, void *model, u8 mode);
void func_001D11E0(s32 id, Slot1468 *m, void *motion);

/* Sets a model up from its data (and its motion, when there is one). */
s32 func_001D2720(Slot1468 *m, s32 mode, void *model, void *motion, s32 a) {
    func_001D5700(m, model, a);
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

