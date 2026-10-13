#include "common.h"
#include "options.h"
#include "sce.h"
#include "ps2mem.h"
#include "libgraph.h"
#include "libdma.h"
#include "cri.h"

/* SUBMAIN.BIN: the game's movie player on CRI Sofdec. Movies come from the AFS archives; each
   decoded frame (RGBA, 16x16 macroblocks one after another) is uploaded to GS memory and drawn
   as a sprite. */

/* The player */
typedef struct {
    MwPly *ply;             /* 0x00 */
    MwsfdCrePrm cprm;       /* 0x04 */
    MwsfdFrmObj frm;        /* 0x34: the last frame shown */
    u8 stat;                /* 0x80 */
    u8 unk81[0xB];
    s16 width;              /* 0x8C */
    s16 height;             /* 0x8E */
    u8 unk90;
    s8 unk91;               /* 0x91 */
    u8 unk92[4];
    u16 errFrames;          /* 0x96: frames in state 1 */
} Movie;

/* Its memory: Sofdec's work area, then the GS packets */
typedef struct {
    u32 work;               /* 0x00: Sofdec's work area (64-byte aligned) */
    u32 workSize;           /* 0x04 */
    s16 width;              /* 0x08 */
    s16 height;             /* 0x0A */
    u32 *upload;            /* 0x0C: upload packet (and the end of the work area) */
    u128 *sprite[2];        /* 0x10: two sprite packets, used in turn */
    u32 vram;               /* 0x18: GS block address of the picture */
    s32 which;              /* 0x1C */
    u32 *blocks;            /* 0x20: the upload packet's per-macroblock part */
    s32 packetSize;         /* 0x24 */
} MovieMem;

extern Movie D_003209B0;
extern Movie *D_0023BC70;
extern s32 D_0023BC74;
extern s32 D_0023BD18;      /* picture width */
extern s32 D_0023BD1C;      /* picture height */
extern s32 D_0023BD20;      /* frame number (for the debug display) */
extern u32 D_0023BD24;      /* the frame being shown */
extern MovieMem *D_0023BD28;
extern s32 D_0023A148;
extern s8 D_0035D350[];
extern u8 D_0048BF88[];
extern u8 D_0048BF9B[];
extern s8 D_0048BF9C[];
extern struct { u8 used, id; } D_0035C810[0x14];

/* Options */
extern u8 D_003065A5[];     /* run func_00349EC0 every frame */
extern u8 D_003065CC[];     /* movie kind: 0 none, 1/2 full screen, 3 small */
extern u8 D_003065CD[];     /* English titles */
extern u8 D_003065CF[];     /* the movie is preparing */
extern u8 D_003065D5[];     /* GS memory for movies: 0 none, 1 small, 2 a movie's */
extern u16 D_003065E6[];    /* its GS address */
extern s32 D_003065E8[];    /* its size */
extern u32 D_003065DC[];
extern u32 D_0030662C[];

/* SysWork */
extern s32 D_00281DBC[];    /* width */
extern s32 D_00281DC0[];    /* display height */
extern void *D_0028217C[];  /* the GIF DMA channel */

s32 func_001AA5C0(const char *name);
s32 func_001B6980(s32 slot);
void func_001B9140(s32 a);
u32 func_0019BF60(u32 size);
void func_0019C020(u32 addr);
s32 func_001937B0(u32 *pkt);
s32 func_0018D0E0(void);
void func_001B2600(s32 a, s32 line, s32 color, const char *s);

void func_003479A0(void);
void func_003478F0(void);
void func_00348DB0(s32 w, s32 h);
void func_00348E60(void);
u32 *func_00348EC0(u32 *p, u32 src, s32 dbp, s32 psm, s32 w, s32 h);
void func_00349070(u32 *p, u32 src, s32 w, s32 h);
u32 *func_003490F0(u32 *p, s32 x, s32 y, s32 z, s32 w, s32 h);
void func_003493B0(void);
void func_00349530(u32 frame, s32 w, s32 h, s32 justDraw);
void func_00349680(u32 frame, s32 w, s32 h);
void func_00349700(u32 frame, s32 w, s32 h);
void func_00349780(s16 w, s16 h);
void func_003498B0(s16 w, s16 h);
void func_00349DE0(void);
void func_00349E20(void);

