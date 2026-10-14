#include "common.h"

/* Model files: a header {flags, count, ?} and count chunks {u8 type, ..., count at 4, size at 8,
   data at 0xC}. Type 0 holds the bone tree's roots, the others bones ({?, ?, child, sibling, ...})
   and so on; type 9 and 10 are tables of sub-chunks. MW only knows a callee's registers when it
   is defined before the caller, so the functions stay in address order. */

typedef struct {
    u8 type;
    u8 unk1[3];
    u32 count;          /* 0x04 */
    u32 size;           /* 0x08: to the next chunk */
    u8 data[0];         /* 0x0C */
} Chunk;

typedef struct {
    u32 flags;          /* 0x00: bit 31: the last chunk isn't counted */
    u32 count;          /* 0x04 */
    u32 unk8;
    Chunk first[0];     /* 0x0C */
} ChunkFile;

#define NEXT_CHUNK(c) ((Chunk *)((u8 *)(c) + (c)->size))

Chunk *func_001A1310(void *p);
static ChunkFile *func_001A21D0(void *p);
Chunk *func_001A21E0(ChunkFile *f, s32 type);
Chunk *func_001A2240(ChunkFile *f, u32 n);
static void *func_001A22C0(ChunkFile *f, u32 n);
static s32 func_001A2140(ChunkFile *f, s32 bone);
static void func_001A2160(ChunkFile *f, s32 bone);
extern s32 D_0023BAC4;

s32 func_001A2030(void *p) {
    return func_001A1310(p)->count;
}

/* Chunk count. */
s32 func_001A2050(void *p) {
    ChunkFile *f = func_001A21D0(p);
    s32 n = f->count;

    if (f->flags & 0x80000000) {
        n--;
    }
    return n;
}

/* Root count. */
s32 func_001A2090(ChunkFile *f) {
    Chunk *c = func_001A21E0(f, 0);

    if (c != NULL) {
        return c->count;
    }
    return 0;
}

/* Root n's bone. */
s32 func_001A20C0(ChunkFile *f, u32 n) {
    Chunk *c = func_001A21E0(f, 0);

    if (c != NULL && n < c->count) {
        return ((s32 *)c->data)[n];
    }
    return -1;
}

/* The size of root n's tree. */
static s32 func_001A2110(ChunkFile *f, u32 n) {
    return func_001A2140(f, func_001A20C0(f, n));
}

static s32 func_001A2140(ChunkFile *f, s32 bone) {
    D_0023BAC4 = 0;
    func_001A2160(f, bone);
    return D_0023BAC4;
}

static void func_001A2160(ChunkFile *f, s32 bone) {
    s32 *b;

    D_0023BAC4++;
    b = func_001A22C0(f, bone);
    if (b[2] != -1) {
        func_001A2160(f, b[2]);
    }
    if (b[3] != -1) {
        func_001A2160(f, b[3]);
    }
}

static ChunkFile *func_001A21D0(void *p) {
    return p;
}

#ifdef NON_MATCHING   /* registers */
/* The first chunk of this type. */
Chunk *func_001A21E0(ChunkFile *f, s32 type) {
    u32 n = func_001A21D0(f)->count;
    u32 i;

    f = (ChunkFile *)f->first;
    for (i = 0; i < n; i++) {
        if (((Chunk *)f)->type == type) {
            return (Chunk *)f;
        }
        f = (ChunkFile *)NEXT_CHUNK((Chunk *)f);
    }
    return NULL;
}
#endif

#ifdef NON_MATCHING   /* registers */
/* The n-th chunk that isn't a type 0 one. */
Chunk *func_001A2240(ChunkFile *f, u32 n) {
    u32 k = 0;
    u32 count = func_001A21D0(f)->count;
    u32 i;

    if (n < count) {
        f = (ChunkFile *)f->first;
        for (i = 0; i < count; i++) {
            if (((Chunk *)f)->type != 0) {
                if (k == n) {
                    return (Chunk *)f;
                }
                k++;
            }
            f = (ChunkFile *)NEXT_CHUNK((Chunk *)f);
        }
    }
    return NULL;
}
#endif

