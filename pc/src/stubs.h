#ifndef STUBS_H
#define STUBS_H

#include <stdint.h>

/* Called by generated stubs for game functions that aren't decompiled yet. */
void pc_unimplemented(uint32_t addr, const char *name);

#endif
