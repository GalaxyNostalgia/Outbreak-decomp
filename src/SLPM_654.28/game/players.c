#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"
#include "model.h"
#include "thing.h"

/* The players when a room starts. */

void *memset(void *p, s32 c, u32 n);
extern DrawObjFull D_00476DD0[];
extern u32 D_0023BE14;              /* players */
extern Thing *D_0023BDF4;           /* the room's things */
s32 func_001AF4D0(void);
s32 func_001AF510(void);
void func_001AF1C0(u8 player);
void func_001924A0(void *bones);
void func_00825E30(s32 a);
void func_00822150(DrawObjFull *p, s32 a);
void func_005811A0(void);
void func_006B9BE0(u8 a);
void func_006B9CC0(u16 scenario);
void func_0061D7A0(DrawObjFull *p, s32 a);
void func_0064C620(DrawObjFull *p, s32 a);
void func_00635780(s32 a);
void func_005C5A90(DrawObjFull *p, s32 a);
void func_00635920(DrawObjFull *p);
void func_0062E1E0(DrawObjFull *p, s32 a);
void func_005A4490(DrawObjFull *p, s16 id, f32 a, f32 b);
void func_0055C4E0(DrawObjFull *p);
void func_00556BD0(DrawObjFull *p);
void func_005AEDB0(DrawObjFull *p);

/* Us (or, online, a player of this machine). */
void func_006391E0(DrawObjFull *p);
#ifdef NON_MATCHING   /* first draft */
void func_006391E0(DrawObjFull *p) {
    p->unk1E2 = D_00306590.unk1C;
    p->visible = 1;
    if (func_001AF4D0() == 1) {
        func_005811A0();
    }
    if (p->unkB98 == 2 && p->isNpc == 0) {
        func_006B9BE0(p->unkB98);
        func_006B9CC0(p->unk1E2);
    }
    if (p->unkAB8 != 0) {
        func_0061D7A0(p, 1);
        func_0064C620(p, 3);
        func_0064C620(p, 1);
        return;
    }
    switch (p->act[0]) {
    case 4:
        p->unkBA4 |= 4;
        return;
    default:
        if (p->unk54F == 0x28) {
            p->unkBA4 |= 4;
            return;
        }
        if (p->unkBA0 & 0x40000) {
            func_00635780(0);
            func_0064C620(p, 1);
            return;
        }
        if (p->unkBA0 & 0x4A0) {
            func_005C5A90(p, 0);
            p->unk54F = 0xA0;
            p->act[0] = 4;
            p->act[1] = 8;
            p->act[2] = 0;
            p->act[3] = 0;
            func_00635920(p);
            p->unkBA0 |= 0x1000;
            func_0064C620(p, 1);
            return;
        }
        if (p->unk544 == 0 && !(p->unkBA0 & 0x18)) {
            func_0062E1E0(p, 0);
            return;
        }
        if (p->unkBA4 & 1) {
            p->unkBA4 &= ~1;
            p->act[0] = 1;
            p->act[1] = 0;
            p->act[2] = 0;
            p->act[3] = 0;
            func_00635920(p);
            func_0064C620(p, 1);
            return;
        }
        p->unkBA4 |= 4;
        return;
    }
}
#endif

/* The others. */
void func_00639400(DrawObjFull *p);
#ifdef NON_MATCHING   /* first draft */
void func_00639400(DrawObjFull *p) {
    if (func_001AF510() == 0) {
        p->unk1E2 = D_00306590.unk1C;
        p->visible = 1;
        p->unk54F = 0;
        p->unk548 &= ~1;
        p->act[0] = 1;
        p->act[1] = 0;
        p->act[2] = 1;
        p->act[3] = 0;
        func_00635920(p);
        func_005A4490(p, 0x2050, 0.0f, 0.0f);
        func_0055C4E0(p);
        return;
    }
    if (func_001AF4D0() == 1 && p->isNpc != 0) {
        if (p->unkBA0 & 0x40) {
            p->unkBA4 |= 4;
            return;
        }
        if (!(p->unkBA4 & 0x80) || (p->unkBA4 & 1)) {
            p->unkBA4 &= ~0x81;
            p->act[0] = 1;
            p->act[1] = 0;
            p->act[2] = 0;
            p->act[3] = 0;
            func_00635920(p);
            return;
        }
    }
    if (p->unkBA0 & 0x120) {
        p->unkBA0 |= 0x40;
    }
    if (p->unk1E2 == D_00306590.unk1C) {
        if (p->act[0] == 1 && p->act[1] == 0xD7) {
            p->unkBA4 &= ~1;
        }
        if (!(p->unkBA4 & 1)) {
            p->unkBA4 |= 4;
            return;
        }
        p->unkBA4 &= ~1;
        switch (p->act[0]) {
        case 4:
            p->unkBA4 |= 4;
            return;
        case 1:
            if (p->act[1] == 0x60) {
                p->unkBA4 |= 4;
                return;
            }
        default:
            p->act[0] = 1;
            p->act[1] = 0;
            p->act[2] = 0;
            p->act[3] = 0;
            func_00635920(p);
            return;
        case 6:
        case 7:
            return;
        }
    }
    if (p->unkBA0 & 0x40) {
        p->unkBA4 &= ~1;
    } else {
        p->unkBA4 &= ~0x81;
    }
}
#endif

