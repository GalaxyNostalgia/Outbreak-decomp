#include "common.h"
#include "enemy.h"
#include "thing.h"

/* A thing's entry in its type's table. */

s32 func_001B7950(Thing *t) {
    return D_00357500[t->kind].defs[t->def].unk6 != 0;
}

s8 func_001B7980(Thing *t) {
    return D_00357500[t->kind].defs[t->def].variant;
}

/* The room's sounds (up to 4, from the room data) and its reverb. */
typedef struct {
    u8 used;            /* 0x00 */
    u8 unk1;            /* 0x01 */
    u16 unk2;           /* 0x02: 0x8000 loops */
    u16 unk4;           /* 0x04: & 0xF its kind */
    u8 unk6[2];
    f32 pos[3];         /* 0x08 */
    u8 unk14[0x1C - 0x14];
} RoomSoundDef;         /* 0x1C */

typedef struct {
    s8 active;          /* 0x00 */
    s8 loop;            /* 0x01 */
    s8 unk2;            /* 0x02 */
    s8 kind;            /* 0x03 */
    s8 unk4[4];         /* 0x04 */
    u8 unk8[0x24 - 8];
    RoomSoundDef *def;  /* 0x24 */
    void *fn;           /* 0x28: per kind */
    f32 pos[3];         /* 0x2C */
    s16 unk38;          /* 0x38 */
    u8 unk3A[2];
} RoomSound;            /* 0x3C */

extern RoomSound D_003938F0[4];
extern RoomSoundDef D_00394EA0[4];
extern void *D_002170D0[];
extern s32 D_00394F14[];    /* [0]: reverb depth (0: 0x300) */
extern u8 D_00394F13[];     /* [0]: reverb delay */
extern u8 D_00394F18[];     /* [0]: reverb feedback */
s32 func_001AC220(u32 core, u32 mode, s32 depth, s32 delay, s32 feedback);

s32 func_001B7D60(void);
#ifdef NON_MATCHING   /* first draft */
s32 func_001B7D60(void) {
    RoomSound *s;
    RoomSoundDef *d;
    s32 depth;
    s32 i;

    for (i = 0; i < 4; i++) {
        s = &D_003938F0[i];
        d = &D_00394EA0[i];
        s->active = 0;
        s->unk4[3] = 0;
        s->unk4[2] = 0;
        s->unk4[1] = 0;
        s->unk4[0] = 0;
        if (d->used != 0) {
            s->active = 1;
            s->loop = (d->unk2 & 0x8000) != 0;
            s->unk2 = d->unk2;
            s->kind = d->unk4 & 0xF;
            s->def = d;
            s->fn = D_002170D0[s->kind];
            s->unk38 = d->unk1;
            s->pos[0] = d->pos[0];
            s->pos[1] = d->pos[1];
            s->pos[2] = d->pos[2];
            if (s->fn == NULL) {
                for (;;) {
                }
            }
        }
    }
    depth = D_00394F14[0];
    if (depth == 0) {
        depth = 0x300;
    }
    func_001AC220(1, 4, depth, D_00394F13[0], D_00394F18[0]);
    return 0;
}
#endif

/* Once a frame: the room's sounds. */
void func_001B7F70(void) {
    RoomSound *s;
    s32 i;

    for (s = D_003938F0, i = 0; i < 4; i++, s++) {
        if (s->active != 0) {
            ((void (*)(RoomSound *))s->fn)(s);
        }
    }
}
