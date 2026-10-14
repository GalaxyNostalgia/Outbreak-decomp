#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"
#include "enemy.h"
#include "model.h"

/* The room's NPCs (survivors and such): 0x1170-byte objects from the pool D_003E8230, placed
   from the 64 NPC records D_003614F0. */

void *memset(void *p, s32 c, u32 n);

typedef struct {
    u8 active;          /* 0x00 */
    u8 scenario;        /* 0x01 */
    u8 unk2[2];
    u16 model;          /* 0x04: its NPC model (D_006E2490) */
    u8 mark;            /* 0x06: put a map mark on it */
    u8 flags;           /* 0x07: 1 placed, 8 ..., 0x10 ... */
    f32 pos[3];         /* 0x08 */
    u16 heading;        /* 0x14 */
    s16 hp;             /* 0x16 */
    u8 unk18[0x1E - 0x18];
    u8 state;           /* 0x1E: 1, 2 ... */
    s8 only;            /* 0x1F: only in this random variant (+1; 0: always) */
    u8 unk20;
    u8 unk21;           /* 0x21 */
    u16 flags2;         /* 0x22: 0x10 ..., 0x20 ... */
    s8 unk24;           /* 0x24 */
    u8 unk25[3];
    s32 unk28;          /* 0x28 */
    u8 unk2C[4];
    s32 unk30;          /* 0x30 */
    s8 variant;         /* 0x34 */
    u8 unk35[3];
    s32 unk38[3];       /* 0x38 */
    u8 unk44[4];
    u8 unk48[4];        /* 0x48 */
    u8 unk4C[4];        /* 0x4C */
    u8 item;            /* 0x50: the item it holds */
    u8 unk51[0x78 - 0x51];
} NpcRecord;            /* 0x78 */

extern NpcRecord D_003614F0[0x40];

typedef struct { u8 unk0[0x1170]; } Obj1170;
extern Obj1170 D_003E8230[10];
extern Npc *D_00386BA0[10];         /* the free ones */
extern s16 D_0023BDFC;              /* free */
extern Npc *D_0023BE00;             /* the list's last */
extern Npc *D_0023BE04;             /* the list */

/* A free NPC (keeping its kind), put first in the list (last with back). */
Npc *func_006510E0(s32 back) {
    Npc *n;
    u8 kind;

    if (D_0023BDFC == 0) {
        return NULL;
    }
    D_0023BDFC--;
    n = D_00386BA0[D_0023BDFC];
    kind = n->kind;
    memset(n, 0, sizeof(Obj1170));
    n->kind = kind;
    n->unk6 = (u32)((u8 *)n - (u8 *)D_003E8230) / sizeof(Obj1170);
    if (D_0023BE04 == NULL) {
        D_0023BE00 = n;
        D_0023BE04 = n;
        n->prev = NULL;
        n->next = NULL;
    } else if (back == 0) {
        n->next = D_0023BE04;
        n->prev = NULL;
        D_0023BE04->prev = n;
        D_0023BE04 = n;
    } else {
        n->next = NULL;
        n->prev = D_0023BE00;
        D_0023BE00->next = n;
        D_0023BE00 = n;
    }
    return n;
}

typedef struct {
    u8 unk0[2];
    u8 shared;          /* 0x02: uses the shared motions */
    u8 unk3;            /* 0x03 */
    u8 unk4;            /* 0x04: D_006E2480 */
    u8 unk5;
    s16 hp;             /* 0x06 */
    f32 scale;          /* 0x08 */
    f32 unkC;           /* 0x0C */
    u8 unk10[4];
} NpcModelInfo;         /* 0x14 */

extern NpcModelInfo D_006E2490[];
extern s32 D_006E2480[];
extern s32 D_006E7C30;
extern f32 **D_006E7C50;
extern s32 D_0023AE88;
extern u8 D_00341EB0[];
extern u8 D_003F3090[][0x1468];     /* the NPC models */
extern u8 D_004BBA40[];
extern u8 D_004B7BA0[];
extern u8 D_004B9AF0[];
extern u8 D_004948D0[];
void func_006399A0(Npc *n);
void func_005BB7C0(u8 slot, u8 model, u8 a, u8 b);
void func_0062E0F0(void *o);
s8 func_006BC9D0(void *o);

