#ifndef SCE_H
#define SCE_H

#include "common.h"

/* Sony SDK and C library functions the game calls. On PS2 they stay as the
   original object code (asm); the PC port implements them in pc/src/. */

char *strcpy(char *dst, const char *src);
s32 strlen(const char *s);
char *strcat(char *dst, const char *src);
char *strupr(char *s);                       /* upper-cases ASCII in place (Sony libc ctype table) */
s32 sprintf(char *dst, const char *fmt, ...);
s32 printf(const char *fmt, ...);

s32 sceOpen(const char *name, s32 flags, ...);
s32 sceClose(s32 fd);
s32 sceLseek(s32 fd, s32 offset, s32 whence);

#endif
