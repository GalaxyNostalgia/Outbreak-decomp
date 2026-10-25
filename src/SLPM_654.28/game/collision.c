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

/* Which quarter of its grid-0 cell p is in: +1 in the far half along x, +2 along z. */
s32 func_005E5040(const f32 *p);
#ifdef NON_MATCHING   /* load order of the grid's sizes (registers) */
s32 func_005E5040(const f32 *p) {
    s32 r;
    f32 dz;
    u32 d;
    s32 cz;
    u32 w;
    f32 dx;
    s32 cx;

    r = 0;
    dz = p[2] - D_003938A8.g[0].unk14;
    d = D_003938A8.g[0].unk4;
    cz = dz / d;
    w = D_003938A8.g[0].unk0;
    dx = p[0] - D_003938A8.g[0].unk10;
    cx = dx / w;
    if (!(dx < w / 2 + cx * w)) {
        r = 1;
    }
    if (!(dz < d / 2 + cz * d)) {
        r += 2;
    }
    return r;
}
#endif

/* The triangle list of grid 0's cell under p. */
CollTri **func_005E5490(const f32 *p) {
    s32 cz = (p[2] - D_003938A8.g[0].unk14) / D_003938A8.g[0].unk4;
    s32 cx = (p[0] - D_003938A8.g[0].unk10) / D_003938A8.g[0].unk0;

    return (CollTri **)D_003938A8.g[0].cells[cx * D_003938A8.g[0].nx + cz];
}

