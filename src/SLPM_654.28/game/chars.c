#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"
#include "model.h"

void func_0018D730(void);
void func_001D0300(s32 first, s32 n);
void func_001D0440(s32 base);

/* The characters' models: D_0070E4D0's slots say which kind (and detail) each slot has. */

typedef struct {
    u8 room;            /* 0x00: the room (and its two sub-numbers) loaded */
    u8 sub1;
    u8 sub2;
    u8 unk3;
    struct {
        u8 kind;        /* 0xFF: none */
        u8 high;
    } slot[8];          /* 0x04 */
} LoadedChars;

extern LoadedChars D_0070E4D0;
extern Slot1468 D_0047F4D0[];
extern u8 D_003BB520[][0x824];
extern u8 D_004A8180[][0x1F44];
void func_001B0450(Slot1468 *m);
void func_001B1320(u8 *held);
void func_001B0C60(u8 *p);
void func_00638C00(DrawObjFull *obj, u8 kind);
void func_001D5030(DrawObjFull *obj, u8 kind, u8 costume);
void func_001D35E0(DrawObjFull *obj, u8 kind, u8 costume);
void func_001D4E00(u8 kind);
void func_001A9A10(s32 slot, DrawObjFull *obj);

void func_005BB4B0(s32 slot, DrawObjFull *obj, s32 tex);
#ifdef NON_MATCHING   /* first draft: the model's buf is a separate symbol in the original */
void func_005BB4B0(s32 slot, DrawObjFull *obj, s32 tex) {
    u8 **buf = &D_0047F4D0[slot].buf;

    if (*buf != NULL) {
        func_001B0450(&D_0047F4D0[slot]);
    }
    if (tex == 0x1100) {
        func_001B1320(D_003BB520[slot]);
        func_001D0300(slot * 8 + 0x1100, 8);
    } else {
        func_001D0300(slot * 8 + 0x1000, 8);
    }
    obj->visible = 0;
    *buf = NULL;
}
#endif

void func_005BB580(s32 kind) {
    func_001B0C60(D_004A8180[kind]);
}

void func_005BB420(s32 slot, DrawObjFull *obj) {
    func_001D0440(slot * 8 + 0x1000);
    func_001D5030(obj, obj->unkB98, obj->costume);
    func_001D35E0(obj, obj->unkB98, obj->costume);
    func_00638C00(obj, obj->unkB98);
}

void func_005BB480(DrawObjFull *obj) {
    if (D_0048BF60.unk28 == 0) {
        func_001D4E00(obj->unkB98);
    }
}

void func_005BB5A0(s32 slot, DrawObjFull *obj);
#ifdef NON_MATCHING   /* first draft */
/* Character slot's model, reloaded when it holds another kind (or detail). */
void func_005BB5A0(s32 slot, DrawObjFull *obj) {
    s32 shared = 0;
    s32 i;
    s32 tex;

    for (i = 0; i < 8; i++) {
        if (D_0070E4D0.slot[i].kind == obj->unkB98) {
            shared = 1;
            break;
        }
    }
    if (D_0070E4D0.slot[slot].kind != 0xFF) {
        if (D_0070E4D0.slot[slot].kind == obj->unkB98 && D_0070E4D0.slot[slot].high == obj->unk54D) {
            func_00638C00(obj, obj->unkB98);
            return;
        }
        tex = 0x1000;
        if (D_0070E4D0.slot[slot].high == 1) {
            tex = 0x1100;
        }
        func_005BB4B0(slot, obj, tex);
        func_005BB580(D_0070E4D0.slot[slot].kind);
        func_0018D730();
    }
    if (D_00306590.unk17 == 0) {
        func_005BB420(slot, obj);
        if (shared == 0) {
            func_005BB480(obj);
        }
    } else {
        func_001A9A10(slot, obj);
        if (shared == 0) {
            func_005BB480(obj);
        }
    }
    D_0070E4D0.slot[slot].kind = obj->unkB98;
    D_0070E4D0.slot[slot].high = obj->unk54D;
    func_0018D730();
}
#endif

extern f32 D_00712280[];
s32 func_001AF4D0(void);
void func_008220A0(DrawObjFull *obj);

void func_00639100(DrawObjFull *obj) {
    if (obj->active != 0) {
        obj->unkD2C = -1;
        obj->unkD28 = -1;
        obj->unkBB0 = D_00712280[obj->unkB98];
        obj->unkBAC = 0;
        obj->unkBB6 = 0;
        obj->unkBB8 = 0;
        if (func_001AF4D0() == 1) {
            func_008220A0(obj);
        }
    }
}

