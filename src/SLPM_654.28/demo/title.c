#include "common.h"
#include "options.h"
#include "task.h"
#include "sce.h"

/* DEMO.BIN: the title screen and its menus, attract mode. Its task runs in slot 4. */

typedef struct {
    s8 state;           /* 0x00 */
    s8 sub;             /* 0x01 */
    s8 step;            /* 0x02 */
    s8 unk3;            /* 0x03 */
    s8 unk4;            /* 0x04 */
    u8 unk5;
    s8 unk6;            /* 0x06 */
    u8 unk7;
    s16 unk8;           /* 0x08: timer */
    s16 unkA;           /* 0x0A */
    u8 unkC;            /* 0x0C */
    u8 unkD[3];
    s32 tex[16];        /* 0x10: texture handles (six from demo_com.tex) */
    u16 held;           /* 0x50 */
    u16 pressed;        /* 0x52 */
    u16 repeat;         /* 0x54: pressed or repeating */
    u16 heldStick;      /* 0x56: with the left stick as the d-pad */
    u16 pressedStick;   /* 0x58 */
    u16 any;            /* 0x5A */
    s32 skip;           /* 0x5C: start was pressed */
    s32 unk60;          /* 0x60 */
    s32 unk64;          /* 0x64 */
    s32 unk68;          /* 0x68 */
    s32 unk6C;          /* 0x6C */
    s32 unk70;          /* 0x70 */
    u8 unk74[0x20C - 0x74];
    s32 unk20C;         /* 0x20C */
    s32 demo;           /* 0x210: attract movie to play next */
    u8 unk214[0x224 - 0x214];
} TitleWork;

extern TitleWork *D_0023BE34;
extern u8 *D_003065F4[];    /* (Options) work memory for the title */
extern u8 D_003065A1[];     /* (Options) */
extern u8 D_003065C3[];     /* (Options) */
extern u8 D_003065BB[];     /* (Options) */
extern u8 D_00306659[];     /* (Options) how the title was entered */
extern u8 D_002F88B0[0x30];
extern u8 D_002F88D1[];

void func_001A8B40(void);
void func_001D6CD0(void);
void func_001B69B0(void);
void func_001B67A0(s16 slot, void (*onKill)(void));
void func_001D5400(void *p);
void func_001B39B0(u32 mode);
void func_001D54D0(void);
void func_001AEC00(void);
void func_00540220(Task *t);

void func_005403B0(TitleWork *w) {
    w->unk70 = 0;
    w->unk68 = w->unkC;
    w->unk6C = 0;
    w->state = 1;
    w->sub = 0;
    w->unk8 = 0;
    w->unk6 = 1;
}

void func_00540BB0(TitleWork *w) {
    w->state = 2;
    w->sub = 5;
    w->unk64 = 0;
    w->unk68 = D_002F88D1[0];
    memset(D_002F88B0, 0, sizeof(D_002F88B0));
    D_002F88D1[0] = w->unk68;
}

/* When the title's task ends. */
void func_00540C10(void) {
    func_001D54D0();
    func_001AEC00();
}

void func_005400D0(Task *t) {
    func_001A8B40();
    func_001D6CD0();
    D_0023BE34 = (TitleWork *)D_003065F4[0];
    memset(D_0023BE34, 0, sizeof(TitleWork));
    if (D_003065A1[0] == 0) {
        D_0023BE34->unk20C = 5;
    } else {
        D_0023BE34->unk20C = 6;
    }
    D_0023BE34->unk4 = 0;
    D_0023BE34->state = 0;
    D_0023BE34->sub = 0;
    func_001B69B0();
    D_00306590.unk0 = 0;
    if (D_003065C3[0] != 0) {
        D_003065C3[0] = 0;
        D_0023BE34->state = 1;
        func_005403B0(D_0023BE34);
    }
    t->step++;
    func_001B67A0(4, func_00540C10);
    func_001D5400(D_0023BE34->tex);
    switch (D_00306659[0]) {
    case 1:
        D_003065BB[0] = 0;
        func_001B39B0(6);
        func_005403B0(D_0023BE34);
        D_00306659[0] = 0;
        break;
    case 0:
    case 2:
        D_00306659[0] = 0;
        break;
    case 3:
        func_00540BB0(D_0023BE34);
        break;
    }
}

