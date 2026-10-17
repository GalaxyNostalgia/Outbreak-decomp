#include "common.h"
#include "object.h"
#include "options.h"

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

void func_0082C640(DrawObjFull *obj);
void func_0082BE00(DrawObjFull *obj);
void func_00637240(DrawObjFull *obj, u8 a);
void func_006CCDA0(DrawObjFull *obj);

/* A partner entering (in our scenario's room) or leaving the room; mode 1 also resets its AI
   (func_006CCDA0). */
void func_00822150(DrawObjFull *obj, s32 mode) {
    u8 old = obj->unk105B;

    if (obj->unk1E2 != D_00306590.unk1C) {
        obj->unk105B = 1;
        if (old == 0) {
            FIELD(obj, s32, 0x1060) = 0x1C2;
            if (!(obj->unkBA0 & 0x20)) {
                func_0082C640(obj);
            }
            func_0082BE00(obj);
        }
    } else {
        obj->unk105B = 0;
        if (old == 1) {
            if (mode == 0) {
                if (!(obj->unkBA0 & 0x20)) {
                    func_0082C640(obj);
                }
                func_00637240(obj, obj->unk54F);
            } else {
                func_0082A6A0(obj, old);
            }
        }
    }
    if (mode == 1) {
        func_006CCDA0(obj);
        if (FIELD(obj, s8, 0x105A) == 1) {
            func_0082A690(obj, 0);
        }
    }
    obj->unk1064 = 1;
    obj->unk103A = 0;
    obj->unkBD0 = 0;
    obj->unkBA0 &= 0xDBD7FFFF;
    obj->unk548 &= ~1;
    obj->unkB90 = 0.0f;
    obj->unk520 = 0;
}
