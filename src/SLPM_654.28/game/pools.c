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

/* 0xA8 objects in two lists (by unk7) plus a free list. In a list, next/prev link them; in the
   free list, prev is the next free one. */
typedef struct Obj250L {
    s8 unk0;
    s8 unk1;
    u8 unk2[5];
    u8 list;
    u8 unk8[0x1C - 8];
    struct Obj250L *next;
    struct Obj250L *prev;
    u8 unk24[0x2C - 0x24];
    DrawObjFull *owner;  /* 0x2C: drawn only when its owner was */
    u8 unk30[0x38 - 0x30];
    Vec3 pos;           /* 0x38 */
    u8 unk44[0x1F0 - 0x44];
    Mat44 *mtx;         /* 0x1F0: follows this matrix (pos is then an offset) */
    u8 unk1F4[0x200 - 0x1F4];
    u32 flags;          /* 0x200 */
    u8 unk204[0x250 - 0x204];
} Obj250L;

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

typedef struct { u8 unk0[0xC0]; } ObjC0;
extern ObjC0 D_003E6A30[0x20];
extern ObjC0 *D_00386A50[0x20];
extern s16 D_0023BDE0;
extern s32 D_0023BDE4;
extern s32 D_0023BDE8;

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

extern u8 D_003939E0[0x7C0];

void func_00543C80(void) {
    memset(D_003939E0, 0, sizeof(D_003939E0));
}

typedef struct {
    u8 unk0[0x14];
    s32 unk14;
    u8 unk18[0x40 - 0x18];
} Obj40b;

extern Obj40b D_00394F20[0x20];
extern s32 D_0023BED8;

void func_00601F60(void) {
    s8 i;

    memset(D_00394F20, 0, sizeof(D_00394F20));
    for (i = 0; i < 0x20; i++) {
        memset(&D_00394F20[i].unk14, -1, 4);
    }
    D_0023BED8 = 0;
}

typedef struct {
    u8 unk0[0xD74];
    s32 unkD74;
    u8 unkD78[0xD80 - 0xD78];
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
