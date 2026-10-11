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

/* Hardware registers (0x10000000-0x1000FFFF EE devices, 0x12000000 GS privileged).
   On PC these go to pc_hw_reg(), which models the few the game uses (timers) and
   ignores the rest. */
#ifdef PLATFORM_PC
volatile u32 *pc_hw_reg(u32 addr);
#define HW_REG32(addr) (*pc_hw_reg(addr))
#else
#define HW_REG32(addr) (*(volatile u32 *)(addr))
#endif

#endif
