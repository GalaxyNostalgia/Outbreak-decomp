#include "cri.h"
#include "sce.h"

/* Game options and sound start-up: the options block, master/BGM/SE volumes, the three ADX
   streams the game keeps for music and sound effects. */

#include "options.h"

extern s8 D_00306390[0x10];        /* options as saved */
extern s8 D_00231B30[0x10];        /* default saved options */

#ifdef NON_MATCHING   /* loads the first field through the base pointer; original uses an absolute load */
void func_001AE0A0(void) {
    Options *o = &D_00306590;

    if (o->unk2C >= 6) {
        o->unk2C = 0;
    }
    o->unk2F &= 1;
    o->sound &= 1;
    if (o->bgmVol >= 8) {
        o->bgmVol = 7;
    }
    if (o->seVol >= 8) {
        o->seVol = 7;
    }
    if (o->adjustX < -10 || o->adjustX > 10) {
        o->adjustX = 0;
    }
    if (o->adjustY < -10 || o->adjustY > 10) {
        o->adjustY = 0;
    }
    if (o->brightness < 0x38 || o->brightness >= 0xA5) {
        o->brightness = 0x80;
    }
}
#endif

void func_001B6970(void);

void func_001ADFC0(void) {
    memset(D_00306390, 0, sizeof(D_00306390));
    memcpy(D_00306390, D_00231B30, sizeof(D_00306390));
    func_001B6970();
}

void func_001B6970(void) {
}

void func_001B6990(s32 id) {
}

extern s32 D_0028216C[];
extern s32 D_00282170[];

void func_001983C0(s32 x, s32 y) {
    D_0028216C[0] = x;
    D_00282170[0] = y & ~1;
}

void func_001AE9D0(s32 x, s32 y) {
    func_001983C0(x, y);
}

s32 func_001A6B30(u16 vol);
s32 func_001A6BD0(u8 ch, u16 vol);
s32 func_001A7050(s32 mute);
s32 func_001A70F0(s32 on);
extern s32 D_0023BB0C;   /* everything muted */

/* Sound effect master volume (ch -1) or one channel's; volume 0 mutes everything. */
s32 func_001AC160(s32 ch, s32 vol) {
    if (vol == 0) {
        if (func_001A7050(1) < 0) {
            return -1;
        }
        D_0023BB0C = 1;
    } else if (D_0023BB0C != 0) {
        if (func_001A7050(0) < 0) {
            return -1;
        }
        D_0023BB0C = 0;
    }
    if (ch == -1) {
        return func_001A6B30(vol);
    }
    return func_001A6BD0(ch, vol);
}

void func_001AC200(s32 mono) {
    func_001A70F0(mono ^ 1);
}

void func_00133168(void);

s32 func_001AD660(s32 unused) {
    func_00133168();
    return 0;
}

void func_001B6D70(s32 sound) {
    func_001AC200(sound);
    func_001AD660(sound);
}

/* The ADX streams: 0 BGM, 1 and 2 sound effects. */
typedef struct {
    s8 active;     /* 0x00 */
    s8 nch;        /* 0x01 */
    s16 pad;
    s32 vol;       /* 0x04 */
    s32 target;    /* 0x08 */
    f32 speed;     /* 0x0C: volume fade per frame */
    f32 fvol;      /* 0x10 */
    s32 limit;     /* 0x14: the fade stops here */
} AdxStream;

extern s32 D_0029B050[3];
extern AdxStream D_0029B060[3];
extern void *D_0029B0A8[4];      /* work areas */
extern void *D_0029B0B8[3];      /* ADXT handles */
extern s32 D_00216570[];         /* volume (0-127) to ADX attenuation, x10 */

#ifdef NON_MATCHING   /* register allocation/order differs */
s32 func_001AD350(s32 n, s32 vol) {
    AdxStream *s;
    f32 f;

    vol &= 0x7F;
    ADXT_SetOutVol(D_0029B0B8[n], D_00216570[vol] / 10);
    s = &D_0029B060[n];
    s->target = vol;
    if (vol < s->vol) {
        s->vol = vol;
    }
    f = vol;
    if (!(s->fvol <= f)) {
        s->fvol = f;
    }
    return 0;
}
#endif

#ifdef NON_MATCHING   /* MW hoists 127*f out of the loop; the original recomputes it */
/* SE volume (0-7) */
void func_001B6DA0(s32 vol) {
    f32 f = (f32)vol / 7.0f;
    s32 i;

    func_001AC160(-1, (s32)(256.0f * f));
    i = 1;
    do {
        func_001AD350(i, (s32)(127.0f * f));
        i++;
    } while (i < 3);
}
#endif

/* BGM volume (0-7) */
void func_001B6E30(s32 vol) {
    func_001AD350(0, (s32)(127.0f * ((f32)vol / 7.0f)));
}

void func_001B7FE0(void);

void func_001B69C0(void) {
    func_001AC160(-1, (s32)(256.0f * (D_00306590.seVol / 7.0f)));
    func_001B7FE0();
}

extern u8 D_3938F0[0xF0];

void func_001B7FE0(void) {
    memset(D_3938F0, 0, sizeof(D_3938F0));
}

