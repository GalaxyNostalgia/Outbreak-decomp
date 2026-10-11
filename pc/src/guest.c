#include "guest.h"
#include "log.h"

#if !defined(__i386__) && !defined(_M_IX86)
#error "The PC port is a 32-bit build: pointers must be 4 bytes like on the PS2 (use -m32 / an i686 toolchain)."
#endif

#ifdef _WIN32
#include <windows.h>

static void *reserve(uint32_t base, uint32_t size) {
    return VirtualAlloc((void *)(uintptr_t)base, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}
static void release(uint32_t base, uint32_t size) {
    (void)size;
    VirtualFree((void *)(uintptr_t)base, 0, MEM_RELEASE);
}
#else
#include <sys/mman.h>
#ifndef MAP_FIXED_NOREPLACE
#define MAP_FIXED_NOREPLACE 0x100000
#endif

static void *reserve(uint32_t base, uint32_t size) {
    void *p = mmap((void *)(uintptr_t)base, size, PROT_READ | PROT_WRITE,
                   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE, -1, 0);
    return p == MAP_FAILED ? NULL : p;
}
static void release(uint32_t base, uint32_t size) {
    munmap((void *)(uintptr_t)base, size);
}
#endif

int guest_init(void) {
    void *ram = reserve(GUEST_RAM_BASE, GUEST_RAM_END - GUEST_RAM_BASE);
    if (ram != guest_ptr(GUEST_RAM_BASE)) {
        log_error("guest: could not reserve PS2 RAM at 0x%08x-0x%08x (something else is mapped there)",
                  GUEST_RAM_BASE, GUEST_RAM_END);
        return -1;
    }
    void *spr = reserve(GUEST_SPR_BASE, GUEST_SPR_SIZE);
    if (spr != guest_ptr(GUEST_SPR_BASE)) {
        log_error("guest: could not reserve the scratchpad at 0x%08x", GUEST_SPR_BASE);
        return -1;
    }
    log_info("guest: PS2 RAM 0x%08x-0x%08x and scratchpad 0x%08x mapped", GUEST_RAM_BASE, GUEST_RAM_END,
             GUEST_SPR_BASE);
    return 0;
}

void guest_shutdown(void) {
    release(GUEST_RAM_BASE, GUEST_RAM_END - GUEST_RAM_BASE);
    release(GUEST_SPR_BASE, GUEST_SPR_SIZE);
}