/* Bone n's data. */
static void *func_001A22C0(ChunkFile *f, u32 n) {
    Chunk *c = func_001A2240(f, n);

    if (c != NULL) {
        return c->data;
    }
    return NULL;
}

/* The size of root's skeleton (0x10 + 0x40 per bone). */
static s32 func_001A22F0(ChunkFile *f, s32 root) {
    if (root >= func_001A2090(f)) {
        return 0;
    }
    return (func_001A2110(f, root) << 6) + 0x10;
}

typedef struct {
    s16 id;             /* 0x00 */
    s16 unk2;           /* 0x02 */
    s16 unk4;
    s16 child;          /* 0x06: -1: none */
    s16 sibling;        /* 0x08 */
    s16 unkA;           /* 0x0A */
    u8 unkC[4];
    f32 m[3][4];        /* 0x10 */
} SkelBone;

typedef struct {
    s16 unk0;
    s16 n;              /* 0x02 */
    s32 unk4;           /* 0x04: 0x10 */
    s32 unk8;
    s32 size;           /* 0x0C */
    SkelBone bones[0];  /* 0x10 */
} Skeleton;

typedef struct {
    f32 w[4];
} Qword;

s32 func_001A2350(Qword *hdr, Skeleton *out, ChunkFile *f, s32 root);
#ifdef NON_MATCHING   /* first draft */
/* Builds root's skeleton in out; hdr gets out's header with word 2 pointing at out. */
s32 func_001A2350(Qword *hdr, Skeleton *out, ChunkFile *f, s32 root) {
    SkelBone *b;
    s32 *src;
    s32 n;
    s32 first;
    s32 i;
    s32 k;

    if (root >= func_001A2090(f)) {
        return 0;
    }
    out->size = func_001A22F0(f, root);
    out->n = n = func_001A2110(f, root);
    b = out->bones;
    out->unk4 = 0x10;
    out->unk8 = 0;
    for (i = 0; i < n; i++) {
        for (k = 0; k < 4; k++) {
            b[i].m[0][k] = 0;
            b[i].m[1][k] = 0;
            b[i].m[2][k] = 0;
        }
    }
    first = func_001A20C0(f, root);
    for (i = 0; i < n; i++) {
        src = func_001A22C0(f, i + first);
        b->id = src[0];
        b->unk2 = src[0x11];
        b->child = src[2];
        b->sibling = src[3];
        if (b->child != -1) {
            b->child -= (s16)first;
        }
        if (b->sibling != -1) {
            b->sibling -= (s16)first;
        }
        b->unkA = src[0x10];
        for (k = 0; k < 4; k++) {
            b->m[0][k] = ((f32 *)src)[4 + k];
            b->m[1][k] = ((f32 *)src)[8 + k];
            b->m[2][k] = ((f32 *)src)[12 + k];
        }
        b++;
    }
    *hdr = *(Qword *)out;
    ((Skeleton **)hdr)[2] = out;
    return 1;
}
#endif

/* The second kind of chunk file (meshes): word types. Type 2 is the table of meshes (sub-files),
   5 the strips (count = the strip's length), 6 indices, 7 vertices (12 bytes), 8 normals (12),
   10 uvs (8), 11 colors (16), 12 weights, 0x50000 bone ids, 0xF0000 a material, 0x100000 bone
   palette. */

typedef struct {
    s32 type;
    u32 count;          /* 0x04 */
    u32 size;           /* 0x08 */
    u8 data[0];         /* 0x0C */
} WChunk;

typedef struct {
    s32 type;
    u32 count;          /* 0x04 */
    u32 unk8;
    WChunk first[0];    /* 0x0C */
} WChunkFile;

#define WNEXT(c) ((WChunk *)((u8 *)(c) + (c)->size))

