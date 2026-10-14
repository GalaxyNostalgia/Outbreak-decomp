#include "common.h"
#include "ps2mem.h"

/* The renderer's state: func_00195970(cmd, arg) sets one piece of it. arg is a value, a float's bits
   or a pointer, depending on cmd. Matrices live in the scratchpad (0x70003700-0x70003FFF). */


typedef struct {
    u32 w[19];
} Light;            /* 0x4C */

typedef struct {
    u8 unk0[0x10];
    s32 unk10;      /* 0x10 */
    u8 unk14[0x20 - 0x14];
    s32 unk20;      /* 0x20 */
    u8 unk24[0x34 - 0x24];
    u8 unk34[0x50 - 0x34];
    f32 unk50;      /* 0x50 */
    u8 unk54[0x68 - 0x54];
} Fog;              /* 0x68 */

extern u32 D_0023BA90;      /* render flags */
extern u32 D_0023BA8C;      /* GS state flags */
extern u32 D_0023BA50;      /* cmd 4-7: four values */
extern u32 D_0023BA58;
extern u32 D_0023BA5C;
extern u32 D_0023BA74;
extern u32 D_0023BA78;      /* colour (cmd 0x67) */
extern u32 D_0023BA7C;      /* colour (cmd 0x0E) */
extern u32 D_0023BA80;
extern f32 D_0023BA84;
extern f32 D_0023BA88;
extern u32 D_0023BAA8;
extern Light D_002767A0[32];
extern Fog D_00276660[3];
extern f32 D_00276648[3];
extern f32 D_002772A0[4];
extern f32 D_002772B0[4];
extern u8 D_002771A0[], D_002771E0[], D_00277220[];

void func_00191750(void *p);
void func_0019CD60(s32 mode, u32 *v);
s32 func_00197BE0(u32 v);
void func_00197330(u32 flags, s32 unused);
void func_00197A50(u32 v);
void func_00196C00(u32 v);
void func_00190C50(void *out, void *a, void *b);
void func_0018F2B0(s32 id, s32 a, s32 b, s32 c, s32 d, f32 e, f32 f);
void func_00197080(u32 flags, s32 unused);
void func_00197B10(u32 flags, s32 unused);
void func_00196F00(u32 flags, s32 unused);

#define SPR_MATRIX 0x70003700   /* 32 matrices */

