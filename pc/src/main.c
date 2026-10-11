/* Biohazard Outbreak PC: entry point.
 *
 * Today this sets up the PS2 side (guest memory, the user's disc, the executable's data)
 * and runs self-tests that call decompiled game code natively. The game's own boot path
 * takes over here as soon as it is decompiled (docs/PCPORT.md).
 */
#include "guest.h"
#include "iso9660.h"
#include "loader.h"
#include "log.h"
#include "stubs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef GAME_ELF
#error "build with -DGAME_ELF=\"SLPM_654.28\" (CMake sets it from OUTBREAK_GAME)"
#endif

/* decompiled game code (src/SLPM_654.28/main/cdfile.c) */
int func_001A8300(const char *name);
void func_001BC780(char *dst, const char *name);

void pc_unimplemented(uint32_t addr, const char *name) {
    log_error("game called %s (0x%08x), which isn't decompiled yet", name, addr);
    exit(3);
}

static int self_test(void) {
    static const char *files[] = {"PROG\\SUBMAIN.BIN", "PROG\\GAME.BIN", "PROG\\NETWK.BIN", "SYSTEM.CNF",
                                  "PROG\\EDITOR.BIN"};
    int fails = 0;
    char path[0x100];
    log_info("self-test: calling decompiled disc functions natively");
    func_001BC780(path, "prog\\game.bin");
    log_info("  func_001BC780(\"prog\\\\game.bin\") -> \"%s\"", path);
    if (strcmp(path, "cdrom0:\\PROG\\GAME.BIN;1") != 0) fails++;
    for (size_t i = 0; i < sizeof files / sizeof *files; i++) {
        IsoFile f;
        int want = iso_find(files[i], &f) == 0 ? (int)f.size : 0;
        int got = func_001A8300(files[i]);
        log_info("  func_001A8300(\"%s\") -> %d (disc: %d) %s", files[i], got, want, got == want ? "ok" : "MISMATCH");
        fails += got != want;
    }
    return fails;
}

int main(int argc, char **argv) {
    const char *iso = NULL;
    int test = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--test")) test = 1;
        else if (argv[i][0] != '-') iso = argv[i];
    }
    if (!iso) {
        fprintf(stderr, "usage: %s [--test] <your %s disc image.iso>\n", argv[0], GAME_ELF);
        return 2;
    }
    if (guest_init() != 0 || iso_open(iso) != 0) return 1;
    uint32_t entry = loader_load_elf(GAME_ELF);
    if (!entry) return 1;
    log_info("%s loaded, PS2 entry point 0x%08x", GAME_ELF, entry);

    if (test) {
        int fails = self_test();
        log_info("self-test %s", fails ? "FAILED" : "passed");
        return fails ? 1 : 0;
    }
    log_info("the boot path isn't decompiled yet; run with --test to exercise the decompiled code");
    return 0;
}
