#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* The scenario's record points (the "events" a room's table lists: conditions met once give
   points and a log entry with the time). */

typedef struct {
    u8 cond[2];         /* 0x0: both must hold */
    u16 arg[2];         /* 0x2 */
    u16 id;             /* 0x6 */
    u16 points;         /* 0x8 */
} RoomEvent;            /* 0xA */

typedef struct {
    u8 unk0[0x30];
} RoomEventSet;

extern s8 D_00359AC0[];         /* per room: its event table */
extern RoomEvent *D_00359AF0[][2];
extern u8 D_0035D430[];         /* each event done */
extern u8 D_0048BBE0[];         /* per item: who has it (player + 1) */
extern u32 D_0048AD90[];
extern u32 D_0048AC10[];        /* the scene flags */
extern s16 D_0048BDE0;
extern DrawObjFull D_00476DD0[];
s32 func_005409B0(u16 id);
u8 func_00634640(DrawObjFull *p, s32 a);

#define BIT(a, n) ((a)[(n) >> 5] & (1 << ((n) & 0x1F)))

s32 func_0034B410(RoomEvent *e, u8 cond);
#ifdef NON_MATCHING   /* first draft */
s32 func_0034B410(RoomEvent *e, u8 cond) {
    u8 me;

    switch (cond) {
    case 0:
        return 1;
    case 2:
        if (D_0048BBE0[e->arg[0]] == D_0048BF60.unk1D + 1) {
            return 1;
        }
        return 0;
    case 3:
        if (BIT(D_0048AD90, e->arg[0])) {
            return 1;
        }
        break;
    case 4:
        if (BIT(D_0048AC10, e->arg[0])) {
            return 1;
        }
        break;
    case 5:
        if (func_005409B0(e->arg[0]) != 0) {
            return 1;
        }
        break;
    case 6:
        if (D_0048BDE0 == e->arg[0]) {
            return 1;
        }
        break;
    case 7:
        me = D_0048BF60.unk1D + 1;
        if (D_0048BBE0[e->arg[0]] == me && D_0048BBE0[e->arg[1]] == me) {
            return 1;
        }
        break;
    case 11:
        if (D_0048BF60.unkBA == 3) {
            return 1;
        }
        break;
    case 12:
        me = D_0048BF60.unk1D + 1;
        if (D_0048BBE0[e->arg[0]] == me || D_0048BBE0[e->arg[1]] == me) {
            return 1;
        }
        break;
    case 13:
        if (BIT(D_0048AC10, e->arg[0]) && BIT(D_0048AC10, e->arg[1])) {
            return 1;
        }
        break;
    case 14:
        if (BIT(D_0048AD90, e->arg[0]) && BIT(D_0048AD90, e->arg[1])) {
            return 1;
        }
        break;
    case 15:
        if (func_00634640(&D_00476DD0[D_0048BF60.unk1D], 0) == 0 &&
            !(D_00476DD0[D_0048BF60.unk1D].unkBA0 & 0x80)) {
            return 1;
        }
        break;
    }
    return 0;
}
#endif

/* The record log: nodes of 0x18 bytes from a pool of 0x80. */
typedef struct RecordNode {
    struct RecordNode *next;    /* 0x00 */
    struct RecordNode *prev;    /* 0x04 */
    u32 time;           /* 0x08: frames played */
    s16 id;             /* 0x0C */
    s8 used;            /* 0x0E */
    s8 level;           /* 0x0F */
    u8 unk10[8];
} RecordNode;

extern RecordNode *D_0035E070[0x80];    /* the free ones */
extern s16 D_0023BD30;          /* free */
extern RecordNode *D_0023BD34;  /* last */
extern RecordNode *D_0023BD38;  /* first */
extern s8 D_0023BD2C;
void *memset(void *p, s32 c, u32 n);
s32 func_0034B9F0(RecordNode *n);

RecordNode *func_0034B950(void) {
    RecordNode *n;

    if (D_0023BD30 == 0) {
        return NULL;
    }
    D_0023BD30--;
    n = D_0035E070[D_0023BD30];
    memset(n, 0, sizeof(RecordNode));
    if (D_0023BD38 == NULL) {
        D_0023BD34 = n;
        D_0023BD38 = n;
        n->prev = NULL;
        n->next = NULL;
    } else {
        n->next = NULL;
        n->prev = D_0023BD34;
        D_0023BD34->next = n;
        D_0023BD34 = n;
    }
    return n;
}

/* Makes room in the log by dropping the oldest entries of the lowest levels (up to 9). */
s32 func_0034BAC0(void);
#ifdef NON_MATCHING   /* first draft */
s32 func_0034BAC0(void) {
    RecordNode *n;
    RecordNode *next;

    if (D_0023BD2C >= 9) {
        return 0;
    }
    if (D_0023BD30 == 0x80) {
        return 0;
    }
    n = D_0023BD38;
    while (n != NULL) {
        if (D_0023BD2C >= n->level) {
            next = n->next;
            n = next;
            if (func_0034B9F0(n) == 0) {
                return 0;
            }
        } else {
            n = n->next;
        }
    }
    D_0023BD2C++;
    return 1;
}
#endif

/* Logs event id now. */
void func_0034B720(u16 id);
#ifdef NON_MATCHING   /* first draft */
void func_0034B720(u16 id) {
    RecordNode *n;
    u32 t;

    for (;;) {
        if (D_0023BD30 != 0) {
            n = func_0034B950();
            if (n != NULL) {
                n->used = 1;
                n->time = D_0048BF60.unk18;
                t = n->time;
                if (t > 0xA4C478) {
                    t = 0xA4C478;
                }
                n->time = t;
                n->id = id;
                n->level = 9;
            }
            return;
        }
        if (D_0023BD2C >= 9 || func_0034BAC0() == 0) {
            return;
        }
    }
}
#endif

extern u32 D_00389698[];        /* events done */
extern s32 D_003896A0[];        /* [0]: points */

void func_0034BB70(RoomEvent *e) {
    D_00389698[e->id >> 5] |= 1 << (e->id & 0x1F);
    D_003896A0[0] += e->points;
}

/* Once a frame: the room's events whose conditions hold now. */
void func_0034B260(void) {
    RoomEvent *e = D_00359AF0[D_00359AC0[D_00306590.unk1A]][0];
    u8 *done;
    s32 i;

    for (i = 0; e->cond[0] != 0xFF; e++, i++) {
        done = &D_0035D430[i];
        if (*done == 0 && func_0034B410(e, e->cond[0]) == 1 && func_0034B410(e, e->cond[1]) == 1) {
            func_0034BB70(e);
            *done = 1;
            func_0034B720(e->id);
        }
    }
}