WChunk *func_001A2760(void *p, u32 n);
WChunk *func_001A2B40(WChunkFile *f, s32 type, u32 n);
WChunk *func_001A2860(void *p, u32 n);
WChunk *func_001A2930(void *p, u32 n);
s32 func_001A2AF0(WChunkFile *f, s32 type);
s32 func_001A2CB0(WChunkFile *m, u32 n);
s32 func_001A2DD0(WChunkFile *m, s32 n, s32 k);
s32 func_001A2D30(WChunkFile *m);
s32 func_001A2C30(WChunkFile *m, u32 n);

WChunk *func_001A2760(void *p, u32 n);
WChunk *func_001A2B40(WChunkFile *f, s32 type, u32 n);

s32 func_001A2540(s16 *out, void *p, u32 n);
s32 func_001A2AF0(WChunkFile *f, s32 type);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Mesh n's bone palette: out[0] = count, then the ids. */
s32 func_001A2540(s16 *out, void *p, u32 n) {
    WChunk *c = func_001A2760(p, n);
    s32 *src;
    s32 i;

    if (c->type != 4) {
        return -1;
    }
    c = func_001A2B40((WChunkFile *)c, 0x100000, 0);
    if (c == NULL) {
        return -1;
    }
    out[0] = c->count;
    src = (s32 *)c->data;
    for (i = 0; i < (s32)c->count; i++) {
        out[1 + i] = *src++;
    }
    return 0;
}
#endif

/* Mesh n's material (0x48 bytes; version 0x1xxxx). */
s32 func_001A25E0(u32 *out, void *p, u32 n) {
    WChunk *c = func_001A2760(p, n);
    u32 *src;

    if (c->type != 4) {
        return -1;
    }
    c = func_001A2B40((WChunkFile *)c, 0xF0000, 0);
    if (c == NULL) {
        return -1;
    }
    src = (u32 *)c->data;
    out[0] = src[0];
    if ((out[0] & 0xFFFF0000) != 0x10000) {
        return -2;
    }
    out[0] = src[0];
    out[1] = src[1];
    out[2] = src[2];
    out[3] = src[3];
    out[4] = src[4];
    out[5] = src[5];
    out[6] = src[6];
    out[7] = src[7];
    out[8] = src[8];
    out[9] = src[9];
    out[10] = src[10];
    out[11] = src[11];
    out[12] = src[12];
    out[13] = src[13];
    out[14] = src[14];
    out[15] = src[15];
    out[16] = src[16];
    out[17] = src[17];
    return 0;
}

static WChunkFile *func_001A2AE0(void *p);
s32 func_001A2AF0(WChunkFile *f, s32 type);

/* Mesh count. */
s32 func_001A2700(void *p) {
    WChunkFile *f = func_001A2AE0(p);
    WChunk *c;

    if (func_001A2AF0(f, 2) != 0) {

        return 0;
    }
    c = func_001A2B40(f, 2, 0);
    if (c == NULL) {
        return 0;
    }
    return c->count;
}

#ifdef NON_MATCHING   /* first draft (registers) */
/* Mesh n. */
WChunk *func_001A2760(void *p, u32 n) {
    WChunkFile *f = func_001A2AE0(p);
    WChunk *c;

    if (func_001A2AF0(f, 2) != 0) {
        return NULL;
    }
    c = func_001A2B40(f, 2, 0);
    if (c == NULL) {
        return NULL;
    }
    if (c->count < n) {
        return NULL;
    }
    c = func_001A2B40((WChunkFile *)c, -1, n);
    if (c != NULL) {
        return c;
    }
    return NULL;
}
#endif

/* Table 9's count. */
s32 func_001A2800(void *p) {
    WChunkFile *f = func_001A2AE0(p);
    WChunk *c;

    if (func_001A2AF0(f, 9) != 0) {
        return 0;
    }
    c = func_001A2B40(f, 9, 0);
    if (c == NULL) {
        return 0;
    }
    return c->count;
}

