#include "common.h"
#include "options.h"
#include "game.h"
#include "object.h"

/* GAME.BIN: the game's object pools. Most are an array of objects, a table of pointers to them and
   three counters (in use, free, and a third) that start empty. */

extern u8 D_00476DD0[0x8700];

void func_00638BE0(void) {
    memset(D_00476DD0, 0, sizeof(D_00476DD0));
}

typedef struct {
    u8 unk0[3];
    u8 id;
    u8 unk4[0x1170 - 4];
} Obj1170;

extern Obj1170 D_003E8230[10];
extern Obj1170 *D_00386BA0[10];
extern s16 D_0023BDFC;
extern s32 D_0023BE00;
extern s32 D_0023BE04;
extern s32 D_0023BDF8;

void func_00651040(void) {
    u8 i;
    Obj1170 *p;

    memset(D_003E8230, 0, sizeof(D_003E8230));
    D_0023BE00 = 0;
    D_0023BDFC = 10;
    D_0023BE04 = 0;
    for (i = 0; i < 10; i++) {
        p = &D_003E8230[i];
        D_00386BA0[i] = p;
        p->id = i;
    }
    memset(&D_0023BDF8, 0, 4);
}

typedef struct { u8 unk0[0xD90]; } ObjD90;
extern ObjD90 D_003FFCA0[0x32];
extern ObjD90 *D_00386AD0[0x32];
extern s16 D_0023BDEC;
extern s32 D_0023BDF0;
extern s32 D_0023BDF4;

void func_00687E90(void) {
    u8 i;

    memset(D_003FFCA0, 0, sizeof(D_003FFCA0));
    D_0023BDF0 = 0;
    D_0023BDEC = 0x32;
    D_0023BDF4 = 0;
    for (i = 0; i < 0x32; i++) {
        D_00386AD0[i] = &D_003FFCA0[i];
    }
}

typedef struct { u8 unk0[0x250]; } Obj250;
extern Obj250 D_003C51B0[0x40];
extern Obj250 *D_00386950[0x40];
extern s16 D_0023BDB0;
extern s32 D_0023BDB4;
extern s32 D_0023BDB8;

void func_00560880(void) {
    s32 i;

    memset(D_003C51B0, 0, sizeof(D_003C51B0));
    D_0023BDB4 = 0;
    D_0023BDB0 = 0x40;
    D_0023BDB8 = 0;
    for (i = 0x3F; i >= 0; i--) {
        D_00386950[i] = &D_003C51B0[i];
    }
}

typedef struct { u8 unk0[0x1F0]; } Obj1F0;
extern Obj1F0 D_003BD5B0[0x40];
extern Obj1F0 *D_00386850[0x40];
extern s16 D_0023BDA4;
extern s32 D_0023BDA8;
extern s32 D_0023BDAC;

void func_00551DB0(void) {
    u8 i;

    memset(D_003BD5B0, 0, sizeof(D_003BD5B0));
    D_0023BDA8 = 0;
    D_0023BDA4 = 0x40;
    D_0023BDAC = 0;
    for (i = 0; i < 0x40; i++) {
        D_00386850[i] = &D_003BD5B0[i];
    }
}

typedef struct { u8 unk0[0x40]; } Obj40;
extern Obj40 D_003B8490[0x40];
extern Obj40 *D_00386750[0x40];
extern s16 D_0023BD98;
extern s32 D_0023BD9C;
extern s32 D_0023BDA0;
extern s16 D_0023BE08;

void func_005A9740(void) {
    u8 i;

    memset(D_003B8490, 0, sizeof(D_003B8490));
    D_0023BD9C = 0;
    D_0023BD98 = 0x40;
    D_0023BDA0 = 0;
    for (i = 0; i < 0x40; i++) {
        D_00386750[i] = &D_003B8490[i];
    }
    D_0023BE08 = 0;
}

typedef struct { u8 unk0[0xC4]; } ObjC4;
extern ObjC4 D_003B2290[0x80];
extern ObjC4 *D_00386410[0x80];
extern s16 D_0023BD88;
extern s32 D_0023BD8C;
extern s32 D_0023BD90;

void func_00590B60(void) {
    u8 i;

    memset(D_003B2290, 0, sizeof(D_003B2290));
    D_0023BD8C = 0;
    D_0023BD88 = 0x80;
    D_0023BD90 = 0;
    for (i = 0; i < 0x80; i++) {
        D_00386410[i] = &D_003B2290[i];
    }
}