/* Whether p is over grid 0. */
s32 func_005E51F0(const f32 *p) {
    s32 r;

    if (p[0] < D_003938A8.g[0].unk10 || p[2] < D_003938A8.g[0].unk14) {
        r = 0;
    } else if (p[0] < D_003938A8.g[0].unk10 + D_003938A8.g[0].unk0 * D_003938A8.g[0].ny && p[2] < D_003938A8.g[0].unk14 + D_003938A8.g[0].unk4 * D_003938A8.g[0].nx) {
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

f32 func_001916E0(const f32 *a, const f32 *b);

/* The nearest of a cell's triangles (of mask's kinds, not b's) the line from-to hits: how many
   it hits; hit, normal and kind (0xFF none) for the nearest. */
s32 func_00549020(CollTri **list, const f32 *from, f32 *to, f32 *hit, f32 *normal, s32 mask,
                  s32 b, s32 *kind);
#ifdef NON_MATCHING   /* close: the hit's last two coordinates go through an address register; s-register pairs */
s32 func_00549020(CollTri **list, const f32 *from, f32 *to, f32 *hit, f32 *normal, s32 mask,
                  s32 b, s32 *kind) {
    f32 p[3];
    u8 flag;
    s32 i = 0;
    s32 n = 0;
    CollTri *t;
    f32 best = 0.0f;
    f32 d;

    *kind = 0xFF;
    for (; (t = list[i]) != (CollTri *)-1; i++) {
        if (!func_005E6050(t, mask, b, 0)) {
            continue;
        }
        flag = 0;
        if (!func_005491E0(t, from, to, p, &flag)) {
            continue;
        }
        if (n) {
            d = func_001916E0(from, p);
            if (!(best <= d)) {
                best = d;
                normal[0] = t->n[0];
                normal[1] = t->n[1];
                normal[2] = t->n[2];
                hit[0] = p[0];
                hit[1] = p[1];
                hit[2] = p[2];
                *kind = func_005E61E0(t->info);
            }
        } else {
            normal[0] = t->n[0];
            normal[1] = t->n[1];
            normal[2] = t->n[2];
            hit[0] = p[0];
            hit[1] = p[1];
            hit[2] = p[2];
            best = func_001916E0(from, hit);
            *kind = func_005E61E0(t->info);
        }
        n++;
    }
    return n;
}
#endif

/* The nearest hit along from-to on grid 0 and the objects' triangles (of mask's kinds): 1 when
   there is one (hit, normal and kind set). */
s32 func_005494C0(const f32 *from, const f32 *to, f32 *hit, f32 *normal, s32 mask, s32 b,
                  s32 *kind) {
    s32 *cells[256];
    Vec3 end;
    Vec3 tmp;
    s32 k;
    s32 n;
    s32 r = 0;
    s32 i;

    end = *(Vec3 *)to;
    *kind = 0;
    n = func_00549610(from, &end.x, cells);
    cells[n] = D_0070E6D0;
    for (i = 0; i < n + 1; i++) {
        if (func_00549020((CollTri **)cells[i], from, &end.x, &tmp.x, normal, mask, b, &k)) {
            r = 1;
            *(Vec3 *)hit = tmp;
            end = tmp;
            *kind = k;
        }
    }
    return r;
}

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

/* A sphere moving from from to pos (radius r, len apart). */
typedef struct {
    f32 pos[3];         /* 0x00 */
    f32 from[3];        /* 0x0C */
    f32 r;              /* 0x18 */
    f32 len;            /* 0x1C */
} CollSweep;

typedef struct {
    f32 v[3];
} CollVec;

extern CollTri *D_0070F120[];   /* the triangles hit this time */
extern f32 D_006DB1C0[][2];     /* per quarter of a cell: which way the next cells lie */
extern s16 D_0023BECC;
extern s8 D_0023BEC8;
s32 func_005E6A90(CollSweep *s, CollTri *t, f32 *n, f32 *q);
s16 func_005E7280(CollSweep *s, f32 *push);

/* A sphere of radius r moving from from to to against the walls (the triangles of the four
   cells round to, then the objects' own) that func_005E6050 says count for want, skip and
   open: push gets the way out, n and q what the last one hit gave; the number hit. */
s32 func_005E63E0(f32 r, f32 *from, f32 *to, f32 *n, f32 *push, f32 *q, u32 want, u32 skip,
                  s32 open);
#ifdef NON_MATCHING   /* first draft */
s32 func_005E63E0(f32 r, f32 *from, f32 *to, f32 *n, f32 *push, f32 *q, u32 want, u32 skip,
                  s32 open) {
    CollSweep sw;
    CollVec p;
    CollTri *recent[20];
    CollTri **list;
    CollTri *t;
    f32 *off;
    s32 nrecent;
    s32 nhit;
    s32 found;
    s32 cells;
    s32 quad;
    s32 half;
    s32 i;
    s32 j;
    s32 k;
    s32 m;

    if (func_005E51F0(to) == 0) {
        return 0;
    }
    nrecent = 0;
    nhit = 0;
    found = 0;
    push[0] = 0.0f;
    push[1] = 0.0f;
    push[2] = 0.0f;
    D_0023BECC = 0;
    D_0023BEC8 = 0;
    *(CollVec *)sw.pos = *(CollVec *)to;
    *(CollVec *)sw.from = *(CollVec *)from;
    sw.r = r;
    sw.len = func_001916E0(sw.pos, sw.from);
    if (D_003938A8.g[0].unk4 >= D_003938A8.g[0].unk0) {
        cells = (s32)(2.0f * r / (f32)D_003938A8.g[0].unk0) + 2;
    } else {
        cells = (s32)(2.0f * r / (f32)D_003938A8.g[0].unk4) + 2;
    }
    if (cells == 2) {
        quad = func_005E5040(to);
    }
    off = D_006DB1C0[quad];
    half = cells / 2;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            p = *(CollVec *)to;
            if (cells == 2) {
                p.v[0] += (f32)D_003938A8.g[0].unk0 * off[0];
                p.v[2] += (f32)D_003938A8.g[0].unk4 * off[1];
            } else {
                p.v[0] -= (f32)half * (f32)D_003938A8.g[0].unk0;
                p.v[2] -= (f32)half * (f32)D_003938A8.g[0].unk4;
            }
            p.v[0] += (f32)(D_003938A8.g[0].unk0 * i);
            p.v[2] += (f32)(D_003938A8.g[0].unk4 * j);
            if (func_005E51F0(p.v) != 1) {
                continue;
            }
            list = func_005E5490(p.v);
            for (k = 0; (t = list[k]) != (CollTri *)-1; k++) {
                if (!func_005E6050(t, want, skip, open)) {
                    continue;
                }
                if (nrecent != 0) {
                    found = 0;
                    for (m = 0; m < nrecent; m++) {
                        if (t == recent[m]) {
                            found = 1;
                        }
                    }
                    if (found) {
                        continue;
                    }
                }
                if (nhit != 0) {
                    found = 0;
                    for (m = 0; m < nhit; m++) {
                        if (t == D_0070F120[m]) {
                            found = 1;
                        }
                    }
                }
                if (found) {
                    continue;
                }
                if (func_005E6A90(&sw, t, n, q)) {
                    D_0070F120[nhit++] = t;
                }
                recent[nrecent++] = t;
                if (nrecent >= 20) {
                    nrecent = 0;
                }
            }
        }
    }
    for (k = 0, t = D_0070E750; k < D_0023BEC4; k++, t++) {
        if (!func_005E6050(t, want, skip, 0)) {
            continue;
        }
        if (nrecent != 0) {
            found = 0;
            for (m = 0; m < nrecent; m++) {
                if (t == recent[m]) {
                    found = 1;
                }
            }
            if (found) {
                continue;
            }
        }
        if (nhit != 0) {
            found = 0;
            for (m = 0; m < nhit; m++) {
                if (t == D_0070F120[m]) {
                    found = 1;
                }
            }
        }
        if (found) {
            continue;
        }
        if (func_005E6A90(&sw, t, n, q)) {
            D_0070F120[nhit++] = t;
        }
        recent[nrecent++] = t;
        if (nrecent >= 20) {
            nrecent = 0;
        }
    }
    func_005E7280(&sw, push);
    return nhit;
}
#endif

