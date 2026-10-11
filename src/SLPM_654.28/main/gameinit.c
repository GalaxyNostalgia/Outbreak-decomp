#include "system.h"
#include "sce.h"

/* Game start-up: heaps for the game's systems, the build id. */

/* Heap marks: up to 8 nested save points of the main heap, each with how much was taken since. */
typedef struct {
    u32 *mark;
    s32 size;
} HeapMark;

extern HeapMark D_0029B100[8];
extern s32 D_0023BB20;          /* deepest mark used */
extern s32 D_0023BB24;          /* most memory taken under one mark */

void func_001ADEA0(void) {
    memset(D_0029B100, 0, sizeof(D_0029B100));
    D_0023BB20 = 0;
    D_0023BB24 = 0;
}

extern u8 D_00306590[0xE0];

void func_001ADE70(void) {
    memset(D_00306590, 0, sizeof(D_00306590));
    func_001ADEA0();
}

s32 func_0018D680(u32 *mark);
void func_001AE160(void);
void func_001B98A0(void);
void func_001D9790(void);

void func_001AE9E0(u32 *mark) {
    s32 i = 0;
    HeapMark *m = D_0029B100;

    for (; i < 8; i++, m++) {
        if (m->mark == NULL) {
            break;
        }
    }
    if (i == 8) {
        while (1) {
        }
    }
    func_0018D680(mark);
    if (D_0023BB20 < i) {
        D_0023BB20 = i;
    }
    m->mark = mark;
}

/* Take memory under the innermost mark. */
u32 func_001AE950(u32 size) {
    s32 i = 7;
    HeapMark *m = &D_0029B100[7];

    for (; i >= 0; i--, m--) {
        if (m->mark != NULL) {
            break;
        }
    }
    m->size += size;
    if (D_0023BB24 < m->size) {
        D_0023BB24 = m->size;
    }
    return func_0018D660(size);
}

void func_001AEB40(void) {
}

/* A simple arena carved from the big block. */
typedef struct {
    u32 base;
    u32 size;
    u32 used;
    u32 left;
} Arena;

extern Arena D_0029B0F0;

void func_001AEB50(u32 base, u32 size) {
    memset(&D_0029B0F0, 0, sizeof(Arena));
    D_0029B0F0.base = base;
    D_0029B0F0.size = size;
    D_0029B0F0.left = size;
}

u32 func_001AEBB0(u32 size) {
    Arena *a = &D_0029B0F0;
    u32 p = 0;

    if (a->left >= size) {
        p = a->base + a->used;
        a->used += size;
        a->left -= size;
    }
    return p;
}

extern u8 D_0023BB38[8];

void func_001AFB80(void) {
    memset(D_0023BB38, 0xFF, sizeof(D_0023BB38));
}

s32 func_001A85B0(s32 unused) {
    return 1;
}

/* The build id, from the date and time the executable was built: "YYMMDDhhmm". */
extern char D_00217248[];       /* "Nov 17 2003" */
extern char D_00217258[];       /* "22:18:28" */
extern char *D_00217270[12];    /* "Jan" ... "Dec" */
extern char D_00232020[];       /* "%02d" */
extern char D_00232028[];       /* "0%s" */
extern char D_00232030[];       /* "%s" */
extern char D_00232038[];       /* "%s%s%s%s%s" */
extern char D_002F87F0[];
char *strtok(char *s, const char *delim);
s32 strcmp(const char *a, const char *b);

/* Not matching yet: stack layout of the local buffers. */
#ifdef NON_MATCHING
void func_001B98A0(void) {
    char date[0x20];
    char time[0x20];
    char minute[8];
    char hour[8];
    char day[8];
    char month[8];
    char year2[8];
    char hh[8];
    char mi[8];
    char mon[8];
    char dd[8];
    char year[8];
    char delim[] = " :\t\n";
    s32 i;

    strcpy(date, D_00217248);
    strcpy(time, D_00217258);
    strcpy(mon, strtok(date, delim));
    strcpy(dd, strtok(NULL, delim));
    strcpy(year, strtok(NULL, delim));
    strcpy(year2, year + 2);
    for (i = 0; i < 12; i++) {
        if (strcmp(D_00217270[i], mon) == 0) {
            sprintf(month, D_00232020, i + 1);
            break;
        }
    }
    if (strlen(dd) == 1) {
        sprintf(day, D_00232028, dd);
    } else {
        sprintf(day, D_00232030, dd);
    }
    strcpy(hh, strtok(time, delim));
    strcpy(mi, strtok(NULL, delim));
    sprintf(hour, D_00232030, hh);
    sprintf(minute, D_00232030, mi);
    sprintf(D_002F87F0, D_00232038, year2, month, day, hour, minute);
}
#endif

extern u8 D_00324580[0x8000];
extern u8 D_0032C580[0x8000];
typedef struct {
    char build[10];     /* "YYMMDDhh" */
    char version[10];   /* "1000" */
    s32 unk14;
} BuildInfo;

extern BuildInfo D_00334580;
extern char D_00239B90[];       /* "1000" */
char *strncpy(char *dst, const char *src, u32 n);

/* Not matching yet: the original keeps the struct address in a register. */
#ifdef NON_MATCHING
void func_001D9790(void) {
    BuildInfo *info = &D_00334580;

    memset(D_00324580, 0, sizeof(D_00324580));
    memset(D_0032C580, 0, sizeof(D_0032C580));
    strncpy(info->build, D_002F87F0, 8);
    strncpy(info->version, D_00239B90, 4);
    info->unk14 = 0;
}
#endif

