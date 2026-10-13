#include "common.h"
#include "object.h"
#include "model.h"
#include "thing.h"

/* Who is how far from whom: each frame (and at room setup) the distances, headings and line of
   sight between the players and the room's things, the NPCs and the things, and the players and
   the NPCs, with each one's list of the others nearest first. */

typedef struct {
    f32 dist;           /* 0x0 */
    s16 angle;          /* 0x4 */
    s8 hidden;          /* 0x6: no line of sight (or far) */
    s8 unk7;            /* 0x7 */
} Relation;

extern Relation D_00386110[8][8];       /* players to players (online) */
extern Relation D_00385490[8][50];      /* players to things */
extern u8 D_003840E0[8][50];            /* each player's things, nearest first */
extern u8 D_00383F50[50][8];            /* each thing's players, nearest first */
extern Relation D_003844F0[10][50];     /* NPCs to things */
extern u8 D_00383D50[10][50];
extern u8 D_00383B50[50][10];
extern Relation D_00384270[8][10];      /* players to NPCs */
extern u8 D_00383B00[8][10];
extern u8 D_00383AB0[10][8];
extern DrawObjFull D_00476DD0[8];
extern Npc D_003E8230[10];
extern Thing D_003FFCA0[50];
s32 func_001AF4D0(void);
f32 func_001916E0(const f32 *a, const f32 *b);
s16 func_00346C50(const f32 *from, const f32 *to);
s8 func_00549860(const f32 *from, const f32 *to, s32 mask, s32 a);
s8 func_0054D4F0(const f32 *from, const f32 *to, s32 a, s32 b);

/* Sorts n distances, and their ids alike, nearest first. */
void func_006A4680(f32 *dist, s32 n, u8 *ids) {
    s32 i;
    s32 j;
    f32 t;
    u8 k;

    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            if (dist[i] > dist[j]) {
                t = dist[i];
                dist[i] = dist[j];
                dist[j] = t;
                k = ids[i];
                ids[i] = ids[j];
                ids[j] = k;
            }
        }
    }
}

#define BONE_POS(m, i) ((f32 *)(((u8 **)((Slot1468 *)(m))->unk820)[i] + 0x30))

