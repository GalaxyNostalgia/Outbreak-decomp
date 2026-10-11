#include "loader.h"
#include "guest.h"
#include "iso9660.h"
#include "log.h"

#include <stdlib.h>
#include <string.h>

static uint32_t rd32(const uint8_t *p) { return p[0] | p[1] << 8 | p[2] << 16 | (uint32_t)p[3] << 24; }
static uint16_t rd16(const uint8_t *p) { return (uint16_t)(p[0] | p[1] << 8); }

static uint8_t *read_whole(const char *path, uint32_t *size) {
    IsoFile f;
    if (iso_find(path, &f) != 0) {
        log_error("loader: %s not found on the disc", path);
        return NULL;
    }
    uint8_t *buf = malloc(f.size ? f.size : 1);
    if (!buf || iso_read(&f, 0, buf, f.size) != (int64_t)f.size) {
        log_error("loader: cannot read %s", path);
        free(buf);
        return NULL;
    }
    *size = f.size;
    return buf;
}

uint32_t loader_load_elf(const char *path) {
    uint32_t size;
    uint8_t *d = read_whole(path, &size);
    if (!d) return 0;
    if (size < 0x34 || memcmp(d, "\x7f" "ELF", 4) != 0 || d[4] != 1 || d[5] != 1) {
        log_error("loader: %s is not a 32-bit little-endian ELF", path);
        free(d);
        return 0;
    }
    uint32_t entry = rd32(d + 0x18), phoff = rd32(d + 0x1C);
    uint16_t phentsize = rd16(d + 0x2A), phnum = rd16(d + 0x2C);
    for (uint16_t i = 0; i < phnum; i++) {
        const uint8_t *ph = d + phoff + (uint32_t)i * phentsize;
        uint32_t type = rd32(ph), off = rd32(ph + 4), vaddr = rd32(ph + 8);
        uint32_t filesz = rd32(ph + 16), memsz = rd32(ph + 20);
        if (type != 1 || filesz == 0) continue;     /* empty PT_LOADs are overlay slots */
        if (!guest_valid(vaddr, memsz) || off + filesz > size) {
            log_error("loader: segment at 0x%08x doesn't fit guest memory", vaddr);
            free(d);
            return 0;
        }
        memcpy(guest_ptr(vaddr), d + off, filesz);
        memset(guest_ptr(vaddr + filesz), 0, memsz - filesz);
        log_info("loader: %s 0x%08x-0x%08x (%u bytes data, %u bytes bss)", path, vaddr, vaddr + memsz,
                 filesz, memsz - filesz);
    }
    free(d);
    return entry;
}

int loader_load_overlay(const char *path) {
    uint32_t size;
    uint8_t *d = read_whole(path, &size);
    if (!d) return -1;
    if (size < 0x40 || memcmp(d, "MWo3", 4) != 0) {
        log_error("loader: %s is not a Metrowerks overlay", path);
        free(d);
        return -1;
    }
    uint32_t id = rd32(d + 4), load = rd32(d + 8), text = rd32(d + 12), data = rd32(d + 16), bss = rd32(d + 20);
    if (0x40 + text + data != size || !guest_valid(load, size + bss)) {
        log_error("loader: overlay %s has an unexpected layout", path);
        free(d);
        return -1;
    }
    memcpy(guest_ptr(load), d, size);
    memset(guest_ptr(load + size), 0, bss);
    log_info("loader: overlay %u '%.32s' at 0x%08x", id, (const char *)d + 0x20, load);
    free(d);
    return (int)id;
}