typedef struct { s32 w[16]; } Blk40;

typedef struct {
    u8 unk0;
    u8 unk1;
    u8 unk2;
    u8 unk3[0x1C - 3];
    u8 unk1C;
    u8 unk1D[0x880 - 0x1D];
    s32 unk880;
    s32 unk884;
    s32 unk888;
    u8 unk88C[0x894 - 0x88C];
    s32 unk894;
    u8 unk898[0x920 - 0x898];
    s32 unk920[0x80];
    Blk40 unkB20[0x80];
    u8 unk2B20[0x2C70 - 0x2B20];
    s32 unk2C70[4];
    s32 unk2C80[0x80];
    u8 unk2E80[0x4730 - 0x2E80];
    u8 unk4730[0x10][0x60];
    Blk40 unk4D30[0x80];
    u8 unk6D30[5];
    u8 unk6D35[8];
    u8 unk6D3D[3];
} Work470090;

extern Work470090 D_00470090;
extern s32 D_0048BE14[4];
extern s16 D_0048AEAA[];
void func_0058E400(void);

void func_005A74C0(void);
#ifdef NON_MATCHING   /* register allocation in the loops differs */
void func_005A74C0(void) {
    Work470090 *w;
    s32 i;

    D_00470090.unk1C = 0;
    D_00470090.unk0 = 1;
    D_00470090.unk1 = 1;
    D_00470090.unk2 = 4;
    w = &D_00470090;
    for (i = 0; i < 8; i++) {
        w->unk6D35[i] = 0;
    }
    w->unk894 = -1;
    w->unk888 = 0;
    w->unk884 = 0;
    w->unk880 = 0;
    for (i = 0; i < 4; i++) {
        w->unk2C70[i] = 0;
    }
    for (i = 0; i < 0x80; i++) {
        w->unk2C80[i] = 0;
        w->unkB20[i] = w->unk4D30[i];
        w->unk920[i] = -1;
    }
    for (i = 0; i < 0x10; i++) {
        memset(D_00470090.unk4730[i], 0, 0x60);
    }
    D_0048BE14[0] = 0;
    D_0048BE14[1] = 0;
    D_0048BE14[2] = 0;
    D_0048BE14[3] = 0;
    D_0048AEAA[0] = 0;
    D_0048BF60.unk3A = 0;
    func_0058E400();
}
#endif

extern u8 D_00720BE0[0x3444];
extern u8 D_00720BF0[];
extern u8 D_00723D28[];
extern u8 D_00720890[];
extern u8 D_00720910[];
extern u8 D_00720790[];
extern u8 *D_0023BFBC;
extern u8 *D_0023BFB8;

void func_006AC300(void) {
    memset(D_00720BE0, 0, sizeof(D_00720BE0));
    D_00720890[0] = 0;
    D_0048BF60.unk63 = 1;
    D_0048BF60.unkBC = 1;
    D_0023BFBC = D_00720BF0;
    D_0023BFB8 = D_00723D28;
    D_00720910[0] = 0;
    D_00720790[0] = 0;
}

#include "effect.h"

extern Obj250L D_003CE5B0[0xA8];
extern Obj250L D_003E67E0;
extern Obj250L *D_0023BDC8;     /* first */
extern Obj250L *D_0023BDC4;     /* last */
extern s16 D_0023BDC0;
extern s16 D_0023BDBC;
extern s16 D_0023BDCC[2];         /* objects in each list */
extern Obj250L *D_0023BDD0[2];     /* last */
extern Obj250L *D_0023BDD8[2];     /* first */
void func_006201F0(u8 list);

void func_006200C0(void) {
    s32 i;
    Obj250L *p;
    u8 *q;

    D_0023BDC8 = D_003CE5B0;
    D_0023BDC4 = &D_003E67E0;
    for (i = 0; i < 0xA8; i++) {
        p = &D_003CE5B0[i];
        memset(p, 0, sizeof(Obj250L));
        if (i + 1 != 0xA8) {
            p->prev = &D_003CE5B0[i + 1];
        }
    }
    for (i = 1; i >= 0; i--) {
        D_0023BDD8[i] = NULL;
        D_0023BDD0[i] = NULL;
        D_0023BDCC[i] = 0;
    }
    D_0023BDC0 = 0xA8;
    func_006201F0(0);
    func_006201F0(1);
    q = D_00306590.unk84;
    D_0023BDBC = 0x100;
    for (i = 0; i < 0xA8; i++) {
        *q = 0;
        q += 0x70;
    }
}

