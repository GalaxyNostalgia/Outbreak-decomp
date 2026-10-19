#include "common.h"
#include "object.h"
#include "model.h"
#include "thing.h"
#include "enemy.h"
#include "vu0.h"

/* Each frame (and at room setup): the characters' skeletons from their positions, scales and
   motions. */

extern DrawObjFull D_00476DD0[8];
extern Npc *D_0023BE04;             /* the NPCs */
extern Thing *D_0023BDF4;           /* the things */
extern Thing *D_0023BCCC;           /* the thing being done */
void func_005A6720(void *o);
void func_005A5BA0(void *o);
void func_00346B30(void *o);
void func_006313A0(void *o, u8 item, s32 bone);
void func_0055F470(Thing *t);

#define SKEL_SCALE(o) \
    func_0018F830((o)->unkB4[0], (o)->unkB4[1], (o)->unkB4[2], \
                  (f32 *)(*(u8 **)((u8 *)(o)->model + 0x820) + 0x40))

void func_00346810(void) {
    DrawObjFull *p;
    s32 i;

    for (i = 0; i < 8; i++) {
        p = &D_00476DD0[i];
        if (p->active == 0) {
            continue;
        }
        p->unk80[0] = p->unk38[0];
        p->unk80[1] = p->unk38[1];
        p->unk80[2] = p->unk38[2];
        SKEL_SCALE(p);
        func_005A6720(p);
        if (!(p->unk54A & 0x12)) {
            func_005A5BA0(p);
        } else {
            func_00346B30(p);
        }
        func_006313A0(p, p->unkC7C, 0x13);
        func_006313A0(p, p->unkC7D, 0xF);
    }
}

void func_00346900(void) {
    Npc *n;

    for (n = D_0023BE04; n != NULL; n = n->next) {
        if (n->active == 0) {
            continue;
        }
        n->unk80[0] = n->unk38[0];
        n->unk80[1] = n->unk38[1];
        n->unk80[2] = n->unk38[2];
        SKEL_SCALE(n);
        func_005A6720(n);
        func_00346B30(n);
        func_006313A0(n, n->unkC7C, 0x13);
    }
}

void func_00346990(void) {
    for (D_0023BCCC = D_0023BDF4; D_0023BCCC != NULL; D_0023BCCC = D_0023BCCC->next) {
        if (D_0023BCCC->visible == 0) {
            continue;
        }
        *(f32 *)((u8 *)D_0023BCCC + 0x80) = D_0023BCCC->pos[0];
        *(f32 *)((u8 *)D_0023BCCC + 0x84) = D_0023BCCC->pos[1];
        *(f32 *)((u8 *)D_0023BCCC + 0x88) = D_0023BCCC->pos[2];
        func_0018F830(D_0023BCCC->scale[0], D_0023BCCC->scale[1], D_0023BCCC->scale[2],
                      (f32 *)(*(u8 **)D_0023BCCC->model->unk820 + 0x40));
        switch (D_0023BCCC->kind) {
        case 1:
            if (D_00357500[D_0023BCCC->kind].defs[D_0023BCCC->def].unk5 == 1) {
                break;
            }
        case 0x1B:
            func_005A6720(D_0023BCCC);
            break;
        case 0x38:
            if (D_0023BCCC->unk4 == 0) {
                func_0055F470(D_0023BCCC);
            }
            break;
        }
        if (!(*(u16 *)((u8 *)D_0023BCCC + 0x54A) & 0x12) && D_0023BCCC->unk54D != 2) {
            func_005A5BA0(D_0023BCCC);
        } else {
            func_00346B30(D_0023BCCC);
        }
    }
}

/* The character's point at 0x1AC through its matrix, into 0x1CC. */
void func_00346B90(DrawObjFull *o) {
    func_001913A0((f32 *)((u8 *)o + 0x1CC), o->unk1AC, (f32 *)((u8 *)o + 0x50));
}

void func_00192BE0(void *b, f32 *parent);

/* Poses o's skeleton from its matrix (0x50) and its motion. */
void func_00346B30(void *p) {
    DrawObjFull *o = p;
    void **b = (void **)((u8 *)o->model + 0x820);

    switch (o->type) {
    case 0:
    case 1:
        func_00192BE0(*b, (f32 *)((u8 *)o + 0x50));
        return;
    }
    func_00192BE0(*b, (f32 *)((u8 *)o + 0x50));
}
