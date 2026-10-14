#include "common.h"
#include "options.h"
#include "model.h"
#include "enemy.h"



void func_001D1750(GameWork *w);

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

extern u16 D_0048B9B8[];    /* the player motion set loaded */
extern s16 D_0048B9BA[];

void func_001B1D40(void) {
    D_0048B9B8[0] = 0;
    D_0048B9BA[0] = 0;
}

extern u16 D_0048B5B0[];    /* per scenario: its player motion set */
void func_001B0EC0(void);
void func_001D4CD0(u16 id);

/* (empty) */
void func_001B1D30(Slot1468 *m) {
}

/* The scenario's player motions, when they differ from the ones loaded. */
void func_001B1D60(void) {
    u16 *p;
    u16 cur = D_0048B9B8[0];
    u16 n = D_0048B5B0[D_00306590.unk1C];
    if (n != cur) {
        if (cur) {
            func_001B0EC0();
        }
        p = &D_0048B5B0[D_00306590.unk1C];
        D_0048B9B8[0] = *p;
        func_001D4CD0(*p);
    }
}

/* The enemy kinds (and their models) the room's enemy sets have now, for the two enemy groups:
   the lists before are kept in prev*, and D_0023BD84/5 say the group's table changed. */
#include "game.h"

extern u16 D_0048B7B0[];
extern u16 D_0048B8B0[];
extern u16 D_0048B9B0[];
extern u16 D_0048B9B2[];
extern u16 D_0048B9BC[];
extern u16 D_0048B9BE[];
extern s8 D_0023BD84;
extern s8 D_0023BD85;
extern u32 D_006D6150[];
extern EnemySet D_0071A030[];

#define BIT(a, n) ((1 << ((n) & 0x1F)) & (a)[(n) >> 5])

void func_001B1E70(void);
#ifdef NON_MATCHING   /* two unrolled passes in the original */
void func_001B1E70(void) {
    u16 id = D_00306590.unk1C;
    u16 cur;
    u16 model;
    EnemySet *e;
    s32 nk;
    s32 nm;
    s32 n = D_0048BF60.unkBE;
    s32 i;
    s32 j;
    s32 found;
    s32 g;

    for (g = 0; g < 2; g++) {
        cur = g == 0 ? D_0048B7B0[id] : D_0048B8B0[id];
        if (cur != (g == 0 ? D_0048B9BC[0] : D_0048B9BE[0])) {
            if (g == 0) {
                D_0023BD84 = 1;
            } else {
                D_0023BD85 = 1;
            }
        }
        for (i = 0x1F; i >= 0; i--) {
            D_0048A810.prevKinds[g][i] = D_0048A810.kinds[g][i];
            D_0048A810.prevModels[g][i] = D_0048A810.models[g][i];
            D_0048A810.kinds[g][i] = -1;
            D_0048A810.models[g][i] = -1;
        }
        nk = 0;
        nm = 0;
        for (i = 0, e = D_0071A030; i < n; i++, e++) {
            if (!BIT(D_006D6150, e->type) || e->type != (g == 0 ? D_0048B9B0[0] : D_0048B9B2[0])) {
                continue;
            }
            if (g == 0) {
                if (D_0048AC10.areaA[e->room] != D_0048AC10.areaA[id]) {
                    continue;
                }
            } else if (D_0048AC10.areaB[e->room] != D_0048AC10.areaB[id]) {
                continue;
            }
            if (e->always == 0 && e->room != id) {
                continue;
            }
            if (!BIT(D_0048AC10.flags, e->flag)) {
                continue;
            }
            found = 0;
            for (j = 0; j < nk; j++) {
                if (D_0048A810.kinds[g][j] == e->variant) {
                    found = 1;
                }
            }
            if (found == 0) {
                D_0048A810.kinds[g][nk++] = e->variant;
            }
            model = g == 0 ? D_0048AC10.modelA[D_0048AC10.areaA[id]]
                           : D_0048AC10.modelB[D_0048AC10.areaB[id]];
            found = 0;
            for (j = 0; j < nm; j++) {
                if (D_0048A810.models[g][j] == model) {
                    found = 1;
                }
            }
            if (found == 0) {
                D_0048A810.models[g][nm++] = model;
            }
        }
        if (g == 0) {
            D_0048B9BC[0] = cur;
        } else {
            D_0048B9BE[0] = cur;
        }
    }
}
#endif

/* Whether an enemy set of a group's type is in the same area (by the group's table) as us. */
s32 func_001B2340(EnemySet *e);
#ifdef NON_MATCHING   /* first draft */
s32 func_001B2340(EnemySet *e) {
    if (e->type == D_0048AC10.type[0] &&
        D_0048AC10.areaA[D_00306590.unk1C] == D_0048AC10.areaA[e->room]) {
        return 1;
    }
    if (e->type == D_0048AC10.type[1]) {
        if (D_0048AC10.areaB[D_00306590.unk1C] == D_0048AC10.areaB[e->room]) {
            return 1;
        }
        return 0;
    }
    return 0;
}
#endif
