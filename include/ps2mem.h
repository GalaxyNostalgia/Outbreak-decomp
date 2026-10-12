#ifndef PS2MEM_H
#define PS2MEM_H

#include "common.h"

/* Fixed PS2 main-RAM addresses the game uses as its own buffers (outside any
   variable), e.g. 0x01000000 as a scratch area at boot. On PS2 it's the address;
   the PC build backs main RAM with a 32 MiB block and maps the address into it. */
#ifdef PLATFORM_PC
extern u8 pc_ram[];
#define PS2_RAM(addr) ((void *)(pc_ram + (u32)(addr)))
#else
#define PS2_RAM(addr) ((void *)(addr))
#endif

/* The scratchpad (16 KiB of fast RAM at 0x70000000). The PC build backs it with a 16 KiB block. */
#ifdef PLATFORM_PC
extern u8 pc_spr[];
#define PS2_SPR(addr) ((void *)(pc_spr + ((u32)(addr) & 0x3FFF)))
#else
#define PS2_SPR(addr) ((void *)(addr))
#endif

/* Hardware registers (0x10000000-0x1000FFFF EE devices, 0x12000000 GS privileged).
   On PC these go to pc_hw_reg(), which models the few the game uses (timers) and
   ignores the rest. */
#ifdef PLATFORM_PC
volatile u32 *pc_hw_reg(u32 addr);
#define HW_REG32(addr) (*pc_hw_reg(addr))
#define HW_REG64(addr) (*(volatile u64 *)pc_hw_reg(addr))
#else
#define HW_REG32(addr) (*(volatile u32 *)(addr))
#define HW_REG64(addr) (*(volatile u64 *)(addr))
#endif

/* GS privileged registers */
#define GS_PMODE 0x12000000
#define GS_SMODE2 0x12000020
#define GS_DISPFB1 0x12000070
#define GS_DISPLAY1 0x12000080
#define GS_DISPFB2 0x12000090
#define GS_DISPLAY2 0x120000A0
#define GS_EXTWRITE 0x120000D0
#define GS_CSR 0x12001000
#define GS_IMR 0x12001010
#define GS_BUSDIR 0x12001040

#endif