/* A new NPC of model. */
Npc *func_00651810(u16 model);
#ifdef NON_MATCHING   /* first draft */
Npc *func_00651810(u16 model) {
    Npc *n = func_006510E0(1);
    NpcModelInfo *info;
    f32 *m;
    u8 i;

    if (n == NULL) {
        return NULL;
    }
    info = &D_006E2490[model];
    n->active = 1;
    n->unkB98 = info->shared != 0 ? 5 : 3;
    n->unkBB4 = model;
    n->unk4[0] = info->shared != 0 ? 5 : 3;
    n->type = 1;
    n->update = func_006399A0;
    n->draw = (void *)D_00341EB0;
    n->model = (ObjModel *)D_003F3090[n->kind];
    n->unk54D = 0;
    n->held = 0xFF;
    func_005BB7C0(n->kind, n->unkBB4, n->unk54D, n->held);
    n->unkB99 = info->shared;
    n->unkB9A = info->unk3;
    n->unk544 = info->hp;
    n->unk546 = n->unk544;
    func_0062E0F0(n);
    n->unkB9C = n->unkB9B;
    n->unkAA8 = info->unkC;
    n->unkBC4 = 1.0f;
    n->unkBC0 = 1.0f;
    n->unk199 = func_006BC9D0(n);
    n->unk38[2] = 0.0f;
    n->unk38[1] = 0.0f;
    n->unk38[0] = 0.0f;
    n->unk94 = 0;
    n->unk92 = 0;
    n->unk90 = 0;
    n->unkB4[2] = n->unkB4[1] = n->unkB4[0] = info->scale;
    n->unkD8[2] = 0;
    n->unkD8[1] = 0;
    n->unkD8[0] = 0;
    n->unkE4[2] = 0;
    n->unkE4[1] = 0;
    n->unkE4[0] = 0;
    n->unkF0 = 1.0f;
    n->unk1A8 = D_006E7C30;
    m = *D_006E7C50;
    n->unk1AC[0] = m[0];
    n->unk1AC[1] = m[1];
    n->unk1B4[0] = m[2];
    n->unk1B4[1] = m[3];
    n->unk1BC[0] = n->unk1AC[0];
    n->unk1BC[1] = n->unk1AC[1];
    n->unk1C4[0] = n->unk1B4[0];
    n->unk1C4[1] = n->unk1B4[1];
    n->unkCB8 = (s32)*D_006E7C50;
    n->unkAB0 = D_0023AE88;
    n->unk1D8 = D_006E2480[info->unk4];
    n->unk1DC = n->unkAEC;
    n->unkBA4 |= 2;
    n->motions[0] = D_004BBA40;
    n->motions[1] = n->unkB99 != 0 ? D_004B7BA0 : D_004B9AF0;
    n->motions[2] = D_004948D0;
    n->motions2[0] = D_004948D0;
    n->motions2[1] = n->unkB99 != 0 ? D_004B7BA0 : D_004B9AF0;
    n->motions2[2] = n->motions2[1];
    n->unk524 = 5;
    for (i = 0; i < ((Slot1468 *)n->model)->nparts; i++) {
        n->unkF4[i] = 1;
    }
    if (n->unk54D != 1) {
        n->unkF4[1] = 0;
    }
    n->unk1E2 = D_00306590.unk1C;
    n->unkFD7 = 0;
    n->unkBB0 = 0;
    n->unkBAC = 0;
    n->unkBB6 = 0;
    n->unkBB8 = 0;
    n->unkCE8 = -1;
    return n;
}
#endif

/* Puts item slot in an NPC's hand. */
extern u8 D_003967D2[][0x3C];
extern u8 D_003967F8[][0x3C];
s32 func_00583EA0(s16 id, s32 a);
void *func_005813A0(u8 slot, s32 owner);
s16 func_005C1240(void *o);