typedef struct Obj110 {
    u8 active;
    u8 unk1[2];
    u8 unk3;
    s8 unk4;
    u8 unk5[0x1C - 5];
    void (*fn)(struct Obj110 *p);
    u8 unk20[0x110 - 0x20];
} Obj110;

typedef struct {
    s32 unk0;
    Obj110 *unk4;
    s32 unk8;
} Unk387650;

extern Unk387650 D_00387650[2];
extern Obj110 D_00387670[3][10];
void func_0058A8B0(s32 i);
void func_0058A910(void);
void func_001AECB0(void (*fn)(void));   /* font.c: drops a per-frame callback */
void func_00575030(void);
void func_00589FB0(void);

void func_0058A910(void) {
    func_001AECB0(func_00575030);
    func_001AECB0(func_00589FB0);
    memset(D_00387650, 0, sizeof(D_00387650));
}

void func_00588A80(void) {
    Obj110 *p;
    Unk387650 *q;
    s32 i;
    s32 j;
    u8 t;

    p = D_00387670[0];
    if (D_0048BF60.state == 4) {
        q = D_00387650;
        for (i = 0; i < 2; i++) {
            if (q->unk4 != NULL && q->unk8 != 0) {
                if (q->unk4->active == 0 || q->unk4->unk3 == 0) {
                    func_0058A8B0(i);
                }
            } else {
                func_0058A8B0(i);
            }
        }
        for (i = 0; i < 3; i++) {
            for (j = 0; j < 10; j++, p++) {
                if (p->active == 0 || p->unk3 == 0) {
                    memset(p, 0, sizeof(Obj110));
                } else {
                    t = p->unk3;
                    p->unk3 = p->unk4;
                    p->unk4 = t;
                    p->fn(p);
                }
            }
        }
    } else {
        p = D_00387670[1];
        for (i = 1; i < 3; i++) {
            for (j = 0; j < 10; j++, p++) {
                if (p->active != 0 && p->fn != NULL && p->unk3 != 0) {
                    t = p->unk3;
                    p->unk3 = p->unk4;
                    p->unk4 = t;
                    p->fn(p);
                }
            }
        }
        func_0058A910();
        memset(D_00387670, 0, sizeof(D_00387670));
    }
}

/* The tasks: 0x20 objects of 0xC0 bytes (D_003E6A30) with a free stack, in a list from
   D_0023BDE8 (run by func_005437C0) to D_0023BDE4. */
typedef struct Task {
    s8 active;          /* 0x00 */
    u8 unk1;
    u8 kind;            /* 0x02 */
    u8 unk3[3];
    u8 index;           /* 0x06: in D_003E6A30 */
    u8 unk7[0x14 - 7];
    void (*update)(struct Task *t); /* 0x14 */
    s32 unk18;          /* 0x18 */
    struct Task *next;  /* 0x1C */
    struct Task *prev;  /* 0x20 */
    u8 unk24[4];
    void *owner;        /* 0x28 */
    u8 unk2C[0xC0 - 0x2C];
} Task;

typedef Task ObjC0;
extern ObjC0 D_003E6A30[0x20];
extern ObjC0 *D_00386A50[0x20];
extern s16 D_0023BDE0;          /* how many are free */
extern Task *D_0023BDE4;        /* the list's last */
extern Task *D_0023BDE8;        /* its first */

void func_00543580(void) {
    u8 i;

    memset(D_003E6A30, 0, sizeof(D_003E6A30));
    D_0023BDE4 = 0;
    D_0023BDE0 = 0x20;
    D_0023BDE8 = 0;
    for (i = 0; i < 0x20; i++) {
        D_00386A50[i] = &D_003E6A30[i];
    }
}

/* A cleared task, put first in the list (last when last != 0); NULL when none is free. */
Task *func_00543600(s32 last) {
    Task *t;

    if (D_0023BDE0 == 0) {
        return NULL;
    }
    D_0023BDE0--;
    t = D_00386A50[D_0023BDE0];
    memset(t, 0, sizeof(Task));
    t->index = (u32)((u8 *)t - (u8 *)D_003E6A30) / sizeof(Task);
    if (D_0023BDE8 == NULL) {
        D_0023BDE4 = t;
        D_0023BDE8 = t;
        t->prev = NULL;
        t->next = NULL;
    } else if (last == 0) {
        t->next = D_0023BDE8;
        t->prev = NULL;
        D_0023BDE8->prev = t;
        D_0023BDE8 = t;
    } else {
        t->next = NULL;
        t->prev = D_0023BDE4;
        D_0023BDE4->next = t;
        D_0023BDE4 = t;
    }
    return t;
}

