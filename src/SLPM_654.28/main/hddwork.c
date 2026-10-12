#include "common.h"
#include "options.h"
#include "cri.h"

/* Work area for the boot-time HDD/network setup screens. */
typedef struct {
    u8 unk0;
    s8 state;           /* 0x01 */
    u8 step;            /* 0x02 */
    u8 unk3[5];
    s16 held;           /* 0x08 */
    u16 pressed;        /* 0x0A */
    s16 trigger;        /* 0x0C */
    u8 unkE[2];
    s32 unk10;
    s32 unk14;
    u8 unk18[4];
    s32 kind;           /* 0x1C */
    s32 mode;           /* 0x20 */
    s32 unk24;
    s32 unk28;
    u8 unk2C[0x40 - 0x2C];
    s32 loaded;         /* 0x40 */
    u8 unk44[0xD4 - 0x44];
    s32 x;              /* 0xD4 */
    s32 y;              /* 0xD8 */
    s32 wide;           /* 0xDC */
    s32 unkE0;
    s32 buf0;           /* 0xE4: 0x100 bytes */
    s32 buf1;           /* 0xE8: 0x40000 bytes */
    s32 buf2;           /* 0xEC: 0x40000 bytes */
    s32 buf3;           /* 0xF0 */
} HddWork;

extern HddWork *D_0023BBE0;
extern s32 D_003065DC[];        /* (Options) work memory */
extern s32 D_00308D80[];
extern s32 D_00308D84[];
extern s32 D_00308E18[];
extern s32 D_00308E20[];

void *memset(void *dst, s32 c, u32 n);

void func_001BE6C0(s32 a) {
    D_00308E20[0] = a;
    D_00308D80[0] = 0;
    D_00308D84[0] = 0;
    D_00308E18[0] = 0;
}

void func_001C0940(s32 mode, s32 kind) {
    if (D_00306590.unk0) {
        D_0023BBE0 = (HddWork *)(D_003065DC[0] + 0xA0000);
    } else {
        D_0023BBE0 = (HddWork *)(D_003065DC[0] + 0x2BF000);
    }
    memset(D_0023BBE0, 0, sizeof(HddWork));
    D_0023BBE0->buf0 = (s32)D_0023BBE0;
    D_0023BBE0->buf1 = D_0023BBE0->buf0 + 0x100;
    D_0023BBE0->buf2 = D_0023BBE0->buf1 + 0x40000;
    D_0023BBE0->buf3 = D_0023BBE0->buf2 + 0x40000;
    D_0023BBE0->mode = mode;
    D_0023BBE0->kind = kind;
    func_001BE6C0(0);
    switch (D_0023BBE0->mode) {
    case 0:
        switch (D_0023BBE0->kind) {
        case 0:
            D_0023BBE0->x = 0x30;
            D_0023BBE0->y = 0x120;
            break;
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        case 3:
        case 4:
            break;
        }
        break;
    case 1:
        switch (D_0023BBE0->kind) {
        case 0:
        case 4:
            break;
        case 3:
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    case 2:
        switch (D_0023BBE0->kind) {
        case 0:
        case 3:
        case 4:
            break;
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    case 3:
        switch (D_0023BBE0->kind) {
        case 0:
        case 3:
            break;
        case 1:
        case 2:
        case 4:
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    case 4:
        switch (D_0023BBE0->kind) {
        case 0:
        case 3:
        case 4:
            break;
        case 1:
        case 2:
            D_0023BBE0->wide = 1;
            D_0023BBE0->x = 0x68;
            D_0023BBE0->y = 0x8C;
            break;
        }
        break;
    }
}

/* Loads a whole file from the disc through ADXF. */
extern s32 func_001BF610(s32 buf);

void func_001C3B00(HddWork *w, s32 buf, char *name) {
    ADXF adxf;

    while ((adxf = ADXF_Open(name, 0)) == 0) {
    }
    ADXF_ReadNw(adxf, ADXF_GetFsizeSct(adxf), (void *)buf);
    while (ADXF_GetStat(adxf) != 3) {
    }
    ADXF_Close(adxf);
    w->loaded = 1;
    w->unk28 = func_001BF610(buf);
}

/* Boot-time save-data screens: one step per frame, returns 0 when finished. */
extern s32 D_003063E0[];
extern s32 D_003063E8[];
extern s32 D_003063F8[];
extern u8 D_00306646[];

s32 func_001BF5B0(void);
void func_001BE6F0(void);
s32 func_001C0FD0(HddWork *w);
s32 func_001C1400(HddWork *w);
s32 func_001C1840(HddWork *w);
s32 func_001C1BF0(HddWork *w);
s32 func_001C1F00(HddWork *w);
s32 func_001C25F0(HddWork *w);
s32 func_001C2BC0(HddWork *w);
s32 func_001C2EE0(HddWork *w);
s32 func_001C3020(HddWork *w);

s32 func_001C0C20(void) {
    HddWork *w = D_0023BBE0;

    w->held = D_003063E8[0];
    w->pressed = D_003063E0[0];
    w->trigger = D_003063F8[0] | w->pressed;
    switch (w->state) {
    case 0:
        switch (w->mode) {
        case 0:
            switch (w->kind) {
            case 0:
                w->state = 1;
                break;
            default:
                return 0;
            }
            break;
        case 1:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 3;
                break;
            case 3:
                w->state = 5;
                break;
            default:
                return 0;
            }
            break;
        case 2:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 3;
                break;
            default:
                return 0;
            }
            break;
        case 3:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 6;
                break;
            case 4:
                w->state = 7;
                break;
            default:
                return 0;
            }
            break;
        case 4:
            switch (w->kind) {
            case 1:
                if (D_00306646[0] == 0) {
                    w->state = 2;
                } else {
                    w->state = 4;
                }
                break;
            case 2:
                w->state = 3;
                break;
            default:
                return 0;
            }
            break;
        }
        {
            char *name = "MCICON.BIN";
            w->unk10 = 0;
            w->unk14 = 0;
            w->unk24 = 0;
            func_001C3B00(w, w->buf3, name);
        }
        break;
    case 1:
        if (func_001C0FD0(w) == 0) {
            return 0;
        }
        break;
    case 2:
        if (func_001C1F00(w) == 0) {
        save_end:
            if (w->wide == 0) {
                return 0;
            }
            if (func_001BF5B0() == 0 && w->unkE0 == 0) {
                return 0;
            }
            w->state = 8;
            w->step = 0;
        }
        break;
    case 3:
        if (func_001C1400(w) == 0) {
            if (w->wide == 0) {
                return 0;
            }
            if (func_001BF5B0() == 0 && w->unkE0 == 0) {
                return 0;
            }
            w->state = 9;
            w->step = 0;
        }
        break;
    case 4:
        if (func_001C2BC0(w) == 0) {
            goto save_end;
        }
        break;
    case 5:
        if (func_001C25F0(w) == 0) {
            return 0;
        }
        break;
    case 6:
        if (func_001C1BF0(w) == 0) {
            return 0;
        }
        break;
    case 7:
        if (func_001C1840(w) == 0) {
            return 0;
        }
        break;
    case 8:
        if (func_001C2EE0(w) == 0) {
            return 0;
        }
        break;
    case 9:
        if (func_001C3020(w) == 0) {
            return 0;
        }
        break;
    }
    func_001BE6F0();
    return 1;
}

extern s32 D_00308DA4[];

s32 func_001BF5B0(void) {
    return D_00308DA4[0];
}
