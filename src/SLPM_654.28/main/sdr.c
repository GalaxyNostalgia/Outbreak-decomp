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
extern volatile s32 D_00292DB0[];   /* queue write index (reloaded on every use) */
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

extern char D_00223F60[];   /* "SDR: SdrSetOutputMode?: Warning: sndque overflow!\n" */

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

/* Sending: each frame the queued commands are packed into a byte stream (D_00293000, ended by 0xFF)
   and sent to the driver in one RPC. */

extern u8 D_00293000[0x200];
extern char D_00224030[];   /* "SDR: SdrSendReq: Caution: sbuff overflow. ..." */
extern char D_00224090[];   /* "SDR: SdrSendReq: Error: Rpc faild.\n" */
extern char D_002240F0[];   /* "SDR: snddrv.c: makebuff8: Error: Length over\n" */
extern char D_00224120[];   /* "SDR: snddrv.c: makebuff: Error: Length over\n" */
extern char D_00224150[];   /* "SDR: snddrv.c: makebuff_ext: Error: Length invalid\n" */
extern char D_00224190[];   /* "SDR: snddrv.c: sending_req: Error: unknown command\n" */

/* A command with optional bytes: flags in the opcode's low bits say which follow. */
s32 func_001A7580(u32 cmd, u8 a, u8 b, u16 c, u8 d) {
    u32 op = cmd >> 24;
    s32 len;
    s32 hasA = op & 1;
    s32 hasB;
    s32 hasC;
    u8 *p;

    len = 4;

    if (hasA) {
        len++;
    }
    hasB = op & 2;
    hasC = op & 4;
    if (hasB) {
        len++;
    }
    if (hasC) {
        len += 2;
    }
    if (D_00292DA8[0] + len >= 0x200) {
        return -1;
    }
    p = &D_00293000[D_00292DA8[0]];
    p[0] = op;
    p[1] = cmd >> 16;
    p[2] = cmd >> 8;
    p[3] = cmd;
    p += 4;
    if (hasA) {
        *p++ = a;
    }
    if (hasB) {
        *p++ = b;
    }
    if (hasC) {
        p[0] = (c & 0xFFFF) >> 8;
        p[1] = c & 0xFFFF;
        p += 2;
    }
    p[0] = d;
    D_00292DA8[0] += len + 1;
    p[1] = 0xFF;
    return 0;
}

/* Opcode 0x35: the command word and three more bytes. */
s32 func_001A7660(SdrSlot *s) {
    s32 sh;
    s32 n;
    u32 w;

    if (D_00292DA8[0] + 8 >= 0x200) {
        return -1;
    }
    w = s->id;
    sh = 24;
    n = 4;
    do {
        D_00293000[D_00292DA8[0]] = w >> sh;
        n--;
        sh -= 8;
        D_00292DA8[0]++;
    } while (n > 0);
    w = s->a;
    sh = 16;
    n = 3;
    do {
        D_00293000[D_00292DA8[0]] = w >> sh;
        n--;
        sh -= 8;
        D_00292DA8[0]++;
    } while (n > 0);
    D_00293000[D_00292DA8[0]] = 0xFF;
    return 0;
}

/* makebuff8: up to 4 bytes of the command word, then up to 4 more bytes */
s32 func_001A7730(u32 cmd, s32 len, u8 b0, u8 b1, u8 b2, u8 b3) {
    s32 sh;
    s32 n;

    if (len > 8) {
        printf(D_002240F0);
        return -1;
    }
    if (D_00292DA8[0] + len >= 0x200) {
        return -1;
    }
    for (sh = 24, n = 4; n > 0 && len > 0; sh -= 8, n--, len--) {
        D_00293000[D_00292DA8[0]++] = cmd >> sh;
    }
    if (len-- > 0) {
        D_00293000[D_00292DA8[0]++] = b0;
    }
    if (len-- > 0) {
        D_00293000[D_00292DA8[0]++] = b1;
    }
    if (len-- > 0) {
        D_00293000[D_00292DA8[0]++] = b2;
    }
    if (len-- > 0) {
        D_00293000[D_00292DA8[0]++] = b3;
    }
    D_00293000[D_00292DA8[0]] = 0xFF;
    return 0;
}

/* makebuff: the first len bytes of the command word */
s32 func_001A78D0(u32 cmd, s32 len) {
    s32 sh;

    if (len > 4) {
        printf(D_00224120);
        return -1;
    }
    if (D_00292DA8[0] + len >= 0x200) {
        return -1;
    }
    for (sh = 24; len > 0; len--) {
        D_00293000[D_00292DA8[0]] = cmd >> sh;
        sh -= 8;
        D_00292DA8[0]++;
    }
    D_00293000[D_00292DA8[0]] = 0xFF;
    return 0;
}

