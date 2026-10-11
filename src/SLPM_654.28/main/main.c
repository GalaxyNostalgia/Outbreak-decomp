#include "sce.h"

/* The game's main(), called by crt0 (_start) after kernel setup.
   On PC, pc/src/main.c calls it once guest memory and the disc are ready. */

extern char D_00295A10[];   /* boot path: argv[0] */
extern u16 D_0023BBA4;
extern s32 *D_0023BC54;
extern s8 D_0023BC38;

void func_001A85C0(void);   /* system init: graphics, IOP modules, ... */
void func_001B9070(void);   /* per-frame work */
s32 func_001C53F0(void);
void func_001C5430(s32 a, s32 b);

s32 func_001A87F0(s32 argc, char **argv) {
    if (argc > 0) {
        strcpy(D_00295A10, argv[0]);
    }
    func_001A85C0();
    while (1) {
        func_001B9070();
        if ((func_001C53F0() != 0) ^ 1) {
            continue;
        }
        if (D_0023BBA4 & 0x1F) {
            continue;
        }
        func_001C5430(*D_0023BC54, D_0023BC38 == 1);
    }
}
