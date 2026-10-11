#include "iso9660.h"
#include "log.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SECTOR 2048u

static FILE *g_iso;
static IsoFile g_root;

static int seek(uint64_t off) {
#ifdef _WIN32
    return _fseeki64(g_iso, (long long)off, SEEK_SET);
#else
    return fseeko(g_iso, (off_t)off, SEEK_SET);
#endif
}

static int read_at(uint64_t off, void *dst, uint32_t n) {
    if (seek(off) != 0) return -1;
    return fread(dst, 1, n, g_iso) == n ? 0 : -1;
}

static uint32_t le32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }

int iso_open(const char *path) {
    uint8_t pvd[SECTOR];
    g_iso = fopen(path, "rb");
    if (!g_iso) {
        log_error("iso: cannot open %s", path);
        return -1;
    }
    if (read_at(16ull * SECTOR, pvd, SECTOR) != 0 || memcmp(pvd + 1, "CD001", 5) != 0) {
        log_error("iso: %s is not an ISO 9660 image", path);
        iso_close();
        return -1;
    }
    g_root.lba = le32(pvd + 156 + 2);
    g_root.size = le32(pvd + 156 + 10);
    log_info("iso: opened %s", path);
    return 0;
}

void iso_close(void) {
    if (g_iso) fclose(g_iso);
    g_iso = NULL;
}

/* compare one path component with a directory record name ("NAME.EXT;1"), ignoring case and ";1" */
static int name_eq(const char *comp, size_t len, const uint8_t *name, uint8_t nlen) {
    size_t n = nlen;
    for (size_t i = 0; i < nlen; i++)
        if (name[i] == ';') { n = i; break; }
    if (len >= 2 && comp[len - 2] == ';') len -= 2;
    if (n != len) return 0;
    for (size_t i = 0; i < n; i++)
        if (toupper((unsigned char)comp[i]) != toupper(name[i])) return 0;
    return 1;
}

static int find_in_dir(const IsoFile *dir, const char *comp, size_t len, IsoFile *out, int *is_dir) {
    uint8_t *buf = malloc(dir->size);
    if (!buf || read_at((uint64_t)dir->lba * SECTOR, buf, dir->size) != 0) {
        free(buf);
        return -1;
    }
    for (uint32_t i = 0; i < dir->size;) {
        uint8_t rlen = buf[i];
        if (rlen == 0) {                      /* records don't cross sectors */
            i = (i / SECTOR + 1) * SECTOR;
            continue;
        }
        uint8_t nlen = buf[i + 32];
        if (name_eq(comp, len, buf + i + 33, nlen)) {
            out->lba = le32(buf + i + 2);
            out->size = le32(buf + i + 10);
            *is_dir = (buf[i + 25] & 2) != 0;
            free(buf);
            return 0;
        }
        i += rlen;
    }
    free(buf);
    return -1;
}

int iso_find(const char *path, IsoFile *out) {
    IsoFile cur = g_root;
    int is_dir = 1;
    if (!g_iso) return -1;
    while (*path == '/' || *path == '\\') path++;
    while (*path) {
        const char *end = path;
        while (*end && *end != '/' && *end != '\\') end++;
        if (!is_dir || find_in_dir(&cur, path, (size_t)(end - path), &cur, &is_dir) != 0) return -1;
        path = end;
        while (*path == '/' || *path == '\\') path++;
    }
    if (is_dir) return -1;
    *out = cur;
    return 0;
}

int64_t iso_read(const IsoFile *f, uint32_t offset, void *dst, uint32_t size) {
    if (offset >= f->size) return 0;
    if (size > f->size - offset) size = f->size - offset;
    if (read_at((uint64_t)f->lba * SECTOR + offset, dst, size) != 0) return -1;
    return size;
}
