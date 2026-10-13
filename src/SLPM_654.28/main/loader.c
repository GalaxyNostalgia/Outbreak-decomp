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

/* A character's model file (the release: *.NBD in ROMDATA.AFS): its textures (LZ packed, ids
   0x1000 + 8 * kind, 0x1100 for the high detail), model, motion and what it holds. */
#include "object.h"
#include "model.h"

typedef struct {
    s32 unk0;
    s32 tex;            /* 0x04: offset of the textures ({size, LZ data}...) */
    s32 unk8;
    s32 ntex;           /* 0x0C */
    s32 unk10;
    s32 model;          /* 0x14 */
    s32 unk18[3];
    s32 motion;         /* 0x24 */
    s32 unk28[7];
    s32 unk44;          /* 0x44: 0x1324 bytes for D_0023BCD0 */
    s32 unk48[3];
    s32 held;           /* 0x54: the held item's model */
    s32 unk58[10];
} NbdHeader;

extern u8 *D_003065DC;          /* (Options.unk4C) work memory */
extern u8 *D_00306640;          /* (Options) */
extern u8 *D_0023BCD0;
extern char D_0022ED60[];   /* "%sPL\\HP%02d.NBD" */
extern char D_0022ED70[];   /* "COMMON\\" */
extern char D_0022ED80[];   /* "%sNPC\\N%02d.NBD" */
extern char D_0022ED90[];   /* "%sNPC\\N%d.NBD" */
extern char D_0022EDA0[];   /* "%sPL\\P%02d_%02d.NBD" */
extern char D_0022EDC0[];   /* "%sPL\\EP%02d_%02d.NBD" */
extern Slot1468 D_0047F4D0[];
extern u8 D_003BB520[][0x824];
void func_001AE810(void *src, void *dst);
s32 func_001D0250(u32 tim, s32 id);
void func_001D0440(s32 base);
void func_0018D730(void);
void func_0018D630(void *dst, void *src, s32 size);
void func_001D35E0(DrawObjFull *obj, u8 kind, u8 costume);
void func_001D3770(DrawObjFull *obj, u8 kind);
s32 func_001D2720(Slot1468 *m, s32 mode, void *model, void *motion, s32 a);
void func_00638C00(DrawObjFull *obj, u8 kind);

void func_001A9A10(s32 slot, DrawObjFull *obj);
#ifdef NON_MATCHING   /* first draft */
void func_001A9A10(s32 slot, DrawObjFull *obj) {
    char name[0x100];
    NbdHeader hdr;
    u8 *buf;
    u8 *p;
    u8 *out;
    s32 id;
    s32 i;
    s32 size;
    s32 n;
    u8 *held;

    if (obj->kind >= 0x14) {
        ((Slot1468 *)obj->model)->unk1460 = NULL;
        ((Slot1468 *)obj->model)->unk1464 = NULL;
    }
    buf = D_003065DC;
    if (obj->unkBB4 != 0) {
        if (obj->unk54D == 1) {
            sprintf(name, D_0022ED60, D_0022ED70, obj->unkB98);
        } else {
            n = obj->unkBB4 - 1;
            if (n < 10) {
                sprintf(name, D_0022ED80, D_0022ED70, n);
            } else {
                sprintf(name, D_0022ED90, D_0022ED70, n);
            }
        }
    } else {
        switch (obj->unk54D) {
        case 1:
            sprintf(name, D_0022ED60, D_0022ED70, obj->unkB98);
            break;
        case 0:
            sprintf(name, D_0022EDA0, D_0022ED70, obj->unkB98, obj->costume);
            break;
        default:
            sprintf(name, D_0022EDC0, D_0022ED70, obj->unkB98, obj->costume);
            break;
        }
    }
    if (func_001A9780(name, buf) <= 0) {
        return;
    }
    memcpy(&hdr, buf, sizeof(hdr));
    if (obj->unk54D == 1) {
        id = slot * 8 + 0x1100;
    } else {
        id = slot * 8 + 0x1000;
    }
    func_001D0440(id);
    if (hdr.tex == 0) {
        return;
    }
    p = buf + hdr.tex;
    out = D_003065DC + 0x180000;
    for (i = 0; i < hdr.ntex; i++) {
        size = *(s32 *)p;
        func_001AE810(p + 4, out);
        func_001D0250((u32)out, id);
        id++;
        p = p + 4 + size;
    }
    func_0018D730();
    if (hdr.model == 0 || hdr.motion == 0) {
        return;
    }
    func_001D35E0(obj, obj->unkB98, obj->costume);
    func_001D2720(&D_0047F4D0[obj->kind], 2, buf + hdr.model, buf + hdr.motion, 0);
    func_0018D730();
    func_00638C00(obj, obj->unkB98);
    if (obj->unkBB4 != 0) {
        return;
    }
    D_0023BCD0 = D_00306640;
    if (D_0023BCD0 != NULL && hdr.unk44 != 0) {
        p = D_0023BCD0 + obj->kind * 0x1324;
        func_0018D630(p, buf + hdr.unk44, 0x1324);
        ((Slot1468 *)obj->model)->unk1460 = p;
        ((Slot1468 *)obj->model)->unk1464 = p + 0x74;
    }
    if (obj->unk54D == 1 && hdr.held != 0) {
        func_001D3770(obj, obj->unkB98);
        held = D_003BB520[obj->kind];
        if (held[0] == 1) {
            func_001D2720((Slot1468 *)(held + 4), 2, buf + hdr.held, NULL, 0);
        }
    }
}
#endif

