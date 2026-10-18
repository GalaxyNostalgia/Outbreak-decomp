#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* GAME.BIN: starting characters' motions by id (0x2000s: the common ones), with the variants
   for their condition and for carrying a heavy weapon. */

/* A motion reference: which set and entry, and two more numbers. */
typedef struct {
    s16 v[4];
} MotRef;

/* (0x44 bytes each, by DrawObjFull.unkCA8) */
typedef struct {
    u8 heavy;           /* 0x00: (1) a weapon with its own walk */
    u8 unk1[0x43];
} WeaponMot;

/* The motion state that a temporary start must not change (0x204-0x223). */
typedef struct {
    u32 w[8];
} MotState;

extern WeaponMot D_006E5944[];
extern MotRef D_006F2780[];     /* the motions */
extern u16 D_006F26D0[];        /* ids 0x1nnn: one motion each */
extern u16 *D_006F1980[];       /* ids 0x2nnn: per weapon kind, four (by condition) each */
extern u16 D_006F2250[][4];     /* ids 0x3nnn: four (by condition) each */

/* The motion for id (0x1000s: plain, 0x2000s: by weapon and condition, 0x3000s: by
   condition; the condition is 3 when poisoned or bleeding, 0 with status 0x20). */
MotRef *func_005A3E60(DrawObjFull *o, s32 id) {
    u16 n;
    u8 cond = o->unkB9B;
    u16 *tbl;
    u16 x;

    if (o->unkBA0 & 0x18) {
        cond = 3;
    }
    if (o->unkBA0 & 0x20) {
        cond = 0;
    }
    x = id;
    n = x & 0xFFF;
    switch ((u16)((x & 0xF000) >> 12)) {
    default:
        return D_006F2780;
    case 1:
        return &D_006F2780[D_006F26D0[n]];
    case 2:
        if ((u16)o->unkCA8 != 0) {
            tbl = D_006F1980[D_006E5944[(u16)o->unkCA8].heavy];
        } else {
            tbl = D_006F1980[0];
        }
        return &D_006F2780[tbl[n * 4 + cond]];
    case 3:
        return &D_006F2780[D_006F2250[n][cond]];
    }
}
s32 func_005A4390(DrawObjFull *o);
void func_005A4890(DrawObjFull *o, MotRef *m, f32 a, f32 b);

/* Starts motion id on o (a and b: the blend and the start frame, it seems). */
void func_005A4490(DrawObjFull *o, s16 id, f32 a, f32 b) {
    MotState save;
    MotRef m;
    s16 alt;
    s32 flags = o->unkBA4;

    if ((o->unkBA0 & 0x80) && func_005A4390(o) != 0) {
        return;
    }
    o->unkB9C = o->unkB9B;
    if (o->unkBA0 & 8) {
        o->unkB9C = 3;
    }
    o->unk520 = id;
    switch (id) {
    case 0x2050:
    case 0x2051:
        if (D_006E5944[(u16)o->unkCA8].heavy == 1 && o->unkB9B < 2) {
            m = *func_005A3E60(o, (u16)id);
            func_005A4890(o, &m, a, b);
            if (flags & 2) {
                o->unkBA4 |= 2;
            }
            func_005A4490(o, 0x2002, 0.0f, 10.0f);
            o->unk204[0xC] = 1;
            o->unk520 = id;
            return;
        }
        if (o->unkB9B == 1) {
            save = *(MotState *)o->unk204;
            o->unkB9B = 0;
            alt = id == 0x2050 ? 0x200C : 0x200D;
            m = *func_005A3E60(o, (u16)alt);
            func_005A4890(o, &m, a, b);
            o->unkB9B = 1;
            if (((u16 *)&save)[7] == 0x82) {
                *(MotState *)o->unk204 = save;
                return;
            }
            m = *func_005A3E60(o, (u16)id);
            if (flags & 2) {
                o->unkBA4 |= 2;
            }
            func_005A4890(o, &m, a, b);
            return;
        }
        m = *func_005A3E60(o, (u16)id);
        func_005A4890(o, &m, a, b);
        return;
    case 0x2003:
        if (o->unkB98 == 5 && a < 1.0f && D_006E5944[(u16)o->unkCA8].heavy == 1
            && o->unkB9B < 2) {
            m = *func_005A3E60(o, (u16)id);
            func_005A4890(o, &m, 62.0f, b);
            return;
        }
        break;
    }
    m = *func_005A3E60(o, (u16)id);
    func_005A4890(o, &m, a, b);
}

