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
s32 func_001AF510(void);
s32 func_0064FD50(s32 a, s32 slot);
void func_00632180(Npc *n);

/* An NPC being pushed (0xD80 a frame for 0xD90 frames, ending at 0xD74): 2 while it lasts, 1
   on its last frame, 0 when there's none (or it can't be pushed now, which cancels it). */
s32 func_0058DC10(Npc *n) {
    f32 *vy;
    f32 *vz;

    if (!func_001AF510()) {
        return 0;
    }
    if (n->type != 1) {
        return 0;
    }
    if (func_0064FD50(1, n->slot) || (n->unk54A & 0x4000)) {
        n->unkD90 = 0;
        n->unkD80[0] = 0.0f;
        n->unkD80[1] = 0.0f;
        n->unkD80[2] = 0.0f;
        n->unkD8E = 0;
        return 0;
    }
    func_00632180(n);
    vy = &n->unkD80[1];
    vz = &n->unkD80[2];
    n->unkD80[1] = 0.0f;
    n->unk38[0] += n->unkD80[0];
    n->unk38[1] += *vy;
    n->unk38[2] += *vz;
    if (n->unkD90 == 0) {
        return 0;
    }
    if (--n->unkD90 != 0) {
        n->unk54A |= 1;
        return 2;
    }
    *(Vec3 *)n->unk38 = *(Vec3 *)n->unkD74;
    if (n->unkD90 == 0) {
        n->unkD80[0] = 0.0f;
        *vy = 0.0f;
        *vz = 0.0f;
        n->unkD8E = 0;
    }
    return 1;
}
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

void func_0018D730(void);
void func_001D50F0(u8 slot, u8 model, u8 a);
void func_001D39C0(u8 slot, u8 model, u8 a, u8 b);

/* NPC slot's model (and its held item's). */
void func_005BB7C0(u8 slot, u8 model, u8 a, u8 b) {
    func_0018D730();
    func_001D50F0(slot, model, a);
    func_001D39C0(slot, model, a, b);
    func_0018D730();
}

/* The NPCs' update: by state, then the common parts. */
s32 func_001AF510(void);
s32 func_0064FD50(s32 a, s32 slot);
void func_00652B50(Npc *n, s32 a);
/* An NPC's turn in a conversation (at 0x10A4). */
typedef struct {
    s8 state;           /* 0x0 */
    u8 unk1[7];
    s16 timer;          /* 0x8 */
    u8 unkA[2];
    s16 wait;           /* 0xC */
} NpcCycle;

void *func_00686150(s32 a, u16 model, s32 b);
s32 func_006861F0(s32 a, u16 model);

/* Steps NPC n's conversation turn b: waits for a speaker as its model (func_00686150) and the
   length of its line (0xDBC), asks to talk (0xDA8), holds while func_006861F0 says so, then
   waits again. */
