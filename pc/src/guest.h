/* PS2 memory inside the PC process.
 *
 * The port is a 32-bit program, so pointers are 4 bytes like on the PS2 and the
 * game's structures keep their layout. The PS2's main RAM range used by the game
 * (0x00100000-0x02000000) and the scratchpad (0x70000000, 16 KiB) are reserved at
 * the same addresses, and the original executable's data is loaded there from the
 * user's disc. Decompiled C refers to that data through its PS2 addresses
 * (D_XXXXXXXX symbols are absolute), so code and data stay consistent while
 * the decomp is incomplete.
 */
#ifndef GUEST_H
#define GUEST_H

#include <stddef.h>
#include <stdint.h>

#define GUEST_RAM_BASE   0x00100000u
#define GUEST_RAM_END    0x02000000u
#define GUEST_SPR_BASE   0x70000000u
#define GUEST_SPR_SIZE   0x4000u

int guest_init(void);           /* 0 on success */
void guest_shutdown(void);

static inline void *guest_ptr(uint32_t addr) { return (void *)(uintptr_t)addr; }
static inline int guest_valid(uint32_t addr, uint32_t size) {
    return addr >= GUEST_RAM_BASE && addr + size <= GUEST_RAM_END && addr + size >= addr;
}

#endif
