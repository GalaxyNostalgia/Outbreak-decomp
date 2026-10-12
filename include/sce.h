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
s32 strncmp(const char *a, const char *b, u32 n);
char *strncpy(char *dst, const char *src, u32 n);
char *strrchr(const char *s, s32 c);
s32 strcmp(const char *a, const char *b);
void *memcpy(void *dst, const void *src, u32 n);
void *memset(void *dst, s32 c, u32 n);
/* On PC the game's heap comes from a block in the program's own image (src/ps2ram.c), so every address the
   game handles stays below 0x10000000 like on the PS2: its DMA queue keeps a type in an address's top 4 bits. */
#ifdef PLATFORM_PC
#define malloc pc_game_malloc
#define free pc_game_free
#endif
void *malloc(u32 size);
void free(void *p);

s32 sceOpen(const char *name, s32 flags, ...);
s32 sceClose(s32 fd);
s32 sceLseek(s32 fd, s32 offset, s32 whence);

#endif