WChunk *func_001A2860(void *p, u32 n);
#ifdef NON_MATCHING   /* first draft (registers) */
WChunk *func_001A2860(void *p, u32 n) {
    WChunkFile *f = func_001A2AE0(p);
    WChunk *c;

    if (func_001A2AF0(f, 9) != 0) {
        return NULL;
    }
    c = func_001A2B40(f, 9, 0);
    if (c == NULL) {
        return NULL;
    }
    if (c->count < n) {
        return NULL;
    }
    c = func_001A2B40((WChunkFile *)c, -1, n);
    if (c != NULL) {
        return c;
    }
    return NULL;
}
#endif

void *func_001A2900(void *p, u32 n) {
    WChunk *c = func_001A2860(p, n);

    if (c == NULL) {
        return NULL;
    }
    return c->data;
}

WChunk *func_001A2930(void *p, u32 n);
s32 func_001A2A50(void *p, u32 n, s32 *out);
#ifdef NON_MATCHING   /* first draft (registers) */
WChunk *func_001A2930(void *p, u32 n) {
    WChunkFile *f = func_001A2AE0(p);
    WChunk *c;

    if (func_001A2AF0(f, 10) != 0) {
        return NULL;
    }
    c = func_001A2B40(f, 10, 0);
    if (c == NULL) {
        return NULL;
    }
    if (c->count < n) {
        return NULL;
    }
    c = func_001A2B40((WChunkFile *)c, -1, n);
    if (c != NULL) {
        return c;
    }
    return NULL;
}
#endif

void *func_001A29D0(void *p, u32 n) {
    WChunk *c = func_001A2930(p, n);

    if (c == NULL) {
        return NULL;
    }
    return c->data;
}

/* Mesh n's bone id count. */
s32 func_001A2A00(void *p, u32 n) {
    WChunk *c = func_001A2760(p, n);

    if (c == NULL) {
        return 0;
    }
    c = func_001A2B40((WChunkFile *)c, 0x50000, 0);
    if (c == NULL) {
        return 0;
    }
    return c->count;
}

s32 func_001A2A50(void *p, u32 n, s32 *out);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Mesh n's bone ids to out. */
s32 func_001A2A50(void *p, u32 n, s32 *out) {
    WChunk *c = func_001A2760(p, n);
    s32 *src;
    u32 i;

    if (c == NULL) {
        return 0;
    }
    c = func_001A2B40((WChunkFile *)c, 0x50000, 0);
    if (c == NULL) {
        return 0;
    }
    src = (s32 *)c->data;
    for (i = 0; i < c->count; i++) {
        out[i] = *src++;
    }
    return 1;
}
#endif

static WChunkFile *func_001A2AE0(void *p) {
    return p;
}

#ifdef NON_MATCHING   /* first draft (registers) */
/* 0 when there's a chunk of this type, -1 when not. */
s32 func_001A2AF0(WChunkFile *f, s32 type) {
    u32 n = f->count;
    u32 i;

    f = (WChunkFile *)f->first;
    for (i = 0; i < n; i++) {
        if (f->type == type) {
            return 0;
        }
        f = (WChunkFile *)WNEXT((WChunk *)f);
    }
    return -1;
}
#endif

s32 func_001A2C30(WChunkFile *m, u32 n);
#ifdef NON_MATCHING   /* first draft (registers) */
/* The n-th chunk of this type (-1: of any). */
WChunk *func_001A2B40(WChunkFile *f, s32 type, u32 n) {
    s32 i;
    s32 count;

    if (type != -1) {
        count = f->count;
        f = (WChunkFile *)f->first;
        for (i = 0; i < count; i++) {
            if (f->type == type) {
                if (n == 0) {
                    return (WChunk *)f;
                }
                n--;
            }
            f = (WChunkFile *)WNEXT((WChunk *)f);
        }
        return NULL;
    }
    if (n < f->count) {
        f = (WChunkFile *)f->first;
        for (i = 0; i < (s32)n; i++) {
            f = (WChunkFile *)WNEXT((WChunk *)f);
        }
        return (WChunk *)f;
    }
    return NULL;
}
#endif

/* Strip count (-1: none). */
static s32 func_001A2BF0(WChunkFile *m) {
    WChunk *c = func_001A2B40(m, 5, 0);

    if (c == NULL) {
        return -1;
    }
    return c->count;
}

