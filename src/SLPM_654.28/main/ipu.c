#include "common.h"

/* Pictures decoded by the IPU (the PS2's MPEG decoder) frame by frame. */

extern u8 D_003209B0[];
extern u8 *D_0023BC70;
void func_001CF890(u8 *dec);

void func_001CF840(void) {
    D_0023BC70 = D_003209B0;
    if (D_0023BC70 != NULL && *(s32 *)D_0023BC70 != 0 && *(s32 *)(D_0023BC70 + 0x84) != 0) {
        func_001CF890(D_0023BC70);
    }
}
