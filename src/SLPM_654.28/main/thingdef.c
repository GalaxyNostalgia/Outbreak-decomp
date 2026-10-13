#include "common.h"
#include "enemy.h"
#include "thing.h"

/* A thing's entry in its type's table. */

s32 func_001B7950(Thing *t) {
    return D_00357500[t->kind].defs[t->def].unk6 != 0;
}

s8 func_001B7980(Thing *t) {
    return D_00357500[t->kind].defs[t->def].variant;
}