/* A motion reference unpacked (func_005A3D10). */
typedef struct {
    s8 set;             /* 0x0: which of the character's motion sets */
    u8 unk1;
    s8 unk2;            /* 0x2: (nonzero) func_005A50F0 */
    s8 unk3;            /* 0x3: its D_006E7C30 entry */
    s8 unk4;            /* 0x4: its D_006E7C50 entry (the move per frame) */
    u8 unk5;            /* 0x5 */
    u8 unk6;            /* 0x6 */
    u8 pad7;
    s16 frame1;         /* 0x8: layer 1's (and 2-4's) motion */
    s16 frame0;         /* 0xA: layer 0's */
} MotInfo;              /* 0xC */

/* A move: its distance and its length in frames. */
typedef struct {
    f32 v[4];
    s32 frames;         /* 0x10 */
} MotMove;

typedef struct {
    f32 v[4];
} Vec4;

extern s32 *D_006E7C30[];       /* per character */
extern MotMove **D_006E7C50[];  /* per character */
extern f32 D_0023A8E0;
void func_005A3D10(DrawObjFull *o, MotInfo *info, MotRef *m);
void func_005A50F0(DrawObjFull *o, s32 a);
s32 func_0055B6D0(DrawObjFull *o, void *ms, s32 motion, u16 layer, f32 start, f32 blend);
void func_00192500(void *anim, u16 layer);
void *memset(void *p, s32 c, u32 n);

/* Unpacks motion reference m (set 0xF: the character's own entry, m's last number on). */
void func_005A3D10(DrawObjFull *o, MotInfo *info, MotRef *m) {
    info->set = (((u8 *)m)[0] & 0xF0) >> 4;
    if (info->set == 0xF) {
        switch (o->type) {
        case 0:
            m = &D_006F2780[(u16)m->v[3] + o->unkB98];
            info->set = (((u8 *)m)[0] & 0xF0) >> 4;
            break;
        case 1:
            m = &D_006F2780[(u16)m->v[3] + o->unk4[0]];
            break;
        }
    }
    if (o->type == 1) {
        switch (info->set) {
        case 2:
        case 0xF:
            info->set = 1;
            break;
        }
    }
    info->unk3 = (((u8 *)m)[2] & 0xF0) >> 4;
    info->unk4 = ((u8 *)m)[2] & 0xF;
    info->unk2 = ((u8 *)m)[0] & 0xF;
    info->unk5 = (((u16)m->v[2] & 0xF000) >> 8) | (((u16)m->v[3] & 0xF000) >> 12);
    info->frame1 = (u16)m->v[2] & 0xFFF;
    info->frame0 = (u16)m->v[3] & 0xFFF;
    info->unk6 = ((u8 *)m)[3];
    info->unk2 = ((info->unk2 & 0xC) << 3) | ((info->unk2 & 3) << 1);
}

DrawObjFull *func_00631920(u8 a, u8 b);

/* Notes (0xCE0, 0xCE1) which motion set a motion comes from: the shared or own (by unkB99),
   the character's (players: +2; NPCs: 10), or another character's (0xFF and its kind). */
void func_005A3C10(DrawObjFull *o, u8 set) {
    u8 a = 0;
    u8 b = 0;
    DrawObjFull *p;

    switch (set) {
    case 0:
    case 1:
        b = (u8)o->unkB99 != 0;
        break;
    case 2:
        switch (o->type) {
        case 0:
            b = o->unkB98 + 2;
            break;
        case 1:
            b = 10;
            break;
        }
        break;
    case 3:
    case 4:
    case 5:
        break;
    case 6:
    case 7:
    case 8:
        p = FIELD(o, DrawObjFull *, 0xB24);
        if (p != NULL) {
            b = p->kind;
            a = 0xFF;
        }
        break;
    case 9:
    case 10:
    case 11:
        p = func_00631920(FIELD(o, u8, 0x148), FIELD(o, u8, 0x149));
        if (p != NULL) {
            b = p->kind;
            a = 0xFF;
        }
        break;
    case 12:
    case 13:
    case 14:
        p = func_00631920(FIELD(o, u8, 0x14A), FIELD(o, u8, 0x14B));
        if (p != NULL) {
            b = p->kind;
            a = 0xFF;
        }
        break;
    }
    o->unkCE0 = a;
    o->unkCE1 = b;
}

/* The motion set number set means for o: its own three, or those of another character. */
void *func_005A3FB0(DrawObjFull *o, s8 set) {
    void *r;
    DrawObjFull *p;

    func_005A3C10(o, set);
    switch (set) {
    case 0:
    case 1:
    case 2:
        r = o->motions[set];
        if (o->type == 1) {
            r = o->motions[1];
        }
        break;
    case 3:
    case 4:
    case 5:
        r = o->motions[(u32)(set - 3)];
        break;
    case 6:
    case 7:
    case 8:
        p = FIELD(o, DrawObjFull *, 0xB24);
        if (p == NULL) {
            return NULL;
        }
        r = p->motions[(u32)(set - 6)];
        break;
    case 9:
    case 10:
    case 11:
        p = func_00631920(FIELD(o, u8, 0x148), FIELD(o, u8, 0x149));
        if (p == NULL) {
            return NULL;
        }
        r = p->motions[(u32)(set - 9)];
        break;
    case 12:
    case 13:
    case 14:
        p = func_00631920(FIELD(o, u8, 0x14A), FIELD(o, u8, 0x14B));
        if (p == NULL) {
            return NULL;
        }
        r = p->motions[(u32)(set - 12)];
        break;
    }
    return r;
}

