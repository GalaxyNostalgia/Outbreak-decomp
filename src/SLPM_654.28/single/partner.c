#include "common.h"
#include "object.h"

/* SINGLE.BIN: the computer-controlled partners' AI state. */

typedef struct {
    u8 unk0[8];         /* 0x00: copied to the partner's unk10B0 */
    u8 unk8[0x38 - 8];
} PartnerMode;          /* 0x38 */

extern PartnerMode D_00838860[0x14];
extern u8 D_008534C0[];

void func_0064F690(u8 type, s16 id);
void func_0064FB70(s32 type, s32 id, u8 *b, s32 player);
void func_00107B50(void *dst, const void *src, s32 n);
void *memset(void *p, s32 c, u32 n);
void func_00825DB0(DrawObjFull *obj, s8 v);
void func_00823340(DrawObjFull *obj, u16 v);
void func_0082A690(DrawObjFull *obj, u16 v);
void func_008223C0(DrawObjFull *obj, s32 mode);

/* A partner's AI from the start (players that are partners only). */
void func_008220A0(DrawObjFull *obj) {
    D_008534C0[0] = 0;
    if (obj->isNpc != 0) {
        func_0064F690(0, obj->kind);
        func_00825DB0(obj, 0);
        obj->unk105B = 0xFF;
        obj->unk1064 = 0;
        obj->unkFF1 = 0;
        obj->unk1098 = 0;
        obj->unk1099 = 0;
        obj->unk109A = 0;
        obj->unk107C = 0xFFFF;
        obj->unk1083 = 0;
        obj->unk1072 = 0x3C;
        obj->unk1034 = 0;
        obj->unk109C = -1;
        func_00823340(obj, 0);
        func_0082A690(obj, 0);
        func_008223C0(obj, 0);
    }
}

/* Its mode (one of 0x14). */
void func_008223C0(DrawObjFull *obj, s32 mode) {
    PartnerMode *m;

    if (mode < 0x14) {
        obj->mode = mode;
        obj->modeInfo = m = &D_00838860[mode];
        func_00107B50(obj->unk10B0, m, 8);
        memset(obj->unk10B8, 0, 8);
        memset(obj->unk10C0, 0, 8);
    }
}

void func_00823340(DrawObjFull *obj, u16 v) {
    s32 i;

    for (i = 0; i < 4; i++) {
        obj->unkFFC[i] = 0;
        obj->unk1000[i] = 0;
    }
    if (v != 0) {
        obj->unk1074 = 0;
    } else {
        obj->unk1074 = 1;
        obj->unkFF6 = obj->unkFF4;
    }
    obj->unkFF4 = v;
}

void func_0082A6A0(DrawObjFull *obj, u16 v);

void func_0082A690(DrawObjFull *obj, u16 v) {
    obj->unkFFA = obj->unkFF8;
    obj->unkFF8 = v;
    func_0082A6A0(obj, v);
}

void func_0082A6A0(DrawObjFull *obj, u16 v) {
    s32 i;

    for (i = 0; i < 4; i++) {
        obj->unk1008[i] = 0;
    }
    obj->unk103A = 0;
}

#ifdef NON_MATCHING   /* argument scheduling */
void func_00825DB0(DrawObjFull *obj, s8 v) {
    u8 b[4];

    b[0] = v;
    b[1] = 0;
    b[2] = 0;
    b[3] = 0;
    func_0064FB70(0, obj->kind, b, -1);
}
#endif
