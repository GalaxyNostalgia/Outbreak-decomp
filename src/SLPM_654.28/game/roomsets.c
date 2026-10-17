#include "common.h"

/* GAME.BIN: four room data records of 0x34-byte entries (kind unknown yet), and the tables reset
   with them. */

typedef struct {
    u16 unk0;           /* 0x000 */
    u16 n;              /* 0x002 */
    u8 unk4[0x34];      /* 0x004 */
    u8 unk38[0x38];     /* 0x038 */
    u8 entries[32][0x34]; /* 0x070 */
    u8 pad[4];
} RoomSet;              /* 0x6F4 */

typedef struct {
    s32 unk0;
    s32 n;              /* 0x04 */
    u8 unk8[0x34];      /* 0x08 */
    u8 unk3C[0x38];     /* 0x3C */
    u8 entries[1][0x34]; /* 0x74 */
} RoomSetDef;

typedef struct {
    s32 version;        /* 0x0: 4 */
    s32 n;              /* 0x4 */
    s32 offs[1];        /* 0x8 */
} RoomSetFile;

extern RoomSet D_003AD380[4];
extern s32 D_0023BF58;
extern s32 D_0023BF5C;
extern u8 D_0023BF60;
extern u8 D_0023BF64;
extern s8 D_003062BD[];
extern s8 D_003062BE[];
extern u8 D_00306290[];
extern u8 D_0030629C[];
extern u8 D_003062A4[];
extern u8 D_003062B0[];
extern s32 D_007122A0[32];
extern u8 D_00712320[];
extern u8 D_007123A0[];
extern u8 D_007131A0[];
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *d, const void *s, u32 n);

/* Resets the tables that go with them. */
void func_0067FA80(void) {
    s32 i;

    D_003062BE[0] = -1;
    D_0023BF5C = 0;
    memset(D_00712320, 0, 0x80);
    memset(D_007131A0, 0, 0xE00);
    D_0023BF64 = 0;
    memset(D_007123A0, 0, 0xE00);
    D_003062BD[0] = -1;
    D_0023BF60 = 0;
    memset(D_00306290, 0, 8);
    memset(D_0030629C, 0, 8);
    memset(D_003062B0, 0, 0xC);
    memset(D_003062A4, 0, 0xC);
    for (i = 31; i >= 0; i--) {
        D_007122A0[i] = 0;
    }
    D_0023BF58 = 0;
}

/* Copies the records in f (version 4 only). */
void func_0067FB70(RoomSetFile *f) {
    s32 i;
    s32 j;
    s32 n;
    s32 *off;
    RoomSetDef *d;
    RoomSet *s;

    s = D_003AD380;
    memset(s, 0, sizeof(D_003AD380));
    n = f->n;
    off = f->offs;
    if (f->version == 4) {
        for (i = 0; i < n; i++, s++, off++) {
            d = (RoomSetDef *)((u8 *)f + *off);
            s->unk0 = d->unk0;
            s->n = d->n;
            memcpy(s->unk4, d->unk8, 0x34);
            memcpy(s->unk38, d->unk3C, 0x38);
            for (j = 0; j < s->n; j++) {
                memcpy(s->entries[j], d->entries[j], 0x34);
            }
        }
        func_0067FA80();
    }
}
