#include "common.h"
#include "object.h"
#include "options.h"

/* SINGLE.BIN: the computer-controlled partners' AI state. */

typedef struct {
    u8 unk0[8];         /* 0x00: copied to the partner's unk10B0 */
    u8 unk8[0x38 - 8];
} PartnerMode;          /* 0x38 */

extern PartnerMode D_00838860[0x14];
extern u8 D_008534C0[];

void func_0064F690(u8 type, s16 id);
void func_0064FB70(s32 type, s32 id, u8 *b, s32 player);
void func_00107B50(void *dst, const void *src, s32 n);
void *memset(void *p, s32 c, u32 n);
void func_00825DB0(DrawObjFull *obj, s8 v);
void func_00823340(DrawObjFull *obj, u16 v);
void func_0082A690(DrawObjFull *obj, u16 v);
void func_008223C0(DrawObjFull *obj, s32 mode);

extern DrawObjFull D_00476DD0[8];   /* the players */
extern u8 D_00838CD0[][2][9];       /* per character, partner or not: by unk1078 (0-3), unk107A
                                       (4-7), and 8 for 0xBA0 & 3 */
s32 func_005C6170(s32 player);
s32 func_005C6240(s32 player);

/* Every player's per-frame AI inputs: what changed since last frame and whether it is in
   danger (unk1077). */
void func_00822290(void) {
    s32 i;
    DrawObjFull *p;
    u8 *t;

    for (i = 0; i < 8; i++) {
        p = &D_00476DD0[i];
        p->unk1082 = 0;
        p->unk1077 = 0;
        if (p->unk1076 != p->unk1075) {
            p->unk1082 = 1;
        }
        p->unk1076 = p->unk1075;
        p->unk106F = p->unk54F;
        p->unk1079 = p->unk1078;
        p->unk107B = p->unk107A;
        p->unk1078 = func_005C6170(i);
        p->unk107A = func_005C6240(i);
        t = &D_00838CD0[0][0][p->isNpc * 9 + p->unkB98 * 18];
        if (t[p->unk1078]) {
            p->unk1077 = 1;
        }
        if (t[4 + p->unk107A]) {
            p->unk1077 = 1;
        }
        if (((p->unkBA0 & 1) || (p->unkBA0 & 2)) && t[8]) {
            p->unk1077 = 1;
        }
    }
}

/* A stick as Ctrl keeps it. */
typedef struct {
    s16 mag;
    s16 angle;
    s16 x;
    s16 y;
} CtrlStick;

void func_00822520(DrawObjFull *obj);
void func_00823390(DrawObjFull *obj);
void func_0082C150(DrawObjFull *obj);
extern s32 (*D_00839240[])(DrawObjFull *obj);  /* the thinking steps (by unkFF4): 1 to go on */
extern void (*D_008394D0[])(DrawObjFull *obj); /* the moving steps while unk105B (by unkFF8) */
extern void (*D_00839460[])(DrawObjFull *obj); /* the moving steps (by unkFF8) */
extern u8 D_0048C01D[];

/* A partner's thinking: its steps until one says stop. */
void func_008232F0(DrawObjFull *obj) {
    do {
        func_00823390(obj);
    } while (D_00839240[obj->unkFF4](obj));
}

/* The partner's moves for this frame (unk105B set). */
void func_0082BE60(DrawObjFull *obj) {
    func_0082C150(obj);
    D_008394D0[obj->unkFF8](obj);
}

/* The partner's moves for this frame. */
void func_0082A6D0(DrawObjFull *obj) {
    if (D_00306590.unk10[2] == 0) {
        D_00839460[obj->unkFF8](obj);
    }
}

/* A partner's frame: its controls cleared, then (unless paused or held) its AI drives them. */
void func_00822440(DrawObjFull *obj) {
    s32 i;

    obj->ctrl.cur = 0;
    obj->ctrl.pressed = 0;
    obj->ctrl.prev[0] = 0;
    for (i = 0; i < 2; i++) {
        ((CtrlStick *)((u8 *)obj + i * 8 + 0xF98))->mag = 0;
        ((CtrlStick *)((u8 *)obj + i * 8 + 0xF98))->angle = 0;
        ((CtrlStick *)((u8 *)obj + i * 8 + 0xF98))->x = 0;
        ((CtrlStick *)((u8 *)obj + i * 8 + 0xF98))->y = 0;
    }
    obj->unkFF2[0] = 1;
    obj->unkFF2[1] = 0;
    obj->unk1098 = 0;
    if (D_00306590.unk10[2] == 0 && !obj->unkFF1 && !obj->unkBD0 && !D_0048C01D[0] &&
        !(obj->unkBA0 & 0x20)) {
        func_00822520(obj);
        func_008232F0(obj);
        if (obj->unk105B) {
            func_0082BE60(obj);
        } else {
            func_0082A6D0(obj);
        }
    }
}