void func_005F41F0(Npc *n, NpcCycle *b) {
    void *m;

    switch (b->state) {
    case 0:
        b->timer = 0;
        m = func_00686150(1, n->unkBB4, 3);
        if (m != NULL) {
            b->timer = FIELD(m, s16, 0xDBC) / 2 + 0x10;
            b->state = 1;
        } else if (b->wait == 0) {
            if (n->visible != 0) {
                FIELD(n, u8, 0xDA8) = 1;
            }
            b->wait = 8;
        } else {
            b->wait--;
        }
        break;
    case 1:
        if (b->timer == 0) {
            b->timer = 0;
            b->state = 0;
            m = func_00686150(1, n->unkBB4, 3);
            if (m != NULL) {
                b->timer = FIELD(m, s16, 0xDBC) + 0x10;
                b->state = 2;
            }
        } else {
            b->timer--;
        }
        break;
    case 2:
        b->state = 3;
        FIELD(n, u8, 0xDA8) = 1;
        break;
    case 3:
        b->timer--;
        if (func_006861F0(1, n->unkBB4) == 0) {
            b->state = 4;
        }
        break;
    case 4:
        if (--b->timer < 0) {
            b->state = 0;
            b->timer = 0;
            b->wait = 8;
        }
        break;
    }
}
/* An NPC's life: 2 dead (0xBA0 & 0x20), 1 dying once its action is 3 or later, else 0. */
s32 func_005F4080(Npc *n) {
    s32 f = n->unkBA0;

    if (f & 0x20) {
        return 2;
    }
    if (f & 0x200) {
        return n->act[0] >= 3;
    }
    return 0;
}
extern u16 D_0048AFB0[][6];
extern u32 D_0048AD90[];
void func_0068A6E0(Npc *n);
s32 func_0068A700(Npc *n, s32 a);
s32 func_0068A950(Npc *n);
void func_0068AAB0(Npc *n, s32 a);
void func_0068AB80(Npc *n, s32 a);
s32 func_0068AA00(Npc *n);
s32 func_0068AA70(Npc *n, s32 a);
s32 func_0068A9B0(Npc *n);
s32 func_0068A9C0(Npc *n);
void func_0068AD30(Npc *n);
s32 func_0068ACA0(Npc *n);
s32 func_0068A660(Npc *n);
void func_005403E0(s32 a, s32 b);
/* The NPC's state becomes s (0xFCF), its steps from the start. */
void func_005F4BB0(Npc *n, s8 s) {
    n->unkFD0[3] = 0;
    n->unkFD0[2] = 0;
    n->unkFD0[1] = 0;
    n->unkFD0[0] = 0;
    n->unkFCF = s;
}

/* Unless held (0xBA0 & 0x80000): its mode 0xFB7 becomes m (restarting its actions if it changes),
   and it's shown. */
#define NPC_MODE(n, m)                      \
    if (!((n)->unkBA0 & 0x80000)) {         \
        if ((n)->unkFB7 != (m)) {           \
            (n)->act[0] = 0;                \
            (n)->act[1] = 0;                \
            (n)->act[2] = 0;                \
            (n)->act[3] = 0;                \
            (n)->unkFB8[0] = 0;             \
        }                                   \
        (n)->unkFB7 = (m);                  \
        (n)->visible = 1;                   \
    }
#define NPC_WAKE(n) NPC_MODE(n, 0)

extern DrawObjFull *D_0023BDF4;     /* the things */
/* Whether player p will do: 2 not (gone, hidden or down: 0xBA0 & 0x1E0), 1 the NPC is held
   (0xBA0 & 0x80000), 0 yes. */
s32 func_005F3C30(Npc *n, DrawObjFull *p);
#ifdef NON_MATCHING   /* nop left in the visible test's delay slot and a second 'return 2' */
s32 func_005F3C30(Npc *n, DrawObjFull *p) {
    if (p->active) {
        if (p->visible) {
            if (n->unkBA0 & 0x80000) {
                return 1;
            }
            return (p->unkBA0 & 0x1E0) ? 2 : 0;
        }
        return 2;
    }
    return 2;
}
#endif
f32 func_00634380(Npc *n, f32 *pos);

/* Finds what the NPC looks for (0xFCD's way, 0xFF6) into 0x1024: 0 the player numbered, 1 the
   character, 2 the NPC of that model, 3 the thing of that kind, 4/5 the nearest/farthest player
   in its room (within 0xFD8), 6/7 NPCs and 8/9 things the same way, 10 its target. */
