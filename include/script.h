#ifndef SCRIPT_H
#define SCRIPT_H

#include "common.h"

/* The room script interpreter's state (0x48BE70; the script is r%03d.evb, at Options.script). */
typedef struct {
    u32 *pc[6];         /* 0x00: each thread's next instruction */
    s32 wait[6];        /* 0x18: frames to sleep */
    s32 entry[6];       /* 0x30: which routine of the script it runs */
    u32 *loop[6];       /* 0x48: where op 0x02 jumps back to (set by op 0x01) */
    u32 **base[6];      /* 0x60: the script's routine table */
    s32 unk78[6];       /* 0x78: how deep in if-blocks the thread is */
    s32 unk90[3][2];    /* 0x90 */
    s16 unkA8[8];       /* 0xA8: frames until sound unkB8 plays */
    u16 unkB8[8];       /* 0xB8: (0x8000: the other kind of sound) */
    s32 unkC8;          /* 0xC8 */
    s32 unkCC;
    s32 unkD0;
    s32 unkD4;
    u8 unkD8[8];        /* 0xD8 */
    u8 unkE0;
    s8 cur;             /* 0xE1: the thread running */
    u8 unkE2[0xE];
} ScriptVM;             /* 0xF0 */

extern ScriptVM D_0048BE70;

/* Per player: timers the script starts (ops 0x05, 0x06). */
typedef struct {
    u8 on[4];
    u8 arg[4];
    s16 count[4];
} ScriptTimers;

extern ScriptTimers D_0048BE24[4];

#define SCRIPT_LEN(insn) (1 << (((insn) & 0xFF000000) >> 24))

#endif