void func_00635390(DrawObjFull *o, u8 slot);
#ifdef NON_MATCHING   /* first draft */
void func_00635390(DrawObjFull *o, u8 slot) {
    s32 owner;

    if (slot == 0 || func_00583EA0(*(s16 *)D_003967D2[slot], 1) == 0) {
        return;
    }
    o->unkC7C = slot;
    if (o->type == 0) {
        owner = o->kind + 1;
    } else {
        owner = o->unkBB4 + 5;
    }
    *(s16 *)D_003967F8[o->unkC7C] = owner;
    func_005813A0(o->unkC7C, owner);
    o->unkCA8 = func_005C1240(o);
}
#endif

void func_005E91A0(Npc *n) {
    s16 i;
    s16 j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            n->unkE58[i][j][0] = 0;
            n->unkE58[i][j][1] = 0;
            n->unkE58[i][j][2] = 0;
        }
    }
}

void func_0064FB70(s32 type, u8 id, u8 *b, u8 player);   /* (marks.c: s32s) */

void func_005F4B30(u8 slot) {
    u8 b[4];

    b[0] = 0;
    b[1] = 0;
    b[2] = 0;
    b[3] = 0;
    func_0064FB70(1, slot, b, D_0048BF60.unk1D);
}

/* Whether NPC record i's model is one the scenario loads (1) or not (-1). */
s32 func_00652AC0(s32 i, s32 scenario);
#ifdef NON_MATCHING   /* first draft */
s32 func_00652AC0(s32 i, s32 scenario) {
    NpcRecord *r = &D_003614F0[i];
    NpcModel *m = D_0048A010[scenario];
    s32 k;

    for (k = 0; k < 4; k++) {
        if (r->model == m[k].model) {
            return 1;
        }
    }
    return -1;
}
#endif

void func_0064F690(u8 type, s16 id);
void func_0018D630(void *dst, const void *src, s32 n);
void func_001D6E20(s32 a);
s32 func_00347340(void *o, s32 a, s32 b);
void func_00692080(Npc *n);
void func_00689E50(Npc *n);
void func_0068B630(Npc *n);
void func_006BA8D0(Npc *n);
void func_006A4700(Npc *n);
void func_00692D80(Npc *n);
void func_00690670(Npc *n);
void func_0068FB70(Npc *n);

