#include "common.h"
#include "options.h"

/* SUBMAIN.BIN: the scenario's item pool and counting its kinds. */

typedef struct { u8 unk0[0x18]; } Obj18;

extern Obj18 D_0035D470[0x80];
extern Obj18 *D_0035E070[0x80];
extern s16 D_0023BD30;
extern s32 D_0023BD34;
extern s32 D_0023BD38;
extern s8 D_0023BD2C;

void func_0034B8D0(void) {
    u8 i;

    memset(D_0035D470, 0, sizeof(D_0035D470));
    D_0023BD34 = 0;
    D_0023BD30 = 0x80;
    D_0023BD38 = 0;
    D_0023BD2C = 0;
    for (i = 0; i < 0x80; i++) {
        D_0035E070[i] = &D_0035D470[i];
    }
}

typedef struct {
    u8 unk0;            /* 0xFF ends a list */
    u8 unk1[5];
    u16 id;             /* 0x06 */
    u8 unk8[2];
} ItemPlace;            /* 0x0A */

typedef struct {
    ItemPlace *a;
    ItemPlace *b;
} ItemLists;

extern u8 D_00389650[0xE8];
extern u8 D_003896A4[];         /* (inside D_00389650) kinds of item in the scenario */
extern u8 D_0035D430[0x40];
extern u8 D_0035D3F0[0x40];
extern s8 D_00359AC0[];         /* by scenario: which lists */
extern ItemLists D_00359AF0[];

void func_0034B100(void);
#ifdef NON_MATCHING   /* the bit test is ordered differently */
void func_0034B100(void) {
    u32 seen[2];
    ItemPlace *p;
    s32 s;

    memset(D_00389650, 0, sizeof(D_00389650));
    func_0034B8D0();
    memset(D_0035D430, 0, sizeof(D_0035D430));
    memset(D_0035D3F0, 0, sizeof(D_0035D3F0));
    seen[1] = 0;
    seen[0] = 0;
    s = D_00359AC0[D_00306590.unk1A];
    for (p = D_00359AF0[s].a; p->unk0 != 0xFF; p++) {
        if (!(seen[p->id >> 5] & (1 << (p->id & 0x1F)))) {
            seen[p->id >> 5] |= 1 << (p->id & 0x1F);
            D_003896A4[0]++;
        }
    }
    for (p = D_00359AF0[s].b; p->unk0 != 0xFF; p++) {
        if (!(seen[p->id >> 5] & (1 << (p->id & 0x1F)))) {
            seen[p->id >> 5] |= 1 << (p->id & 0x1F);
            D_003896A4[0]++;
        }
    }
}
#endif