void func_00540080(Task *t) {
    switch (t->step) {
    case 0:
        func_005400D0(t);
        break;
    case 1:
        func_00540220(t);
        break;
    }
}

/* --- the title loop --- */

extern volatile s32 D_003064D0[];    /* pads: pressed */
extern volatile s32 D_003064D8[];    /* held */
extern volatile s32 D_003064E8[];    /* repeating */

void func_00540340(TitleWork *w);
void func_00540450(TitleWork *w);
void func_00540A50(TitleWork *w);
void func_00540E30(TitleWork *w);
void func_00540F30(TitleWork *w);
void func_005412A0(TitleWork *w);
void func_001BA7A0(s16 id);
void func_001BA870(void);

void func_00540220(Task *t) {
    D_0023BE34->held = D_003064D8[0];
    D_0023BE34->pressed = D_003064D0[0];
    D_0023BE34->repeat = (u16)D_003064E8[0] | D_0023BE34->pressed;
    D_0023BE34->heldStick = (u16)((u32)(D_003064D8[0] & 0xF0000) >> 16) | D_0023BE34->held;
    D_0023BE34->pressedStick = (u16)((u32)(D_003064D0[0] & 0xF0000) >> 16) | D_0023BE34->pressed;
    D_0023BE34->any = D_0023BE34->repeat | ((u16)((u32)(D_003064E8[0] & 0xF0000) >> 16) | D_0023BE34->pressedStick);
    switch (D_0023BE34->state) {
    case 0:
        func_00540340(D_0023BE34);
        break;
    case 1:
        func_00540450(D_0023BE34);
        break;
    case 2:
        func_00540A50(D_0023BE34);
        break;
    }
}

/* State 0: the logos and the attract mode. */
void func_00540340(TitleWork *w) {
    switch (w->sub) {
    case 0:
        func_00540E30(w);
        break;
    case 2:
        func_00540F30(w);
        break;
    case 1:
        func_005412A0(w);
        break;
    }
}

void func_00540C40(TitleWork *w) {
    w->sub = 0;
    w->step = 0;
    w->unk3 = 0;
    w->unk8 = 0;
    w->unkA = 0;
    w->unk60 = 0;
}

void func_00540C60(TitleWork *w) {
    w->sub = 2;
    w->step = 0;
    w->unk3 = 0;
    w->unk8 = 0;
    w->unkA = 0;
    w->unk60 = 0;
    w->skip = 0;
}

/* Start pressed? */
s32 func_00540CB0(TitleWork *w) {
    if (w->pressed & 0x8000) {
        w->skip = 1;
        return 1;
    }
    return 0;
}

typedef struct {
    s16 x, y, w, h;
    u32 color;
    s16 u0, v0, u1, v1;
} Sprite2D;

void func_0019CD60(s32 n, s32 *handles);
void func_001AE290(void);
void func_001AE9C0(s32 cmd, u32 arg);
void func_001AE430(s32 a, s32 b);
s32 func_001AAB10(Sprite2D *s);

/* A picture with texture handle tex. */
void func_005415E0(s32 tex, Sprite2D *s) {
    if (tex != 0) {
        func_001AE290();
        func_001AE9C0(0xD, 0);
        func_001AE9C0(0x5F, 5);
        func_001AE9C0(0x60, 0);
        func_001AE9C0(0x63, 0);
        func_001AE9C0(0x6C, 0);
        func_0019CD60(1, &tex);
        func_001AE9C0(4, tex);
        func_001AE430(4, 5);
        func_001AAB10(s);
    }
}

extern u8 D_00558070, D_00558071, D_00558072, D_00558073;
extern Sprite2D D_00558080;
extern Sprite2D D_00558094;
extern Sprite2D D_005580A8;
extern Sprite2D D_005580BC;