/* makebuff_ext: the first len bytes of a command that goes on for total bytes (no end mark yet) */
s32 func_001A7980(u32 cmd, s32 len, s32 total) {
    s32 sh;

    if (len >= total || len > 4) {
        printf(D_00224150);
        return -1;
    }
    if (D_00292DA8[0] + total >= 0x200) {
        return -1;
    }
    for (sh = 24; len > 0; len--) {
        D_00293000[D_00292DA8[0]] = cmd >> sh;
        sh -= 8;
        D_00292DA8[0]++;
    }
    return 0;
}

#define SLOT_U8(s, off) (((u8 *)(s))[off])
#define SLOT_U16(s, off) (*(u16 *)((u8 *)(s) + (off)))

#ifdef NON_MATCHING   /* keeps the command word in a0; the original in v0 (and three instructions fewer) */
/* sending_req: pack one queued command; its opcode says how many bytes it takes. */
s32 func_001A7A20(SdrSlot *s) {
    s32 cmd = s->id;
    s32 op = cmd >> 24;
    s32 grp;

    if (op == 0x7F) {
        return 0;
    }
    grp = op & 0xF0;
    if (op == -1) {
        return 0;
    }
    switch (grp) {
    case 0x00:
        return func_001A7580(cmd, SLOT_U8(s, 4), SLOT_U8(s, 5), SLOT_U16(s, 6), SLOT_U8(s, 8));
    case 0x20:
        if (op & 8) {
            if (op == 0x29) {
                return func_001A78D0(cmd, 3);
            }
            return func_001A78D0(cmd, 1);
        }
        if (op == 0x22 || op == 0x20 || op == 0x24 || op == 0x23) {
            return func_001A78D0(cmd, 3);
        }
        return func_001A78D0(cmd, 2);
    case 0x30:
    case 0x70:
        if (op == 0x38 || op == 0x30 || op == 0x71 || op == 0x73) {
            return func_001A78D0(cmd, 2);
        }
        if (op == 0x31 || op == 0x37 || (op >= 0x39 && op <= 0x3B) || op == 0x72 || op == 0x75) {
            return func_001A78D0(cmd, 3);
        }
        if (op == 0x3D || op == 0x36 || op == 0x74) {
            return func_001A78D0(cmd, 4);
        }
        if (op == 0x77 || op == 0x3C || op == 0x3E) {
            return func_001A7730(cmd, 5, SLOT_U8(s, 4), 0, 0, 0);
        }
        if (op == 0x3F) {
            return func_001A7730(cmd, 7, SLOT_U8(s, 4), SLOT_U8(s, 5), SLOT_U8(s, 6), 0);
        }
        if (op == 0x35) {
            return func_001A7660(s);
        }
        return func_001A78D0(cmd, 1);
    case 0x40:
        if ((op >= 0x47 && op <= 0x4A) || op == 0x41 || op == 0x42) {
            return func_001A78D0(cmd, 2);
        }
        if (op == 0x4B) {
            return func_001A78D0(cmd, 3);
        }
        if (op == 0x45 || op == 0x4C) {
            return func_001A78D0(cmd, 4);
        }
        if (op == 0x44) {
            return func_001A7730(cmd, 6, SLOT_U8(s, 4), SLOT_U8(s, 5), 0, 0);
        }
        if (op == 0x4D || op == 0x4E) {
            return func_001A7980(cmd, 3, 0x15);
        }
        if (op == 0x4F) {
            return func_001A7730(s->a, 6, (cmd >> 8) & 0xFF, cmd & 0xFF, 0, 0);
        }
        return func_001A78D0(cmd, 1);
    case 0x60:
    case 0x50:
        return func_001A78D0(cmd, 2);
    default:
        printf(D_00224190);
        return -1;
    }
}
#endif

s32 func_001A7A20(SdrSlot *s);

/* SdrSendReq: pack the queued commands and send them (nowait: give up if the last send is still
   going). The RPC's end callback releases the semaphore. */
s32 func_001A7280(s32 nowait) {
    s32 i;
    SdrSlot *s;
    s32 size;

    if (nowait != 0) {
        if (PollSema(D_00239F18) < 0) {
            return -1;
        }
    } else {
        WaitSema(D_00239F18);
    }
    i = D_00292DB8[0];
    while ((s = &D_00292DC0[i])->id >= 0) {
        if (func_001A7A20(s) < 0) {
            printf(D_00224030);
            break;
        }
        s->id = -1;
        i = (i + 1) % 32;
    }
    D_00292DB8[0] = i;
    if (D_00292DA8[0] != 0) {
        size = (D_00292DA8[0] + 16) & ~15;
        if (nowait != 0) {
            nowait = 1;
        }
        if (sceSifCallRpc(&D_00292F70, SDR_DEV, nowait, D_00293000, size, D_00293000, 0,
                          func_001A7430, (void *)D_00239F18) < 0) {
            printf(D_00224090);
            SignalSema(D_00239F18);
            return -1;
        }
        D_00292DA8[0] = 0;
    } else {
        SignalSema(D_00239F18);
    }
    return 0;
}