void func_00340410(f32 *out, const f32 *a, const f32 *b);
void func_003404F0(f32 *out, const f32 *a, const f32 *b);
void func_003408A0(f32 *out, const f32 *v);
f32 func_00191A70(f32 x);

/* Where sweep s crosses the plane through p with normal n (out): 1 when it starts behind it and
   ends in front, 2 the other way round; 0 when it doesn't cross (or stays on it). */
s32 func_005E55D0(CollSweep *s, const f32 *n, const f32 *p, f32 *out);
#ifdef NON_MATCHING   /* first draft: registers and the stack */
s32 func_005E55D0(CollSweep *s, const f32 *n, const f32 *p, f32 *out) {
    f32 w[4];
    f32 fwd[3];
    f32 back[3];
    f32 da;
    f32 db;
    f32 k;
    s32 side;

    func_00340410(back, s->from, s->pos);
    func_00340410(fwd, s->pos, s->from);
    side = 0;
    func_00340410(w, s->pos, p);
    if (!((da = func_001917C0(w, n)) < 0.0f)) {
        side |= 1;
    }
    func_00340410(w, s->from, p);
    if (!((db = func_001917C0(w, n)) < 0.0f)) {
        side |= 2;
    }
    switch (side) {
    case 0:
    case 3:
        side = 0;
        break;
    case 1:
        k = func_001917C0(fwd, n);
        if (k == 0.0f || !(k != 0.0f)) {
            func_003405D0(da, w, n);
        } else {
            func_003405D0(da / k, w, fwd);
        }
        func_00340410(out, s->pos, w);
        break;
    case 2:
        k = func_001917C0(back, n);
        if (k == 0.0f || !(k != 0.0f)) {
            func_003405D0(db, w, n);
        } else {
            func_003405D0(db / k, w, back);
            func_00340410(out, s->from, w);
        }
        break;
    default:
        return side;
    }
    return side;
}
#endif
void func_00340630(const f32 *a, const f32 *b, const f32 *p, f32 *out);

/* Whether p (on its plane) is inside triangle v (normal n). */
s32 func_005E57F0(const f32 *p, const f32 *v, const f32 *n) {
    f32 c[4];

    func_00340630(v, v + 3, p, c);
    if (func_001917C0(c, n) < 0.0f) {
        return 0;
    }
    func_00340630(v + 3, v + 6, p, c);
    if (func_001917C0(c, n) < 0.0f) {
        return 0;
    }
    func_00340630(v + 6, v, p, c);
    return (func_001917C0(c, n) < 0.0f) ^ 1;
}

/* The walls the sphere touched this frame (up to 6). */
extern s16 D_0070EE50[];        /* 1: touched at an edge or corner */
extern s16 D_0070EE60[];        /* the side (1: behind the plane) and 2: past it */
extern f32 D_0070EE70[][3];     /* the point on the plane (edge touches) */
extern f32 D_0070EED0[][3];     /* the point touched */
extern f32 D_0070EF30[][3];     /* where the sphere was */
extern CollTri D_0070EF90[];    /* the walls */

/* Whether sweep s touches triangle t (from its front): noted in D_0070EE50 on (the plane at the
   sphere, or where the move crosses it; then the nearest point of an edge when that isn't in
   the triangle). hit and n get the point and the wall's normal when it is far off (10000+). */