void func_005F3C90(Npc *n);
#ifdef NON_MATCHING   /* matches once func_005F3C30 is matching (static) C */
void func_005F3C90(Npc *n) {
    DrawObjFull *best = NULL;
    s32 i;
    DrawObjFull *p;
    f32 d;
    f32 bd;
    s32 r;

    switch (n->unkFCD) {
    case 0:
        for (i = 0, p = D_00476DD0; i < 8; i++, p++) {
            if (p->active && p->unk1E2 == n->unk1E2 && p->kind == n->unkFF6) {
                best = p;
                break;
            }
        }
        break;
    case 1: {
        DrawObjFull *q;
        s32 j;

        for (j = 0, q = D_00476DD0; j < 8; j++, q++) {
            if (q->active && q->unk1E2 == n->unk1E2 && q->unkB98 == n->unkFF6) {
                best = q;
                break;
            }
        }
        break;
    }
    case 2:
        for (p = (DrawObjFull *)D_0023BE04; p != NULL; p = p->next) {
            if (p->active && p->unkBB4 == n->unkFF6) {
                best = p;
                break;
            }
        }
        break;
    case 3:
        for (p = D_0023BDF4; p != NULL; p = p->next) {
            if (p->active && p->kind == n->unkFF6) {
                best = p;
                break;
            }
        }
        break;
    case 4: {
        DrawObjFull *q;
        s32 j;

        bd = n->unkFD8;
        for (q = D_00476DD0, j = 0; j < 8; j++, q++) {
            if (q->active && q->unk1E2 == n->unk1E2) {
                r = func_005F3C30(n, q);
                if (r == 0) {
                    d = func_00634380(n, q->unk38);
                    if (!(bd <= d)) {
                        bd = d;
                        best = q;
                    }
                } else if (r == 1) {
                    best = &D_00476DD0[(u16)n->unkC66];
                }
            }
        }
        break;
    }
    case 5: {
        DrawObjFull *q;
        s32 j;

        bd = n->unkFD8;
        for (q = D_00476DD0, j = 0; j < 8; j++, q++) {
            if (q->active && q->unk1E2 == n->unk1E2) {
                d = func_00634380(n, q->unk38);
                if (bd < d) {
                    bd = d;
                    best = q;
                }
            }
        }
        break;
    }
    case 8:
        bd = 10000;
        for (p = D_0023BDF4; p != NULL; p = p->next) {
            if (p->active) {
                d = func_00634380(n, p->unk38);
                if (!(bd <= d)) {
                    bd = d;
                    best = p;
                }
            }
        }
        break;
    case 9:
        bd = 0.0f;
        for (p = D_0023BDF4; p != NULL; p = p->next) {
            if (p->active) {
                d = func_00634380(n, p->unk38);
                if (!(bd <= d)) {
                    bd = d;
                    best = p;
                }
            }
        }
        break;
    case 6:
        bd = 10000;
        for (p = (DrawObjFull *)D_0023BE04; p != NULL; p = p->next) {
            if (p->active) {
                d = func_00634380(n, p->unk38);
                if (!(bd <= d)) {
                    bd = d;
                    best = p;
                }
            }
        }
        break;
    case 7:
        bd = 0.0f;
        for (p = (DrawObjFull *)D_0023BE04; p != NULL; p = p->next) {
            if (p->active) {
                d = func_00634380(n, p->unk38);
                if (!(bd <= d)) {
                    bd = d;
                    best = p;
                }
            }
        }
        break;
    case 10:
        best = (DrawObjFull *)n->unk24;
        break;
    }
    n->unk1024 = best;
}
#endif
/* Queues a func_00652B50 request (0x1138, 0x1139). */
void func_005F4BD0(Npc *n, u8 kind) {
    n->unk1138 = 1;
    n->unk1139 = kind;
}

/* Whether it has a player to follow: online only for its owner (func_0064FD50), it looks
   (func_005F3C90) every 8 frames and takes the one found. */
s32 func_0068B4E0(Npc *n);
s32 func_0068B4E0(Npc *n) {
    s32 p;

    if (func_0064FD50(1, n->slot)) {
        if (n->unkFCA == 0) {
            n->unkFCD = 4;
            func_005F3C90(n);
            if (n->unk1024 == NULL) {
                p = 0;
                NPC_WAKE(n);
            } else {
                p = n->unk1024->kind + 1;
            }
            if (p != n->unk1020) {
                n->unk1020 = p;
                func_005F4BD0(n, 3);
                n->unkFCA = 8;
            }
            if (p == 0) {
                return 0;
            }
        } else {
            n->unkFCA--;
            return 0;
        }
    } else if (n->unk1020 == 0) {
        NPC_WAKE(n);
        return 0;
    }
    return 1;

}

