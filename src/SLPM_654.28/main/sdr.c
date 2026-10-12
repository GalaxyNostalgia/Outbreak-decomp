#include "kernel.h"
#include "sce.h"
#include "system.h"

/* SDR: the EE side of the Tamsoft Sound Driver (TSNDDRV.IRX on the IOP). Commands go over
   two SIF RPC channels: TSNDDRV_DEV sends commands, TSNDGET_DEV reads the driver's state. */

typedef struct {
    u32 pkt_addr;
    u32 rpc_id;
    s32 sema_id;
    u32 mode;
    u32 command;
    void *buff;
    void *cbuff;
    void (*end_function)(void *);
    void *end_param;
    void *server;           /* 0x24: set once the IOP side answers */
} sceSifClientData;

s32 sceSifBindRpc(sceSifClientData *cd, u32 rpc_number, u32 mode);
s32 sceSifCallRpc(sceSifClientData *cd, u32 fno, u32 mode, void *send, s32 ssize, void *recv, s32 rsize,
                  void (*end_func)(void *), void *end_param);
s32 SetAlarm(u16 time, void (*handler)(s32, u16, void *), void *arg);
s32 SleepThread(void);
s32 func_00115210(s32 thread);   /* wakes a thread from an interrupt handler */

#define SDR_DEV 0x00000000       /* RPC ids of the two driver channels */
#define SDR_GET 0x00000001

typedef struct {
    s32 id;
    s32 a;
    s32 b;
} SdrSlot;

extern s32 D_00292DA8[];
extern s32 D_00292DB0[];
extern s32 D_00292DB8[];
extern SdrSlot D_00292DC0[32];

extern sceSifClientData D_00292F40;   /* TSNDGET_DEV */
extern sceSifClientData D_00292F70;   /* TSNDDRV_DEV */
extern s32 D_00292FC0[4];             /* state request buffer */
extern s32 D_00239F18;                /* semaphore: command channel */
extern s32 D_00239F1C;                /* semaphore: state channel */
extern s32 D_0023BAF0;                /* connected */
extern s32 D_0023BB0C;

extern char D_00223BB0[];   /* "SDR: SdrDelayThread: Error: SetAlarm faild.\n" */
extern char D_00223BE0[];   /* "SDR: SdrInit: Error: TSNDDRV_DEV: sceSifBindRpc faild.\n" */
extern char D_00223C20[];   /* "SDR: SdrInit: Error: TSNDGET_DEV: sceSifBindRpc faild.\n" */
extern char D_00223C60[];   /* "SDR: SdrInit: Error: SmId_send faild.\n" */
extern char D_00223C90[];   /* "SDR: SdrInit: Error: SmId_get faild.\n" */
extern char D_00223CC0[];   /* "Connected to TamsoftSoundDriver: Version %d.%d (C)Tamsoft\n" */
extern char D_002240C0[];   /* "SDR: SdrGetStateSend: Error: Rpc faild.\n" */

s32 printf(const char *fmt, ...);
void func_001AC860(void);
void *memset(void *dst, s32 c, u32 n);

/* SdrInitSlots */
void func_001A65E0(void) {
    s32 i = 31;

    D_00292DA8[0] = 0;
    D_00292DB8[0] = 0;
    D_00292DB0[0] = 0;
    do {
        D_00292DC0[i].id = -1;
        i--;
    } while (i >= 0);
}

/* SdrDelayThread's alarm: wake the sleeping thread */
void func_001A6550(s32 id, u16 time, void *thread) {
    func_00115210((s32)thread);
    SYNC_EI();
}

/* SdrDelayThread: sleep for `hsync` horizontal blanks */
s32 func_001A6580(s32 hsync) {
    if (SetAlarm(hsync & 0xFFFF, func_001A6550, (void *)GetThreadId()) < 0) {
        printf(D_00223BB0);
        return -2;
    }
    return SleepThread();
}

/* SdrBind: bind an RPC channel, waiting until the IOP side is up */
s32 func_001A6630(sceSifClientData *cd, u32 id) {
loop:
    if (sceSifBindRpc(cd, id, 0) < 0) {
        return -1;
    }
    if (cd->server == NULL) {
        if (func_001A6580(1) < 0) {
            return -1;
        }
        goto loop;
    }
    return 0;
}