/* Frees task t (takes it out of the list). */
s32 func_005436F0(void *p) {
    Task *t = p;

    if (D_0023BDE0 == 0x20) {
        return 0;
    }
    t->active = 0;
    D_00386A50[D_0023BDE0] = t;
    D_0023BDE0++;
    if (D_0023BDE8 == t) {
        if (t->next == NULL) {
            D_0023BDE4 = NULL;
            D_0023BDE8 = NULL;
        } else {
            t->next->prev = NULL;
            D_0023BDE8 = t->next;
        }
        return 1;
    }
    if (D_0023BDE4 == t) {
        if (t->prev == NULL) {
            D_0023BDE4 = NULL;
            D_0023BDE8 = NULL;
        } else {
            t->prev->next = NULL;
            D_0023BDE4 = t->prev;
        }
        return 1;
    }
    t->prev->next = t->next;
    t->next->prev = t->prev;
    return 1;
}

/* A task of kind 5 running fn for owner. */
void *func_00543800(void *owner, void *fn) {
    Task *t = func_00543600(1);

    if (t == NULL) {
        return NULL;
    }
    t->active = 1;
    t->kind = 5;
    t->update = fn;
    t->unk18 = 0;
    t->owner = owner;
    return t;
}

extern u8 D_003939E0[0x7C0];

void func_00543C80(void) {
    memset(D_003939E0, 0, sizeof(D_003939E0));
}

typedef struct {
    u8 unk0[0x14];
    s32 unk14;
    u8 unk18[0x40 - 0x18];
} Obj40b;

extern Obj40b D_00394F20[0x20];    /* the route nodes (navnodes.c) */
extern s32 D_0023BED8;

void func_00601F60(void) {
    s8 i;

    memset(D_00394F20, 0, sizeof(D_00394F20));
    for (i = 0; i < 0x20; i++) {
        memset(&D_00394F20[i].unk14, -1, 4);
    }
    D_0023BED8 = 0;
}

/* The room's sound sources and ambient sounds (from the room data) and its reverb. */
typedef struct {
    u8 unk0;            /* 0x00 */
    u8 used;            /* 0x01 */
    s16 unk2;           /* 0x02 */
    u8 data[0x60];      /* 0x04 */
    u8 unk64[4];
} SoundSrc;             /* 0x68 */

typedef struct {
    u8 used;            /* 0x00 */
    u8 unk1;            /* 0x01 */
    s16 unk2;           /* 0x02 */
    s16 unk4;           /* 0x04 */
    u8 unk6[2];
    f32 pos[3];         /* 0x08 */
    u8 unk14[8];
} RoomAmbient;          /* 0x1C */

typedef struct {
    SoundSrc src[32];       /* 0x000 */
    RoomAmbient amb[4];     /* 0xD00 */
    s16 unkD70;             /* 0xD70 */
    s8 unkD72;              /* 0xD72 */
    u8 reverbDelay;         /* 0xD73 */
    s32 unkD74;             /* 0xD74: reverb depth (or-ed in) */
    u8 reverbFeedback;      /* 0xD78 */
    u8 unkD79[0xD80 - 0xD79];
} Work3941A0;

extern Work3941A0 D_003941A0;
extern s8 D_0023BE4C;
extern s8 D_0023BE48;

void func_00543D00(void) {
    memset(&D_003941A0, 0, sizeof(D_003941A0));
    D_0023BE4C = 0;
    D_003941A0.unkD74 = 0x80000000;
    D_0023BE48 = 0;
}

typedef struct {
    s32 unk0;
    u8 data[0x60];      /* 0x04 */
    s32 unk64;          /* 0x64 */
} SoundSrcDef;

typedef struct {
    s32 used;           /* 0x00 */
    s32 unk4;           /* 0x04 */
    f32 pos[3];         /* 0x08 */
    s32 unk14;          /* 0x14 */
    s32 unk18;          /* 0x18 */
} RoomAmbientDef;

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 reverbDepth;    /* 0x08 */
    s32 reverbDelay;    /* 0x0C */
    s32 reverbFeedback; /* 0x10 */
} RoomReverbDef;

