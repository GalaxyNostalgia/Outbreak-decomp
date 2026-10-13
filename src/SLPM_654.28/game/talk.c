#include "common.h"
#include "game.h"
#include "object.h"

/* The characters' ad-libs (the R2 "talk" commands): who wants to talk this frame, and the
   speech that follows. */

extern DrawObjFull D_00476DD0[];
extern u32 D_0023BE14;          /* players */
extern Npc *D_0023BE04;
extern s16 D_006EF7B0[];
s32 func_001AF4D0(void);
s32 func_006859A0(DrawObjFull *p);
void func_00686CF0(void);
void func_00685D80(void *o);
void func_00686090(void *o);
void func_006871E0(void);
void func_00685CF0(void);

void func_00685AA0(void);
#ifdef NON_MATCHING   /* first draft */
void func_00685AA0(void) {
    DrawObjFull *p;
    Npc *n;
    u32 i;
    s8 v;

    if (func_001AF4D0() == 0) {
        p = &D_00476DD0[D_0048BF60.unk1D];
        if (func_006859A0(p) == 0 && (FIELD(&D_0048BF60, u32, 0xC0) & 0x100)) {
            FIELD(&D_00476DD0[D_0048BF60.unk1D], s8, 0xDA8) = 1;
            FIELD(&D_00476DD0[D_0048BF60.unk1D], s8, 0xDA9) = 0;
        }
    } else {
        for (i = 0, p = D_00476DD0; i < D_0023BE14; i++, p++) {
            if (p->kind == 0) {
                if (func_006859A0(p) == 0 && (FIELD(&D_0048BF60, u32, 0xC0) & 0x100)) {
                    FIELD(p, s8, 0xDA8) = 1;
                    FIELD(p, s8, 0xDA9) = 0;
                }
            } else if (FIELD(p, u8, 0x1098) != 0) {
                FIELD(p, s8, 0xDA8) = 1;
                v = D_006EF7B0[FIELD(p, u8, 0x1098) - 1];
                FIELD(p, s8, 0xDA9) = v;
                if (v < 0) {
                    FIELD(p, s8, 0xDA8) = 0;
                    FIELD(p, s8, 0xDA9) = 0;
                }
                FIELD(p, u8, 0x1098) = 0;
            }
        }
    }
    func_00686CF0();
    if (func_001AF4D0() == 1) {
        for (i = 0, p = D_00476DD0; i < D_0023BE14; i++, p++) {
            func_00685D80(p);
            func_00686090(p);
        }
    } else {
        func_00685D80(&D_00476DD0[D_0048BF60.unk1D]);
        for (i = 0, p = D_00476DD0; i < D_0023BE14; i++, p++) {
            func_00686090(p);
        }
    }
    for (n = D_0023BE04; n != NULL; n = n->next) {
        func_00685D80(n);
        func_00686090(n);
    }
    func_006871E0();
    func_00685CF0();
}
#endif
