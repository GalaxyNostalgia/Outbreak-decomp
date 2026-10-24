#include "common.h"
#include "object.h"
#include "options.h"
#include "thing.h"
#include "relation.h"
#include "zone.h"
#include "sgl.h"

typedef struct {
    f32 v[3];
} F3;

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

s32 func_00827100(s32 id);
s8 func_00835C50(DrawObjFull *obj, DrawObjFull *player, s32 *item, u16 id);
s32 func_00833130(DrawObjFull *obj);

/* The partner picks what to think about (func_00823340's step) when it may: the urgent cases
   first, then its strongest wish (unk10C8). */
void func_00823390(DrawObjFull *obj) {
    DrawObjFull *me = &D_00476DD0[0];
    DrawObjFull *other;
    s32 i;
    u8 best;
    u8 sel;

    if (obj->kind == 1) {
        other = &D_00476DD0[2];
    } else {
        other = &D_00476DD0[1];
    }
    if (obj->unk1074 == 0) {
        return;
    }
    switch (obj->unk54F) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 0x1A:
    case 0x27:
    case 0x31:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x3B:
        break;
    default:
        return;
    }
    if (obj->unkBA4 & 0x4010000) {
        return;
    }
    if (func_00827100(obj->unk1E2) == 0) {
        return;
    }
    obj->unk1083 = 0;
    if (obj->unkBA0 & 8) {
        func_00823340(obj, 13);
        return;
    }
    if (obj->unk54F == 0x3B) {
        func_00823340(obj, 14);
        return;
    }
    if (me->unk1E2 == obj->unk1E2 && obj->unk1081 == 0 && other->unkFF4 != 10) {
        if (me->unkBA0 & 8) {
            func_00823340(obj, 10);
            obj->unkFFC[3] = 0;
            return;
        }
        if (me->unk54F == 0x30 && (me->unkBA4 & 0x1000000)) {
            func_00823340(obj, 10);
            obj->unkFFC[3] = 1;
            return;
        }
        if (me->unk54F == 0x25) {
            func_00823340(obj, 10);
            obj->unkFFC[3] = 2;
            return;
        }
    }
    if (obj->unk1080 == 1) {
        obj->unk1090 = me->unkC7F;
        func_00823340(obj, 11);
        return;
    }
    if (obj->unk108A != 0 && obj->unk1E2 == me->unk1E2) {
        obj->unk108A = 0;
        func_00823340(obj, 12);
        return;
    }
    if (obj->unkB98 == 7 && me->unk1E2 == obj->unk1E2 &&
        func_00835C50(obj, me, &obj->unk1090, me->unk1E2) != 0) {
        func_00823340(obj, 15);
        return;
    }
    sel = 0xFF;
    best = 0;
    for (i = 0; i < 8; i++) {
        if (best < obj->unk10C8[i]) {
            best = obj->unk10C8[i];
            sel = i;
        }
    }
    switch (sel) {
    case 0:
        func_00823340(obj, 3);
        break;
    case 1:
        func_00823340(obj, 4);
        break;
    case 2:
        func_00823340(obj, 2);
        break;
    case 3:
        if (func_00833130(obj) == 0) {
            func_00823340(obj, 5);
        } else {
            func_00823340(obj, 2);
        }
        break;
    case 4:
        func_00823340(obj, 1);
        break;
    }
}

s8 func_008264F0(DrawObjFull *obj, s32 not, s32 room, s32 check);
void func_008294A0(s32 n);
s32 func_00826850(DrawObjFull *obj, f32 *pos, u16 id);
void func_00827130(f32 *out, s32 id, s32 place);
s8 func_00826BD0(void);
SglLink *func_00829350(s32 room, s32 variant, s32 place);
u16 func_00825CA0(s32 lo, s32 hi);
s32 func_00823780(DrawObjFull *obj);
s32 func_00823980(DrawObjFull *obj, s32 mask);
s32 func_008270C0(f32 r, f32 *pos, f32 *to);
extern u8 D_0048AEB0[];

