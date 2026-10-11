#ifndef LOADER_H
#define LOADER_H

#include <stdint.h>

/* Copy the game executable's loadable data from the disc into guest memory and
   clear its .bss, as the PS2 kernel does. Returns the entry point, 0 on error. */
uint32_t loader_load_elf(const char *disc_path);

/* Load a Metrowerks "MWo3" overlay file at its load address and clear its .bss.
   Returns the overlay id, -1 on error. Code stays native on PC; this brings in the data. */
int loader_load_overlay(const char *disc_path);

#endif
