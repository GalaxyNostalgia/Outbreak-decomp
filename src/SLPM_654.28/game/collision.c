#include "common.h"
#include "vu0.h"

/* GAME.BIN: line-of-sight tests against the room's collision triangles. The second collision grid
   (see collgrid.c) gives the cells a segment crosses; each cell lists triangles (-1 ends). */

typedef struct {
    u32 unk0;           /* cell width */
    u32 unk4;           /* cell depth */
    u32 ny;             /* 0x08: cells along x */
    u32 nx;             /* 0x0C: cells along z */
    u32 unk10;          /* origin x */
    u32 unk14;          /* origin z */
    s32 **cells;        /* 0x18 */
    u8 *list;           /* 0x1C */
} CollGrid;

typedef struct {
    CollGrid g[2];
    s32 unk40[2];
} CollGrids;

extern CollGrids D_003938A8;

/* A triangle: its kind (low byte), sub-kind and id, three vertices, the plane (normal, d). */
typedef struct {
    u32 info;
    f32 v[3][3];        /* 0x04 */
    f32 n[3];           /* 0x28 */
    f32 d;              /* 0x34 */
} CollTri;

extern f32 D_00239CA8;  /* 0.001 */

u32 func_001918C0(f32 rad);
f32 func_00191AD0(f32 x);
void func_00340FC0(f32 s, const f32 *dir, const f32 *p, f32 *out);
void func_003405D0(f32 s, f32 *out, const f32 *v);
s32 func_00340C50(const f32 *poly, const f32 *p, s32 n);
void func_00341460(const f32 *n, const f32 *a, const f32 *b, f32 *m);
void func_0018F7C0(f32 *m);

typedef struct {
    f32 x, y, z;
} Vec3f;

/* Whether p is over grid 1 (in x/z). */
s32 func_005E5340(const f32 *p) {
    s32 r;

    if (p[0] < D_003938A8.g[1].unk10 || p[2] < D_003938A8.g[1].unk14) {
        r = 0;
    } else if (p[0] < D_003938A8.g[1].unk10 + D_003938A8.g[1].unk0 * D_003938A8.g[1].ny && p[2] < D_003938A8.g[1].unk14 + D_003938A8.g[1].unk4 * D_003938A8.g[1].nx) {
        r = 1;
    } else {
        r = 0;
    }
    return r;
}

/* The triangle list of grid 1's cell under p. */
CollTri **func_005E5F10(const f32 *p) {
    s32 cz = (p[2] - D_003938A8.g[1].unk14) / D_003938A8.g[1].unk4;
    s32 cx = (p[0] - D_003938A8.g[1].unk10) / D_003938A8.g[1].unk0;

    return (CollTri **)D_003938A8.g[1].cells[cx * D_003938A8.g[1].nx + cz];
}

/* Whether p (on the triangle's plane) is inside it: the angles it sees the edges under add up to
   a full turn. */
s32 func_005E58E0(CollTri *t, const f32 *p) {
    f32 a[4];
    f32 b[4];
    f32 c[4];
    f32 d0;
    f32 d1;
    f32 d2;
    u32 sum;

    a[0] = t->v[0][0] - p[0];
    a[1] = t->v[0][1] - p[1];
    a[2] = t->v[0][2] - p[2];
    b[0] = t->v[1][0] - p[0];
    b[1] = t->v[1][1] - p[1];
    b[2] = t->v[1][2] - p[2];
    c[0] = t->v[2][0] - p[0];
    c[1] = t->v[2][1] - p[1];
    c[2] = t->v[2][2] - p[2];
    func_00191750(a);
    func_00191750(b);
    func_00191750(c);
    d0 = func_001917C0(a, b);
    if (d0 > 1.0f) {
        d0 = 1.0f;
    }
    if (d0 < -1.0f) {
        d0 = -1.0f;
    }
    d1 = func_001917C0(b, c);
    if (d1 > 1.0f) {
        d1 = 1.0f;
    }
    if (d1 < -1.0f) {
        d1 = -1.0f;
    }
    d2 = func_001917C0(c, a);
    if (d2 > 1.0f) {
        d2 = 1.0f;
    }
    if (d2 < -1.0f) {
        d2 = -1.0f;
    }
    d0 = func_00191AD0(d0);
    d1 = func_00191AD0(d1);
    d2 = func_00191AD0(d2);
    sum = (u16)func_001918C0(d0) + (u16)func_001918C0(d1) + (u16)func_001918C0(d2);
    if (sum >= 0xFFF0) {
        return 1;
    }
    return 0;
}