/* Thinking step: go to a place (0: pick and set off, 1: on the way, 2: wait there). */
s32 func_00823FF0(DrawObjFull *obj) {
    s32 done = 0;
    s8 place;
    f32 pos[4];

    switch (obj->unkFFC[0]) {
    case 0:
        place = func_008264F0(obj, obj->unk100C, obj->unk1E2, 0);
        if (place == -1) {
            func_00823340(obj, 0);
            return 0;
        }
        func_00827130(pos, obj->unk1E2, place);
        if (func_00826850(obj, pos, obj->unk1E2) == -1) {
            func_00823340(obj, 0);
            return 0;
        }
        func_0082A690(obj, 2);
        obj->unk1070 = 60;
        obj->unkFFC[0]++;
        break;
    case 1:
        obj->unk105A = func_00826BD0();
        if (obj->unk105A < 0) {
            func_0082A690(obj, 0);
            func_00823340(obj, 0);
            return 0;
        }
        if (!(obj->unk1034 & 1) && func_008270C0(400.0f, obj->unk38, obj->unk1010) == 0) {
            obj->unkFF2[0] = 0;
        }
        if (func_00829350(obj->unk1058, D_0048AEB0[obj->unk1058], obj->unk100D) != 0) {
            if (obj->unk1E2 != obj->unk1058) {
                done = 1;
            }
        } else if (obj->unkFF8 == 0) {
            if (obj->unk105A == 1) {
                obj->unk1096 = 15;
                if (func_00823780(obj) != 0) {
                    return 1;
                }
                obj->unkFFC[0]++;
                obj->unk1000[0] = func_00825CA0(1, 4);
            } else {
                done = 1;
            }
        }
        break;
    case 2:
        if (obj->unk1000[0] == 0) {
            done = 1;
        } else {
            obj->unk1000[0]--;
        }
        break;
    }
    if (func_00823980(obj, 0xFFF) != 0) {
        done = 1;
    }
    if (done != 0) {
        func_0082A690(obj, 0);
        func_00823340(obj, 0);
    }
    return 0;
}

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

extern Thing D_003FFCA0[50];
extern s32 D_0023C264;
extern Zone *D_0084E0A0[];      /* the zones by the partner (func_00829B70) */
extern u16 D_00838CC0[];        /* per character: frames a reaction's unk10B8 holds */
extern s8 D_00838D60[][4];      /* danger by enemy class: three distances (in 100s), priority */
extern u8 D_00839AB0[];         /* each enemy kind's class */
extern u8 D_0023B900[4];        /* by unk106C */
s8 func_00825B80(DrawObjFull *obj, f32 *pos, s32 a);
s32 func_00823B90(DrawObjFull *obj);
s32 func_00829B70(DrawObjFull *obj, Zone **out);
f32 func_00191950(u32 a);
void func_00191590(f32 *v, f32 angle);
s32 func_00549860(const f32 *from, const f32 *to, s32 mask, s32 a, s32 *hit);
s32 func_00822EC0(DrawObjFull *obj, DrawObjFull *player, s32 a);
void func_00823090(DrawObjFull *obj, s32 what);
u8 func_005C14C0(DrawObjFull *obj, u16 a);

/* A partner's look at the scene each frame: where it waits, whether it is stuck, how it
   stands to us and the other partner, its wishes (unk10C8: mode's + reaction's + danger's),
   the nearest dangerous enemy, and what to say (func_00823090). */
