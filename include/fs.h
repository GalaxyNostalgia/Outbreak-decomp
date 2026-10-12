#ifndef FS_H
#define FS_H

#include "common.h"

/* An AFS archive used as a partition: ROMDATA.AFS (the game's common files) or the current room's
   archive. Files are looked up by name in the archive's name table. */
typedef struct {
    s32 kind;           /* 0x00: 0 names are XOR-0xFF strings (func_001AA320), else 16-byte names */
    s32 unk4;
    void *ptinfo;       /* 0x08: the partition's table of contents */
    void *names;        /* 0x0C */
    s32 unk10;
    s32 count;          /* 0x14 */
    s32 base;           /* 0x18: file number of the first name */
    u32 room;           /* 0x1C: the room loaded (0xFFFF: the common one) */
    s32 ptid;           /* 0x20 */
} RoomPart;             /* 0x24 */

extern RoomPart *D_0023BAFC;    /* ROMDATA.AFS */
extern RoomPart *D_0023BB00;    /* the room */

#endif
