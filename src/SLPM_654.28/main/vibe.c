#include "sce.h"
#include "pad.h"
#include "task.h"
#include "options.h"

/* Controller vibration patterns, run by a task (slot 14). */

typedef struct {
    s16 frames;         /* 0: end */
    s16 pad2;
    u8 *motor;          /* 0x04: small on/off, large strength */
} VibStep;

typedef struct {
    u8 enabled;         /* 0x00: the pad can vibrate */
    u8 playing;         /* 0x01 */
    u8 pad2[0xC - 2];
    s16 timer;          /* 0x0C */
    s16 step;           /* 0x0E */
    VibStep *pattern;   /* 0x10 */
    u8 pad14[0x1C - 0x14];
} Vibration;

extern Vibration D_002F87D0;
extern s32 D_0023BB58;
extern s32 D_0023BB5C;
s32 func_00189388(s32 socket, s32 n, u8 *mode, s32 len, u8 *data);

void func_001B8D90(void) {
    memset(&D_002F87D0, 0, sizeof(Vibration));
}

void func_001B8DB0(s32 port) {
    Vibration *v = &D_002F87D0;

    if (D_00292D60[port].state != 1) {
        v->enabled = 0;
        v->timer = 0;
        v->step = 0;
        v->pattern = NULL;
        v->playing = 0;
        return;
    }
    v->enabled = 1;
}

void func_001B8F10(s32 port, u8 *motor) {
    PadPort *pp = &D_00292D60[port];
    u8 mode;
    u16 d;

    if (pp->actuators != 0 && pp->state == 1) {
        mode = 3;
        d = (motor[1] << 1) | (motor[0] & 1);
        func_00189388(pp->socket, 1, &mode, 2, (u8 *)&d);
    }
}

void func_001B8EB0(s32 port) {
    u8 off[2];

    off[0] = 0;
    off[1] = 0;
    func_001B8F10(port, off);
    D_002F87D0.timer = 0;
    D_002F87D0.step = 0;
    D_002F87D0.pattern = NULL;
    D_002F87D0.playing = 0;
}

#ifdef NON_MATCHING   /* the pointer to the state is set a little earlier in the original */
void func_001B8F90(s32 port) {
    Vibration *v = &D_002F87D0;

    if (D_002F87D0.enabled != 0 && v->pattern != NULL) {
        v->timer -= (s16)(1 << D_00306590.unkC);
        if (v->timer > 0) {
            return;
        }
        v->timer = v->pattern[v->step].frames;
        if (v->timer != 0) {
            func_001B8F10(port, v->pattern[v->step].motor);
            v->step++;
            return;
        }
    }
    func_001B8EB0(port);
}
#endif

void func_001B8D00(Task *t) {
    switch (t->step) {
    case 0:
        func_001B8D90();
        D_0023BB5C = 0;
        D_0023BB58 = 0;
        t->step++;
    case 1:
        func_001B8DB0(0);
        switch (D_002F87D0.playing) {
        case 0:
            break;
        case 1:
            func_001B8F90(0);
            break;
        }
        break;
    }
}

extern VibStep *D_00217220[9];  /* the patterns */

/* Start vibration pattern n (when vibration is on and the pad has a motor). */
void func_001B8E00(s32 port, s32 n) {
    Vibration *v = &D_002F87D0;

    if (D_00306590.vibration != 0 && v->enabled != 0 && n < 9) {
        v->pattern = D_00217220[n];
        v->step = 0;
        v->timer = 1;
        v->playing = 1;
    }
}

void func_001B9060(s32 n) {
    func_001B8E00(0, n);
}

void func_001B8F00(void) {
    func_001B8EB0(0);
}
