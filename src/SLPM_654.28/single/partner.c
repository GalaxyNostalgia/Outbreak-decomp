#include "common.h"
#include "object.h"
#include "options.h"
#include "thing.h"
#include "relation.h"
#include "zone.h"

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

/* The room's partner places (SINGLE.BIN's per-room data, 0xA4 bytes each). */
typedef struct {
    u8 used;            /* 0x00 */
    u8 unk1;
    u16 who;            /* 0x02 */
    f32 pos[3];         /* 0x04 */
    u8 unk10[0x40 - 0x10];
} SglPlace;             /* 0x40 */

typedef struct {
    u8 unk0[8];
    s32 nplaces;        /* 0x08 */
    SglPlace *places;   /* 0x0C */
    u8 unk10[0xA4 - 0x10];
} SglRoom;              /* 0xA4 */

extern SglRoom D_0084E2B0[];
f32 func_001916E0(const f32 *a, const f32 *b);
s32 func_0082A2F0(f32 *from, f32 *to, u16 *hit);

/* Place `place` of room id's: its point. */
typedef struct {
    f32 v[3];
} F3;

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
