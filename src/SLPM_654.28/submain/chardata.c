#include "common.h"
#include "options.h"
#include "object.h"
#include "enemy.h"

/* SUBMAIN.BIN: each character's 0x1324-byte data block (D_0023BCD0: players 0-3, NPCs 4, enemy
   types from 5), loaded from the files D_00358310 names and pointed to by the character's model. */

extern u8 *D_0023BCD0;
extern char **D_00358310[];
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
void func_0018D630(void *dst, const void *src, s32 n);

typedef struct {
    u8 unk0[0x1460];
    u8 *data;           /* 0x1460 */
    u8 *data74;         /* 0x1464: data + 0x74 */
} ModelData;

u8 *func_00347320(void) {
    D_0023BCD0 = D_00306590.charData;
    return D_0023BCD0;
}

void func_00347590(DrawObjFull *o, s32 i) {
    ((ModelData *)o->model)->data = D_0023BCD0 + i * 0x1324;
    ((ModelData *)o->model)->data74 = D_0023BCD0 + i * 0x1324 + 0x74;
}

/* Loads o's data block (players by character, NPCs, enemies whose type has one); other types:
   just file D_00358310[table][index]. Returns the size read. */
s32 func_00347340(DrawObjFull *o, s8 table, s32 index);
#ifdef NON_MATCHING   /* first draft */
s32 func_00347340(DrawObjFull *o, s8 table, s32 index) {
    s32 n;
    s32 k;
    ThingDef *def;

    if (D_0023BCD0 == NULL && func_00347320() == NULL) {
        return 0;
    }
    switch (o->type) {
    case 0:
        k = o->kind;
        ((ModelData *)o->model)->data = NULL;
        ((ModelData *)o->model)->data74 = NULL;
        n = func_001CFBB0(D_00358310[0][o->unkB98], (void *)D_00306590.unk4C, 1, 0);
        if (n > 0) {
            func_0018D630(D_0023BCD0 + k * 0x1324, (void *)D_00306590.unk4C, 0x1324);
            func_00347590(o, k);
        }
        return n;
    case 1:
        ((ModelData *)o->model)->data = NULL;
        ((ModelData *)o->model)->data74 = NULL;
        n = func_001CFBB0(D_00358310[0][0], (void *)D_00306590.unk4C, 1, 0);
        if (n > 0) {
            func_0018D630(D_0023BCD0 + 0x4C90, (void *)D_00306590.unk4C, 0x1324);
            func_00347590(o, 4);
        }
        return n;
    case 2:
        ((ModelData *)o->model)->data = NULL;
        ((ModelData *)o->model)->data74 = NULL;
        def = &D_00357500[o->kind].defs[o->unkBBA];
        if (def->hasData == 0) {
            return 0;
        }
        k = def->data;
        n = func_001CFBB0(D_00358310[0][k], (void *)D_00306590.unk4C, 1, 0);
        if (n > 0) {
            func_0018D630(D_0023BCD0 + (k + 5) * 0x1324, (void *)D_00306590.unk4C, 0x1324);
            func_00347590(o, k + 5);
        }
        return n;
    }
    return func_001CFBB0(D_00358310[table][index], (void *)D_00306590.unk4C, 1, 0);
}
#endif

void func_00347580(void) {
}

/* How far v sits off the camera's view, as two shares (sideways and up) of a turn, each
   scaled by x / y, fractional part kept. */
extern f32 D_00306338[3];       /* the camera */
extern f32 D_00306344[3];       /* what it looks at */
f32 func_00191BD0(f32 x);
u32 func_00340140(const f32 *a, const f32 *b);
u32 func_00340190(const f32 *a, const f32 *b);

void func_00347040(f32 x, f32 y, f32 *v, f32 *outA, f32 *outB) {
    f32 b;
    f32 a;
    f32 c;
    f32 d;

    a = (f32)(func_00340140(D_00306344, D_00306338) & 0xFFFF) / 65536.0f;
    b = (f32)(func_00340190(D_00306344, D_00306338) & 0xFFFF) / 65536.0f;
    c = (f32)((0x10000 - func_00340140(v, D_00306338)) & 0xFFFF) / 65536.0f;
    d = (f32)(func_00340190(v, D_00306338) & 0xFFFF) / 65536.0f;
    *outA = func_00191BD0(x * (a + c));
    *outB = func_00191BD0(y * (b + d));
}
