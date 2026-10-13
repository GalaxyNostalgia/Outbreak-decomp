#include "common.h"
#include "options.h"

/* The enemy sound bank: enemy.tq/enemy.tsb, then the sounds of up to four of the room's enemy
   types, one step a call. */

void *memset(void *p, s32 c, u32 n);
s32 sprintf(char *dst, const char *fmt, ...);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
u16 func_001ADEF0(s32 n);
void func_001AC6B0(u8 *src, s32 size, s32 size2);
void func_001AC750(u8 *seqs, u8 *waves);
void func_001AC780(u8 *snd);

extern s8 D_0023BB48[5];    /* [0]: the step */
extern char D_00231ED8[];   /* "%senemy.tq" */
extern char D_00231EF0[];   /* "data\\rom\\sound\\ps2\\snd\\" */
extern char D_00231F08[];   /* "%senemy.tsb" */
extern char D_00231F18[];   /* "%s%s" */
extern char *D_00358254[];  /* by type (from 0x3A) */
extern char *D_0035831C[];  /* by type */
extern char *D_00358324[]; /* type 3: one of two */
extern char *D_0035832C[]; /* type 4 */
extern char *D_00358334[]; /* type 2 */

void func_001B6A40(void) {
    memset(D_0023BB48, 0, sizeof(D_0023BB48));
}

/* 0 more to do, 1 done, -1 failed. */
s32 func_001B6A50(u8 *types);
#ifdef NON_MATCHING   /* the 0x8003F adds are emitted as lui/addu/addiu in the original */
s32 func_001B6A50(u8 *types) {
    char name[0x100];
    s32 n2;
    u32 t;
    s32 r = 0;
    char *p;
    s32 n;
    u8 *buf;

    switch (D_0023BB48[0]) {
    case 0:
        if (types[0] == 0) {
            return -1;
        }
        func_001AC750((u8 *)((u32)((u8 *)D_00306590.unk4C + 0x80000 + 0x3F) & ~0x3F),
                      (u8 *)((u32)((u8 *)D_00306590.unk4C + 0xC0000 + 0x3F) & ~0x3F));
        sprintf(name, D_00231ED8, D_00231EF0);
        p = name;
        buf = (u8 *)((u32)((u8 *)D_00306590.unk4C + 0x4000) + 0x3F & ~0x3F);
        n = func_001CFBB0(p, buf, 1, 0);
        if (n <= 0) {
            return -1;
        }
        sprintf(p, D_00231F08, D_00231EF0);
        n2 = func_001CFBB0(p, buf + n, 1, 0);
        if (n2 <= 0) {
            return -1;
        }
        func_001AC6B0(buf, n, n2);
        D_0023BB48[0]++;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        t = types[D_0023BB48[0] - 1];
        if (t == 0) {
            D_0023BB48[0]++;
            break;
        }
        if (t >= 0x3A) {
            sprintf(name, D_00231F18, D_00231EF0, D_00358254[t]);
        } else if (t == 2) {
            sprintf(name, D_00231F18, D_00231EF0, D_00358334[func_001ADEF0(1) & 1]);
        } else if (t == 3) {
            sprintf(name, D_00231F18, D_00231EF0, D_00358324[func_001ADEF0(1) & 1]);
        } else if (t == 4) {
            sprintf(name, D_00231F18, D_00231EF0, D_0035832C[func_001ADEF0(1) & 1]);
        } else {
            sprintf(name, D_00231F18, D_00231EF0, D_0035831C[t]);
        }
        buf = (u8 *)D_00306590.unk4C + 0x4000;
        if (func_001CFBB0(name, buf, 1, 0) <= 0) {
            return -1;
        }
        func_001AC780(buf);
        D_0023BB48[0]++;
        break;
    default:
        r = 1;
        break;
    }
    return r;
}
#endif
