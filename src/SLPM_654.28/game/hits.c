#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"
#include "enemy.h"

/* Attacks in progress: each a list node with its attacker, the attack's definition, a delay,
   a count of hits and the interval between them. Damage done by us goes into the room's
   per-target totals (halved, or fifthed for the others' attacks, on the targets the
   scenario marks). */

typedef struct {
    s32 kind;           /* 0x00: 1-4 */
} HitDef;

typedef struct {
    u8 unk0[0xA];
    u8 hits;            /* 0x0A: how many */
    u8 interval;        /* 0x0B */
    u8 unkC[0xF - 0xC];
    u8 missSound;       /* 0x0F */
    u8 unk10[4];
    u16 damage;         /* 0x14 */
    u8 unk16[0x19 - 0x16];
    u8 hitSound;        /* 0x19 */
} HitDef2;

typedef struct Hit {
    struct Hit *next;   /* 0x00 */
    u8 unk4[4];
    u8 *owner;          /* 0x08: the attacker */
    u8 active;          /* 0x0C */
    u8 unkD;
    u8 delay;           /* 0x0E */
    u8 count;           /* 0x0F: frames left (0xFF: no limit) */
    HitDef *def;        /* 0x10 */
    HitDef2 *def2;      /* 0x14 */
    u8 unk18[3];
    u8 result;          /* 0x1B: 1 hit */
    u8 unk1C[0x28 - 0x1C];
    u8 hits;            /* 0x28 */
    u8 wait;            /* 0x29 */
    u8 unk2A;
    u8 unk2B;           /* 0x2B */
    u8 *target;         /* 0x2C */
} Hit;

extern Hit *D_0023BDA0;
extern s16 D_0023BE08;
extern u8 D_00470090[0x6D40];
void func_005A9890(Hit *h);
s32 func_005AA410(Hit *h, u8 *o, HitDef *d, s32 *k, s32 *done);
s32 func_005AA9B0(Hit *h, u8 *o, HitDef *d, s32 *k, s32 *done);
s32 func_005AAE60(Hit *h, u8 *o, HitDef *d, s32 *k, s32 *done);
s32 func_005AB3C0(Hit *h, u8 *o, HitDef *d, s32 *k, s32 *done);
void func_00548FC0(Hit *h);
void func_001B8C60(u8 *o, u8 sound);
s32 func_001B8BA0(u8 *o);
s32 func_001AF340(u8 player);
void func_0064EF20(void);

#define TICK(x)                                 \
    if (D_00306590.unkC != 0) {                 \
        (x) -= 2;                               \
        if ((x) == 0xFF) {                      \
            (x) = 0;                            \
        }                                       \
    } else {                                    \
        (x)--;                                  \
    }

/* Adds an attack's damage to target k's total (divided on the targets the scenario marks). */
#define ADD_DAMAGE(h, k, div)                                                              \
    do {                                                                                   \
        s16 *total_ = (s16 *)(D_00470090 + 0x4700);                                        \
        if ((k) < 0x10 &&                                                                  \
            (FIELD(&D_0048AC10, u16, D_00306590.unk1C * 0xC + 0x3A6) & (1 << (k)))) {      \
            total_[k] = total_[k] + (h)->def2->damage / (div);                             \
        } else {                                                                           \
            total_[k] += (s16)(h)->def2->damage;                                           \
        }                                                                                  \
    } while (0)