/* The player it follows (0x1020, 1-based; 0 none): while held (0xBA0 & 0x80000) the one holding
   it (0xC66), else it lets go of one that's down (0xBA0 & 0x1E0) unless func_0068B4E0 says. */
u8 func_0068AF60(Npc *n) {
    u8 i;

    if (n->unkBA0 & 0x80000) {
        s32 v = (u16)n->unkC66 + 1;

        if (n->unk1020 == v) {
            return n->unk1020;
        }
        n->unk1020 = v;
    } else {
        i = n->unk1020 & 0xFF;
        if (n->unk1020 == 0) {
            if (!func_0068B4E0(n)) {
                NPC_WAKE(n);
                n->unk1020 = 0;
            }
        } else if ((D_00476DD0[i - 1].unkBA0 & 0x1E0) && !func_0068B4E0(n)) {
            NPC_WAKE(n);
            n->unk1020 = 0;
        }
    }
    return n->unk1020;
}
/* Lets go of its player once they're in different rooms; always 1. */
s32 func_0068B0A0(Npc *n) {
    DrawObjFull *p = &D_00476DD0[n->unk1020 - 1];

    if (n->unk1E2 != p->unk1E2) {

        NPC_WAKE(n);
        n->unk1020 = 0;
    }
    return 1;
}
/* Always 1 (a byte tested against 0x20000000). */
s32 func_0068B130(Npc *n, DrawObjFull *p) {
    return !(p->unk54F & 0x20000000);
}
f32 func_00634380(Npc *n, f32 *pos);
/* Stops following (telling the others if it's ours) and isn't held any more. */
void func_0068B150(Npc *n) {
    if (func_0064FD50(1, n->slot)) {
        func_00652B50(n, 3);
    }
    n->unk1020 = 0;
    n->unkBA0 &= ~0x80000;
}
s32 func_0068B1B0(Npc *n);
void func_0068AD40(Npc *n, DrawObjFull *p);

/* Following its player (0x1020): stays put near (100), walks up (func_0068AD40) further away,
   and gives up past 300. */
void func_0068A780(Npc *n) {
    DrawObjFull *p;
    f32 d;

    n->unkFD8 = 300.0f;
    if (func_0068AF60(n)) {
        p = &D_00476DD0[n->unk1020 - 1];
        if (func_0068B0A0(n) && func_0068B130(n, p)) {
            d = func_00634380(n, p->unk38);
            if (d > 300.0f) {
                func_0068B150(n);
                NPC_WAKE(n);
            } else if (!(p->unkBA0 & 0x10000) || !func_0068B1B0(n)) {
                if (d > 100.0f) {
                    NPC_MODE(n, 1);
                    func_0068AD40(n, p);
                    return;
                }
                NPC_WAKE(n);
            }
        }
    }
}

