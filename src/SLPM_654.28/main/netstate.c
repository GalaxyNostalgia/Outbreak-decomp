#include "common.h"

/* Network connection state. */

extern s32 D_00309490[];    /* 3: connected (in a structure not mapped yet) */

s32 func_001C53F0(void) {
    return D_00309490[0] == 3;
}
