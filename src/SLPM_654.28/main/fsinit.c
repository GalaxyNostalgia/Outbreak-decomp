#include "cri.h"
#include "sce.h"

/* File system start-up: CRI ADX's file system on the DVD (or the host/HDD), the disc's file
   list, and the AFS partitions. The game's data lives in three big AFS archives on the disc,
   NETBIO00.DAT (rooms), NETBIO01.DAT (sound) and NETBIO02.DAT (movies); each holds smaller
   AFS archives, which become partitions of their own. */

typedef struct {
    const char *name;
    s32 nameLen;
    void *buf;
    s32 bufSize;
} FsListParam;

typedef struct {
    s32 kind;          /* 0x00 */
    s32 unk4;
    void *ptinfo;      /* 0x08 */
    const char **names;/* 0x0C */
    s32 unk10;
    s32 size;          /* 0x14 */
    s32 unk18;
    s32 unk1C;
    s32 ptid;          /* 0x20 */
} RoomPart;            /* 0x24 bytes */

void ADXPS2_SetupThrd(void *tprm);
void ADXPS2_SetupHstFs(void *prm);
void ADXPS2_SetupDvdFs(void *prm);
void ADXPS2_LoadFileListHst(FsListParam *prm);
s32 ADXPS2_LoadFileListDvd(FsListParam *prm);
void func_0012B360(void (*cb)(void), void *obj);   /* ADX user v-sync callback */
void func_0012C1E0(s32 mode);
s32 func_001A8300(const char *name);
void func_001CF5C0(void);

extern s32 D_0023BAF4;           /* the file system is up */
extern s32 D_0023BAF8;           /* 0: host file system, 1: DVD */
extern char D_00293200[0x100];   /* root of the game's files */
extern char D_00293300[0x10];    /* device prefix for the sound/movie archives */
extern char D_00293310[0x10];    /* device prefix for the room archive */
extern void *D_00293330[4];      /* host file system parameters */
extern void *D_00293350[5];      /* DVD file system parameters */
extern char D_00293370[2][0x10]; /* file list currently loaded, per file system */
extern FsListParam D_00293320;   /* file list parameters: host */
extern FsListParam D_00293340;   /* file list parameters: DVD */
extern char D_00293390[2][0x100];/* root, per file system */
extern void *D_00239F20[2];      /* file list buffers */
extern s32 D_00239F28[2];        /* file list buffer sizes */

extern char D_002241D0[];   /* "Setup HOST file system.\n" */
extern char D_002241F0[];   /* "HST:" */
extern char D_00224200[];   /* "Setup DVD file system.\n" */
extern char D_00224218[];   /* "" */
extern char D_00224220[];   /* "pfs1:\\0flist.dir" */
extern char D_00224238[];   /* "0flist.dir" */
extern char D_00224248[];   /* "%s\\r%03d.dir" */
extern char D_00224258[];   /* "r%03d.dir" */
extern char D_00224520[];   /* "%snetbio00.dat" */
extern char D_00224530[];   /* "%snetbio01.dat" */
extern char D_00224540[];   /* "%snetbio02.dat" */

void func_001A7DF0(const char *host, const char *dvd) {
    D_00293310[0] = 0;
    D_0023BAF8 = 0;
    D_00293300[0] = 0;
    D_00293200[0] = 0;
    D_00293390[0][0] = 0;
    D_00293390[1][0] = 0;
    D_00293370[0][0] = 0;
    D_00293370[1][0] = 0;
    if (host != NULL) {
        printf(D_002241D0);
        memset(D_00293330, 0, sizeof(D_00293330));
        D_00293330[0] = (void *)host;
        ADXPS2_SetupHstFs(D_00293330);
        strcpy(D_00293390[0], host);
        D_0023BAF8 = 0;
        strcpy(D_00293310, D_002241F0);
        strcpy(D_00293300, D_002241F0);
        strcpy(D_00293200, host);
    }
    if (dvd != NULL) {
        printf(D_00224200);
        memset(D_00293350, 0, sizeof(D_00293350));
        D_00293350[0] = (void *)dvd;
        ADXPS2_SetupDvdFs(D_00293350);
        strcpy(D_00293390[1], dvd);
        D_0023BAF8 = 1;
        strcpy(D_00293310, D_00224218);
        strcpy(D_00293300, D_00224218);
        strcpy(D_00293200, dvd);
    }
    ADXPS2_SetupThrd(NULL);
    D_0023BAF4 = 1;
    func_0012C1E0(2);
}