s32 func_001A2C30(WChunkFile *m, u32 n);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Strip n's type. */
s32 func_001A2C30(WChunkFile *m, u32 n) {
    WChunk *c = func_001A2B40(m, 5, 0);
    s32 i;

    if (c == NULL) {
        return -1;
    }
    if (n >= c->count) {
        return -1;
    }
    c = ((WChunkFile *)c)->first;
    for (i = 0; i < (s32)n; i++) {
        c = WNEXT(c);
    }
    return c->type;
}
#endif

s32 func_001A2CB0(WChunkFile *m, u32 n);
s32 func_001A2D30(WChunkFile *m);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Strip n's count. */
s32 func_001A2CB0(WChunkFile *m, u32 n) {
    WChunk *c = func_001A2B40(m, 5, 0);
    s32 i;

    if (c == NULL) {
        return -1;
    }
    if (n >= c->count) {
        return -1;
    }
    c = ((WChunkFile *)c)->first;
    for (i = 0; i < (s32)n; i++) {
        c = WNEXT(c);
    }
    return c->count;
}
#endif

s32 func_001A2D30(WChunkFile *m);
#ifdef NON_MATCHING   /* first draft (registers) */
/* All strips' counts. */
s32 func_001A2D30(WChunkFile *m) {
    s32 sum = 0;
    u32 n = func_001A2BF0(m);
    u32 i;

    if (n == (u32)-1) {
        return -1;
    }
    for (i = 0; i < n; i++) {
        sum += func_001A2CB0(m, i);
    }
    return sum;
}
#endif

/* Vertex count (-1: none). */
s32 func_001A2D90(WChunkFile *m) {
    WChunk *c = func_001A2B40(m, 0x70000, 0);

    if (c == NULL) {
        return -1;
    }
    return c->count;
}

s32 func_001A2DD0(WChunkFile *m, s32 n, s32 k);
s32 func_001A2F30(WChunkFile *m, s32 n, s32 k);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Strip n's k-th index. */
s32 func_001A2DD0(WChunkFile *m, s32 n, s32 k) {
    WChunk *c;
    s32 sum = 0;
    s32 i;

    if (func_001A2BF0(m) < n) {
        return -1;
    }
    c = func_001A2B40(m, 5, 0);
    if (c == NULL) {
        return -1;
    }
    c = ((WChunkFile *)c)->first;
    for (i = 0; i < n; i++) {
        sum += c->count;
        c = WNEXT(c);
    }
    sum += k;
    c = func_001A2B40(m, 0x60000, 0);
    if (c == NULL) {
        return -1;
    }
    return ((s32 *)c->data)[sum];
}
#endif

/* Strip n's k-th primitive's length (each is {flags | length, length words}). */
s32 func_001A2E80(WChunkFile *m, s32 n, s32 k);
#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
s32 func_001A2E80(WChunkFile *m, s32 n, s32 k) {
    WChunk *c;
    u32 *p;
    u32 len;
    s32 i;

    if (func_001A2BF0(m) < n) {
        return -1;
    }
    c = func_001A2B40(m, 5, 0);
    if (c == NULL) {
        return -1;
    }
    c = ((WChunkFile *)c)->first;
    for (i = 0; i < n; i++) {
        c = WNEXT(c);
    }
    p = (u32 *)c->data;
    for (i = 0; i <= k; i++) {
        len = *p & 0x7FFFFFFF;
        p += len + 1;
    }
    return len;
}
#endif

s32 func_001A2F30(WChunkFile *m, s32 n, s32 k);
s32 func_001A2FF0(WChunkFile *m, s32 n, s32 k, s32 j);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Strip n's k-th primitive's flag (bit 31). */
s32 func_001A2F30(WChunkFile *m, s32 n, s32 k) {
    WChunk *c;
    u32 *p;
    u32 flag;
    s32 i;

    if (func_001A2BF0(m) < n) {
        return -1;
    }
    c = func_001A2B40(m, 5, 0);
    if (c == NULL) {
        return -1;
    }
    c = ((WChunkFile *)c)->first;
    for (i = 0; i < n; i++) {
        c = WNEXT(c);
    }
    p = (u32 *)c->data;
    for (i = 0; i <= k; i++) {
        flag = *p & 0x80000000;
        p += (*p & 0x7FFFFFFF) + 1;
    }
    return flag != 0;
}
#endif