/* SdrInit */
s32 func_001A66A0(void) {
    struct SemaParam sp;
    s32 id;

    if (D_0023BAF0 != 0) {
        return 0;
    }
    func_001A65E0();
    if (func_001A6630(&D_00292F70, SDR_DEV) < 0) {
        printf(D_00223BE0);
        return -1;
    }
    if (func_001A6630(&D_00292F40, SDR_GET) < 0) {
        printf(D_00223C20);
        return -1;
    }
    sp.initCount = 1;
    sp.maxCount = 1;
    sp.option = 0;
    if (D_00239F18 < 0) {
        if ((id = CreateSema(&sp)) <= 0) {
            printf(D_00223C60);
            return -1;
        }
        D_00239F18 = id;
    }
    if (D_00239F1C < 0) {
        if ((id = CreateSema(&sp)) <= 0) {
            printf(D_00223C90);
            return -1;
        }
        D_00239F1C = id;
    }
    D_0023BAF0 = 1;
    printf(D_00223CC0, 2, 0);
    return 0;
}

/* end of a state request: release the channel (the semaphore id comes as the parameter) */
void func_001A7430(void *sema) {
    iSignalSema((s32)sema);
}

/* SdrGetStateSend: ask the driver for its state (answer arrives in D_00292FC0) */
s32 func_001A7440(u32 cmd, s32 arg) {
    WaitSema(D_00239F1C);
    D_00292FC0[0] = arg;
    if (sceSifCallRpc(&D_00292F40, cmd, 1, D_00292FC0, 16, D_00292FC0, 16, func_001A7430, (void *)D_00239F1C) < 0) {
        printf(D_002240C0);
        SignalSema(D_00239F1C);
        return -9;
    }
    return 0;
}

s32 func_001AB980(void) {
    func_001A66A0();
    func_001A7440(0x10, 0);
    D_0023BB0C = 0;
    func_001AC860();
    return 1;
}

void func_001AB9D0(void) {
    func_001AB980();
}

extern u8 D_00295B40[0x108];
extern s32 D_00295B48[];
extern s32 D_003065DC[];

void func_001AC860(void) {
    memset(D_00295B40, 0, sizeof(D_00295B40));
    if (D_003065DC[0] != 0) {
        D_00295B48[0] = D_003065DC[0] + 0x80000;
    }
}

/* Sound command queue: 32 slots, a slot is free while its first word is negative. A command word
   is (opcode << 24) | 24-bit argument. */
extern char D_00223D90[];   /* "SDR: SdrSeMasterVol: Warning: sndque overflow!\n" */
extern char D_00223DC0[];   /* "SDR: SdrMasterVol: Warning: sndque overflow!\n" */
extern char D_00223F30[];   /* "SDR: SdrAllMute: Warning: sndque overflow!\n" */

#ifdef NON_MATCHING   /* reads the write index once; the original reloads it after the store */
/* SdrSeMasterVol */
s32 func_001A6B30(u16 vol) {
    SdrSlot *s = &D_00292DC0[D_00292DB0[0]];

    if (s->id >= 0) {
        printf(D_00223D90);
        return -1;
    }
    s->id = 0x29000000 | ((u32)(vol << 8) & 0xFFFFFF);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    return 0;
}
#endif

#ifdef NON_MATCHING   /* reads the write index once; the original reloads it after the store */
/* SdrMasterVol */
s32 func_001A6BD0(u8 ch, u16 vol) {
    SdrSlot *s = &D_00292DC0[D_00292DB0[0]];

    if (s->id >= 0) {
        printf(D_00223DC0);
        return -1;
    }
    s->id = 0x4C000000 | ((u32)((ch << 16) | vol) & 0xFFFFFF);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    return 0;
}
#endif

#ifdef NON_MATCHING   /* reads the write index once; the original reloads it after the store */
/* SdrAllMute */
s32 func_001A7050(s32 mute) {
    SdrSlot *s = &D_00292DC0[D_00292DB0[0]];

    if (s->id >= 0) {
        printf(D_00223F30);
        return -1;
    }
    s->id = 0x41000000 | ((u32)((mute != 0) << 16) & 0xFFFFFF);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    return 0;
}
#endif

extern char D_00223F60[];   /* "SDR: SdrSetOutputMode?: Warning: sndque overflow!\n" */

#ifdef NON_MATCHING   /* reads the write index once; the original reloads it after the store */
/* stereo (1) / mono (0) output */
s32 func_001A70F0(s32 stereo) {
    SdrSlot *s = &D_00292DC0[D_00292DB0[0]];

    if (s->id >= 0) {
        printf(D_00223F60);
        return -1;
    }
    s->id = 0x42000000 | ((u32)((stereo != 0) << 16) & 0xFFFFFF);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    return 0;
}
#endif