s32 func_005E6A90(CollSweep *s, CollTri *t, f32 *hit, f32 *n);
#ifdef NON_MATCHING   /* first draft */
s32 func_005E6A90(CollSweep *s, CollTri *t, f32 *hit, f32 *n) {
    f32 e[4][3];
    Vec3f near;
    Vec3f pc;
    Vec3f best;
    f32 v[3];
    f32 w[3];
    Vec3f c;
    f32 *a;
    f32 most;
    f32 d;
    s8 behind;
    s8 how;
    s32 k;
    s32 i;
    s32 side;

    func_00340410(w, s->from, t->v[0]);
    behind = (func_001917C0(w, t->n) > 0.0f) ^ 1;
    func_00340410(w, s->pos, t->v[0]);
    d = func_001917C0(w, t->n);
    how = (d <= 0.0f) ? 1 : 0;
    *(Vec3f *)v = *(Vec3f *)s->pos;
    if (func_00191A70(d) < s->r) {
        func_003405D0(d, &c.x, t->n);
        func_00340410(&c.x, s->pos, &c.x);
    } else {
        if (!func_005E55D0(s, t->n, t->v[0], &c.x)) {
            return 0;
        }
        how |= 2;
    }
    if (behind & 1) {
        return 0;
    }
    if (func_005E57F0(&c.x, t->v[0], t->n)) {
        if (D_0023BECC >= 6) {
            return 0;
        }
        D_0070EF90[D_0023BECC] = *t;
        func_003408A0(D_0070EF90[D_0023BECC].n, D_0070EF90[D_0023BECC].n);
        k = D_0023BECC;
        *(Vec3f *)D_0070EF30[k] = *(Vec3f *)v;
        D_0070EE60[k] = how;
        *(Vec3f *)D_0070EED0[k] = c;
        D_0070EE50[k] = 0;
        if (10000.0f < func_001916E0(s->pos, &c.x)) {
            *(Vec3f *)hit = c;
            *(Vec3f *)n = *(Vec3f *)t->n;
        }
        D_0023BECC++;
        return 1;
    }
    most = 8000.0f;
    D_0070EE50[D_0023BECC] = 1;
    *(Vec3f *)e[0] = *(Vec3f *)t->v[0];
    *(Vec3f *)e[1] = *(Vec3f *)t->v[1];
    *(Vec3f *)e[2] = *(Vec3f *)t->v[2];
    *(Vec3f *)e[3] = *(Vec3f *)t->v[0];
    pc = c;
    for (i = 0, a = e[0]; i < 3; i++, a += 3) {
        f32 *p = (how & 2) ? &pc.x : s->pos;

        side = 0;
        func_00340410(w, p, a);
        func_00340410(v, a + 3, a);
        func_00191750(v);
        if (func_001917C0(w, v) < 0.0f) {
            side |= 1;
        }
        func_00340410(w, p, a + 3);
        d = func_001917C0(w, v);
        if (!(d <= 0.0f)) {
            side |= 2;
        }
        switch (side) {
        case 0:
            func_003405D0(d, &c.x, v);
            func_003404F0(&c.x, &c.x, a + 3);
            break;
        case 1:
            *(Vec3f *)&c = *(Vec3f *)a;
            break;
        case 2:
            *(Vec3f *)&c = *(Vec3f *)(a + 3);
            break;
        case 3:
            return 0;
        }
        d = func_001916E0(p, &c.x);
        if (d < most) {
            most = d;
            best = c;
        }
    }
    if (!(most < s->r)) {
        return 0;
    }
    if (D_0023BECC >= 6) {
        return 0;
    }
    D_0070EF90[D_0023BECC] = *t;
    func_003408A0(D_0070EF90[D_0023BECC].n, D_0070EF90[D_0023BECC].n);
    k = D_0023BECC;
    *(Vec3f *)D_0070EF30[k] = *(Vec3f *)s->pos;
    D_0070EE60[k] = how;
    *(Vec3f *)D_0070EED0[k] = best;
    near = pc;
    *(Vec3f *)D_0070EE70[k] = near;
    if (!(10000.0f <= most)) {
        *(Vec3f *)hit = near;
        *(Vec3f *)n = *(Vec3f *)t->n;
    }
    D_0023BECC++;
    return 1;
}
#endif

extern s8 D_0023BED0[];         /* the walls that pushed this time */
extern f32 D_00239E70;          /* the longest push, as a share of the move */

/* The push off wall touch i for sweep s, into out (1; 0 when there is none): a face's out
   along its normal from the touch, an edge's away from the edge point; either as far as the
   sphere's centre then was from where it ends up. */
s8 func_005E8180(CollSweep *s, f32 *out, s8 k);
#ifdef NON_MATCHING   /* registers: the original keeps i * 12 (in i's s0) for the edge path's tables */
s8 func_005E8180(CollSweep *s, f32 *out, s8 k) {
    f32 n[3];
    f32 b[3];
    f32 a[3];
    f32 c[3];
    f32 l;
    f32 *p;
    s32 i = k;

    if (D_0070EE50[i] != 1) {
        n[0] = D_0070EF90[i].n[0];
        n[1] = D_0070EF90[i].n[1];
        n[2] = D_0070EF90[i].n[2];
        func_003405D0(s->r, b, n);
        func_003404F0(a, D_0070EED0[i], b);
        func_00340410(b, a, D_0070EF30[i]);
        l = func_00191690(b);
        func_00191750(b);
        func_003405D0(l, a, b);
    } else {
        switch (D_0070EE60[i]) {
        case 0:
            p = D_0070EED0[i];
            func_00340410(a, D_0070EF30[i], p);
            break;
        case 1:
        default:
            p = D_0070EED0[i];
            func_00340410(a, D_0070EE70[i], p);
            break;
        }
        func_00191750(a);
        func_003405D0(s->r, b, a);
        func_003404F0(a, p, b);
        c[0] = D_0070EF30[i][0];
        c[1] = D_0070EF30[i][1];
        c[2] = D_0070EF30[i][2];
        func_00340410(b, a, c);
        l = func_00191690(b);
        if (l == 0.0f) {
            return 0;
        }
        func_00191750(b);
        func_003405D0(l, a, b);
    }
    out[0] = a[0];
    out[1] = a[1];
    out[2] = a[2];
    return 1;
}
#endif
void func_005E8410(const f32 *v, f32 *acc);