/* The game's persistent state (what goes into a save), cleared at start-up. */
typedef struct {
    u8 unk0[0x1C];
    u8 unk1C;
    u8 unk1D[3];
    u8 nameCount;               /* 0x20 */
    u8 unk21[2];
    u8 unk23;
    u8 unk24[0x9C - 0x24];
    char names[1][0x18];        /* 0x9C */
    u8 unkB4[0x2394 - 0xB4];
    s32 unk2394;
    u8 unk2398[0x4078 - 0x2398];
    s32 unk4078;
    u8 unk407C[0x4094 - 0x407C];
    u32 unlocked[(0x4234 - 0x4094) / 4];     /* 0x4094: bit set */
} GameState;

extern GameState D_00301D60;
extern s16 D_002167D0[];        /* bits set at start, -1 terminated */
extern u8 D_00239F58[];         /* name slots, 0xFF terminated */
extern char *D_002167B0[];
extern char D_00231C70[];       /* "%s" */

/* Not matching yet: first draft. */
#ifdef NON_MATCHING
void func_001AE160(void) {
    s32 i;
    s16 bit;
    s8 n;
    u8 slot;

    memset(&D_00301D60, 0, sizeof(GameState));
    D_00301D60.unk1C = 1;
    D_00301D60.unk4078 |= 1;
    D_00301D60.unk2394 |= 1;
    D_00301D60.unk23 |= 3;
    for (i = 0; (bit = D_002167D0[i]) != -1; i++) {
        D_00301D60.unlocked[bit >> 5] |= 1 << (bit & 0x1F);
    }
    for (n = 0; (slot = D_00239F58[n]) != 0xFF; n++) {
        sprintf(D_00301D60.names[slot], D_00231C70, D_002167B0[n]);
    }
    D_00301D60.nameCount = n;
}
#endif

/* Memory for the game's systems: a 5.75 MiB block from the heap, split into named areas. */
extern s16 D_0023BBA4;
extern s16 D_0023BB94;
extern u32 D_0023BB68[2];       /* heap mark for the game */
typedef struct {
    u32 a3065D8;
    u32 a3065DC;
    u32 a3065E0;
    u8 pad3065E4[0xc];
    u32 a3065F0;
    u32 a3065F4;
    u32 a3065F8;
    u32 a3065FC;
    u32 a306600;
    u32 a306604;
    u32 a306608;
    u32 a30660C;
    u32 a306610;
    u32 a306614;
    u32 a306618;
    u32 a30661C;
    u32 a306620;
    u32 a306624;
    u32 a306628;
    u32 a30662C;
    u32 a306630;
    u32 a306634;
    u32 a306638;
    u32 a30663C;
    u32 a306640;
} GameAreas;

extern GameAreas D_003065D8;   /* addresses of the game's memory areas */
void func_001A02A0(s32 wait);
void func_001A02B0(s32 value);

/* Not matching yet: constant and register order near the end (11 instructions). */
#ifdef NON_MATCHING
void func_001B94E0(void) {
    func_001A85B0(0);
    D_0023BBA4 = 0;
    func_001AFB80();
    func_001ADE70();
    D_0023BB94 = 0;
    func_001AEB40();
    func_001A02A0(1);
    func_001AE9E0(D_0023BB68);
    D_003065D8.a3065E0 = func_001AE950(0x5C0000);
    memset((void *)D_003065D8.a3065E0, 0, 0x5C0000);
    D_003065D8.a3065D8 = D_003065D8.a3065E0;
    D_003065D8.a3065DC = D_003065D8.a3065E0 + 0x240000;
    func_001AEB50(D_003065D8.a3065D8, 0x240000);
    D_003065D8.a306624 = func_001AEBB0(0x3C000);
    func_001A02B0(D_003065D8.a306624);
    D_003065D8.a306618 = func_001AEBB0(0xC000);
    D_003065D8.a30661C = func_001AEBB0(0xC000);
    D_003065D8.a3065F4 = func_001AEBB0(0x18000);
    D_003065D8.a3065FC = func_001AEBB0(0x20000);
    D_003065D8.a30662C = func_001AEBB0(0x18000);
    D_003065D8.a30663C = func_001AEBB0(0x10000);
    D_003065D8.a30660C = func_001AEBB0(0x10000);
    D_003065D8.a3065F8 = func_001AEBB0(0x80000);
    D_003065D8.a306614 = func_001AEBB0(0x10000);
    D_003065D8.a306640 = func_001AEBB0(0x18000);
    D_003065D8.a306608 = func_001AEBB0(0x80000);
    D_003065D8.a306628 = func_001AEBB0(0x4000);
    D_003065D8.a306634 = func_001AEBB0(0xC000);
    D_003065D8.a306638 = func_001AEBB0(0x5400);
    D_003065D8.a306604 = D_003065D8.a3065F4 + 0x8000;
    D_003065D8.a3065F0 = D_003065D8.a3065DC;
    D_003065D8.a306610 = D_003065D8.a3065DC + 0xA0000;
    D_003065D8.a306600 = D_003065D8.a3065F0 + 0x180000;
    D_003065D8.a306630 = ((u32)malloc(0x110000 + 0x3F) + 0x3F) & ~0x3F;
    D_003065D8.a306620 = D_003065D8.a306610 + 0x18000;
    func_001AE160();
    func_001B98A0();
    func_001D9790();
}
#endif
