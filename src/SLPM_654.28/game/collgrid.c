#include "common.h"

/* GAME.BIN: the room's collision grid (from the room data): a cell per (x, z) with a list of
   entries (-1 ends), made pointers when it is set up. */

typedef struct {
    u8 unk0[8];
    s32 unk8;           /* 0x08: (the rest from here: offsets from here) */
    s32 unkC;
    u32 ny;             /* 0x10 */
    u32 nx;             /* 0x14 */
    s32 unk18;
    s32 unk1C;
    s32 cells;          /* 0x20 */
    s32 list;           /* 0x24 */
} CollFile;

typedef struct {
    s32 unk0;
    s32 unk4;
    u32 ny;             /* 0x08 */
    u32 nx;             /* 0x0C */
    s32 unk10;
    s32 unk14;
    s32 **cells;        /* 0x18 */
    u8 *list;           /* 0x1C */
} CollGrid;

/* The two grids (0x3938A8, 0x3938C8), and a word for each (0x3938E8). */
typedef struct {
    CollGrid g[2];
    s32 unk40[2];
} CollGrids;

extern CollGrids D_003938A8;
void func_005E8600(void);

void func_005E4E00(CollFile *f);
#ifdef NON_MATCHING   /* an addu's operand order */
void func_005E4E00(CollFile *f) {
    u8 *base = (u8 *)&f->unk8;
    u32 y;
    u32 x;
    s32 k;
    s32 **c;

    D_003938A8.g[0].unk0 = f->unk8;
    D_003938A8.g[0].unk4 = f->unkC;
    D_003938A8.g[0].ny = f->ny;
    D_003938A8.g[0].nx = f->nx;
    D_003938A8.g[0].unk10 = f->unk18;
    D_003938A8.g[0].unk14 = f->unk1C;
    D_003938A8.g[0].cells = (s32 **)(base + f->cells);
    D_003938A8.unk40[0] = 0;
    D_003938A8.g[0].list = base + f->list;
    func_005E8600();
    for (y = 0; y < ((CollFile *)(base - 8))->ny; y++) {
        for (x = 0; x < ((CollFile *)(base - 8))->nx; x++) {
            c = &D_003938A8.g[0].cells[x + y * ((CollFile *)(base - 8))->nx];
            *c = (s32 *)((u8 *)*c + (s32)base);
            for (k = 0; (*c)[k] != -1; k++) {
                (*c)[k] += (s32)D_003938A8.g[0].list;
            }
        }
    }
}

/* The second grid (no reset). */
void func_005E4F30(CollFile *f) {
    u8 *base = (u8 *)&f->unk8;
    u32 y;
    u32 x;
    s32 k;
    s32 **c;

    D_003938A8.g[1].unk0 = f->unk8;
    D_003938A8.g[1].unk4 = f->unkC;
    D_003938A8.g[1].ny = f->ny;
    D_003938A8.g[1].nx = f->nx;
    D_003938A8.g[1].unk10 = f->unk18;
    D_003938A8.g[1].unk14 = f->unk1C;
    D_003938A8.g[1].cells = (s32 **)(base + f->cells);
    D_003938A8.unk40[1] = 0;
    D_003938A8.g[1].list = base + f->list;
    for (y = 0; y < ((CollFile *)(base - 8))->ny; y++) {
        for (x = 0; x < ((CollFile *)(base - 8))->nx; x++) {
            c = &D_003938A8.g[1].cells[x + y * ((CollFile *)(base - 8))->nx];
            *c = (s32 *)((u8 *)*c + (s32)base);
            for (k = 0; (*c)[k] != -1; k++) {
                (*c)[k] += (s32)D_003938A8.g[1].list;
            }
        }
    }
}
#endif

extern s32 D_0023BEC4;
extern s32 D_0070E6D0[];

void func_005E8600(void) {
    D_0023BEC4 = 0;
    D_0070E6D0[0] = -1;
}
