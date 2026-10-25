#include "cri.h"
#include "sce.h"
#include "object.h"

/* Sound effect banks (.snd files in partition 3): a queue of banks to load, read with ADXF and sent
   to the sound driver one per frame. */

s32 printf(const char *fmt, ...);
s32 sprintf(char *dst, const char *fmt, ...);
s32 strlen(const char *s);
char *strupr(char *s);
s32 FlushCache(s32 op);

/* Look a file name up in a partition's name table (stored XORed with 0xFF). Leading ROM\, RDT\ or
   EDIT\ is dropped and \ / . - become _. */
s32 func_001AA320(char **table, s32 n, const char *name);
#ifdef NON_MATCHING   /* first draft */
s32 func_001AA320(char **table, s32 n, const char *name) {
    char tmp[0x100];
    char key[0x100];
    char up[0x100];
    char *p;
    char *q;
    char *src;
    s32 len;
    s32 i;
    s32 found;
    s32 k;

    strcpy(up, name);
    strupr(up);
    p = up;
    len = strlen(p);
    i = 0;
    found = 0;
    while (*p != 0) {
        if (p[0] == 'R' && p[1] == 'O' && p[2] == 'M' && (p[3] == '\\' || p[3] == '/')) {
            found = 1;
            p += 4;
            break;
        }
        if (p[0] == 'R' && p[1] == 'D' && p[2] == 'T' && (p[3] == '\\' || p[3] == '/')) {
            found = 1;
            p += 4;
            break;
        }
        if (p[0] == 'E' && p[1] == 'D' && p[2] == 'I' && p[3] == 'T' && (p[4] == '\\' || p[4] == '/')) {
            found = 1;
            p += 5;
            break;
        }
        i++;
        p++;
        if (i + 3 >= len) {
            break;
        }
    }
    strcpy(key, found ? p : up);
    for (q = key; *q != 0; q++) {
        if (*q == '\\') {
            *q = '_';
        }
        if (*q == '/') {
            *q = '_';
        }
        if (*q == '.') {
            *q = '_';
        }
        if (*q == '-') {
            *q = '_';
        }
    }
    for (k = 0; k < n; k++, table++) {
        src = *table;
        len = strlen(src);
        for (i = 0; i < len; i++) {
            tmp[i] = *src++ ^ 0xFF;
        }
        tmp[i] = 0;
        if (strcmp(key, tmp) == 0) {
            return k;
        }
    }
    return -1;
}
#endif

extern char *D_00215CF0[];  /* partition 3's names */

/* (the movie player's copy) */
s32 func_001AA5C0(const char *name) {
    return func_001AA320(D_00215CF0, 420, name);
}

s32 func_001AA5E0(const char *name) {
    return func_001AA320(D_00215CF0, 420, name);
}

typedef struct {
    u8 state;           /* 0x000: 0 idle, 1 next request, 2 reading, 3-5 sending */
    u8 count;           /* 0x001: requests queued */
    u8 head;            /* 0x002 */
    u8 pad3;
    s32 cur;            /* 0x004: request being loaded */
    u8 *buffer;         /* 0x008 */
    u8 slot[8];         /* 0x00C: each request's bank slot */
    s16 result[8];      /* 0x014 */
    char name[8][16];   /* 0x024 */
    char *loaded[16];   /* 0x0A4: name of the bank in each slot */
    u8 slotState[16];   /* 0x0E4: 1 loading, 2 loaded */
    u8 slotUsed[16];    /* 0x0F4 */
    ADXF adxf;          /* 0x104 */
} SeBankLoader;

extern SeBankLoader D_00295B40;
extern char D_00231AF8[];   /* "%s.snd" */

s32 func_001ACA30(SeBankLoader *q);
#ifdef NON_MATCHING   /* first draft */
/* Take the next request */
s32 func_001ACA30(SeBankLoader *q) {
    u8 h;

    if (q->count == 0) {
        q->state = 0;
        return -1;
    }
    h = q->head++;
    if (q->head >= 8) {
        q->head = 0;
    }
    q->count--;
    return h;
}
#endif

s32 func_001A6FB0(s32 port);

void func_001AC150(u8 slot) {
    func_001A6FB0(slot);
}

typedef struct {
    u8 b0;
    u8 pad1;
    s16 h2;
    u8 b4;
    u8 b5;
    u8 b6;
    u8 b7;
    u8 b8;
    u8 b9;
    u8 bA;
    u8 bB;
} SeInfo;               /* 0x0C */

extern SeInfo D_00295C50[14][128];
s32 func_001A6DA0(u32 bank, u32 a0, s32 n0, u32 a1, s32 n1, u32 a2, s32 n2);
s32 func_001A7280(s32 nowait);

