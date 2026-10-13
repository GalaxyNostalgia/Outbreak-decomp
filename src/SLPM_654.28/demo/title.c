#include "common.h"
#include "options.h"
#include "task.h"
#include "sce.h"

/* DEMO.BIN: the title screen and its menus, attract mode. Its task runs in slot 4. */

typedef struct {
    s16 x, y, w, h;
    u32 color;
    s16 u0, v0, u1, v1;
} Sprite2D;

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
    u8 visible[13];     /* 0x74: menu pictures shown */
    u8 unk81[3];
    s32 itemTex[13];    /* 0x84: their textures */
    u8 unkB8[0xC];
    Sprite2D items[10]; /* 0xC4: 0 background, 1 frame, 2 flash, 3 logo, 4.. the menu entries */
    u8 unk18C[0x204 - 0x18C];
    s16 sel;            /* 0x204: entry being highlighted */
    s16 blink;          /* 0x206 */
    u8 unk208[4];
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
extern Sprite2D D_00558080[13];     /* the menu pictures (0: the logo) */
extern Sprite2D D_00558094;
extern Sprite2D D_005580A8;
extern Sprite2D D_005580BC;

#ifdef NON_MATCHING   /* struct copies differ */
/* The title pictures: the logo (0), or the three title parts (1). */
void func_00541680(TitleWork *w, s32 which) {
    Sprite2D s;

    switch (which) {
    case 0:
        s = D_00558080[0];
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

/* --- state 1: the main menu --- */

void func_00541270(void);
void func_00541800(TitleWork *w);
void func_005403E0(TitleWork *w);
void func_00540940(TitleWork *w, s32 a, s32 n);
void func_001B9060(s32 a);
void func_001B9140(s32 frames);

#ifdef NON_MATCHING   /* the original keeps nav in a2 */
void func_00540450(TitleWork *w) {
    u16 nav = w->any;

    switch (w->sub) {
    case 0:
        func_00541270();
        w->unk8 = 0x14;
        w->unkC = w->unk64;
        w->step = 0;
        w->sub = 1;
        func_001BA7A0(8);
        break;
    case 1:
        if (--w->unk8 == 0) {
            w->sub++;
            w->unk8 = 0x14;
        }
        func_001BA7A0(8);
        break;
    case 2:
        if (--w->unk8 == 0) {
            w->sub++;
        }
        break;
    case 3:
        if (func_001B6980(0x52) != 0) {
            w->sub++;
            break;
        }
        func_00541800(w);
        if (w->step == 4) {
            w->sub++;
        }
        break;
    case 4:
        if (func_001B6980(0x52) == 0) {
            func_00541800(w);
        }
        if (++w->unk8 >= 10) {
            w->sub++;
            w->unk8 = 0;
        }
        break;
    case 5:
        if (++w->unk8 >= 0x708 || (w->pressed & 0x10)) {
            /* back to the title (cancel, or 30 seconds untouched) */
            func_001B6EF0(1);
            w->sub = 8;
            w->unk8 = 0x14;
            func_001BA7A0(7);
        } else if (w->pressed & 0x20) {
            w->unk64 = w->unkC;
            if (w->unkC == 0) {
                w->unk68 = 0;
                w->state = 2;
                w->sub = 0;
                w->step = 0;
                w->unk8 = 0;
                w->unk6 = 0;
            } else {
                w->unk8 = 0x14;
                w->sub++;
                func_001BA7A0(7);
            }
            switch (w->unk64) {
            case 0:
                func_001B6EF0(0x10);
                func_001B9060(3);
                break;
            case 1:
                func_001B6EF0(0x11);
                func_001B9060(3);
                break;
            default:
                func_001B6EF0(4);
                break;
            }
            if (w->unkC == 0) {
                return;
            }
        } else {
            if (w->held != 0) {
                w->unk8 = 0;
            }
            if (w->unkC < w->unk20C - 1 && (nav & 2)) {
                func_001B6EF0(2);
                func_005403E0(w);
                w->unkC++;
            } else if (w->unkC > 0 && (nav & 1)) {
                func_005403E0(w);
                w->unkC--;
                func_001B6EF0(2);
            }
        }
        if (func_001B6980(0x52) == 0) {
            func_00541800(w);
        }
        break;
    case 6:
        if (--w->unk8 == 0) {
            func_00349E20();
            w->sub++;
            w->unk8 = 10;
        }
        if (func_001B6980(0x52) == 0) {
            func_00541800(w);
        }
        break;
    case 7:
        if (--w->unk8 == 0) {
            func_001B9140(3);
            func_001BA870();
            w->unk68 = 0;
            w->state = 2;
            w->sub = 0;
            w->step = 0;
            w->unk8 = 0;
            w->unk6 = 0;
        }
        break;
    case 8:
        if (--w->unk8 == 0) {
            func_00349E20();
            w->sub++;
            w->unk8 = 10;
        }
        if (func_001B6980(0x52) == 0) {
            func_00541800(w);
        }
        break;
    case 9:
        if (--w->unk8 == 0) {
            func_001B9140(3);
            func_001BA870();
            w->state = 0;
            w->sub = 2;
            w->step = 0;
            w->unk8 = 0;
            w->unk6 = 0;
        }
        return;
    }
    if (func_001B6980(0x52) != 0) {
        func_00541680(w, 1);
        func_00540940(w, 0, w->unk20C);
    }
}
#endif

void func_0034A970(void);   /* SUBMAIN: the menu's background movie */

void func_00541270(void) {
    if (func_001B6980(0xE) == 0) {
        func_0034A970();
    }
}

/* Highlight the chosen entry (blinks it). */
void func_005403E0(TitleWork *w) {
    if (func_001B6980(0x52) == 0 && w->step != 5) {
        w->sel = w->unkC;
        w->blink = 0;
        w->step = 5;
        w->items[w->sel + 4].color = -1;
    }
}


#ifdef NON_MATCHING   /* first draft: two loop counters in the original */
/* The menu pictures: the frame opens, flashes, the entries appear; then the highlight. */
void func_00541800(TitleWork *w) {
    s32 i;

    switch (w->step) {
    case 0:
        for (i = 0; i < 10; i++) {
            w->visible[i] = 0;
            w->itemTex[i] = 0;
        }
        for (i = 3; i < 10; i++) {
            w->itemTex[i] = w->tex[(&D_00558070)[i]];
            w->items[i] = D_00558080[i];
            if (i == 3) {
                w->items[i].color = -1;
                w->visible[i] = 1;
            } else {
                w->items[i].color = 0xFF555555;
            }
        }
        w->sel = w->unkC;
        w->blink = 0;
        for (i = 4; i < 10; i++) {
            w->items[i].color = 0xFF555555;
            w->visible[i] = 1;
        }
        w->step = 3;
        break;
    case 1:
        w->items[1].y -= 10;
        w->items[1].h += 20;
        if (w->items[1].h >= 0xA0) {
            w->items[1].h = 0xA0;
        }
        if (w->items[1].y < 0x61) {
            w->items[1].y = 0x60;
            w->items[1].h = 0xA0;
            w->visible[2] = 1;
            w->step++;
        }
        break;
    case 2: {
        s32 a = (u8)(w->items[2].color >> 24) - 0x10;

        if (a <= 0) {
            w->visible[2] = 0;
            a = 0;
            w->step++;
        }
        w->items[2].color = (a << 24) | 0xFFFFFF;
        w->items[2].x -= 16;
        w->items[2].w += 32;
        if (w->items[2].h != 0) {
            w->items[2].y += 10;
            w->items[2].h -= 20;
            if (w->items[2].h <= 0) {
                w->items[2].h = 0;
            }
        }
        break;
    }
    case 3: {
        s32 c = (u8)w->items[0].color - 0x22;

        if (c < 0x56) {
            c = 0x55;
        }
        w->items[0].color = 0xFF000000 | (c << 16) | (c << 8) | c;
        c = (u8)w->items[1].color - 10;
        if (c < 0xCE) {
            c = 0xCD;
        }
        w->items[1].color = 0xFF000000 | (c << 16) | (c << 8) | c;
        if (w->items[0].color == 0xFF555555 || w->items[1].color == 0xFFCDCDCD) {
            w->items[0].color = 0xFF555555;
            w->items[1].color = 0xFFCCCCCC;
            w->step++;
            w->sel = w->unkC;
            w->blink = 0;
            for (i = 4; i < 10; i++) {
                w->items[i].color = 0xFF555555;
                w->visible[i] = 1;
            }
        }
        break;
    }
    case 4:
        w->blink++;
        if (w->blink & 1) {
            w->items[w->sel + 4].color = -1;
        } else {
            w->items[w->sel + 4].color = 0xFF555555;
        }
        if (w->blink == 5) {
            w->items[w->sel + 4].color = -1;
            w->step++;
        }
        break;
    case 5:
        if (w->sel != w->unkC) {
            for (i = 4; i < 10; i++) {
                w->items[i].color = 0xFF555555;
            }
            w->sel = w->unkC;
            w->blink = 0;
            w->step--;
        }
        break;
    }
    for (i = 0; i < 10; i++) {
        if (w->visible[i] && w->itemTex[i] != 0) {
            if (i - 4 >= w->unk20C) {
                return;
            }
            func_005415E0(w->itemTex[i], &w->items[i]);
        }
    }
}
#endif

void func_001B4300(s32 size);
void func_001B4320(s32 x, s32 y);
void func_001B4340(s32 z);
void func_001B4370(s32 clut);
void func_001B4380(u32 color);
void func_001B43C0(u8 *s);
char *func_001C0240(s32 n);     /* menu text n */
char *strcpy(char *dst, const char *src);
s32 strlen(const char *s);

#ifdef NON_MATCHING   /* first draft: scheduling of the centring */
/* The menu's labels, centred, the chosen one in another colour. */
void func_00540940(TitleWork *w, s32 first, s32 n) {
    u8 buf[0x80];
    s32 i;
    s32 x;
    s32 y;

    func_001B4340(-2);
    func_001B4300(0x1C);
    func_001B4370(0);
    for (i = first, y = 0x11E; i < first + n; i++, y += 0x18) {
        strcpy((char *)buf, func_001C0240(i));
        x = (0x280 - strlen((char *)buf) * 14) / 2;
        if (i == w->unkC + first) {
            func_001B4380(0x80008080);
        } else {
            func_001B4380(0x80808080);
        }
        func_001B4320(x, y);
        func_001B43C0(buf);
    }
}
#endif


/* --- state 2: the chosen menu entry --- */

void func_00542020(TitleWork *w);
void func_005423B0(TitleWork *w);
void func_00542490(TitleWork *w);
void func_00542550(TitleWork *w);
void func_00542540(TitleWork *w);
void func_0054CBB0(TitleWork *w);
void func_00540B10(TitleWork *w);

void func_00540A50(TitleWork *w) {
    switch (w->unk64) {
    case 0:
        func_00542020(w);
        break;
    case 1:
        func_005423B0(w);
        break;
    case 2:
        func_00542490(w);
        break;
    case 3:
        func_00542550(w);
        break;
    case 4:
        func_00542540(w);
        break;
    case 5:
        func_0054CBB0(w);
        break;
    }
    if (w->unk70 != 0) {
        func_00540B10(w);
    }
}

/* --- menu entry 0: the first sub-menu (two choices; the second needs D_00301D7F bit 0) --- */

extern u8 D_00301D7F[];
void func_00541FB0(TitleWork *w);
void func_00541CB0(TitleWork *w);

#ifdef NON_MATCHING   /* first draft: delay slots */
void func_00542020(TitleWork *w) {
    switch (w->sub) {
    case 0:
        memset(D_002F88B0, 0, sizeof(D_002F88B0));
        w->unk6 = 1;
        w->unkC = w->unk68;
        w->unk8 = 0;
        w->sub++;
    case 1:
        if (++w->unk8 >= 10) {
            w->sub++;
            w->unk8 = 0;
        }
        break;
    case 2:
        if (++w->unk8 >= 0x708 || (w->pressed & 0x10)) {
            func_001B6EF0(1);
            w->sub = 6;
            w->unk8 = 0x14;
            func_001BA7A0(7);
            break;
        }
        if (w->held != 0) {
            w->unk8 = 0;
        }
        if (w->pressed & 0x20) {
            D_002F88B0[1] = 0;
            if (w->unkC == 1) {
                if (!(D_00301D7F[0] & 1)) {
                    break;
                }
                w->unk68 = w->unkC;
                D_002F88B0[1] |= 1;
                w->sub++;
                w->unk8 = 0x14;
                w->step = 0;
                w->unk3 = 0;
                func_001B6EF0(4);
                func_001BA7A0(7);
            } else {
                w->unk68 = w->unkC;
                w->sub++;
                w->unk8 = 0x14;
                w->step = 0;
                w->unk3 = 0;
                func_001B6EF0(4);
                func_001BA7A0(7);
            }
        } else if (w->unkC <= 0 && (w->any & 2)) {
            w->unkC++;
            func_001B6EF0(2);
        } else if (w->unkC > 0 && (w->any & 1)) {
            w->unkC--;
            func_001B6EF0(2);
        }
        break;
    case 3:
        if (--w->unk8 != 0) {
            break;
        }
        func_005411F0(w);
        w->unk8 = 5;
        w->sub++;
    case 4:
        if (--w->unk8 != 0) {
            break;
        }
        w->sub++;
    case 5:
        switch (w->unk68) {
        case 2:
            break;
        case 1:
            func_00541FB0(w);
            break;
        case 0:
            func_00541FB0(w);
            break;
        }
        break;
    case 6:
        if (--w->unk8 == 0) {
            func_00349E20();
            w->sub++;
            w->unk8 = 10;
        }
        break;
    case 7:
        if (--w->unk8 != 0) {
            return;
        }
        func_001B9140(3);
        func_001BA870();
        func_005403B0(w);
        return;
    }
    if (w->sub != 0 && w->sub < 3) {
        func_00541CB0(w);
    }
}
#endif

void func_00542560(TitleWork *w);
void func_00543050(TitleWork *w);

void func_00541FB0(TitleWork *w) {
    switch (w->step) {
    case 0:
        func_001AEC00();
        func_00542560(w);
        w->step++;
        break;
    case 1:
        func_00543050(w);
        break;
    }
}

#ifdef NON_MATCHING   /* first draft: struct copies */
/* The sub-menu's pictures: its frame and two entries (the second one depends on D_00301D7F). */
void func_00541CB0(TitleWork *w) {
    s32 i;
    s32 k;

    switch (w->step) {
    case 0:
        for (i = 0; i < 13; i++) {
            w->visible[i] = 0;
            w->itemTex[i] = 0;
        }
        w->itemTex[3] = w->tex[D_00558073];
        w->items[3] = D_00558080[3];
        w->items[3].color = -1;
        w->visible[3] = 1;
        w->itemTex[4] = w->tex[(&D_00558070)[10]];
        w->items[4] = D_00558080[10];
        w->items[4].color = 0xFF555555;
        w->visible[4] = 1;
        k = (D_00301D7F[0] & 1) ? 11 : 12;
        w->itemTex[5] = w->tex[(&D_00558070)[k]];
        w->items[5] = D_00558080[k];
        w->items[5].color = 0xFF555555;
        w->visible[5] = 1;
        w->sel = w->unkC;
        w->blink = 0;
        w->step = 1;
        break;
    case 1:
        w->blink++;
        if (w->blink & 1) {
            w->items[w->sel + 4].color = -1;
        } else {
            w->items[w->sel + 4].color = 0xFF555555;
        }
        if (w->blink == 5) {
            w->items[w->sel + 4].color = -1;
            w->step++;
        }
        break;
    case 2:
        if (w->sel != w->unkC) {
            for (i = 4; i < 6; i++) {
                w->items[i].color = 0xFF555555;
            }
            w->sel = w->unkC;
            w->blink = 0;
            w->step--;
        }
        break;
    }
    for (i = 0; i < 6; i++) {
        if (w->visible[i] && w->itemTex[i] != 0) {
            func_005415E0(w->itemTex[i], &w->items[i]);
        }
    }
}
#endif


/* --- the player select screen (select.tex) --- */

/* A picture on the select screen, from a UiDef table entry */
typedef struct {
    u8 page, b, c, d;
    s16 x, y, w, h;
    s16 u0, v0, u1, v1;
} UiDef;                /* 0x14 */

typedef struct {
    s16 used;
    s16 visible;        /* 0x02 */
    u8 anim;            /* 0x04 */
    u8 unk5[3];
    s16 timer;          /* 0x08 */
    u8 unkA[4];
    s16 frame;          /* 0x0E */
    s32 alpha;          /* 0x10 */
    s32 phase;          /* 0x14 */
    u8 unk18[8];
    s16 x, y, w, h;     /* 0x20 */
    s16 u0, v0, u1, v1; /* 0x28 */
    s32 page;           /* 0x30 */
    s32 b;              /* 0x34 */
    s16 c;              /* 0x38 */
    s16 d;              /* 0x3A */
    s16 bx, by, bw, bh; /* 0x3C: where it started */
    u32 color;          /* 0x44 */
} UiElem;               /* 0x48 */

#define UI_SET(e, def)              \
    do {                            \
        (e)->used = 1;              \
        (e)->page = (def)->page;    \
        (e)->b = (def)->b;          \
        (e)->c = (def)->c;          \
        (e)->d = (def)->d;          \
        (e)->x = (def)->x;          \
        (e)->y = (def)->y;          \
        (e)->w = (def)->w;          \
        (e)->h = (def)->h;          \
        (e)->u0 = (def)->u0;        \
        (e)->v0 = (def)->v0;        \
        (e)->u1 = (def)->u1;        \
        (e)->v1 = (def)->v1;        \
        (e)->bx = (e)->x;           \
        (e)->by = (e)->y;           \
        (e)->bw = (e)->w;           \
        (e)->bh = (e)->h;           \
    } while (0)

typedef struct {
    u8 unk0[4];
    s8 state;           /* 0x04 */
    s8 sel;             /* 0x05: yes/no */
    s8 sub;             /* 0x06 */
    s8 unk7;            /* 0x07 */
    u16 held;           /* 0x08 */
    u16 pressed;        /* 0x0A */
    u16 nav;            /* 0x0C: pressed or repeating */
    u8 unkE[2];
    s32 timer;          /* 0x10 */
    u8 unk14[0xC];
    s32 timer2;         /* 0x20 */
    s32 unk24;
    s32 shown[35];      /* 0x28 */
    s32 unkB4;
    s32 file[35];       /* 0xB8: the save files listed (-1: none) */
    s32 score[35];      /* 0x144: capped at 9999 */
    s32 unk1D0[35];     /* 0x1D0 */
    s32 files;          /* 0x25C */
    s32 cur;            /* 0x260 */
    f32 fx[35];         /* 0x264: the wheel's turn */
    f32 tx[35];         /* 0x2F0 */
    s32 angle[35];      /* 0x37C */
    s32 speed[35];      /* 0x408 */
    f32 dx[35];         /* 0x494 */
    f32 dh[35];         /* 0x520 */
    f32 dw[35];         /* 0x5AC */
    f32 dy[35];         /* 0x638 */
    u8 unk6C4[0x750 - 0x6C4];
    s32 order[35];      /* 0x750: the files' places on the wheel */
    s32 order2[35];     /* 0x7DC: during a turn */
    u8 unk868[0x8F4 - 0x868];
    s32 tex[0x1B];      /* 0x8F4: select.tex pictures */
    u8 unk960[0x974 - 0x960];
    UiElem elems[283];  /* 0x974 */
    u8 unk590C[0x7574 - 0x590C];
    s32 extraCount;     /* 0x7574: extra characters unlocked */
    s32 extras[0x80];   /* 0x7578 */
    u8 unk7778[0x77A0 - 0x7778];
    s32 wheel[9];       /* 0x77A0: the characters' places */
    f32 cfx[9];         /* 0x77C4: the wheel's turn */
    f32 ctx[9];         /* 0x77E8 */
    s32 cangle[9];      /* 0x780C */
    s32 cspeed[9];      /* 0x7830 */
    f32 cdx[9];         /* 0x7854 */
    s32 frames;         /* 0x7878 */
    s32 costume[8][3];  /* 0x787C: costumes available, per character */
    s32 costumes[8];    /* 0x78DC */
    s32 anim[8];        /* 0x78FC: the difficulty/scenario screens' pictures: 1 coming in, 2 going out */
    f32 open[8];        /* 0x791C: how far each is in, 0..1 */
    s32 blink2;         /* 0x793C */
    s32 back;           /* 0x7940: came back from the next step */
    s8 items[35][64];   /* 0x7944 */
    u8 unk8204[0x8290 - 0x8204];
    s32 itemCount[35];  /* 0x8290 */
    s16 unk831C[35];    /* 0x831C */
    s16 unk8362[35];    /* 0x8362 */
    s32 scroll;         /* 0x83A8: item list scroll */
    s32 unk83AC[5];     /* 0x83AC */
    u8 unk83C0[0x8438 - 0x83C0];
    u8 unk8438[5][8];   /* 0x8438 */
    u8 unk8460[0x8550 - 0x8460];
    s32 unk8550;        /* 0x8550: from D_002F88D1 */
    s32 unk8554;        /* 0x8554: from D_002F88B1 */
    s32 movieTimer;     /* 0x8558 */
    s32 movieStep;      /* 0x855C */
    s32 idle;           /* 0x8560 */
    s32 moviePlaying;   /* 0x8564: background movie (2: stopped for the restart) */
    s16 blink;          /* 0x8568 */
    u8 unk856A[2];
    s32 saveTex;        /* 0x856C: save.tm2 */
} SelectWork;           /* 0x8570 */

extern SelectWork *D_003065FC[];    /* (Options) */
extern u32 D_003065DC[];            /* (Options) work buffer */
extern s32 D_00559540[0x1B];        /* offsets of the pictures in select.tex */
extern char D_00560580[];           /* "data\rom\scene\pit_tex\save.tm2" */
void func_001AE8E0(s32 i);
void func_001D5500(s32 *offsets, s32 *tex, s32 n);
void func_001D0440(s32 base);
s32 func_001D04D0(s32 id);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 byNumber);
void *func_001CFE20(s32 id, u32 addr);

void func_00542560(TitleWork *w) {
    SelectWork *s;
    u32 buf;

    func_001AE8E0(0);
    s = D_003065FC[0];
    memset(s, 0, sizeof(SelectWork));
    func_001D5500(D_00559540, s->tex, 0x1B);
    s->unk8550 = D_002F88D1[0];
    s->unk8554 = D_002F88B0[1];
    func_001D0440(0x4200);
    if (func_001D04D0(0x421B) == 0) {
        buf = D_003065DC[0];
        if (func_001CFBB0(D_00560580, (void *)buf, 1, 0) != 0) {
            func_001CFE20(0x421B, buf);
            s->saveTex = func_001D04D0(0x421B);
        }
    }
}

extern s32 D_003063E8;          /* menu controls: held */
extern s32 D_003063E0;          /* pressed */
extern u16 D_003063F8;          /* repeating */
extern u8 D_00306658[];         /* (Options) */
extern char *D_005595D0[4];     /* the question's lines */
extern char D_005606A0[], D_005606A8[], D_005606B0[], D_005606B8[];   /* yes / no, highlighted or not */
void func_001AD680(s32 n, s32 limit, s32 frames);
void func_001B6E80(s32 id, s32 bank);
void func_001B87F0(s32 a, s32 b, s32 c);
s32 func_001BF5B0(void);
void func_001C0940(s32 a, s32 b);
s32 func_001C0C20(void);
void func_001D3500(u32 addr);
void func_0034A3D0(void);       /* SUBMAIN: the select screen's movie */
void func_00542640(TitleWork *w);
void func_00542F70(TitleWork *w);
void func_00542FA0(SelectWork *s);
void func_00544620(TitleWork *w, SelectWork *s);
void func_00545AB0(TitleWork *w, SelectWork *s);
void func_00547340(TitleWork *w, SelectWork *s);
void func_005482F0(TitleWork *w, SelectWork *s);

/* The background movie restarts every 0x1A5E0 frames. */
#define SELECT_MOVIE(s)                                 \
    do {                                                \
        (s)->movieTimer++;                              \
        if ((s)->movieTimer >= 0x1A5E0) {               \
            func_00349E20();                            \
            (s)->moviePlaying = 2;                      \
        }                                               \
        if ((s)->movieTimer >= 0x1A5EA) {               \
            func_0034A3D0();                            \
            (s)->moviePlaying = 1;                      \
            (s)->movieTimer = 0;                        \
        }                                               \
    } while (0)

#ifdef NON_MATCHING   /* first draft from m2c */
/* The select screen: first the save data question, then the screens of the select itself. */
void func_00543050(TitleWork *w) {
    SelectWork *s = D_003065FC[0];

    s->held = D_003063E8;
    s->pressed = D_003063E0;
    s->nav = D_003063F8 | s->pressed;
    if (s->state >= 8 && s->state < 12) {
        s->idle++;
        if (s->held != 0) {
            s->idle = 0;
        }
        if (s->idle >= 9000) {
            func_001B6EF0(1);
            func_001AD680(0, 0, 0x14);
            s->state = 12;
            s->sel = 0;
            s->sub = 0;
        }
    }
    switch (s->state) {
    case 0:
        func_001B9140(3);
        func_001BA870();
        if (D_00306659[0] == 3) {
            D_00306659[0] = 0;
            if (!(D_002F88B0[1] & 1)) {
                D_00306658[0] = 0;
                D_002F88B0[0x20] = 0;
            }
            s->state = 6;
        } else if (D_002F88B0[1] & 1) {
            s->state = 6;
        } else {
            D_00306658[0] = 0;
            func_001BA7A0(10);
            s->timer = 10;
            s->state++;
        }
        break;
    case 1:
        if (--s->timer == 0) {
            func_001C0940(3, 2);
            s->state++;
        }
        break;
    case 2:
        if (func_001C0C20() == 0) {
            if (D_002F88B0[0x22] != 0) {
                s->state++;
                s->sel = 0;
            } else {
                s->state = 5;
                s->sel = 0;
                func_001BA7A0(9);
                s->timer = 10;
            }
        }
        break;
    case 3:
        if ((s->nav & 8) && s->sel == 0) {
            func_001B6E80(2, 0);
            s->sel = 1;
        } else if ((s->nav & 4) && s->sel == 1) {
            func_001B6E80(2, 0);
            s->sel = 0;
        } else if (s->pressed & 0x20) {
            func_001B6E80(0, 0);
            func_001C0940(3, 4);
            s->state = 4;
            break;
        }
        func_001B4340(-2);
        func_001B4300(0x14);
        func_001B4370(0);
        func_001B4380(0x80808080);
        func_001B4320(0x64, 0x90);
        func_001B43C0((u8 *)D_005595D0[0]);
        func_001B4320(0x64, 0xA4);
        func_001B43C0((u8 *)D_005595D0[1]);
        func_001B4320(0x64, 0xB8);
        func_001B43C0((u8 *)D_005595D0[2]);
        func_001B4320(0x64, 0xCC);
        func_001B43C0((u8 *)D_005595D0[3]);
        func_001B4320(0x118, 0xE0);
        if (s->sel == 0) {
            func_001B4380(0x80000080);
            func_001B43C0((u8 *)D_005606A0);
            func_001B4380(0x80808080);
            func_001B43C0((u8 *)D_005606A8);
        } else {
            func_001B4380(0x80808080);
            func_001B43C0((u8 *)D_005606B0);
            func_001B4380(0x80000080);
            func_001B43C0((u8 *)D_005606B8);
        }
        break;
    case 4:
        if (func_001C0C20() == 0) {
            if (func_001BF5B0() != 0) {
                D_002F88B0[0x20] = 0;
                D_002F88B0[0x22] = 0;
            } else if (D_002F88B0[0x22] == 1) {
                if (s->sel == 1) {
                    D_002F88B0[0x20] = 0;
                    D_002F88B0[0x22] = 0;
                } else {
                    D_002F88B0[0x20] = 1;
                }
            } else {
                D_002F88B0[0x20] = 0;
                D_002F88B0[0x22] = 0;
            }
            s->state = 5;
            s->sel = 0;
            func_001BA7A0(9);
            s->timer = 10;
        }
        break;
    case 5:
        if (--s->timer == 0) {
            if (D_002F88B0[0x20] != 0) {
                func_001B9140(3);
                func_001BA870();
                func_00542F70(w);
                D_002F88B0[0x22] = 0;
                D_002F88B0[0x20] = 1;
                w->unk70 = 1;
                return;
            }
            s->state = 6;
        }
        break;
    case 6:
        func_00542640(w);
        D_002F88D1[0] = s->unk8550;
        D_002F88B0[1] = s->unk8554;
        func_001D3500(D_003065DC[0] + 0x33F000);
        s->state++;
        s->sel = 0;
        if (func_001B6980(0xE) != 0) {
            s->timer = 1;
        } else {
            s->timer = 0x96;
            func_0034A3D0();
            s->moviePlaying = 1;
        }
        break;
    case 7:
        s->timer--;
        if (func_0034A290() == 2 || s->timer <= 0) {
            s->timer = 0;
            s->state++;
            func_001B87F0(0, 0x45, 0);
        }
        func_001BA7A0(8);
        break;
    case 8:
        func_00544620(w, s);
        if (s->state == 8 && s->moviePlaying != 0) {
            SELECT_MOVIE(s);
        }
        break;
    case 9:
        func_00545AB0(w, s);
        if (s->state == 9 && s->moviePlaying != 0) {
            SELECT_MOVIE(s);
        }
        break;
    case 10:
        func_00547340(w, s);
        if (s->state == 10 && s->moviePlaying != 0) {
            SELECT_MOVIE(s);
        }
        break;
    case 11:
        func_005482F0(w, s);
        break;
    case 12:
        s->state++;
        s->timer2 = 0x14;
        func_001BA7A0(7);
        break;
    case 13:
        func_00349E20();
        s->timer2 = 10;
        s->state++;
        break;
    case 14:
        if (--s->timer2 == 0) {
            func_00542F70(w);
            func_001BA870();
            func_005403B0(w);
            return;
        }
        return;
    }
    if (s->state >= 8 && s->state < 12 && s->moviePlaying != 0) {
        s->movieTimer++;
        switch (s->movieStep) {
        case 0:
            if (s->movieTimer >= 0x1A5E0) {
                func_00349E20();
                s->moviePlaying = 2;
                s->movieStep = 1;
            }
            break;
        case 1:
            if (s->movieTimer >= 0x1A5EA) {
                func_0034A3D0();
                s->moviePlaying = 1;
                s->movieTimer = 0;
                s->movieStep = 0;
            }
            break;
        }
    }
    if (s->state > 0 && s->state < 6) {
        func_00542FA0(s);
    }
}
#endif


void func_001AD470(void);
void func_001D55D0(s32 n);

/* Leaving the select screen: stop the music, free select.tex. */
void func_00542F70(TitleWork *w) {
    func_001AD470();
    func_001D55D0(0x1C);
}

extern Sprite2D D_005595B0;     /* the save data picture */

void func_00542FA0(SelectWork *s) {
    if (s->saveTex != 0) {
        func_001AE290();
        func_001AE9C0(0xD, 0);
        func_001AE9C0(0x5F, 5);
        func_001AE9C0(0x60, 0);
        func_001AE9C0(0x63, 0);
        func_001AE9C0(0x6C, 0);
        func_0019CD60(1, &s->saveTex);
        func_001AE9C0(4, s->saveTex);
        func_001AE430(4, 5);
        func_001AAB10(&D_005595B0);
    }
}

extern UiDef D_005584A8[];      /* the select screen's pictures */
extern UiDef D_005589C0[];
extern UiDef D_00559120[];
extern UiDef D_00559190[];
extern UiDef D_00558430[5];     /* a save file's frame, per kind */
extern UiDef D_00558494;        /* an empty file */
extern u8 D_00301D60[];         /* the system save */
extern u8 D_0030665B[];         /* (Options) */
s32 func_0034B080(s32 c);
s32 func_0034B0C0(s32 i);
s32 func_0034D610(s32 i);

#ifdef NON_MATCHING   /* first draft from m2c */
/* Sets the select screen up: its pictures, and the save files with their items. */
void func_00542640(TitleWork *w) {
    SelectWork *s;
    UiElem *e;
    UiDef *d;
    s32 i;
    s32 j;
    s32 k;
    s32 n;
    s32 m;
    s32 c;
    s32 v;
    f32 f;
    s32 cnt;

    D_002F88B0[3] = 0;
    s = D_003065FC[0];
    for (i = 0; i < 4; i++) {
        D_002F88B0[8 + i] = 0;
        ((s16 *)(D_002F88B0 + 0x10))[i] = 0;
        D_002F88B0[0xC + i] = 0;
    }
    D_002F88B0[4] = 1;
    D_002F88B0[5] = 0;
    e = &s->elems[35];
    d = D_005584A8;
    for (i = 6; i < 0x80 && d->page != 0xFF; i++, d++, e++) {
        UI_SET(e, d);
        e->color = -1;
    }
    e = &s->elems[128];
    d = D_005589C0;
    for (i = 0; i < 0x5D && d->page != 0xFF; i++, d++, e++) {
        UI_SET(e, d);
        e->color = -1;
    }
    d = D_00559120;
    for (i = 0; i < 4 && d->page != 0xFF; i++, d++, e++) {
        UI_SET(e, d);
        e->color = -1;
    }
    e = &s->elems[256];
    d = D_00559190;
    for (i = 0; i < 0x1B && d->page != 0xFF; i++, d++, e++) {
        UI_SET(e, d);
        e->color = -1;
    }
    for (i = 0; i < 35; i++) {
        s->file[i] = -1;
        s->score[i] = 0;
        s->unk1D0[i] = -1;
    }
    n = 0;
    for (i = 0; i < 5; i++) {
        if (((s32 *)(D_00301D60 + 0x4078))[i >> 5] & (1 << (i & 0x1F))) {
            s->file[n] = i;
            s->unk1D0[n] = func_0034B0C0(i);
            v = *(u16 *)(D_00301D60 + i * 0x98 + 0x23BA);
            if (v >= 10000) {
                v = 9999;
            }
            s->score[n] = v;
            s->itemCount[n] = 0;
            k = 0;
            for (j = 0; j < 0x40; j++) {
                if (((s32 *)(D_00301D60 + i * 8 + 0x51C))[j >> 5] & (1 << (j & 0x1F))) {
                    s->items[n][k] = j;
                    k++;
                    s->itemCount[n]++;
                }
            }
            cnt = s->itemCount[n];
            if (cnt < 13) {
                f = 0.0f;
            } else {
                f = 12.0f / (f32)cnt;
            }
            if (cnt - 12 <= 0) {
                s->unk8362[n] = 0;
                s->unk831C[n] = 0;
            } else {
                s->unk831C[n] = 200.0f * f;
                s->unk8362[n] = (200 - s->unk831C[n]) / (s16)(s->itemCount[n] - 12);
                s->unk831C[n] += (s16)(200 - (s->unk831C[n] + s->unk8362[n] * (s->itemCount[n] - 12)));
            }
            n++;
        }
        s->unk83AC[i] = func_0034D610(i & 0xFF) & 0xFF;
        for (j = 0; j < 8; j++) {
            u16 *q = (u16 *)(D_00301D60 + i * 0x98 + j * 8 + 0x23FC);

            if (q[0] != 0 || q[1] != 0 || q[2] != 0 || q[3] != 0) {
                s->unk8438[i][j] = 1;
            } else {
                s->unk8438[i][j] = 0;
            }
        }
    }
    s->files = n;
    if (!(D_002F88B0[1] & 1)) {
        if (s->file[n - 1] == 4) {
            if (s->score[n - 1] == 0) {
                s->cur = n - 1;
            } else {
                c = D_0030665B[0];
                for (k = 0; k < n && s->unk1D0[k] != c; k++) {
                }
                if (k < n) {
                    s->cur = func_0034B080(c);
                } else {
                    s->cur = 0;
                }
            }
        } else {
            s->cur = n - 1;
        }
    } else {
        s->cur = 0;
    }
    m = 0;
    for (i = 0; i < 35; i++) {
        c = s->file[i];
        if (c != -1) {
            d = c < 5 ? &D_00558430[c] : &D_00558494;
            e = &s->elems[m];
            UI_SET(e, d);
            if (i == 0) {
                e->color = -1;
            } else {
                e->color = 0xFF808080;
            }
            s->shown[i] = 1;
            m++;
        }
    }
    for (; m < 35; m++) {
        e = &s->elems[m];
        UI_SET(e, &D_00558494);
        e->color = 0xFF808080;
        s->shown[m] = -1;
    }
}
#endif

void func_00348DA0(void);
void func_00543A40(SelectWork *s);
void func_00543B40(SelectWork *s, s32 dir);
void func_00543DA0(SelectWork *s, s32 dir);
void func_005440C0(UiElem *e, s32 n);
void func_00544310(SelectWork *s);
void func_00544370(SelectWork *s);
void func_00544390(SelectWork *s);
void func_00544430(SelectWork *s);
void func_00544B80(SelectWork *s);

#ifdef NON_MATCHING   /* first draft from m2c */
/* Select step 8: choose a save file (a wheel of files; the items list on request). */
void func_00544620(TitleWork *w, SelectWork *s) {
    s32 i;
    s32 k;
    s32 v;

    switch (s->sel) {
    case 0:
        for (i = 0; i < 35; i++) {
            s->order[i] = i;
            s->order2[i] = i;
        }
        for (k = 0; k < s->cur; k++) {
            for (i = 0; i < 35; i++) {
                v = s->order[i] - 1;
                if (v < 0) {
                    v += 35;
                }
                s->order[i] = v;
                s->order2[i] = v;
            }
        }
        func_00543A40(s);
        for (i = 35; i < 62; i++) {
            s->elems[i].visible = 1;
        }
        for (i = 62; i < 83; i++) {
            s->elems[i].visible = 0;
        }
        s->elems[36].visible = 0;
        s->elems[37].visible = 0;
        s->unk24 = 0;
        s->sel++;
        s->timer2 = 0x14;
        func_001BA7A0(8);
        func_00348DA0();
        break;
    case 1:
        if (s->timer2 != 0) {
            s->timer2--;
        } else if (s->pressed & 0x200) {
            s->sel = 3;
            func_00544430(s);
            func_001B6EF0(2);
            break;
        } else if (s->pressed & 0x10) {
            func_00544370(s);
            func_001B6EF0(1);
            s->unk24 = 1;
            s->sel = 4;
            s->timer2 = 0x14;
            func_001BA7A0(7);
            func_001AD680(0, 0, 0x14);
            break;
        } else if (s->pressed & 0x20) {
            func_00544370(s);
            func_001B6EF0(7);
            s->sel = 4;
            s->timer2 = 0x14;
            func_001BA7A0(7);
            break;
        } else if (((s->nav & 8) && s->cur != s->files - 1 && (s->sub = 0, 1)) ||
                   ((s->nav & 4) && s->cur != 0 && (s->sub = 1, 1))) {
            func_00544370(s);
            func_001B6EF0(6);
            s->timer2 = 8;
            func_00543B40(s, s->sub);
            func_00544B80(s);
            s->sel = 2;
            return;
        }
        func_005440C0(&s->elems[36], 0x24);
        func_005440C0(&s->elems[37], 0x25);
        s->elems[36].visible = 1;
        s->elems[37].visible = 1;
        if (s->cur == 0) {
            s->elems[36].visible = 0;
        }
        if (s->cur == s->files - 1) {
            s->elems[37].visible = 0;
        }
        break;
    case 2:
        func_00543DA0(s, s->sub);
        if (--s->timer2 == 0) {
            if (s->sub == 0) {
                s->cur++;
            } else {
                s->cur--;
            }
            for (i = 0; i < 35; i++) {
                s->order[i] = s->order2[i];
            }
            func_00543A40(s);
            s->sel--;
        }
        break;
    case 3:
        if (s->pressed & 0x200) {
            s->sel = 1;
            func_00544390(s);
            func_001B6EF0(2);
        } else if (s->nav & 1) {
            if (s->scroll > 0) {
                s->scroll--;
                func_001B6EF0(2);
            }
        } else if (s->nav & 2) {
            if (s->scroll + 12 < s->itemCount[s->file[s->cur]]) {
                s->scroll++;
                func_001B6EF0(2);
            }
        }
        break;
    case 4:
        if (--s->timer2 == 0) {
            v = 1;
            if (s->unk24 == 1) {
                if (func_001B6980(0xE) != 0) {
                    v = 1;
                } else {
                    func_00349E20();
                    v = 10;
                }
            }
            s->timer2 = v;
            s->sel++;
            return;
        }
        break;
    case 5:
        if (--s->timer2 == 0) {
            if (s->unk24 == 1) {
                func_00542F70(w);
                func_001BA870();
                func_005403B0(w);
            } else {
                D_002F88B0[6] = s->file[s->cur];
                D_002F88B0[3] = func_0034B0C0(D_002F88B0[6]);
                func_00544310(s);
            }
        }
        return;
    }
    func_00544B80(s);
}
#endif


void func_00544310(SelectWork *s) {
    s->state = 9;
    s->sel = 0;
    s->sub = 0;
}

/* Hides the wheel's arrows. */
void func_00544370(SelectWork *s) {
    s->elems[36].anim = 0;
    s->elems[37].anim = 0;
    s->elems[36].visible = 0;
    s->elems[37].visible = 0;
}

extern s32 D_005593B0[35], D_00559400[35], D_00559450[35], D_005594A0[35];   /* wheel places */

#ifdef NON_MATCHING   /* first draft from m2c */
/* Puts the files on the wheel, the current one in front. */
void func_00543A40(SelectWork *s) {
    UiElem *e;
    s32 t = s->cur;
    s32 k;

    for (k = 0; k < 35; k++) {
        e = &s->elems[t];
        e->visible = 1;
        e->bx = D_005593B0[k];
        e->by = D_00559400[k];
        e->bw = D_00559450[k];
        e->bh = D_005594A0[k];
        if (e->bx < 0) {
            if (e->bx + e->bw <= 0) {
                e->visible = 0;
            }
        } else if (e->bx >= 0x280) {
            e->visible = 0;
        }
        if (t == s->cur) {
            e->color = -1;
        } else {
            e->color = 0xFF808080;
        }
        if (++t == 35) {
            t = 0;
        }
    }
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
/* Back from the item list to the wheel. */
void func_00544390(SelectWork *s) {
    s32 i;

    func_00543A40(s);
    for (i = 0; i < 0x3B; i++) {
        s->elems[36 + i].visible = 1;
    }
    for (i = 62; i < 99; i++) {
        s->elems[i].visible = 0;
    }
    for (i = 35; i < 62; i++) {
        s->elems[i].visible = 1;
    }
    s->elems[36].visible = 0;
    s->elems[37].visible = 0;
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
/* The current file's item list. */
void func_00544430(SelectWork *s) {
    s32 i;

    for (i = 35; i < 62; i++) {
        s->elems[i].visible = 0;
    }
    for (i = 0; i < 6; i++) {
        s->elems[i].visible = 0;
    }
    for (i = 36; i < 0x3B; i++) {
        s->elems[i].visible = 0;
    }
    for (i = 62; i < 99; i++) {
        s->elems[i].visible = 1;
    }
    for (i = 0x53; i < 0x58; i++) {
        if (i - 0x53 == s->file[s->cur]) {
            s->elems[i].visible = 1;
        } else {
            s->elems[i].visible = 0;
        }
    }
    s->elems[67].bh = s->unk831C[s->file[s->cur]];
    s->scroll = 0;
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
/* Starts a turn of the wheel (dir 1: backwards). */
void func_00543B40(SelectWork *s, s32 dir) {
    s32 i;
    s32 v;

    if (dir == 1) {
        for (i = 0; i < 35; i++) {
            v = s->order[i] + 1;
            if (v == 35) {
                v = 0;
            }
            s->order2[i] = v;
        }
    } else {
        for (i = 0; i < 35; i++) {
            v = s->order[i] - 1;
            if (v < 0) {
                v += 35;
            }
            s->order2[i] = v;
        }
    }
    for (i = 0; i < 35; i++) {
        s->fx[i] = (s16)D_005593B0[s->order[i]];
        s->tx[i] = (s16)D_005593B0[s->order2[i]];
        s->angle[i] = 0;
        if (dir == 1) {
            s->dx[i] = (s->tx[i] - s->fx[i]) / 8.0f;
        } else {
            s->dx[i] = -1.0f * ((s->fx[i] - s->tx[i]) / 8.0f);
        }
        s->speed[i] = 0x1000;
        s->dy[i] = (f32)((s16)D_00559400[s->order2[i]] - (s16)D_00559400[s->order[i]]) / 8.0f;
        s->dh[i] = (f32)((s16)D_005594A0[s->order2[i]] - (s16)D_005594A0[s->order[i]]) / 4.0f;
        s->dw[i] = (f32)((s16)D_00559450[s->order2[i]] - (s16)D_00559450[s->order[i]]) / 4.0f;
        s->elems[i].color = (s->elems[i].color & 0xFF000000) | 0x808080;
    }
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
/* One frame of the wheel's turn. */
void func_00543DA0(SelectWork *s, s32 dir) {
    UiElem *e = s->elems;
    s32 i;
    s16 t;

    for (i = 0; i < 35; i++, e++) {
        e->visible = 1;
        if (s->fx[i] < 800.0f && !(s->fx[i] <= -300.0f)) {
            if (!(s->tx[i] < 800.0f)) {
                e->visible = 0;
            }
            s->angle[i] = (s->angle[i] + s->speed[i]) & 0x7FFF;
            s->fx[i] += s->dx[i];
            if (!(s->dx[i] < 0.0f)) {
                if (!(s->fx[i] < s->tx[i])) {
                    s->fx[i] = s->tx[i];
                    s->dx[i] = 0.0f;
                    s->speed[i] = 0;
                    if (s->fx[i] == (f32)(s16)D_005593B0[0]) {
                        s->elems[i].color = -1;
                    }
                }
            } else if (s->fx[i] <= s->tx[i]) {
                s->fx[i] = s->tx[i];
                s->dx[i] = 0.0f;
                s->speed[i] = 0;
                if (s->fx[i] == (f32)(s16)D_005593B0[0]) {
                    s->elems[i].color = -1;
                }
            }
            e->bx = s->fx[i];
            e->bw = (f32)e->bw + s->dw[i];
            if (!(s->dw[i] < 0.0f)) {
                if (e->bw >= (s16)D_00559450[s->order2[i]]) {
                    s->dw[i] = 0.0f;
                    e->bw = D_00559450[s->order2[i]];
                }
            } else if ((s16)D_00559450[s->order2[i]] >= e->bw) {
                s->dw[i] = 0.0f;
                e->bw = D_00559450[s->order2[i]];
            }
            e->bh = (f32)e->bh + s->dh[i];
            if (!(s->dh[i] < 0.0f)) {
                if (e->bh >= (s16)D_005594A0[s->order2[i]]) {
                    s->dh[i] = 0.0f;
                    e->bh = D_005594A0[s->order2[i]];
                }
            } else if ((s16)D_005594A0[s->order2[i]] >= e->bh) {
                s->dh[i] = 0.0f;
                e->bh = D_005594A0[s->order2[i]];
            }
            e->by = 0x110 - e->bh;
        }
        t = e->bx;
        if (t < 0) {
            if (t + e->bw <= 0) {
                e->visible = 0;
            }
        } else if (t >= 0x280) {
            e->visible = 0;
        }
    }
}
#endif

extern f32 D_00239CB8;          /* 2 pi */

/* The wheel's arrows (pictures 0x24, 0x25): fade in, hold, fade out. */
void func_005440C0(UiElem *e, s32 n) {
    if (n == 0x24 || n == 0x25) {
        switch (e->anim) {
        case 0:
            e->visible = 1;
            e->alpha = 0;
            e->phase = 0;
            e->anim++;
            break;
        case 1:
            e->alpha = 255.0f * func_00191B60(D_00239CB8 * (f32)(u16)e->phase / 65536.0f);
            if (e->alpha >= 0xFF) {
                e->alpha = 0xFF;
                e->timer = 0x10;
                e->phase = 0x4000;
                e->anim++;
            } else {
                e->phase = (e->phase + 0x400) & 0xFFFF;
            }
            break;
        case 2:
            if (--e->timer == 0) {
                e->anim++;
            }
            break;
        case 3:
            e->alpha = 255.0f * func_00191B60(D_00239CB8 * (f32)(u16)e->phase / 65536.0f);
            if (e->alpha <= 0) {
                e->alpha = 0;
                e->phase = 0;
                e->anim = 1;
            } else {
                e->phase = (e->phase + 0x400) & 0xFFFF;
            }
            break;
        }
        e->color = (e->alpha << 24) | 0xFFFFFF;
    }
}

void func_00548A00(SelectWork *s, UiElem *e, s32 n);   /* draws n pictures */
char **func_001C0260(s32 file);                        /* a file's description lines */
char *func_001C02A0(s32 file, s32 item);               /* an item's name */
s32 sprintf(char *dst, const char *fmt, ...);
extern char D_00560720[];       /* no items */
extern char D_00560740[];       /* "%3d%%" (found) */
extern char *D_00558410[];      /* debug: file names */

#ifdef NON_MATCHING   /* first draft from m2c */
/* Draws the file wheel, or the item list. */
void func_00544B80(SelectWork *s) {
    UiElem tmp;
    char buf[0x30];
    UiElem *e;
    char **lines;
    s32 file;
    s32 t;
    s32 i;
    s32 y;

    if (s->sel == 3) {
        s->elems[67].visible = 1;
        s->elems[67].bh = s->unk831C[s->file[s->cur]];
        if (s->elems[67].bh != 0) {
            s->elems[67].by = s->unk8362[s->file[s->cur]] * s->scroll + 0x9A;
            func_00548A00(s, &s->elems[67], 1);
        }
        s->elems[67].visible = 0;
        for (i = 0; i < 8; i++) {
            if (s->unk8438[s->file[s->cur]][i] != 0) {
                s->elems[90 + i].color = -1;
            } else {
                s->elems[90 + i].color = 0xFF404040;
            }
        }
    }
    t = s->cur - 2;
    if (t < 0) {
        t += 35;
    }
    for (i = 0; i < 6; i++) {
        e = &s->elems[t];
        if (s->sel == 3) {
            e->visible = 0;
        } else {
            if (e->page == 5) {
                e->frame = (e->frame + 1) & 7;
                tmp = *e;
                tmp.page = e->frame + 0x11;
                func_00548A00(s, &tmp, 1);
            } else {
                func_00548A00(s, e, 1);
            }
            if (++t == 35) {
                t = 0;
            }
        }
    }
    func_00548A00(s, &s->elems[35], 0x40);
    if (s->cur < 5) {
        func_001B4340(-2);
        func_001B4370(0);
        func_001B4380(0x80808080);
        if (s->sel == 3) {
            file = s->file[s->cur];
            if (s->itemCount[file] == 0) {
                func_001B4300(0x14);
                func_001B4320(0x90, 0xEA);
                func_001B43C0((u8 *)D_00560720);
            } else {
                func_001B4300(0x14);
                for (i = s->scroll, y = 0x90; i < s->itemCount[s->file[s->cur]] && i - s->scroll < 12; i++, y += 0x14) {
                    func_001B4320(0x90, y);
                    file = s->file[s->cur];
                    func_001B43C0((u8 *)func_001C02A0(file, s->items[file][i]));
                }
                func_001B4320(0x1F4, 0x6E);
                sprintf(buf, D_00560740, s->unk83AC[s->file[s->cur]]);
                func_001B43C0((u8 *)buf);
            }
            return;
        }
        func_001B4300(0x14);
        func_001B4320(0x54, 0x12A);
        for (lines = func_001C0260(s->file[s->cur]); *lines != NULL; lines++) {
            func_001B43C0((u8 *)*lines);
        }
        if (func_001B6980(0) != 0) {
            func_001B43C0((u8 *)D_00558410[s->file[s->cur]]);
        }
    }
}
#endif


typedef struct {
    s16 x0, y0, x1, y1, x2, y2;
    u32 color;
    s16 u0, v0, u1, v1, u2, v2;
} Tri2D;

typedef struct {
    s16 x0, y0, x1, y1;
    u32 color;
} Rect2D;

void func_00548EB0(Sprite2D *s, s32 off);
s32 func_001AAE10(Rect2D *r);
s32 func_001AB4F0(Tri2D *t);

#ifdef NON_MATCHING   /* first draft from m2c */
/* Draws n of the select screen's pictures: blend mode by kind, mirrored (c) or flipped (d). */
void func_00548A00(SelectWork *s, UiElem *e, s32 n) {
    Sprite2D sp;
    Tri2D tri;
    Rect2D r;
    s32 tex;
    s32 i;

    func_001AE290();
    func_001AE9C0(0x5F, 5);
    func_001AE9C0(0x60, 0);
    for (i = 0; i < n; i++, e++) {
        if (e->visible == 0) {
            continue;
        }
        tex = s->tex[e->page];
        switch (e->b) {
        case 0:
            func_001AE9C0(0x63, 0);
            func_001AE9C0(0x6C, 0);
            func_001AE430(4, 5);
            break;
        case 1:
            func_001AE9C0(0x63, 0);
            func_001AE9C0(0x6C, 0);
            func_001AE430(4, 1);
            break;
        case 2:
            func_001AE9C0(0x6C, 1);
            func_001AE9C0(0x63, 0x10000);
            func_001AE9C0(0x5E, 0x12);
            break;
        case 3:
        case 4:
            func_001AE9C0(0x6C, 1);
            func_001AE9C0(0x63, 0x10000);
            func_001AE430(4, 5);
            break;
        }
        if (tex != 0) {
            func_0019CD60(1, &tex);
            func_001AE9C0(4, tex);
            sp.color = e->color;
            sp.x = e->bx;
            sp.y = e->by;
            sp.w = e->bw;
            sp.h = e->bh;
            if (sp.x < 0x280 && sp.x + sp.w > 0 && sp.y < 0x1E0 && sp.y + sp.h > 0) {
                if (e->c == 1) {
                    sp.u1 = e->u0;
                    sp.u0 = sp.u1 + e->u1 - 1;
                    sp.v0 = e->v0;
                    sp.v1 = sp.v0 + e->v1 - 1;
                } else {
                    sp.u0 = e->u0;
                    sp.v0 = e->v0;
                    sp.u1 = sp.u0 + e->u1 - 1;
                    sp.v1 = sp.v0 + e->v1 - 1;
                }
                if (e->d == 0) {
                    switch (e->b) {
                    case 2:
                        func_00548EB0(&sp, -0x18);
                        break;
                    case 3:
                        func_00548EB0(&sp, -0x20);
                        break;
                    case 4:
                        func_00548EB0(&sp, -0x10);
                        break;
                    default:
                        func_001AAB10(&sp);
                        break;
                    }
                } else {
                    tri.color = sp.color;
                    tri.x0 = sp.x;
                    tri.y0 = sp.y;
                    tri.u0 = sp.u0;
                    tri.v0 = sp.v1;
                    tri.x1 = sp.x + sp.w;
                    tri.y1 = sp.y;
                    tri.u1 = sp.u1;
                    tri.v1 = sp.v1;
                    tri.x2 = sp.x;
                    tri.y2 = sp.y + sp.h;
                    tri.u2 = sp.u0;
                    tri.v2 = sp.v0;
                    func_001AB4F0(&tri);
                    tri.x0 = sp.x + sp.w;
                    tri.y0 = sp.y + sp.h;
                    tri.u0 = sp.u1;
                    tri.v0 = sp.v0;
                    func_001AB4F0(&tri);
                }
            }
        } else {
            r.x0 = e->bx;
            r.y0 = e->by;
            r.x1 = r.x0 + e->bw;
            r.y1 = r.y0 + e->bh;
            r.color = e->color;
            func_001AAE10(&r);
        }
    }
}
#endif

/* --- select step 9: the character (and costume) --- */

extern u8 *D_00558300[];        /* per save file: the character/costume pairs */
extern u8 D_00358F00[];         /* SUBMAIN: the extra characters, 12 bytes each */
void func_005442F0(SelectWork *s);
void func_00544330(SelectWork *s);
void func_00544540(SelectWork *s);
void func_00544570(SelectWork *s);
void func_00544FA0(UiElem *e, s32 n);
void func_00545290(TitleWork *w, SelectWork *s);
void func_00545350(TitleWork *w, SelectWork *s);
void func_00545530(TitleWork *w, SelectWork *s);
void func_00545780(TitleWork *w, SelectWork *s);
void func_00545840(UiElem *e, s32 n);
void func_00546500(TitleWork *w, SelectWork *s);

/* Hides the arrows and the characters' highlights. */
void func_00545A70(SelectWork *s) {
    s32 i;

    s->elems[151].anim = 0;
    s->elems[152].anim = 0;
    s->elems[151].visible = 0;
    s->elems[152].visible = 0;
    for (i = 0; i < 8; i++) {
        s->elems[128 + i].anim = 0;
        s->elems[128 + i].visible = 0;
    }
}

#ifdef NON_MATCHING   /* first draft from m2c */
void func_00545AB0(TitleWork *w, SelectWork *s) {
    s32 n;
    s32 i;
    s32 j;
    s32 k;
    s32 v;
    u8 c;
    u8 *pair;

    if (--s->blink < 0) {
        s->blink += 0x20;
    }
    s->frames++;
    switch (s->sel) {
    case 0:
        D_002F88B0[4] = 1;
        s->unk24 = 0;
        if (s->back != 0) {
            func_00545A70(s);
            for (i = 9; i < 0x38; i++) {
                s->elems[128 + i].visible = 1;
            }
            *(s16 *)(D_002F88B0 + 0x10) = 0;
            D_002F88B0[5] = 0;
            s->back = 0;
            s->timer2 = 1;
            s->sel++;
            s->frames = 0;
        } else {
            s->extraCount = 0;
            for (i = 0; i < 0x80; i++) {
                if (((s32 *)(D_00301D60 + 0xC))[i >> 5] & (1 << (i & 0x1F))) {
                    s->extras[s->extraCount] = i;
                    s->extraCount++;
                }
            }
            for (i = 0; i < 8; i++) {
                s->costumes[i] = 0;
                s->costume[i][0] = 1;
                for (j = 0; j < 2; j++) {
                    if (D_00301D60[i + 4] & (1 << j)) {
                        s->costume[i][j + 1] = 1;
                    } else {
                        s->costume[i][j + 1] = 0;
                    }
                }
                for (k = 0; k < 3; k++) {
                    if (s->costume[i][k] != 0) {
                        s->costumes[i]++;
                    }
                }
            }
            for (i = 0; i < 0x38; i++) {
                func_00544FA0(&s->elems[128 + i], i);
            }
            w->unkC = 0;
            w->unkD[0] = 0;
            func_00545290(w, s);
            func_00545780(w, s);
            s->timer2 = 0x14;
            func_001BA7A0(8);
            s->sel++;
            s->frames = 0;
            func_00348DA0();
        }
        break;
    case 1:
        if (s->timer2 != 0) {
            s->timer2--;
        } else if (s->pressed & 0x200) {
            s->sel = 6;
            func_00544570(s);
            func_001B6EF0(2);
        } else {
            n = (s->extraCount == 0) ? 8 : 9;
            c = w->unkC;
            if (s->nav & 4) {
                c++;
                if (n - 1 < c) {
                    c = 0;
                }
                s->sub = 0;
            } else if (s->nav & 8) {
                c--;
                if ((s8)c < 0) {
                    c = n - 1;
                }
                s->sub = 1;
            }
            if (c != w->unkC) {
                func_00545A70(s);
                func_001B6EF0(5);
                s->unk7 = 0;
                func_00545350(w, s);
                s->timer = 8;
                s->sel++;
                s->frames = 0;
            } else {
                if (w->unkC == 8) {
                    v = s->extraCount;
                } else {
                    v = s->costumes[w->unkC];
                }
                if (w->unkC == 8 || v != 1) {
                    if (s->nav & 1) {
                        if (w->unkD[0] != 0) {
                            w->unkD[0]--;
                        } else {
                            w->unkD[0] = v - 1;
                        }
                        func_001B6EF0(2);
                    }
                    if (s->nav & 2) {
                        if (w->unkD[0] != v - 1) {
                            w->unkD[0]++;
                        } else {
                            w->unkD[0] = 0;
                        }
                        func_001B6EF0(2);
                    }
                }
                if (s->pressed & 0x10) {
                    func_00545A70(s);
                    func_001B6EF0(1);
                    s->unk24 = 1;
                    s->sel = 4;
                    s->timer2 = 0x14;
                    func_001BA7A0(7);
                } else if (s->pressed & 0x20) {
                    func_00545A70(s);
                    func_001B6EF0(w->unkC + 8);
                    s->sel = 3;
                    func_001BA7A0(4);
                    s->timer2 = 8;
                } else if (s->sel == 1) {
                    func_00545840(&s->elems[151], 0x17);
                    func_00545840(&s->elems[152], 0x18);
                    s->elems[151].visible = 1;
                    s->elems[152].visible = 1;
                    for (i = 0; i < 8; i++) {
                        func_00545840(&s->elems[128 + i], i);
                        s->elems[128 + i].visible = 1;
                    }
                }
            }
        }
        break;
    case 2:
        n = 8;
        if (s->extraCount != 0) {
            n = 9;
        }
        if (--s->timer == 0) {
            if (s->sub == 0) {
                v = w->unkC - 1;
                if (v < 0) {
                    v += n;
                }
                w->unkC = v;
                for (i = 0; i < n; i++) {
                    if (n - 1 < ++s->wheel[i]) {
                        s->wheel[i] = 0;
                    }
                }
            } else {
                v = w->unkC + 1;
                if (v >= n) {
                    v = 0;
                }
                w->unkC = v;
                for (i = 0; i < n; i++) {
                    if (--s->wheel[i] < 0) {
                        s->wheel[i] += n;
                    }
                }
            }
            w->unkD[0] = 0;
            s->sel--;
        } else {
            func_00545530(w, s);
        }
        break;
    case 3:
        if (--s->timer2 == 0) {
            if (s->unk24 == 1) {
                s->timer2 = 0x14;
                func_001BA7A0(7);
                s->sel++;
            } else {
                s->timer2 = 1;
                s->sel = 5;
            }
        } else if (s->unk24 != 1) {
            for (i = 0; i < 0x80; i++) {
                if (i != 0x16 && i != 0x15 && i != 0x14 && i != 0x13 && i != 0x12) {
                    s->elems[128 + i].visible = 0;
                } else {
                    s->elems[128 + i].visible = 1;
                }
            }
        }
        break;
    case 4:
        if (--s->timer2 == 0) {
            v = 1;
            if (s->unk24 == 1) {
            } else if (func_001B6980(0xE) != 0) {
                v = 1;
            } else {
                func_00349E20();
                v = 10;
            }
            s->timer2 = v;
            s->sel++;
            return;
        }
        break;
    case 5:
        if (--s->timer2 == 0) {
            if (s->unk24 == 1) {
                func_005442F0(s);
                return;
            }
            if (w->unkC == 8) {
                D_002F88B0[8] = D_00358F00[s->extras[w->unkD[0]] * 12 + 2];
                D_002F88B0[0xC] = 0;
                *(s16 *)(D_002F88B0 + 0x10) = *(s16 *)(D_00358F00 + s->extras[w->unkD[0]] * 12) + 1;
            } else {
                k = 0;
                D_002F88B0[8] = w->unkC;
                for (j = 0; j < 3; j++) {
                    if (s->costume[w->unkC][j] != 0) {
                        if (k == w->unkD[0]) {
                            break;
                        }
                        k++;
                    }
                }
                D_002F88B0[0xC] = j;
            }
            D_002F88B0[5] = 0;
            for (i = 0; i < 4; i++) {
                D_002F88B0[0x18 + i] = 0;
                D_002F88B0[0x1A + i] = 0;
                ((s16 *)(D_002F88B0 + 0x1C))[i] = 0;
            }
            D_002F88B0[5] = 2;
            pair = D_00558300[D_002F88B0[6]] + D_002F88B0[8] * 2;
            D_002F88B0[0x18] = pair[0];
            D_002F88B0[0x19] = pair[1];
            func_00546500(w, s);
            func_00544330(s);
            return;
        }
        return;
    case 6:
        if (s->pressed & 0x200) {
            s->sel = 1;
            func_00544540(s);
            func_001B6EF0(2);
        } else if (s->nav & 1) {
            if (s->scroll > 0) {
                s->scroll--;
                func_001B6EF0(2);
            }
        } else if (s->nav & 2) {
            if (s->scroll + 12 < s->itemCount[s->cur]) {
                s->scroll++;
                func_001B6EF0(2);
            }
        }
        break;
    }
    func_00546500(w, s);
}
#endif


void func_005442F0(SelectWork *s) {
    s->state = 8;
    s->sel = 0;
    s->sub = 0;
}

void func_00544330(SelectWork *s) {
    s->state = 10;
    s->sel = 0;
    s->sub = 0;
}

#ifdef NON_MATCHING   /* first draft from m2c */
/* The character's item list off, and on. */
void func_00544540(SelectWork *s) {
    s32 i;

    for (i = 0x38; i < 0x5D; i++) {
        s->elems[i + 128].visible = 0;
    }
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
void func_00544570(SelectWork *s) {
    s32 i;

    for (i = 0x38; i < 0x5D; i++) {
        s->elems[i + 128].visible = 1;
    }
    for (i = 0x4D; i < 0x52; i++) {
        if (i - 0x4D == s->file[s->cur]) {
            s->elems[i + 128].visible = 1;
        } else {
            s->elems[i + 128].visible = 0;
        }
    }
    s->elems[189].bh = s->unk831C[s->file[s->cur]];
    s->scroll = 0;
}
#endif

extern s16 D_005594F0[][2];     /* the character wheel's places */

#ifdef NON_MATCHING   /* first draft from m2c */
/* The character screen's pictures: which are shown at first, and where. */
void func_00544FA0(UiElem *e, s32 n) {
    switch (n) {
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
    case 0x20:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2A:
    case 0x2B:
    case 0x2C:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x30:
    case 0x31:
    case 0x32:
    case 0x33:
    case 0x34:
    case 0x35:
    case 0x36:
    case 0x37:
    case 0x5D:
    case 0x5E:
    case 0x5F:
    case 0x60:
        e->visible = 1;
        break;
    case 0x17:
    case 0x18:
        e->bw = 0x1E;
        e->bh = 0x1E;
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        e->visible = 0;
        e->bx = D_005594F0[2][0];
        e->by = D_005594F0[2][1];
        break;
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
        e->visible = 1;
        e->bx = D_005594F0[n - 9][0];
        e->by = D_005594F0[n - 9][1];
        break;
    default:
        e->visible = 0;
        break;
    }
}
#endif

extern s32 D_00558B28;

#ifdef NON_MATCHING   /* first draft from m2c */
/* Puts the characters' portraits on the wheel, the chosen one in the middle. */
void func_00545290(TitleWork *w, SelectWork *s) {
    UiElem *e;
    UiDef *d;
    s32 end = 0x11;
    s32 n;
    s32 t;
    s32 i;

    if (s->extraCount != 0) {
        end = 0x12;
        n = 9;
    } else {
        n = 8;
    }
    t = w->unkC - 2;
    if (t < 0) {
        t += n;
    }
    e = &s->elems[137];
    for (i = 9; i < end; i++) {
        d = &D_005589C0[t + 9];
        if (d->page == 0xFF) {
            break;
        }
        t++;
        e->u0 = d->u0;
        e->v0 = d->v0;
        e++;
        if (t >= n) {
            t = 0;
        }
        s->wheel[i - 9] = i - 9;
    }
    s->elems[146].page = D_00558B28;
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
/* The character wheel at rest, the first character in front. */
void func_00545780(TitleWork *w, SelectWork *s) {
    s32 last = 7;
    s32 n;
    s32 i;
    u8 t;

    if (s->extraCount != 0) {
        last = 8;
        n = 9;
    } else {
        n = 8;
    }
    w->unkC = 0;
    w->unkD[0] = 0;
    for (i = 0; i < n; i++) {
        s->wheel[i] = i - w->unkC;
        if (s->wheel[i] < 0) {
            s->wheel[i] += n;
        }
    }
    t = w->unkC;
    for (i = 0; i < n; i++) {
        s->elems[137 + t].bx = D_005594F0[i][0];
        t++;
        if (last < t) {
            t = 0;
        }
    }
}
#endif

/* The character wheel's arrows (pictures 0x17, 0x18): fade in, hold, fade out. */
void func_00545840(UiElem *e, s32 n) {
    if (n == 0x17 || n == 0x18) {
        switch (e->anim) {
        case 0:
            e->visible = 1;
            e->alpha = 0;
            e->phase = 0;
            e->anim++;
            break;
        case 1:
            e->alpha = 255.0f * func_00191B60(D_00239CB8 * (f32)(u16)e->phase / 65536.0f);
            if (e->alpha >= 0xFF) {
                e->alpha = 0xFF;
                e->timer = 0x10;
                e->phase = 0x4000;
                e->anim++;
            } else {
                e->phase = (e->phase + 0x400) & 0xFFFF;
            }
            break;
        case 2:
            if (--e->timer == 0) {
                e->anim++;
            }
            break;
        case 3:
            e->alpha = 255.0f * func_00191B60(D_00239CB8 * (f32)(u16)e->phase / 65536.0f);
            if (e->alpha <= 0) {
                e->alpha = 0;
                e->phase = 0;
                e->anim = 1;
            } else {
                e->phase = (e->phase + 0x400) & 0xFFFF;
            }
            break;
        }
        e->color = (e->alpha << 24) | 0xFFFFFF;
    }
}

#ifdef NON_MATCHING   /* first draft from m2c */
/* Starts a turn of the character wheel (sub 0: to the right). */
void func_00545350(TitleWork *w, SelectWork *s) {
    s32 last = 7;
    s32 n;
    s32 i;
    s32 from;
    s32 to;
    f32 d;
    f32 right = 0x28A;

    if (s->extraCount != 0) {
        last = 8;
        n = 9;
    } else {
        n = 8;
    }
    for (i = 0; i < n; i++) {
        from = s->wheel[i];
        if (s->sub == 0) {
            to = from + 1;
            if (last < to) {
                to = 0;
            }
        } else {
            to = from - 1;
            if (to < 0) {
                to = last;
            }
        }
        s->cfx[i] = D_005594F0[from][0];
        s->ctx[i] = D_005594F0[to][0];
        s->cangle[i] = 0;
        if (s->sub == 0) {
            if (s->wheel[i] == last) {
                s->unk7 = 1;
                d = (right + s->elems[137 + i].w - s->cfx[i]) + (s->ctx[i] + s->elems[137 + i].w);
            } else {
                d = s->ctx[i] - s->cfx[i];
            }
            s->cdx[i] = 2.0f * (d / 8.0f);
        } else {
            if (s->wheel[i] == 0) {
                s->unk7 = 1;
                d = s->cfx[i] + ((right - s->ctx[i]) + s->elems[137 + i].w);
            } else {
                d = s->cfx[i] - s->ctx[i];
            }
            s->cdx[i] = -1.0f * (2.0f * (d / 8.0f));
        }
        s->cspeed[i] = 0x1000;
    }
    s->elems[151].visible = 0;
    s->elems[152].visible = 0;
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
/* One frame of the character wheel's turn. */
void func_00545530(TitleWork *w, SelectWork *s) {
    UiElem *e;
    s32 last;
    s32 n;
    s32 i;

    if (s->extraCount != 0) {
        last = 8;
        n = 9;
    } else {
        last = 7;
        n = 8;
    }
    for (i = 0; i < n; i++) {
        e = &s->elems[137 + i];
        s->cangle[i] = (s->cangle[i] + s->cspeed[i]) & 0x7FFF;
        s->cfx[i] += s->cdx[i] * func_00191B60(D_00239CB8 * (f32)(u16)s->cangle[i] / 65536.0f);
        if (s->sub == 0) {
            if (s->wheel[i] == last && (f32)0x28A <= s->cfx[i]) {
                s->cfx[i] = -1.0f * (f32)e->w;
                s->unk7 = 0;
            }
            if (s->unk7 == 0 && !(s->cfx[i] < s->ctx[i])) {
                s->cfx[i] = s->ctx[i];
                s->cdx[i] = 0.0f;
                s->cspeed[i] = 0;
            }
        } else {
            if (s->wheel[i] == 0 && !(-1.0f * (f32)e->w < s->cfx[i])) {
                s->cfx[i] = 0x28A;
                s->unk7 = 0;
            }
            if (s->unk7 == 0 && s->cfx[i] <= s->ctx[i]) {
                s->cfx[i] = s->ctx[i];
                s->cdx[i] = 0.0f;
                s->cspeed[i] = 0;
            }
        }
        e->bx = s->cfx[i];
    }
}
#endif

extern s16 D_00559520[5];       /* the costume arrows' places */
extern s16 D_0055952A[5];
extern s16 D_005583F0[5][3];    /* the profile lines: x, y, font size */
extern char *D_00558330[];      /* profile text, five lines per character */
extern char *D_00558318[];      /* costume names */
extern s16 D_0023A438[2];       /* where the costume name goes */
char *func_001C0180(s32 n);     /* an extra character's name */
s32 strlen(const char *s);

#ifdef NON_MATCHING   /* first draft from m2c */
/* Draws the character screen (or the item list, sel 6). */
void func_00546500(TitleWork *w, SelectWork *s) {
    char buf[0x40];
    UiElem *e;
    s16 *pos;
    s32 many;
    s32 n;
    s32 i;
    s32 j;
    s32 k;
    s32 x;
    s32 y;
    s32 size;

    if (s->sel == 6) {
        s->elems[189].visible = 1;
        s->elems[189].bh = s->unk831C[s->file[s->cur]];
        e = &s->elems[189];
        if (s->elems[189].bh != 0) {
            e->by = s->unk8362[s->file[s->cur]] * s->scroll + 0x9A;
            func_00548A00(s, e, 1);
        }
        s->elems[189].visible = 0;
        e = &s->elems[184];
        for (i = 0x38; i < 0x5D; i++, e++) {
            if (i < 0x54 || i >= 0x5C) {
                func_00548A00(s, e, 1);
            }
        }
        e = &s->elems[212];
        for (i = 0; i < 8; i++, e++) {
            e->color = s->unk8438[s->cur][i] != 0 ? -1 : 0xFF404040;
            func_00548A00(s, e, 1);
        }
        func_001B4340(-2);
        func_001B4370(0);
        func_001B4380(0x80808080);
        if (s->itemCount[s->cur] == 0) {
            func_001B4300(0x14);
            func_001B4320(0x90, 0xEA);
            func_001B43C0((u8 *)D_00560720);
        } else {
            func_001B4300(0x14);
            for (i = s->scroll, y = 0x90; i < s->itemCount[s->cur] && i - s->scroll < 12; i++, y += 0x14) {
                func_001B4320(0x90, y);
                func_001B43C0((u8 *)func_001C02A0(s->cur, s->items[s->cur][i]));
            }
            func_001B4320(0x1F4, 0x6E);
            sprintf(buf, D_00560740, s->unk83AC[s->cur]);
            func_001B43C0((u8 *)buf);
        }
        return;
    }
    many = 0;
    for (i = 0; i < 9; i++) {
        if (s->costumes[i] >= 2) {
            many = 1;
            break;
        }
    }
    pos = many == 0 ? D_00559520 : D_0055952A;
    e = &s->elems[153];
    for (i = 0; i < 5; i++, e++, pos++) {
        if (*pos == 0) {
            e->visible = 0;
        } else {
            e->bx = *pos;
        }
    }
    if (s->unk24 != 1 && s->sel >= 3) {
        e = &s->elems[146];
        if (w->unkC != 8) {
            e->page = D_00558B28;
            e->page += w->unkC;
            e->visible = 1;
        } else {
            e->visible = 0;
        }
        func_00548A00(s, e, 1);
        func_00548A00(s, &s->elems[147], 1);
        func_00548A00(s, &s->elems[148], 1);
        func_00548A00(s, &s->elems[149], 1);
        func_00548A00(s, &s->elems[150], 1);
        return;
    }
    n = 8;
    if (s->extraCount != 0) {
        n = 9;
    }
    e = &s->elems[146];
    if (w->unkC != 8) {
        e->page = D_00558B28;
        e->page += w->unkC;
        e->visible = 1;
    } else {
        e->visible = 0;
    }
    for (i = 0; i < n; i++) {
        s->elems[128 + i].visible = 0;
    }
    e = &s->elems[137];
    for (i = 0; i < n; i++, e++) {
        k = i - 2;
        if (s->sel != 2) {
            if (k < 0) {
                k += n;
            }
            if (k == w->unkC) {
                e->color = -1;
                s->elems[128 + w->unkC].visible = 1;
                continue;
            }
        }
        e->color = 0xFF808080;
    }
    func_00548A00(s, &s->elems[128], 0x36);
    func_001B4340(-2);
    func_001B4370(0);
    for (i = 0; i < 5; i++) {
        x = D_005583F0[i][0];
        y = D_005583F0[i][1];
        size = D_005583F0[i][2];
        func_001B4300(size);
        if (w->unkC == 8) {
            if (i != 0) {
                func_001B4380(0x80000000);
                func_001B4320(x + 2, y + 2);
                func_001B43C0((u8 *)D_00558330[i + w->unkC * 5]);
                func_001B4380(0x80808080);
                func_001B4320(x, y);
                func_001B43C0((u8 *)D_00558330[i + w->unkC * 5]);
            } else {
                func_001B4380(0x80000000);
                func_001B4320(x + 2, y + 2);
                func_001B43C0((u8 *)func_001C0180(s->extras[w->unkD[0]]));
                func_001B4380(0x80808080);
                func_001B4320(x, y);
                func_001B43C0((u8 *)func_001C0180(s->extras[w->unkD[0]]));
            }
        } else {
            func_001B4380(0x80000000);
            func_001B4320(x + 2, y + 2);
            func_001B43C0((u8 *)D_00558330[i + w->unkC * 5]);
            func_001B4380(0x80808080);
            func_001B4320(x, y);
            func_001B43C0((u8 *)D_00558330[i + w->unkC * 5]);
        }
    }
    s->elems[221].visible = 1;
    s->elems[224].visible = 1;
    func_00548A00(s, &s->elems[221], 1);
    s->elems[224].v0 = s->blink;
    func_00548A00(s, &s->elems[224], 1);
    s->elems[222].visible = 1;
    func_00548A00(s, &s->elems[222], 1);
    func_001AE9C0(0x6C, 0);
    if (s->sel != 2) {
        if (w->unkC == 8) {
            if (s->extraCount != 0) {
                if (s->extraCount >= 2) {
                    func_00548A00(s, &s->elems[182], 2);
                }
                y = D_0023A438[1];
                strcpy(buf, func_001C0180(s->extras[w->unkD[0]]));
                x = (0x48 - (s32)strlen(buf) * 9) / 2;
                func_001B4320(x + 0xB0, y);
                func_001B4300(0x12);
                func_001B43C0((u8 *)buf);
            }
        } else if (s->costumes[w->unkC] >= 2) {
            func_00548A00(s, &s->elems[182], 2);
            func_001B4320(D_0023A438[0], D_0023A438[1]);
            func_001B4300(0x12);
            k = 0;
            for (j = 0; j < 3; j++) {
                if (s->costume[w->unkC][j] != 0) {
                    if (k == w->unkD[0]) {
                        break;
                    }
                    k++;
                }
            }
            func_001B43C0((u8 *)D_00558318[j]);
        }
    }
}
#endif

/* --- select step 10: the difficulty --- */

extern char *D_005595F0[];      /* debug menu texts */
extern char D_00560810[], D_00560828[], D_00560830[], D_00560848[], D_00560850[], D_00560860[],
    D_00560868[], D_00560870[], D_00560880[], D_00560890[], D_00560898[], D_005608A0[], D_005608D0[],
    D_005608E0[];
void func_00544350(SelectWork *s);
void func_00546D40(TitleWork *w, SelectWork *s);
void func_00547290(TitleWork *w);

#ifdef NON_MATCHING   /* first draft from m2c */
void func_00547340(TitleWork *w, SelectWork *s) {
    char buf[0x40];
    s32 y;
    s32 x;
    s32 i;
    s32 n;
    s16 v;

    if (func_001B6980(0x52) != 0) {
        /* debug: set the players up by hand */
        y = 0x20;
        func_001B4340(-2);
        func_001B4370(0);
        func_001B4380(0x80808080);
        func_001B4300(0x14);
        func_001B4320(0x20, 0x20);
        func_001B43C0((u8 *)D_00560810);
        switch (s->sel) {
        case 0:
            D_002F88B0[5] = 0;
            w->unkC = 0;
            w->unkD[0] = 0;
            s->sel++;
            return;
        case 1:
            func_001B4320(0x40, 0x60);
            func_001B4380(0x80008000);
            func_001B43C0((u8 *)D_00560828);
            func_001B4380(0x80808080);
            func_001B43C0((u8 *)D_00560830);
            func_001B4320(0x40, 0x74);
            func_001B4380(0x80808000);
            func_001B43C0((u8 *)D_00560848);
            func_001B4380(0x80808080);
            func_001B43C0((u8 *)D_00560850);
            if (s->pressed & 0x200) {
                D_002F88B0[5] = 2;
                s->sel++;
                return;
            }
            if (s->pressed & 0x10) {
                s->sel = 3;
                return;
            }
            return;
        case 2:
            if (s->pressed & 0x10) {
                func_00544310(s);
                return;
            }
            if (w->pressed & 0xC) {
                w->unkD[0] ^= 1;
            }
            if (w->pressed & 0x400) {
                w->unkC = (w->unkC + 1) & 3;
            }
            if (w->pressed & 0x800) {
                w->unkC = (w->unkC - 1) & 3;
            }
            switch (w->unkC) {
            case 0:
                if (s->nav & 1) {
                    D_002F88B0[0x18 + w->unkD[0]] = (D_002F88B0[0x18 + w->unkD[0]] + 1) & 7;
                }
                if (s->nav & 2) {
                    D_002F88B0[0x18 + w->unkD[0]] = (D_002F88B0[0x18 + w->unkD[0]] - 1) & 7;
                }
                break;
            case 1:
                n = (D_002F88B0[0x18 + w->unkD[0]] >= 5) ? 3 : 2;
                if (s->nav & 2) {
                    if (D_002F88B0[0x1A + w->unkD[0]] != 0) {
                        D_002F88B0[0x1A + w->unkD[0]]--;
                    } else {
                        D_002F88B0[0x1A + w->unkD[0]] = n - 1;
                    }
                }
                if (s->nav & 1) {
                    D_002F88B0[0x1A + w->unkD[0]]++;
                    if (D_002F88B0[0x1A + w->unkD[0]] == n) {
                        D_002F88B0[0x1A + w->unkD[0]] = 0;
                    }
                }
                break;
            case 2:
                func_00547290(w);
                break;
            case 3:
                if (s->pressed & 0x20) {
                    s->sel++;
                    return;
                }
                break;
            }
            for (i = 0; i < 2; i++) {
                x = i * 0x78 + 0x40;
                func_001B4380(0x80008080);
                func_001B4320(x, 0x40);
                func_001B43C0((u8 *)D_005595F0[i]);
                func_001B4320(x, 0x54);
                func_001B4380((w->unkC == 0 && w->unkD[0] == i) ? 0x80000080 : 0x80808080);
                func_001B43C0((u8 *)D_005595F0[D_002F88B0[0x18 + i] + 2]);
                func_001B4320(x, 0x68);
                func_001B4380((w->unkC == 1 && w->unkD[0] == i) ? 0x80000080 : 0x80808080);
                sprintf(buf, D_00560860, D_002F88B0[0x1A + i]);
                func_001B43C0((u8 *)buf);
                y = 0x7C;
                func_001B4320(x, 0x7C);
                func_001B4380((w->unkC == 2 && w->unkD[0] == i) ? 0x80000080 : 0x80808080);
                v = ((s16 *)(D_002F88B0 + 0x1C))[i];
                if (v == 0) {
                    strcpy(buf, D_00560868);
                } else {
                    sprintf(buf, D_00560860, v - 1);
                }
                func_001B43C0((u8 *)buf);
                if (i == 0) {
                    y = 0x90;
                    func_001B4320(x, 0x90);
                    func_001B4380(w->unkC == 3 ? 0x80000080 : 0x80808080);
                    func_001B43C0((u8 *)D_00560870);
                    func_001B4380(0x80808080);
                    func_001B43C0((u8 *)D_00560880);
                    func_001B4380(0x80000080);
                    func_001B43C0((u8 *)D_00560890);
                    func_001B4380(0x80808080);
                    func_001B43C0((u8 *)D_00560898);
                }
            }
            func_001B4300(0x10);
            func_001B4320(0x40, y + 0x50);
            func_001B4380(0x80808000);
            func_001B43C0((u8 *)D_005608A0);
            func_001B43C0((u8 *)D_005608D0);
            func_001B4300(0x14);
            func_001B4320(0x28, w->unkC * 0x14 + 0x54);
            func_001B4380(0x80008000);
            func_001B43C0((u8 *)D_005608E0);
            return;
        case 3:
            func_00544350(s);
            return;
        }
        return;
    }
    switch (s->sel) {
    case 0:
        s->unk24 = 0;
        for (i = 0; i < 8; i++) {
            s->elems[256 + i].visible = 1;
        }
        for (i = 0; i < 5; i++) {
            s->anim[i] = 0;
            s->open[i] = 0;
        }
        if (s->back != 0) {
            if (!(D_00301D7F[0] & 4)) {
                s->back = 1;
                func_00544310(s);
                func_00546D40(w, s);
                return;
            }
            s->back = 0;
            if (D_002F88B0[5] != 0) {
                w->unkD[1] = 0;
            } else {
                w->unkD[1] = 1;
            }
        } else {
            w->unkD[1] = 0;
        }
        if (!(D_00301D7F[0] & 4)) {
            D_002F88B0[5] = 2;
            func_00544350(s);
        } else {
            s->anim[0] = 1;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 1:
        if (--s->timer2 == 0) {
            s->anim[3] = 1;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 2:
        if (--s->timer2 == 0) {
            s->anim[1] = 1;
            s->anim[2] = 1;
            s->anim[4] = 1;
            s->sel++;
        }
        break;
    case 3:
        if ((s->nav & 1) && w->unkD[1] != 0) {
            w->unkD[1]--;
            func_001B6EF0(2);
        } else if ((s->nav & 2) && w->unkD[1] == 0) {
            w->unkD[1]++;
            func_001B6EF0(2);
        } else if (s->pressed & 0x20) {
            func_001B6EF0(4);
            s->anim[4] = 2;
            s->anim[1] = 2;
            s->anim[2] = 2;
            s->timer2 = 3;
            s->sel++;
        } else if (s->pressed & 0x10) {
            func_001B6EF0(1);
            s->unk24 = 1;
            s->anim[4] = 2;
            s->anim[1] = 2;
            s->anim[2] = 2;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 4:
        if (--s->timer2 == 0) {
            s->anim[3] = 2;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 5:
        if (--s->timer2 == 0) {
            s->anim[0] = 2;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 6:
        if (--s->timer2 == 0) {
            if (s->unk24 != 0) {
                s->back = 1;
                func_00544310(s);
            } else {
                if (w->unkD[1] == 0) {
                    D_002F88B0[5] = 2;
                } else {
                    D_002F88B0[5] = 0;
                }
                func_00544350(s);
            }
        }
        break;
    }
    func_00546D40(w, s);
}
#endif


void func_00544350(SelectWork *s) {
    s->state = 11;
    s->sel = 0;
    s->sub = 0;
}

/* --- select step 11: the scenario --- */

extern char *D_00559630[];      /* debug: scenario names */
extern char D_00560930[], D_00560940[], D_00560950[], D_00560970[];
void func_00547CF0(TitleWork *w, SelectWork *s);

/* Leaves the select screen with the choices made. */
#define SELECT_ANIM_OUT(s, w, a, next)  \
    do {                                \
        (s)->anim[a] = 2;               \
        (s)->anim[2] = 2;               \
        (s)->anim[1] = 2;               \
        (s)->timer2 = 3;                \
        (s)->sel = (next);              \
    } while (0)

#ifdef NON_MATCHING   /* first draft from m2c */
void func_005482F0(TitleWork *w, SelectWork *s) {
    s32 i;

    if (func_001B6980(0x52) != 0) {
        func_001B4340(-2);
        func_001B4370(0);
        func_001B4380(0x80808080);
        func_001B4300(0x14);
        func_001B4320(0x20, 0x20);
        func_001B43C0((u8 *)D_00560930);
        switch (s->sel) {
        case 0:
            w->unkC = D_002F88B0[2];
            s->sel++;
            break;
        case 1:
            if (!(w->pressed & 2)) {
                if (w->pressed & 8) {
                    w->unkC = (w->unkC + 1) & 3;
                }
                if (w->pressed & 4) {
                    w->unkC = (w->unkC - 1) & 3;
                }
            }
            if (w->pressed & 0x10) {
                func_00544330(s);
                return;
            }
            if (w->pressed & 0x20) {
                s->sel = 3;
            }
            break;
        case 3:
            D_002F88B0[2] = w->unkC;
            func_001B9140(3);
            func_00542F70(w);
            w->unk70 = 1;
            return;
        }
        func_001B4320(0x48, 0x5C);
        func_001B4380(s->sel == 1 ? 0x80000080 : 0x80808080);
        func_001B43C0((u8 *)D_00560940);
        func_001B4380(0x80008080);
        func_001B43C0((u8 *)D_00559630[w->unkC]);
        func_001B4300(0x10);
        func_001B4320(0x40, 0xAC);
        func_001B4380(0x80808000);
        func_001B43C0((u8 *)D_00560950);
        func_001B43C0((u8 *)D_00560970);
        return;
    }
    switch (s->sel) {
    case 0:
        s->unk24 = 0;
        w->unkD[1] = 1;
        w->unkD[2] = 2;
        if (D_00301D7F[4] & 4) {
            w->unkD[2]++;
        }
        if (D_00301D7F[4] & 8) {
            w->unkD[2]++;
        }
        for (i = 0; i < 8; i++) {
            s->elems[256 + i].visible = 1;
        }
        for (i = 0; i < 8; i++) {
            s->anim[i] = 0;
            s->open[i] = 0;
        }
        s->anim[0] = 1;
        s->timer2 = 3;
        s->sel++;
        break;
    case 1:
        if (--s->timer2 == 0) {
            s->anim[3] = 1;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 2:
        if (--s->timer2 == 0) {
            s->anim[4] = 1;
            s->anim[1] = 1;
            s->anim[2] = 1;
            s->timer2 = 3;
            s->sel++;
            if (w->unkD[2] < 3) {
                s->sel = 5;
            }
        }
        break;
    case 3:
        if (--s->timer2 == 0) {
            s->anim[5] = 1;
            s->timer2 = 3;
            s->sel++;
            if (w->unkD[2] < 4) {
                s->sel = 5;
            }
        }
        break;
    case 4:
        if (--s->timer2 == 0) {
            s->anim[6] = 1;
            s->sel++;
        }
        break;
    case 5:
        if ((s->nav & 1) && w->unkD[1] != 0) {
            w->unkD[1]--;
            func_001B6EF0(2);
        } else if ((s->nav & 2) && w->unkD[1] != w->unkD[2] - 1) {
            w->unkD[1]++;
            func_001B6EF0(2);
        } else if (s->pressed & 0x20) {
            func_001B6EF0(4);
            func_001AD680(0, 0, 0x1C);
            if (w->unkD[2] == 3) {
                SELECT_ANIM_OUT(s, w, 5, 7);
            } else if (w->unkD[2] == 2) {
                SELECT_ANIM_OUT(s, w, 4, 8);
            } else {
                SELECT_ANIM_OUT(s, w, 6, s->sel + 1);
            }
        } else if (s->pressed & 0x10) {
            func_001B6EF0(1);
            s->unk24 = 1;
            if (w->unkD[2] == 3) {
                SELECT_ANIM_OUT(s, w, 5, 7);
            } else if (w->unkD[2] == 2) {
                SELECT_ANIM_OUT(s, w, 4, 8);
            } else {
                SELECT_ANIM_OUT(s, w, 6, s->sel + 1);
            }
        }
        break;
    case 6:
        if (--s->timer2 == 0) {
            s->anim[5] = 2;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 7:
        if (--s->timer2 == 0) {
            s->anim[4] = 2;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 8:
        if (--s->timer2 == 0) {
            s->anim[3] = 2;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 9:
        if (--s->timer2 == 0) {
            s->anim[0] = 2;
            s->timer2 = 3;
            s->sel++;
        }
        break;
    case 10:
        if (--s->timer2 == 0) {
            if (s->unk24 != 0) {
                s->back = 1;
                func_00544330(s);
            } else {
                D_002F88B0[2] = w->unkD[1];
                s->timer2 = 0x14;
                func_001BA7A0(7);
                s->sel++;
            }
        }
        break;
    case 11:
        if (--s->timer2 == 0) {
            func_00349E20();
            s->timer2 = 5;
            s->sel++;
        }
        break;
    case 12:
        if (--s->timer2 == 0) {
            func_001B9140(3);
            func_001BA870();
            func_00542F70(w);
            w->unk70 = 1;
            return;
        }
        break;
    }
    func_00547CF0(w, s);
}
#endif

extern f32 D_00239CBC;          /* 0.2: how fast the pictures come in */
extern char *D_0023A440[2];     /* the difficulty names */
extern char *D_00559620[];      /* the scenario names */

/* Moves n of the pictures in or out. */
#define SELECT_OPEN(s, n)                                   \
    do {                                                    \
        s32 i_;                                             \
        for (i_ = 0; i_ < (n); i_++) {                      \
            if ((s)->anim[i_] == 1 && (s)->open[i_] < 1.0f) {   \
                (s)->open[i_] += D_00239CBC;                \
                if (!((s)->open[i_] < 1.0f)) {              \
                    (s)->open[i_] = 1.0f;                   \
                }                                           \
            }                                               \
            if ((s)->anim[i_] == 2 && !((s)->open[i_] <= 0.0f)) {   \
                (s)->open[i_] -= D_00239CBC;                \
                if ((s)->open[i_] <= 0.0f) {                \
                    (s)->open[i_] = 0.0f;                   \
                }                                           \
            }                                               \
        }                                                   \
    } while (0)

#ifdef NON_MATCHING   /* first draft from m2c */
/* Draws the difficulty screen. */
void func_00546D40(TitleWork *w, SelectWork *s) {
    UiElem tmp;
    UiElem *e;
    s32 i;
    s32 x;

    SELECT_OPEN(s, 5);
    if (--s->blink2 < 0) {
        s->blink2 += 0x20;
    }
    e = &s->elems[256];
    for (i = 0; i < 5; i++, e++) {
        if ((*(s16 *)(D_002F88B0 + 0x10) == 0 || i != 0) && e->visible != 0) {
            if (i == 0) {
                tmp = *e;
                tmp.page += D_002F88B0[8];
                func_00548A00(s, &tmp, 1);
            } else {
                func_00548A00(s, e, 1);
            }
        }
    }
    for (; i < 8; i++, e++) {
        if (e->visible != 0) {
            tmp = *e;
            if (i - 5 == 1 || i - 5 == 2) {
                tmp.color = ((u32)(255.0f * s->open[i - 5]) << 24) | 0xFFFFFF;
            } else {
                tmp.bw = (f32)tmp.bw * s->open[i - 5];
            }
            func_00548A00(s, &tmp, 1);
        }
    }
    if (s->sel == 3) {
        tmp = s->elems[264];
        tmp.visible = 1;
        tmp.by = (w->unkD[1] == 0 ? s->elems[266].by : s->elems[268].by) + 4;
        tmp.color = 0xFF804000;
        tmp.b = 1;
        tmp.u0 = s->blink2;
        func_00548A00(s, &tmp, 1);
    }
    tmp = s->elems[266];
    tmp.visible = 1;
    tmp.bw = (f32)tmp.bw * s->open[3];
    func_00548A00(s, &tmp, 1);
    tmp = s->elems[268];
    tmp.visible = 1;
    tmp.bw = (f32)tmp.bw * s->open[4];
    func_00548A00(s, &tmp, 1);
    if (s->sel == 3) {
        tmp = s->elems[265];
        tmp.visible = 1;
        tmp.by = w->unkD[1] == 0 ? s->elems[266].by : s->elems[268].by;
        func_00548A00(s, &tmp, 1);
    }
    if (s->sel == 3) {
        func_001B4340(-2);
        func_001B4370(0);
        func_001B4300(0x18);
        func_001B4380(0x80808080);
        x = (0x100 - (s32)strlen(D_0023A440[0]) * 12) / 2;
        func_001B4320(x + 0x38, 0xB6);
        func_001B43C0((u8 *)D_0023A440[0]);
        x = (0x100 - (s32)strlen(D_0023A440[1]) * 12) / 2;
        func_001B4320(x + 0x38, 0xE6);
        func_001B43C0((u8 *)D_0023A440[1]);
    }
}
#endif

#ifdef NON_MATCHING   /* first draft from m2c */
/* Draws the scenario screen. */
void func_00547CF0(TitleWork *w, SelectWork *s) {
    UiElem tmp;
    UiElem *e;
    s32 i;
    s32 x;
    s32 y;

    SELECT_OPEN(s, 8);
    if (--s->blink2 < 0) {
        s->blink2 += 0x20;
    }
    e = &s->elems[256];
    for (i = 0; i < 5; i++, e++) {
        if ((*(s16 *)(D_002F88B0 + 0x10) == 0 || i != 0) && e->visible != 0) {
            if (i == 0) {
                tmp = *e;
                tmp.page += D_002F88B0[8];
                func_00548A00(s, &tmp, 1);
            } else {
                func_00548A00(s, e, 1);
            }
        }
    }
    e = &s->elems[270];
    for (i = 0; i < 3; i++, e++) {
        tmp = *e;
        tmp.visible = 1;
        if (i == 1 || i == 2) {
            tmp.color = ((u32)(255.0f * s->open[i]) << 24) | 0xFFFFFF;
        } else {
            tmp.bw = (f32)tmp.bw * s->open[i];
        }
        func_00548A00(s, &tmp, 1);
    }
    if (s->sel == 5) {
        tmp = s->elems[264];
        tmp.visible = 1;
        tmp.by = s->elems[275 + w->unkD[1] * 2].by + 4;
        tmp.color = 0xFF804000;
        tmp.b = 1;
        tmp.u0 = s->blink2;
        func_00548A00(s, &tmp, 1);
    }
    for (i = 0; i < 4; i++) {
        tmp = s->elems[275 + i * 2];
        tmp.visible = 1;
        tmp.bw = (f32)tmp.bw * s->open[3 + i];
        func_00548A00(s, &tmp, 1);
    }
    if (s->sel == 5) {
        tmp = s->elems[265];
        tmp.visible = 1;
        tmp.by = s->elems[275 + w->unkD[1] * 2].by;
        func_00548A00(s, &tmp, 1);
    }
    if (s->sel == 5) {
        func_001B4340(-2);
        func_001B4370(0);
        func_001B4300(0x18);
        func_001B4380(0x80808080);
        for (i = 0, y = 0xB0; i < w->unkD[2]; i++, y += 0x30) {
            x = (0x100 - (s32)strlen(D_00559620[i]) * 12) / 2;
            func_001B4320(x + 0x38, y + 6);
            func_001B43C0((u8 *)D_00559620[i]);
        }
    }
}
#endif


extern s16 D_0023BB98;
extern u8 D_003065C2[];         /* (Options) */
void func_001B6880(void);       /* start the game (single player) */
void func_001B68E0(void);       /* start the game (network) */

/* The title is done: free its textures and start what was chosen. */
void func_00540B10(TitleWork *w) {
    switch (w->unk64) {
    case 0:
        D_002F88D1[0] = w->unk68;
        func_001D54D0();
        func_001AEC00();
        D_0023BB98 = 0;
        D_003065C2[0] = 0;
        func_001B6880();
        break;
    case 1:
        func_001D54D0();
        func_001AEC00();
        D_003065C2[0] = 1;
        D_0023BB98 = 0;
        func_001B68E0();
        break;
    default:
        func_005403B0(w);
        break;
    }
}