#ifdef NON_MATCHING   /* struct copies differ */
/* The title pictures: the logo (0), or the three title parts (1). */
void func_00541680(TitleWork *w, s32 which) {
    Sprite2D s;

    switch (which) {
    case 0:
        s = D_00558080;
        s.color = 0xFFFFFFFF;
        func_005415E0(w->tex[D_00558070], &s);
        break;
    case 1:
        s = D_00558094;
        s.color = 0xFF555555;
        func_005415E0(w->tex[D_00558071], &s);
        s = D_005580A8;
        s.color = 0xFFCCCCCC;
        func_005415E0(w->tex[D_00558072], &s);
        s = D_005580BC;
        s.color = 0xFFFFFFFF;
        func_005415E0(w->tex[D_00558073], &s);
        break;
    }
}
#endif

/* Sub 0: the splash, 160 frames (start skips to the last 20). */
void func_00540E30(TitleWork *w) {
    switch (w->step) {
    case 0:
        w->unk8 = 0xA0;
        func_001BA7A0(8);
        w->step++;
        break;
    case 1:
        if (w->unk8 > 0x15 && func_00540CB0(w) != 0) {
            w->unk8 = 0x14;
        } else if (--w->unk8 <= 0) {
            w->step++;
        }
        if (w->unk8 == 0x14) {
            func_001BA7A0(7);
        }
        func_00541680(w, 0);
        break;
    case 2:
        func_001BA870();
        func_00540C60(w);
        break;
    }
}

/* --- sub 2: the opening movie, then the title screen; after a while the attract movie --- */

s32 func_001B6980(s32 slot);
void func_001AD470(void);
void func_001AD680(s32 a, s32 b, s32 frames);
void func_001AECB0(void (*fn)(void));
void func_001B6EF0(s32 a);
s32 func_0034A290(void);    /* SUBMAIN: movie state, 2 finished */
void func_0034A7E0(void);   /* SUBMAIN: start the movie */
void func_00349E20(void);   /* SUBMAIN: stop it */
f32 func_00191B60(f32 x);   /* sin */

void func_00540C90(TitleWork *w) {
    w->sub = 1;
    w->step = 0;
    w->unk3 = 0;
    w->unk8 = 0;
    w->unkA = 0;
    w->unk60 = 0;
}

/* To the main menu. */
void func_00540CD0(TitleWork *w) {
    w->state = 1;
    w->sub = 0;
    w->step = 0;
    w->unk3 = 0;
    w->unk8 = 0;
    w->unkA = 0;
    w->unk60 = 0;
    w->skip = 0;
    w->unk6 = 1;
}

extern u8 D_0055807D;
extern Sprite2D D_00558184;     /* "PRESS START" */
extern f32 D_00239CB0;          /* pi * 2 (gp) */

#ifdef NON_MATCHING   /* the original copies the sprite word by word (its data is in this file) */
/* "PRESS START", pulsing. */
void func_00540D00(TitleWork *w) {
    Sprite2D s;
    u32 a;

    w->unk60 = (w->unk60 + 0x300) & 0x7FFF;
    a = 255.0f * func_00191B60(D_00239CB0 * (f32)(u32)(u16)w->unk60 / 65536.0f);
    s = D_00558184;
    s.color = (a << 24) | 0xFFFFFF;
    func_005415E0(w->tex[D_0055807D], &s);
}
#endif

/* Stops the movie: wait 10 frames for it, 1 if none is running. */
void func_005411F0(TitleWork *w) {
    if (func_001B6980(0xE) == 0) {
        func_00349E20();
        w->unk8 = 0xA;
    } else {
        w->unk8 = 1;
    }
}

void func_00541240(void) {
    if (func_001B6980(0xE) == 0) {
        func_0034A7E0();
    }
}

void func_00541FA0(void) {
}