s32 func_001AC250(u8 *snd, u32 slot);
#ifdef NON_MATCHING   /* first draft */
/* Send a loaded .snd file to the driver as bank `slot`; keeps its sound table for the game. */
s32 func_001AC250(u8 *snd, u32 slot) {
    SeInfo *dst;
    u8 *src;
    s32 i;
    s32 r;
    u32 n;

    if (slot >= 14) {
        return -1;
    }
    dst = D_00295C50[slot];
    src = snd + *(s32 *)(snd + 0x20) + 0x10;
    memset(D_00295C50[slot], 0, sizeof(D_00295C50[0]));
    n = *(u32 *)(snd + 0x24) >> 4;
    for (i = 0; i < 0x80 && (u32)i < (*(u32 *)(snd + 0x24) - 0x10) >> 4; i++) {
        dst->b0 = src[0];
        dst->b5 = src[9];
        dst->b4 = src[8];
        dst->h2 = *(s16 *)(src + 0xA);
        dst->b6 = src[0xC];
        dst->b7 = src[0xD];
        dst->b8 = src[6] & 0x7F;
        dst->b9 = (src[6] & 0x80) != 0;
        dst->bB = src[0xF];
        src += 0x10;
        dst++;
    }
    r = func_001A6DA0(slot, (u32)(snd + *(s32 *)(snd + 0x8)), *(s32 *)(snd + 0xC), (u32)(snd + *(s32 *)(snd + 0x10)),
                      *(s32 *)(snd + 0x14), (u32)(snd + *(s32 *)(snd + 0x18)), *(s32 *)(snd + 0x1C));
    if (r < 0) {
        memset(D_00295C50[slot], 0xFF, sizeof(D_00295C50[0]));
        return r;
    }
    FlushCache(0);
    func_001A7280(1);
    return n;
}
#endif

s32 func_001A73E0(s32 nowait);
s32 func_001A7540(u32 cmd, s32 arg);

void func_001ACA80(void);
#ifdef NON_MATCHING   /* first draft */
/* Once a frame: work on the bank queue. */
void func_001ACA80(void) {
    SeBankLoader *q = &D_00295B40;
    char path[0x40];
    u8 slot;
    s32 fid;

    switch (q->state) {
    case 1:
        q->cur = func_001ACA30(q);
        if (q->cur < 0) {
            return;
        }
        slot = q->slot[q->cur];
        if (slot < 15 && q->loaded[slot] != NULL && strcmp(q->name[q->cur], q->loaded[slot]) == 0) {
            return;
        }
        func_001AC150(slot);
        sprintf(path, D_00231AF8, q->name[q->cur]);
        fid = func_001AA5E0(path);
        if (fid < 0) {
            return;
        }
        q->adxf = ADXF_OpenAfs(3, fid);
        if (q->adxf == NULL) {
            return;
        }
        ADXF_ReadNw(q->adxf, ADXF_GetFsizeSct(q->adxf), q->buffer);
        q->state++;
        q->slotState[slot] = 1;
        q->slotUsed[slot] = 1;
        return;
    case 2:
        if (ADXF_GetStat(q->adxf) != 3) {
            return;
        }
        ADXF_Close(q->adxf);
        q->adxf = NULL;
        slot = q->slot[q->cur];
        q->result[q->cur] = func_001AC250(q->buffer, slot);
        if (q->result[q->cur] < 0) {
            q->state = 1;
            q->slotState[slot] = 0;
            return;
        }
        if (slot < 16) {
            q->loaded[slot] = q->name[q->cur];
        }
        q->state++;
    case 3:
        if (func_001A73E0(1) > 0) {
            return;
        }
        q->state++;
    case 4:
        if (func_001A7540(0x8000000C, 1) > 0) {
            return;
        }
        q->state++;
    case 5:
        q->slotState[q->slot[q->cur]] = 2;
        q->state = 1;
        break;
    }
}
#endif

void func_0012BAB8(void);   /* CRI: hold the disc for us */
void func_0012BAD0(void);   /* and let it go */
extern s32 D_0023BAF4;      /* CRI file system up */
extern s32 D_0023BB08;      /* disc hold count */

void func_001AAA60(void) {
    if (D_0023BAF4 != 0) {
        if (D_0023BB08 == 0) {
            func_0012BAB8();
            D_0023BB08++;
        } else {
            D_0023BB08++;
        }
    }
}

void func_001AAAB0(void) {
    if (D_0023BAF4 != 0) {
        if (D_0023BB08 == 1) {
            func_0012BAD0();
            D_0023BB08--;
            return;
        }
        D_0023BB08--;
        if (D_0023BB08 < 0) {
            D_0023BB08 = 0;
        }
    }
}

