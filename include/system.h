#ifndef SYSTEM_H
#define SYSTEM_H

#include "common.h"
#include "libdma.h"

/* Interrupt handlers end with "sync; ei". On PC handlers are plain calls with nothing to re-enable. */
#ifdef PLATFORM_PC
#define SYNC_EI()
#else
#define SYNC_EI() asm("sync"); asm("ei")
#endif

/* A linear heap: allocations come off the bottom or the top and are never freed one by one. */
typedef struct {
    u32 mem;      /* 0x00 */
    u32 start;    /* 0x04: mem, aligned */
    u32 end;      /* 0x08 */
    u32 lo;       /* 0x0C: next free byte from the bottom */
    u32 hi;       /* 0x10: end of free space at the top */
    s32 align;    /* 0x14 */
} LinearHeap;

extern LinearHeap D_00282220;   /* the game's main heap (right after the system work area) */

/* The engine's system work area (D_00281DB0), cleared at boot. */
typedef struct {
    s32 videoMode;          /* 0x000: 0 NTSC, 1 PAL, 2 PAL 512 lines */
    s32 field;              /* 0x004: 0 frame mode, else field mode */
    s32 blendFields;        /* 0x008: field mode: show both fields blended (circuits 1 and 2) */
    s32 width;              /* 0x00C */
    s32 dispHeight;         /* 0x010 */
    s32 magH;               /* 0x014: DISPLAY horizontal magnification - 1 (4: 512 wide, 3: 640) */
    s32 fbDepth;            /* 0x018: bytes per pixel of the frame buffers (4, 3 or 2) */
    s32 fbPsm;              /* 0x01C */
    s32 fbPageW;            /* 0x020 */
    s32 fbPageH;            /* 0x024 */
    u32 fbp[2];             /* 0x028: the two frame buffers' GS addresses */
    s32 zDepth;             /* 0x030 */
    s32 zPsm;               /* 0x034 */
    s32 zPageW;             /* 0x038 */
    s32 zPageH;             /* 0x03C */
    u32 zbp;                /* 0x040 */
    f32 zMax;               /* 0x044 */
    u32 vramNext;           /* 0x048: GS memory allocator */
    s32 vsync;              /* 0x04C */
    s32 dispBuffer;         /* 0x050: frame buffer on screen */
    s32 swapWait;           /* 0x054: v-blanks to wait before the next flip */
    s32 nextSwapWait;       /* 0x058 */
    s32 vcount;             /* 0x05C: v-blanks since the last frame was finished */
    s32 swapRequest;        /* 0x060 */
    u8 unk64[0x3A0 - 0x64];
    u32 clearColor;         /* 0x3A0: 0xAARRGGBB the frame is cleared to */
    s32 offsetX;            /* 0x3A4: XYOFFSET (2048 - width / 2) */
    s32 offsetY;            /* 0x3A8 */
    s32 unk3AC;
    s32 unk3B0;
    s32 dispX;              /* 0x3B4: DISPLAY position */
    s32 dispY;              /* 0x3B8 */
    s32 dispAdjX;           /* 0x3BC: screen position adjustment */
    s32 dispAdjY;           /* 0x3C0 */
    sceDmaChan *dmaChan[10]; /* 0x3C4: sceDmaGetChan(0..9) */
    s32 unk3EC;             /* 0x3EC */
    s32 unk3F0;             /* 0x3F0 */
    s32 unk3F4;
    u32 blockMem;           /* 0x3F8: area for the block allocator */
    u32 blockSize;          /* 0x3FC */
    s32 bufState;           /* 0x400 */
    s32 bufIndex;           /* 0x404: which of the two packet buffers is being filled */
    u32 bufBase;            /* 0x408 */
    u32 bufEnd;             /* 0x40C */
    u32 bufCur;             /* 0x410 */
    s32 bufHandle[2];       /* 0x414: block-allocator handles of the two 512 KiB packet buffers */
    s32 unk41C;
    u64 test[2];            /* 0x420: last TEST_1/TEST_2 sent */
    u64 zbuf[2];            /* 0x430: last ZBUF_1/ZBUF_2 sent */
    u64 scissor1;           /* 0x440 */
    s32 scissor1Rect[4];    /* 0x448: x0, y0, x1, y1 */
    u64 scissor2;           /* 0x458 */
    s32 scissor2Rect[4];    /* 0x460 */
} SysWork;