/* The room's geometry, when it isn't the one loaded. */
extern u8 D_004700F0[];
extern u8 D_0038E774[];
void func_001B08C0(void *m);
void func_001D5110(u8 room, u8 a, u8 b);
void func_001D3C30(u8 room, u8 a, u8 b);
void func_001D3E40(u8 room, u8 a, u8 b);
void func_001A97A0(u8 room, u8 a, u8 b);
void func_001D56F0(u8 room, u8 a, u8 b);

void func_005BB2D0(u8 room, u8 a, u8 b);
#ifdef NON_MATCHING   /* first draft */
void func_005BB2D0(u8 room, u8 a, u8 b) {
    if (D_0070E4D0.room != room || D_0070E4D0.sub1 != a || D_0070E4D0.sub2 != b) {
        func_001B08C0(D_004700F0);
        func_001B08C0(D_0038E774);
        func_001D0300(0x1800, 0x800);
        func_0018D730();
        D_0070E4D0.room = room;
        D_0070E4D0.sub1 = a;
        D_0070E4D0.sub2 = b;
        if (D_00306590.unk17 == 0) {
            func_001D5110(room, a, b);
            func_0018D730();
            func_001D3C30(room, a, b);
            func_0018D730();
            func_001D3E40(room, a, b);
            func_0018D730();
        } else {
            func_001A97A0(room, a, b);
            func_001D3E40(room, a, b);
            func_0018D730();
        }
        func_001D56F0(room, a, b);
    }
}
#endif

/* A character's setup: model, parts, motions and the per-character tables; when the characters
   are set up already only the start position (unless Options.unk18). */
typedef struct {
    u8 unk0[0x10];
    u32 hidden;         /* 0x10: parts the NPC model doesn't draw */
} NpcModelInfo;

typedef struct {
    u8 unk0[0xA];
    s16 unkA;           /* 0x0A */
    u8 unkC;
    u8 unkD;            /* 0x0D */
    u8 unkE[0x2E - 0xE];
    u8 unk2E;           /* 0x2E */
    u8 unk2F[0x10C - 0x2F];
} SavedChar;

extern NpcModelInfo D_006E2490[];
extern s8 D_0023AE78[];
extern u8 D_0023AE80[];
extern s16 D_006E6ED0[];
extern f32 D_006E6CB0[][3];
extern f32 D_006E6D10[];
extern u8 D_002F88D0[];
extern SavedChar D_002FC4D0[];
extern u8 D_004BBA40[];
extern u8 D_004B7BA0[];
extern u8 D_004B9AF0[];
extern u8 *D_006E7C30[];
extern f32 **D_006E7C50[];
extern s32 *D_006E7C70[];
extern s32 D_0023AE88;
void func_006399A0(DrawObjFull *obj);
void func_00341EB0(DrawObjFull *obj);
void func_00637D30(DrawObjFull *obj, u16 npc);
s16 func_005C1240(DrawObjFull *obj);
s32 func_0062E0F0(DrawObjFull *obj);
void func_005A5FC0(DrawObjFull *obj);
u8 func_006BC9D0(DrawObjFull *obj);

