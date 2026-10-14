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

/* The message being shown (inside a 0xB10-byte work area). */
typedef struct {
    u8 unk0[8];
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
    u8 unk24[0x2E - 0x24];
    s8 unk2E;           /* 0x2E */
    u8 kind;            /* 0x2F */
    u8 a;               /* 0x30: the text */
    u8 b;               /* 0x31 */
    u8 who;             /* 0x32: the speaker's character */
    u8 unk33;
    s32 who2;           /* 0x34 */
    u8 unk38[8];
    s32 item;           /* 0x40 */
    u8 unk44[0xB10 - 0x44];
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