s32 sceGsSyncV(s32 mode);

/* Send a bank and wait until the driver has it. */
s32 func_001AB9E0(u8 *snd, u32 slot) {
    s32 r;

    func_001A7540(0x8000000C, 1);
    if ((r = func_001AC250(snd, slot)) < 0) {
        return r;
    }
    func_001AAA60();
    while (func_001A73E0(1) > 0) {
        sceGsSyncV(0);
    }
    while (func_001A7540(0x8000000C, 1) > 0) {
        sceGsSyncV(0);
    }
    func_001AAAB0();
    return r;
}

extern char D_00238648[];   /* ".snd" */
extern u8 *D_003065DC[];    /* (Options) bank buffer */

/* Load a .snd file into bank `slot` now. */
void func_001D6EF0(const char *name, u32 slot) {
    char path[0x80];
    s32 fid;
    u8 *buf;
    ADXF adxf;

    strcpy(path, name);
    strcat(path, D_00238648);
    fid = func_001AA5E0(path);
    if (fid < 0) {
        return;
    }
    buf = D_003065DC[0];
    adxf = ADXF_OpenAfs(3, fid);
    if (adxf == NULL) {
        return;
    }
    ADXF_ReadNw(adxf, ADXF_GetFsizeSct(adxf), buf);
    while (ADXF_GetStat(adxf) != 3) {
    }
    ADXF_Close(adxf);
    func_001AB9E0(buf, slot);
}

extern char D_002385E8[];   /* "common" */

void func_001D6CC0(void) {
    func_001D6EF0(D_002385E8, 0);
}

s32 func_001A7190(u32 core, u32 mode, s16 depth, u8 delay, u8 feedback);

s32 func_001AC220(u32 core, u32 mode, s32 depth, s32 delay, s32 feedback) {
    return func_001A7190(core, mode, depth, delay, feedback);
}

extern char D_002385F0[];   /* "start" */
extern char D_002385F8[];   /* "download" */
extern char D_00238608[];   /* "result" */
extern char D_00238610[];   /* "shop" */

/* The sound bank and reverb of a front-end screen. */
void func_001D6D10(s32 screen) {
    char name[0x80];
    s32 depth;

    switch (screen) {
    case 0:
        sprintf(name, D_002385F0);
        depth = 0x2000;
        break;
    case 1:
        sprintf(name, D_002385F8);
        depth = 0x2500;
        break;
    case 2:
        sprintf(name, D_00238608);
        depth = 0x2500;
        break;
    case 3:
        sprintf(name, D_00238610);
        depth = 0x2500;
        break;
    }
    func_001D6EF0(name, 0xC);
    func_001AC220(1, 4, depth, 0, 0);
}

void func_001D6CD0(void) {
    func_001D6D10(0);
}

extern u8 D_00295C34[];     /* banks that are always there (no loading) */
u16 func_001ADEF0(s32 which);   /* random numbers */
s32 func_001A67B0(u32 key, s8 vol, s8 pan, s16 pitch, s8 flag);
s32 func_001A68D0(u32 key);

/* Plays sound `id` of sound effect bank `bank`, then the sounds chained after it. vol (scaled from
   the table's, -1: as is) and pan (-1: the table's) apply to all of them; pitch and volume get the
   table's random spread. */
s32 func_001ABAA0(s32 bank, s32 id, s32 unused, u32 vol, s32 pan, s32 unused2, s32 flag);
#ifdef NON_MATCHING   /* first draft: block order */
s32 func_001ABAA0(s32 bank, s32 id, s32 unused, u32 vol, s32 pan, s32 unused2, s32 flag) {
    SeInfo *tbl = D_00295C50[bank];
    SeInfo *e = &tbl[id];
    u32 key;
    s32 sign;
    s32 pitch;
    s32 v;

    if (D_00295C34[bank] == 0) {
        key = (bank & 0x7F) << 16;
    } else if (D_00295B40.slotState[bank] == 2) {
        key = (bank & 0x7F) << 16;
    } else {
        return -1;
    }
    for (;;) {
        if ((u32)id >= 0x80) {
            return 0;
        }
        if (e->b0 == 0) {
            if (e->b5 != 0xFF || pan == -1) {
                pan = e->b5;
            }
            if (e->b7 != 0) {
                sign = 1;
                if (!(func_001ADEF0(1) & 1)) {
                    sign = -1;
                }
                pitch = (e->h2 + 0x2000 + (func_001ADEF0(1) % e->b7) * sign * 32) & 0x3FFF;
            } else {
                pitch = (e->h2 + 0x2000) & 0x3FFF;
            }
            sign = 1;
            if (!(func_001ADEF0(1) & 1)) {
                sign = -1;
            }
            if (e->b6 != 0) {
                v = e->b4 + (func_001ADEF0(1) % e->b6) * sign;
            } else {
                v = e->b4;
            }
            if (v < 0) {
                v = 0;
            } else if (v >= 0x80) {
                v = 0x7F;
            }
            if (vol == (u32)-1) {
                vol = v;
            } else {
                vol = (f32)v * ((f32)vol / 128.0f);
            }
            if (func_001A67B0(key | ((id & 0x7F) << 8) | ((e->b9 & 1) << 7) | (e->b8 & 0x7F), vol, pan, pitch,
                              flag) < 0) {
                return 0;
            }
        } else if (e->b0 == 1) {
            if (func_001A68D0(key | ((id & 0x7F) << 8) | ((e->b9 & 1) << 7) | (e->b8 & 0x7F)) < 0) {
                return 0;
            }
        } else {
            return 0;
        }
        if (e->bB == 0xFF || e->bB == id) {
            return 0;
        }
        id = e->bB;
        e = &tbl[id];
    }
}
#endif


