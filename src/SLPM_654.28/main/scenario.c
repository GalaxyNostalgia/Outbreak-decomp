#include "common.h"
#include "options.h"
#include "cri.h"

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

/* Player p's voice archive, from the sound archive (partition 3) into partition p + 5; waits
   for it. 0 when the file isn't there. */
extern char D_00231A08[];   /* "vo001%02d.afs" */
s32 func_001AA5C0(const char *name);

typedef struct {
    u8 data[0x10C0];
} VoicePt;

extern VoicePt *D_00306638;     /* (Options.voice) */

s32 func_001AA720(s32 player, s32 kind);
#ifdef NON_MATCHING   /* player * 0x10C0 is a mult in the original */
s32 func_001AA720(s32 player, s32 kind) {
    char name[0x100];
    s32 fid;
    s32 pt;

    sprintf(name, D_00231A08, kind);
    fid = func_001AA5C0(name);
    if (fid < 0) {
        return 0;
    }
    pt = player + 5;
    ADXF_LoadPtFromAfsNw(pt, 3, fid, &D_00306638[player]);
    while (ADXF_GetPtStat(pt) != 3) {
    }
    return 1;
}
#endif

extern u8 D_0035C580[];         /* (SUBMAIN.BIN) */

/* The scenario's movie archive. */
void func_001AA640(void) {
    char name[0x10];

    sprintf(name, D_002319E0, D_00306590.unk1A);
    func_001A8E10(name, D_0035C580, 2, 2);
}

void func_0065A370(void);
void func_0065A2B0(void);

/* The room script, from loose files or (the release) the AFS archives. */
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

/* The character's weapon sound bank (slot kind + 8) and voice bank (slot kind + 4). */
#include "object.h"

extern char D_002385D8[];   /* "weapon%02d" */
extern char D_00238618[];   /* "pl%02d_voi" */
s8 func_00584790(u8 item);
void func_00682750(u8 kind, s32 weapon);

void func_001D6BA0(DrawObjFull *obj) {
    char name[0x80];
    s32 item = obj->unkC7C;
    s32 w;

    if (item != 0xFF) {
        w = func_00584790(item);
        if (w != -1) {
            func_00682750(obj->kind, w);
            sprintf(name, D_002385D8, w);
            func_001D6EF0(name, obj->kind + 8);
        }
    }
}

void func_001D6DD0(DrawObjFull *obj) {
    char name[0x80];

    sprintf(name, D_00238618, obj->unkB98);
    func_001D6EF0(name, obj->kind + 4);
}

extern char D_002386C8[];   /* "%s/r%03d.bin" */
extern char D_002386E0[];   /* "data/rom/scene/message" */
extern char D_002386F8[];   /* "%s/r%03du.bin" */
extern char D_00238708[];   /* "r%03d.bin" */
extern char D_00238718[];   /* "r%03du.bin" */

/* The room's messages. */
void func_001D7150(void) {
    char name[0x80];
    Options *opt = &D_00306590;
    s32 *buf = D_00306590.message;

    if (D_00306590.unk17 == 0) {
        if (D_00306590.unk3D == 0) {
            sprintf(name, D_002386C8, D_002386E0, opt->unk1A);
        } else {
            sprintf(name, D_002386F8, D_002386E0, opt->unk1A);
        }
        if (func_001CFBB0(name, buf, 1, 0) <= 0) {
            *buf = 0;
        }
    } else {
        if (D_00306590.unk3D == 0) {
            sprintf(name, D_00238708, opt->unk1A);
        } else {
            sprintf(name, D_00238718, opt->unk1A);
        }
        if (func_001A96E0(name, buf) <= 0) {
            *buf = 0;
        }
    }
}

extern char D_002388A0[];   /* "data/rom/scene/rtp/r%02d.rtp" */
extern char D_002388C0[];   /* "r%02d.rtp" */

/* The room's r%02d.rtp. */
void func_001D76A0(void) {
    char name[0x30];

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_002388A0, D_00306590.unk1A);
    } else {
        sprintf(name, D_002388C0, D_00306590.unk1A);
    }
    func_001CFBB0(name, D_00306590.rtp, 0x10001, 0);
}

extern char D_002389C8[];   /* "%s/r%02d.epp" */
extern char D_002389E0[];   /* "data/rom/scene/em_set" */

void func_001D7B10(void);
#ifdef NON_MATCHING   /* the bgtz delay slot is left empty */
/* The room's enemy placements. */
void func_001D7B10(void) {
    char name[0x100];
    s8 *buf = D_00306590.emSet;

    sprintf(name, D_002389C8, D_002389E0, D_00306590.unk1A);
    if (func_001CFBB0(name, buf, 0x10001, 0) <= 0) {
        *buf = -1;
    }
}
#endif

