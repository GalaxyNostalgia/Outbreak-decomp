/* Read-only access to the user's disc image (ISO 9660, 2048-byte sectors). */
#ifndef ISO9660_H
#define ISO9660_H

#include <stdint.h>

typedef struct IsoFile {
    uint32_t lba;
    uint32_t size;
} IsoFile;

int iso_open(const char *path);                          /* 0 on success */
void iso_close(void);
/* Path like "PROG\\GAME.BIN", "/PROG/GAME.BIN" or "\\SLPM_654.28;1"; case-insensitive. */
int iso_find(const char *path, IsoFile *out);            /* 0 if found */
int64_t iso_read(const IsoFile *f, uint32_t offset, void *dst, uint32_t size);  /* bytes read or -1 */

#endif