void func_00822520(DrawObjFull *obj) {
    s32 say = -1;
    DrawObjFull *me = &D_00476DD0[0];
    f32 v[3];
    s32 prio;
    s32 hit;
    u8 w[2][8];
    s32 i;
    s32 k;
    s32 n;
    s32 best;
    s32 near;
    DrawObjFull *other;
    s32 dist;
    Thing *t;
    Relation *r;
    s8 *d;
    s32 id;
    s32 cls;
    s32 j;

    if (obj->kind == 1) {
        other = &D_00476DD0[2];
    } else {
        other = &D_00476DD0[1];
    }
    obj->unk100C = func_00825B80(obj, obj->unk38, 0);
    if (obj->unk1E2 == D_00306590.unk1C) {
        D_0023C264 = func_00829B70(obj, D_0084E0A0);
        if (func_00823B90(obj) == 0) {
            v[0] = 0.0f;
            v[1] = 0.0f;
            v[2] = 100.0f;
            func_00191590(v, func_00191950((u16)obj->unk92));
            v[0] += obj->unk38[0];
            v[1] += 10.0f + obj->unk38[1];
            v[2] += obj->unk38[2];
            if (func_00549860(obj->unk38, v, 0x1FFFF, 0, &hit) != 0) {
                obj->unk10A4++;
                if (obj->unk10A4 >= 150) {
                    obj->unk10A4 = 0;
                    func_0082A690(obj, 0);
                    func_00823340(obj, 0);
                }
            } else {
                obj->unk10A4 = 0;
            }
        } else {
            obj->unk10A4 = 0;
        }
        if (obj->unk54F == 8) {
            func_0082A690(obj, 0);
            func_00823340(obj, 0);
        }
    } else {
        D_0023C264 = 0;
        obj->unk10A4 = 0;
    }
    obj->unk1081 = 0;
    if (obj->unk1E2 == other->unk1E2 && obj->unk1088 == 0) {
        if (other->unk1088 != 0) {
            obj->unk1081 = 2;
        } else if (!(D_00386110[obj->kind][me->kind].dist <= D_00386110[other->kind][me->kind].dist)) {
            obj->unk1081 = 1;
        }
    }
    if (me->unk1077 == 0) {
        memset(w[0], 0, 8);
    } else {
        func_00107B50(w[0], (u8 *)obj->modeInfo + 0x28, 8);
    }
    if (obj->unk1077 == 0) {
        memset(w[1], 0, 8);
    } else {
        func_00107B50(w[1], (u8 *)obj->modeInfo + 0x30, 8);
    }
    for (i = 0; i < 8; i++) {
        if (w[0][i] >= w[1][i]) {
            obj->unk10C0[i] = w[0][i];
        } else {
            obj->unk10C0[i] = w[1][i];
        }
    }
    if (me->unkBA0 & 8) {
        obj->unk10C0[0] = 0x32;
    }
    if (D_00386110[obj->kind][me->kind].dist < 1000.0f && obj->unk1086 == 0 && me->unk1082 != 0) {
        switch (me->unk1075) {
        case 1:
            if (!(obj->unkBA0 & 8)) {
                say = 10;
            }
            break;
        case 9:
        case 10:
            if (obj->kind == me->unk1075 - 8) {
                obj->unk1088 = 300;
                other->unk1088 = 0;
            }
            break;
        case 2:
            if (obj->unk1081 == 2) {
                break;
            }
            if (obj->unk1081 == 0) {
                if (!(obj->unkBA0 & 8)) {
                    say = 5;
                }
                if (func_00822EC0(obj, me, 0) != 0) {
                    obj->unk1084 = D_00838CC0[obj->unkB98];
                    obj->unk1083 = 1;
                    func_00107B50(obj->unk10B8, (u8 *)obj->modeInfo + 0x8, 8);
                }
            }
            obj->unk1086 = 90;
            break;
        case 4:
            if (obj->unk1081 == 2) {
                break;
            }
            if (obj->unk1081 == 0) {
                say = 6;
                if (func_00822EC0(obj, me, 0) != 0) {
                    obj->unk1084 = D_00838CC0[obj->unkB98];
                    obj->unk1083 = 1;
                    func_00107B50(obj->unk10B8, (u8 *)obj->modeInfo + 0x18, 8);
                }
            }
            obj->unk1086 = 90;
            break;
        case 5:
            if (obj->unk1081 == 2) {
                break;
            }
            if (!(obj->unkBA0 & 8)) {
                say = 7;
            }
            obj->unk1084 = D_00838CC0[obj->unkB98];
            obj->unk1083 = 1;
            func_00107B50(obj->unk10B8, (u8 *)obj->modeInfo + 0x10, 8);
            obj->unk1086 = 90;
            break;
        case 8:
            if (obj->unk1081 == 2) {
                break;
            }
            if (obj->unk1081 == 0) {
                if (!(obj->unkBA0 & 8)) {
                    say = 8;
                }
                if (func_00822EC0(obj, me, 1) != 0) {
                    obj->unk1084 = D_00838CC0[obj->unkB98];
                    obj->unk1083 = 1;
                    func_00107B50(obj->unk10B8, (u8 *)obj->modeInfo + 0x20, 8);
                }
            }
            obj->unk1086 = 90;
            break;
        case 0:
            break;
        case 3:
            break;
        case 6:
            break;
        case 7:
            break;
        case 11:
            break;
        }
    }
    if (obj->unk1084 == 0) {
        memset(obj->unk10B8, 0, 8);
    }
    n = 0;
    i = 0;
    near = 0xFF;
    while (D_003840E0[obj->kind][i] != 0xFF) {
        n++;
        i++;
    }
    best = 3;
    obj->unk107F = obj->unk107E;
    obj->unk107E = n;
    for (j = 0; j < n; j++) {
        id = D_003840E0[obj->kind][j];
        t = &D_003FFCA0[id];
        r = &D_00385490[obj->kind][t->index];
        cls = D_00839AB0[t->kind];
        dist = r->dist;
        if (*(u8 *)&r->hidden == 0 && *(u8 *)&r->unk7 == 0 && !(t->unkC94 & 0x200)) {
            d = D_00838D60[cls];
            for (k = 0; k < 3; k++) {
                if (dist < d[k] * 100) {
                    if (k < best) {
                        prio = d[3];
                        best = k;
                        near = (u8)id;
                    } else if (best == k && prio < d[3]) {
                        best = k;
                        near = (u8)id;
                        prio = d[3];
                    }
                    break;
                }
            }
        }
    }
    obj->unk106D = obj->unk106C;
    obj->unk106E = near;
    obj->unk106C = best;
    memset(obj->unk10C8, 0, 8);
    for (i = 0; i < 8; i++) {
        obj->unk10C8[i] += obj->unk10B0[i];
        obj->unk10C8[i] += obj->unk10B8[i];
        obj->unk10C8[i] += obj->unk10C0[i];
    }
    obj->unk10C8[3] += D_0023B900[obj->unk106C];
    if (obj->unk109C != -1) {
        if (func_005C14C0(obj, *(u16 *)&obj->unk10A0) == 0) {
            obj->unk109C = -1;
        } else {
            obj->unk10C8[obj->unk109C] = 0xFF;
        }
    }
    if (me->unk54F == 0x15 && me->act[2] == 10 && me->unk1E2 == obj->unk1E2) {
        if (obj->unk1081 == 0 && other->unkFF4 != 11 && obj->unk1080 == 0) {
            obj->unk1080 = 1;
        }
    } else {
        obj->unk1080 = 0;
    }
    if (obj->unk1072 != 0) {
        obj->unk1072--;
    } else {
        obj->unk1072 = 60;
        say = 9;
    }
    if (obj->unkBA4 & 0x200000) {
        say = 0;
    }
    if (obj->unkBA4 & 0x400000) {
        say = 1;
    }
    if (say != -1) {
        func_00823090(obj, say);
    }
    if (obj->unk109A != 0) {
        obj->unk109A--;
        if (obj->unk109A == 0) {
            obj->unk1098 = obj->unk1099;
            obj->unk1099 = 0;
        }
    }
    if (obj->unk1084 != 0) {
        obj->unk1084--;
    } else {
        obj->unk1083 = 0;
    }
    if (obj->unk1086 != 0) {
        obj->unk1086--;
    }
    if (obj->unk1088 != 0) {
        obj->unk1088--;
    }
    if (obj->unk108A != 0) {
        obj->unk108A--;
    }
    if (obj->unk1070 != 0) {
        obj->unk1070--;
    }
    if (obj->unk107C != obj->unk1E2) {
        obj->unk107C = 0xFFFF;
    }
    if (obj->unk1060 != 0) {
        obj->unk1060--;
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

f32 func_001916E0(const f32 *a, const f32 *b);
s32 func_0082A2F0(f32 *from, f32 *to, u16 *hit);

/* 0 when pos is within r of to. */
s32 func_008270C0(f32 r, f32 *pos, f32 *to) {
    return !(func_001916E0(pos, to) < r);
}

s32 func_00828FC0(DrawObjFull *obj, s32 place, s32 a, s32 id);
s8 func_00827FC0(DrawObjFull *obj, s32 id, s32 from, s32 to, s8 *na, s8 *nb, f32 *pa, f32 *pb,
                 f32 *dist);
s8 func_00828570(DrawObjFull *obj, s32 id, s32 from, s32 to, s8 *na, s8 *nb, f32 *pa, f32 *pb,
                 f32 *dist);
s8 func_00828B20(DrawObjFull *obj, s32 id, s32 from, s32 to, s8 *na, s8 *nb, f32 *pa, f32 *pb,
                 f32 *dist);
s32 func_00829310(DrawObjFull *obj, SglLink *l);

/* The way from place `from` to place `to` of room id (mode 0, 1 or 2 picks the search): the
   next places and their points; *link gets the room link at `to` the partner may take (-1:
   none). -1 when there is none. */
s8 func_00827D60(DrawObjFull *obj, s32 id, s32 from, s32 to, s8 *na, s8 *nb, f32 *pa, f32 *pb,
                 f32 *e, s16 *link, s32 mode) {
    s32 r;
    s32 i;
    SglLink *l;
    u8 *rv;
    f32 v[4];

    if (from == to) {
        *na = from;
        *nb = from;
        func_00827130(v, id, from);
        *(F3 *)pa = *(F3 *)v;
        *(F3 *)pb = *(F3 *)v;
        r = from;
    } else {
        if (func_00829350(id, D_0048AEB0[id], to) != 0 && func_00828FC0(obj, to, -1, id) == 0) {
            return -1;
        }
        if (mode == 0) {
            r = func_00827FC0(obj, id, from, to, na, nb, pa, pb, e);
        } else if (mode == 1) {
            r = func_00828570(obj, id, from, to, na, nb, pa, pb, e);
        } else {
            r = func_00828B20(obj, id, from, to, na, nb, pa, pb, e);
        }
    }
    *link = -1;
    if (r != -1) {
        l = D_0084E2A0.links;
        rv = &D_0048AC10.roomVariant[id];
        for (i = 0; i < D_0084E2A0.size; i++, l++) {
            if (l->room == id && l->variant == *rv && l->place == to) {
                if (func_00829310(obj, l) == 1) {
                    *link = i;
                    break;
                }
            }
        }
    }
    return r;
}

/* The room link at place `place` of room/variant, or NULL. */
SglLink *func_00829350(s32 room, s32 variant, s32 place) {
    SglLink *l = D_0084E2A0.links;
    s32 i;

    for (i = 0; i < D_0084E2A0.size; i++, l++) {
        if (l->room == room && l->variant == variant && l->place == place) {
            return l;
        }
    }
    return NULL;
}


s32 func_008291D0(DrawObjFull *obj, s32 n, s32 id);

s32 func_00829310(DrawObjFull *obj, SglLink *l) {
    return func_008291D0(obj, l->unk3, l->room);
}

extern s8 D_008A34C0[0x80];     /* the search's places: 1 visited/closed, -1 open */
s32 func_008288F0(DrawObjFull *obj, s32 id, SglPlace *pl, s32 at, s32 to, s32 depth, s32 *next,
                  f32 *dist);

/* The depth-first part of search 1: from place `at` on to `to`; *next gets the second step
   (at depth 1), *dist adds up. 1 when found, -1 not. */
#ifdef NON_MATCHING   /* the order the loop invariants are kept on the stack */
s32 func_008288F0(DrawObjFull *obj, s32 id, SglPlace *pl, s32 at, s32 to, s32 depth, s32 *next,
                  f32 *dist) {
    s32 k = 0;
    s32 hop;
    s32 got = -1;
    s32 found = got;
    u8 c;
    u8 **routes;
    s32 *np;

    D_008A34C0[at] = 1;
    routes = &D_0084E2B0[id].routes;
    np = &D_0084E2B0[id].nplaces;
    for (; k < 1; k++) {
        c = (*routes)[at * 4 + k + to * *np * 4];
        if (c == 0xFF) {
            continue;
        }
        hop = pl[at].next[c];
        if (hop < 0) {
            continue;
        }
        if (D_008A34C0[hop] != -1) {
            continue;
        }
        if (func_00828FC0(obj, at, hop, id) == 0) {
            continue;
        }
        *dist += pl[at].cost[k];
        if (to == hop) {
            got = hop;
            found = 1;
            break;
        }
        if (func_008288F0(obj, id, pl, hop, to, depth + 1, next, dist) != -1) {
            got = hop;
            found = 1;
            break;
        }
    }
    D_008A34C0[at] = -1;
    if (depth == 1) {
        *next = got;
    }
    return found;
}
#endif

/* Search 1 for the way from `from` to `to`: the first route step, then func_008288F0 on. */
#ifdef NON_MATCHING   /* the original's frame is 0x20 bigger (more spill slots); registers */
s8 func_00828570(DrawObjFull *obj, s32 id, s32 from, s32 to, s8 *na, s8 *nb, f32 *pa, f32 *pb,
                 f32 *dist) {
    s32 nb2 = -1;
    s32 hop;
    s32 r;
    s32 k;
    u8 c;
    SglPlace *pl;
    s32 i;
    s32 next;

    r = nb2;
    pl = D_0084E2B0[id].places;
    if (from == to) {
        r = from;
    } else {
        for (k = 0; k < 1; k++) {
            c = D_0084E2B0[id].routes[from * 4 + k + to * D_0084E2B0[id].nplaces * 4];
            if (c == 0xFF) {
                continue;
            }
            hop = pl[from].next[c];
            if (hop < 0) {
                continue;
            }
            if ((1 << obj->kind) & D_0084E2B0[id].seen[hop]) {
                continue;
            }
            if (func_00828FC0(obj, from, hop, id) == 0) {
                continue;
            }
            *dist = pl[from].cost[k];
            if (hop == to) {
                r = hop;
                break;
            }
            for (i = 0; i < 0x80; i++) {
                if ((1 << obj->kind) & D_0084E2B0[id].seen[i]) {
                    D_008A34C0[i] = 1;
                } else {
                    D_008A34C0[i] = -1;
                }
            }
            D_008A34C0[from] = 1;
            if (func_008288F0(obj, id, pl, hop, to, 1, &next, dist) != -1) {
                nb2 = next;
                r = hop;
                break;
            }
        }
    }
    if (r != -1) {
        *na = r;
        if (nb2 != -1) {
            *nb = nb2;
        } else {
            *nb = to;
        }
        *(F3 *)pa = *(F3 *)pl[*na].pos;
        *(F3 *)pb = *(F3 *)pl[*nb].pos;
    } else {
        *na = from;
        *nb = from;
        *(F3 *)pa = *(F3 *)pl[*na].pos;
        *(F3 *)pb = *(F3 *)pl[*nb].pos;
    }
    return r;
}
#endif

extern u16 D_0048AFB0[][6];
s32 func_0082A090(DrawObjFull *obj, s32 n, s32 id);

/* Switch n of room id if it concerns the partner (kind 1: by who), else -1. */
s32 func_0082A090(DrawObjFull *obj, s32 n, s32 id) {
    SglSwitch *sw;
    SglRoom *rm;
    s32 r = -1;

    rm = &D_0084E2B0[id];
    sw = &rm->sw[n];

    if (sw->on != 0) {
        switch (sw->kind) {
        default:
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
            r = n;
            break;
        case 1:
            switch (sw->who) {
            default:
            case 0:
            case 1:
                r = n;
                break;
            case 2:
                if (sw->whoArg == obj->unkB98) {
                    r = n;
                }
                break;
            case 3:
                if (sw->whoArg == obj->kind) {
                    r = n;
                }
                break;
            case 4:
                if (obj->isNpc != 0) {
                    r = n;
                }
                break;
            case 5:
                if (obj->isNpc != 0 && sw->whoArg == obj->unkB98) {
                    r = n;
                }
                break;
            case 6:
                if (obj->isNpc != 0 && sw->whoArg == obj->kind) {
                    r = n;
                }
                break;
            case 7:
            case 8:
                break;
            }
            break;
        }
    }
    return r;
}

/* Whether switch n of room id lets the partner through. */
#ifdef NON_MATCHING   /* if (bit) return 0; return 1; folds into sltu/xori */
s32 func_008291D0(DrawObjFull *obj, s32 n, s32 id) {
    SglSwitch *sw = D_0084E2B0[id].sw + n;

    switch (sw->kind) {
    case 0:
    case 2:
        if (func_0082A090(obj, n, id) == -1) {
            return 0;
        }
        if (D_0048AFB0[id][0] & (1 << sw->id)) {
            return 0;
        }
        return 1;
    case 1:
    case 6:
    case 13:
        if (func_0082A090(obj, n, id) == -1) {
            return 0;
        }
        break;
    case 7:
        switch (sw->id2) {
        case 19:
        case 20:
            if (func_0082A090(obj, n, id) == -1) {
                return 0;
            }
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
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 21:
        case 22:
        case 23:
        case 24:
        case 25:
        case 26:
        case 27:
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
        case 40:
            break;
        }
        break;
    case 3:
    case 4:
    case 5:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        break;
    }
    return 1;
}
#endif

/* Whether the partner may go from place `from` to `to` (-1: just whether it may leave `from`)
   in room id: the way's kind, its conditions, and the place's switch. */
#ifdef NON_MATCHING   /* the default case's block sits after the jump table's cases in the original */
s32 func_00828FC0(DrawObjFull *obj, s32 from, s32 to, s32 id) {
    s32 slot = -1;
    s32 sw = 0;
    SglPlace *pl;
    s32 i;
    u32 w;
    u8 c;
    u8 **ids;

    if (from == to) {
        return 1;
    }
    pl = &D_0084E2B0[id].places[from];
    if (to == -1) {
        sw = 1;
    } else {
        for (i = 0; i < 4; i++) {
            if (pl->next[i] == to) {
                slot = i;
                break;
            }
        }
        if (slot == -1) {
            return 0;
        }
        switch (pl->kind[slot][0]) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 11:
        case 12:
        case 13:
            if (obj->unkBA0 & 8) {
                return 0;
            }
            sw = 1;
            if (obj->unkBA0 & 0x80) {
                return 0;
            }
            break;
        case 10:
            if (obj->unkBA0 & 8) {
                return 0;
            }
            sw = 1;
            if (obj->unkBA0 & 0x80) {
                return 0;
            }
            goto check;
        case 0:
        default:
            break;
        }
        w = pl->keys[slot];
        ids = &D_0084E2B0[id].ids;
        for (i = 0; i < 4; i++) {
            c = w >> (i * 8);
            switch (c) {
            case 0:
                break;
            case 1:
            case 3:
                if ((*ids)[i] == 0) {
                    return 0;
                }
                break;
            case 2:
                if ((*ids)[i] != 0) {
                    return 0;
                }
                break;
            }
        }
    }
check:
    if (sw != 0) {
        if (pl->sw != -1 && func_008291D0(obj, pl->sw, id) == 0) {
            return 0;
        }
    }
    return 1;
}
#endif

/* How many places room id has. */
s32 func_00827100(s32 id) {
    return D_0084E2B0[id].nplaces;
}

s32 func_008264C0(s32 room, s32 variant);
s8 func_00827D60(DrawObjFull *obj, s32 id, s32 from, s32 to, s8 *na, s8 *nb, f32 *pa, f32 *pb,
                 f32 *e, s16 *link, s32 mode);
u16 func_001ADEF0(s32 n);

/* A place of room `room` to go to (not `not`; with check 1 only ones func_00829350 allows) that
   the partner can reach, picked at random by the places' weights; -1 none. */
#ifdef NON_MATCHING   /* spill slots and the pick loop's slt at; registers in the sort */
s8 func_008264F0(DrawObjFull *obj, s32 not, s32 room, s32 check) {
    s32 i;
    s32 n;
    s32 total;
    SglPlace *p;
    s32 off;
    SglPlace *base;
    u8 *wt;
    u8 *rv;
    s32 *np;
    s32 j;
    s32 a;
    s32 b;
    s32 acc;
    s32 pick;
    s32 r;
    s8 h;
    s8 g;
    s16 f;
    f32 e;
    f32 c[4];
    f32 d[4];
    s32 list[0x80];

    func_008294A0(obj->kind);
    off = func_008264C0(obj->unk1E2, D_0048AEB0[obj->unk1E2]);
    n = 0;
    total = 0;
    i = 0;
    base = D_0084E2B0[room].places;
    rv = &D_0048AC10.roomVariant[room];
    np = &D_0084E2B0[room].nplaces;
    for (p = base; i < *np; i++, p++) {
        if (p->used != 0) {
            wt = (u8 *)p + off + 0x20;
            if (*wt != 0) {
                if (check == 1 && func_00829350(room, *rv, i) != 0) {
                    continue;
                }
                if (func_00827D60(obj, obj->unk1E2, obj->unk100C, i, &h, &g, c, d, &e, &f, 1) != -1 &&
                    not != i) {
                    list[n++] = i;
                    total += *wt;
                }
            }
        }
    }
    if (n == 0) {
        return -1;
    }
    wt = (u8 *)base + off;
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            a = list[i];
            b = list[j];
            if (wt[a * 0x40 + 0x20] < wt[b * 0x40 + 0x20]) {
                list[i] = b;
                list[j] = a;
            }
        }
    }
    r = func_001ADEF0(1) & 0xFFFF;
    if (total != 0 && r != 0) {
        total = r % total;
    }
    pick = 0;
    acc = 0;
    for (i = 0; i < n; i++) {
        a = list[i];
        acc += ((u8 *)&base[a] + off)[0x20];
        if (acc >= total) {
            pick = a;
            break;
        }
    }
    return pick;
}
#endif