/* Starts motion m on o: its move per frame, its layers' motions. */
void func_005A4890(DrawObjFull *o, MotRef *m, f32 a, f32 b);
#ifdef NON_MATCHING   /* first draft */
void func_005A4890(DrawObjFull *o, MotRef *m, f32 a, f32 b) {
    MotInfo info;
    s32 *t30;
    MotMove **t50;
    MotMove *mv;
    void *set;
    u32 i;
    s8 *p;

    func_005A3D10(o, &info, m);
    switch (o->type) {
    case 0:
        t50 = D_006E7C50[o->unkB98];
        t30 = D_006E7C30[o->unkB98];
        break;
    case 1:
        t30 = D_006E7C30[o->unk4[0]];
        t50 = D_006E7C50[o->unk4[0]];
        break;
    }
    o->unk1A8 = (s32)&t30[info.unk3];
    FIELD(o, MotMove *, 0xCB8) = t50[info.unk4];
    mv = FIELD(o, MotMove *, 0xCB8);
    FIELD(o, f32, 0xCC8) = mv->v[0] - o->unk1AC[0];
    FIELD(o, f32, 0xCCC) = FIELD(o, MotMove *, 0xCB8)->v[1] - o->unk1AC[1];
    FIELD(o, f32, 0xCD0) = FIELD(o, MotMove *, 0xCB8)->v[2] - o->unk1B4[0];
    FIELD(o, u8, 0xCB2) = t50[info.unk4]->frames;
    if (FIELD(o, u8, 0xCB2) != 0) {
        FIELD(o, f32, 0xCC8) /= (u32)FIELD(o, u8, 0xCB2);
        FIELD(o, f32, 0xCCC) /= (u32)FIELD(o, u8, 0xCB2);
        FIELD(o, f32, 0xCD0) /= (u32)FIELD(o, u8, 0xCB2);
    } else {
        *(Vec4 *)o->unk1AC = *(Vec4 *)FIELD(o, MotMove *, 0xCB8)->v;
    }
    for (i = 0; i < 2; i++) {
        p = &FIELD(o, s8, 0x1F1 + i * 0x20);
        if (*p & 0x80) {
            *p &= 0x7F;
        } else {
            switch (i) {
            case 0:
                if (info.frame0 != 0) {
                    *p = 0;
                }
                break;
            case 1:
                if (info.frame1 != 0) {
                    *p = 0;
                }
                break;
            default:
                *p = 0;
                break;
            }
        }
    }
    if (info.unk2 != 0) {
        func_005A50F0(o, info.unk2);
    }
    set = func_005A3FB0(o, info.set);
    if (set == NULL) {
        return;
    }
    if (o->type == 0 && (o->unkCE2 & 0x20)) {
        b += D_0023A8E0;
    }
    if (o->unkBA4 & 2) {
        o->unkBA4 &= ~2;
        b = 0.0f;
    }
    func_0055B6D0(o, set, info.frame1, 1, info.frame1 != 0 ? a : FIELD(o, f32, 0x204), b);
    func_0055B6D0(o, set, info.frame0, 0, info.frame0 != 0 ? a : FIELD(o, f32, 0x1E4), b);
    FIELD(o, s16, 0x212) = info.frame1;
    FIELD(o, s16, 0x1F2) = info.frame0;
    func_00192500(FIELD(o->model, void *, 0x820), 2);
    func_00192500(FIELD(o->model, void *, 0x820), 3);
    func_00192500(FIELD(o->model, void *, 0x820), 4);
    memset(&FIELD(o, u8, 0x224), 0, 0x20);
    memset(&FIELD(o, u8, 0x244), 0, 0x20);
    memset(&FIELD(o, u8, 0x264), 0, 0x20);
    func_0055B6D0(o, set, info.frame1, 2, a, b);
    func_0055B6D0(o, set, info.frame1, 3, a, b);
    func_0055B6D0(o, set, info.frame1, 4, a, b);
    if (o->type == 0) {
        o->unkCE3 = o->unkB99 != 0 ? info.unk6 : info.unk5;
        o->unkCE2 = 0;
    }
}
#endif
