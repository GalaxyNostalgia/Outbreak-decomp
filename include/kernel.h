#ifndef KERNEL_H
#define KERNEL_H

#include "common.h"

/* Sony EE kernel (libkernel) and SIF/IOP calls the game makes. */

struct ThreadParam {
    s32 status;
    void (*entry)(void *);
    void *stack;
    s32 stackSize;
    void *gpReg;
    s32 initPriority;
    s32 currentPriority;
    u32 attr;
    u32 option;
    s32 waitType;
    s32 waitId;
    s32 wakeupCount;
};

struct SemaParam {
    s32 currentCount;
    s32 maxCount;
    s32 initCount;
    s32 numWaitThreads;
    u32 attr;
    u32 option;
};

typedef struct {
    void *src;
    void *dest;
    s32 size;
    s32 attr;
} sceSifDmaData;

s32 CreateThread(struct ThreadParam *param);
s32 StartThread(s32 id, void *arg);
s32 GetThreadId(void);
s32 ChangeThreadPriority(s32 id, s32 priority);
s32 SleepThread(void);
s32 WakeupThread(s32 id);
s32 iWakeupThread(s32 id);
s32 RotateThreadReadyQueue(s32 priority);
s32 CreateSema(struct SemaParam *param);
s32 DeleteSema(s32 id);
s32 SignalSema(s32 id);
s32 iSignalSema(s32 id);
s32 WaitSema(s32 id);
s32 PollSema(s32 id);

void sceSifInitRpc(u32 mode);
s32 sceSifRebootIop(const char *img);
s32 sceSifSyncIop(void);
void *sceSifAllocIopHeap(u32 size);
s32 sceSifFreeIopHeap(void *addr);
u32 sceSifSetDma(sceSifDmaData *sdd, s32 len);
s32 sceSifDmaStat(u32 id);
s32 sceSifLoadStartModuleBuffer(void *addr, s32 args, const char *argp, s32 *result);
s32 sceRead(s32 fd, void *buf, s32 nbyte);

/* Named from behaviour and their place in the libraries (see symbol_addrs.txt). */
s32 sceSifInitIopHeap(void);
s32 sceSifLoadFileReset(void);
s32 sceFsReset(void);
s32 sceSifSearchModuleByName(const char *name);
s32 sceSifLoadModule(const char *path, s32 argLen, const char *args);
s32 sceSifLoadStartModule(const char *path, s32 argLen, const char *args, s32 *result);
s32 sceDevctl(const char *name, s32 cmd, void *arg, u32 argLen, void *buf, u32 bufLen);
s32 sceCdInit(s32 mode);
s32 sceCdMmode(s32 media);
void *sceDmaGetChan(s32 id);

#endif
