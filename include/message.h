#ifndef MESSAGE_H
#define MESSAGE_H

#include "common.h"

/* The in-game message queue: scripts and the game queue messages (func_005BF4C0), the game task
   shows them (func_005BF6E0). 16 entries, read at D_0023BD7C, written at D_0023BD78. */
typedef struct {
    u16 v[8];           /* kind (1 talk, 3 an item's, ...), the text (two bytes), who says it, the item */
} EventMsg;

extern EventMsg D_003839B0[16];
extern s16 D_0023BD78;          /* write position */
extern s16 D_0023BD7C;          /* read position */

/* A message's set of positions (0x8C bytes). */
typedef struct {
    s8 type;            /* 0x00: 0-2 its moves play */
    u8 unk1[3];
    s32 id;             /* 0x04: (its character, a DrawObjFull *) */
    s32 n;              /* 0x08 */
    u16 a[16][2];       /* 0x0C */
    u16 b[16][2];       /* 0x4C */
} MsgSet;

/* The message being shown (inside a 0xB10-byte work area). */
typedef struct {
    s8 step;            /* 0x00 */
    s8 sub;             /* 0x01 */
    s8 sub2;            /* 0x02 */
    s8 sub3;            /* 0x03 */
    s32 done;           /* 0x04 */
    s32 frames;         /* 0x08: time the message has been up, in 60ths of a second */
    u8 unkC[4];
    s32 count;          /* 0x10: entries in the loaded messages (func_005BD280's counts[0]) */
    s16 unk14;
    s16 unk16;
    s16 unk18;
    s16 n2;             /* 0x1A: counts[2] */
    s16 unk1C;
    s16 n3;             /* 0x1E: counts[3] */
    s16 unk20;
    s16 unk22;
    s8 scene;           /* 0x24: the scene now (D_0023BD60's entries set it) */
    s8 ended;           /* 0x25: a bit per move list ended; 3 both */
    u8 unk26[2];
    u8 room2[3];        /* 0x28: the room it plays in */
    u8 room[3];         /* 0x2B: the room it's shown in (Options.unk1A, unk1C, unk1E) */
    s8 unk2E;           /* 0x2E */
    u8 kind;            /* 0x2F */
    u8 a;               /* 0x30: the text */
    u8 b;               /* 0x31 */
    u8 who;             /* 0x32: the speaker's character */
    u8 unk33;
    s32 who2;           /* 0x34 */
    u8 unk38[4];
    s32 owner;          /* 0x3C: who has the item (player + 1), then its place in the list + 1 */
    s32 item;           /* 0x40 */
    u8 unk44[4];
    s32 status[0x28];   /* 0x48: the listed players' status (unkBA0) */
    u8 pl[8];           /* 0xE8: the listed players' numbers */
    u8 chr[8];          /* 0xF0: the listed players' characters */
    u8 unkF8[0x12C - 0xF8];
    s32 n;              /* 0x12C: players listed */
    void *objs;         /* 0x130: the message's 8 characters (DrawObjFull, in the work area) */
    void *things;       /* 0x134: its 16 things (0x11A0 bytes each) */
    MsgSet sets[16];    /* 0x138 */
    u8 unkAF8[0xB10 - 0xAF8];
} MsgWork;

extern MsgWork D_0035FFD0;

/* The loaded messages' first table (0x18 bytes an entry, 0xFFFF time ends it): cues by time. */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02: when (MsgWork.frames) */
    s16 unk4;
    s16 fx;             /* 0x06: the room dm file's entry */
    s32 dur;            /* 0x08 */
    u8 unkC[0xC];
} MsgCue;

extern MsgCue *D_0023BD74;      /* the next cue (NULL: none) */

/* The loaded messages' second table (0x28 bytes an entry): things to do at a time. */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02: when (MsgWork.frames); 0xFFFF once done */
    s32 kind;           /* 0x04: 0 nothing, 1 set the scene flags below, 2 the backdrop */
    u16 bits[8];        /* 0x08: (1) 8 x 16 flags */
    u16 a[2];           /* 0x18: (1) two words */
    u16 b[2];           /* 0x1C */
    u8 unk20[8];
} MsgAction;

extern MsgAction *D_0023BD6C;

/* The third table (0x28 bytes an entry): pad vibrations at a time. */
typedef struct {
    u16 unk0;
    u16 time;           /* 0x02: when; 0xFFFF once done */
    s32 arg;            /* 0x04: the vibration (func_001B9060) */
    u8 unk8[0x20];
} MsgCall;

extern MsgCall *D_0023BD68;

void func_005BF4C0(EventMsg *m);

#endif