#ifdef NON_MATCHING   /* first draft */
void func_003479A0(void) {
    s8 i;

    for (i = 0; i < 0x14; i++) {
        D_0035C810[i].used = 0;
        D_0035C810[i].id = i;
    }
}
#endif

/* GS memory for a full-width 448-line picture. */
void func_003478F0(void) {
    if (D_003065D5[0] == 0) {
        D_003065E8[0] = D_00281DBC[0] * 0x380;
        D_003065E6[0] = func_0019BF60(D_003065E8[0]);
        D_003065D5[0] = 1;
        func_003479A0();
    }
}

s32 func_0019E4B0(void);

/* Drops the textures cached in GS memory. */
void func_00348DA0(void) {
    func_0019E4B0();
}

/* GS memory for a w x h movie. */
void func_00348DB0(s32 w, s32 h) {
    if (func_001B6980(0xE) == 0) {
        func_001B9140(3);
        if (D_003065D5[0] != 0) {
            func_0019C020(D_003065E6[0]);
            D_003065D5[0] = 0;
            D_003065E6[0] = 0;
            D_003065E8[0] = 0;
        }
        D_003065E8[0] = w * h * 4;
        D_003065E6[0] = func_0019BF60(D_003065E8[0]);
        D_003065D5[0] = 2;
        D_0023BD18 = w;
        D_0023BD1C = h;
    }
}

void func_00348E60(void) {
    if (D_003065D5[0] != 0) {
        func_0019C020(D_003065E6[0]);
        D_003065D5[0] = 0;
        D_003065E6[0] = 0;
        D_003065E8[0] = 0;
        if (D_00306590.unk0 != 0) {
            func_003478F0();
        }
    }
}

/* The upload packet: a w x h picture of psm to dbp, as 16x16 IMAGE transfers of 0x40
   quadwords each, column by column (their source addresses are filled in per frame). */
