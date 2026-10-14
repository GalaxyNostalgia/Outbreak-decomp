#include "common.h"
#include "options.h"
#include "game.h"
#include "model.h"
#include "enemy.h"
#include "thing.h"

/* The room's things: the enemies (and such) its enemy sets place, from the pool D_003FFCA0. */

void *memset(void *p, s32 c, u32 n);

typedef struct { u8 unk0[0xD90]; } ObjD90;
extern ObjD90 D_003FFCA0[0x32];
extern Thing *D_00386AD0[0x32];     /* the free ones */
extern s16 D_0023BDEC;              /* free */
extern Thing *D_0023BDF0;           /* the list's last */
extern Thing *D_0023BDF4;           /* the list */

/* A free thing, put first in the list (last with back). */
Thing *func_00687F20(s32 back) {
    Thing *t;

    if (D_0023BDEC == 0) {
        return NULL;
    }
    D_0023BDEC--;
    t = D_00386AD0[D_0023BDEC];
    memset(t, 0, sizeof(ObjD90));
    t->index = (u32)((u8 *)t - (u8 *)D_003FFCA0) / sizeof(ObjD90);
    if (D_0023BDF4 == NULL) {
        D_0023BDF0 = t;
        D_0023BDF4 = t;
        t->prev = NULL;
        t->next = NULL;
    } else if (back == 0) {
        t->next = D_0023BDF4;
        t->prev = NULL;
        D_0023BDF4->prev = t;
        D_0023BDF4 = t;
    } else {
        t->next = NULL;
        t->prev = D_0023BDF0;
        D_0023BDF0->next = t;
        D_0023BDF0 = t;
    }
    return t;
}

void func_001D5100(u8 n, u8 slot, u8 type, u8 def);
void func_001D3AE0(u8 n, u8 slot, u8 type, u8 def, s32 shared);
void func_001D6700(u8 slot, u8 type, u8 def);

/* Gives a thing's model slot the model, textures and motions of its type's entry n. */
void func_00688390(u8 n, u8 slot, u8 type, u8 def, s32 shared);
#ifdef NON_MATCHING   /* first draft */
void func_00688390(u8 n, u8 slot, u8 type, u8 def, s32 shared) {
    ThingDef *defs = D_00357500[type].defs;

    if (defs[n].tex != 0xFF) {
        func_001D5100(n, slot, type, def);
    }
    if (defs[n].model != 0xFF) {
        func_001D3AE0(n, slot, type, def, shared);
    }
    if (defs[n].unk2 != 0xFF) {
        func_001D6700(slot, type, def);
    }
}
#endif

extern u8 D_00720030[];     /* per scenario: its enemy sets */
extern u8 D_007200B0[];     /* per scenario: the first */
extern EnemySet D_0071A030[];
extern s8 D_0023BF80;
extern s16 D_006D7040[];    /* per type: its update table */
extern void *D_006D7110[];
extern u8 D_00341EB0[];     /* the things' draw function */
extern Slot1468 D_0042A2C0[];
extern u32 D_006D6150[];    /* types whose models are shared */
extern u8 D_0048C030[];     /* per type: its motion buffer */
extern u8 D_0048E620[][0x3EC];
extern EnemyMotions D_0048FDB0[];
s32 func_001B6980(s32 flag);
s32 func_001B2340(EnemySet *e);
s32 func_001B7950(Thing *t);
s8 func_001B7980(Thing *t);
void func_00689980(Thing *t, EnemySet *e);
void func_006895F0(Thing *t);
void func_00578850(Thing *t);
s32 func_00347340(Thing *t, s32 a, u8 type);
void func_00347580(Thing *t, s32 a);
void func_001D6E70(u8 *types);

#define BIT(a, n) ((a)[(n) >> 5] & (1 << ((n) & 0x1F)))

s32 func_001AF4D0(void);

/* Playing offline. */
u8 func_00688C10(void) {
    return func_001AF4D0() == 1;
}

/* Places the room's enemies: the scenario's enemy sets (when their scene flag is on), then the
   ones placed whatever the scenario, of the groups' types; then loads the types' code. */
