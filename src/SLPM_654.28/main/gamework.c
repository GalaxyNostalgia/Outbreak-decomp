#include "common.h"
#include "options.h"
#include "model.h"



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
    w->used = 0;
    w->nbufs = 0;
    func_001D1750(w);
    D_00306590.unk10[3] = 1;
}

void func_001B2870(const char *fmt, ...);     /* debug print, empty */
extern char D_00231C80[];
extern char D_00231CA0[];

/* A free part buffer of count entries handed out before (to owner). */
u8 *func_001B1A40(s16 count, u8 owner) {
    GameWork *w = &D_0029B140;
    s32 i;

    if (D_0029B140.nbufs == 0) {
        return NULL;
    }
    for (i = 0; i < w->nbufs; i++) {
        if (w->owner[i] == 0 && w->counts[i] == count) {
            w->owner[i] = owner + 1;
            return w->bufs[i];
        }
    }
    return NULL;
}

u8 *func_001B1AD0(s32 count, s32 owner);
#ifdef NON_MATCHING   /* registers/scheduling of the stores */
/* A part buffer of count entries for owner. */
u8 *func_001B1AD0(s32 count, s32 owner) {
    GameWork *w;
    u8 *p;
    s32 n;

    if (D_00306590.unk10[3] == 0) {
        func_001B2870(D_00231C80);
    }
    if ((p = func_001B1A40(count, owner)) != NULL) {
        return p;
    }
    w = &D_0029B140;
    if (D_0029B140.nbufs == 100) {
        func_001B2870(D_00231CA0);
    }
    if (w->count - w->used < count) {
        func_001B2870(D_00231CA0);
    }
    n = w->nbufs;
    p = w->unk10 + w->used * 0x98;
    w->bufs[n] = p;
    w->counts[n] = count;
    w->owner[n] = (u8)owner + 1;
    w->used += (s16)count;
    w->nbufs++;
    return p;
}
#endif

extern s16 D_0048B9B8[];
extern s16 D_0048B9BA[];

void func_001B1D40(void) {
    D_0048B9B8[0] = 0;
    D_0048B9BA[0] = 0;
}