/* Places the scenario's NPCs from their records. */
s32 func_00651EF0(void);
#ifdef NON_MATCHING   /* first draft */
s32 func_00651EF0(void) {
    NpcRecord *r;
    Npc *n;
    s32 i;
    s32 k;

    for (r = D_003614F0, i = 0, k = 0; i < 0x40; i++, r++) {
        if (r->active == 0 || !(r->flags & 1) || func_00652AC0(i, D_00306590.unk1C) < 0) {
            continue;
        }
        n = func_00651810(r->model);
        if (r->model == 0x64) {
            if (D_00306590.unk1A != 1 && r->mark != 0) {
                func_0064F690(1, i);
            }
        } else if (r->mark != 0) {
            func_0064F690(1, i);
        }
        n->slot = i;
        n->unk38[0] = r->pos[0];
        n->unk38[1] = r->pos[1];
        n->unk38[2] = r->pos[2];
        n->unk92 = r->heading;
        n->unk544 = r->hp;
        *(s8 *)&n->unk31[2] = r->variant;
        func_0062E0F0(n);
        n->unkB9C = n->unkB9B;
        n->unkFB7 = 0x10;
        func_0018D630(n->unkC84, r->unk4C, 4);
        func_00635390((DrawObjFull *)n, r->item);
        n->unk1162 = 0;
        if (r->flags2 & 0x10) {
            r->flags |= 0x10;
        }
        if (r->flags & 8) {
            n->unkBA0 |= 0x1000000;
        }
        if (r->unk30 < 0) {
            n->unkFCF = 0;
        } else {
            n->unkFCF = r->unk30;
        }
        n->unk1150 = r->unk28;
        n->unk1E2 = r->scenario;
        n->unk102C = 0;
        n->unkFD6 = 0xFF;
        n->unk1040[0] = r->unk38[0];
        n->unk1040[1] = r->unk38[1];
        n->unk1040[2] = r->unk38[2];
        n->unk1134[0] = r->unk48[0];
        n->unk1134[1] = r->unk48[1];
        n->unk1134[2] = r->unk48[2];
        n->unk1134[3] = r->unk48[3];
        n->unk1020 = r->unk24;
        n->unk1154 = r->unk21;
        if (n->unk1E2 == D_00306590.unk1C) {
            n->visible = 1;
        } else {
            n->visible = 0;
        }
        switch (n->unkBB4) {
        case 0xC:
            n->update = func_00692080;
            break;
        case 0x1D:
            n->update = func_00689E50;
            break;
        case 0x64:
            n->update = func_0068B630;
            if (r->unk38[0] == 9) {
                n->unk1150 = 0x28;
            } else if (r->unk38[0] == 6) {
                n->unk1150 = 0x14;
            } else {
                n->unk1150 = 0;
            }
            break;
        case 0x6A:
            n->update = func_0068B630;
            break;
        case 0x65:
            n->update = func_006BA8D0;
            break;
        case 0x67:
            n->update = func_006A4700;
            break;
        case 0x26:
        case 0x79:
        case 0x7C:
        case 0x84:
            n->update = func_00692D80;
            break;
        case 0xCB:
            n->update = func_00690670;
            break;
        case 0xCE:
            n->update = func_0068FB70;
            break;
        default:
            n->update = NULL;
            break;
        }
        n->unkFD0[0] = 0;
        n->unkFD0[1] = 0;
        n->unkFD0[2] = 0;
        n->unkFD0[3] = 0;
        if (n->unk544 > 0) {
            if (r->state == 2) {
                n->unk1154 = r->unk21;
                n->visible = 0;
                n->unkFCF = 5;
            } else if (r->state == 1) {
                n->visible = 0;
                n->unkFD0[0] = 0xA;
            }
        } else if (r->flags2 & 0x20) {
            r->unk30 = 4;
            n->unkFCF = 4;
        } else {
            n->unkFCF = 2;
        }
        n->unkFB8[0] = 0;
        n->unkFB8[1] = 0;
        n->unkFB8[2] = 0;
        n->unkFB8[3] = 0;
        func_005E91A0(n);
        func_005F4B30(n->slot);
        func_00347340(n, 0, k);
        k++;
    }
    func_001D6E20(0);
    return 0;
}
#endif

extern DrawObjFull D_00476DD0[];
void func_005516D0(Npc *n);
void func_0062D2C0(Npc *n);
void func_00634670(Npc *n);
void func_005E9390(Npc *n);

/* The NPCs in our scenario when the room starts. */
void func_00651660(void) {
    Npc *n = D_0023BE04;

    while (n != NULL) {
        if (n->active == 0) {
            continue;
        }
        if (n->unk1E2 != D_00476DD0[D_0048BF60.unk1D].unk1E2) {
            n = n->next;
            continue;
        }
        func_005516D0(n);
        n->unkBA4 &= ~0x80;
        if (n->unkBA0 & 0x80000000) {
            func_0062D2C0(n);
        }
        func_00634670(n);
        func_005E9390(n);
        n = n->next;
    }
}

void func_005E9220(Npc *n);
void func_00346B90(void *o);
void func_00652910(Npc *n);
void func_005ABBE0(f32 r, f32 *pos, void *o, s32 a);
s32 func_0058DC10(Npc *n);
void func_0062CB30(void *o);
void func_0062CA30(void *o);
void func_0062CD00(void *o);
void func_0062CC50(void *o);
s32 func_0067DA50(s8 *area, f32 *pos);
void func_0067D7C0(s8 area, s8 prev);
f32 func_0067DAF0(u8 area);

