#ifndef MCJOB_H
#define MCJOB_H

#include "common.h"

/* Memory cards: Sony's libmc2, and the game's save code on top of it. */

typedef struct {
    s32 type;           /* 2: memory card */
    s32 port;
    s32 slot;
} sceMc2SocketParam;

typedef struct {
    s32 type;           /* 2: a PS2 memory card */
    s32 formatted;
    s32 freeClusters;
} McInfo;

typedef struct {
    u8 created[8];
    u8 modified[8];
    u32 size;           /* 0x10 */
    u16 attr;
    u16 unk16;
    u32 unk18;
    u32 unk1C;
    char name[32];
} McDirEntry;

s32 sceMc2GetInfoAsync(s32 socket, McInfo *info);
s32 sceMc2SearchFileAsync(s32 socket, const char *name, McDirEntry *ent);
s32 sceMc2ReadFileAsync(s32 socket, const char *name, void *buf, s32 offset, s32 size);
s32 sceMc2WriteFileAsync(s32 socket, const char *name, const void *buf, s32 offset, s32 size);
s32 sceMc2CreateFileAsync(s32 socket, const char *name);
s32 sceMc2DeleteAsync(s32 socket, const char *name);
s32 sceMc2GetDirAsync(s32 socket, const char *name, s32 offset, s32 max, McDirEntry *ents, s32 *count);
s32 sceMc2MkdirAsync(s32 socket, const char *name);
s32 sceMc2ChdirAsync(s32 socket, const char *name, char *cur);
s32 sceMc2ChmodAsync(s32 socket, const char *name, s32 mode);
s32 sceMc2RenameAsync(s32 socket, const char *from, const char *to);
s32 sceMc2GetEntSpaceAsync(s32 socket, const char *dir);
s32 sceMc2FormatAsync(s32 socket);
s32 sceMc2UnformatAsync(s32 socket);
/* 1 when a command has finished (its number and result filled in), 0 while one runs, -1 none */
s32 sceMc2CheckAsync(s32 *cmd, s32 *result);
s32 sceMc2Init(s32 mode);
s32 sceMc2CreateSocket(sceMc2SocketParam *param, void *buffer);

/* The memory card job: an operation on the save, run a step per frame by func_001BE6F0. */
typedef struct {
    s32 step;           /* 0x00 */
    s32 sub;            /* 0x04 */
    u8 unk8[8];
    s32 ports;          /* 0x10: memory card ports in use */
    s32 port;           /* 0x14 */
    s32 mcCmd;          /* 0x18: from sceMc2CheckAsync */
    s32 mcResult;       /* 0x1C */
    s32 retries;        /* 0x20 */
    s32 result;         /* 0x24: -1 while busy, 0 done, else an error */
    s32 card[3];        /* 0x28: per port: 0 none, 1 ok, 2 new card, 3 unformatted */
    s32 free[3];        /* 0x34: free clusters */
    s32 newCards;       /* 0x40: bit per port, cleared by func_001BF5E0 */
    char path[0x44];    /* 0x44: "<dir>/<file>" */
    s32 size;           /* 0x88 */
    s32 unk8C;
    s32 flags;          /* 0x90 */
    s32 unk94;
    s32 job;            /* 0x98: index into D_00217570 */
    s32 buf;            /* 0x9C */
    s32 layout;         /* 0xA0: index into D_00217560 */
    s32 unkA4;
    s32 unkA8;
} McJob;

extern McJob D_00308D80;

/* Save-data layouts: which files a save has and how many memory card clusters it needs. */
typedef struct {
    s32 used;
    char *name;
    s32 data;           /* where the file's contents are */
    s32 size;
} SaveFile;

typedef struct {
    char *title;
    char *unk4;
    s32 unk8;
    SaveFile files[5];  /* 0x0C: [0] the directory and its main file, then the icons */
    s32 blocks;         /* 0x5C: 1 KB clusters the save takes */
} SaveDesc;

extern SaveDesc *D_00217560[];      /* the save layouts */
extern s32 D_0023BBD8[2];           /* memory card sockets, per port */

/* Job results */
#define MC_BROKEN       -0x100  /* couldn't read it */
#define MC_NO_CARD      -0xFF
#define MC_UNFORMATTED  -0xFE
#define MC_NO_SAVE      -0xFD   /* and room to make one */
#define MC_NO_ROOM      -0xFC   /* no save and no room for one */
#define MC_BAD_SAVE     -0xFB   /* a save of the wrong size */

#endif