/* Turns this frame's wall touches (D_0070EE50 on) into one push for sweep s, added to push
   (no longer than D_00239E70 of the move): faces each push; edges only when no face already
   covers them (the same plane, a convex corner, or the same point). Returns how many touched. */
s16 func_005E7280(CollSweep *s, f32 *push);
#ifdef NON_MATCHING   /* first draft */
/* Whether wall b's vertex that isn't shared with wall a (when they share an edge and neither
   is level) lies on or behind face f: the corner between them is convex. */
static s32 corner_hidden(CollTri *a, CollTri *b, CollTri *f) {
    f32 p[3];
    f32 w[4];
    s8 sv[4];
    s8 n;
    s8 i;
    s8 j;
    s32 other;

    if (a->n[1] == 1.0f || a->n[1] == -1.0f || b->n[1] == 1.0f || b->n[1] == -1.0f) {
        return 0;
    }
    n = 0;
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (a->v[i][0] == b->v[j][0] && a->v[i][1] == b->v[j][1] && a->v[i][2] == b->v[j][2]) {
                sv[n++] = j;
                break;
            }
        }
    }
    if (n < 2) {
        return 0;
    }
    other = 0xFF;
    for (i = 0; i < 3; i++) {
        if (!(((1 << sv[0]) | (1 << sv[1])) & 0xFF & (1 << i))) {
            other = i & 0xFF;
            break;
        }
    }
    if (other == 0xFF) {
        return 0;
    }
    p[0] = b->v[other][0];
    p[1] = b->v[other][1];
    p[2] = b->v[other][2];
    func_00340410(w, p, f->v[0]);
    return !(func_001917C0(w, f->n) > 0.0f);
}

