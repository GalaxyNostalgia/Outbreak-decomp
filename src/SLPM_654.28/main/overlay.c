#include "cri.h"
#include "sce.h"
#include "ps2mem.h"

/* Loading the code overlays (PROG/*.BIN). Each starts with a Metrowerks "MWo3" header and is linked
   to run at a fixed address; three ranges are shared: 0x540000 (DEMO, GAME, NETWK), 0x6C0000
   (NETDNAS, NETHTTP) and 0x820000 (EDITOR, NETAQ, SINGLE). GAME is big enough to cover 0x6C0000 too. */

typedef struct {
    u32 magic;          /* 0x00: "MWo3" */
    u32 id;             /* 0x04 */
    u32 load;           /* 0x08: where it runs (the header is part of the image) */
    u32 textSize;       /* 0x0C */
    u32 dataSize;       /* 0x10 */
    u32 bssSize;        /* 0x14 */
    u32 end;            /* 0x18: end of text and data */
    u32 sinitEnd;       /* 0x1C */
    char name[0x20];    /* 0x20 */
} OvlHeader;

void MWo3_Init(void *image, s32 size);   /* Metrowerks runtime: clears bss, runs static initialisers */

/* Copy a loaded overlay to where it runs and start it. */
s32 func_001A8870(OvlHeader *h) {
    s32 size;
    u32 load;

    if ((h->magic & 0xFFFFFF) != 0x6F574D) {   /* "MWo" */
        return 0;
    }
    load = h->load;
    size = h->end - load;
    if (size <= 0) {
        return 0;
    }
    memcpy(PS2_RAM(load), h, size);
    MWo3_Init(PS2_RAM(load), size);
    return 1;
}

/* Read a whole file from the disc into buf. */
s32 func_001A88F0(const char *name, void *buf) {
    char path[0x800];
    ADXF adxf;

    strcpy(path, name);
    while ((adxf = ADXF_Open(path, NULL)) == NULL) {
    }
    ADXF_ReadNw(adxf, ADXF_GetFsizeSct(adxf), buf);
    while (ADXF_GetStat(adxf) != 3) {
    }
    ADXF_Close(adxf);
    return 1;
}

extern const char *D_002136C0[];   /* overlay file names */
extern u32 D_002136F0[];           /* where each is read to */
extern s8 D_00239F30;              /* overlay at 0x540000, -1 none */
extern s8 D_00239F34;              /* overlay at 0x820000 */
extern s8 D_00239F38;              /* overlay at 0x6C0000 */
void func_001D9810(s32 id);        /* apply the patches downloaded for overlay `id` */

void func_001A8990(u32 ovl) {
    s8 *slot;
    void *buf;
    s32 ok;

    switch (ovl) {
    case 0:
        D_00239F30 = -1;
        D_00239F34 = -1;
        D_00239F38 = -1;
        break;
    case 1:
    case 2:
    case 3:
        slot = &D_00239F30;
        if (ovl == D_00239F30) {
            return;
        }
        if (ovl == 2) {
            D_00239F38 = -1;
        }
        break;
    case 4:
        /* NETDNAS: not while GAME covers its range */
        if (D_00239F30 != 3 && D_00239F30 != 1 && D_00239F30 != -1) {
            return;
        }
        slot = &D_00239F38;
        break;
    case 5:
        /* NETHTTP: only with NETWK */
        if (D_00239F30 != 3) {
            return;
        }
        slot = &D_00239F38;
        break;
    case 6:
    case 7:
    case 8:
        slot = &D_00239F34;
        if (ovl == D_00239F34) {
            return;
        }
        break;
    }
    buf = PS2_RAM(D_002136F0[ovl]);
    if (func_001A88F0(D_002136C0[ovl], buf) != 0) {
        ok = func_001A8870(buf);
        func_001D9810(ovl + 1);
        if (ovl != 0 && ok == 1) {
            *slot = ovl;
        }
    }
}

extern u8 D_0030665A[];     /* (Options) overlay to come back to */

/* Loads SINGLE.BIN/NETAQ.BIN's slot (overlay 4). */
void func_001A8B40(void) {
    func_001A8990(4);
    D_0030665A[0] = 4;
}
