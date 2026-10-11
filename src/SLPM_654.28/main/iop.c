#include "sce.h"
#include "kernel.h"

extern char D_002238D0[];   /* "Can't load module %s" */

/* Load an IOP module from disc, retrying `tries` times (0: once, report failure). */
void func_0018D1F0(char *path, s32 argLen, char *args, s32 tries) {
    s32 i;

    if (tries == 0) {
        if (sceSifLoadModule(path, argLen, args) < 0) {
            printf(D_002238D0, path);
        }
    } else {
        for (i = 0; i < tries; i++) {
            if (sceSifLoadModule(path, argLen, args) > 0) {
                return;
            }
        }
        printf(D_002238D0, path);
    }
}

/* Load and start an IOP module; -1 when the module ends right away (its device isn't there). */
s32 func_001BB500(char *path, s32 argLen, char *args) {
    s32 result;

    while (sceSifLoadStartModule(path, argLen, args, &result) < 0) {
    }
    return -(result == 1);
}

extern s32 D_00239FE0;      /* hdd0: status (3: no HDD) */
extern s32 D_0023BBB4;      /* game data on the HDD: 0 no, 1 yes, 2 out of date */
extern char D_002320F0[];   /* "dev9" */
extern char D_00232100[];   /* "cdrom0:\IOP\MODULES\DEV9.IRX;1" */
extern char D_00232120[];   /* "...ATAD.IRX;1" */
extern char D_00232140[];   /* "...HDD.IRX;1" */
extern char D_00232160[];   /* "...PFS.IRX;1" */
extern char D_00232180[];   /* "hdd0:" */
extern char D_00217368[];   /* HDD.IRX args: "-o" "2" "-n" "3" */
extern char D_00217380[];   /* PFS.IRX args: "-m" "2" "-n" "24" "-o" "8" */
void func_001BB570(void);
void func_001BB6F0(void);

/* HDD setup: load the HDD driver stack and check for the game's data on the HDD. */
void func_001BB420(void) {
    D_00239FE0 = 3;
    D_0023BBB4 = 0;
    if (sceSifSearchModuleByName(D_002320F0) < 0) {
        if (func_001BB500(D_00232100, 0, NULL) < 0) {
            return;
        }
    }
    if (func_001BB500(D_00232120, 0, NULL) < 0) {
        return;
    }
    if (func_001BB500(D_00232140, 10, D_00217368) < 0) {
        return;
    }
    if (func_001BB500(D_00232160, 16, D_00217380) < 0) {
        return;
    }
    func_001BB570();
    D_00239FE0 = sceDevctl(D_00232180, 0x4807, NULL, 0, NULL, 0);
    func_001BB6F0();
}
