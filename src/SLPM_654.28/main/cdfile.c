#include "common.h"

/* Disc file helpers: build "cdrom0:\NAME;1" paths and query file sizes.
   On the PC port these paths go through pc/src/sce_io.c, which reads the user's disc image. */

extern char D_00224268[];   /* "cdrom0:" */
extern char D_00224270[];   /* ";1" */
extern char D_00232210[];   /* "cdrom0:\\%s;1" */
extern char D_00232220[];   /* "pfs0:%s" */
extern s32 D_0023BAF4;      /* non-zero: hold the disc lock around file access */

#include "sce.h"
void func_0012BAB8(void);                              /* disc lock */
void func_0012BAD0(void);                              /* disc unlock */

/* Size in bytes of a file on the disc, 0 if it doesn't exist. */
s32 func_001A8300(const char *name) {
    char path[0x800];
    char *p;
    s32 fd, size;

    strcpy(path, D_00224268);
    p = path + strlen(path);
    strcat(path, name);
    strupr(p);
    strcat(path, D_00224270);
    if (D_0023BAF4 != 0) func_0012BAB8();
    if ((fd = sceOpen(path, 1)) < 0) {
        if (D_0023BAF4 != 0) func_0012BAD0();
        return 0;
    }
    size = sceLseek(fd, 0, 2);
    sceClose(fd);
    if (D_0023BAF4 != 0) func_0012BAD0();
    return size;
}

/* "cdrom0:\NAME;1", with NAME upper-cased */
void func_001BC780(char *dst, const char *name) {
    sprintf(dst, D_00232210, name);
    strupr(dst + 8);
}

/* "pfs0:NAME" (HDD install) */
void func_001BC7C0(char *dst, const char *name) {
    sprintf(dst, D_00232220, name);
}
