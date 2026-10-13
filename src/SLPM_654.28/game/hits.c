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