/* Once a frame: the NPCs (only those in our scenario do anything). */
void func_00651340(void);
#ifdef NON_MATCHING   /* first draft */
void func_00651340(void) {
    NpcRecord *r;
    Npc *n = D_0023BE04;
    s8 area;

    while (n != NULL) {
        if (n->active == 0) {
            continue;
        }
        if (n->unk1E2 != D_00476DD0[D_0048BF60.unk1D].unk1E2) {
            n->visible = 0;
            n = n->next;
            continue;
        }
        func_005E9220(n);
        FIELD(n, s32, 0x190) = 0x1FFFF;
        FIELD(n, s32, 0x194) = 0;
        FIELD(n, s8, 0xC82) = 0;
        FIELD(n, s16, 0xD92) = 0;
        FIELD(n, s8, 0xB9E) = 0;
        n->unk54A = 0;
        FIELD(n, s16, 0xBDC) = 0;
        FIELD(n, s16, 0xBDE) = 0;
        n->unkBA0 &= 0xDDBDFFFF;
        n->unkBA4 &= ~0x40;
        n->unk553 = 1;
        if (n->unk31[1] == 0xFF) {
            n->unk31[1] = 0;
        }
        r = &D_003614F0[n->slot];
        if (!(r->flags2 & 1)) {
            n->unk54A |= 1;
        }
        if (!(r->flags2 & 2)) {
            n->unk54A |= 4;
        }
        if (r->flags2 & 8) {
            n->unkBA0 |= 0x400000;
        }
        if (r->flags2 & 0x10) {
            n->unk31[1] = 0xFF;
        }
        FIELD(n, f32, 0x44) = n->unk38[0];
        FIELD(n, f32, 0x48) = n->unk38[1];
        FIELD(n, f32, 0x4C) = n->unk38[2];
        FIELD(n, s16, 0xA0) = n->unk90;
        FIELD(n, s16, 0xA2) = n->unk92;
        FIELD(n, s16, 0xA4) = n->unk94;
        FIELD(n, s16, 0xA6) = FIELD(n, s16, 0x96);
        func_00346B90(n);
        if (n->update != NULL) {
            ((void (*)(Npc *))n->update)(n);
        } else {
            func_00652910(n);
        }
        func_005ABBE0(50.0f, n->unk38, n, 1);
        if (func_0058DC10(n) != 0) {
            func_0062CB30(n);
            n->unk54A |= 4;
        } else {
            func_0062CA30(n);
            func_0062CB30(n);
        }
        func_0062CD00(n);
        func_0062CC50(n);
        if (n->unkBA4 & 0x08000000) {
            n->unkBA4 &= ~0x08000000;
            func_00346B90(n);
        }
        if (func_0067DA50(&area, n->unk38) != 0) {
            n->unkBA0 |= 0x4000;
            if (FIELD(n, u8, 0xCB0) != area) {
                func_0067D7C0(area, FIELD(n, s8, 0xCB0));
                FIELD(n, u8, 0xCB0) = area;
            }
            FIELD(n, f32, 0x170) += func_0067DAF0(FIELD(n, u8, 0xCB0));
            n->unk54A |= 1;
        } else {
            if ((n->unkBA0 & 0x4000) && !(n->unk54A & 2)) {
                func_0067D7C0(-1, FIELD(n, s8, 0xCB0));
                FIELD(n, u8, 0xCB0) = 0;
            }
            n->unkBA0 &= ~0x4000;
        }
        FIELD(n, s8, 0x102A) = 0;
        FIELD(n, s8, 0x1112) = 0;
        FIELD(n, s8, 0x1113) = 0;
        FIELD(n, s8, 0xC53) = 0;
        n = n->next;
    }
}
#endif

#include "item.h"

/* Script op: the NPCs of model args[0] hold item args[1] (if they carry it). */
s32 func_0065C020(u32 *args);
#ifdef NON_MATCHING   /* registers */
s32 func_0065C020(u32 *args) {
    s8 model = *(s8 *)&args[0];
    s16 id = *(s16 *)&args[1];
    NpcRecord *r;
    s32 i;
    s32 k;
    u8 s;

    if (model < 0 || id < 0) {
        return 0;
    }
    for (r = D_003614F0, i = 0; i < 0x40; i++, r++) {
        if (r->active != 0 && r->model == model) {
            for (k = 0; k < 4; k++) {
                s = r->unk4C[k];
                if (s != 0 && id == D_003967D0[s].unk6) {
                    r->item = s;
                    return 0;
                }
            }
        }
    }
    return 0;
}
#endif

