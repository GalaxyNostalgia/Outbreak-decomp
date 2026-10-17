#include "common.h"
#include "thing.h"

/* GAME.BIN: a thing's distance and direction to its target, from the relation tables that
   relations.c fills each frame. */

typedef struct {
    f32 dist;           /* 0x0 */
    u16 angle;          /* 0x4 */
    s8 hidden;          /* 0x6 */
    s8 unk7;            /* 0x7 */
} Relation;

/* The start every character shares: type 0 is a player, 1 an NPC. */
typedef struct {
    s8 active;
    s8 visible;
    u8 type;            /* 0x2 */
    s8 index;           /* 0x3 */
} ObjHead;

extern Relation D_00385490[8][50];      /* players to things */
extern Relation D_003844F0[10][50];     /* NPCs to things */

/* How far player p is from t. */
f32 func_00579440(Thing *t, s8 p) {
    if (t == NULL) {
        return 0.0f;
    }
    return D_00385490[p & 3][t->index].dist;
}

/* Which way player p is from t, relative to its heading. */
u16 func_005795F0(Thing *t, s8 p) {
    if (t == NULL) {
        return 0;
    }
    return D_00385490[p & 3][t->index].angle + 0x8000 - t->rot[1];
}

/* How far NPC n is from t. */
f32 func_00579810(Thing *t, s8 n) {
    if (t == NULL) {
        return 0.0f;
    }
    return D_003844F0[n][t->index].dist;
}

/* Which way NPC n is from t, relative to its heading. */
u16 func_00579960(Thing *t, s8 n) {
    if (t == NULL) {
        return 0;
    }
    return D_003844F0[n][t->index].angle + 0x8000 - t->rot[1];
}

/* Updates t's distance (keeping last frame's) and direction to target. */
void func_00579A40(Thing *t, ObjHead *target) {
    if (target != NULL) {
        t->unkC3C = t->unkBD4;
        if (target->type == 0) {
            t->unkBD4 = func_00579440(t, target->index);
            t->unkBD8 = func_005795F0(t, target->index);
        } else if (target->type == 1) {
            t->unkBD4 = func_00579810(t, target->index);
            t->unkBD8 = func_00579960(t, target->index);
        }
    }
}
