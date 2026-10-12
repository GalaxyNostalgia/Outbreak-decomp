#include "cri.h"
#include "sce.h"
#include "fs.h"
#include "options.h"

/* Loading files by name from ROMDATA.AFS or the current room's archive. */

s32 sprintf(char *dst, const char *fmt, ...);
s32 strlen(const char *s);
char *strupr(char *s);
s32 func_001AA320(char **table, s32 n, const char *name);
s32 func_001A8D30(const char *path);
void func_001A90B0(const char *name, RoomPart *part, s32 unused);
s32 func_001AF500(void);
void func_822180(void);     /* NETAQ.BIN: keep the connection going while loading */

extern ADXF D_0023BB04;     /* file being read */

/* Is overlay `id` loaded in range `slot` (0: 0x540000, 1: 0x820000, 2: 0x6C0000)? */
s32 func_001A8AD0(s32 id, s32 slot) {
    switch (slot) {
    case 0:
        if (D_00306590.ovl54 == id) {
            return 1;
        }
        break;
    case 1:
        if (D_00306590.ovl82 == id) {
            return 1;
        }
        break;
    case 2:
        if (((u8 *)&D_00306590)[0xCA] == id) {
            return 1;
        }
        break;
    }
    return 0;
}

#ifdef NON_MATCHING   /* first draft */
/* A file's number in a partition, or -1. */
s32 func_001A90C0(RoomPart *pt, const char *name) {
    char tmp[0x10];
    char end;
    char base[0x100];
    char up[0x100];
    char *p;
    char *names;
    s32 i;

    strcpy(up, name);
    if (pt->kind == 0) {
        return func_001AA320(pt->names, pt->count, up);
    }
    strupr(up);
    p = strrchr(up, '/');
    if (p != NULL) {
        strcpy(base, p + 1);
    } else {
        p = strrchr(up, '\\');
        if (p == NULL) {
            strcpy(base, up);
        } else {
            strcpy(base, p + 1);
        }
    }
    p = base;
    if (strlen(base) == 0) {
        return -1;
    }
    for (; *p != 0; p++) {
        if (*p == '\\') {
            *p = '_';
        }
        if (*p == '/') {
            *p = '_';
        }
        if (*p == '.') {
            *p = '_';
        }
        if (*p == '-') {
            *p = '_';
        }
    }
    names = pt->names;
    for (i = 0; i < pt->count; i++, names += 0x10) {
        memcpy(tmp, names, 0x10);
        end = 0;
        if (strcmp(base, tmp) == 0) {
            return i + pt->base;
        }
    }
    return -1;
}
#endif

/* While a file loads online, keep the connection serviced */
void func_001AEF20(void) {
    s32 m;

    if (D_00306590.unk3A == 0) {
        return;
    }
    m = func_001AF500();
    switch (m) {
    case 1:
        func_822180();
        break;
    case 0:
    case 2:
    case 3:
        break;
    }
}

#ifdef NON_MATCHING   /* first draft */
/* Read a whole file into buf; returns its size (sectors * 2048), 0 if it isn't there. */
s32 func_001A9270(RoomPart *pt, const char *name, void *buf, s32 byNumber) {
    s32 online = func_001A8AD0(7, 1);
    s32 fid;
    s32 n;

    if (byNumber == 0) {
        fid = func_001A90C0(pt, name);
    } else {
        fid = (s32)name;
    }
    if (fid == -1) {
        return 0;
    }
    while ((D_0023BB04 = ADXF_OpenAfs(pt->ptid, fid)) == NULL) {
    }
    n = ADXF_GetFsizeSct(D_0023BB04);
    ADXF_ReadNw(D_0023BB04, n, buf);
    while (ADXF_GetStat(D_0023BB04) != 3) {
        if (online == 1) {
            func_001AEF20();
        }
    }
    ADXF_Close(D_0023BB04);
    D_0023BB04 = NULL;
    return n << 11;
}
#endif

extern char D_0022ECE0[];   /* "rfff/rfff.afs" */
extern char D_0022ECF0[];   /* "%sr%03d/r%03d.afs" */
extern char D_0022ED08[];   /* "scenario/" */
extern char D_0022ED18[];   /* "RFFF_H.BIN" */
extern char D_0022ED28[];   /* "R%03d_H.BIN" */

#ifdef NON_MATCHING   /* first draft */
/* Make `room`'s archive the room partition (0xFFFF: the common room). */
void func_001A9510(u32 room) {
    char path[0x100];
    s32 fid;
    s32 *h;

    if (D_0023BB00 == NULL || (D_0023BB00->ptinfo != NULL && D_0023BB00->room == room)) {
        return;
    }
    if (D_00306590.roomBuf == NULL) {
        while (1) {
        }
    }
    D_0023BB00->ptinfo = D_00306590.roomBuf;
    D_0023BB00->names = D_00306590.roomBuf + 0x6000;
    if (room == 0xFFFF) {
        strcpy(path, D_0022ECE0);
    } else {
        sprintf(path, D_0022ECF0, D_0022ED08, room, room);
    }
    func_001A90B0(path, D_0023BB00, 0x6000);
    D_0023BB00->room = room;
    if (room == 0xFFFF) {
        sprintf(path, D_0022ED18, room);
    } else {
        sprintf(path, D_0022ED28, room);
    }
    fid = func_001A8D30(path);
    if (fid < 0) {
        return;
    }
    while ((D_0023BB04 = ADXF_OpenAfs(10, fid)) == NULL) {
    }
    ADXF_ReadNw(D_0023BB04, ADXF_GetFsizeSct(D_0023BB04), D_0023BB00->names);
    while (ADXF_GetStat(D_0023BB04) != 3) {
    }
    ADXF_Close(D_0023BB04);
    D_0023BB04 = NULL;
    h = D_0023BB00->names;
    D_0023BB00->base = h[0];
    D_0023BB00->count = h[1];
    D_0023BB00->names = (u8 *)D_0023BB00->names + 8;
}
#endif

/* From the room's archive */
s32 func_001A96E0(const char *name, void *buf) {
    func_001A9510(D_00306590.unk1A);
    return func_001A9270(D_0023BB00, name, buf, 0);
}

/* From ROMDATA.AFS */
s32 func_001A9780(const char *name, void *buf) {
    return func_001A9270(D_0023BAFC, name, buf, 0);
}

#ifdef NON_MATCHING   /* first draft */
/* Load a file: flags 0x10000 from the room, else from ROMDATA.AFS. */
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 byNumber) {
    if (flags & 0x10000) {
        return func_001A96E0(name, buf);
    }
    if (flags & 0xFFFF) {
        return func_001A9780(name, buf);
    }
    return 0;
}
#endif