/* Whether the segment from-to crosses the triangle; hit gets the crossing (nudged past it). Only
   triangles facing the segment count, unless *flag is set (then set again). */
s32 func_005491E0(CollTri *t, const f32 *from, const f32 *to, f32 *hit, u8 *flag);
#ifdef NON_MATCHING   /* the plane maths' register use */
s32 func_005491E0(CollTri *t, const f32 *from, const f32 *to, f32 *hit, u8 *flag) {
    u8 back = 0;
    s32 res = 0;
    f32 e[4];
    f32 a[4];
    f32 b[4];
    f32 v[4];
    f32 n[4];
    f32 k;
    f32 den;

    v[0] = t->v[0][0] - from[0];
    v[1] = t->v[0][1] - from[1];
    v[2] = t->v[0][2] - from[2];
    n[0] = t->n[0];
    n[1] = t->n[1];
    n[2] = t->n[2];
    k = func_001917C0(v, n);
    if (*flag == 0) {
        if (k > 0.0f) {
            return 0;
        }
    } else {
        back = 1;
    }
    e[0] = to[0] - from[0];
    e[1] = to[1] - from[1];
    e[2] = to[2] - from[2];
    k = func_001917C0(e, n);
    if (*flag == 0) {
        if (k > 0.0f) {
            return 0;
        }
    } else {
        back = 1;
    }
    e[1] = to[1] - from[1];
    k = -(t->d + (t->n[0] * from[0] + t->n[1] * from[1] + t->n[2] * from[2]));
    den = t->n[0] * e[0] + t->n[1] * e[1] + t->n[2] * e[2];
    if (den != 0.0f) {
        k = k / den;
    }
    hit[0] = from[0] + e[0] * k;
    hit[1] = from[1] + e[1] * k;
    hit[2] = from[2] + e[2] * k;
    a[0] = from[0] - hit[0];
    a[1] = from[1] - hit[1];
    a[2] = from[2] - hit[2];
    b[0] = to[0] - hit[0];
    b[1] = to[1] - hit[1];
    b[2] = to[2] - hit[2];
    func_00191750(a);
    func_00191750(b);
    if (func_001917C0(b, a) < 0.0f && func_005E58E0(t, hit)) {
        res++;
        func_00340FC0(D_00239CA8, a, hit, hit);
    }
    *flag = back;
    return res;
}
#endif

/* The cells of grid 1 the segment from-to crosses (in x/z), in order, without repeats; out ends
   with 0. */
#ifdef NON_MATCHING   /* float register allocation and the loop's shape */
s32 func_0054CF00(const f32 *from, const f32 *to, s32 **out) {
    f32 x;
    f32 z;
    f32 step;
    f32 len;
    f32 s;
    f32 v[3];
    f32 p[3];
    s32 n = 0;
    s32 cx;
    s32 cz;
    s32 *cell;

    *out = 0;
    v[0] = (u32)D_003938A8.g[1].unk0;
    v[2] = (u32)D_003938A8.g[1].unk4;
    v[1] = 0.0f;
    step = func_00191690(v) / 2.0f;
    x = from[0];
    z = from[2];
    v[0] = to[0] - x;
    v[2] = to[2] - from[2];
    len = func_00191690(v);
    func_00191750(v);
    s = 0.0f;
    for (;;) {
        func_003405D0(s, p, v);
        cx = (u32)((s32)(x + p[0]) - D_003938A8.g[1].unk10) / (u32)D_003938A8.g[1].unk0;
        cz = (u32)((s32)(z + p[2]) - D_003938A8.g[1].unk14) / (u32)D_003938A8.g[1].unk4;
        if (cx < 0 || cz < 0 || cx >= D_003938A8.g[1].ny || cz >= D_003938A8.g[1].nx) {
            break;
        }
        cell = D_003938A8.g[1].cells[cz + cx * D_003938A8.g[1].nx];
        if (*out != cell) {
            *out = cell;
            n++;
            out++;
            *out = 0;
        }
        if (s == len) {
            break;
        }
        s += step;
        if (!(s <= len)) {
            s = len;
        }
    }
    return n;
}
#endif