#ifdef NON_MATCHING   /* first draft */
u32 *func_00348EC0(u32 *p, u32 src, s32 dbp, s32 psm, s32 w, s32 h) {
    s32 bw, bh;
    s32 x, y;

    p[0] = 0x10000004;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    ((u64 *)p)[2] = 0x8003 | ((u64)0x10000000 << 32);
    ((u64 *)p)[3] = 0xE;
    ((u64 *)p)[4] = 0;
    ((u64 *)p)[5] = 0x3F;
    ((u64 *)p)[6] = ((u64)psm << 56) | ((u64)dbp << 32) | ((u64)(D_0023BD18 / 64) << 48);
    ((u64 *)p)[7] = 0x50;
    ((u64 *)p)[8] = 0x10 | ((u64)0x10 << 32);
    ((u64 *)p)[9] = 0x52;
    p += 20;
    D_0023BD28->blocks = p;
    bh = h / 16;
    bw = w / 16;
    for (x = 0; x < bw; x++) {
        for (y = 0; y < bh; y++) {
            p[0] = 0x10000004;
            p[1] = 0;
            p[2] = 0;
            p[3] = 0;
            ((u64 *)p)[2] = 2 | ((u64)0x10000000 << 32);
            ((u64 *)p)[3] = 0xE;
            ((u64 *)p)[4] = ((u64)(x * 16) << 32) | ((u64)(y * 16) << 48);
            ((u64 *)p)[5] = 0x51;
            ((u64 *)p)[6] = 0;
            ((u64 *)p)[7] = 0x53;
            ((u64 *)p)[8] = 0x8040 | ((u64)0x08000000 << 32);
            ((u64 *)p)[9] = 0;
            p[20] = 0x30000040;
            p[21] = src;
            p[22] = 0;
            p[23] = 0x51000040;
            p += 24;
        }
    }
    p[0] = 0x70000000;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    return p + 4;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Points the upload packet's macroblock transfers at a frame. */
void func_00349070(u32 *p, u32 src, s32 w, s32 h) {
    s32 bh = h / 16;
    s32 bw = w / 16;
    s32 y, x;

    /* macroblocks column by column */
    for (x = 0; x < bw; x++) {
        for (y = 0; y < bh; y++) {
            p[21] = src;
            p += 24;
            src += 0x400;
        }
        if (D_0023BD1C == 0x1E0) {
            src += 0x800;
        }
    }
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* The sprite packet: the picture at (x, y), w x h on screen. */
u32 *func_003490F0(u32 *p, s32 x, s32 y, s32 z, s32 w, s32 h) {
    s32 tw, th;
    s32 v, r = 0;
    s32 ox;

    for (tw = 0, v = D_0023BD18; v >= 2; tw++) {
        r = v % 2;
        v /= 2;
    }
    if (r != 0) {
        tw++;
    }
    v = D_0023BD1C;
    th = 0;
    if (v > 0x100 && v < 0x200) {
        v = 0x200;
    }
    for (; v >= 2; th++) {
        r = v % 2;
        v /= 2;
    }
    if (r != 0) {
        th++;
    }
    p[0] = 0xF000000A;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0x5100000A;
    ((u64 *)p)[2] = 5 | ((u64)0x10000000 << 32);
    ((u64 *)p)[3] = 0xE;
    ((u64 *)p)[4] = 0x44;
    ((u64 *)p)[5] = 0x42;           /* ALPHA_1 */
    ((u64 *)p)[6] = 5;
    ((u64 *)p)[7] = 8;              /* TEST_1 */
    ((u64 *)p)[8] = 0;
    ((u64 *)p)[9] = 0x3F;           /* TEXFLUSH */
    ((u64 *)p)[10] = ((u64)th << 30) | ((u64)tw << 26) | (D_0023BD28->vram | ((u64)(D_0023BD18 / 64) << 14)) |
                     ((u64)8 << 32);
    ((u64 *)p)[11] = 6;             /* TEX0_1 */
    ((u64 *)p)[12] = 0;
    ((u64 *)p)[13] = 0x14;          /* TEX1_1 */
    ((u64 *)p)[14] = 0x8001 | ((u64)0x64000000 << 32);
    ((u64 *)p)[15] = 0x535310;
    ox = (0x1000 - D_00281DBC[0]) / 2 * 16;
    ((u64 *)p)[16] = 0x116;
    ((u64 *)p)[17] = (0x80808080) | ((u64)0x3F800000 << 32);
    ((u64 *)p)[18] = 0;
    ((u64 *)p)[19] = ((u64)z << 32) | (s64)(ox + x * 16) | ((s64)(y * 16 + 0x7200) << 16);
    ((u64 *)p)[20] = (D_0023BD18 * 16) | ((s64)(D_0023BD1C * 16) << 16);
    ((u64 *)p)[21] = ((u64)z << 32) | (s64)(ox + (x + w) * 16) | ((s64)((y + h) * 16 + 0x7200) << 16);
    return p + 44;
}
#endif

/* (Re)builds the packets for the current picture size. */
void func_003493B0(void) {
    u8 zero[0x400];
    s32 h;

    D_0023BD28->vram = D_003065E6[0];
    memset(zero, 0, sizeof(zero));
    D_0023BD28->sprite[0] = (u128 *)((u32)D_0023BD28->upload + D_0023BD28->packetSize);
    D_0023BD28->sprite[1] = (u128 *)((u32)D_0023BD28->upload + D_0023BD28->packetSize + 0xC0);
    h = D_0023BD1C;
    if (h > 0x1C0) {
        h = 0x1C0;
    }
    func_00348EC0(D_0023BD28->upload, (u32)zero, D_0023BD28->vram, 0, D_0023BD18, h);
    if (D_003065CC[0] == 3) {
        func_003490F0((u32 *)D_0023BD28->sprite[0], 0, 0, 1, D_0023BD18 * 2, D_0023BD1C * 2);
        func_003490F0((u32 *)D_0023BD28->sprite[1], 0, 0, 1, D_0023BD18 * 2, D_0023BD1C * 2);
        return;
    }
    func_003490F0((u32 *)D_0023BD28->sprite[0], 0, (D_00281DC0[0] - h) / 2, 1, D_00281DBC[0], h);
    func_003490F0((u32 *)D_0023BD28->sprite[1], 0, (D_00281DC0[0] - h) / 2, 1, D_00281DBC[0], h);
}

#ifdef NON_MATCHING   /* first draft */
/* Uploads frame (unless it's 0, or justDraw) and draws the picture. */
void func_00349530(u32 frame, s32 w, s32 h, s32 justDraw) {
    if (D_003065D5[0] == 2) {
        if (D_0023BD28->vram != D_003065E6[0]) {
            func_003493B0();
        }
        if (justDraw != 1) {
            if (frame != 0) {
                if (D_0023BD1C == 0x1E0) {
                    frame += 0x400;
                }
                D_0023BD24 = frame;
                func_00349070(D_0023BD28->blocks, frame, w, h);
                FlushCache(0);
                func_0018D0E0();
                sceDmaSend((sceDmaChan *)D_0028217C[0], D_0023BD28->upload);
                sceGsSyncPath(0, 0);
            }
            if (D_0023BD24 != 0) {
                func_001937B0((u32 *)(D_0023BD28->which != 0 ? D_0023BD28->sprite[0] : D_0023BD28->sprite[1]));
                D_0023BD28->which ^= 1;
            }
        }
    }
}
#endif

void func_00349680(u32 frame, s32 w, s32 h) {
    if (D_003065D5[0] == 2) {
        if (w != D_0023BD18 || h != D_0023BD1C) {
            D_0023BD18 = w;
            D_0023BD1C = h;
            func_003493B0();
        }
        func_00349530(frame, w, h, 0);
    }
}

void func_00349700(u32 frame, s32 w, s32 h) {
    if (D_003065D5[0] == 2) {
        if (w != D_0023BD18 || h != D_0023BD1C) {
            D_0023BD18 = w;
            D_0023BD1C = h;
            func_003493B0();
        }
        func_00349530(frame, w, h, 1);
    }
}

#ifdef NON_MATCHING   /* first draft */
/* The player's memory for movie kind D_003065CC. */
void func_00349780(s16 w, s16 h) {
    u32 size;
    s32 packets;

    switch (D_003065CC[0]) {
    case 0:
        size = 0x2C0000;
        packets = 0x10000;
        D_0023BD28 = (MovieMem *)D_003065DC[0];
        break;
    case 3:
        size = 0x1C0000;
        packets = 0x10000;
        D_0023BD28 = (MovieMem *)(D_003065DC[0] + 0x100000);
        break;
    case 1:
    case 2:
        size = 0x4E4000;
        packets = 0x16000;
        D_0023BD28 = (MovieMem *)D_0030662C[0];
        break;
    }
    memset(D_0023BD28, 0, size);
    D_0023BD28->work = ((u32)D_0023BD28 + 0x6B) & ~0x3F;
    D_0023BD28->packetSize = packets;
    D_0023BD28->workSize = size - ((packets + 0x1EB) & ~0x3F);
    D_0023BD28->upload = (u32 *)(D_0023BD28->work + D_0023BD28->workSize);
    D_0023BD28->width = w;
    D_0023BD28->height = h;
}
#endif

#ifdef NON_MATCHING   /* first draft */
/* Creates the player for movies up to w x h. */
void func_003498B0(s16 w, s16 h) {
    MwsfdCrePrm *c;

    D_0023BD20 = 0;
    if (D_003065CC[0] == 0 && D_00306590.unk0 != 0 && D_0048BF88[0] != 0 && D_0048BF9B[0] == 0) {
        return;
    }
    func_00349780(w, h);
    D_0023BC70 = &D_003209B0;
    D_0023BC70->width = w;
    D_0023BC70->height = h;
    c = &D_0023BC70->cprm;
    memset(c, 0, sizeof(MwsfdCrePrm));
    c->unk20 = 0x11;
    D_0023BC70->cprm.ftype = 3;
    if (D_003065CC[0] != 3 && D_003065CC[0] != 0) {
        if (D_00306590.unk0 == 0) {
            c->nfrmPool = 2;
            c->maxBps = 4800000;
        } else if (D_003065CC[0] == 2) {
            c->nfrmPool = 2;
            c->maxBps = 4000000;
        } else {
            c->nfrmPool = 2;
            c->maxBps = 3000000;
        }
    } else {
        c->maxBps = 1500000;
        c->nfrmPool = 1;
    }
    c->maxWidth = (u16)D_0023BD28->width;
    c->maxHeight = (u16)D_0023BD28->height;
    c->unk14 = 2;
    c->workSize = mwPlyCalcWorkCprmSfd(c);
    if (c->workSize >= D_0023BD28->workSize) {
        D_0023BD28 = NULL;
        return;
    }
    D_0023BC74 = D_0023BD28->work;
    c->work = D_0023BC74;
    func_00348DB0((u16)D_0023BD28->width, (u16)D_0023BD28->height);
    if (D_003065D5[0] != 2) {
        D_0023BD28 = NULL;
        return;
    }
    D_0048BF9C[0] = 0;
    D_0023BD24 = 0;
    D_003065A5[0] = 1;
    D_0023BD28->vram = 0;
    func_00349700(0, (u16)D_0023BD28->width, (u16)D_0023BD28->height);
    D_0023BC70->ply = mwPlyCreateSofdec(c);
    if (D_0023BC70->ply == NULL) {
        D_0023BD28 = NULL;
        D_003065A5[0] = 0;
        return;
    }
    D_0023BC70->unk91 = 0;
    D_0023BC70->errFrames = 0;
    D_0035D350[0] = 0;
}
#endif

/* Stops the movie (it ends at its end). */
void func_00349DE0(void) {
    D_0023BC70 = &D_003209B0;
    if (D_0023BC70->ply != NULL) {
        D_0023BC70->ply->vtbl->stop(D_0023BC70->ply);
    }
}

/* Stops and destroys the player. */
void func_00349E20(void) {
    D_0023BC70 = &D_003209B0;
    D_0023A148 = -1;
    D_0035D350[0] = 0;
    if (D_0023BC70->ply != NULL) {
        D_0023BC70->ply->vtbl->stop(D_0023BC70->ply);
        D_0023BC70->ply->vtbl->destroy(D_0023BC70->ply);
        D_0023BC70->ply = NULL;
    }
    D_003065A5[0] = 0;
    D_0048BF9C[0] = 1;
    D_003065CC[0] = 0;
    func_00348E60();
    D_0023BD28 = NULL;
}

typedef struct {
    const char *n[5];
} StatNames;
extern StatNames D_00358E80;        /* state names */
extern char D_0035BAC0[], D_0035BAD0[], D_0035BAE0[], D_0035BAF0[], D_0035BAF8[], D_0035BB08[],
    D_0035BB18[], D_0035BB28[], D_0035BB38[];

#ifdef NON_MATCHING   /* first draft */
/* Every frame while a movie plays: show its next frame. */
void func_00349EC0(void) {
    char s[0x100];
    MwsfdPlyInf inf;
    StatNames names;
    MwsfdFrmObj frm;
    s32 time, tunit;
    s32 t;
    s32 line;
    Movie *m;

    names = D_00358E80;
    m = D_0023BC70;
    D_003065CF[0] = 0;
    if (m == NULL || m->ply == NULL) {
        return;
    }
    mwPlyGetPlyInf(D_0023BC70->ply, &inf);
    if (func_001B6980(0x76) != 0) {
        D_0023BC70->ply->vtbl->getTime(D_0023BC70->ply, &time, &tunit);
        t = time / tunit;
    }
    D_0023BC70->stat = D_0023BC70->ply->vtbl->getStat(D_0023BC70->ply);
    switch (D_0023BC70->stat) {
    case 0:
        break;
    case 1:
        D_003065CF[0] = 1;
        D_0023BC70->errFrames++;
        if (D_0023BC70->errFrames >= 300) {
            func_00349E20();
        }
        break;
    case 2:
        mwPlyGetCurFrm(D_0023BC70->ply, &frm);
        func_00349680(frm.bufadr, (u16)D_0023BC70->width, (u16)D_0023BC70->height);
        if (frm.bufadr != 0) {
            D_0023BC70->width = frm.width;
            D_0023BC70->height = frm.height;
            D_0023BD20 = frm.frmno;
            D_0023BC70->frm = frm;
            mwPlyRelCurFrm(D_0023BC70->ply, &frm);
        }
        break;
    case 3:
        func_00349E20();
        break;
    case 4:
        func_00349E20();
        break;
    }
    if (func_001B6980(0xE) != 0) {
        func_00349DE0();
    }
    if (func_001B6980(0x76) != 0) {
        sprintf(s, D_0035BAC0, t);
        func_001B2600(0xA, 2, -1, s);
        sprintf(s, D_0035BAD0, D_0023BD20);
        func_001B2600(0xA, 3, -1, s);
        sprintf(s, D_0035BAE0, names.n[D_0023BC70->stat]);
        line = 5;
        func_001B2600(0xA, 4, -1, s);
        if (D_00306590.unk0 != 0) {
            sprintf(s, D_0035BAF0, D_0035D350, D_0023A148);
            line = 6;
            func_001B2600(0xA, 5, -1, s);
        }
        if (func_001B6980(0x56) != 0) {
            sprintf(s, D_0035BAF8, inf.dropFrm);
            func_001B2600(0xA, line++, -1, s);
            sprintf(s, D_0035BB08, inf.skipDec);
            func_001B2600(0xA, line++, -1, s);
            sprintf(s, D_0035BB18, inf.skipDisp);
            func_001B2600(0xA, line++, -1, s);
            sprintf(s, D_0035BB28, inf.unkC);
            func_001B2600(0xA, line++, -1, s);
            sprintf(s, D_0035BB38, inf.unk10);
            func_001B2600(0xA, line, -1, s);
        }
    }
}
#endif

/* 2 when no movie is playing, else its Sofdec state. */
u8 func_0034A290(void) {
    if (func_001B6980(0xE) != 0) {
        return 2;
    }
    if (D_0023BC70 == NULL) {
        return 2;
    }
    if (D_0023BC70->ply == NULL) {
        return 2;
    }
    return D_0023BC70->stat;
}

extern char D_0035BBC0[], D_0035BBE0[], D_0035BC00[], D_0035BC20[];   /* PS2title1/2_jpn/eng.sfd */

#ifdef NON_MATCHING   /* first draft */
/* The title movie: part 1 once, then part 2 over and over. */
void func_0034A7E0(void) {
    s32 fid1, fid2;

    if (func_001B6980(0xE) == 0) {
        D_0023BC70 = &D_003209B0;
        D_003065CC[0] = 2;
        if (D_0023BC70->ply == NULL) {
            func_003498B0(0x200, 0x1C0);
            if (D_0023BC70->ply == NULL) {
                D_003065CC[0] = 0;
                return;
            }
        }
        if (D_003065CD[0] == 0) {
            fid1 = func_001AA5C0(D_0035BBC0);
            fid2 = func_001AA5C0(D_0035BBE0);
        } else {
            fid1 = func_001AA5C0(D_0035BC00);
            fid2 = func_001AA5C0(D_0035BC20);
        }
        if (fid1 == -1 || fid2 == -1) {
            func_00349E20();
            return;
        }
        mwPlyEntryAfs(D_0023BC70->ply, 3, fid1);
        mwPlyEntryAfs(D_0023BC70->ply, 3, fid2);
        mwPlyEntryAfs(D_0023BC70->ply, 3, fid2);
        mwPlyEntryAfs(D_0023BC70->ply, 3, fid2);
        mwPlyEntryAfs(D_0023BC70->ply, 3, fid2);
        mwPlyEntryAfs(D_0023BC70->ply, 3, fid2);
        mwPlySetLpFlg(D_0023BC70->ply, 0);
        mwPlyStartSeamless(D_0023BC70->ply);
    }
}
#endif

void func_0034A970(void);
#ifdef NON_MATCHING   /* first draft (like func_0034A7E0) */
/* The menu background: part 2 of the title movie, looping. */
void func_0034A970(void) {
    s32 fid;

    if (func_001B6980(0xE) == 0) {
        D_0023BC70 = &D_003209B0;
        D_003065CC[0] = 2;
        if (D_0023BC70->ply == NULL) {
            func_003498B0(0x200, 0x1C0);
            if (D_0023BC70->ply == NULL) {
                D_003065CC[0] = 0;
                return;
            }
        }
        if (D_003065CD[0] == 0) {
            fid = func_001AA5C0(D_0035BBE0);
        } else {
            fid = func_001AA5C0(D_0035BC20);
        }
        if (fid == -1) {
            func_00349E20();
            return;
        }
        mwPlyEntryAfs(D_0023BC70->ply, 3, fid);
        mwPlySetLpFlg(D_0023BC70->ply, 1);
        mwPlyStartSeamless(D_0023BC70->ply);
    }
}
#endif

extern char D_0035BB48[];        /* "PS2select.sfd" */

void func_0034A3D0(void);
#ifdef NON_MATCHING   /* first draft (like func_0034A7E0) */
/* The select screen's background movie, looping. */
void func_0034A3D0(void) {
    s32 fid;

    if (func_001B6980(0xE) == 0) {
        D_0023BC70 = &D_003209B0;
        D_003065CC[0] = 2;
        if (D_0023BC70->ply == NULL) {
            func_003498B0(0x200, 0x1A0);
            if (D_0023BC70->ply == NULL) {
                D_003065CC[0] = 0;
                return;
            }
        }
        fid = func_001AA5C0(D_0035BB48);
        if (fid == -1) {
            func_00349E20();
            return;
        }
        mwPlyStartAfsLp(D_0023BC70->ply, 3, fid);
    }
}
#endif

extern char D_0035BC48[], D_0035BC58[];     /* PS2demo01/02.sfd */

#ifdef NON_MATCHING   /* first draft */
/* An attract-mode movie. */
void func_0034AFB0(s32 which) {
    s32 fid;

    if (func_001B6980(0xE) == 0) {
        D_0023BC70 = &D_003209B0;
        D_003065CC[0] = 2;
        if (D_0023BC70->ply == NULL) {
            func_003498B0(0x200, 0x1C0);
            if (D_0023BC70->ply == NULL) {
                D_003065CC[0] = 0;
                return;
            }
        }
        if (which == 0) {
            fid = func_001AA5C0(D_0035BC48);
        } else {
            fid = func_001AA5C0(D_0035BC58);
        }
        if (fid == -1) {
            func_00349E20();
            return;
        }
        mwPlyStartAfs(D_0023BC70->ply, 3, fid);
    }
}
#endif