/* Player n's buffer. */
u8 *func_00829480(s32 n) {
    return D_0084E2A0.buf + n * D_0084E2A0.size;
}

/* Clears player n's buffer and its bit in every room's places. */
#ifdef NON_MATCHING   /* room and its row pointer swap a2/a3 */
void func_008294A0(s32 n) {
    u32 i;
    u8 *p;
    s32 k;
    u8 mask;
    s32 room;

    p = func_00829480(n);
    for (i = 0; i < D_0084E2A0.size; i++) {
        *p++ = 0;
    }
    mask = ~(1 << n);
    for (room = 0; room < 0x80; room++) {
        for (k = 0x7F; k >= 0; k--) {
            D_0084E2B0[room].seen[k] &= mask;
        }
    }
}
#endif

/* Place `place` of room id's: its point. */
void func_00827130(f32 *out, s32 id, s32 place) {
    *(F3 *)out = *(F3 *)D_0084E2B0[id].places[place].pos;
}

/* The nearest place of the room (for who, any when < 0) to pos; with flags & 1 the nearest
   that func_0082A2F0 says is free to reach. */
s8 func_008271A0(f32 *pos, s8 who, s32 room, s32 flags);
#ifdef NON_MATCHING   /* regalloc in the sort; the original addresses d[j] off sp */
s8 func_008271A0(f32 *pos, s8 who, s32 room, s32 flags) {
    SglPlace *p;
    s32 n;
    s32 i;
    s32 cnt = 0;
    s32 best = -1;
    s32 j;
    s32 idx[0x80];
    f32 d[0x80];
    s32 out;
    f32 t;
    SglPlace **pl;

    p = D_0084E2B0[room].places;
    n = D_0084E2B0[room].nplaces;
    for (i = 0; i < n; i++, p++) {
        if (p->used != 0 && (who < 0 || p->who == who)) {
            t = func_001916E0(p->pos, pos);
            idx[cnt] = i;
            d[cnt] = t;
            cnt++;
        }
    }
    for (i = 0; i < cnt - 1; i++) {
        for (j = i + 1; j < cnt; j++) {
            if (!(d[i] <= d[j])) {
                t = d[i];
                best = idx[i];
                d[i] = d[j];
                idx[i] = idx[j];
                d[j] = t;
                idx[j] = best;
            }
        }
    }
    if (flags & 1) {
        pl = &D_0084E2B0[room].places;
        for (i = 0; i < cnt; i++) {
            if (func_0082A2F0(pos, (*pl)[idx[i]].pos, (u16 *)&out) == 0) {
                best = idx[i];
                break;
            }
        }
    } else {
        best = idx[0];
    }
    return best;
}
#endif