s16 func_005E7280(CollSweep *s, f32 *push) {
    f32 v[3];
    f32 out[3];
    f32 acc[6];
    s8 face[8];
    s8 edge[8];
    s8 res[8];
    s8 kind[8];
    s8 nface;
    s8 nedge;
    s8 nres;
    s32 i;
    s32 k;
    s32 a;
    s32 found;
    f32 len;
    f32 most;
    CollTri *tf;
    CollTri *te;

    switch (D_0023BECC) {
    case 0:
        break;
    case 1:
        if (func_005E8180(s, out, 0)) {
            v[0] = out[0];
            v[1] = out[1];
            v[2] = out[2];
            len = func_00191690(v);
            if (s->len != 0.0f) {
                most = D_00239E70 * s->len;
                if (!(len <= most)) {
                    len = most;
                }
            }
            func_00191750(v);
            func_003405D0(len, out, v);
            push[0] += out[0];
            push[1] += out[1];
            push[2] += out[2];
            D_0023BED0[D_0023BEC8] = 0;
            D_0023BEC8++;
        }
        break;
    default:
        nres = 0;
        nface = 0;
        nedge = 0;
        for (i = 0; i < D_0023BECC; i++) {
            if (D_0070EE50[i] == 0) {
                face[nface++] = i;
            } else {
                edge[nedge++] = i;
            }
        }
        if (nface != 0 && nedge != 0) {
            for (i = 0; i < nface; i++) {
                for (k = 0; k < nedge; k++) {
                    for (a = 0; a < nres; a++) {
                        if (edge[k] == res[a]) {
                            break;
                        }
                    }
                    te = &D_0070EF90[edge[k]];
                    tf = &D_0070EF90[face[i]];
                    if (tf->n[0] == te->n[0] && tf->n[1] == te->n[1] && tf->n[2] == te->n[2]) {
                        kind[nres] = 1;
                        res[nres] = edge[k];
                        nres++;
                    } else if (corner_hidden(tf, te, tf)) {
                        kind[nres] = 2;
                        res[nres] = edge[k];
                        nres++;
                    }
                }
            }
        }
        if (nedge >= 2) {
            for (i = 0; i < nedge - 1; i++) {
                found = 0;
                for (a = 0; a < nres; a++) {
                    if (edge[i] == res[a]) {
                        found = 1;
                        break;
                    }
                }
                if ((s8)found == 1) {
                    continue;
                }
                for (k = 1; k < nedge; k++) {
                    found = 0;
                    for (a = 0; a < nres; a++) {
                        if (edge[k] == res[a]) {
                            found = 1;
                            break;
                        }
                    }
                    if ((s8)found == 1 || edge[i] == edge[k]) {
                        continue;
                    }
                    if (D_0070EED0[edge[i]][0] == D_0070EED0[edge[k]][0] &&
                        D_0070EED0[edge[i]][1] == D_0070EED0[edge[k]][1] &&
                        D_0070EED0[edge[i]][2] == D_0070EED0[edge[k]][2]) {
                        kind[nres] = 3;
                        res[nres] = edge[k];
                        nres++;
                    }
                }
            }
        }
        if (nres > 0 && nres < nedge) {
            for (i = 0; i < nres; i++) {
                if (kind[i] != 1) {
                    continue;
                }
                for (k = 0; k < nedge; k++) {
                    if (edge[k] == res[i]) {
                        continue;
                    }
                    if (corner_hidden(&D_0070EF90[res[i]], &D_0070EF90[edge[k]], &D_0070EF90[face[i]])) {
                        kind[nres] = 2;
                        res[nres] = edge[k];
                        nres++;
                    }
                }
            }
        }
        for (i = 0; i < 6; i++) {
            acc[i] = 0.0f;
        }
        for (i = 0; i < nface; i++) {
            if (func_005E8180(s, out, face[i])) {
                func_005E8410(out, acc);
                D_0023BED0[D_0023BEC8] = face[i];
                D_0023BEC8++;
            }
        }
        for (i = 0; i < nedge; i++) {
            found = 0;
            for (a = 0; a < nres; a++) {
                if (res[a] == edge[i]) {
                    found = 1;
                    break;
                }
            }
            if (found == 0 && func_005E8180(s, out, edge[i])) {
                func_005E8410(out, acc);
                D_0023BED0[D_0023BEC8] = edge[i];
                D_0023BEC8++;
            }
        }
        for (i = 0; i < 3; i++) {
            if (acc[i] != 0.0f && acc[3 + i] != 0.0f) {
                out[i] = (acc[i] + acc[3 + i]) / 2.0f;
            } else if (acc[3 + i] == 0.0f) {
                out[i] = acc[i];
            } else {
                out[i] = acc[3 + i];
            }
        }
        acc[0] = out[0];
        acc[1] = out[1];
        acc[2] = out[2];
        len = func_00191690(acc);
        if (s->len != 0.0f) {
            most = D_00239E70 * s->len;
            if (!(len <= most)) {
                len = most;
            }
        }
        func_00191750(acc);
        func_003405D0(len, out, acc);
        push[0] += out[0];
        push[1] += out[1];
        push[2] += out[2];
        break;
    }
    return D_0023BECC;
}
#endif

/* The room objects' grid (D_00395EA0: cells of 50, from x, z) and the objects (0x3C each). */
typedef struct {
    f32 x;              /* 0x0 */
    f32 y;              /* 0x4 */
    f32 z;              /* 0x8 */
    s16 w;              /* 0xC: cells along x */
    s16 d;              /* 0xE: along z */
} ObjGrid;

typedef struct {
    u8 unk0[0x18];
    s8 x;               /* 0x18: its cell */
    s8 z;               /* 0x19 */
    u8 unk1A[5];
    s8 w;               /* 0x1F: its size in cells */
    s8 floor;           /* 0x20 */
    s8 d;               /* 0x21 */
    u8 unk22[0x3C - 0x22];
} GridObj;

/* A cell of the objects' grid. */
typedef struct {
    s8 x;
    u8 unk1[3];
    s8 z;
    u8 unk5[3];
} GridCell;

extern ObjGrid D_00395EA0;
extern GridObj D_00395720[];
extern s8 D_0023BF4C;           /* objects placed */
extern s8 D_0023BF48;           /* (the objects' grid has floors) */
s8 func_0067D8B0(s8 x, s8 z);
s8 func_0067E490(GridCell *c, const f32 *p);
s8 func_0067E570(GridCell *c, const f32 *p);
s8 func_0067E790(GridCell *c, const f32 *p, s8 floor);
void func_0067DA50(s8 *floor, const f32 *p);

/* The probes of func_0054BF70 round the next cell boundary: blocked (2 and up) or not. */
#define GRID_BLOCKED(c, q, floor) (func_0067E790(c, q, floor) >= 2)

/* A move from from to to against the room objects' grid (doors, boxes): out gets where it ends,
   stopped at the objects' sides (radius 49.5) or, on a grid with floors above 50, slid along
   them cell by cell (radius 20). */
