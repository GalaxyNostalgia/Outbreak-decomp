#include "task.h"
#include "cri.h"
#include "sce.h"

/* Game start-up, called once from the main loop's first frame (frame.c): sound driver, file
   system and archives, ADX streams, options, the movie player, clocks and timers. */

/* Sofdec (CRI movie player) start-up parameters: mwPlyInitSfdFx's MwsfdInitPrm */
typedef struct {
    f32 vhz;           /* display refresh rate */
    s32 dispCycle;
    s32 dispLatency;
    s8 decSvr;
    s8 pad[0x20 - 0x0D];
} MwsfdInitPrm;

void mwPlyInitSfdFx(MwsfdInitPrm *prm);

extern u8 D_003209B0[0x98];
extern void *D_0023BC70;
extern f32 D_00239C88;    /* 59.94 Hz */

void func_001CF5C0(void) {
    MwsfdInitPrm prm;

    D_0023BC70 = D_003209B0;
    memset(D_0023BC70, 0, 0x98);
    memset(&prm, 0, sizeof(prm));
    prm.dispCycle = 1;
    prm.dispLatency = 1;
    prm.decSvr = 0;
    prm.vhz = D_00239C88;
    mwPlyInitSfdFx(&prm);
}

void func_001AE9B0(void) {
}

extern u8 D_00321590[0x1000];
extern s32 D_0023A0C4;
extern s32 D_0023BC7C;
extern s32 D_0023BC80;
extern s32 D_003065DC[];

void func_001CFC00(void) {
    memset(D_00321590, 0, sizeof(D_00321590));
    D_0023A0C4 = -1;
    D_0023BC80 = D_003065DC[0];
    D_0023BC7C = D_003065DC[0] + 0x10000;
}

extern s32 D_0023BB2C;
extern s32 D_0023BB30;
extern s32 D_0029B0D0[8];

void func_001AEC00(void) {
    s32 i;

    D_0023BB30 = 0;
    i = 7;
    D_0023BB2C = 0;
    do {
        D_0029B0D0[i] = 0;
        i--;
    } while (i >= 0);
}

/* sceCdCLOCK: the console's real-time clock, BCD */
typedef struct {
    u8 stat;
    u8 second;
    u8 minute;
    u8 hour;
    u8 pad;
    u8 day;
    u8 month;
    u8 year;
} sceCdCLOCK;

s32 sceCdReadClock(sceCdCLOCK *clock);

extern s32 D_00239F98;          /* frames until the next clock read */
extern sceCdCLOCK D_0023BB88;

/* Read the clock every 20 calls. */
void func_001B9720(void) {
    if (--D_00239F98 == 0) {
        D_00239F98 = 20;
        sceCdReadClock(&D_0023BB88);
    }
}

extern u8 D_0023BB28;
extern u16 D_0023BB9C;
extern u16 D_0023BBA0;

void func_001ADED0(void) {
    D_0023BB28 = 0;
    D_0023BBA0 = D_0023BB88.second;
    D_0023BB9C = D_0023BB88.second;
}


void func_001B63A0(void) {
    memset(D_002F85D0, 0, sizeof(D_002F85D0));
}

s32 func_001ACEA0(void);
void func_001B69C0(void);
void func_001ADFC0(void);
void func_001B9240(void);
extern s32 D_0023BB80;

void func_001B9830(void) {
    func_001ACEA0();
    func_001B69C0();
    func_001ADFC0();
    func_001B9240();
    func_001AE9B0();
    func_001CFC00();
    func_001AEC00();
    D_00239F98 = 1;
    func_001B9720();
    func_001ADED0();
    func_001B63A0();
    D_0023BB80 = 0;
}

extern void *D_0023BB50;
extern u8 D_00217080[];

void func_001B6E70(s32 unused) {
    D_0023BB50 = D_00217080;
}

u32 func_0019BF60(u32 size);    /* GS memory block (vram.c) */
extern u16 D_003065E4[];

void func_001B3980(void) {
    D_003065E4[0] = func_0019BF60(0x2000);
}

void func_001B28B0(s32 unused) {
}

void func_001B28A0(void) {
}

extern u8 D_002F8800[0x20];
extern u8 D_002F8820[0x18];

void func_001BA940(void) {
    memset(D_002F8800, 0, sizeof(D_002F8800));
}

void func_001BA540(void) {
    memset(D_002F8820, 0, sizeof(D_002F8820));
    func_001BA940();
}

void func_001AB9D0(void);
void func_001AB900(void);

void func_001B97D0(void) {
    func_001AB9D0();
    func_001AB900();
    func_001B6E70(0);
    func_001B9830();
    func_001B3980();
    func_001B28B0(0);
    func_001B28A0();
    func_001BA540();
}