/* The same for grid 0, in steps of 0.4 cells. */
s32 func_00549610(const f32 *from, const f32 *to, s32 **out);
#ifdef NON_MATCHING   /* float register allocation and the loop's shape (as func_0054CF00) */
s32 func_00549610(const f32 *from, const f32 *to, s32 **out) {
    f32 x;
    f32 z;
    f32 step;
    f32 len;
    f32 s;
    f32 v[3];
    f32 p[3];
    s32 n = 0;
    s32 cx;
    s32 cz;
    s32 *cell;

    *out = 0;
    v[0] = (u32)D_003938A8.g[0].unk0;
    v[2] = (u32)D_003938A8.g[0].unk4;
    v[1] = 0.0f;
    step = func_00191690(v) / 2.5f;
    x = from[0];
    z = from[2];
    v[0] = to[0] - x;
    v[2] = to[2] - from[2];
    len = func_00191690(v);
    func_00191750(v);
    s = 0.0f;
    for (;;) {
        func_003405D0(s, p, v);
        cx = (u32)((s32)(x + p[0]) - D_003938A8.g[0].unk10) / (u32)D_003938A8.g[0].unk0;
        cz = (u32)((s32)(z + p[2]) - D_003938A8.g[0].unk14) / (u32)D_003938A8.g[0].unk4;
        if (cx < 0 || cz < 0 || cx >= D_003938A8.g[0].ny || cz >= D_003938A8.g[0].nx) {
            break;
        }
        cell = D_003938A8.g[0].cells[cz + cx * D_003938A8.g[0].nx];
        if (*out != cell) {
            *out = cell;
            n++;
            out++;
            *out = 0;
        }
        if (s == len) {
            break;
        }
        s += step;
        if (!(s <= len)) {
            s = len;
        }
    }
    return n;
}
#endif

extern u8 D_004747C0[0x10][0x60];   /* (D_00470090's switch entries, see aots.c) */

/* Whether triangle t counts: its kind (low byte) is one of the bits of want's low half, its
   flags (high half) meet want's, it isn't one of skip's kinds or flags nor switched off
   (D_003938A8.unk40[0]), and a kind-1 (door) triangle's switch is shut (with open: or opening). */
s32 func_005E6050(CollTri *t, u32 want, u32 skip, s32 open);
#ifdef NON_MATCHING   /* first draft: registers, the 0xFFFF compare's constant */
s32 func_005E6050(CollTri *t, u32 want, u32 skip, s32 open) {
    s32 ok = 0;
    s32 i;
    u32 info;
    u32 hi;
    u32 k;

    if ((u8)want != 0xFFFF) {
        for (i = 0; i < 0x10; i++) {
            if (((u16)want & (1 << i)) && *(u8 *)&t->info == i) {
                ok = 1;
            }
        }
    }
    if (ok == 0) {
        return 0;
    }
    info = t->info;
    hi = info & 0xFFFF0000;
    if (!(hi & want)) {
        return 0;
    }
    if ((u8)skip != 0xFFFF) {
        for (i = 0; i < 0x10; i++) {
            if (((u16)skip & (1 << i)) && (info & 0xFF) == i) {
                return 0;
            }
        }
    }
    if (hi & skip) {
        return 0;
    }
    if (hi & D_003938A8.unk40[0]) {
        return 0;
    }
    if ((info & 0xFF) == 1) {
        k = ((hi >> 16) & 0xF00) >> 8;
        if (open != 0 && D_004747C0[k][0x14] != 2) {
            return 1;
        }
        if (D_004747C0[k][1] != 2) {
            return 1;
        }
        if (!((hi >> 16) & 0x40)) {
            return 0;
        }
    }
    return 1;
}
#endif

