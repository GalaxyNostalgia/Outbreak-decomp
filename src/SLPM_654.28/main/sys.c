#include "sce.h"
#include "kernel.h"
#include "libgraph.h"
#include "ps2mem.h"
#include "cri.h"

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

typedef struct {
    u8 unk0[0x4C];
    s32 vsync;          /* 0x4C */
    u8 unk50[4];
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    u8 unk60[0x3C4 - 0x60];
    void *dmaChan[10];  /* 0x3C4 */
} SysWork;

extern s32 D_00281AF0[];
extern SysWork D_00281DB0;

s32 func_0019EA50(void);
void func_0017FEB0(void);
void func_0018C8E0(s32 *a, s32 size, void (*fn)(void));
void func_0018CC10(void);
void func_0018D760(void);
void func_0019ED40(void);
void func_0019EE00(s32 a, s32 b, s32 c, s32 d, s32 e);
void func_0019F930(s32 a, s32 b);
void func_001A0E40(void);

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
    D_00281AF0[0] = 1;
    func_0018C8E0(D_00281AF0, 0x1000, func_0018CC10);
    return 1;
}

s32 func_0019E9A0(s32 width, s32 height) {
    if (func_0019EB50() == 0) {
        return 0;
    }
    func_0018D760();
    D_00281DB0.unk54 = 0;
    D_00281DB0.unk58 = 0;
    D_00281DB0.unk5C = 0;
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