/* Changes sound `id` of bank `bank` (and those chained after it) as it plays: volume (scaled from
   the table's) and pan (-1: the table's), with the table's random spread. */
s32 func_001A69F0(u32 key, s8 vol, s8 pan, s16 pitch);
s32 func_001ABE10(s32 bank, s32 id, s32 unused, u32 vol, s32 pan, s32 unused2);
#ifdef NON_MATCHING   /* first draft: as func_001ABAA0 */
s32 func_001ABE10(s32 bank, s32 id, s32 unused, u32 vol, s32 pan, s32 unused2) {
    SeInfo *tbl = D_00295C50[bank];
    SeInfo *e = &tbl[id];
    u32 key;
    s32 sign;
    s32 pitch;
    s32 v;

    if (D_00295C34[bank] == 0) {
        key = (bank & 0x7F) << 16;
    } else if (D_00295B40.slotState[bank] == 2) {
        key = (bank & 0x7F) << 16;
    } else {
        return -1;
    }
    for (;;) {
        if ((u32)id >= 0x80) {
            return 0;
        }
        if (e->b0 == 0) {
            if (e->b5 != 0xFF || pan == -1) {
                pan = e->b5;
            }
            if (e->b7 != 0) {
                sign = 1;
                if (!(func_001ADEF0(1) & 1)) {
                    sign = -1;
                }
                pitch = (e->h2 + 0x2000 + (func_001ADEF0(1) % e->b7) * sign * 32) & 0x3FFF;
            } else {
                pitch = (e->h2 + 0x2000) & 0x3FFF;
            }
            sign = 1;
            if (!(func_001ADEF0(1) & 1)) {
                sign = -1;
            }
            if (e->b6 != 0) {
                v = e->b4 + (func_001ADEF0(1) % e->b6) * sign;
            } else {
                v = e->b4;
            }
            if (v < 0) {
                v = 0;
            } else if (v >= 0x80) {
                v = 0x7F;
            }
            vol = (f32)v * ((f32)vol / 128.0f);
            if (func_001A69F0(key | ((id & 0x7F) << 8) | ((e->b9 & 1) << 7) | (e->b8 & 0x7F), vol, pan,
                              pitch) < 0) {
                return 0;
            }
        } else if (e->b0 == 1) {
            if (func_001A68D0(key | ((id & 0x7F) << 8) | ((e->b9 & 1) << 7) | (e->b8 & 0x7F)) < 0) {
                return 0;
            }
        } else {
            return 0;
        }
        id = e->bB;
        if (id == 0xFF) {
            return 0;
        }
        e = &tbl[id];
    }
}
#endif

/* Play a sound effect (the table's volume and pan). */
s32 func_001B7A50(s32 bank, s32 id, s32 unused) {
    return func_001ABAA0(bank, id, unused, -1, -1, -1, 0);
}

void func_001B6E80(s32 id, s32 bank) {
    func_001B7A50(bank, id, 0);
}

/* The menu sounds (bank 0) */
void func_001B6EA0(void) {
    func_001B7A50(0, 0, 0);
}

void func_001B6EB0(void) {
    func_001B7A50(0, 1, 0);
}

void func_001B6EC0(void) {
    func_001B7A50(0, 2, 0);
}

void func_001B6ED0(void) {
    func_001B7A50(0, 3, 0);
}

void func_001B6EE0(void) {
    func_001B7A50(0, 4, 0);
}

/* Bank 12 */
void func_001B6EF0(s32 id) {
    func_001B7A50(12, id, 0);
}