void *memcpy(void *d, const void *s, u32 n);

/* Takes the room's sound sources, ambient sounds and reverb from f. */
void func_00543D40(s32 *f) {
    s8 i;
    SoundSrc *e;
    s32 *off;
    s32 n1;
    s32 n2;
    RoomAmbient *a;
    u8 *d;
    RoomReverbDef *r;

    func_00543D00();
    n1 = f[0];
    n2 = f[1];
    off = f + 2;
    if (n1 != 0 || n2 != 0) {
        D_0023BE4C = n1;
        D_0023BE48 = n2;
        for (e = D_003941A0.src, i = 0; i < D_0023BE4C; i++, off++, e++) {
            d = (u8 *)f + *off;
            e->unk0 = ((SoundSrcDef *)d)->unk0;
            e->used = 1;
            e->unk2 = ((SoundSrcDef *)d)->unk64;
            memcpy(e->data, ((SoundSrcDef *)d)->data, sizeof(e->data));
        }
        for (i = 0, a = D_003941A0.amb; i < D_0023BE48; i++, a++) {
            d = (u8 *)f + *off++;
            a->used = ((RoomAmbientDef *)d)->used;
            a->unk1 = ((RoomAmbientDef *)d)->unk14;
            a->unk2 = ((RoomAmbientDef *)d)->unk4;
            a->unk4 = ((RoomAmbientDef *)d)->unk18;
            *(Vec3 *)a->pos = *(Vec3 *)((RoomAmbientDef *)d)->pos;
        }
        r = (RoomReverbDef *)((u8 *)off + n1 * sizeof(SoundSrc) + n2 * sizeof(RoomAmbient));
        D_003941A0.unkD70 = r->unk0;
        D_003941A0.unkD72 = r->unk4;
        D_003941A0.unkD74 |= r->reverbDepth;
        D_003941A0.reverbDelay = r->reverbDelay;
        D_003941A0.reverbFeedback = r->reverbFeedback;
    }
}

extern u8 D_00395EA0[0x930];
extern s8 D_0023BF50;
extern s8 D_0023BF4C;

void func_0067B520(void) {
    memset(D_00395EA0, 0, sizeof(D_00395EA0));
    D_0023BF50 = 0;
    D_0023BF4C = 0;
}

extern u8 D_003873D0[0x280];
extern u8 D_00386BD0[0x800];
extern u8 D_00710A80[0x180];
extern u8 D_00710900[0x180];
extern u8 D_00710880[0x80];
extern s8 D_0023BF40;
extern s8 D_0023BF3C;
extern s32 D_0023BF38;
extern s32 D_0023BF34;

void func_006631B0(void) {
    memset(D_003873D0, 0, sizeof(D_003873D0));
    memset(D_00386BD0, 0, sizeof(D_00386BD0));
    D_0023BF40 = 0;
    D_0023BF3C = 0;
    memset(D_00710A80, 0, sizeof(D_00710A80));
    memset(D_00710900, 0, sizeof(D_00710900));
    memset(D_00710880, 0, sizeof(D_00710880));
    D_0023BF38 = 0;
    D_0023BF34 = 0;
}

extern u8 D_003614F0[0x1E00];

void func_00651DF0(void) {
    memset(D_003614F0, 0, sizeof(D_003614F0));
}

extern s32 D_003614C0[10];

void func_00651E10(u8 i) {
    if (i == 0xFF) {
        memset(D_003614C0, 0, sizeof(D_003614C0));
    } else {
        memset(&D_003614C0[i], 0, sizeof(D_003614C0[i]));
    }
}

extern s16 D_0023BD78;
extern s16 D_0023BD7C;
extern u8 D_003839B0[0x100];
extern u8 D_0035FFD0[0xB10];

void func_005BBF30(void) {
    D_0023BD78 = 0;
    D_0023BD7C = 0;
    memset(D_003839B0, 0, sizeof(D_003839B0));
    memset(D_0035FFD0, 0, sizeof(D_0035FFD0));
}

extern s16 D_003AEF56[];
extern s8 D_003AEF75[];

void func_0058E400(void) {
    D_003AEF56[0] = 0;
    D_003AEF75[0] = 0;
}