extern char D_002384F0[];   /* "data\rom\scene\pit_tex\cockpit.tex" */
extern char D_00238518[];   /* "%ssii_%03d.tm2" */
extern char D_00238530[];   /* "data\rom\scene\item_msg\" */
extern char D_00238550[];   /* "sii_%03d.itt" */
extern s32 D_00218E50[4];   /* offsets of the cockpit's 4 textures */
char *strcpy(char *dst, const char *src);
void func_001AE810(s32 src, s32 dst);
void func_001CFE20(s32 tex, s32 addr);
void func_001D0020(const char *name, s32 tex, s32 flags);

/* The status screen's textures (the "cockpit") and the room's item pictures. */
void func_001D5200(void) {
    char name[0x100];
    s32 buf = D_00306590.unk4C;
    s32 tex = 0x2600;
    s32 out = buf + 0x100000;
    s32 i;

    strcpy(name, D_002384F0);
    if (func_001CFBB0(name, (void *)buf, 1, 0) == 0) {
        return;
    }
    func_001AE810(buf, out);
    for (i = 0; i < 4; i++) {
        func_001CFE20(tex, out + D_00218E50[i]);
        tex++;
    }
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238518, D_00238530, D_00306590.unk1A);
        func_001D0020(name, tex, 1);
        return;
    }
    sprintf(name, D_00238550, D_00306590.unk1A);
    func_001D0020(name, tex, 0x10001);
}

/* The room's speech data for the scene variant func_00685060 picks: per character (and a shared
   one, slot 4) sp%02d%02d%02d.bin and spm...bin. */
#include "object.h"

extern char D_00238760[];   /* "data/rom/scene/spk/r%03d/sp%02d%02d%02d.bin" */
extern char D_00238790[];   /* "sp%02d%02d%02d.bin" */
extern char D_002387B0[];   /* "data/rom/scene/spk/r%03d/sp%02d%02d08.bin" */
extern char D_002387E0[];   /* "sp%02d%02d08.bin" */
extern char D_00238800[];   /* "data/rom/scene/spk/r%03d/spm%02d%02d%02d.bin" */
extern char D_00238830[];   /* "spm%02d%02d%02d.bin" */
extern char D_00238850[];   /* "data/rom/scene/spk/r%03d/spm%02d%02d08.bin" */
extern char D_00238880[];   /* "spm%02d%02d08.bin" */
extern u32 D_0023BE14;
extern DrawObjFull D_00476DD0[];
extern u8 D_0048AEA4[];
s32 func_00685060(void);
void func_00685380(void);
void func_00684FC0(u8 *data, s32 kind, s32 slot);

void func_001D7320(void);
#ifdef NON_MATCHING   /* first draft */
void func_001D7320(void) {
    char name[0x80];
    s32 v = func_00685060();
    u8 *buf;
    s32 i;

    if (D_0048AEA4[0] == v) {
        return;
    }
    func_00685380();
    if (v < 0) {
        return;
    }
    for (i = 0, buf = D_00306590.speech + 0x6000; i < (s32)D_0023BE14; i++, buf += 0x12800) {
        if (D_00306590.unk17 == 0) {
            sprintf(name, D_00238760, D_00306590.unk1A, D_00306590.unk1A, v, D_00476DD0[i].unkB98);
        } else {
            sprintf(name, D_00238790, D_00306590.unk1A, v, D_00476DD0[i].unkB98);
        }
        if (func_001CFBB0(name, buf, 0x10001, 0) > 0) {
            func_00684FC0(buf, 0, i);
        }
    }
    buf = D_00306590.speech + 0x6000;
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_002387B0, D_00306590.unk1A, D_00306590.unk1A, v);
    } else {
        sprintf(name, D_002387E0, D_00306590.unk1A, v);
    }
    if (func_001CFBB0(name, buf + 0x4A000, 0x10001, 0) > 0) {
        func_00684FC0(buf + 0x4A000, 0, 4);
    }
    for (i = 0, buf = D_00306590.speech + 0x5C000; i < (s32)D_0023BE14; i++, buf += 0x5C00) {
        if (D_00306590.unk17 == 0) {
            sprintf(name, D_00238800, D_00306590.unk1A, D_00306590.unk1A, v, D_00476DD0[i].unkB98);
        } else {
            sprintf(name, D_00238830, D_00306590.unk1A, v, D_00476DD0[i].unkB98);
        }
        if (func_001CFBB0(name, buf, 0x10001, 0) > 0) {
            func_00684FC0(buf, 1, i);
        }
    }
    buf = D_00306590.speech + 0x5C000;
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238850, D_00306590.unk1A, D_00306590.unk1A, v);
    } else {
        sprintf(name, D_00238880, D_00306590.unk1A, v);
    }
    if (func_001CFBB0(name, buf + 0x17000, 0x10001, 0) > 0) {
        func_00684FC0(buf + 0x17000, 1, 4);
    }
    D_0048AEA4[0] = v;
}
#endif