/* A player's state when a room starts. */
void func_00639650(DrawObjFull *p);
#ifdef NON_MATCHING   /* first draft */
void func_00639650(DrawObjFull *p) {
    u8 i;

    if (p->active == 0) {
        return;
    }
    p->unkBA0 &= ~0x10000;
    p->unkD32 = 0;
    func_001924A0(*(void **)((u8 *)p->model + 0x820));
    p->unk4E4[0] = 0;
    p->unk4E4[1] = 0;
    p->unk4E4[2] = 0;
    for (i = 0; i < 0xC; i++) {
        memset((u8 *)p + 0x1E4 + i * 0x20, 0, 0x20);
    }
    p->unkBA4 |= 0x08000000;
    p->unkBD0 = 0;
    p->unkCE8 = -1;
    p->unkCEC = 0;
    p->unkCF0 = 0;
    p->unkBA4 &= ~0xC000;
    p->unkBA0 &= 0xDBD7FFFF;
    p->unk548 &= ~1;
    p->unkB90 = 0;
    p->unk520 = 0;
    p->unkF4[0x13F - 0xF4] = 0;
    p->unkC64 = 0;
    if ((p->unkBA4 & 0x80) && p->unkAD4 == 2) {
        p->unkAD4 = 0;
    }
    if (func_001AF4D0() == 1) {
        if (p->kind == 0) {
            func_006391E0(p);
        } else {
            func_00639400(p);
        }
        if (p->isNpc != 0) {
            func_00822150(p, 0);
        }
    } else if (p->kind == D_0048BF60.unk1D) {
        func_006391E0(p);
    } else {
        func_00639400(p);
    }
    func_001AF1C0(p->kind);
}
#endif

/* The players when a room starts, then what goes with them. */
void func_00639800(void);
#ifdef NON_MATCHING   /* first draft */
void func_00639800(void) {
    DrawObjFull *p;
    Thing *t;
    u8 i;

    if (func_001AF4D0() == 1) {
        func_00825E30(2);
    }
    for (i = 0; i < D_0023BE14; i++) {
        func_00639650(&D_00476DD0[i]);
    }
    if (func_001AF4D0() == 1) {
        func_00825E30(0);
    }
    for (i = 0; i < D_0023BE14; i++) {
        p = &D_00476DD0[i];
        if (D_00306590.unk1C == p->unk1E2) {
            func_00556BD0(p);
        }
        if (p->unkBA0 & 2) {
            func_005AEDB0(p);
        }
    }
    for (t = D_0023BDF4; t != NULL; t = t->next) {
        if (t->active != 0 && t->visible != 0) {
            u8 *seen = (u8 *)&D_00476DD0[D_0048BF60.unk1D] + 0xFB0;

            if (seen[t->kind] != 0xFF) {
                seen[t->kind]++;
            }
        }
    }
}
#endif

extern s16 D_0023BF0C;
void func_00822290(void);
void func_00822440(DrawObjFull *p);
void func_00637B40(DrawObjFull *p);
void func_005E9220(void *o);
void func_001AFC60(DrawObjFull *p);
void func_00637C60(DrawObjFull *p);
void func_006399A0(void *o);
s32 func_00632DC0(DrawObjFull *p);
void func_0062CB30(void *o);
void func_0062CA30(void *o);
void func_0062CD00(void *o);
void func_0062CC50(void *o);
void func_00346B90(void *o);
s32 func_0067DA50(s8 *area, f32 *pos);
void func_0067D7C0(s8 area, s8 prev);
f32 func_0067DAF0(u8 area);
void func_00633BC0(DrawObjFull *p);