s32 func_00195970(u8 cmd, u32 arg) {
    s32 n;
    s32 i;
    u32 v;
    u32 a;

    if (cmd >= 0x1A && cmd < 0x3A) {
        if (arg != 0) {
            i = cmd - 0x1A;
            ((Mat44 *)PS2_SPR(SPR_MATRIX))[i] = *(Mat44 *)arg;
        }
        return 1;
    }
    if (cmd >= 0x3A && cmd < 0x5A) {
        i = cmd - 0x3A;
        D_002767A0[i] = *(Light *)arg;
        return 1;
    }
    if (cmd >= 0x5A && cmd < 0x5D) {
        n = cmd - 0x5A;
        D_00276660[n] = *(Fog *)arg;
        func_00191750(D_00276660[n].unk34);
        D_00276660[n].unk10 = 0;
        D_00276660[n].unk20 = 0;
        D_00276648[n] = 1.0f / D_00276660[n].unk50;
        return 1;
    }
    switch (cmd) {
    case 0x00:
        D_0023BA90 &= ~0x60;
        D_0023BA90 |= arg;
        break;
    case 0x03:
        break;
    case 0x01:
        D_0023BA90 &= ~0xF00;
        switch (arg) {
        case 0x200:
            D_0023BA90 |= 0x200;
            break;
        case 0x100:
            D_0023BA90 |= 0x100;
            break;
        case 0x300:
            D_0023BA90 |= 0x300;
            break;
        case 0x400:
            D_0023BA90 |= 0x400;
            break;
        case 0x500:
            D_0023BA90 |= 0x500;
            break;
        case 0x600:
            D_0023BA90 |= 0x600;
            break;
        case 0x700:
            D_0023BA90 |= 0x700;
            break;
        case 0x800:
            D_0023BA90 |= 0x800;
            break;
        case 0:
            break;
        }
        break;
    case 0x02:
        D_0023BA90 &= ~0x1C;
        if (arg == 0) {
            break;
        }
        switch (arg) {
        case 0x04:
        case 0x08:
        case 0x0C:
        case 0x10:
        case 0x14:
        case 0x18:
            D_0023BA90 |= arg;
            break;
        }
        break;
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
        v = arg;
        if (cmd == 4) {
            func_0019CD60(1, &v);
            func_00197BE0(v);
            D_0023BAA8 = v;
        }
        i = cmd - 4;
        (&D_0023BA50)[i] = v;
        break;
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
        break;
    case 0x0C:
        D_0023BA90 &= ~0x80;
        if (arg == 1) {
            D_0023BA90 |= 0x80;
        }
        break;
    case 0x0D:
        if ((D_0023BA8C & 0xC00) != arg) {
            D_0023BA8C &= ~0xC00;
            D_0023BA8C |= arg;
            func_00197330(D_0023BA8C, 0);
        }
        break;
    case 0x5E:
        if ((D_0023BA8C & 0xFF) != arg) {
            D_0023BA8C &= ~0xFF;
            D_0023BA8C |= arg;
            func_00197330(D_0023BA8C, 0);
        }
        break;
    case 0x61:
        if ((D_0023BA8C & 0x1000000) != arg) {
            D_0023BA8C &= ~0x1000000;
            D_0023BA8C |= arg;
            func_00197330(D_0023BA8C, 0);
        }
        break;
    case 0x0E:
        D_0023BA7C = arg;
        a = *(volatile u32 *)&D_0023BA7C;
        D_002772B0[0] = ((a >> 16) & 0xFF) / 255.0f;
        D_002772B0[1] = ((a >> 8) & 0xFF) / 255.0f;
        D_002772B0[2] = (a & 0xFF) / 255.0f;
        a = (a >> 24) & 0xFF;
        if (a == 0xFF) {
            D_002772B0[3] = 128.0f;
        } else if (a != 0) {
            a >>= 1;
            if (a == 0) {
                a = 1;
            }
            D_002772B0[3] = a;
        } else {
            D_002772B0[3] = 0.0f;
        }
        break;
    case 0x0F:
        if (D_0023BA80 != arg) {
            D_0023BA80 = arg;
            func_00197A50(arg);
        }
        break;
    case 0x10:
        D_0023BA88 = *(f32 *)&arg;
        break;
    case 0x11:
        D_0023BA84 = *(f32 *)&arg;
        break;
    case 0x12:
        D_0023BA90 &= ~0x18000;
        if (arg == 0x8000) {
            D_0023BA90 |= 0x8000;
        }
        break;
    case 0x13:
        break;
    case 0x14:
        func_00196C00(arg);
        break;
    case 0x15:
        D_0023BA90 &= ~3;
        if (arg != 0 && arg == 2) {
            D_0023BA90 |= 2;
        }
        break;
    case 0x16:
        *(Mat44 *)PS2_SPR(0x70003F40) = *(Mat44 *)arg;
        break;
    case 0x17:
        *(Mat44 *)PS2_SPR(0x70003F00) = *(Mat44 *)arg;
        func_00190C50(D_002771A0, PS2_SPR(0x70003F00), PS2_SPR(0x70003F80));
        func_00190C50(D_002771E0, PS2_SPR(0x70003F00), D_00277220);
        break;
    case 0x18:
        func_0018F2B0(0x22, ((s32 *)arg)[0], ((s32 *)arg)[1], ((s32 *)arg)[2], ((s32 *)arg)[3],
                      ((f32 *)arg)[4], ((f32 *)arg)[5]);
        break;
    case 0x19:
        *(Mat44 *)PS2_SPR(0x70003FC0) = *(Mat44 *)arg;
        break;
    case 0x5D:
        D_0023BA90 &= 0xFFFFFF;
        D_0023BA90 |= arg;
        break;
    case 0x5F:
        switch (arg) {
        case 2:
            arg = 0x100000;
            break;
        case 5:
            arg = 0x280000;
            break;
        case 4:
            arg = 0x200000;
            break;
        case 6:
            arg = 0x300000;
            break;
        case 0:
            arg = 0;
            break;
        case 7:
            arg = 0x380000;
            break;
        case 1:
            arg = 0x80000;
            break;
        case 3:
            arg = 0x180000;
            break;
        }
        if ((D_0023BA8C & 0x380000) != arg) {
            D_0023BA8C &= ~0x380000;
            D_0023BA8C |= arg;
            func_00197080(D_0023BA8C, 0);
        }
        break;
    case 0x60:
        if (arg != D_0023BA74) {
            D_0023BA74 = arg;
            func_00197080(D_0023BA8C, 0);
        }
        break;
    case 0x62:
        D_0023BA90 &= ~0x4000;
        if (arg != 0) {
            D_0023BA90 |= 0x4000;
        }
        break;
    case 0x63:
        D_0023BA8C &= ~0x10000;
        if (arg == 0x10000) {
            D_0023BA8C |= 0x10000;
        }
        func_00197B10(D_0023BA8C, 0);
        break;
    case 0x64:
        D_0023BA8C &= ~0x60000;
        if (arg != 0 && arg == 0x20000) {
            D_0023BA8C |= 0x20000;
        }
        break;
    case 0x65:
        break;
    case 0x66:
        D_0023BA90 &= ~0x20000;
        if (arg == 1) {
            D_0023BA90 |= 0x20000;
        }
        break;
    case 0x67:
        D_0023BA78 = arg;
        D_002772A0[0] = ((arg >> 16) & 0xFF) / 255.0f;
        D_002772A0[1] = ((arg >> 8) & 0xFF) / 255.0f;
        D_002772A0[2] = (arg & 0xFF) / 255.0f;
        D_002772A0[3] = ((arg >> 24) & 0xFF) / 255.0f;
        break;
    case 0x68:
        break;
    case 0x69:
        D_0023BA5C = arg;
        break;
    case 0x6A:
        D_0023BA58 = arg;
        break;
    case 0x6B:
        if ((D_0023BA8C & 0xC00000) != arg) {
            D_0023BA8C &= ~0xC00000;
            D_0023BA8C |= arg;
        }
        break;
    case 0x6C:
        arg = (arg == 1) ? 0x8000 : 0;
        if ((D_0023BA8C & 0x8000) != arg) {
            D_0023BA8C &= ~0x8000;
            D_0023BA8C |= arg;
            func_00196F00(D_0023BA8C, 0);
        }
        break;
    case 0x6D:
        switch (arg) {
        case 0:
            arg = 0;
            break;
        case 7:
            arg = 0x7000;
            break;
        case 1:
            arg = 0x1000;
            break;
        case 3:
            arg = 0x3000;
            break;
        case 2:
        case 4:
        case 5:
        case 6:
            break;
        }
        if ((D_0023BA8C & 0x7000) != arg) {
            D_0023BA8C &= ~0x7000;
            D_0023BA8C |= arg;
            func_00197080(D_0023BA8C, 0);
        }
        break;
    }
    return 1;
}

