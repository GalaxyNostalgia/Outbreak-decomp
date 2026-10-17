#include "common.h"

/* GAME.BIN: the room's light sets (16 at most, from the room data); setting one fills the
   render state (fog, colours, the light's direction). */

typedef struct {
    s32 used;           /* 0x00 */
    f32 fogNear;        /* 0x04 */
    f32 fogFar;         /* 0x08 */
    u32 fogColor;       /* 0x0C: 0xRRGGBB */
    u32 colorA;         /* 0x10 */
    u32 colorB;         /* 0x14 */
    u32 unk18;          /* 0x18: low byte and high half */
    u16 rx;             /* 0x1C */
    u8 pad1E[2];
    u16 ry;             /* 0x20 */
    u8 pad22[2];
    u8 unk24[0x10];     /* 0x24 */
    u8 unk34[0x10];     /* 0x34 */
    u8 unk44[0x10];     /* 0x44 */
    s32 unk54;          /* 0x54 */
    s32 unk58;          /* 0x58 */
    s32 unk5C;          /* 0x5C */
} LightSet;             /* 0x60 */

typedef struct {
    s32 version;        /* 2 */
    s32 n;
    s32 offs[1];
} LightFile;

extern LightSet D_003ACD80[16];
extern Vec3 D_006EC0D8;         /* the light's direction before turning */
extern s8 D_003062BD[];         /* the set in use (-1: none) */
extern f32 D_00306210[];
extern f32 D_00306214[];
extern u8 D_00306218[];
extern u8 D_00306219[];
extern u8 D_0030621A[];
extern u8 D_0030628C[];
extern u8 D_0030628D[];
extern u8 D_0030628E[];
extern u8 D_0030628F[];
extern u8 D_00306298[];
extern u8 D_00306299[];
extern u8 D_0030629A[];
extern u8 D_0030629B[];
extern s16 D_003062CC[];
extern s16 D_003062CE[];
extern f32 D_003062C0[];
extern u8 D_003060DC[];
extern u8 D_003060EC[];
extern u8 D_003060FC[];
extern s32 D_0030624C[];
extern s32 D_00306250[];
extern u8 D_003062BF[];
extern u8 D_0030627C[];
extern u8 D_0030627D[];
extern u8 D_0030627E[];
void *memset(void *p, s32 c, u32 n);
void *memcpy(void *d, const void *s, u32 n);
void func_00340310(s32 rx, s32 ry, s32 rz, f32 *m);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);

/* Makes light set n the room's (unless it already is). */
void func_0067FEB0(u8 n) {
    LightSet *l;
    f32 m[16];
    Vec3 dir = D_006EC0D8;

    l = &D_003ACD80[n];
    if (D_003062BD[0] == n) {
        return;
    }
    if (n < 16 && l->used != 0) {
        D_00306210[0] = l->fogNear;
        D_00306214[0] = l->fogFar;
        D_00306218[0] = l->fogColor >> 16;
        D_00306219[0] = l->fogColor >> 8;
        D_0030621A[0] = l->fogColor;
        D_0030628C[0] = l->colorA >> 16;
        D_0030628D[0] = l->colorA >> 8;
        D_0030628E[0] = l->colorA;
        D_0030628F[0] = 0xFF;
        D_00306298[0] = l->colorB >> 16;
        D_00306299[0] = l->colorB >> 8;
        D_0030629A[0] = l->colorB;
        D_0030629B[0] = 0xFF;
        D_003062CC[0] = (u8)l->unk18;
        D_003062CE[0] = l->unk18 >> 16;
        func_00340310(l->rx, l->ry, 0, m);
        func_001913A0(D_003062C0, &dir.x, m);
        memcpy(D_003060DC, l->unk24, 0x10);
        memcpy(D_003060FC, l->unk34, 0x10);
        memcpy(D_003060EC, l->unk44, 0x10);
        D_0030624C[0] = l->unk54;
        D_00306250[0] = l->unk58;
        D_003062BF[0] = l->unk5C;
    }
    D_003062BD[0] = n;
}

/* Copies the room's light sets from f (version 2), takes three more bytes after them, and sets
   the first. */
void func_00680090(LightFile *f);
#ifdef NON_MATCHING   /* the n == 0 exit's layout and registers */
void func_00680090(LightFile *f) {
    s32 version;
    s32 i;
    s32 n;
    s32 *off;
    LightSet *l;
    u32 w;

    version = f->version;
    n = f->n;
    l = D_003ACD80;
    off = f->offs;
    memset(D_003ACD80, 0, sizeof(D_003ACD80));
    if (version == 2) {
        if (n == 0) {
            return;
        }
        for (i = 0; i < n; i++, off++, l++) {
            memcpy(l, (u8 *)f + *off, sizeof(LightSet));
        }
        memcpy(&w, (u8 *)off + n * sizeof(LightSet), 4);
        D_0030627E[0] = w;
        D_0030627C[0] = w >> 16;
        D_0030627D[0] = w >> 8;
        func_0067FEB0(0);
    }
}
#endif