s32 func_001A2FF0(WChunkFile *m, s32 n, s32 k, s32 j);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Strip n's k-th primitive's j-th word. */
s32 func_001A2FF0(WChunkFile *m, s32 n, s32 k, s32 j) {
    WChunk *c;
    u32 *p;
    s32 i;

    if (func_001A2BF0(m) < n) {
        return -1;
    }
    c = func_001A2B40(m, 5, 0);
    if (c == NULL) {
        return -1;
    }
    c = ((WChunkFile *)c)->first;
    for (i = 0; i < n; i++) {
        c = WNEXT(c);
    }
    p = (u32 *)c->data;
    for (i = 0; i < k; i++) {
        p += (*p & 0x7FFFFFFF) + 1;
    }
    if (j >= (s32)(*p & 0x7FFFFFFF)) {
        return -1;
    }
    return p[1 + j];
}
#endif

/* Vertex n (3 floats). */
f32 *func_001A30D0(WChunkFile *m, s32 n);
#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
f32 *func_001A30D0(WChunkFile *m, s32 n) {
    WChunk *c = func_001A2B40(m, 0x70000, 0);

    if (c == NULL) {
        return NULL;
    }
    return (f32 *)c->data + n * 3;
}
#endif

/* Normal n. */
f32 *func_001A3120(WChunkFile *m, s32 n);
#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
f32 *func_001A3120(WChunkFile *m, s32 n) {
    WChunk *c = func_001A2B40(m, 0x80000, 0);

    if (c == NULL) {
        return NULL;
    }
    return (f32 *)c->data + n * 3;
}
#endif

/* UV n. */
f32 *func_001A3170(WChunkFile *m, s32 n);
#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
f32 *func_001A3170(WChunkFile *m, s32 n) {
    WChunk *c = func_001A2B40(m, 0xA0000, 0);

    if (c == NULL) {
        return NULL;
    }
    return (f32 *)c->data + n * 2;
}
#endif

/* Color n. */
f32 *func_001A31B0(WChunkFile *m, s32 n);
#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
f32 *func_001A31B0(WChunkFile *m, s32 n) {
    WChunk *c = func_001A2B40(m, 0xB0000, 0);

    if (c == NULL) {
        return NULL;
    }
    return (f32 *)c->data + n * 4;
}
#endif

/* Vertex n's weight count (each vertex: {count, count * {bone, weight}}). */
s32 func_001A31F0(WChunkFile *m, s32 n);
#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
s32 func_001A31F0(WChunkFile *m, s32 n) {
    WChunk *c = func_001A2B40(m, 0xC0000, 0);
    s32 *p;
    s32 i;

    if (c == NULL) {
        return -1;
    }
    p = (s32 *)c->data;
    for (i = 0; i < n; i++) {
        p += 1 + *p * 2;
    }
    return *p;
}
#endif

/* Vertex n's k-th weight ({bone, weight}). */
s32 *func_001A3250(WChunkFile *m, s32 n, s32 k);
#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
s32 *func_001A3250(WChunkFile *m, s32 n, s32 k) {
    WChunk *c = func_001A2B40(m, 0xC0000, 0);
    s32 *p;
    s32 i;

    if (c == NULL) {
        return NULL;
    }
    p = (s32 *)c->data;
    for (i = 0; i < n; i++) {
        p += 1 + *p * 2;
    }
    if (k >= *p) {
        return NULL;
    }
    return p + 1 + k * 2;
}
#endif