void func_00688C40(void);
#ifdef NON_MATCHING   /* first draft */
void func_00688C40(void) {
    u8 seen[0x100];
    u8 types[8];
    EnemySet *e;
    Thing *t;
    s8 slot = 0;
    u8 group;
    s8 i;
    s32 j;
    s32 n;
    s32 has;
    s8 k;

    if (D_0048BF60.unk28 != 0) {
        return;
    }
    D_0023BF80 = 0;
    e = &D_0071A030[D_007200B0[D_00306590.unk1C]];
    n = D_00720030[D_00306590.unk1C];
    memset(seen, 0, sizeof(seen));
    group = 0;
    for (i = 0; i < n; i++, e++) {
        e->obj = 0xFF;
        if (!BIT(D_0048AC10.flags, e->flag)) {
            continue;
        }
        if (e->type == 0x3D && (D_0048BF60.unkC6 & 1)) {
            continue;
        }
        if (func_001B6980(0x46) == 0) {
            if (e->type == 0xA && e->unk2F[0] == 0xFF) {
                continue;
            }
            if (e->type == 0x29 && e->unk2F[0] == 0xFF) {
                continue;
            }
            if (e->type == 0x1D && e->unk2F[0] == 0xFF) {
                continue;
            }
            if (e->type == 0x13 && e->unk2F[0] == 0xFF) {
                continue;
            }
        }
        if (e->always != 0) {
            continue;
        }
        t = func_00687F20(1);
        if (t == NULL) {
            return;
        }
        t->active = 1;
        t->type = 2;
        t->kind = e->type;
        e->obj = t->index;
        t->update = D_006D7110[D_006D7040[e->type]];
        t->draw = D_00341EB0;
        t->slot = slot;
        t->model = &D_0042A2C0[slot];
        slot++;
        t->color[0] = 0xFF;
        t->color[1] = 0xFF;
        t->color[2] = 0xFF;
        t->color[3] = 0xFF;
        t->unkCA0 = -1;
        t->unkCA1 = 0xFF;
        func_00689980(t, e);
        func_006895F0(t);
        t->scenario = D_00306590.unk1C;
        if (t->kind == 1 || t->kind == 0x11 || t->kind == 0x25) {
            if (func_001B7950(t) == 0) {
                t->unk554 = group + 2;
                group++;
            } else {
                t->unk554 = group + 0x21;
            }
            t->variant = func_001B7980(t);
            if (t->kind == 1) {
                seen[t->kind] = t->variant;
            }
        } else {
            t->unk554 = 0;
            if (seen[t->kind] == 0) {
                t->variant = func_001B7980(t);
                seen[t->kind] = t->variant;
            } else {
                t->variant = seen[t->kind];
            }
        }
        func_00688390(t->unk54D, t->slot, t->kind, t->def, BIT(D_006D6150, t->kind) != 0);
        t->model->unk1464 = NULL;
        t->model->unk1460 = NULL;
        if (func_00347340(t, 2, t->kind) != 0) {
            func_00347580(t, t->slot + 4);
        }
        for (k = 0; k < t->model->nparts; k++) {
            t->parts[k] = 1;
        }
        t->unk54E = t->kind;
        t->anims = &D_0048FDB0[D_0048C030[t->kind]];
        t->motions = D_0048E620[D_0048C030[t->kind]];
        t->unk524 = t->anims->per;
    }
    for (j = 0, e = D_0071A030; j < D_0048BF60.unkBE; j++, e++) {
        if (func_001B6980(0x46) == 0) {
            if (e->type == 0xA && e->unk2F[0] == 0xFF) {
                continue;
            }
            if (e->type == 0x29 && e->unk2F[0] == 0xFF) {
                continue;
            }
            if (e->type == 0x1D && e->unk2F[0] == 0xFF) {
                continue;
            }
            if (e->type == 0x13 && e->unk2F[0] == 0xFF) {
                continue;
            }
        }
        if (e->always == 0) {
            continue;
        }
        if (e->type != D_0048AC10.type[0] && e->type != D_0048AC10.type[1]) {
            continue;
        }
        e->obj = 0xFF;
        if (!BIT(D_0048AC10.flags, e->flag)) {
            continue;
        }
        if (func_001B2340(e) == 0) {
            continue;
        }
        t = func_00687F20(1);
        if (t == NULL) {
            return;
        }
        t->active = 1;
        t->type = 2;
        t->kind = e->type;
        e->obj = t->index;
        t->update = D_006D7110[D_006D7040[e->type]];
        t->draw = D_00341EB0;
        t->slot = slot;
        t->model = &D_0042A2C0[slot];
        slot++;
        t->color[0] = 0xFF;
        t->color[1] = 0xFF;
        t->color[2] = 0xFF;
        t->color[3] = 0xFF;
        t->unkCA0 = -1;
        t->unkCA1 = 0xFF;
        func_00689980(t, e);
        func_006895F0(t);
        t->scenario = D_00306590.unk1C;
        if (t->kind == 1 || t->kind == 0x11 || t->kind == 0x25) {
            if (func_001B7950(t) == 0) {
                t->unk554 = group + 2;
                group++;
            } else {
                t->unk554 = group + 0x21;
            }
            t->variant = func_001B7980(t);
            if (t->kind == 1) {
                seen[t->kind] = t->variant;
            }
        } else {
            t->unk554 = 0;
            if (seen[t->kind] == 0) {
                t->variant = func_001B7980(t);
                seen[t->kind] = t->variant;
            } else {
                t->variant = seen[t->kind];
            }
        }
        if (e->room == D_00306590.unk1C) {
            e->unk6 = 1;
        } else {
            func_00578850(t);
        }
        func_00688390(t->unk54D, t->slot, t->kind, t->def, BIT(D_006D6150, t->kind) != 0);
        t->model->unk1464 = NULL;
        t->model->unk1460 = NULL;
        if (func_00347340(t, 2, t->kind) != 0) {
            func_00347580(t, t->slot + 4);
        }
        for (k = 0; k < t->model->nparts; k++) {
            t->parts[k] = 1;
        }
        t->unk54E = t->kind;
        t->anims = &D_0048FDB0[D_0048C030[t->kind]];
        t->motions = D_0048E620[D_0048C030[t->kind]];
        t->unk524 = t->anims->per;
    }
    /* Which types' code to load: type 1 by the others with it (2 with 0x59, 3 with other types,
       4 with only 2 or 3). */
    has = 0;
    memset(types, 0, sizeof(types));
    for (j = 0; j < 0x100; j++) {
        switch (seen[j]) {
        case 0:
            break;
        case 0x59:
            has |= 2;
            break;
        case 2:
        case 3:
            has |= 4;
            break;
        default:
            has |= 1;
            break;
        }
    }
    for (k = 0, j = 0; j < 0x100; j++) {
        if (seen[j] != 0) {
            if (j == 1) {
                if (has & 2) {
                    types[k] = 2;
                } else if (has & 1) {
                    types[k] = 3;
                } else {
                    types[k] = 4;
                }
            } else {
                types[k] = seen[j];
            }
            k++;
        }
    }
    func_001D6E70(types);
}
#endif

