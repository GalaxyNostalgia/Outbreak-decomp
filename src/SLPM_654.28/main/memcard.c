#include "sce.h"
#include "mcjob.h"

/* Memory cards (libmc2). */

extern u8 D_00308C80[][0x100];  /* each socket's buffer */
extern u8 D_0023BBD0[8];        /* the clock when the cards were opened */

#ifdef NON_MATCHING   /* the original keeps pointers to the socket parameters' fields in saved registers */
void func_001BD8F0(void) {
    McJob *mc = &D_00308D80;
    sceMc2SocketParam p;
    s32 i;

    memset(mc, 0, sizeof(McJob));
    D_00308D80.ports = 1;
    while (sceMc2Init(0) < 0) {
    }
    for (i = 0; i < mc->ports; i++) {
        p.type = 2;
        p.port = i + 2;
        p.slot = 0;
        D_0023BBD8[i] = sceMc2CreateSocket(&p, D_00308C80[i]);
    }
    sceCdReadClock(D_0023BBD0);
}
#endif