s32 func_001AB9C0(s32 nowait) {
    return func_001A7280(nowait);
}

extern char D_00223ED0[];   /* "SDR: SdrPortClr: Warning: sndque overflow!\n" */

/* SdrPortClr */
s32 func_001A6FB0(s32 port) {
    SdrSlot *s = &D_00292DC0[D_00292DB0[0]];

    if (s->id >= 0) {
        printf(D_00223ED0, s);
        return -1;
    }
    s->id = 0x4A000000 | ((u32)((port & 0xFF) << 16) & 0xFFFFFF);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    return 0;
}

#ifdef NON_MATCHING   /* first draft */
/* Three transfers (0x4F): the driver reads each block from EE memory. */
void func_001A6C70(u32 a0, s32 n0, u32 a1, s32 n1, u32 a2, s32 n2) {
    SdrSlot *s;

    n0 = (n0 + 15) & ~15;
    s = &D_00292DC0[D_00292DB0[0]];
    s->id = 0x4F000000 | ((u32)(n0 >> 4) & 0xFFFFFF);
    s->a = a0 | (n0 >> 20);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    n1 = (n1 + 15) & ~15;
    s = &D_00292DC0[D_00292DB0[0]];
    s->id = 0x4F000000 | ((u32)(n1 >> 4) & 0xFFFFFF);
    s->a = a1 | (n1 >> 20);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    n2 = (n2 + 15) & ~15;
    s = &D_00292DC0[D_00292DB0[0]];
    s->id = 0x4F000000 | ((u32)(n2 >> 4) & 0xFFFFFF);
    s->a = a2 | (n2 >> 20);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
}
#endif

extern char D_00223DF0[];   /* bank number error */
extern char D_00223E20[];   /* alignment error */
extern char D_00223E70[];   /* size error */
extern char D_00223EA0[];   /* "sndque overflow" */

#ifdef NON_MATCHING   /* first draft */
/* SdrTransBank: send sound bank `bank` (header, wave table, waves; each 16-byte aligned). */
s32 func_001A6DA0(u32 bank, u32 a0, s32 n0, u32 a1, s32 n1, u32 a2, s32 n2) {
    s32 i;
    s32 n;

    if (bank >= 0x1000) {
        printf(D_00223DF0);
        return -2;
    }
    if ((a2 | (a0 | a1)) & 0xF) {
        printf(D_00223E20);
        return -3;
    }
    if (n0 >= 0x1FFFF1 || n1 >= 0x1FFFF1 || n2 >= 0x1FFFF1) {
        printf(D_00223E70);
        return -4;
    }
    for (i = D_00292DB0[0], n = 4; n > 0; n--) {
        if (D_00292DC0[i].id >= 0) {
            printf(D_00223EA0);
            return -1;
        }
        i = (i + 1) % 32;
    }
    D_00292DC0[D_00292DB0[0]].id = 0x4D000000 | ((u32)(bank << 8) & 0xFFFFFF);
    D_00292DB0[0] = (D_00292DB0[0] + 1) % 32;
    func_001A6C70(a0, n0, a1, n1, a2, n2);
    return 0;
}
#endif

/* Has the last send finished? (nowait: 1 if it hasn't) */
s32 func_001A73E0(s32 nowait) {
    if (nowait != 0) {
        if (PollSema(D_00239F18) < 0) {
            return 1;
        }
    } else {
        WaitSema(D_00239F18);
    }
    SignalSema(D_00239F18);
    return 0;
}

/* The driver state asked for with SdrGetStateSend */
s32 func_001A74E0(s32 nowait) {
    s32 v;

    if (nowait != 0) {
        if (PollSema(D_00239F1C) < 0) {
            return -1;
        }
    } else {
        WaitSema(D_00239F1C);
    }
    v = D_00292FC0[0];
    SignalSema(D_00239F1C);
    return v;
}

s32 func_001A7440(u32 cmd, s32 arg);

/* Ask for a state value and read it */
s32 func_001A7540(u32 cmd, s32 arg) {
    s32 r = func_001A7440(cmd, arg);

    if (r < 0) {
        return r;
    }
    return func_001A74E0(0);
}