void func_0054BF70(f32 *to, f32 *from, f32 *out);
#ifdef NON_MATCHING   /* first draft */
void func_0054BF70(f32 *to, f32 *from, f32 *out) {
    f32 lim[4];
    Vec3f d0;
    f32 e[3];
    f32 q[3];
    f32 p[3];
    f32 dir[3];
    GridCell cell;
    s8 floor;
    s8 lift;
    s8 tries;
    s8 i;
    s8 side;
    s8 k;
    GridObj *o;
    f32 r;
    f32 best;
    f32 dd;
    f32 sx;
    f32 sz;
    f32 bx;
    f32 bz;
    f32 tx;
    f32 tz;
    f32 dz0;
    s32 n;
    s32 zfirst;
    s32 hit;

    tries = 0;
    lift = 0;
    r = 49.5f;
    floor = -1;
    if (D_0023BF4C == 0) {
        *(Vec3f *)out = *(Vec3f *)to;
        return;
    }
    if (D_0023BF48 != 0 && !(from[1] - D_00395EA0.y <= 50.0f) && func_0067E490(&cell, from) == 1) {
        lift = 1;
        r = 20.0f;
    }
    func_00340410(dir, to, from);
    dir[1] = 0.0f;
    if (func_00191690(dir) == 0.0f) {
        *(Vec3f *)out = *(Vec3f *)to;
        return;
    }
    func_00191750(dir);
    e[0] = from[0] + dir[0] * r;
    e[1] = from[1];
    e[2] = from[2] + dir[2] * r;
    if (func_0067E570(&cell, e) >= 2) {
        best = 10000;
        *(Vec3f *)out = *(Vec3f *)to;
        side = -1;
        k = func_0067D8B0(cell.x, cell.z);
        if (k < 0) {
            return;
        }
        o = &D_00395720[k];
        lim[0] = D_00395EA0.z + 50.0f * o->z - (1.0f + r);
        lim[1] = (1.0f + r) + (D_00395EA0.z + 50.0f * (o->z + o->d));
        lim[2] = D_00395EA0.x + 50.0f * o->x - (1.0f + r);
        lim[3] = (1.0f + r) + (D_00395EA0.x + 50.0f * (o->x + o->w));
        for (i = 0; i < 4; i++) {
            if (i < 2) {
                dd = func_00191A70(lim[i] - from[2]);
            } else {
                dd = func_00191A70(lim[i] - from[0]);
            }
            if (!(best <= dd)) {
                best = dd;
                side = i;
            }
        }
        if (side >= 0 && side < 2) {
            out[2] = lim[side];
        } else if (side >= 2 && side < 4) {
            out[0] = lim[side];
        }
        return;
    }
    if (lift == 0) {
        *(Vec3f *)out = *(Vec3f *)to;
        return;
    }
    if (to[0] + r < D_00395EA0.x || !(to[0] - r < D_00395EA0.x + 50.0f * D_00395EA0.w) ||
        to[2] + r < D_00395EA0.z || !(to[2] - r < D_00395EA0.z + 50.0f * D_00395EA0.d)) {
        if (lift != 0 && func_0067E570(&cell, to) == 3) {
            *(Vec3f *)out = *(Vec3f *)from;
        } else {
            *(Vec3f *)out = *(Vec3f *)to;
        }
        return;
    }
    *(Vec3f *)p = *(Vec3f *)from;
    d0 = *(Vec3f *)dir;
    sx = 0.0f;
    sz = 0.0f;
    if (!(dir[0] <= 0.0f)) {
        sx = r;
    }
    if (dir[0] < 0.0f) {
        sx = -r;
    }
    if (!(dir[2] <= 0.0f)) {
        sz = r;
    }
    if (dir[2] < 0.0f) {
        sz = -r;
    }
    q[2] = 0.0f;
    q[0] = 0.0f;
    e[2] = 0.0f;
    e[0] = 0.0f;
    e[1] = q[1] = from[1];
    func_0067DA50(&floor, from);
    dz0 = d0.z;
    do {
        bz = 0.0f;
        tz = 1000.0f;
        bx = 0.0f;
        tx = 1000.0f;
        if (!(dir[0] <= 0.0f)) {
            n = (s32)((p[0] + r - D_00395EA0.x) / 50.0f) + 1;
            if (p[0] + r < D_00395EA0.x) {
                n--;
            }
            bx = D_00395EA0.x + 50.0f * n;
            tx = (bx - (p[0] + r)) / dir[0];
        } else if (dir[0] < 0.0f) {
            n = (s32)((p[0] - r - D_00395EA0.x) / 50.0f);
            if (p[0] - r < D_00395EA0.x) {
                n--;
            }
            bx = D_00395EA0.x + 50.0f * n;
            tx = (bx - (p[0] - r)) / dir[0];
        }
        if (!(dir[2] <= 0.0f)) {
            n = (s32)((p[2] + r - D_00395EA0.z) / 50.0f) + 1;
            if (p[2] + r < D_00395EA0.z) {
                n--;
            }
            bz = D_00395EA0.z + 50.0f * n;
            tz = (bz - (p[2] + r)) / dir[2];
        } else if (dir[2] < 0.0f) {
            n = (s32)((p[2] - r - D_00395EA0.z) / 50.0f);
            if (p[2] - r < D_00395EA0.z) {
                n--;
            }
            bz = D_00395EA0.z + 50.0f * n;
            tz = (bz - (p[2] - r)) / dir[2];
        }
        if (!(tx < 0.0f) && !(tz < 0.0f)) {
            zfirst = 1;
            if (tx < tz) {
                zfirst = 0;
            }
        } else if (tx < 0.0f) {
            tx = 1000.0f;
            zfirst = 1;
        } else if (tz < 0.0f) {
            zfirst = 0;
            tz = 1000.0f;
        }
        if (tx == 1000.0f && tz == 1000.0f) {
            break;
        }
        if (!(zfirst & 1)) {
            /* the next x boundary first */
            e[0] = bx - sx;
            e[2] = p[2] + dir[2] * ((e[0] - p[0]) / dir[0]);
            if ((!(dir[0] <= 0.0f) && !(e[0] < to[0])) || (dir[0] < 0.0f && e[0] <= to[0])) {
                *(Vec3f *)out = *(Vec3f *)to;
                return;
            }
            q[0] = bx + d0.x;
            q[2] = sz + (e[2] + dz0);
            hit = GRID_BLOCKED(&cell, q, floor);
            if (!hit) {
                q[2] = e[2] + dz0;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (!hit) {
                q[2] = e[2] + dz0 - sz;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (!hit) {
                q[2] = sz + (e[2] + dz0);
                q[0] -= sx;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (!hit) {
                q[0] -= sx;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (hit) {
                p[0] = e[0] - d0.x;
                q[0] = bx - d0.x;
                q[2] = to[2] + sz;
                hit = GRID_BLOCKED(&cell, q, floor);
                if (!hit) {
                    q[0] -= sx;
                    hit = GRID_BLOCKED(&cell, q, floor);
                }
                if (!hit) {
                    q[0] -= sx;
                    hit = GRID_BLOCKED(&cell, q, floor);
                }
                if (!hit) {
                    p[2] = to[2];
                } else if ((!(dir[2] <= 0.0f) && bz < to[2]) || (dir[2] < 0.0f && !(bz <= to[2]))) {
                    p[2] = bz;
                } else {
                    p[2] = e[2];
                }
                break;
            }
        } else {
            /* the next z boundary first */
            e[2] = bz - sz;
            e[0] = p[0] + dir[0] * ((e[2] - p[2]) / dir[2]);
            if ((!(dir[2] <= 0.0f) && !(e[2] < to[2])) || (dir[2] < 0.0f && e[2] <= to[2])) {
                *(Vec3f *)out = *(Vec3f *)to;
                return;
            }
            q[2] = bz + dz0;
            q[0] = sx + (e[0] + d0.x);
            hit = GRID_BLOCKED(&cell, q, floor);
            if (!hit) {
                q[0] = e[0] + d0.x;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (!hit) {
                q[0] = e[0] + d0.x - sx;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (!hit) {
                q[0] = sx + (e[0] + d0.x);
                q[2] -= sz;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (!hit) {
                q[2] -= sz;
                hit = GRID_BLOCKED(&cell, q, floor);
            }
            if (hit) {
                p[2] = e[2] - dz0;
                q[2] = bz - dz0;
                q[0] = to[0] + sx;
                hit = GRID_BLOCKED(&cell, q, floor);
                if (!hit) {
                    q[2] -= sz;
                    hit = GRID_BLOCKED(&cell, q, floor);
                }
                if (!hit) {
                    q[2] -= sz;
                    hit = GRID_BLOCKED(&cell, q, floor);
                }
                if (!hit) {
                    p[0] = to[0];
                } else if ((!(dir[0] <= 0.0f) && bx < to[0]) || (dir[0] < 0.0f && !(bx <= to[0]))) {
                    p[0] = bx;
                } else {
                    p[0] = e[0];
                }
                break;
            }
        }
        /* nothing in the way: to, unless that goes back past from */
        *(Vec3f *)p = *(Vec3f *)to;
        if ((!(dir[0] <= 0.0f) && p[0] <= from[0]) || (dir[0] < 0.0f && !(p[0] < from[0]))) {
            *(Vec3f *)p = *(Vec3f *)from;
            break;
        }
        if ((!(dir[2] <= 0.0f) && p[2] <= from[2]) || (dir[2] < 0.0f && !(p[2] < from[2]))) {
            *(Vec3f *)p = *(Vec3f *)from;
            break;
        }
    } while (++tries < 4);
    *(Vec3f *)out = *(Vec3f *)p;
}
#endif