extern SysWork D_00281DB0;

/* The DMA queue (D_00281AF0): packets for one DMA channel (VIF1), double-buffered like the packet
   buffers. Entries are DMA tag addresses with a type in the top 4 bits. */
typedef struct {
    s32 channel;        /* 0x00 */
    u32 size;           /* 0x04: entries per buffer */
    s32 handlerId;      /* 0x08 */
    s32 handle[2];      /* 0x0C: block-allocator handles of the two entry arrays */
    u32 count[2];       /* 0x14 */
    s32 wr[2];          /* 0x1C */
    s32 rd[2];          /* 0x24 */
    u32 current;        /* 0x2C: the entry being sent */
    s32 flags;          /* 0x30: bit 0: waiting for a GS download */
    u32 lastTag;        /* 0x34 */
    u32 *lastPtr;       /* 0x38 */
    s32 unk3C;
} DmaQueue;

extern DmaQueue D_00281AF0;

/* GS packets: a DMA tag, two VIF codes, a GIF tag and address+data register pairs. */
typedef struct {
    u64 data;
    u64 addr;
} GsReg;

typedef struct {
    u64 dmaTag;
    u32 vif[2];
    u64 gifTag[2];
    GsReg reg[8];
} GsEnvPacket;          /* 0xA0 */

typedef struct {
    u64 dmaTag;
    u32 vif[2];
    u64 gifTag[2];
    GsReg reg[18];
} GsClearPacket;        /* 0x140 */

extern GsClearPacket D_00281B30;
extern GsEnvPacket D_00281C70[2];     /* per frame buffer: FRAME, ZBUF, XYOFFSET, DTHE, COLCLAMP */

/* Pixel format of the frame buffer, as the engine's image code sees it (D_00282240). */
typedef struct {
    s32 unk0;
    s32 width;
    s32 height;
    s32 stride;
    s32 unk10;
    s32 depth;
    struct { s32 bits, shift, mask; } r, g, b, a;   /* 0x18 */
} FbFormat;

extern FbFormat D_00282240;

/* GS memory: up to 6 fixed blocks (frame and Z buffers) from the bottom, in 256-byte block units. */
typedef struct {
    s16 used;
    u16 addr;
    u32 size;
} VramBlock;

/* Texture cache entries, a linked list of what's resident in GS memory. */
typedef struct TexOwner {
    s32 state;
    u8 unk4[0x31];
    u8 resident;     /* 0x35 */
} TexOwner;

typedef struct TexEntry {
    struct TexEntry *prev;
    struct TexEntry *next;
    TexOwner *owner;
    u8 unkC[8];
    s16 used;        /* 0x14 */
    u8 unk16[6];
} TexEntry;

extern s32 D_0023BA60;           /* VRAM blocks in use */
extern s32 D_0023BA68;           /* texture entries in use */
extern TexEntry *D_0023BA6C;     /* first texture entry */
extern VramBlock D_0027E2C0[6];
extern TexEntry D_0027E2F0[0x200];

/* memory.c */
s32 func_00189480(LinearHeap *heap, u32 mem, u32 size, s32 align);
void func_0018D600(void *dst, u8 value, s32 size);
u32 func_0018D660(u32 size);
u32 func_0018D6B0(u32 size);
s32 func_0018D6D0(u32 size, s32 unused);
u32 func_0018D720(s32 handle);
void func_0018D7C0(void);

#endif
