#include "common.h"
#include "thing.h"
#include "options.h"
#include "game.h"

/* GAME.BIN: the per-frame update of one enemy kind: bookkeeping around its state handler
   (D_006E49E0, by action). */

typedef struct {
    f32 x, y, z;
} Vec3f;

typedef struct {
    s16 v[4];
} Rot4;

extern void (*D_006E49E0[])(Thing *t);
extern u8 D_006E4C78[];
extern EnemySet D_0071A030[];

void func_00579A40(Thing *t, void *target);
s32 func_00678750(Thing *t, s32 n);
void func_006782F0(Thing *t);
void func_00576690(Thing *t, s8 n);
void func_005E5B40(f32 *p, u8 *kind, s32 want);
void func_0067A960(Thing *t);
void func_005516D0(Thing *t);
void func_0062CA30(Thing *t);
void func_006031B0(Thing *t);
void func_00577870(Thing *t, u8 *tbl, u8 *buf);
void func_006798F0(Thing *t);
void func_00664570(Thing *t);

/* Counts unkC2E down on every other frame, to 0. */
void func_0060EB00(Thing *t) {
    if (t->unkC2E != 0 && (D_0048BF60.unk18 & 1)) {
        t->unkC2E--;
        if (t->unkC2E <= 0) {
            t->unkC2E = 0;
        }
    }
}

void func_00664290(Thing *t) {
    f32 p[3];
    u8 kind;

    *(Vec3f *)t->prevPos = *(Vec3f *)t->pos;
    *(Rot4 *)t->prevRot = *(Rot4 *)t->rot;
    t->unk54A = 0x10;
    t->unkBA5 = 0;
    t->unkC82 = 0;
    t->unk32 = 0;
    t->unkC36 = 0;
    func_00579A40(t, t->target);
    func_0060EB00(t);
    if (t->parts[0x13F - 0xF4] != 0 && func_00678750(t, 0) != 0) {
        func_006782F0(t);
        if (t->unkB9C == 0) {
            func_00576690(t, t->unk4 == 6 ? 0x2D : 0x2A);
        }
    }
    *(Vec3f *)p = *(Vec3f *)t->pos;
    p[1] += 200.0f;
    func_005E5B40(p, &kind, 0x81);
    if (kind == 0x81) {
        t->unkD7D = 1;
    } else {
        t->unkD7D = 0;
    }
    func_0067A960(t);
    t->unkC29 = t->unkBA3;
    if (t->hp <= 0 || ((t->unkB9B & 1) && t->unkBAC[2] != 0)) {
        t->unkC94 |= 1;
        if (t->hp > 0) {
            t->unkC94 &= ~0x200;
        } else {
            t->unkC94 |= 0x200;
        }
    } else {
        t->unkC94 &= ~1;
        t->unkC94 &= ~0x200;
    }
    t->unkC94 &= ~0x908;
    if (t->unk4 == 6) {
        t->unkC94 |= 0x10;
    }
    D_006E49E0[t->act[0]](t);
    if (t->visible == 1) {
        func_005516D0(t);
    }
    if (t->unkC82 == 0) {
        func_0062CA30(t);
    }
    func_006031B0(t);
    func_00577870(t, D_006E4C78, t->unkD1C);
    func_006798F0(t);
    func_00576690(t, t->unkC36);
    func_00664570(t);
    if (t->act[0] != 0 && (t->act[0] != 5 || t->act[1] != 0x11) &&
        D_00306590.unk1C != D_0071A030[t->set].room) {
        t->act[0] = 5;
        t->act[1] = 0x11;
        t->act[2] = 0;
        t->act[3] = 0;
    }
}

s32 func_0060EF30(Thing *t);
s32 func_0067A830(Thing *t);
void func_00577CA0(Thing *t, u8 motion);
void func_0067A370(Thing *t, s32 n);

/* While its enemy set is flagged (0x8) to wither: a thing standing (action 1, or 2 at step 7)
   loses 5 hp a frame and, when that would kill it (and func_0060EF30 allows), goes down. */
#ifdef NON_MATCHING   /* branch layout around the hp checks */
void func_0067A960(Thing *t) {
    s32 die = 0;
    u8 m;

    if (D_0071A030[t->set].unk45 & 8) {
        if (t->act[0] == 1 || (t->act[0] == 2 && t->act[1] == 7)) {
            if (t->hp != 0x7FFF) {
                if (t->hp > 0) {
                    t->hp -= 5;
                    if (t->hp <= 0) {
                        if (func_0060EF30(t)) {
                            die = 1;
                        } else {
                            die = 0;
                            t->hp = 1;
                        }
                    }
                    if (die) {
                        if (t->unk4 == 6) {
                            t->act[1] = 6;
                            m = 8;
                            t->unkBA3 = 2;
                        } else if (t->unk4 == 9) {
                            m = 9;
                            t->act[1] = 8;
                            t->unkBA3 = 1;
                        } else {
                            t->act[1] = 2;
                            t->unkBA3 = 1;
                            if (func_0067A830(t)) {
                                m = 0xB;
                            } else {
                                m = 2;
                            }
                        }
                        func_00577CA0(t, m);
                        D_0071A030[t->set].unk45 = 1;
                        t->act[0] = 3;
                        t->act[2] = 0;
                        t->act[3] = 0;
                        func_006782F0(t);
                        func_0067A370(t, 1);
                    }
                }
            }
        }
    }
}
#endif

extern f32 D_00239E7C;          /* -9.8 */
extern void *D_006E0450[6];
extern void *D_006DFD48[3];
extern Vec4f **D_006F4C84[];
extern u8 D_006E4CA0[];
extern u8 D_006E4CC0[];

void func_00340310(s32 rx, s32 ry, s32 rz, f32 *m);