void func_001B6F00(s32 id) {
    func_001B7A50(12, id, 0);
}

s32 func_001B6F10(s32 sound);
s32 func_001B7A70(s32 bank, s32 id, s32 unused, f32 *pos, s32 a, s32 b, s32 c);
extern s16 D_0023BB40;      /* the last such sound */

/* Plays a sound effect of bank 2 (func_001B6F10 picks it; 0xFF: none), at pos if given. */
void func_001B6FD0(f32 *pos, s32 id) {
    id = func_001B6F10(id);

    if (id != 0xFF) {
        if (pos == NULL) {
            func_001B7A50(2, id, 0);
        } else {
            func_001B7A70(2, id, 0, pos, 1, 0, 0);
        }
        D_0023BB40 = id;
    }
}

u16 func_001ADEF0(s32 n);
s8 func_001B75E0(DrawObjFull *o);
u16 func_5445D0(f32 *pos);
void func_00190E00(Mat44 *out, const Mat44 *m);
extern Mat44 D_003062E0;

/* A character's footstep (kind 0/1 left and right foot, 2, 3, 4): bank 3's sound for it (players
   add their floor's, func_001B75E0) at its feet, plus the floor's (func_5445D0: low byte, high
   byte a second sound). */
void func_001B7410(DrawObjFull *o, s8 kind) {
    u16 id;
    u16 se;
    u16 floor;
    u16 at;
    Mat44 m;
    Vec3 pos;

    switch (kind) {
    case 0:
    case 1:
        id = kind * 2 + (func_001ADEF0(1) & 1);
        break;
    case 2:
        id = 4;
        break;
    case 3:
        id = 5;
        break;
    case 4:
        id = (func_001ADEF0(1) & 1) + 6;
        break;
    }
    at = 0;
    if (o->type == 0) {
        se = id + func_001B75E0(o);
        if (o->unk54F == 0x2B && o->act[1] == 0x6D) {
            at = 1;
        }
    } else {
        se = id;
    }
    floor = func_5445D0(o->unk38);
    if (at) {
        func_00190E00(&m, &D_003062E0);
        pos.x = m.m[3][0];
        pos.y = m.m[3][1];
        pos.z = m.m[3][2];
    } else {
        pos = *(Vec3 *)o->unk38;
    }
    func_001B7A70(3, se, o->kind, &pos.x, 1, 0, floor & 0xFF);
    if ((u16)(floor >> 8)) {
        func_001B7A70(3, se, o->kind, &pos.x, 1, 0, (u16)(floor >> 8));
    }
}

/* The enemy sound bank: pieces (sequences and waves) added one by one, then merged and sent as
   one bank. */
typedef struct {
    s32 count;          /* pieces */
    u8 *seqs[4];        /* each piece's sequences */
    s32 seqSize;        /* sequence bytes so far */
    s32 waveSize;       /* wave bytes so far */
    u8 *seqBase;        /* where the sequences go */
    u8 *waveBase;       /* where the waves go */
} SeMerge;

extern SeMerge D_00295B10;
extern char D_00231AE8[];   /* "SND DATA OVER!" */
s32 func_001BCA40(s32 n, u8 **seqs, u8 *out);
void func_001B2870(const char *fmt, ...);
void func_001AAA60(void);
void func_001AAAB0(void);

s32 func_001AC4C0(u8 *data, u8 *work, u32 slot);

/* Merges the pieces into work and sends them as bank slot (waiting until the driver has it). */
s32 func_001AC3F0(u8 *data, u8 *work, u32 slot) {
    s32 r;

    func_001A7540(0x8000000C, 1);
    if ((r = func_001AC4C0(data, work, slot)) < 0) {
        return r;
    }
    func_001AAA60();
    while (func_001A73E0(1) > 0) {
        sceGsSyncV(0);
    }
    while (func_001A7540(0x8000000C, 1) > 0) {
        sceGsSyncV(0);
    }
    func_001AAAB0();
    return r;
}

