#include "common.h"
#include "options.h"

/* Loading a scenario's files: its sound bank, environment and movie archives, room scripts. */

extern char D_00238638[];   /* "s%02d_com" */
extern char D_002319D0[];   /* "s%02denv.afs" */
extern char D_002319E0[];   /* "movie%03d.afs" */
extern char D_00238650[];   /* "%s/r%03d.evb" */
extern char D_00238660[];   /* "data/rom/scene/script" */
extern char D_00238678[];   /* "r%03d.evb" */
extern char D_00238688[];   /* "%s/r%03d.sgl" */
extern char D_002386A0[];   /* "data/rom/scene/single" */
extern char D_002386B8[];   /* "r%03d.sgl" */

s32 sprintf(char *buf, const char *fmt, ...);
void func_001D6EF0(const char *name, u32 slot);
s32 func_001A8E10(const char *name, void *part, s32 a, s32 b);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
s32 func_001A96E0(const char *name, void *buf);
s32 func_001AF4D0(void);

/* Left empty in the release. */
void func_001AB970(u16 scenario) {
}

/* The scenario's common sound bank. */
void func_001D6E30(u16 scenario) {
    char name[0x80];

    sprintf(name, D_00238638, D_00306590.unk1A);
    func_001D6EF0(name, 1);
}

extern u8 D_00294C80[];

/* The scenario's environment archive. */
void func_001AA600(s32 scenario) {
    char name[0x10];

    sprintf(name, D_002319D0, scenario);
    func_001A8E10(name, D_00294C80, 4, 1);
}

extern u8 D_0035C580[];         /* (SUBMAIN.BIN) */

/* The scenario's movie archive. */
void func_001AA640(void) {
    char name[0x10];

    sprintf(name, D_002319E0, D_00306590.unk1A);
    func_001A8E10(name, D_0035C580, 2, 2);
}

void func_0065A370(void);
void func_0065A2B0(void);

/* The scenario's room script, from the disc or the hard disk. */
void func_001D6FC0(void) {
    char name[0x80];
    Options *opt = &D_00306590;
    u8 *buf = D_00306590.script;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238650, D_00238660, opt->unk1A);
        if (func_001CFBB0(name, buf, 1, 0) > 0) {
            func_0065A370();
        } else {
            func_0065A2B0();
        }
    } else {
        sprintf(name, D_00238678, opt->unk1A);
        if (func_001A96E0(name, buf) != 0) {
            func_0065A370();
        } else {
            func_0065A2B0();
        }
    }
}

extern s32 D_008534C0[];        /* (SINGLE.BIN) the offline script */
void func_008263B0(void);
void func_00825E30(s32 a);

/* Offline: the scenario's single-player script. */
void func_001D7090(void) {
    char name[0x80];
    Options *opt = &D_00306590;
    s32 *buf;
    s32 n;

    if (func_001AF4D0() == 0) {
        return;
    }
    buf = D_008534C0;
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238688, D_002386A0, opt->unk1A);
        n = func_001CFBB0(name, buf, 1, 0);
    } else {
        sprintf(name, D_002386B8, opt->unk1A);
        n = func_001A96E0(name, buf);
    }
    if (n <= 0) {
        buf[0] = 0;
        return;
    }
    func_008263B0();
    func_00825E30(2);
}

/* The room's NPC placements (r%03d.npc). */

typedef struct {
    s32 a;              /* 0x00 */
    s32 b;
    s32 c;
    s32 type;           /* 0x0C */
    f32 pos[3];         /* 0x10 */
    s32 unk1C;
    s32 d;              /* 0x20 */
    s32 mode;           /* 0x24: 0-2 */
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34[4];       /* 0x34 */
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
} NpcEntry;

typedef struct {
    s8 a;               /* 0x00 */
    s8 b;
    s8 c;
    s8 d;
    u16 type;           /* 0x04 */
    s8 mode;            /* 0x06 */
    u8 unk7;
    f32 pos[3];         /* 0x08 */
    s16 unk14;
    s16 unk16;
    u8 unk18[2];
    s16 unk1A;          /* 0x1A */
    u8 unk1C[2];
    s8 unk1E;
    s8 unk1F;
    u8 unk20[2];
    s16 unk22;
    u8 unk24[4];
    s32 unk28;          /* 0x28 */
    u8 unk2C[4];
    s32 unk30;          /* 0x30 */
    s8 unk34;
    u8 unk35[0x54 - 0x35];
    s16 unk54;          /* 0x54 */
    s16 unk56;
    s8 unk58;
    u8 unk59[3];
    s32 unk5C[4];       /* 0x5C */
    s32 unk6C;
    u8 unk70[8];
} NpcSet;               /* 0x78 */

extern char D_002388D0[];   /* "%s/r%03d.npc" */
extern char D_002388E0[];   /* "data/rom/scene/npc_set" */
extern char D_002388F8[];   /* "r%03d.npc" */
extern NpcSet D_003614F0[0x40];
void func_0018D630(void *dst, void *src, s32 size);

void func_001D7720(void);
#ifdef NON_MATCHING   /* first draft */
void func_001D7720(void) {
    char name[0x100];
    u32 *file = (u32 *)D_00306590.unk4C;
    u32 *grp;
    u32 ngroups;
    u32 count;
    u32 g;
    u32 j;
    u32 k;
    NpcEntry *e;
    NpcSet *s;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_002388D0, D_002388E0, D_00306590.unk1A);
        if (func_001CFBB0(name, file, 1, 0) <= 0) {
            return;
        }
    } else {
        sprintf(name, D_002388F8, D_00306590.unk1A);
        if (func_001A96E0(name, file) == 0) {
            return;
        }
    }
    k = 0;
    ngroups = (file[0] >> 1) >> 2;
    for (g = 0; g < ngroups; g++) {
        grp = (u32 *)((u8 *)file + file[g * 2]);
        count = grp[0];
        for (j = 0; j < count; j++, k++) {
            if (k >= 0x40) {
                break;
            }
            e = (NpcEntry *)((u8 *)grp + grp[1 + j]);
            s = &D_003614F0[k];
            s->a = e->a;
            s->b = e->b;
            s->c = e->c;
            s->d = e->d;
            s->type = e->type;
            s->mode = e->mode;
            s->pos[0] = e->pos[0];
            s->pos[1] = e->pos[1];
            s->pos[2] = e->pos[2];
            s->unk14 = e->unk1C;
            s->unk16 = e->unk48;
            s->unk22 = e->unk4C;
            s->unk1E = 0;
            s->unk34 = e->unk50;
            s->unk28 = -1;
            s->unk30 = -1;
            if (s->type != 0x1D) {
                e->mode = 0;
                s->mode = 0;
            }
            switch (e->mode) {
            case 0:
                s->unk54 = e->unk2C;
                s->unk56 = e->unk30;
                s->unk58 = e->unk28;
                s->unk1F = e->unk34[0];
                break;
            case 1:
                s->unk58 = e->unk28;
                break;
            case 2:
                func_0018D630(s->unk5C, e->unk34, 4);
                s->unk6C = e->unk44;
                s->unk58 = e->unk28;
                s->unk1A = s->unk5C[0];
                break;
            }
        }
    }
}
#endif