/* A mesh converted for drawing (built in work memory: Options.unk4C + 0x200000 / + 0x240000). */
typedef struct {
    u32 flags;          /* 0x00: 0x100000 primitives carry an index, 0x200000 a type, 0x1000 their words */
    s32 count;          /* 0x04: primitives */
    u16 *prims;         /* 0x08: per primitive: length | 0x8000 (flagged) or 0x4000, then the above */
    u32 format;         /* 0x0C */
    struct MeshVertex *verts; /* 0x10 */
    s32 nverts;         /* 0x14 */
    s32 vertSize;       /* 0x18: bytes */
    s32 primSize;       /* 0x1C: bytes */
    s16 nbones;         /* 0x20 */
} MeshOut;

typedef struct MeshVertex {
    f32 pos[3];         /* 0x00 */
    f32 normal[3];      /* 0x0C */
    u32 color;          /* 0x18: ARGB */
    f32 uv[2];          /* 0x1C */
    u32 bones;          /* 0x24: four bytes, the first in the top byte */
    u32 weights;        /* 0x28: their weights (0-255, adding up to 255) */
} MeshVertex;           /* 0x2C */

s32 func_001A33E0(MeshOut *out, WChunkFile *m);

s32 func_001A32E0(MeshOut *out, void *p, u32 n);
s32 func_001A3430(WChunkFile *m);
#ifdef NON_MATCHING   /* first draft (registers) */
/* Mesh n of the file, converted (only type 4 meshes; 3, 6, 7, 8 are skipped). */
s32 func_001A32E0(MeshOut *out, void *p, u32 n) {
    WChunk *c = func_001A2B40(func_001A2AE0(p), 2, 0);
    WChunk *m;

    if (c == NULL) {
        return 0;
    }
    if (n >= c->count) {
        return 0;
    }
    m = func_001A2760(p, n);
    if (m == NULL) {
        return 0;
    }
    switch (m->type) {
    case 4:
        out->nbones = func_001A2A00(p, n);
        return func_001A33E0(out, (WChunkFile *)m);
    case 8:
        return 0;
    case 7:
    case 6:
    case 3:
        return 0;
    }
    return 0;
}
#endif

s32 func_001A34C0(MeshOut *out, WChunkFile *m);
s32 func_001A3AE0(MeshOut *out, WChunkFile *m);

#ifdef NON_MATCHING   /* needs func_001A2B40 to match (callers rely on its registers) */
s32 func_001A33E0(MeshOut *out, WChunkFile *m) {
    if (func_001A2AF0(m, 0xC0000) != 0) {
        return func_001A34C0(out, m);
    }
    return func_001A3AE0(out, m);
}
#endif

s32 func_001A3430(WChunkFile *m);
#ifdef NON_MATCHING   /* first draft (registers) */
/* 1 when the strips' indices aren't all the same as strip 0's first. */
s32 func_001A3430(WChunkFile *m) {
    s32 first = func_001A2DD0(m, 0, 0);
    s32 n = func_001A2BF0(m);
    s32 count;
    s32 i;
    s32 k;

    for (i = 0; i < n; i++) {
        count = func_001A2CB0(m, i);
        for (k = 0; k < count; k++) {
            if (first != func_001A2DD0(m, i, k)) {
                return 1;
            }
        }
    }
    return 0;
}
#endif

extern s32 D_0023BAC8;          /* the biggest converted mesh part so far (bytes) */
extern u8 *D_003065DC;          /* Options.unk4C: work memory */

/* Convert a mesh with vertex weights: its primitives to a u16 list, its vertices to MeshVertex
   (colours to bytes, weights to 0-255 over at most 4 bones, the strongest last one taking what the
   rounding left). 0 when a part is too big (over 0x40000 bytes) or there's no work memory. */