/* A partner's AI from the start (players that are partners only). */
void func_008220A0(DrawObjFull *obj) {
    D_008534C0[0] = 0;
    if (obj->isNpc != 0) {
        func_0064F690(0, obj->kind);
        func_00825DB0(obj, 0);
        obj->unk105B = 0xFF;
        obj->unk1064 = 0;
        obj->unkFF1 = 0;
        obj->unk1098 = 0;
        obj->unk1099 = 0;
        obj->unk109A = 0;
        obj->unk107C = 0xFFFF;
        obj->unk1083 = 0;
        obj->unk1072 = 0x3C;
        obj->unk1034 = 0;
        obj->unk109C = -1;
        func_00823340(obj, 0);
        func_0082A690(obj, 0);
        func_008223C0(obj, 0);
    }
}

/* Its mode (one of 0x14). */
void func_008223C0(DrawObjFull *obj, s32 mode) {
    PartnerMode *m;

    if (mode < 0x14) {
        obj->mode = mode;
        obj->modeInfo = m = &D_00838860[mode];
        func_00107B50(obj->unk10B0, m, 8);
        memset(obj->unk10B8, 0, 8);
        memset(obj->unk10C0, 0, 8);
    }
}

void func_00823340(DrawObjFull *obj, u16 v) {
    s32 i;

    for (i = 0; i < 4; i++) {
        obj->unkFFC[i] = 0;
        obj->unk1000[i] = 0;
    }
    if (v != 0) {
        obj->unk1074 = 0;
    } else {
        obj->unk1074 = 1;
        obj->unkFF6 = obj->unkFF4;
    }
    obj->unkFF4 = v;
}

void func_0082A6A0(DrawObjFull *obj, u16 v);

void func_0082A690(DrawObjFull *obj, u16 v) {
    obj->unkFFA = obj->unkFF8;
    obj->unkFF8 = v;
    func_0082A6A0(obj, v);
}

void func_0082A6A0(DrawObjFull *obj, u16 v) {
    s32 i;

    for (i = 0; i < 4; i++) {
        obj->unk1008[i] = 0;
    }
    obj->unk103A = 0;
}

#ifdef NON_MATCHING   /* argument scheduling */
void func_00825DB0(DrawObjFull *obj, s8 v) {
    u8 b[4];

    b[0] = v;
    b[1] = 0;
    b[2] = 0;
    b[3] = 0;
    func_0064FB70(0, obj->kind, b, -1);
}
#endif

void func_0082C640(DrawObjFull *obj);
void func_0082BE00(DrawObjFull *obj);
void func_00637240(DrawObjFull *obj, u8 a);
void func_006CCDA0(DrawObjFull *obj);

/* A partner entering (in our scenario's room) or leaving the room; mode 1 also resets its AI
   (func_006CCDA0). */
void func_00822150(DrawObjFull *obj, s32 mode) {
    u8 old = obj->unk105B;

    if (obj->unk1E2 != D_00306590.unk1C) {
        obj->unk105B = 1;
        if (old == 0) {
            FIELD(obj, s32, 0x1060) = 0x1C2;
            if (!(obj->unkBA0 & 0x20)) {
                func_0082C640(obj);
            }
            func_0082BE00(obj);
        }
    } else {
        obj->unk105B = 0;
        if (old == 1) {
            if (mode == 0) {
                if (!(obj->unkBA0 & 0x20)) {
                    func_0082C640(obj);
                }
                func_00637240(obj, obj->unk54F);
            } else {
                func_0082A6A0(obj, old);
            }
        }
    }
    if (mode == 1) {
        func_006CCDA0(obj);
        if (FIELD(obj, s8, 0x105A) == 1) {
            func_0082A690(obj, 0);
        }
    }
    obj->unk1064 = 1;
    obj->unk103A = 0;
    obj->unkBD0 = 0;
    obj->unkBA0 &= 0xDBD7FFFF;
    obj->unk548 &= ~1;
    obj->unkB90 = 0.0f;
    obj->unk520 = 0;
}