extern u32 D_00282150[];

s32 func_00196C10(s32 cmd);

s32 func_001966F0(u8 cmd);
#ifdef NON_MATCHING   /* the switch is laid out differently */
/* Reads back a piece of the renderer's state (the commands of func_00195970). */
s32 func_001966F0(u8 cmd) {
    s32 r = 0;

    switch (cmd) {
    case 0x01:
        return D_0023BA90 & 0xF00;
    case 0x02:
        return D_0023BA90 & 0x1C;
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
        return (&D_0023BA50)[cmd - 4];
    case 0x0C:
        if ((D_0023BA90 & 0x80) == 0x80) {
            r = 1;
        }
        break;
    case 0x0D:
        return D_0023BA8C & 0xC00;
    case 0x0E:
        return D_0023BA7C;
    case 0x0F:
        return D_0023BA80;
    case 0x12:
        if ((D_0023BA90 & 0x18000) == 0x8000) {
            r = 1;
        }
        break;
    case 0x14:
        return func_00196C10(0x14);
    case 0x15:
        r = D_0023BA90 & 3;
        if (r == 0) {
            return 0;
        }
        if (r == 2) {
            return 2;
        }
        break;
    case 0x5A:
    case 0x5B:
    case 0x5C:
        return (s32)&D_00276660[cmd - 0x5A];
    case 0x5D:
        return D_0023BA90 & 0xFF000000;
    case 0x5E:
        return (u8)D_0023BA8C;
    case 0x5F:
        switch (D_0023BA8C & 0x380000) {
        case 0x080000:
            return 1;
        case 0x100000:
            return 2;
        case 0x180000:
            return 3;
        case 0x200000:
            return 4;
        case 0x280000:
            return 5;
        case 0x300000:
            return 6;
        case 0x380000:
            r = 7;
            break;
        default:
            return 0;
        }
        break;
    case 0x60:
        return D_0023BA74;
    case 0x61:
        if ((D_0023BA8C & 0x01000000) == 0x01000000) {
            r = 1;
        }
        break;
    case 0x62:
        if ((D_0023BA90 & 0x4000) == 0x4000) {
            r = 1;
        }
        break;
    case 0x63:
        return D_0023BA8C & 0x10000;
    case 0x64:
        return D_0023BA8C & 0x60000;
    case 0x66:
        if ((D_0023BA90 & 0x20000) == 0x20000) {
            r = 1;
        }
        break;
    case 0x69:
        return D_0023BA5C;
    case 0x6A:
        return D_0023BA58;
    case 0x6B:
        return D_0023BA8C & 0xC00000;
    case 0x6C:
        if ((D_0023BA8C & 0x8000) == 0x8000) {
            r = 1;
        }
        break;
    case 0x6D:
        switch (D_0023BA8C & 0x7000) {
        case 0x1000:
            return 1;
        case 0x2000:
            return 2;
        case 0x3000:
            return 3;
        case 0x4000:
            return 4;
        case 0x5000:
            return 5;
        case 0x6000:
            return 6;
        case 0x7000:
            r = 7;
            break;
        default:
            return 0;
        }
        break;
    case 0x00:
    case 0x03:
    case 0x08:
    case 0x09:
    case 0x0A:
    case 0x0B:
    case 0x10:
    case 0x11:
    case 0x13:
    case 0x65:
    case 0x68:
        break;
    default:
        return 0;
    }
    return r;
}
#endif

void func_00196C00(u32 v) {
    D_00282150[0] = v;
}