/* Start a session's options from the saved ones. */
void func_001B9240(void) {
    D_00306590.unkB5 = 0;
    D_00306590.unk3D = 0;
    D_00306590.unkF = 1;
    D_00306590.unkC = 1;
    D_00306590.unk3 = 0;
    D_00306590.unk4 = 1;
    D_00306590.unk7 = 0;
    D_00306590.unk8 = 0;
    D_00306590.unk2 = 1;
    D_00306590.unk1 = 0;
    D_00306590.unk6 = 1;
    D_00306590.unkB = 1;
    D_00306590.sound = D_00306390[0];
    D_00306590.bgmVol = D_00306390[1];
    D_00306590.seVol = D_00306390[2];
    D_00306590.unk2C = D_00306390[3];
    D_00306590.unk2F = D_00306390[4];
    D_00306590.adjustX = D_00306390[6];
    D_00306590.adjustY = D_00306390[7];
    D_00306590.brightness = D_00306390[5];
    func_001AE0A0();
    D_00306590.online = 0;
    D_00306590.unk1A = 0xFFFF;
    D_00306590.unk18 = 0;
    D_00306590.unk19 = 0;
    D_00306590.unk1C = 0;
    D_00306590.unk1E = 0;
    D_00306590.unk20 = 0;
    D_00306590.unk22 = 0;
    D_00306590.unk24 = 0;
    D_00306590.unk26 = 0;
    D_00306590.unk28 = 0;
    D_00306590.unk29 = 0;
    D_00306590.unk2A = 0;
    D_00306590.unk3A = 0;
    D_00306590.unk3B = 0;
    func_001B6990(0x1B);
    func_001B6990(0x50);
    func_001B6990(0x51);
    func_001B6990(0x54);
    func_001AE9D0(D_00306590.adjustX, D_00306590.adjustY);
    func_001B6D70(D_00306590.sound);
    func_001B6DA0(D_00306590.seVol);
    func_001B6E30(D_00306590.bgmVol);
}

/* Stop streams that are playing */
void func_001AD470(void) {
    s32 i = 0;
    void **h;

    do {
        h = &D_0029B0B8[i];
        if (*h != NULL && ADXT_GetStat(*h) == 3) {
            D_0029B060[i].active = 0;
            ADXT_Pause(*h, 0);
            ADXT_Stop(*h);
        }
        i++;
    } while (i < 3);
}

extern s32 D_0023BB10;   /* streams created */
extern u32 D_0023BB18[];
extern s8 D_00239F50[3]; /* channels per stream */
void func_001AE9E0(u32 *mark);
u32 func_001AE950(u32 size);
void func_00132908(void *adxt, s32 n);

/* ADXT_CALC_WORK for one stream: input buffer for 1.2 MB (stereo) or 0.6 MB (mono) a second,
   sector aligned, plus the output buffer. */
#define ADX_WORK(nch, ibuf) ((nch) * 0x40C0 + (((ibuf) + 0x800) / 0x800) * 0x800 + 0x24 + 0x40)

#ifdef NON_MATCHING   /* work-size arithmetic schedules differently */
s32 func_001ACEA0(void) {
    s32 i;
    AdxStream *s;
    s32 size;

    i = 2;
    if (D_0023BB10 != 0) {
        func_001AD470();
        return -1;
    }
    do {
        D_0029B050[i] = 0;
        i--;
    } while (i >= 0);
    memset(D_0029B060, 0, sizeof(D_0029B060));
    func_001AE9E0(D_0023BB18);
    for (i = 0; i < 3; i++) {
        s = &D_0029B060[i];
        s->active = 0;
        if (i == 0) {
            s->target = s->vol = D_00306590.bgmVol * 127 / 100;
            s->fvol = (f32)(D_00306590.bgmVol * 127) / 100.0f;
        } else {
            s->target = s->vol = D_00306590.seVol * 127 / 100;
            s->fvol = (f32)(D_00306590.seVol * 127) / 100.0f;
        }
        s->speed = 0;
        s->nch = D_00239F50[i];
        if (s->nch == 2) {
            size = ADX_WORK(s->nch, s->nch * 1200000 / 44 * 5);
        } else {
            size = ADX_WORK(s->nch, s->nch * 600000 / 44 * 4);
        }
        D_0029B0A8[i] = (void *)func_001AE950(size);
        while ((D_0029B0B8[i] = ADXT_Create(s->nch, D_0029B0A8[i], size)) == NULL) {
        }
        if (s->nch == 2) {
            ADXT_SetReloadSct(D_0029B0B8[i], 25);
        }
        func_00132908(D_0029B0B8[i], 2);
        ADXT_SetSvrFreq(D_0029B0B8[i], 30);
    }
    D_0023BB10 = 1;
    return 0;
}
#endif

s32 func_001AF500(void) {
    return D_00306590.online;
}

void func_0012BC38(void);    /* CRI ADX: run the server */

#ifdef NON_MATCHING   /* first draft: one instruction short, fade-end test scheduled differently */
/* Volume fades of the three ADX streams, once a frame. */
void func_001AD740(void) {
    s32 i;
    s32 j;
    void **h;
    AdxStream *s;
    f32 v;
    s32 vol;

    D_00306590.adxBusy = 0;
    i = 0;
    if (func_001B6980(0xE) == 0) {
        for (j = 0, h = D_0029B0B8; j < 3; j++, h++) {
            if (h != NULL && (ADXT_GetStat(*h) == 1 || ADXT_GetStat(*h) == 2)) {
                D_00306590.adxBusy = 1;
                break;
            }
        }
        i = 0;
    }
    do {
        s = &D_0029B060[i];
        if (s->active == 0 && s->speed != 0.0f) {
            v = s->fvol + s->speed;
            if (v < 0.0f) {
                v = 0.0f;
            }
            vol = (v / 127.0f) * s->target;
            if (s->target < vol) {
                vol = s->target;
                v = s->target;
            }
            if (vol != s->vol) {
                ADXT_SetOutVol(D_0029B0B8[i], D_00216570[vol & 0x7F] / 10);
            }
            s->vol = vol;
            s->fvol = v;
            if ((s->vol >= s->limit && s->speed > 0.0f) || (s->vol <= s->limit && s->speed < 0.0f)) {
                s->speed = 0.0f;
            }
        }
        i++;
    } while (i < 3);
    func_0012BC38();
}
#endif