#ifdef NON_MATCHING   /* first draft */
s32 func_001AC4C0(u8 *data, u8 *work, u32 slot) {
    u8 *snd;
    SeInfo *dst;
    SeInfo *d0;
    u8 *src;
    s32 i;
    s32 n;
    s32 r;

    if (slot >= 0x10) {
        return -1;
    }
    snd = D_003065DC[0];
    d0 = dst = D_00295C50[slot];
    src = snd + *(s32 *)(snd + 0x20) + 0x10;
    memset(dst, 0, sizeof(D_00295C50[0]));
    for (i = 0; i < 0x80 && (u32)i < (*(u32 *)(snd + 0x24) - 0x10) >> 4; i++) {
        dst->b0 = src[0];
        dst->b5 = src[9];
        dst->b8 = src[6] & 0x7F;
        dst->b9 = (src[6] & 0x80) != 0;
        dst->b4 = src[8];
        dst->b6 = src[0xC];
        dst->b7 = src[0xD];
        dst->bB = src[0xF];
        src += 0x10;
        dst++;
    }
    memset(work, 0, 0x40000);
    n = func_001BCA40(D_00295B10.count, D_00295B10.seqs, work);
    if (*(s32 *)(snd + 0x1C) + (n + D_00295B10.waveSize) > 0xBFFE0) {
        func_001B2870(D_00231AE8, n, D_00295B10.waveSize, *(s32 *)(snd + 0x1C));
    }
    r = func_001A6DA0(slot, (u32)work, n, (u32)D_00295B10.waveBase, D_00295B10.waveSize,
                      (u32)(snd + *(s32 *)(snd + 0x18)), *(s32 *)(snd + 0x1C));
    if (r < 0) {
        memset(d0, 0, sizeof(D_00295C50[0]));
        return r;
    }
    FlushCache(0);
    func_001A7280(1);
    return 0;
}
#endif

/* The bank's table and its first piece's waves: {.., 0x18 table offset, 0x1C size, 0x20 info
   offset, 0x24 info size} with the table and the waves after a 0x40-byte header. */
void func_001AC6B0(u8 *src, s32 size, s32 size2) {
    u8 *snd = D_003065DC[0];
    u8 *p;

    memset(snd, 0, 0x40);
    *(s32 *)(snd + 0x18) = 0x40;
    *(s32 *)(snd + 0x1C) = size;
    memcpy(snd + 0x40, src, size);
    p = (u8 *)((u32)(snd + 0x40) + size + 0x3F & ~0x3F);
    *(s32 *)(snd + 0x20) = p - snd;
    *(s32 *)(snd + 0x24) = size2;
    memcpy(p, src + size, size2);
}

void func_001AC750(u8 *seqs, u8 *waves) {
    D_00295B10.seqBase = seqs;
    D_00295B10.waveBase = waves;
    D_00295B10.count = 0;
    D_00295B10.seqSize = 0;
    D_00295B10.waveSize = 0;
}

/* Adds a piece (.snd: sequences at +8, waves at +0x10). */
void func_001AC780(u8 *snd);
#ifdef NON_MATCHING   /* first draft */
void func_001AC780(u8 *snd) {
    u8 *seq = D_00295B10.seqBase + D_00295B10.seqSize;

    memcpy(seq, snd + *(s32 *)(snd + 8), *(s32 *)(snd + 0xC));
    D_00295B10.seqs[D_00295B10.count] = seq;
    memcpy(D_00295B10.waveBase + D_00295B10.waveSize, snd + *(s32 *)(snd + 0x10), *(s32 *)(snd + 0x14));
    D_00295B10.seqSize += *(s32 *)(snd + 0xC);
    D_00295B10.count++;
    D_00295B10.waveSize += *(s32 *)(snd + 0x14);
}
#endif

extern char D_00238628[];   /* "s01_npc00" */

/* The NPC voice bank. */
void func_001D6E20(s32 unused) {
    func_001D6EF0(D_00238628, 0xD);
}

void func_001A6F20(void);

void func_001AC210(void) {
    func_001A6F20();
}

/* Merging sound-driver sequence banks: each piece is a 16-byte header, a 0x28-byte header of chunk
   offsets (-1: none) and up to five chunks, each {4 words: the 3rd its size, the 4th its last
   index} then a table of entry offsets (-1: none) and the entries. The merge keeps piece 0's
   headers and concatenates the entries, renumbering what entries refer to in the chunk before. */
typedef struct {
    u8 *base;           /* the piece */
    u8 *base2;
    u8 *hdr;            /* base + 0x10 */
    u8 *c24;            /* its chunks (or (u8 *)-1) */
    u8 *c14;
    u8 *c18;
    u8 *c1C;
    u8 *c20;
} SePiece;

extern u8 *D_003089D0;      /* the output */
extern u8 *D_003089D4;      /* its header */
extern u8 *D_003089D8;      /* its chunk-offset header */
extern u8 *D_003089E0;      /* the chunks written */
extern u8 *D_003089E4;
extern u8 *D_003089E8;
extern u8 *D_003089EC;
extern SePiece D_003089F0[16];
void func_0018D600(void *p, s32 c, u32 n);

#ifdef NON_MATCHING   /* first draft (the original copies field by field) */
#define CH_SIZE(c) (*(s32 *)((c) + 8))
#define CH_LAST(c) (*(s32 *)((c) + 0xC))
#define CH_OFF(c, j) (*(s32 *)((c) + 0x10 + (j) * 4))