/* Returns an object to the free list. */
s32 func_00620530(Obj250L *p) {
    Obj250L *prev;
    Obj250L *next;

    p->unk1 = 0;
    p->unk0 = 0;
    prev = p->prev;
    if (prev == NULL) {
        next = p->next;
        D_0023BDD8[p->list] = next;
        if (next == NULL) {
            D_0023BDD0[p->list] = NULL;
        } else {
            next->prev = NULL;
        }
    } else if (p->next == NULL) {
        D_0023BDD0[p->list] = prev;
        prev->next = NULL;
    } else {
        prev->next = p->next;
        p->next->prev = p->prev;
    }
    D_0023BDCC[p->list]--;
    if (D_0023BDC4 != NULL) {
        D_0023BDC4->prev = p;
    } else {
        D_0023BDC8 = p;
    }
    p->prev = NULL;
    D_0023BDC4 = p;
    D_0023BDC0++;
    return 1;
}

#ifdef NON_MATCHING   /* the list number is kept in a different register */
/* Frees every object of a list. */
void func_006201F0(u8 list) {
    Obj250L **head;
    Obj250L *p;

    head = &D_0023BDD8[list];
    for (p = *head; p != NULL; p = p->next) {
        func_00620530(p);
    }
    *head = NULL;
    D_0023BDD0[list] = NULL;
    D_0023BDCC[list] = 0;
}
#endif

extern u8 D_0070F8B0[0x848];

void func_0064F600(void) {
    memset(D_0070F8B0, 0, sizeof(D_0070F8B0));
}

/* Drawing the shadows and lights of list 0 and list 1 (renderer callbacks queued per object). */

void func_00190ED0(Mat44 *dst, Mat44 *src);
void func_0018FA40(Mat44 *m, f32 x, f32 y, f32 z);
void func_001B1460(void *fn, void *a, void *b);
void func_001B1600(void *fn, void *a, void *b);
void func_00620770(Obj250L *o);
s32 func_00346F60(Vec3 *pos);
void func_00345D00(Obj250L *o);
extern u8 D_00343330[];         /* (SUBMAIN.BIN) the callback */

#ifdef NON_MATCHING   /* the vector's element addresses are kept in registers in the original */
void func_006208B0(void) {
    Obj250L *o;
    Mat44 m;
    f32 v[4];

    for (o = D_0023BDD8[0]; o != NULL; o = o->next) {
        if ((o->owner != NULL && o->owner->drawn == 0) || o->unk0 == 0 || o->unk1 == 0) {
            continue;
        }
        if (o->flags & 0x8000) {
            if (o->flags & 0x2000) {
                func_00620770(o);
            } else {
                if (o->mtx != NULL) {
                    func_00190ED0(&m, o->mtx);
                    func_0018FA40(&m, o->pos.x, o->pos.y, o->pos.z);
                    v[0] = m.m[3][0];
                    v[1] = m.m[3][1];
                    v[2] = m.m[3][2];
                } else {
                    v[0] = o->pos.x;
                    v[1] = o->pos.y;
                    v[2] = o->pos.z;
                }
                v[3] = 1.0f;
                func_001B1460(D_00343330, o, v);
            }
        } else if (func_00346F60(&o->pos) == 1) {
            func_00345D00(o);
        }
    }
}
#endif

#ifdef NON_MATCHING   /* the vector's element addresses are kept in registers in the original */
void func_00620A30(void) {
    Obj250L *o;
    Mat44 m;
    f32 v[4];

    for (o = D_0023BDD8[1]; o != NULL; o = o->next) {
        if (o->unk0 == 0 || o->unk1 == 0) {
            continue;
        }
        if (o->mtx != NULL) {
            func_00190ED0(&m, o->mtx);
            func_0018FA40(&m, o->pos.x, o->pos.y, o->pos.z);
            v[0] = m.m[3][0];
            v[1] = m.m[3][1];
            v[2] = m.m[3][2];
        } else {
            v[0] = o->pos.x;
            v[1] = o->pos.y;
            v[2] = o->pos.z;
        }
        v[3] = 1.0f;
        if (o->flags & 0x10000) {
            func_001B1600(D_00343330, o, NULL);
        } else {
            func_001B1460(D_00343330, o, v);
        }
    }
}
#endif

void func_00620740(void) {
    func_006208B0();
    func_00620A30();
}

extern u8 D_00724030[0x200];

void func_006B9A00(void) {
    memset(D_00724030, 0, sizeof(D_00724030));
}

void func_0060DE80(void) {
}

void func_0060F2B0(void) {
}

void func_0060F2C0(void) {
}