/* An NPC's scripted job (0x1150): each step calls func_0068A6E0 once it's done. */
void func_0068A170(Npc *n) {
    switch (n->unk1150) {
    case 0:
        func_0068A780(n);
        if (func_0068A700(n, 0)) {
            func_0068A6E0(n);
            return;
        }
        break;
    case 1:
        func_0068A780(n);
        if (func_0068A950(n)) {
            func_0068A6E0(n);
        }
        break;
    case 2:
        func_0068AAB0(n, 0);
        if (func_0068AA00(n)) {
            func_0068A6E0(n);
        } else if (func_0068AA70(n, 0)) {
            func_0068A6E0(n);
        }
        break;
    case 3:
        func_0068AB80(n, 0);
        if (func_0068AA00(n)) {
            func_0068A6E0(n);
        } else if (func_0068A9B0(n)) {
            func_0068A6E0(n);
        }
        break;
    case 4:
        NPC_WAKE(n);
        if (func_0068A9C0(n)) {
            func_0068A6E0(n);
        }
        break;
    case 5:
        func_0068AD30(n);
        if (func_0068ACA0(n)) {
            func_0068A6E0(n);
        }
        break;
    case 6:
        func_0068AAB0(n, 1);
        if (!(D_0048AFB0[5][0] & 8)) {
            func_00652B50(n, 1);
            func_0068A6E0(n);
        } else if (func_0068AA70(n, 1)) {
            func_0068A6E0(n);
        }
        break;
    case 7:
        func_0068AB80(n, 1);
        if (!(D_0048AFB0[5][0] & 8)) {
            func_00652B50(n, 1);
            func_0068A6E0(n);
        } else if (func_0068A9B0(n)) {
            func_005403E0(0x3E, 1);
            func_0068A6E0(n);
        }
        break;
    case 8:
        NPC_WAKE(n);
        if (!(D_0048AFB0[5][0] & 8)) {
            func_0068A6E0(n);
        }
        break;
    case 9:
        func_0068A780(n);
        if (func_0068A700(n, 1)) {
            func_0068A6E0(n);
        }
        break;
    case 10:
        func_0068AAB0(n, 2);
        if (D_0048AD90[0] & 0x2000) {
            func_0068A6E0(n);
        } else if (func_0068AA70(n, 2)) {
            func_0068A6E0(n);
        }
        break;
    case 11:
        func_0068AB80(n, 2);
        if (D_0048AD90[0] & 0x2000) {
            func_0068A6E0(n);
        } else if (func_0068A9B0(n)) {
            func_0068A6E0(n);
        }
        break;
    case 12:
        NPC_WAKE(n);
        if (D_0048AD90[0] & 0x2000) {
            func_0068A6E0(n);
        }
        break;
    case 13:
        func_0068A780(n);
        if (func_0068A700(n, 2)) {
            func_0068A6E0(n);
        }
        break;
    case 14:
        NPC_WAKE(n);
        if (func_0068A660(n)) {
            func_005403E0(0x35, 1);
            n->unk544 = 0;
            func_005F4BB0(n, 4);
            func_0068A6E0(n);
        }
        break;
    case 15:
        NPC_WAKE(n);
        break;
    }
}
void func_00639CE0(Npc *n);

/* Empty (a debug hook). */
void func_005F4C10(Npc *n) {
}

extern u8 D_0048BF93[];         /* counts frames */

/* Every 30 (2-frame) ticks, in its kind's slot: the NPC's idle func_00652B50 requests (3 in the
   slot 15 later; 5 when 0xBA0 has 0x80000, else 7 in action 1 and 0 in most of action 2). */
void func_0058DD50(Npc *n) {
    s32 f = D_0048BF93[0] >> 1;
    u8 k = n->kind;
    s32 t = f % 30;

    if (t == k + 15) {
        func_00652B50(n, 3);
        return;
    }
    if (t == k) {
        if (n->unkBA0 & 0x80000) {
            func_00652B50(n, 5);
        } else {
            switch (n->act[0]) {
            case 1:
                switch (n->act[1]) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                case 8: case 9: case 10: case 11: case 12: case 13: case 14:
                    func_00652B50(n, 7);
                    break;
                }
                break;
            case 2:
                switch (n->act[1]) {
                case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                case 10:
                    func_00652B50(n, 0);
                    break;
                }
                break;
            }
        }
    }
}

/* Plays the NPC's queued func_00652B50 request (0x1139), once. */
void func_005F4BE0(Npc *n) {
    if (n->unk1138) {
        n->unk1138 = 0;
        func_00652B50(n, n->unk1139);
    }
}

void func_00689F40(Npc *n);
void func_00689FE0(Npc *n);
void func_0068A0A0(Npc *n);
void func_0068A0B0(Npc *n);
void func_0068A0C0(Npc *n);
void func_0068A0D0(Npc *n);
void func_0068A0E0(Npc *n);
void func_0068A0F0(Npc *n);

