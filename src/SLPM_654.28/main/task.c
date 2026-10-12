#include "sce.h"
#include "task.h"
#include "options.h"

/* The task slots and the code overlays (PROG/*.BIN) the tasks live in. */


void func_001B65D0(void (*fn)(Task *), s16 slot) {
    Task *t = &D_002F85D0[slot];

    memset(t, 0, sizeof(Task));
    t->state = TASK_RESUME;
    t->fn = fn;
}

void func_001B6630(void (*fn)(Task *), s16 slot) {
    Task *t = &D_002F85D0[slot];

    if (t->onKill != NULL) {
        t->onKill();
    }
    memset(t, 0, sizeof(Task));
    t->fn = fn;
    t->state = TASK_START;
}

void func_001B66A0(s16 slot) {
    Task *t = &D_002F85D0[slot];

    t->state = TASK_FREE;
    if (t->onKill != NULL) {
        t->onKill();
    }
    memset(t, 0, sizeof(Task));
}

/* Clear slots 0-11, telling the ones that are alive. */
void func_001B6700(void) {
    s32 i = 0;
    Task *t = D_002F85D0;

    do {
        if (i < 12) {
            switch (t->state) {
            case TASK_RUN:
            case 1:
            case TASK_WAIT:
                if (t->onKill != NULL) {
                    t->onKill();
                }
                break;
            }
            memset(t, 0, sizeof(Task));
        }
        i++;
        t++;
    } while (i < TASK_MAX);
}

void func_001B67A0(s16 slot, void (*onKill)(void)) {
    Task *t = &D_002F85D0[slot];

    t->onKill = onKill;
}

extern s8 D_00239F88;    /* overlay to load next frame (slot 0x540000), -1 none */
extern s8 D_00239F8C;    /* overlay to load next frame (slot 0x820000), -1 none */

void func_001B67C0(s8 ovl, s8 ovl2) {
    if (ovl != D_00306590.ovl54 && D_00239F88 != ovl) {
        D_00239F88 = ovl;
    }
    if (ovl2 != -1 && ovl2 != D_00306590.ovl82 && D_00239F8C != ovl2) {
        D_00239F8C = ovl2;
    }
}

/* Overlays (index into the file table D_002136C0): 0 SUBMAIN, 1 DEMO, 2 GAME, 3 NETWK (at 0x540000),
   4 NETDNAS, 5 NETHTTP (0x6C0000), 6 EDITOR, 7 NETAQ, 8 SINGLE (0x820000).
   Each overlay's entry tasks, by their PS2 address inside it: */
void D_540080(Task *t);          /* DEMO.BIN */
void D_5CD580(Task *t);          /* GAME.BIN */
void D_599570(Task *t);          /* NETWK.BIN */
void D_599730(Task *t);          /* NETWK.BIN */

void func_001B6820(void) {
    func_001B6630(D_540080, 4);
    func_001B67C0(1, -1);
}

void func_001B6850(void) {
    func_001B65D0(D_540080, 4);
    func_001B67C0(1, -1);
}

s32 func_001AF500(void);

void func_001B6880(void) {
    s32 online = func_001AF500();

    func_001B6630(D_5CD580, 4);
    if (online == 1) {
        func_001B67C0(2, 7);
    } else if (online == 0) {
        func_001B67C0(2, 8);
    }
}

void func_001B68E0(void) {
    func_001AF500();
    func_001B6630(D_599570, 4);
    func_001B67C0(3, 7);
}

void func_001B6920(void) {
    func_001AF500();
    func_001B6630(D_599730, 4);
    func_001B67C0(3, 7);
}

extern u8 D_003063A0[];   /* (in a structure not mapped yet) */

s32 func_001B6960(void) {
    return D_003063A0[0];
}

s32 func_001A8990(s8 ovl);
void func_001AED60(void);
void func_001BA570(void);
extern u16 D_0023BBAC;        /* the game is paused */
extern u8 D_48BF7C[];         /* (SUBMAIN.BIN) */

/* Once a frame: load the overlays asked for, then run the tasks. While the game is paused only
   slots 0-3 and 13-15 run. */
void func_001B63C0(void) {
    s32 i;
    s32 lo;
    s32 hi;
    Task *t;
    Task *ovl;
    void (*fn)(Task *);

    if (D_00239F88 != -1) {
        func_001A8990(D_00239F88);
        D_00306590.ovl54 = D_00239F88;
        D_00239F88 = -1;
        ovl = &D_002F85D0[4];
        fn = ovl->fn;
        memset(ovl, 0, sizeof(Task));
        ovl->fn = fn;
        ovl->state = TASK_START;
    }
    if (D_00239F8C != -1) {
        func_001A8990(D_00239F8C);
        D_00306590.ovl82 = D_00239F8C;
        D_00239F8C = -1;
    }
    lo = 4;
    if (D_0023BBAC == 0) {
        lo = 16;
        if (D_00306590.unk0 == 0 || D_48BF7C[0] == 0) {
            hi = -1;
            goto run;
        }
        lo = 4;
    }
    hi = 12;
run:
    if (func_001B6960() != 0) {
        lo = 0;
        hi = 12;
    }
    i = 0;
    t = D_002F85D0;
    do {
        if (i < lo || hi < i) {
            switch (t->state) {
            case TASK_FREE:
                break;
            case TASK_START:
                t->state = TASK_RUN;
                break;
            case TASK_RESUME:
                t->state = TASK_RUN;
                break;
            case TASK_RUN:
                t->fn(t);
                break;
            case 1:
                break;
            case TASK_RESTART:
                t->timer = 0;
                t->state = TASK_START;
                break;
            case TASK_WAIT:
                t->timer--;
                if (t->timer < 0) {
                    t->timer = 0;
                } else if (t->timer == 0) {
                    t->state = TASK_START;
                }
                break;
            }
        }
        i++;
        t++;
    } while (i < TASK_MAX);
    if (func_001B6960() == 0) {
        func_001AED60();
        func_001BA570();
    }
}

void func_001B69B0(void) {
}
