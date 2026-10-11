#include "sce.h"
#include "kernel.h"
#include "libgraph.h"
#include "ps2mem.h"
#include "cri.h"
#include "system.h"

/* Boot-time system setup: IOP reboot and modules, DMA channels, the GS and the frame timer. */

typedef struct { s32 index; s32 argLen; char *args; } ModuleEntry;

/* DRIVE.DAT: packed IOP modules */
typedef struct { s32 offset; s32 size; } DriveEntry;
typedef struct {
    s32 unk0;
    s32 count;
    DriveEntry entry[1];  /* offset from the file start; IOP address once copied */
} DriveDat;

extern char D_00231A40[];        /* "cdrom0:\IOP\MODULES\DNAS280.IMG;1" (IOP replacement image) */
extern char D_00231A70[];        /* "cdrom0:\IOP\MODULES\DRIVE.DAT;1" (packed IOP modules) */
extern char D_00231A90[];        /* "cdrom0:\IOP\MODULES\USBD.IRX;1" */
extern char D_00231AB0[];        /* "cdrom0:\IOP\MODULES\USBKB.IRX;1" */
extern char D_00231AD0[];        /* "keybd=1" */
extern ModuleEntry D_00216380[]; /* which modules of DRIVE.DAT to start, -1 terminated */

void func_0018D1F0(char *path, s32 argLen, char *args, s32 tries);
void func_001BB420(void);

s32 func_001AA7D0(DriveDat *archive, s32 index, s32 argLen, char *args) {
    s32 result;
    void *module = (void *)archive->entry[index].offset;
    while (sceSifLoadStartModuleBuffer(module, argLen, args, &result) < 0) {
    }
    return result;
}

/* Not matching yet: the DRIVE.DAT fix-up loop builds its pointer with lui/ori, the original with
   lui/addiu, and allocates registers differently (7 instructions). */
#ifdef NON_MATCHING
void func_001AA830(void) {
    sceSifDmaData dma;
    s32 fd, size, id, n, i;
    u8 *iop;
    ModuleEntry *m;
    DriveDat *drive = (DriveDat *)PS2_RAM(0x01000000);
    DriveEntry *entry;

    sceSifInitRpc(0);
    sceSifInitIopHeap();
    sceCdInit(0);
    sceFsReset();
    while ((sceSifRebootIop(D_00231A40) != 0) ^ 1) {
    }
    while ((sceSifSyncIop() != 0) ^ 1) {
    }
    sceSifInitRpc(0);
    sceSifLoadFileReset();
    sceFsReset();
    sceSifInitIopHeap();
    sceCdInit(0);
    sceCdMmode(2);

    /* read DRIVE.DAT, copy it to IOP memory, turn its offsets into IOP addresses */
    while ((fd = sceOpen(D_00231A70, 1)) < 0) {
    }
    size = sceLseek(fd, 0, 2);
    sceLseek(fd, 0, 0);
    sceRead(fd, drive, size);
    sceClose(fd);
    while ((iop = sceSifAllocIopHeap(size)) == NULL) {
    }
    dma.src = drive;
    dma.dest = iop;
    dma.size = size;
    dma.attr = 0;
    FlushCache(0);
    id = sceSifSetDma(&dma, 1);
    while (sceSifDmaStat(id) >= 0) {
    }
    n = drive->count;
    entry = drive->entry;
    for (i = 0; i < n; i++) {
        entry->offset += (s32)iop;
        entry++;
    }
    for (m = D_00216380; m->index != -1; m++) {
        if (m->index == 9) {
            ADXPS2_SetupVoice(2, 1);   /* 2 stereo, 1 mono voices */
        }
        func_001AA7D0(drive, m->index, m->argLen, m->args);
    }
    sceSifFreeIopHeap(iop);
    func_001BB420();
    func_0018D1F0(D_00231A90, 0, NULL, 0);
    func_0018D1F0(D_00231AB0, 0x10, D_00231AD0, 0);
}
#endif

/* --- boot: system init (func_0019EB50, func_0019E9A0) --- */


void func_0017FEB0(void);
void func_0018C8E0(DmaQueue *q, s32 size, s32 (*handler)(s32));
s32 func_0018CC10(s32 channel);
void func_0018D760(void);
void func_0019ED40(void);
void func_0019EE00(s32 a, s32 b, s32 c, s32 d, s32 e);
void func_0019F930(s32 a, s32 b);
void func_001A0E40(void);

/* Memory: a 22 MiB heap from malloc; 14 MiB of it is managed by the block allocator. */
extern s32 D_0023BA28;
extern s32 D_0023BA64;
extern s32 D_0023BA8C;
extern s32 D_0023BA90;
extern s32 D_0023BA94;
extern s32 D_0023BAA4;
extern void *D_0023BAD0;    /* free() for the engine */
extern void *D_0023BAD4;    /* malloc() for the engine */
extern s32 D_0023BAA8;