static u8 *se_pad16(u8 *p, u8 *chunk) {
    u32 k = 0x10 - ((p - chunk) & 0xF);
    u32 i;

    if (k != 0x10) {
        for (i = 0; i < k; i++) {
            *p++ = 0xFF;
        }
    }
    return p;
}

static u8 *se_rel(u8 *base, s32 off) {
    return off == -1 ? (u8 *)-1 : base + off;
}

s32 func_001BCA40(s32 n, u8 **seqs, u8 *out) {
    u16 nA[16];
    u16 nB[16];
    u16 nC[16];
    u32 i;
    u32 j;
    u32 k;
    u8 *src;
    u8 *dst;
    s32 *tab;
    s32 waves;
    u16 acc;

    func_0018D600(nA, 0, 0x20);
    func_0018D600(nB, 0, 0x20);
    func_0018D600(nC, 0, 0x20);
    func_0018D600(&D_003089D0, 0, 0x20);
    func_0018D600(D_003089F0, 0, 0x200);
    for (i = 0; i < (u32)n; i++) {
        SePiece *p = &D_003089F0[i];
        p->base = seqs[i];
        p->base2 = p->base;
        p->hdr = p->base + 0x10;
        p->c24 = se_rel(p->base, *(s32 *)(p->hdr + 0x24));
        p->c14 = se_rel(p->base, *(s32 *)(p->hdr + 0x14));
        p->c18 = se_rel(p->base, *(s32 *)(p->hdr + 0x18));
        p->c1C = se_rel(p->base, *(s32 *)(p->hdr + 0x1C));
        p->c20 = se_rel(p->base, *(s32 *)(p->hdr + 0x20));
    }

    D_003089D0 = out;
    D_003089D4 = D_003089D0;
    memcpy(D_003089D4, D_003089F0[0].base2, 0x10);
    *(s32 *)(D_003089D4 + 8) = 0x10;
    D_003089D8 = D_003089D0 + 0x10;
    memcpy(D_003089D8, D_003089F0[0].hdr, 0x28);
    for (k = 0; k < 7; k++) {
        *(s32 *)(D_003089D8 + 0xC + k * 4) = -1;
    }
    dst = D_003089D8 + 0x28;
    for (k = 0; k < 0x18; k++) {
        *dst++ = 0xFF;
    }

    /* chunk 0x20: 8-byte entries; the first word moves by the wave bytes before */
    D_003089EC = dst;
    memcpy(D_003089EC, D_003089F0[0].c20, 0x10);
    CH_SIZE(D_003089EC) = -1;
    CH_LAST(D_003089EC) = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c20;
        if (src != (u8 *)-1) {
            nA[i] = CH_LAST(src) + 1;
            CH_LAST(D_003089EC) += nA[i];
        }
    }
    CH_LAST(D_003089EC)--;
    tab = (s32 *)(D_003089EC + 0x10);
    dst = (u8 *)tab + (CH_LAST(D_003089EC) + 1) * 4;
    waves = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c20;
        if (src == (u8 *)-1) {
            continue;
        }
        for (j = 0; j < nA[i]; j++) {
            s32 o = CH_OFF(src, j);
            if (o != -1) {
                *tab++ = dst - D_003089EC;
                memcpy(dst, src + o, 8);
                *(s32 *)dst += waves;
                dst += 8;
            } else {
                *tab++ = o;
            }
        }
        waves += *(s32 *)(D_003089F0[i].hdr + 0x10);
    }
    dst = se_pad16(dst, D_003089EC);
    CH_SIZE(D_003089EC) = dst - D_003089EC;

    /* chunk 0x1C: 0x2A-byte entries; the first halfword indexes chunk 0x20 */
    D_003089E8 = dst;
    memcpy(D_003089E8, D_003089F0[0].c1C, 0x10);
    CH_SIZE(D_003089E8) = -1;
    CH_LAST(D_003089E8) = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c1C;
        if (src != (u8 *)-1) {
            nB[i] = CH_LAST(src) + 1;
            CH_LAST(D_003089E8) += nB[i];
        }
    }
    CH_LAST(D_003089E8)--;
    tab = (s32 *)(D_003089E8 + 0x10);
    dst = (u8 *)tab + (CH_LAST(D_003089E8) + 1) * 4;
    acc = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c1C;
        if (src == (u8 *)-1) {
            continue;
        }
        for (j = 0; j < nB[i]; j++) {
            s32 o = CH_OFF(src, j);
            if (o != -1) {
                *tab++ = dst - D_003089E8;
                memcpy(dst, src + o, 0x2A);
                *(u16 *)dst += acc;
                dst += 0x2A;
            } else {
                *tab++ = o;
            }
        }
        acc += nA[i];
    }
    dst = se_pad16(dst, D_003089E8);
    CH_SIZE(D_003089E8) = dst - D_003089E8;

    /* chunk 0x18: {4 bytes, the 4th a count} then that many halfwords indexing chunk 0x1C */
    D_003089E4 = dst;
    memcpy(D_003089E4, D_003089F0[0].c18, 0x10);
    CH_SIZE(D_003089E4) = -1;
    CH_LAST(D_003089E4) = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c18;
        if (src != (u8 *)-1) {
            nC[i] = CH_LAST(src) + 1;
            CH_LAST(D_003089E4) += nC[i];
        }
    }
    CH_LAST(D_003089E4)--;
    tab = (s32 *)(D_003089E4 + 0x10);
    dst = (u8 *)tab + (CH_LAST(D_003089E4) + 1) * 4;
    acc = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c18;
        if (src == (u8 *)-1) {
            continue;
        }
        for (j = 0; j < nC[i]; j++, tab++) {
            s32 o = CH_OFF(src, j);
            if (o != -1) {
                u8 *e = src + o;
                u16 *v = (u16 *)(e + 4);
                *tab = dst - D_003089E4;
                memcpy(dst, e, 4);
                dst += 4;
                for (k = 0; k < e[3]; k++) {
                    *(u16 *)dst = *v++ + acc;
                    dst += 2;
                }
            } else {
                *tab = o;
            }
        }
        acc += nB[i];
    }
    dst = se_pad16(dst, D_003089E4);
    CH_SIZE(D_003089E4) = dst - D_003089E4;

    /* chunk 0x14: programs by number, each a 0x24-byte header and [4] 0x14-byte entries whose
       first halfword indexes chunk 0x18; two pieces may not use the same number */
    D_003089E0 = dst;
    memcpy(D_003089E0, D_003089F0[0].c14, 0x10);
    CH_SIZE(D_003089E0) = -1;
    *(u32 *)(D_003089E0 + 0xC) = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c14;
        if (src != (u8 *)-1 && *(u32 *)(D_003089E0 + 0xC) < *(u32 *)(src + 0xC)) {
            *(u32 *)(D_003089E0 + 0xC) = *(u32 *)(src + 0xC);
        }
    }
    for (k = 0; k <= *(u32 *)(D_003089E0 + 0xC); k++) {
        CH_OFF(D_003089E0, k) = -1;
    }
    dst = D_003089E0 + 0x10 + (*(u32 *)(D_003089E0 + 0xC) + 1) * 4;
    acc = 0;
    for (i = 0; i < (u32)n; i++) {
        src = D_003089F0[i].c14;
        if (src == (u8 *)-1) {
            continue;
        }
        for (j = 0; j <= *(u32 *)(src + 0xC); j++) {
            u8 *e;
            u8 *s;
            if (CH_OFF(src, j) == -1) {
                continue;
            }
            if (CH_OFF(D_003089E0, j) != -1) {
                return -4;
            }
            CH_OFF(D_003089E0, j) = dst - D_003089E0;
            e = src + CH_OFF(src, j);
            memcpy(dst, e, 0x24);
            dst += 0x24;
            s = e + 0x24;
            for (k = 0; k < e[4]; k++) {
                memcpy(dst, s, 0x14);
                s += 0x14;
                *(u16 *)dst += acc;
                dst += 0x14;
            }
        }
        acc += nC[i];
    }
    dst = se_pad16(dst, D_003089E0);
    CH_SIZE(D_003089E0) = dst - D_003089E0;

    *(s32 *)(D_003089D8 + 0xC) = dst - D_003089D0;
    *(s32 *)(D_003089D8 + 0x10) = waves;
    *(s32 *)(D_003089D8 + 0x14) = D_003089E0 - D_003089D0;
    *(s32 *)(D_003089D8 + 0x18) = D_003089E4 - D_003089D0;
    *(s32 *)(D_003089D8 + 0x1C) = D_003089E8 - D_003089D0;
    *(s32 *)(D_003089D8 + 0x20) = D_003089EC - D_003089D0;
    *(s32 *)(D_003089D8 + 0x24) = -1;
    return *(s32 *)(D_003089D8 + 0xC);
}
#endif

/* Sends a bank into slot 3 (the room's). */
s32 func_001B8AA0(u8 *snd) {
    return func_001AB9E0(snd, 3);
}

/* Sends a bank into slot 2. */
s32 func_001B8AB0(u8 *snd) {
    return func_001AB9E0(snd, 2);
}