void func_00540F30(TitleWork *w) {
    switch (w->step) {
    case 0:
        func_00541240();
        w->unk8 = 0x14;
        w->step++;
        break;
    case 1:
        w->unk8--;
        if (func_001B6980(0xE) != 0) {
            if (w->unk8 <= 0) {
                w->unk8 = 0x14;
                w->step++;
            }
        } else if (func_0034A290() == 2 || w->unk8 <= 0) {
            w->unk8 = 0x14;
            w->step++;
        }
        func_001BA7A0(8);
        break;
    case 2:
        if (--w->unk8 == 0) {
            w->unk8 = 0x3C0;
            w->step++;
        }
        break;
    case 3:
        if (w->skip != 0) {
            if (w->unk8 > 0x14) {
                w->unk8 = 0x14;
                func_001AD680(1, 0, 0x14);
                func_001BA7A0(7);
            } else if (w->unk8 < 2) {
                w->unk8 = 1;
            }
            func_001B6EF0(0);
            w->step = 5;
            break;
        }
        if (w->unk8 <= 1 || func_00540CB0(w) == 0) {
            if (--w->unk8 <= 0) {
                func_005411F0(w);
                w->step++;
            } else if (w->unk8 == 0x14) {
                func_001AD680(1, 0, 0x14);
                func_001BA7A0(7);
            }
        }
        func_00540D00(w);
        break;
    case 4:
        if (--w->unk8 <= 0) {
            func_001BA870();
            func_00540C90(w);
            func_001AECB0(func_00541FA0);
        }
        break;
    case 5:
        if (--w->unk8 <= 0) {
            func_001AD470();
            func_005411F0(w);
            w->unk8 = 0xA;
            w->step++;
        }
        break;
    case 6:
        if (--w->unk8 <= 0) {
            func_001BA870();
            func_00540CD0(w);
        }
        break;
    }
}

void func_001ADA60(s32 stream, s32 a, const char *name, s32 b);   /* play an ADX file */
void func_001ADC30(s32 stream);                                     /* stop an ADX stream */
void func_0034AFB0(s32 which);  /* SUBMAIN: an attract movie */
extern char D_0055FC70[];       /* "demo/battle.adx" */
extern char D_0055FC80[];       /* "demo/hint.adx" */

#ifdef NON_MATCHING   /* argument order of the ADX calls, registers in step 3 */
/* Sub 1: attract mode, the two demo movies in turn (each with its own sound). */
void func_005412A0(TitleWork *w) {
    switch (w->step) {
    case 0:
        w->step++;
        if (func_001B6980(0xE) != 0) {
            w->unk8 = 1;
        } else {
            w->unk8 = 0x96;
            func_0034AFB0(w->demo);
            if (w->demo == 0) {
                func_001ADA60(1, 0, D_0055FC70, 0);
            } else {
                func_001ADA60(1, 0, D_0055FC80, 0);
            }
        }
        w->demo ^= 1;
        func_001BA7A0(8);
        break;
    case 1:
        w->unk8--;
        if (func_001B6980(0xE) != 0) {
            if (w->unk8 <= 0) {
                w->unk8 = 0x3C0;
                w->step++;
            }
        } else if (func_0034A290() == 2 || w->unk8 <= 0) {
            if (w->demo == 1) {
                w->unk8 = 0x20A;
            } else {
                w->unk8 = 0x45F;
            }
            func_001ADC30(1);
            w->step++;
        }
        func_001BA7A0(8);
        break;
    case 2:
        if (w->skip != 0) {
            if (w->unk8 > 0x14) {
                w->unk8 = 0x14;
                func_001AD680(1, 0, 0x14);
                func_001BA7A0(7);
            } else if (w->unk8 < 2) {
                w->unk8 = 1;
            }
            w->step = 4;
            break;
        }
        if (w->unk8 <= 1 || func_00540CB0(w) == 0) {
            if (--w->unk8 <= 0) {
                func_005411F0(w);
                w->step++;
            } else if (w->unk8 == 0x14) {
                func_001AD680(1, 0, 0x14);
                func_001BA7A0(7);
            }
        }
        break;
    case 3:
        if (--w->unk8 <= 0) {
            func_001BA870();
            if (w->skip == 1) {
                func_00540C60(w);
            } else if (w->demo == 0) {
                func_00540C40(w);
            } else {
                func_00540C60(w);
            }
        }
        break;
    case 4:
        if (--w->unk8 <= 0) {
            func_001AD470();
            func_005411F0(w);
            w->unk8 = 2;
            w->step++;
        }
        break;
    case 5:
        if (--w->unk8 <= 0) {
            func_001BA870();
            func_00540C60(w);
        }
        break;
    }
}
#endif