/* The thing's pattern row (D_006FB4E0[unkBA4][its set's row], 1 with 4 first: 2 from then on). */
extern u8 D_006FB4E0[][8][5];
extern u8 D_006FB508[][5];

#ifdef NON_MATCHING   /* first draft */
void func_006895F0(Thing *t) {
    u8 row = D_0071A030[t->set].unk42;
    s16 i;
    s16 j;

    switch (t->unkBA4) {
    case 0:
        for (i = 0; i < 4; i++) {
            t->unkBB6[i] = D_006FB4E0[0][row][i];
        }
        break;
    case 1:
        if (D_006FB508[row][0] == 4) {
            t->unkBA4 = 2;
            for (j = 1, i = 0; j < 5; j++, i++) {
                t->unkBB6[i] = D_006FB4E0[1][row][j];
            }
        } else {
            for (i = 0; i < 4; i++) {
                t->unkBB6[i] = D_006FB4E0[1][row][i];
            }
        }
        break;
    case 2:
        for (i = 0; i < 4; i++) {
            t->unkBB6[i] = D_006FB4E0[2][row][i];
        }
        break;
    }
}
#endif

extern s32 D_006F6E40[];
extern s32 D_006F6E60[];
extern f32 D_006F6E80[];
extern s16 D_006F78E0[][8];
extern s16 D_006F7960[][8];
extern u8 D_006F7AE0[];
extern u8 D_003E8230[][0x1170];     /* the enemies */
extern u8 D_00476DD0[][0x10E0];     /* the players */
s8 func_006BC9D0(Thing *t);
u16 func_001ADEF0(s32 n);