#ifdef NON_MATCHING   /* first draft */
s32 func_001A3AE0(MeshOut *out, WChunkFile *m) {
    s32 bones[4];
    s32 w[4];
    MeshVertex *v;
    u16 *p;
    f32 *f;
    s32 *wt;
    s32 nstrips;
    s32 nprims;
    s32 len;
    s32 type;
    s32 size;
    s32 total;
    s32 last;
    s32 n;
    s32 i;
    s32 k;
    s32 j;

    func_001A2AF0(m, 0xA0000);
    out->flags = 0x221000;
    nstrips = func_001A2BF0(m);
    out->count = func_001A2D30(m);
    total = 0;
    if (func_001A3430(m) == 1) {
        out->flags |= 0x100000;
    }
    for (i = 0; i < nstrips; i++) {
        nprims = func_001A2CB0(m, i);
        for (k = 0; k < nprims; k++) {
            total += func_001A2E80(m, i, k);
        }
    }
    size = (out->count + total) * 2;
    if (out->flags & 0x100000) {
        size += out->count * 2;
    }
    if (out->flags & 0x200000) {
        size += out->count * 2;
    }
    if (D_0023BAC8 < size) {
        D_0023BAC8 = size;
    }
    if (size > 0x40000) {
        return 0;
    }
    if (D_003065DC == NULL) {
        return 0;
    }
    out->prims = (u16 *)(D_003065DC + 0x200000);
    out->primSize = size;
    p = out->prims;
    for (i = 0; i < nstrips; i++) {
        nprims = func_001A2CB0(m, i);
        type = func_001A2C30(m, i);
        for (k = 0; k < nprims; k++) {
            len = func_001A2E80(m, i, k);
            *p = len;
            if (func_001A2F30(m, i, k) != 0) {
                *p |= 0x8000;
            } else {
                *p |= 0x4000;
            }
            p++;
            if (out->flags & 0x100000) {
                *p++ = func_001A2DD0(m, i, k);
            }
            if (out->flags & 0x200000) {
                if (type == 0x40000) {
                    *p = 1;
                } else if (type == 0x30000) {
                    *p = 0;
                }
                p++;
            }
            if (out->flags & 0x1000) {
                for (j = 0; j < len; j++) {
                    *p++ = func_001A2FF0(m, i, k, j);
                }
            }
        }
    }
    out->format = 0x35;
    out->format |= 0x100;
    out->nverts = func_001A2D90(m);
    size = out->nverts * sizeof(MeshVertex);
    if (D_0023BAC8 < size) {
        D_0023BAC8 = size;
    }
    if (size > 0x40000) {
        return 0;
    }
    if (D_003065DC == NULL) {
        return 0;
    }
    out->verts = (MeshVertex *)(D_003065DC + 0x240000);
    out->vertSize = size;
    v = out->verts;
    for (i = 0; i < out->nverts; i++, v++) {
        f = func_001A30D0(m, i);
        v->pos[0] = f[0];
        v->pos[1] = f[1];
        v->pos[2] = f[2];
        f = func_001A3120(m, i);
        v->normal[0] = f[0];
        v->normal[1] = f[1];
        v->normal[2] = f[2];
        f = func_001A3170(m, i);
        v->uv[0] = f[0];
        v->uv[1] = f[1];
        f = func_001A31B0(m, i);
        v->color = ((u32)f[3] << 24) | ((u32)f[0] << 16) | ((u32)f[1] << 8) | (u32)f[2];
        v->bones = 0;
        v->weights = 0;
        n = func_001A31F0(m, i);
        w[0] = w[1] = w[2] = w[3] = 0;
        bones[0] = bones[1] = bones[2] = bones[3] = 0;
        last = 0;
        for (k = 0; k < n; k++) {
            wt = func_001A3250(m, i, k);
            w[k] = (s32)(255.0f * *(f32 *)&wt[1] / 100.0f) & 0xFF;
            if (w[k] > 0) {
                last = k;
            }
            bones[k] = *(u8 *)wt;
        }
        w[last] += 0xFF - w[0] - w[1] - w[2] - w[3];
        for (k = 0; k < n - 1; k++) {
            if (w[k] == 0) {
                w[k] = w[k + 1];
                bones[k] = bones[k + 1];
                w[k + 1] = 0;
            }
        }
        v->weights = (w[0] << 24) | (w[1] << 16) | (w[2] << 8) | w[3];
        v->bones = (bones[0] << 24) | (bones[1] << 16) | (bones[2] << 8) | bones[3];
    }
    return 1;
}
#endif