/* The (first unplaced) NPC record of a model: its hp (if >= 0) and its flags (top bits: 0x80000000
   clear, 0x40000000 set, 0x20000000 assign the low byte); flags2 follows flags 0x10 and 0x40. */
s32 func_00652610(u16 model, s16 hp, u32 flags);
#ifdef NON_MATCHING   /* first draft */
s32 func_00652610(u16 model, s16 hp, u32 flags) {
    u8 i;
    NpcRecord *r;

    for (i = 0; i < 0x40; i++) {
        r = &D_003614F0[i];
        if (model == r->model && !(r->flags & 1)) {
            if (hp >= 0) {
                D_003614F0[i].hp = hp;
            }
            if (flags & 0x80000000) {
                D_003614F0[i].flags &= ~(flags & 0xFFFFFFF);
            } else if (flags & 0x40000000) {
                D_003614F0[i].flags |= flags;
            } else if (flags & 0x20000000) {
                D_003614F0[i].flags = flags;
            }
            if (D_003614F0[i].flags & 0x10) {
                D_003614F0[i].flags2 |= 0x10;
            } else {
                D_003614F0[i].flags2 &= ~0x10;
            }
            D_003614F0[i].state = 0;
            D_003614F0[i].unk28 = 0;
            if (D_003614F0[i].flags & 0x40) {
                D_003614F0[i].flags2 |= 0x20;
            } else {
                D_003614F0[i].flags2 &= ~0x20;
            }
            return 0;
        }
    }
    return 0;
}
#endif

u16 func_001ADEF0(s32 n);

void func_00652360(void) {
}

/* The NPC of a model (in this variant) is placed at (x, y, z); its map mark shown. */
s32 func_00652370(u16 model, s32 x, s32 y, s32 z);
#ifdef NON_MATCHING   /* first draft */
s32 func_00652370(u16 model, s32 x, s32 y, s32 z) {
    u8 i;
    NpcRecord *r;
    s32 ok;

    for (i = 0; i < 0x40; i++) {
        r = &D_003614F0[i];
        ok = 1;
        if (r->only != 0 && r->only - 1 != (func_001ADEF0(2) & 3)) {
            ok = 0;
        }
        if (model == r->model && ok) {
            D_003614F0[i].flags |= 1;
            D_003614F0[i].unk38[0] = x;
            D_003614F0[i].unk38[1] = y;
            D_003614F0[i].unk38[2] = z;
            D_003614F0[i].state = 0;
            D_003614F0[i].unk28 = 0;
            if (D_003614F0[i].model == 100) {
                if (D_00306590.unk1A != 1 && D_003614F0[i].mark != 0) {
                    func_0064F690(1, i);
                }
            } else if (D_003614F0[i].mark != 0) {
                func_0064F690(1, i);
            }
            return 0;
        }
    }
    return 0;
}
#endif

/* Where an unplaced NPC of a model will be: its scenario, position and heading. */
s32 func_00652540(u16 model, s8 scenario, f32 *pos, u16 heading);
#ifdef NON_MATCHING   /* first draft */
s32 func_00652540(u16 model, s8 scenario, f32 *pos, u16 heading) {
    u8 i;

    for (i = 0; i < 0x40; i++) {
        if (model == D_003614F0[i].model && !(D_003614F0[i].flags & 1)) {
            D_003614F0[i].scenario = scenario;
            D_003614F0[i].pos[0] = pos[0];
            D_003614F0[i].pos[1] = pos[1];
            D_003614F0[i].pos[2] = pos[2];
            D_003614F0[i].heading = heading;
            D_003614F0[i].state = 0;
            D_003614F0[i].unk28 = 0;
            return 0;
        }
    }
    return 0;
}
#endif

/* The NPC of a model is no longer placed. */
s32 func_006527B0(u16 model);
#ifdef NON_MATCHING   /* first draft */
s32 func_006527B0(u16 model) {
    u8 i;

    for (i = 0; i < 0x40; i++) {
        if (model == D_003614F0[i].model) {
            D_003614F0[i].flags &= ~1;
            return 0;
        }
    }
    return 0;
}
#endif
