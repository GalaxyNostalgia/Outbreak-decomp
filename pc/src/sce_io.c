/* Sony libkernel file I/O (sceOpen/sceRead/sceLseek/sceClose) for the PC port.
 *
 * "cdrom0:\PATH;1" and "pfs0:PATH" (the HDD install) read from the user's disc image;
 * "host:PATH" and "host0:PATH" read from the folder the game was started in.
 */
#include "iso9660.h"
#include "log.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MAX_FD 32
#define SCE_RDONLY 0x0001
#define SCE_SEEK_SET 0
#define SCE_SEEK_CUR 1
#define SCE_SEEK_END 2
#define SCE_ENOENT (-2)
#define SCE_EBADF (-9)
#define SCE_EMFILE (-24)

typedef struct {
    int used;
    int on_disc;
    IsoFile iso;
    FILE *host;
    uint32_t pos, size;
} Fd;

static Fd g_fd[MAX_FD];

static const char *strip_prefix(const char *name, int *on_disc) {
    static const char *disc[] = {"cdrom0:", "cdrom:", "pfs0:", "pfs1:"};
    static const char *host[] = {"host0:", "host:"};
    for (size_t i = 0; i < sizeof disc / sizeof *disc; i++)
        if (strncmp(name, disc[i], strlen(disc[i])) == 0) { *on_disc = 1; return name + strlen(disc[i]); }
    for (size_t i = 0; i < sizeof host / sizeof *host; i++)
        if (strncmp(name, host[i], strlen(host[i])) == 0) { *on_disc = 0; return name + strlen(host[i]); }
    return NULL;
}

int sceOpen(const char *name, int flags, ...) {
    int on_disc = 0, fd;
    const char *path = strip_prefix(name, &on_disc);
    if (!path) {
        log_warn("sceOpen(%s): unsupported device", name);
        return SCE_ENOENT;
    }
    for (fd = 0; fd < MAX_FD && g_fd[fd].used; fd++) {}
    if (fd == MAX_FD) return SCE_EMFILE;
    Fd *f = &g_fd[fd];
    memset(f, 0, sizeof *f);
    if (on_disc) {
        if ((flags & 3) != SCE_RDONLY || iso_find(path, &f->iso) != 0) return SCE_ENOENT;
        f->size = f->iso.size;
    } else {
        char local[512];
        snprintf(local, sizeof local, "%s", path);
        for (char *c = local; *c; c++) if (*c == '\\') *c = '/';
        f->host = fopen(local, (flags & 3) == SCE_RDONLY ? "rb" : "r+b");
        if (!f->host) return SCE_ENOENT;
        fseek(f->host, 0, SEEK_END);
        f->size = (uint32_t)ftell(f->host);
        fseek(f->host, 0, SEEK_SET);
    }
    f->used = 1;
    f->on_disc = on_disc;
    return fd;
}

int sceClose(int fd) {
    if (fd < 0 || fd >= MAX_FD || !g_fd[fd].used) return SCE_EBADF;
    if (g_fd[fd].host) fclose(g_fd[fd].host);
    g_fd[fd].used = 0;
    return 0;
}

int sceLseek(int fd, int offset, int whence) {
    if (fd < 0 || fd >= MAX_FD || !g_fd[fd].used) return SCE_EBADF;
    Fd *f = &g_fd[fd];
    int64_t base = whence == SCE_SEEK_SET ? 0 : whence == SCE_SEEK_CUR ? f->pos : f->size;
    int64_t pos = base + offset;
    if (pos < 0) return -22;
    f->pos = (uint32_t)pos;
    return (int)f->pos;
}

int sceRead(int fd, void *buf, int size) {
    if (fd < 0 || fd >= MAX_FD || !g_fd[fd].used || size < 0) return SCE_EBADF;
    Fd *f = &g_fd[fd];
    int64_t n;
    if (f->on_disc) {
        n = iso_read(&f->iso, f->pos, buf, (uint32_t)size);
    } else {
        fseek(f->host, (long)f->pos, SEEK_SET);
        n = (int64_t)fread(buf, 1, (size_t)size, f->host);
    }
    if (n < 0) return -5;
    f->pos += (uint32_t)n;
    return (int)n;
}