s8 func_00827180(f32 *pos, s32 id) {
    return func_008271A0(pos, 0, id, 1);
}

s8 func_00827190(f32 *pos, s32 id) {
    return func_008271A0(pos, 0, id, 0);
}

/* Whether the way from `from` to `to` (10 up, `to` 20 up) is blocked by something other than
   the kinds a partner can pass (hit gets what it hit). */
s32 func_0082A2F0(f32 *from, f32 *to, u16 *hit) {
    f32 a[4];
    f32 b[4];

    *(F3 *)a = *(F3 *)from;
    *(F3 *)b = *(F3 *)to;
    a[1] += 10.0f;
    b[1] = a[1];
    b[1] += 10.0f;
    if (func_00549860(a, b, 0x1FFFF, 0, (s32 *)hit) != 0) {
        switch (*hit) {
        case 0x4:
        case 0x10:
        case 0x8:
        case 0x20:
        case 0x40:
        case 0x100:
        case 0x80:
        case 0x800:
        case 0x200:
        case 0x1000:
            break;
        default:
            return 1;
        }
    }
    return 0;
}

f32 func_003400C0(s16 a);
f32 func_00340100(s16 a);
s32 func_00829D50(f32 r, DrawObjFull *obj, f32 *pos, Zone **out);

/* func_00829D50 at a point by the partner (by its action: where it stands, or ahead). */
#ifdef NON_MATCHING   /* the original branches for m (if/return 1 fold), the rest follows */
s32 func_00829B70(DrawObjFull *obj, Zone **a) {
    s32 r;
    f32 v[3];
    u8 m;

    m = obj->unk54F != 0x1A ? 1 : 0;
    switch (m) {
    case 0:
        v[0] = obj->unk38[0];
        v[1] = obj->unk38[1];
        v[2] = obj->unk38[2];
        r = func_00829D50(50.0f, obj, v, a);
        break;
    case 1:
        v[0] = obj->unk38[0] + (46.0f + obj->unk1B4[0]) * func_003400C0(obj->unk92);
        v[2] = obj->unk38[2] + (46.0f + obj->unk1B4[0]) * func_00340100(obj->unk92);
        v[1] = obj->unk38[1];
        r = func_00829D50(50.0f, obj, v, a);
        break;
    case 2:
        v[0] = obj->unk38[0] + 46.0f * func_003400C0(obj->unk92);
        v[2] = obj->unk38[2] + 46.0f * func_00340100(obj->unk92);
        v[1] = obj->unk38[1];
        break;
    case 3:
        v[0] = obj->unk38[0] + obj->unk1B4[0] * func_003400C0(obj->unk92);
        v[2] = obj->unk38[2] + obj->unk1B4[0] * func_00340100(obj->unk92);
        v[1] = obj->unk38[1];
        r = func_00829D50(60.0f, obj, v, a);
        break;
    }
    return r;
}
#endif

