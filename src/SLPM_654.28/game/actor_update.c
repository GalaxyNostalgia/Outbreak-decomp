#include "actor.h"

/* Per-enemy-type update wrappers: refresh the previous position while in one
   particular state, run the shared step, then the type's own update. */

extern void func_00577E00(void *a, void *b, s32 c);
extern void func_005F10B0(Actor *a);
extern void func_00629B30(Actor *a);
extern void func_00698350(Actor *a);

void func_005F37D0(Actor *a) {
    if (a->state == 1 && a->substate == 15) {
        a->prevPos.x = a->pos.x;
        a->prevPos.y = a->pos.y;
        a->prevPos.z = a->pos.z;
    }
    func_00577E00(a->unkD18, a->unkD00, a->unkBA3);
    func_005F10B0(a);
}

void func_0062C1D0(Actor *a) {
    if (a->state == 1 && a->substate == 15) {
        a->prevPos.x = a->pos.x;
        a->prevPos.y = a->pos.y;
        a->prevPos.z = a->pos.z;
    }
    func_00577E00(a->unkD18, a->unkD00, a->unkBA3);
    func_00629B30(a);
}

void func_006994D0(Actor *a) {
    if (a->state == 1 && a->substate == 7) {
        a->prevPos.x = a->pos.x;
        a->prevPos.y = a->pos.y;
        a->prevPos.z = a->pos.z;
    }
    func_00577E00(a->unkD18, a->unkD00, a->unkBA3);
    func_00698350(a);
}