/* A triangle's flags with its kind as a bit (kinds 0-15). */
s32 func_005E61E0(u32 info);
#ifdef NON_MATCHING   /* register choice */
s32 func_005E61E0(u32 info) {
    u32 hi = info & 0xFFFF0000;

    info &= 0xF;
    return hi | (1 << info);
}
#endif

/* Whether a triangle of a cell's list (that func_005E6050 accepts for kind, a) blocks from-to;
   *which gets the blocker's func_005E61E0 (0xFF: none). */
s32 func_00549930(CollTri **list, const f32 *from, const f32 *to, u32 kind, u32 a, s32 *which) {
    f32 hit[3];
    u8 flag;
    s32 i;
    CollTri *t;

    *which = 0xFF;
    for (i = 0; (t = list[i]) != (CollTri *)-1; i++) {
        if (func_005E6050(t, kind, a, 0)) {
            flag = 0;
            if (func_005491E0(t, from, to, hit, &flag)) {
                *which = func_005E61E0(t->info);
                return 1;
            }
        }
    }
    return 0;
}

extern s32 D_0070E6D0[];        /* the objects' triangles (collgrid.c) */

/* Whether anything blocks from-to on grid 0 (or among the objects' triangles). */
s8 func_00549860(const f32 *from, const f32 *to, s32 kind, s32 a, s32 *which) {
    s32 *cells[256];
    s32 n;
    s32 i;

    n = func_00549610(from, to, cells);
    cells[n] = D_0070E6D0;
    for (i = 0; i < n + 1; i++) {
        if (func_00549930((CollTri **)cells[i], from, to, kind, a, which)) {
            return 1;
        }
    }
    return 0;
}

/* Whether any triangle in a cell's list blocks from-to: kind 0xFF means any kind; skip is a kind
   to ignore. */
s32 func_0054D360(CollTri **list, const f32 *from, const f32 *to, s32 kind, s32 skip) {
    f32 hit[3];
    u8 flag;
    s32 i;
    CollTri *t;

    for (i = 0; (t = list[i]) != (CollTri *)-1; i++) {
        flag = 1;
        if (skip != *(u8 *)&t->info && (kind == 0xFF || kind == *(u8 *)&t->info)) {
            if (func_005491E0(t, from, to, hit, &flag)) {
                return 1;
            }
        }
    }
    return 0;
}

/* Whether anything of kind (0xFF: any) other than skip blocks the line from-to. */
s32 func_0054D4F0(const f32 *from, const f32 *to, s32 kind, s32 skip) {
    s32 *cells[256];
    s32 n;
    s32 i;

    n = func_0054CF00(from, to, cells);
    for (i = 0; i < n; i++) {
        if (func_0054D360((CollTri **)cells[i], from, to, kind, skip)) {
            return 1;
        }
    }
    return 0;
}

/* The floor under p (the highest facing up below p + 100, of kind want or, with 0xFF, any without
   bit 0x80): its height (p's own when none), normal, info, and with doMtx a matrix tilting to it. */
f32 func_005E5C20(const f32 *p, f32 *normal, f32 *m, u8 *kind, s32 want, u8 *sub, u16 *id,
                  u8 doMtx);