/* A model file (*.NBD) by name (from the room's archive with 0x10000 in flags, else COMMON\ in
   ROMDATA.AFS), or with no name the one texture id's list names; textures from id on. */
extern char D_0022EDD8[];   /* ".NBD" */
extern char D_0022EDE0[];   /* "%s.NBD" */
extern char D_0022EDE8[];   /* "%s%s.NBD" */
s32 func_001D0450(void);
char *func_001D0960(s32 id, u32 arg, s32 kind, s32 alt);
s32 func_001A96E0(const char *name, void *buf);
char *strcpy(char *dst, const char *src);
char *strcat(char *dst, const char *src);
char *strrchr(const char *s, s32 c);

void func_001A9DA0(const char *name, Slot1468 *m, s32 id, u32 flags);
#ifdef NON_MATCHING   /* first draft */
void func_001A9DA0(const char *name, Slot1468 *m, s32 id, u32 flags) {
    char path[0x100];
    NbdHeader hdr;
    u8 *buf = D_003065DC;
    u8 *p;
    u8 *out;
    char *s;
    s32 size;
    s32 n;
    s32 i;

    if (name != NULL) {
        if (id == -1) {
            return;
        }
        if (flags & 0x10000) {
            sprintf(path, D_0022EDE0, name, name);
            n = func_001A96E0(path, buf);
        } else {
            sprintf(path, D_0022EDE8, D_0022ED70, name);
            n = func_001A9780(path, buf);
        }
        if (n <= 0) {
            return;
        }
    } else {
        n = id;
        id = func_001D0450();
        if (id == -1) {
            return;
        }
        s = func_001D0960(id, n, 0, 0);
        if (s == NULL || strlen(s) == 0) {
            return;
        }
        s = strrchr(s, '\\\\');
        if (s == NULL) {
            return;
        }
        strcpy(path, s + 1);
        s = strrchr(path, '.');
        if (s != NULL) {
            *s = 0;
        }
        strcat(path, D_0022EDD8);
        path[0x10] = 0;
        if (func_001A96E0(path, buf) <= 0) {
            return;
        }
    }
    memcpy(&hdr, buf, 0x30);
    func_001D0440(id);
    if (hdr.tex == 0) {
        return;
    }
    p = buf + hdr.tex;
    out = D_003065DC + 0x180000;
    for (i = 0; i < hdr.ntex; i++) {
        size = *(s32 *)p;
        func_001AE810(p + 4, out);
        func_001D0250((u32)out, id);
        id++;
        p = p + 4 + size;
    }
    func_0018D730();
    if (hdr.model != 0 && hdr.motion != 0) {
        func_001D2720(m, flags & 0x7FFFFFFF, buf + hdr.model, buf + hdr.motion, 0);
        func_0018D730();
    }
}
#endif
