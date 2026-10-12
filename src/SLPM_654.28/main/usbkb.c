#include "common.h"

/* USB keyboard set-up (for chat), on Sony's libusbkb. Started at boot; run one step per call,
   returns 0 once done (with or without a keyboard). */

typedef struct {
    s32 maxConnect;
    s32 nowConnect;
    u8 status[8];
} UsbKbInfo;

s32 func_001D8230(s32 *max);                    /* sceUsbKbInit */
s32 func_001D84F0(UsbKbInfo *info);             /* sceUsbKbGetInfo */
s32 func_001D8830(s32 no, s32 mode);
s32 func_001D8930(s32 no, s32 a, s32 b);
s32 func_001D8978(s32 no, s32 a);
s32 func_001D89C8(s32 no, s32 a);
s32 func_001D8A18(s32 mode, s32 *result);       /* sceUsbKbSync */
s32 func_001D9638(s32 no, s32 a);

extern s8 D_0023BC8C;       /* step */
extern s8 D_0023BC90;       /* library started */
extern s8 D_0023BC94;       /* a keyboard is ready */
extern s16 D_0023BC88;
extern s32 D_0023BC9C;
extern s32 D_0023BCA0;
extern s32 D_0023BCA4;
extern UsbKbInfo D_00322630;

/* Start the library: done (0) if it can't or no keyboard can connect. */
s32 func_001D7E20(s8 *step) {
    s32 max;

    if (D_0023BC90 == 0) {
        if (func_001D8230(&max) != 0) {
            *step = 0;
            return 0;
        }
        if (max == 0) {
            *step = 0;
            return 0;
        }
        D_0023BC9C = 0;
        D_0023BC90 = 1;
        (*step)++;
        return 1;
    }
    *step = 0;
    return 0;
}

/* Is a keyboard plugged in? */
s32 func_001D7EA0(s8 *step) {
    s32 r;

    r = func_001D84F0(&D_00322630);
    if (r != 0) {
        *step = 0;
        return 0;
    }
    D_0023BC9C = func_001D8A18(0, &r);
    if (D_0023BC9C == 0) {
        if (r == 0) {
            if (D_00322630.nowConnect == 0) {
                *step = 0;
                return 0;
            }
            (*step)++;
        }
    }
    return 1;
}

/* Set the keyboard up. */
s32 func_001D7F40(s8 *step) {
    s32 a, b, c, d;

    if (D_00322630.status[0] == 0) {
        *step = 0;
        return 0;
    }
    a = func_001D8930(0, 0, 0);
    b = func_001D8978(0, 0);
    D_0023BCA4 = 1;
    c = func_001D89C8(0, 1);
    D_0023BCA0 = 0;
    d = func_001D9638(0, 0);
    if (a == 0 && b == 0 && c == 0 && d == 0) {
        (*step)++;
    }
    return 1;
}

s32 func_001D8010(s8 *step) {
    s32 r;

    r = func_001D8830(0, 2);
    if (r == 0) {
        D_0023BC9C = func_001D8A18(0, &r);
        if (D_0023BC9C == 0 && r == 0) {
            (*step)++;
        }
    }
    return 1;
}

/* Ready. */
s32 func_001D8080(s8 *step) {
    *step = 0;
    D_0023BC88 = 1;
    D_0023BC94 = 1;
    return 0;
}

typedef s32 (*KbStep)(s8 *step);
extern KbStep D_00219060[];     /* func_001D7E00, func_001D7E20, ... */

s32 func_001D7DD0(void) {
    return D_00219060[D_0023BC8C](&D_0023BC8C);
}

s32 func_001D7E00(s8 *step) {
    (*step)++;
    return 1;
}
