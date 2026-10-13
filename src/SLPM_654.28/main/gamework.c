#include "common.h"
#include "options.h"

/* The game's work memory: one block taken from the heap when a game starts, cut into slots. */

typedef struct {
    u32 mark[2];        /* heap mark the block is taken under */
    s16 count;          /* 0x08: entries of 0x98 bytes at unk10 */
    s16 unkA;
    s16 unkC;
    u8 unkE[2];
    u8 *unk10;          /* 0x10: count * 0x98 bytes */
    u32 unk14;          /* 0x14: the slots below, 0x2B680 bytes */
    u8 unk18[0x33C - 0x18];
} GameWork;

/* 16 slots of 0x1468 bytes, each with a 0x1300-byte buffer after them */
typedef struct {
    u8 *unk0;           /* -> unk820 */
    u8 *unk4;           /* -> unkA20 */
    u8 unk8[0x808 - 8];
    u8 *buf;            /* 0x808 */
    u8 unk80C[0x818 - 0x80C];
    u8 *unk818;         /* -> unkC20 */
    u8 unk81C[4];
    u8 unk820[0x200];
    u8 unkA20[0x200];
    u8 unkC20[0x1468 - 0xC20];
} Slot1468;

typedef struct {
    u8 unk0;
    s8 id;
    u8 unk2[2];
    Slot1468 *slot;
    u8 unk8[4];
} SlotRef;

typedef struct {
    u8 unk0[2];
    s16 unk2[6];
    s8 id;
    u8 unkF[0x14 - 0xF];
} SlotInfo;

typedef struct {
    u8 unk0;
    s8 id;
    u8 unk2[2];
    u8 *buf;            /* 0x1000 bytes */
    u8 unk8[8];
} Buf1000;

extern SlotInfo D_00321450[0x10];
extern SlotRef D_00321390[0x10];
extern Buf1000 D_00321350[4];
extern Slot1468 *D_0023BC84;

#ifdef NON_MATCHING   /* the second loop's registers differ */
void func_001D1750(GameWork *w) {
    s32 i;
    SlotRef *ref;
    SlotInfo *info;
    Slot1468 *slot;
    u8 *buf;
    s32 j;
    Buf1000 *b;

    memset(D_00321450, 0, sizeof(D_00321450));
    memset(D_00321390, 0, sizeof(D_00321390));
    ref = D_00321390;
    info = D_00321450;
    i = 0;
    D_0023BC84 = (Slot1468 *)w->unk14;
    slot = D_0023BC84;
    buf = (u8 *)(w->unk14 + 0x14680);
    do {
        memset(ref, 0, sizeof(SlotRef));
        memset(slot, 0, sizeof(Slot1468));
        slot->unk0 = slot->unk820;
        slot->unk4 = slot->unkA20;
        slot->unk818 = slot->unkC20;
        slot->buf = buf;
        ref->id = i;
        buf += 0x1300;
        ref->slot = slot;
        info->id = i;
        for (j = 0; j < 6; j++) {
            info->unk2[j] = -1;
        }
        i++;
        ref++;
        slot++;
        info++;
    } while (i < 0x10);
    memset(D_00321350, 0, sizeof(D_00321350));
    b = D_00321350;
    i = 0;
    buf = (u8 *)(w->unk14 + 0x27680);
    do {
        b->buf = buf;
        memset(b->buf, 0, 0x1000);
        b->id = i;
        buf += 0x1000;
        i++;
        b++;
    } while (i < 4);
}
#endif

extern GameWork D_0029B140;
void func_001AE9E0(u32 *mark);
u32 func_001AE950(u32 size);
void func_001AEA90(u32 *mark);

/* Takes the game's work memory (again), with room for count entries. */
void func_001B1910(s32 count) {
    GameWork *w;
    u8 *p;
    s32 size;
    s32 n;

    w = &D_0029B140;
    if (D_00306590.unk10[3] != 0) {
        func_001AEA90(w->mark);
    }
    memset(w, 0, sizeof(GameWork));
    func_001AE9E0(w->mark);
    n = count * 0x98;
    size = n + 0x2B680;
    p = (u8 *)func_001AE950(size);
    memset(p, 0, size);
    w->unk10 = p;
    w->unk14 = (u32)(p + n);
    w->count = count;
    w->unkA = 0;
    w->unkC = 0;
    func_001D1750(w);
    D_00306590.unk10[3] = 1;
}