#ifdef NON_MATCHING   /* the 2D copies' addressing (addiu per element in the original) */
f32 func_005E5C20(const f32 *p, f32 *normal, f32 *m, u8 *kind, s32 want, u8 *sub, u16 *id,
                  u8 doMtx) {
    CollTri *hit = NULL;
    s32 i;
    s32 n;
    CollTri **list;
    CollTri *t;
    u32 w;
    f32 best;
    f32 y;

    best = p[1];
    *kind = 0xFF;
    *sub = 0xFF;
    *id = 0;
    normal[2] = 0.0f;
    normal[0] = 0.0f;
    normal[1] = 1.0f;
    if (func_005E5340(p) == 1) {
    n = 0;
    list = func_005E5F10(p);
    i = 0;
    w = want & 0xFF;
    while ((t = list[i]) != (CollTri *)-1) {
        f32 v[6];
        f32 a[2];

        if (w != 0xFF) {
            if (*(u8 *)&t->info != w) {
                i++;
                continue;
            }
        } else if (t->info & 0x80) {
            goto next;
        }
        a[0] = p[0];
        a[1] = p[2];
        v[0] = t->v[0][0];
        v[1] = t->v[0][2];
        v[2] = t->v[1][0];
        v[3] = t->v[1][2];
        v[4] = t->v[2][0];
        v[5] = t->v[2][2];
        if (t->n[1] <= 0.0f) {
            goto next;
        }
        if (func_00340C50(v, a, 3) == 1) {
            y = -(t->d + (t->n[0] * p[0] + t->n[2] * p[2])) / t->n[1];
            if (y < 100.0f + p[1]) {
                if (n == 0) {
                    best = y;
                    hit = t;
                    *kind = t->info;
                    *sub = (t->info & 0xFF00) >> 8;
                    *id = (t->info & 0xFFFF0000) >> 16;
                } else if (best < y) {
                    best = y;
                    hit = t;
                    *kind = t->info;
                    *sub = (t->info & 0xFF00) >> 8;
                    *id = (t->info & 0xFFFF0000) >> 16;
                }
                n++;
            }
        }
    next:
        i++;
    }
    if (hit != NULL) {
        *(Vec3f *)normal = *(Vec3f *)hit->n;
    }
    if (doMtx) {
        if (hit != NULL) {
            func_00341460(hit->n, hit->v[0], hit->v[1], m);
        } else {
            func_0018F7C0(m);
        }
    }
    }
    return best;
}
#endif

/* The height of the floor under p (want: a kind, 0xFF any); kind gets the floor's. */
f32 func_005E5B40(const f32 *p, u8 *kind, s32 want) {
    f32 m[16];
    f32 normal[3];
    u16 id;
    u8 sub;

    return func_005E5C20(p, normal, m, kind, want, &sub, &id, 0);
}

/* The same, wanting the floor's sub-kind. */
f32 func_005E5B80(const f32 *p, u8 *sub, s32 want) {
    f32 m[16];
    f32 normal[3];
    u16 id;
    u8 kind;

    return func_005E5C20(p, normal, m, &kind, want, sub, &id, 0);
}

extern s32 D_0023BEC4;          /* the objects' triangles in use */
extern CollTri D_0070E750[0x1F];

/* A new object triangle a, b, c with normal n (kind 0, flags 0x27), listed in D_0070E6D0; 0
   when all 31 are taken. */
s32 func_005E84F0(f32 *a, f32 *b, f32 *c, f32 *n) {
    CollTri *t;

    if (D_0023BEC4 >= 0x1F) {
        return 0;
    }
    t = &D_0070E750[D_0023BEC4];
    D_0070E6D0[D_0023BEC4] = (s32)t;
    D_0023BEC4++;
    D_0070E6D0[D_0023BEC4] = -1;
    t->info = 0x270000;
    *(Vec3f *)t->v[0] = *(Vec3f *)a;
    *(Vec3f *)t->v[1] = *(Vec3f *)b;
    *(Vec3f *)t->v[2] = *(Vec3f *)c;
    *(Vec3f *)t->n = *(Vec3f *)n;
    t->d = -(a[0] * n[0] + a[1] * n[1] + a[2] * n[2]);
    return 1;
}