void func_001A02C0(u32 base, u32 size, s32 align);

void func_0019E0A0(void) {
    s32 i;

    D_0023BA60 = 0;
    D_0023BAA8 = 1;
    for (i = 0; i < 6; i++) {
        func_0018D600(&D_0027E2C0[i], 0, sizeof(VramBlock));
    }
    D_0023BA68 = 0;
    D_0023BA6C = NULL;
    for (i = 0; i < 0x200; i++) {
        func_0018D600(&D_0027E2F0[i], 0, sizeof(TexEntry));
    }
}

s32 func_0019EA50(void) {
    u32 mem;

    func_0018D600(&D_00281DB0, 0, sizeof(SysWork));
    func_0019E0A0();
    mem = ((u32)malloc(0x01600000 + 0x3F) + 0x3F) & ~0x3F;
    if (mem == 0) {
        return 0;
    }
    func_00189480(&D_00282220, mem, 0x01600000, 0x40);
    D_00281DB0.blockSize = 0xE00000;
    mem = func_0018D6B0(D_00281DB0.blockSize);
    D_00281DB0.blockMem = mem;
    func_001A02C0(mem, D_00281DB0.blockSize, 0x40);
    D_00281DB0.unk3F0 = 0;
    D_0023BAD4 = func_0018D660;
    D_0023BAA4 = 0;
    D_0023BA64 = 0;
    D_0023BAD0 = free;
    D_0023BA28 = 0;
    D_0023BA94 = 100;
    D_00281DB0.unk3EC = -1;
    D_0023BA90 = 0x202;
    D_0023BA8C = 0x0138B032;
    return 1;
}

/* The packet buffers: two 512 KiB blocks, one filled while the other is drawn. */
void func_0018D7C0(void);
u32 func_0018D0E0(void);

void func_0018D760(void) {
    s32 i;

    for (i = 0; i < 2; i++) {
        D_00281DB0.bufHandle[i] = func_0018D6D0(0x80000, 1);
    }
    func_0018D7C0();
}

void func_0018D7C0(void) {
    switch (D_00281DB0.bufState) {
    case 0:
    case 2:
    case 1:
        D_00281DB0.bufBase = func_0018D720(D_00281DB0.bufHandle[D_00281DB0.bufIndex]);
        D_00281DB0.bufCur = D_00281DB0.bufBase;
        D_00281DB0.bufEnd = D_00281DB0.bufBase + 0x80000;
        break;
    }
}

/* Not matching yet: register allocation (4 instructions). */
#ifdef NON_MATCHING
u32 func_0018D850(u32 size, s32 align) {
    u32 p = ~(align - 1) & (D_00281DB0.bufCur + align - 1);
    u32 end = p + size;

    if (D_00281DB0.bufEnd < end) {
        func_0018D0E0();
        p = D_00281DB0.bufBase;
        end = p + size;
    }
    D_00281DB0.bufCur = end;
    return p;
}
#endif

s32 func_0019EB50(void) {
    s32 i;

    func_001AA830();
    if (func_0019EA50() == 0) {
        return 0;
    }
    sceGsResetPath();
    sceDmaReset(1);
    for (i = 0; i < 10; i++) {
        D_00281DB0.dmaChan[i] = sceDmaGetChan(i);
    }
    func_0017FEB0();
    D_00281AF0.channel = 1;
    func_0018C8E0(&D_00281AF0, 0x1000, func_0018CC10);
    return 1;
}

s32 func_0019E9A0(s32 width, s32 height) {
    if (func_0019EB50() == 0) {
        return 0;
    }
    func_0018D760();
    D_00281DB0.swapWait = 0;
    D_00281DB0.nextSwapWait = 0;
    D_00281DB0.vcount = 0;
    func_0019ED40();
    func_0019EE00(4, 4, 1, 0, 1);
    func_0019F930(0, 1);
    func_001A0E40();
    while ((D_00281DB0.vsync = sceGsSyncV(0)) != 0) {
    }
    HW_REG32(0x10000810) = 0x80;   /* T1_MODE: count bus clocks */
    HW_REG32(0x10000800) = 0;      /* T1_COUNT */
    return 1;
}

void func_001A0280(void) {
    D_00281DB0.bufState = 0;
}

void func_001A0290(void) {
    D_00281DB0.bufState = 2;
}

/* v-blanks per frame */
void func_001A02A0(s32 wait) {
    D_00281DB0.nextSwapWait = wait;
}

extern s32 D_0023BA2C;

void func_001A02B0(s32 value) {
    D_0023BA2C = value;
}
