#include "common.h"

/* Text tables: the title menus, the save files' descriptions, item names from the event list. */

extern char *D_00217870[];          /* menu labels */
extern char **D_00217900[];         /* description lines of each save file */
extern char *D_0023A078[2];
extern u8 *D_00306650[];            /* (Options) the event list (evelist.dat) */
extern char D_00308F30[];           /* converted text */
void func_001BFFB0(u8 *src, char *dst);

char *func_001C0240(s32 n) {
    return D_00217870[n];
}

char **func_001C0260(s32 file) {
    return D_00217900[file];
}

char *func_001C0280(s32 n) {
    return D_0023A078[n];
}

/* An item's name: entry `item` of file `file` in the event list, converted for the font. */
char *func_001C02A0(s32 file, s32 item) {
    u8 *p = D_00306650[0];

    if (p == NULL) {
        return NULL;
    }
    p += file * 0x1000;
    p += item * 0x40;
    func_001BFFB0(p, D_00308F30);
    return D_00308F30;
}

extern char *D_002180F0[90];        /* the extra characters' names */

char *func_001C0180(s32 n) {
    if (n < 0 || n >= 90) {
        return NULL;
    }
    return D_002180F0[n];
}