s32 func_00340DB0(Zone *z, s32 *p, s32 n, f32 r);
s32 func_00340940(Zone *z, s32 *p, s32 n);

/* The zones (on, and not unk3D) at pos (kind 0x85: at the partner; 0x80/0x81 within r) whose
   top is near pos's height, into out by priority (unk32); how many. */
#ifdef NON_MATCHING   /* the original keeps &q[1] in s3 across the loop; registers shift */
s32 func_00829D50(f32 r, DrawObjFull *obj, f32 *pos, Zone **out) {
    s32 ok;
    Zone *z;
    s32 n = 0;
    s32 p[2];
    s32 q[2];
    f32 y;
    f32 top;
    s32 i;
    s32 j;
    Zone *a;
    Zone *b;

    z = D_0023BD90;
    p[0] = pos[0];
    p[1] = pos[2];
    while (z != NULL) {
        if (z->on == 0) {
            z = z->next;
        } else if (z->unk3D != 0) {
            z = z->next;
        } else {
            switch (z->kind) {
            case 0x80:
            case 0x81:
                if (func_00340DB0(z, p, 4, r) == 0) {
                    z = z->next;
                    continue;
                }
                break;
            case 0x85:
                q[0] = obj->unk38[0];
                q[1] = obj->unk38[2];
                if (func_00340940(z, q, 4) == 0) {
                    z = z->next;
                    continue;
                }
                break;
            default:
                if (func_00340940(z, p, 4) == 0) {
                    z = z->next;
                    continue;
                }
                break;
            }
            top = z->top;
            y = pos[1];
            ok = 0;
            if (!(top <= y)) {
                if (top <= 200.0f + y) {
                    ok = 1;
                }
            } else if (top < y) {
                if (!((f32)z->bottom < y)) {
                    ok = 1;
                }
            } else {
                ok = 1;
            }
            if (ok != 0) {
                out[n++] = z;
            }
            z = z->next;
        }
    }
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n - 1; j++) {
            a = out[i];
            b = out[j];
            if (a->unk32 > b->unk32) {
                out[i] = b;
                out[j] = a;
            }
        }
    }
    return n;
}
#endif