void func_006A3B60(void);
#ifdef NON_MATCHING   /* first draft */
void func_006A3B60(void) {
    f32 rows[68][68];
    f32 cols[68][68];
    s32 count[10];
    f32 from[3];
    f32 to[3];
    DrawObjFull *p;
    Npc *n;
    Thing *t;
    Relation *r;
    f32 *pos;
    f32 d;
    s32 nrows;
    s32 i;
    s32 j;

    nrows = 0;
    if (func_001AF4D0() == 1) {
        for (i = 0; i < 8; i++) {
            for (j = 0; j < 8; j++) {
                r = &D_00386110[i][j];
                r->dist = func_001916E0(D_00476DD0[i].unk38, D_00476DD0[j].unk38);
                r->angle = func_00346C50(D_00476DD0[i].unk38, D_00476DD0[j].unk38);
            }
        }
        nrows = 0;
    }

    /* players and things */
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 50; j++) {
            D_00383F50[j][i] = 0xFF;
            D_003840E0[i][j] = 0xFF;
        }
    }
    for (i = 0; i < 8; i++, nrows++) {
        count[i] = 0;
        p = &D_00476DD0[i];
        if (p->active == 0) {
            continue;
        }
        pos = BONE_POS(p->model, 11);
        from[0] = pos[0];
        from[1] = pos[1];
        from[2] = pos[2];
        for (j = 0; j < 50; j++) {
            t = &D_003FFCA0[j];
            if (p->unk1E2 != t->scenario || t->active == 0 || (t->unkC94 & 0x100)) {
                continue;
            }
            r = &D_00385490[i][j];
            D_003840E0[nrows][count[i]] = j;
            D_00383F50[count[i]][nrows] = i;
            d = func_001916E0(p->unk38, t->pos);
            r->dist = d;
            cols[count[i]][nrows] = d;
            rows[nrows][count[i]] = d;
            if (*(s32 *)t->model->unk820 != 0) {
                pos = BONE_POS(t->model, t->unk551);
            } else {
                pos = t->pos;
            }
            to[0] = pos[0];
            to[1] = pos[1];
            to[2] = pos[2];
            r->angle = func_00346C50(from, to);
            if (r->dist < 3000.0f) {
                r->hidden = func_00549860(from, to, 0x4FFFF, 0);
            } else {
                r->hidden = 1;
            }
            if (from[1] > to[1]) {
                to[1] += 1.0f;
            } else {
                to[1] -= 1.0f;
            }
            r->unk7 = func_0054D4F0(from, to, 0xFF, 0x81);
            count[i]++;
        }
    }
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < count[i]; j++) {
            func_006A4680(rows[i], count[i], D_003840E0[i]);
            func_006A4680(cols[j], nrows, D_00383F50[j]);
        }
    }

    /* NPCs and things */
    nrows = 0;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 50; j++) {
            D_00383B50[j][i] = 0xFF;
            D_00383D50[i][j] = 0xFF;
        }
    }
    for (i = 0; i < 10; i++, nrows++) {
        count[i] = 0;
        n = &D_003E8230[i];
        if (n->active == 0) {
            continue;
        }
        pos = BONE_POS(n->model, 11);
        from[0] = pos[0];
        from[1] = pos[1];
        from[2] = pos[2];
        for (j = 0; j < 50; j++) {
            t = &D_003FFCA0[j];
            if (n->unk1E2 != t->scenario || t->active == 0 || (t->unkC94 & 0x100)) {
                continue;
            }
            r = &D_003844F0[i][j];
            D_00383D50[nrows][count[i]] = j;
            D_00383B50[count[i]][nrows] = i;
            d = func_001916E0(n->unk38, t->pos);
            r->dist = d;
            cols[count[i]][nrows] = d;
            rows[nrows][count[i]] = d;
            if (*(s32 *)t->model->unk820 != 0) {
                pos = BONE_POS(t->model, t->unk551);
            } else {
                pos = t->pos;
            }
            to[0] = pos[0];
            to[1] = pos[1];
            to[2] = pos[2];
            r->angle = func_00346C50(from, to);
            r->hidden = 0;
            r->unk7 = 0;
            count[i]++;
        }
    }
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < count[i]; j++) {
            func_006A4680(rows[i], count[i], D_00383D50[i]);
            func_006A4680(cols[j], nrows, D_00383B50[j]);
        }
    }

    /* players and NPCs */
    nrows = 0;
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 10; j++) {
            D_00383AB0[j][i] = 0xFF;
            D_00383B00[i][j] = 0xFF;
        }
    }
    for (i = 0; i < 8; i++) {
        count[i] = 0;
        p = &D_00476DD0[i];
        if (p->active == 0) {
            continue;
        }
        for (j = 0; j < 10; j++) {
            n = &D_003E8230[j];
            if (p->unk1E2 != n->unk1E2 || n->active == 0) {
                continue;
            }
            r = &D_00384270[i][j];
            D_00383B00[nrows][count[i]] = j;
            D_00383AB0[count[i]][nrows] = i;
            d = func_001916E0(p->unk38, n->unk38);
            r->dist = d;
            cols[count[i]][nrows] = d;
            rows[nrows][count[i]] = d;
            r->angle = p->unk92 - n->unk92;
            r->hidden = 0;
            count[i]++;
        }
        nrows++;
    }
    for (i = 0; i < nrows; i++) {
        for (j = 0; j < count[i]; j++) {
            func_006A4680(rows[i], count[i], D_00383B00[i]);
            func_006A4680(cols[j], nrows, D_00383AB0[j]);
        }
    }
}
#endif