#ifdef NON_MATCHING   /* first draft */
void func_00638C00(DrawObjFull *obj, u8 kind) {
    u8 slot = obj->kind;
    f32 *p;
    u16 i;
    u32 k;

    if (D_0048BF60.unk2B == 0 || D_0048BF60.unk28 != 0) {
        obj->active = 1;
        obj->kind = slot;
        obj->unk6 = slot;
        obj->unkB98 = kind;
        obj->type = 0;
        obj->update = func_006399A0;
        obj->draw = (void (*)(DrawObjFull *))func_00341EB0;
        obj->model = (ObjModel *)&D_0047F4D0[obj->kind];
        obj->unk30 = obj->unkB98;
        for (i = 0; i < ((Slot1468 *)obj->model)->nparts; i++) {
            obj->unkF4[i] = 1;
        }
        if (obj->unk54D != 1) {
            obj->unkF4[1] = 0;
            if (obj->unkBB4 != 0) {
                for (k = 0; k < 0x20; k++) {
                    if ((1 << k) & D_006E2490[obj->unkBB4 - 1].hidden) {
                        obj->unkF4[k] = 0;
                    }
                }
            }
        }
        if (obj->unkBB4 != 0) {
            func_00637D30(obj, obj->unkBB4);
        } else {
            obj->unkB99 = D_0023AE78[obj->unkB98];
            obj->unkB9A = D_0023AE80[obj->unkB98];
            obj->unk544 = D_006E6ED0[obj->unkB98];
            obj->unk546 = D_006E6ED0[obj->unkB98];
            obj->unkB4[0] = D_006E6CB0[obj->unkB98][0];
            obj->unkB4[1] = D_006E6CB0[obj->unkB98][1];
            obj->unkB4[2] = D_006E6CB0[obj->unkB98][2];
            obj->unkAA8 = D_006E6D10[obj->unkB98];
            obj->unkBC4 = 1.0f;
            obj->unkBC0 = 1.0f;
        }
        if (D_002F88D0[0] != 0) {
            obj->unk544 = D_002FC4D0[obj->kind].unkA;
            obj->unkB9B = D_002FC4D0[obj->kind].unkD;
            obj->unkC7C = D_002FC4D0[obj->kind].unk2E;
            obj->unkCA8 = func_005C1240(obj);
        }
        func_0062E0F0(obj);
        obj->unkB9C = obj->unkB9B;
        obj->unkA8 = 0;
        obj->unkAC = 0;
        obj->unkB0 = 0;
        obj->motions[0] = D_004BBA40;
        obj->motions[1] = (u8)obj->unkB99 != 0 ? D_004B7BA0 : D_004B9AF0;
        obj->motions[2] = D_004A8180[obj->unkB98];
        obj->motions2[0] = D_004BBA40;
        obj->motions2[1] = (u8)obj->unkB99 != 0 ? D_004B7BA0 : D_004B9AF0;
        obj->motions2[2] = D_004A8180[obj->unkB98];
        obj->unk524 = 5;
        obj->unkD8[2] = 0;
        obj->unkD8[1] = 0;
        obj->unkD8[0] = 0;
        obj->unkE4[2] = 0;
        obj->unkE4[1] = 0;
        obj->unkE4[0] = 0;
        obj->unkF0 = 1.0f;
        obj->unk1A8 = (s32)D_006E7C30[obj->unkB98];
        p = *D_006E7C50[obj->unkB98];
        obj->unk1AC[0] = p[0];
        obj->unk1AC[1] = p[1];
        obj->unk1B4[0] = p[2];
        obj->unk1B4[1] = p[3];
        obj->unk1BC[0] = obj->unk1AC[0];
        obj->unk1BC[1] = obj->unk1AC[1];
        obj->unk1C4[0] = obj->unk1B4[0];
        obj->unk1C4[1] = obj->unk1B4[1];
        obj->unkCB8 = (s32)*D_006E7C50[obj->unkB98];
        obj->unkAB0 = D_0023AE88;
        obj->unk1D8 = *D_006E7C70[obj->unkB98];
        obj->unk1DC = obj->unkAEC;
        obj->unkD90 = 0;
        func_005A5FC0(obj);
        obj->unk199 = func_006BC9D0(obj);
    } else if (D_00306590.unk18 == 0) {
        func_005A5FC0(obj);
        if (obj->unkD94[0] != 0.0f || obj->unkD94[1] != 0.0f || obj->unkD94[2] != 0.0f || obj->unkDA0 != 0) {
            ((f32 *)obj->unk38)[0] = obj->unkD94[0];
            ((f32 *)obj->unk38)[1] = obj->unkD94[1];
            ((f32 *)obj->unk38)[2] = obj->unkD94[2];
            obj->unk92 = obj->unkDA0;
        }
    }
    obj->unkBA4 |= 2;
    obj->unkBA4 &= ~0xC000;
    obj->unkCEC = 0;
    obj->unkCF0 = 0;
    obj->unkCE8 = -1;
}
#endif

/* The condition from the health percentage (D_006E6EE0: the two thresholds) and the status. */
extern f32 D_006E6EE0[2];

#ifdef NON_MATCHING   /* first draft */
s32 func_0062E0F0(DrawObjFull *obj) {
    u8 i;

    if (obj->type >= 2) {
        return 0;
    }
    if (obj->unkBA0 & 8) {
        obj->unkB9B = 3;
        return 0;
    }
    if (obj->unkBA0 & 2) {
        obj->unkB9B = 2;
        return 0;
    }
    for (i = 0; i < 2; i++) {
        if (!(100.0f * obj->unk544 / obj->unk546 < D_006E6EE0[i])) {
            obj->unkB9B = i;
            return 0;
        }
    }
    obj->unkB9B = 2;
    return 0;
}
#endif

/* The character's unk199 (players: by character, partners 10; type 1: 30; type 2: per slot,
   offline or online table). */
extern u8 D_006FFFF0[];
extern u8 D_00700040[];

#ifdef NON_MATCHING   /* first draft (the switch) */
u8 func_006BC9D0(DrawObjFull *obj) {
    u8 t[8] = {8, 9, 5, 4, 7, 4, 3, 2};

    switch (obj->type) {
    case 0:
        if (obj->isNpc == 0) {
            return 10;
        }
        return t[obj->unkB98];
    case 1:
        return 30;
    case 2:
        if (func_001AF4D0() == 1) {
            return D_00700040[obj->kind];
        }
        return D_006FFFF0[obj->kind];
    }
    return 0;
}
#endif