/* Whether the partner is busy (in an action outside the free ones, held, or 0xBA0 & 0x200). */
s32 func_00823B90(DrawObjFull *obj) {
    switch (obj->unk54F) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 0x21:
    case 0x22:
    case 0x23:
    case 0x24:
    case 0x2D:
    case 0x2E:
    case 0x2F:
    case 0x3B:
        break;
    default:
        return 1;
    }
    if (obj->unkBA4 & 0x4030000) {
        return 1;
    }
    return (obj->unkBA0 & 0x200) != 0;
}


s8 func_00827180(f32 *pos, s32 id);
s8 func_00827190(f32 *pos, s32 id);

/* The place (of the room's, for id unk1E2) at pos: by mode 2, 1 or (0) as the partner's state
   says. */
#ifdef NON_MATCHING   /* mode 1's m = 1 reuses the compare's register in the original; one delay slot */
s8 func_00825B80(DrawObjFull *obj, f32 *pos, s32 mode) {
    s32 m = 0;
    s8 r;

    if (mode != 2) {
        if (mode != 1) {
            if (obj->unk105B == 0) {
                if (func_00823B90(obj) != 0) {
                    m = 1;
                }
            } else {
                m = 1;
            }
        } else {
            m = 1;
        }
    } else {
        m = 2;
    }
    switch (m) {
    default:
        r = func_00827180(pos, obj->unk1E2);
        if (r < 0) {
            r = func_00827190(pos, obj->unk1E2);
        }
        break;
    case 1:
        r = func_00827190(pos, obj->unk1E2);
        if (r < 0) {
            obj->unk100C = func_00827180(pos, obj->unk1E2);
        }
        break;
    case 2:
        r = func_00827180(pos, obj->unk1E2);
        break;
    }
    return r;
}
#endif

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