/* Whether t's variant is flagged (unk5) as a second form. */
s32 func_006798B0(Thing *t) {
    return D_00357500[t->kind].defs[t->def].unk5 == 1;
}
s32 func_001AF4D0(void);
void func_005AF010(Thing *t, s32 n);
void func_00679370(Thing *t, s32 a, s32 b);
void func_00678E60(Thing *t, s32 motion, s8 mode);
void func_00554600(Thing *t, s32 n);
void func_00677CA0(Thing *t);
void *memset(void *p, s32 c, u32 n);

/* Action 0: sets the thing up (its gravity, motion set and first action) when it appears. */
void func_00664600(Thing *t);
#ifdef NON_MATCHING   /* the 0xF..0x11 test (a switch range check in the original, uses at) */
void func_00664600(Thing *t) {
    u8 k;

    func_00340310(t->rot[0], t->rot[1], t->rot[2], t->mtx);
    t->visible = 1;
    t->unkF0 = 1.0f;
    t->unkC80 = 0;
    t->unk520 = -1;
    t->unkC37 = 0;
    t->unkC2E = 0;
    t->unkBA5 = 1;
    switch (t->def) {
    case 0x50:
        t->unkAA8 = D_00239E7C;
        break;
    case 0x51:
        t->unkAA8 = -7.5f;
        break;
    case 0x55:
        t->unkAA8 = -2.25f;
        break;
    }
    t->unkC94 |= 0x201;
    t->unkD45 = 0;
    t->unkC9A = 0xFF;
    t->unkC9B = 0xFF;
    t->unk190 = 0x20FFFF;
    t->unk194 = 0;
    if (func_006798B0(t)) {
        if (t->unk4 == 9) {
            if (func_001AF4D0() == 1) {
                t->unk1A8 = D_006E0450[5];
            } else {
                t->unk1A8 = D_006E0450[3];
            }
            t->unk1D8 = D_006DFD48[1];
            t->unk1BC = t->unk1AC = t->unkC70 = *D_006F4C84[0][0];
        } else {
            t->unk1A8 = D_006E0450[1];
            t->unk1D8 = D_006DFD48[1];
            t->unk1BC = t->unk1AC = t->unkC70 = *D_006F4C84[0][0];
        }
        t->unkB9B = 1;
        t->unkD55 = 0xE;
        t->unk552 = 0xE;
        t->unk551 = 8;
        t->unkD09 = 3;
        t->unkD1B = 8;
    } else {
        if (t->unk4 == 6) {
            t->unk1A8 = D_006E0450[0];
            t->unk1D8 = D_006DFD48[2];
            t->unk1BC = t->unk1AC = t->unkC70 = *D_006F4C84[0][0];
        } else if (t->unk4 == 9) {
            if (func_001AF4D0() == 1) {
                t->unk1A8 = D_006E0450[4];
            } else {
                t->unk1A8 = D_006E0450[2];
            }
            t->unk1D8 = D_006DFD48[0];
            t->unk1BC = t->unk1AC = t->unkC70 = *D_006F4C84[0][1];
        } else {
            t->unk1A8 = D_006E0450[0];
            t->unk1D8 = D_006DFD48[0];
            t->unk1BC = t->unk1AC = t->unkC70 = *D_006F4C84[0][0];
        }
        t->unkB9B = 0;
        t->unkD55 = 0x15;
        t->unk551 = 0xA;
        t->unk552 = 0x15;
        t->unkBA8 = 0x18;
        t->unkD09 = 3;
        t->unkD1B = 0xA;
    }
    t->unkD2C = 0;
    t->unkD40 = 0;
    memset(t->unkBE8, 0, 4);
    memset(t->unkC, 0, 8);
    func_005AF010(t, 3);
    if (D_00306590.unk1C != D_0071A030[t->set].room) {
        t->act[0] = 5;
        t->act[1] = 0x11;
        t->act[2] = 0;
        t->act[3] = 0;
        t->visible = 0;
        func_00679370(t, 0, 0);
        return;
    }
    if (D_0071A030[t->set].unk45 & 3) {
        func_00678E60(t, D_006E4CA0[t->unkB9E], 0);
        if (D_0071A030[t->set].unk45 & 1) {
            t->act[2] = 5;
            t->hp = 0;
        } else {
            t->act[2] = 6;
        }
        t->act[0] = 4;
        t->act[1] = t->unkB9E;
        t->act[3] = 0;
        goto done;
    }
    if (t->unk4 == 0x63) {
        t->unk4 = 0;
        t->act[0] = 4;
        t->act[1] = 1;
        t->act[2] = 0;
        t->act[3] = 0;
    } else if ((k = t->unkC2B) != 0) {
        func_00678E60(t, D_006E4CC0[k], 0);
        t->act[0] = 5;
        t->act[1] = t->unkC2B;
        t->act[2] = 0;
        t->act[3] = 0;
        if ((u32)(t->unkC2B - 0xF) < 2 || t->unkC2B == 0x11) {
            t->visible = 0;
        }
        return;
    } else {
        func_00678E60(t, 0, 0);
        if (D_0071A030[t->set].unk45 & 8) {
            func_00554600(t, 3);
        }
        t->unkBA3 = 0;
        if (t->unk4 == 6) {
            func_00677CA0(t);
            return;
        }
        if (t->unk4 == 9) {
            func_00678E60(t, 0x33, 0);
            t->act[0] = 1;
            t->act[1] = 0x19;
            t->act[2] = 0;
            t->act[3] = 0;
            t->unkBA3 = 1;
        } else {
            t->act[0] = 1;
            t->act[1] = 0;
            t->act[2] = 0;
            t->act[3] = 0;
        }
    }
    func_006782F0(t);
done:
    func_0067A370(t, 0);
}
#endif