/* Sets a thing up from its enemy set. */
void func_00689980(Thing *t, EnemySet *e) {
    s16 i;

    t->set = e->index;
    t->hp = e->hp;
    t->maxHp = e->maxHp;
    t->unkBA3 = e->unk20;
    t->unkB9E = e->unk21;
    t->pos[0] = e->pos[0];
    t->pos[1] = e->pos[1];
    t->pos[2] = e->pos[2];
    t->rot[0] = e->rot[0];
    t->rot[1] = e->rot[1];
    t->rot[2] = 0;
    t->shadow = 1;
    t->unkAA8 = 0;
    t->def = e->variant;
    t->unkBA9 = e->flag;
    t->unk1DC = t->unkAEC;
    t->unk54D = 0;
    t->unkBA7 = e->unk29;
    t->unkC2B = e->unk2A;
    t->unkB9C = e->unk2B;
    t->unkB9D = e->unk3A;
    t->unkB9A = e->always;
    t->unkBAA = e->unk2D;
    for (i = 0; i < 10; i++) {
        t->unkBAC[i] = e->unk2F[i];
    }
    t->unkC26 = e->unk7 & 0xF;
    if (e->unk7 & 0x80) {
        t->unkC25 = 1;
        t->target = D_003E8230[t->unkC26 % 10];
    } else {
        t->unkC25 = 0;
        t->target = D_00476DD0[t->unkC26 & 3];
    }
    t->unkBA6 = 0;
    t->unkC2E = 0;
    t->unk199 = func_006BC9D0(t);
    t->scale[0] = t->scale[1] = t->scale[2] = D_00357500[t->kind].defs[t->def].scale;
    if (e->scale[0] != 0) {
        t->scale[0] = e->scale[0] / 100.0f;
    }
    if (e->scale[1] != 0) {
        t->scale[1] = e->scale[1] / 100.0f;
    }
    if (e->scale[2] != 0) {
        t->scale[2] = e->scale[2] / 100.0f;
    }
    t->unk4 = e->unk46;
    t->unk5 = e->unk47;
    t->unkB9F = 0;
    t->unkBA1 = e->unk53;
    t->unkC86 = e->unk50;
    t->unkC87 = e->unk51;
    t->unkC88 = e->unk52;
    t->unkC8C = D_006F6E40[e->unk4C];
    t->unkC90 = D_006F6E60[e->unk4D];
    t->unkBCC = D_006F6E80[e->unk4A];
    t->unkBD0 = D_006F6E80[e->unk4B];
    t->unkC89 = e->unk48;
    t->unkC8A = e->unk49;
    t->unkBDA = D_006F78E0[e->unk54][func_001ADEF0(0) & 7];
    t->unkBDC = D_006F7960[e->unk54][func_001ADEF0(0) & 7];
    t->unkC20 = 1;
    t->unkC21 = 0;
    t->unkC22 = 0;
    t->unkC23 = 0;
    t->unkC24 = D_006F7AE0[t->kind];
    t->unkC27 = e->unk55;
    t->unkC28 = e->unk56;
    t->unkC2A = e->unk4E;
    t->unkC85 = e->unk4F;
    t->unkC84 = 3;
    t->unkBA4 = e->unk5A;
    t->target = D_00476DD0[D_0048BF60.unk1D];
}

/* Some types' things outside their room start differently. */
#ifdef NON_MATCHING   /* first draft */
void func_00578850(Thing *t) {
    if (t == NULL) {
        return;
    }
    t->visible = 0;
    switch (t->kind) {
    case 0x2D:
        t->unkC2B = 1;
        break;
    case 0x2B:
        t->unkC2B = 4;
        break;
    case 0x29:
        t->unkC2B = 2;
        break;
    case 0x21:
        t->unkC2B = 2;
        break;
    case 0x20:
        t->unkC2B = 6;
        break;
    case 0x1B:
        t->unkC2B = 1;
        break;
    case 0x16:
        t->unkC2B = 1;
        break;
    case 1:
        t->unkC2B = 0x11;
        break;
    }
}
#endif

extern s8 D_0023BF7C;
extern u8 D_0023BF84[4];
extern u8 D_0023BD80;
extern s8 D_0023A434;
extern u8 D_00386310[0x100];   /* per enemy set: a timer (0xFF: off) */
void func_0060F2B0(void);
void func_0060F2C0(void);
void func_00577060(EnemySet *e, s16 i, u8 *timer);
void func_0062CB30(Thing *t);
void func_00346B90(Thing *t);

/* Once a frame: the enemy sets' timers (a tick every 5 seconds of play), then each thing's
   update. */
void func_006897A0(void);
#ifdef NON_MATCHING   /* first draft */
void func_006897A0(void) {
    ObjD90 *pool;
    Thing *t;
    s8 tick = 0;
    s16 i;

    func_0060F2B0();
    func_0060F2C0();
    if (D_00306590.unkC != 0) {
        D_0023BF7C = 2;
    } else {
        D_0023BF7C = 1;
    }
    for (i = 0; i < 4; i++) {
        D_0023BF84[i] = 0;
    }
    pool = D_003FFCA0;
    if (D_0048BF60.timeS != D_0023BD80) {
        D_0023A434--;
        D_0023BD80 = D_0048BF60.timeS;
        if (D_0023A434 <= 0) {
            tick = 1;
            D_0023A434 = 5;
        }
    }
    if (pool != NULL) {
        for (i = 0; i < 0x100; i++) {
            u8 *c = &D_00386310[i];

            if (*c != 0xFF && tick) {
                (*c)--;
                if (*c == 0) {
                    *c = 0xFF;
                    func_00577060(&D_0071A030[i], i, c);
                }
            }
        }
    }
    for (t = D_0023BDF4; t != NULL; t = t->next) {
        if (t->active != 0) {
            D_0071A030[t->set].unk45 &= ~0x10;
            ((void (*)(Thing *, u8))t->update)(t, t->set);
            if (!(t->unkC94 & 4)) {
                func_0062CB30(t);
            }
            if (t->unkC8B == 0) {
                t->unkC8B = 1;
                func_00346B90(t);
            }
        }
        t->unkC98 = 0;
    }
}
#endif
