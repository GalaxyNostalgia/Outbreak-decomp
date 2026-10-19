#include "cri.h"
#include "sce.h"

/* Game options and sound start-up: the options block, master/BGM/SE volumes, the three ADX
   streams the game keeps for music and sound effects. */

#include "options.h"
#include "game.h"
#include "object.h"

extern s8 D_00306390[0x10];        /* options as saved */
extern s8 D_00231B30[0x10];        /* default saved options */

#ifdef NON_MATCHING   /* loads the first field through the base pointer; original uses an absolute load */
void func_001AE0A0(void) {
    Options *o = &D_00306590;

    if (o->unk2C >= 6) {
        o->unk2C = 0;
    }
    o->vibration &= 1;
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
    s8 wait;       /* 0x02: started paused, waiting for the go */
    s8 started;    /* 0x03 */
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
    D_00306590.vibration = D_00306390[4];
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

s32 func_001AA5C0(const char *name);
s32 func_001B6980(s32 slot);

/* Sets stream n's volume now (vol 0-127, scaled by its set volume), stopping any fade. */
s32 func_001AD260(s32 n, s32 vol) {
    s32 v;

    v = (s32)(((f32)vol / 127.0f) * (f32)D_0029B060[n].target) & 0x7F;
    ADXT_SetOutVol(D_0029B0B8[n], D_00216570[v] / 10);
    D_0029B060[n].vol = v;
    D_0029B060[n].fvol = v;
    D_0029B060[n].speed = 0.0f;
    return 0;
}

/* The next start of stream n waits for func_001AD9D0. */
void func_001AD960(s32 n) {
    D_0029B060[n].wait = 1;
    ADXT_SetWaitPlayStart(D_0029B0B8[n], D_0029B060[n].wait);
    D_0029B060[n].started = 0;
}

/* Lets stream n play, at its set volume. */
void func_001AD9D0(s32 n) {
    D_0029B060[n].wait = 0;
    ADXT_SetWaitPlayStart(D_0029B0B8[n], D_0029B060[n].wait);
    func_001AD260(n, D_0029B060[n].target);
    D_0029B060[n].started = 0;
}

/* Starts an ADX file on stream n, paused until func_001ADC30: by name from partition 3, or
   (byId) file `file` of partition ptid (0 if negative). */
void func_001ADA60(s32 n, s32 byId, s32 file, s32 ptid) {
    s32 fid;
    ADXT *h;

    if (byId == 0) {
        fid = func_001AA5C0((const char *)file);
        if (fid < 0) {
            return;
        }
    }
    h = &D_0029B0B8[n];
    if (*h != NULL && func_001B6980(0xE) == 0) {
        func_001AD260(n, D_0029B060[n].target);
        func_001AD960(n);
        if (byId == 1) {
            if (ptid < 0) {
                ADXT_StartAfs(*h, 0, file);
            } else {
                ADXT_StartAfs(*h, ptid, file);
            }
        } else {
            ADXT_StartAfs(*h, 3, fid);
        }
        D_0029B060[n].started = 1;
        ADXT_Pause(*h, 0);
    }
}

/* Whether stream n is ready to start (1 when there's nothing to wait for). */
s32 func_001ADBA0(s32 n) {
    if (func_001B6980(0xE) != 0) {
        return 1;
    }
    if (D_0029B0B8[n] == NULL) {
        return 1;
    }
    if (D_0029B060[n].started == 0) {
        return 1;
    }
    return ADXT_IsReadyPlayStart(D_0029B0B8[n]);
}

/* Starts stream n playing. */
void func_001ADC30(s32 n) {
    if (func_001B6980(0xE) == 0 && D_0029B0B8[n] != NULL) {
        func_001AD9D0(n);
    }
}

void func_001AD680(s32 n, s32 limit, s32 frames);
#ifdef NON_MATCHING   /* the original computes n * 0x18 once */
/* Fades stream n's volume to limit (at most its set volume) over the given frames. */
void func_001AD680(s32 n, s32 limit, s32 frames) {
    if (D_0029B060[n].target < limit) {
        limit = D_0029B060[n].target;
    }
    D_0029B060[n].speed = ((f32)limit - D_0029B060[n].fvol) / (f32)frames;
    D_0029B060[n].limit = limit;
}
#endif

/* Whether stream n is fading. */
s32 func_001AD700(s32 n) {
    if (D_0029B060[n].speed == 0.0f) {
        return 0;
    }
    return 1;
}

s32 func_001AF500(void) {
    return D_00306590.online;
}

/* 1 when Options.unk3B is 1 or 2 */
s32 func_001AF510(void) {
    s8 x = D_00306590.unk3B;

    if (x == 1 || x == 2) {
        return 1;
    }
    return 0;
}

/* 1 offline (SINGLE.BIN), 0 online */
s32 func_001AF4D0(void) {
    return func_001AF500() == 0;
}

/* Whether player o is played here: offline the first (the others are partners), online ours. */
s32 func_001AF540(DrawObjFull *o);
#ifdef NON_MATCHING   /* the compiler folds the returns into set-if-equal */
s32 func_001AF540(DrawObjFull *o) {
    if (o->type != 0) {
        return 0;
    }
    if (func_001AF4D0() == 1) {
        if (o->kind != 0) {
            return 0;
        }
        return 1;
    }
    if (D_0048BF60.unk1D == o->kind) {
        return 1;
    }
    return 0;
}
#endif

void func_008201D0(s32 a);    /* SINGLE.BIN */
void func_00821B50(s32 a);    /* NETAQ.BIN */
void func_00820200(void);     /* SINGLE.BIN */
void func_00821D60(void);     /* NETAQ.BIN */
void func_00820820(void);     /* SINGLE.BIN */
void func_00822760(void);     /* NETAQ.BIN */
s32 func_00821750(void);      /* SINGLE.BIN */
s32 func_00820F20(void);      /* NETAQ.BIN */
s32 func_00821D00(s32 i);     /* SINGLE.BIN */
s32 func_00821570(s32 i);     /* NETAQ.BIN */

void func_00821170(s32 to, void *msg, s32 kind, s32 a3);  /* SINGLE.BIN */
void func_00820860(s32 to, void *msg, s32 kind, s32 a3);  /* NETAQ.BIN */

/* Sends a message to the others (offline: to the partners' code). */
void func_001AF000(s32 to, void *msg, s32 kind, s32 a3) {
    switch (func_001AF500()) {
    case 0:
        func_00821170(to, msg, kind, a3);
        break;
    case 1:
        func_00820860(to, msg, kind, a3);
        break;
    case 2:
    case 3:
        break;
    }
}

/* The session is up (always, outside the network modes). */
s32 func_001AF0A0(void) {
    switch (func_001AF500()) {
    case 0:
        return func_00821750();
    case 1:
        return func_00820F20();
    case 2:
    case 3:
        break;
    }
    return 1;
}

/* Player i is in the session. */
s32 func_001AF110(s32 i) {
    switch (func_001AF500()) {
    case 0:
        return func_00821D00(i);
    case 1:
        return func_00821570(i);
    case 2:
    case 3:
        break;
    }
    return i == 0;
}

s32 func_00821D20(s32 room);   /* SINGLE.BIN */
s32 func_00821590(s32 room);   /* NETAQ.BIN */

/* Which player is in room (-1: nobody; outside the network modes, us when it's ours). */
s32 func_001AF440(s32 room) {
    switch (func_001AF500()) {
    case 0:
        return func_00821D20(room);
    case 1:
        return func_00821590(room);
    case 2:
    case 3:
        break;
    }
    return D_00306590.unk1C == room ? 0 : -1;
}

/* Hands a call to the loaded game mode's overlay. */
void func_001AEDC0(s32 a) {
    switch (func_001AF500()) {
    case 0:
        func_008201D0(a);
        break;
    case 1:
        func_00821B50(a);
        break;
    case 2:
    case 3:
        break;
    }
}

/* Once a frame: the game mode overlay's work. */
void func_001AEEA0(void) {
    if (D_00306590.unk3A == 0) {
        return;
    }
    switch (func_001AF500()) {
    case 0:
        if (D_0048BF60.unkC4 == 0) {
            func_00820200();
        }
        break;
    case 1:
        func_00821D60();
        break;
    case 2:
    case 3:
        break;
    }
}

/* Once a frame, after the game's work: the game mode overlay's. */
void func_001AEF80(void) {
    if (D_00306590.unk3A == 0) {
        return;
    }
    switch (func_001AF500()) {
    case 0:
        if (D_0048BF60.unkC4 == 0) {
            func_00820820();
        }
        break;
    case 1:
        func_00822760();
        break;
    case 2:
    case 3:
        break;
    }
}

void func_00820F30(void);     /* NETAQ.BIN */
void func_00821950(u8 player, s32 a, s32 b);  /* SINGLE.BIN */
void func_00821130(u8 player, s32 a, s32 b);  /* NETAQ.BIN */

void func_001AF190(void) {
    if (func_001AF500() == 1) {
        func_00820F30();
    }
}

/* A player is in the room now. */
void func_001AF1C0(u8 player) {
    switch (func_001AF500()) {
    case 0:
        func_00821950(player, 0, 0);
        break;
    case 1:
        func_00821130(player, 0, 0);
        break;
    case 2:
    case 3:
        break;
    }
}

void func_00820AA0(void);     /* SINGLE.BIN */
void func_008229C0(void);     /* NETAQ.BIN */

void func_001AEE30(void) {
    switch (func_001AF500()) {
    case 0:
        func_00820AA0();
        break;
    case 1:
        func_008229C0();
        break;
    case 2:
    case 3:
        break;
    }
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

extern u16 D_0023BBA0;      /* random seed (game) */
extern u16 D_0023BB9C;      /* random seed (sounds, effects) */
extern u8 D_0023BB28;

/* Random numbers: 0 the game seed as is, 1 and 3 the next number from each generator
   (x * 176 mod 65363), 2 a byte the game keeps. */
u16 func_001ADEF0(s32 which) {
    u32 x;

    switch (which) {
    case 0:
        return D_0023BBA0;
    case 1:
        x = D_0023BB9C;
        if (x == 0) {
            x = 1;
        }
        x = (x * 176) % 0xFF53u;
        D_0023BB9C = x;
        return x;
    case 2:
        return D_0023BB28;
    case 3:
        x = D_0023BBA0;
        if (x == 0) {
            x = 1;
        }
        x = (x * 176) % 0xFF53u;
        D_0023BBA0 = x;
        return x;
    }
    return 0;
}

extern s32 D_0048BFB4[];    /* the music on each stream */
s32 func_001ACCC0(s32 n, s32 fid, s32 ptid);
s32 func_001AD570(s32 n, s32 what);
void func_001AD1E0(s32 n);

s32 func_001B87F0(s32 n, s32 bgm, s32 c);
#ifdef NON_MATCHING   /* branch operand order */
/* Plays music `bgm` on stream n; if it's already playing (paused), fades it back in. */
s32 func_001B87F0(s32 n, s32 bgm, s32 c) {
    s32 r;

    if (D_0048BFB4[n] != bgm) {
        D_0048BFB4[n] = bgm;
        func_001ACCC0(n, bgm, c);
        return 0;
    }
    if (func_001AD570(n, 4) != 0) {
        r = func_001AD570(n, 0);
        if (r == 0 || r == 5) {
            func_001ACCC0(n, bgm, c);
        } else {
            func_001AD1E0(n);
            func_001AD680(n, 0x7F, 0x3C);
            return 1;
        }
    } else {
        func_001ACCC0(n, bgm, c);
    }
    return 0;
}
#endif

/* Starts file `fid` of partition ptid (0 if negative) on stream n at its set volume. */
s32 func_001ACCC0(s32 n, s32 fid, s32 ptid) {
    ADXT *h;

    if (func_001B6980(0xE) != 0) {
        return 0;
    }
    if (func_001B6980(0xA) != 0 && n == 0) {
        return 0;
    }
    func_001AD260(n, D_0029B060[n].target);
    if (ptid < 0) {
        h = &D_0029B0B8[n];
        ADXT_StartAfs(*h, 0, fid);
    } else {
        h = &D_0029B0B8[n];
        ADXT_StartAfs(*h, ptid, fid);
    }
    ADXT_Pause(*h, 0);
    D_0029B060[n].active = 0;
    return 0;
}

extern char D_00231B18[];

/* Starts the stream for an event on the BGM stream (paused until it is ready). */
void func_001ADDF0(void) {
    s32 fid;

    if (D_0029B0B8[0] == NULL) {
        return;
    }
    func_001AD260(0, D_0029B060[0].target);
    func_001AD960(0);
    D_0029B060[0].started = 1;
    ADXT_Pause(D_0029B0B8[0], 0);
    fid = func_001AA5C0(D_00231B18);
    if (fid != -1) {
        ADXT_StartAfs(D_0029B0B8[0], 3, fid);
    }
}

/* Player p's voice archive (the name it builds is left unused). */
extern char D_00231FB0[];   /* "vo001%02d.afs" */
s32 func_001AA720(s32 player, s32 kind);

void func_001B8AC0(s32 player, s32 kind) {
    char name[0x100];

    if (player < 5) {
        sprintf(name, D_00231FB0, kind);
        if (func_001AA720(player, kind) != 0) {
            D_0048BF60.unkA1[player] = kind + 1;
            return;
        }
        D_0048BF60.unkA1[player] = 0;
    }
}

/* Each character in the game is handed its pad. */
#include "object.h"

extern s8 D_0023BB90;
extern DrawObjFull D_00476DD0[];
s32 func_001A12B0(s32 pad);
extern s8 D_0023BB38[8];         /* per character: its pad */

void func_001AFB90(DrawObjFull *obj, s8 pad) {
    D_0023BB38[obj->kind] = pad;
}

void func_001AFBB0(void) {
    s8 pad;
    s8 n;
    DrawObjFull *obj;

    for (pad = 0, n = 0; pad < D_0023BB90; pad++) {
        if (func_001A12B0(pad) == 1) {
            obj = &D_00476DD0[n];
            if (obj->active != 0) {
                n++;
                func_001AFB90(obj, pad);
            }
        }
    }
}