void func_001A80C0(void) {
    if (D_00293370[0][0] != 0) {
        ADXPS2_LoadFileListHst(NULL);
        D_00293370[0][0] = 0;
    }
}

void func_001A8100(void) {
    if (D_00293370[1][0] != 0) {
        ADXPS2_LoadFileListDvd(NULL);
        D_00293370[1][0] = 0;
    }
}

/* Load the file list of scenario `n` (-1: the disc's own list, -2: the HDD install's). Returns 1 if
   a new list was loaded, 0 if it was already the current one. */
s32 func_001A8140(s32 n) {
    char name[0x100];
    FsListParam *prm;

    if (D_0023BAF8 == 0) {
        prm = &D_00293320;
    } else {
        prm = &D_00293340;
    }
again:
    if (n == -2) {
        strcpy(name, D_00224220);
    } else if (n == -1) {
        strcpy(name, D_00224238);
    } else {
        sprintf(name, D_00224248, D_00293390[D_0023BAF8], n);
        if (func_001A8300(name) == 0) {
            n = -1;
            goto again;
        }
        sprintf(name, D_00224258, n);
    }
    if (strcmp(D_00293370[D_0023BAF8], name) == 0) {
        return 0;
    }
    if (D_0023BAF8 == 0) {
        func_001A80C0();
    } else {
        func_001A8100();
    }
    memset(prm, 0, sizeof(*prm));
    prm->name = name;
    prm->nameLen = 0x20;
    prm->bufSize = D_00239F28[D_0023BAF8];
    prm->buf = D_00239F20[D_0023BAF8];
    if (D_0023BAF8 == 0) {
        ADXPS2_LoadFileListHst(prm);
    } else {
        ADXPS2_LoadFileListDvd(prm);
    }
    strcpy(D_00293370[D_0023BAF8], name);
    return 1;
}

extern u8 D_002958B0[];          /* partition 10: NETBIO00.DAT */
extern u8 D_00295760[];          /* partition 13: NETBIO01.DAT */
extern u8 D_00295620[];          /* partition 14: NETBIO02.DAT */
extern RoomPart D_002955D0[2];
extern RoomPart *D_0023BAFC;
extern RoomPart *D_0023BB00;
void func_001A9450(void);

void func_001A8BD0(void) {
    char path[0x100];

    sprintf(path, D_00224520, D_00293310);
    ADXF_LoadPartitionNw(10, path, NULL, D_002958B0);
    do {
    } while (ADXF_GetPtStat(10) != 3);
    sprintf(path, D_00224530, D_00293300);
    ADXF_LoadPartitionNw(13, path, NULL, D_00295760);
    do {
    } while (ADXF_GetPtStat(13) != 3);
    sprintf(path, D_00224540, D_00293300);
    ADXF_LoadPartitionNw(14, path, NULL, D_00295620);
    do {
    } while (ADXF_GetPtStat(14) != 3);
    memset(D_002955D0, 0, sizeof(D_002955D0));
    D_0023BAFC = &D_002955D0[0];
    D_0023BB00 = &D_002955D0[1];
    D_0023BAFC->kind = 0;
    D_0023BAFC->ptid = 11;
    D_0023BB00->kind = 1;
    D_0023BB00->ptid = 12;
    func_001A9450();
}

/* Index of an archive in one of the name tables ("R001.AFS" -> "R001_AFS"), -1 if not there. */
extern const char *D_00214760[];  /* NETBIO00.DAT: ROMDATA_AFS, R001_AFS, R001_H_BIN, ... */
extern const char *D_002147A0[];  /* NETBIO01.DAT: ADXSND_AFS, BGM_AFS, ... */
extern const char *D_002147D0[];  /* NETBIO02.DAT: MOVIE001_AFS, ... */