/* Once a frame: the players (offline, each one is "us" in turn while it runs). */
void func_006383E0(void);
#ifdef NON_MATCHING   /* first draft */
void func_006383E0(void) {
    DrawObjFull *p;
    s8 area;
    u8 i;

    if (func_001AF4D0() == 1) {
        func_00822290();
    }
    for (i = 0; i < 8; i++) {
        p = &D_00476DD0[i];
        if (p->active == 0) {
            continue;
        }
        D_0023BF0C = 0;
        FIELD(p, s32, 0x190) = 0x1FFFF;
        FIELD(p, s32, 0x194) = 0;
        FIELD(p, s8, 0xC82) = 0;
        FIELD(p, u16, 0xD92) = 0;
        FIELD(p, s8, 0xB9E) = 0;
        p->unk54A = 0;
        FIELD(p, s16, 0xBDC) = 0;
        FIELD(p, s16, 0xBDE) = 0;
        p->unkBA0 &= 0xDDFDFFFF;
        p->unkBA4 &= ~0x40;
        p->unk553 = 1;
        if (p->unk31[1] == 0xFF) {
            p->unk31[1] = 0;
        }
        if (func_001AF4D0() == 1) {
            D_0048BF60.unk1D = p->kind;
            if (p->isNpc != 0) {
                func_00822440(p);
            }
        }
        if (D_0048BF60.unk28 == 0) {
            if (p->unk1E2 != D_00306590.unk1C) {
                p->visible = 0;
                if (func_001AF4D0() == 1) {
                    func_00637B40(p);
                }
                FIELD(p, s8, 0xC53) = 0;
                p->unkBA0 &= 0xFF7FFFFF;
                p->unkBA0 &= ~0x4000;
                p->unkBA4 &= 0xFA9CFEFF;
                continue;
            }
            if (!(p->unkBA0 & 0x100)) {
                p->visible = 1;
            }
        }
        func_005E9220(p);
        func_001AFC60(p);
        p->unkBA4 &= 0xFA9DFFFF;
        if (func_001AF510() == 1 && p->kind != D_0048BF60.unk1D) {
            memset((u8 *)p + 0xF78, 0, 0x30);
        }
        func_00637C60(p);
        func_006399A0(p);
        if (func_00632DC0(p) != 0) {
            func_0062CB30(p);
            p->unk54A |= 4;
        } else {
            func_0062CA30(p);
            func_0062CB30(p);
        }
        func_0062CD00(p);
        func_0062CC50(p);
        if (p->unkBA4 & 0x08000000) {
            p->unkBA4 &= ~0x08000000;
            func_00346B90(p);
        }
        if (func_0067DA50(&area, p->unk38) != 0) {
            p->unkBA0 |= 0x4000;
            if (FIELD(p, u8, 0xCB0) != area) {
                func_0067D7C0(area, FIELD(p, s8, 0xCB0));
                FIELD(p, u8, 0xCB0) = area;
            }
            FIELD(p, f32, 0x170) += func_0067DAF0(FIELD(p, u8, 0xCB0));
            p->unk54A |= 1;
        } else {
            if ((p->unkBA0 & 0x4000) && !(p->unk54A & 2)) {
                func_0067D7C0(-1, FIELD(p, s8, 0xCB0));
                FIELD(p, u8, 0xCB0) = 0;
            }
            p->unkBA0 &= ~0x4000;
        }
        func_00633BC0(p);
        if (FIELD(p, u16, 0xD92) != 0) {
            func_0064C620(p, 7);
            FIELD(p, u16, 0xD92) = 0;
        }
        FIELD(p, s8, 0xC53) = 0;
        FIELD(p, s8, 0x10A9) = 0;
        p->unkBA0 &= 0xFFBFFFFF;
        if (p->kind == D_0048BF60.unk1D) {
            FIELD(&D_0048BF60, s8, 0xB9) = 0;
        }
    }
    if (func_001AF4D0() == 1) {
        D_0048BF60.unk1D = 0;
    }
}
#endif

/* The play record: distance moved and frames spent in each condition. */
typedef struct {
    f32 dist;           /* 0x00 */
    u8 unk4[0x18 - 4];
    s32 fine;           /* 0x18 */
    s32 caution;        /* 0x1C: (and 0.25-0.5 of health) */
    s32 danger;         /* 0x20 */
    s32 status1;        /* 0x24 */
    s32 status2;        /* 0x28 */
} PlayRecord;

extern PlayRecord D_00389650;
void func_005516D0(void *o);
void func_00634670(void *o);
void func_0062D2C0(void *o);
void func_0062F930(DrawObjFull *p, s32 a);
void func_0062F9B0(DrawObjFull *p, s32 a);
void func_005E9390(void *o);
s32 func_001AF540(DrawObjFull *p);
f32 func_00191A70(f32 x);

