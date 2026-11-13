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

#include "options.h"
void func_00349E20(void);       /* SUBMAIN.BIN: the movie stops */
void func_001D0300(s32 first, s32 n);
void *memset(void *p, s32 c, u32 n);

/* The movie (or picture) is let go: its texture slot freed and its state cleared. */
void func_001CF7B0(void) {
    D_0023BC70 = D_003209B0;
    if (D_00306590.unk15 != 0) {
        func_00349E20();
        return;
    }
    if (*(s32 *)D_0023BC70 != 0) {
        if (*(s32 *)(D_0023BC70 + 0x84) != 0) {
            func_001D0300(0xFF0, 1);
            *(s32 *)(D_0023BC70 + 0x84) = 0;
        }
        *(s32 *)D_0023BC70 = 0;
        memset(D_003209B0, 0, 0x98);
    }
}