s32 func_001A8D30(const char *path) {
    char name[0x20];
    char *p;
    s32 i;

    p = strrchr(path, '/');
    if (p == NULL) {
        p = strrchr(path, '\\');
    }
    if (p != NULL) {
        p = p + 1;
    } else {
        p = (char *)path;
    }
    strcpy(name, p);
    if (strlen(name) == 0) {
        return -1;
    }
    strupr(name);
    p = strrchr(name, '.');
    if (p != NULL) {
        *p = '_';
    }
    for (i = 0; D_00214760[i] != NULL; i++) {
        if (strcmp(name, D_00214760[i]) == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_001A8EF0(const char *path) {
    char name[0x20];
    char *p;
    s32 i;

    p = strrchr(path, '/');
    if (p == NULL) {
        p = strrchr(path, '\\');
    }
    if (p != NULL) {
        p = p + 1;
    } else {
        p = (char *)path;
    }
    strcpy(name, p);
    if (strlen(name) == 0) {
        return -1;
    }
    strupr(name);
    p = strrchr(name, '.');
    if (p != NULL) {
        *p = '_';
    }
    for (i = 0; D_002147A0[i] != NULL; i++) {
        if (strcmp(name, D_002147A0[i]) == 0) {
            return i;
        }
    }
    return -1;
}

s32 func_001A8FD0(const char *path) {
    char name[0x20];
    char *p;
    s32 i;

    p = strrchr(path, '/');
    if (p == NULL) {
        p = strrchr(path, '\\');
    }
    if (p != NULL) {
        p = p + 1;
    } else {
        p = (char *)path;
    }
    strcpy(name, p);
    if (strlen(name) == 0) {
        return -1;
    }
    strupr(name);
    p = strrchr(name, '.');
    if (p != NULL) {
        *p = '_';
    }
    for (i = 0; D_002147D0[i] != NULL; i++) {
        if (strcmp(name, D_002147D0[i]) == 0) {
            return i;
        }
    }
    return -1;
}

/* Make partition `ptid` out of the archive `name` inside NETBIO0x.DAT (`afs` 0, 1 or 2) and wait
   for it. */
void func_001A8E10(const char *name, void *ptinfo, s32 ptid, s32 afs) {
    s32 fid;

    switch (afs) {
    case 0:
        fid = func_001A8D30(name);
        if (fid < 0) {
            return;
        }
        ADXF_LoadPtFromAfsNw(ptid, 10, fid, ptinfo);
        break;
    case 1:
        fid = func_001A8EF0(name);
        if (fid < 0) {
            return;
        }
        ADXF_LoadPtFromAfsNw(ptid, 13, fid, ptinfo);
        break;
    case 2:
        fid = func_001A8FD0(name);
        if (fid < 0) {
            return;
        }
        ADXF_LoadPtFromAfsNw(ptid, 14, fid, ptinfo);
        break;
    }
    do {
    } while (ADXF_GetPtStat(ptid) != 3);
}

void func_001A90B0(const char *name, RoomPart *part, s32 unused) {
    func_001A8E10(name, part->ptinfo, part->ptid, 0);
}

extern void *D_00306618[];       /* ROMDATA.AFS partition info buffer */
extern const char *D_002147F0[];
extern char D_0022ECD0[];        /* "romdata.afs" */
void func_001AA690(void);
void func_001AA5A0(void);

void func_001A9450(void) {
    if (D_0023BAFC == NULL) {
        while (1) {
        }
    }
    if (D_00306618[0] == NULL) {
        while (1) {
        }
    }
    D_0023BAFC->ptinfo = D_00306618[0];
    D_0023BAFC->unk1C = 0;
    D_0023BAFC->names = D_002147F0;
    D_0023BAFC->unk18 = 0;
    D_0023BAFC->size = 0x540;
    func_001A90B0(D_0022ECD0, D_0023BAFC, 0x6000);
    func_001AA690();
    func_001AA5A0();
}

/* The two sound archives that stay loaded: ADXSND.AFS and BGM.AFS (partitions 0 and 1). */
extern const char *D_00239F40[2];
extern s32 D_00239F48[2];        /* number of files in each */
extern u8 D_002939D0[];

void func_001AA690(void) {
    s32 i = 0;
    u8 *ptinfo = D_002939D0;

    do {
        func_001A8E10(D_00239F40[i], ptinfo, i, 1);
        ptinfo += ((D_00239F48[i] + 1) * 4 + 0x11C) & ~3;
        i++;
    } while (i < 2);
}

extern char D_002319C0[];   /* "adxsnd.afs" */
extern u8 D_00294E20[];

void func_001AA5A0(void) {
    func_001A8E10(D_002319C0, D_00294E20, 3, 1);
}

void func_001AA7C0(void) {
}

void func_001AB8F0(void) {
}

extern char D_00231AE0[];   /* "\\" */
extern u8 D_003065A7[];

void func_001AB900(void) {
    func_001A7DF0(NULL, D_00231AE0);
    func_001CF5C0();
    ADXT_Init();
    func_0012B360(func_001AB8F0, NULL);
    D_003065A7[0] = 1;
    func_001A8140(-1);
    func_001A8BD0();
    func_001AA7C0();
}