/* Once a frame. */
void func_005A9DB0(void);
#ifdef NON_MATCHING   /* first draft */
void func_005A9DB0(void) {
    Hit *h = D_0023BDA0;
    Hit *next;
    u8 *o;
    u8 *t;
    s32 r;
    s32 k;
    s32 done;

    while (h != NULL) {
        o = h->owner;
        if (o == NULL || o[0] == 0 || (o[0x115] & 1)) {
            next = h->next;
            func_005A9890(h);
            h = next;
            continue;
        }
        h->unk2B = 0;
        if (o[0x115] & 2) {
            h = h->next;
            continue;
        }
        h->unk2B = 0;
        if (h->active == 0) {
            h = h->next;
            continue;
        }
        if (h->delay != 0) {
            TICK(h->delay);
            h->unk2B = 0;
            h = h->next;
            continue;
        }
        if (h->count == 0) {
            next = h->next;
            func_005A9890(h);
            h = next;
            continue;
        }
        if (h->count != 0xFF) {
            TICK(h->count);
        }
        if (h->wait != 0) {
            TICK(h->wait);
            h->unk2B = 0;
            h = h->next;
            continue;
        }
        h->result = 0;
        done = 0;
        switch (h->def->kind) {
        case 1:
            r = func_005AA410(h, o, h->def, &k, &done);
            break;
        case 2:
            r = func_005AA9B0(h, o, h->def, &k, &done);
            break;
        case 3:
            r = func_005AAE60(h, o, h->def, &k, &done);
            break;
        case 4:
            r = func_005AB3C0(h, o, h->def, &k, &done);
            break;
        default:
            next = h->next;
            func_005A9890(h);
            h = next;
            continue;
        }
        if (h->result != 0) {
            func_00548FC0(h);
        }
        if (done == 0) {
            h = h->next;
            continue;
        }
        if (h->result == 1) {
            func_001B8C60(o, h->def2->hitSound);
        }
        if (r != 0) {
            if (o != NULL) {
                switch (o[2]) {
                case 0:
                    if (o[3] == D_0048BF60.unk1D) {
                        ADD_DAMAGE(h, k, 2.0f);
                    }
                    break;
                case 9:
                    t = h->target;
                    if (t == NULL) {
                        break;
                    }
                    if (t[2] == 0) {
                        if (t[3] == D_0048BF60.unk1D) {
                            ADD_DAMAGE(h, k, 2.0f);
                        }
                        break;
                    }
                default:
                    if (func_001AF340(D_0048BF60.unk1D) != 0) {
                        ADD_DAMAGE(h, k, 5.0f);
                    }
                    break;
                }
            }
        } else if (h->result == 0 && func_001B8BA0(o) != 0) {
            func_001B8C60(o, h->def2->missSound);
        }
        if (h->def2->hits < h->hits) {
            next = h->next;
            func_005A9890(h);
            h = next;
            continue;
        }
        h->wait = h->def2->interval;
        h = h->next;
    }
    func_0064EF20();
    D_0023BE08 = 0;
}
#endif

extern u8 *D_003937A0[0x40];    /* the objects that can be hit this frame */

/* Clears o's hits for the frame and, unless it is still in its hit wait (0x32; 0xFF: never),
   lists it among the targets (when it has a hit shape, 0x1D8). */
void func_005ABA30(u8 *o);
#ifdef NON_MATCHING   /* the original leaves nops in its branch delay slots */
void func_005ABA30(u8 *o) {
    u8 w;

    *(s32 *)(o + 0x138) = 0;
    *(s16 *)(o + 0x14E) = 0;
    o[0x13C] = 0;
    o[0x13F] = 0;
    o[0x116] = 0;
    w = o[0x32];
    if (w != 0) {
        if ((w & 0xFF) != 0xFF) {
            o[0x32] = w - 1;
        }
    } else if ((u16)D_0023BE08 < 0x40 && *(s32 *)(o + 0x1D8) != 0) {
        D_003937A0[(u16)D_0023BE08] = o;
        (*(u16 *)&D_0023BE08)++;
    }
}
#endif

/* A character's hit spheres (0x1D8, ended by radius -1): offset from a bone, radius, bone. */
typedef struct {
    f32 off[3];         /* 0x00 */
    f32 r;              /* 0x0C */
    s32 bone;           /* 0x10 */
} HitSphere;

void func_001913A0(f32 *out, const f32 *v, const f32 *m);

typedef struct {
    f32 x, y, z;
} Vec3f;

/* Where o's hit spheres are this frame (into 0x1DC, their number into 0x1E0), by its bones. */
void func_005AB840(u8 *o);
#ifdef NON_MATCHING   /* the original keeps &p.y and &p.z in s registers */
void func_005AB840(u8 *o) {
    HitSphere *h = *(HitSphere **)(o + 0x1D8);
    f32 *out;
    s32 n;
    f32 *m;
    Vec3f p;

    if (h == NULL) {
        return;
    }
    out = *(f32 **)(o + 0x1DC);
    for (n = 0; h->r != -1.0f; h++, out += 3) {
        m = *(f32 **)(*(u8 **)(o + 0x34) + 0x820 + h->bone * 4);
        func_001913A0(&p.x, h->off, m);
        n++;
        out[0] = p.x + m[12];
        out[1] = p.y + m[13];
        out[2] = p.z + m[14];
    }
    o[0x1E0] = n;
}
#endif

/* The same for a thing, by its own matrix (0x50). */
void func_005AB930(u8 *o);
#ifdef NON_MATCHING   /* the original keeps &p.y and &p.z in s registers; loop layout */
void func_005AB930(u8 *o) {
    HitSphere *h = *(HitSphere **)(o + 0x1D8);
    f32 *out;
    s32 n;
    f32 *m;
    Vec3f p;

    if (h == NULL) {
        return;
    }
    out = *(f32 **)(o + 0x1DC);
    m = (f32 *)(o + 0x50);
    for (n = 0; h->r != -1.0f; h++, out += 3) {
        func_001913A0(&p.x, h->off, m);
        n++;
        out[0] = p.x + m[12];
        out[1] = p.y + m[13];
        out[2] = p.z + m[14];
    }
    o[0x1E0] = n;
}
#endif
