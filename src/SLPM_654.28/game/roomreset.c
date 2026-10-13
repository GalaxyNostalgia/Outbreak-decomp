#include "common.h"
#include "game.h"
#include "object.h"

/* Small resets and hooks of the room setup. */

void *memset(void *p, s32 c, u32 n);
extern u8 D_00386650[0x100];
extern u8 D_00386610[0x40];

extern s16 D_0023BD94;

void func_005A9CA0(void) {
    D_0023BD94 = 0;
    memset(D_00386650, 0, sizeof(D_00386650));
}

void func_006BE5A0(void) {
    memset(D_00386610, 0, sizeof(D_00386610));
}

void func_005A9C70(void) {
    func_005A9CA0();
    func_006BE5A0();
}

extern DrawObjFull D_00476DD0[];
void func_00551590(u8 a, u8 b, DrawObjFull *player);

void func_005BAED0(void);
#ifdef NON_MATCHING   /* first draft */
void func_005BAED0(void) {
    DrawObjFull *p;

    if (D_0048BF60.unk2B != 0 && D_0048BF60.state != 4) {
        p = &D_00476DD0[D_0048BF60.unk1D];
        if (D_0048BF60.unk64 != 0) {
            D_0048BF60.unk35 = 0;
            D_0048BF60.unk37 = 1;
        }
        func_00551590(D_0048BF60.unk37, D_0048BF60.unk35, p);
        D_0048BF60.unk37 = 0;
    }
}
#endif