/* Once a frame, after the players' update: their motions, and the play record. */
void func_00638850(void);
#ifdef NON_MATCHING   /* first draft */
void func_00638850(void) {
    DrawObjFull *p;
    f32 h;
    u32 i;

    for (i = 0; i < 8; i++) {
        p = &D_00476DD0[i];
        if (p->active == 0) {
            continue;
        }
        if (func_001AF4D0() == 1) {
            D_0048BF60.unk1D = p->kind;
        }
        p->unkBA4 &= ~0x80;
        if (p->unkBA0 & 0x80000000) {
            func_0062D2C0(p);
        }
        if (p->unk1E2 != D_00306590.unk1C) {
            continue;
        }
        func_005516D0(p);
        func_00634670(p);
        if (p->type == 0) {
            if (p->act[0] == 1 && !(p->unkBA0 & 0x80)) {
                switch (p->unk54F) {
                case 0:
                    if (p->act[1] != 0) {
                        func_0062F930(p, 1);
                    } else {
                        func_0062F9B0(p, 1);
                    }
                    break;
                case 1:
                case 3:
                case 25:
                    func_0062F9B0(p, 1);
                    break;
                case 2:
                    switch (p->unk520) {
                    case 0x2011:
                    case 0x2014:
                        func_0062F930(p, 1);
                        break;
                    default:
                        func_0062F9B0(p, 1);
                        break;
                    }
                    break;
                default:
                    func_0062F930(p, 1);
                    break;
                }
            } else {
                func_0062F930(p, 1);
            }
        }
        func_005E9390(p);
        if (func_001AF540(p) == 1 && !(p->unkBA0 & 4)) {
            D_00389650.dist += func_00191A70(*(f32 *)&p->unk4E4[2]);
            if (p->unkBA0 & 1) {
                D_00389650.status1++;
            } else if (p->unkBA0 & 2) {
                D_00389650.status2++;
            } else {
                h = (f32)p->unk544 / (f32)(p->unk546 < p->unk544 ? p->unk544 : p->unk546);
                if (h >= 0.75f) {
                    D_00389650.fine++;
                } else if (h >= 0.5f) {
                    D_00389650.caution++;
                } else if (h >= 0.25f) {
                    D_00389650.caution++;
                } else {
                    D_00389650.danger++;
                }
            }
        }
    }
    if (func_001AF4D0() == 1) {
        D_0048BF60.unk1D = 0;
    }
}
#endif

/* A start point for the players (room data). */
typedef struct {
    s32 x;              /* 0x00 */
    s32 z;              /* 0x04 */
    s32 angle;          /* 0x08 */
    s32 unkC[2];
    s32 y;              /* 0x14 */
    s32 unk18[2];
} StartPoint;           /* 0x20 */

/* (D_0048AC10 + 0x280: per player, whether it was placed and on which start point) */
typedef struct {
    u8 placed;
    u8 unk1;
    u8 point;
} PlayerPlace;

typedef struct {
    u8 unk0[0x280];
    PlayerPlace place[8];
} ScenePlaces;

extern s32 D_0023BE10;          /* start points */
extern StartPoint D_00489C10[];
void func_0018D630(void *dst, void *src, s32 size);

/* Takes the room's start points from f and puts each player not yet placed (and not in one of
   the 0x5E0 states) on its own. */
void func_00631290(s32 *f);
#ifdef NON_MATCHING   /* two base registers swapped */
void func_00631290(s32 *f) {
    DrawObjFull *p;
    StartPoint *s;
    u8 i;
    u32 n;

    D_0023BE10 = f[0];
    func_0018D630(D_00489C10, f + 1, D_0023BE10 * sizeof(StartPoint));
    n = D_0023BE14;
    for (i = 0; i < n; i++) {
        p = &D_00476DD0[i];
        if (p->active != 0 && ((ScenePlaces *)&D_0048AC10)->place[i].placed == 0 &&
            !(p->unkBA0 & 0x5E0)) {
            s = &D_00489C10[((ScenePlaces *)&D_0048AC10)->place[i].point];
            p->unk38[0] = s->x;
            p->unk38[1] = s->y;
            p->unk38[2] = s->z;
            p->unk90 = 0;
            p->unk92 = s->angle;
            p->unk94 = 0;
            ((ScenePlaces *)&D_0048AC10)->place[i].placed = 1;
            p->unkBA4 |= 1;
        }
    }
}
#endif
