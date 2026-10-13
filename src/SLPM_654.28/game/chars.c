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
    u8 unk0[4];
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