void func_00689E50(Npc *n) {
    switch (n->unkFCF) {
    case 0:
        func_00689F40(n);
        break;
    case 1:
        func_00689FE0(n);
        break;
    case 2:
        func_0068A0F0(n);
        break;
    case 3:
        func_0068A0C0(n);
        break;
    case 5:
        func_0068A0A0(n);
        break;
    case 6:
        func_0068A0B0(n);
        break;
    case 7:
        func_0068A0D0(n);
        break;
    case 4:
        func_0068A0E0(n);
        break;
    }
    func_00639CE0(n);
    func_005F4BE0(n);
    func_0058DD50(n);
    func_005F4C10(n);
}

/* State 0: its record marked, everything reset, then state 1 (shown unless it is gone). */
void func_00689F40(Npc *n) {
    NpcRecord *r = &D_003614F0[n->slot];

    r->flags2 |= 1;
    n->unkFD0[3] = 0;
    n->unkFD0[2] = 0;
    n->unkFD0[1] = 0;
    n->unkFD0[0] = 0;
    n->act[0] = 0;
    n->act[1] = 0;
    n->act[2] = 0;
    n->act[3] = 0;
    n->unkFB8[0] = 0;
    n->unkFB7 = 0;
    n->unkFCF = 1;
    if (!(n->unkBA0 & 0x80000)) {
        if (n->unkFB7 != 0) {
            n->act[0] = 0;
            n->act[1] = 0;
            n->act[2] = 0;
            n->act[3] = 0;
            n->unkFB8[0] = 0;
        }
        n->unkFB7 = 0;
        n->visible = 1;
    }
    n->unk1150 = 0;
}

/* State 1: on to state 2 (in the network modes once func_0064FD50 allows it). */
void func_00689FE0(Npc *n) {
    if (func_001AF510() == 1) {
        if (func_0064FD50(1, n->slot)) {
            n->unkFD0[3] = 0;
            n->unkFD0[2] = 0;
            n->unkFD0[1] = 0;
            n->unkFD0[0] = 0;
            n->act[3] = 0;
            n->act[2] = 0;
            n->act[1] = 0;
            n->act[0] = 0;
            n->unkFCF = 2;
            n->visible = 1;
            func_00652B50(n, 0);
        }
    } else {
        n->unkFD0[3] = 0;
        n->unkFD0[2] = 0;
        n->unkFD0[1] = 0;
        n->unkFD0[0] = 0;
        n->act[3] = 0;
        n->act[2] = 0;
        n->act[1] = 0;
        n->act[0] = 0;
        n->unkFCF = 2;
        n->visible = 1;
    }
    func_005F41F0(n, (NpcCycle *)((u8 *)n + 0x10A4));
}

void func_005F4380(Npc *n);
void func_005F46A0(Npc *n);
void func_005F40C0(Npc *n);

void func_0068A0A0(Npc *n) {
    FIELD(n, u8, 0x32) = 0xFF;
    func_005F4380(n);
}

void func_0068A0B0(Npc *n) {
    func_005F46A0(n);
}

void func_0068A0C0(Npc *n) {
    func_005F40C0(n);
}

void func_0068A0D0(Npc *n) {
    func_005F41F0(n, (NpcCycle *)((u8 *)n + 0x10A4));
}

void func_0068A0E0(Npc *n) {
}

/* State 2: alive (func_005F4080 0), or on to state 3 (1) or 4 (dead). */
void func_0068A0F0(Npc *n) {
    s32 r = func_005F4080(n);

    if (r == 0) {
        func_0068A170(n);
        func_005F41F0(n, (NpcCycle *)((u8 *)n + 0x10A4));
        return;
    }
    if (r == 1) {
        n->unkFCF = 3;
    } else {
        n->unkFCF = 4;
        n->unk544 = 0;
        func_005F4B30(n->slot);
    }
    func_00652B50(n, 0);
}
