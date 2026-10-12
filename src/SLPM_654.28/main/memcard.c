#include "sce.h"

/* Memory cards (libmc2). */

typedef struct {
    s32 type;       /* 2: memory card */
    s32 port;
    s32 slot;
} sceMc2SocketParam;

s32 sceMc2Init(s32 mode);
s32 sceMc2CreateSocket(sceMc2SocketParam *param, void *buffer);
s32 sceCdReadClock(void *clock);

typedef struct {
    u8 unk0[0x10];
    s32 ports;      /* 0x10 */
    u8 unk14[0xAC - 0x14];
} McState;

extern McState D_00308D80;
extern u8 D_00308C80[][0x100];  /* each socket's buffer */
extern s32 D_0023BBD8[2];       /* sockets */
extern u8 D_0023BBD0[8];        /* the clock when the cards were opened */

#ifdef NON_MATCHING   /* the original keeps pointers to the socket parameters' fields in saved registers */
void func_001BD8F0(void) {
    McState *mc = &D_00308D80;
    sceMc2SocketParam p;
    s32 i;

    memset(mc, 0, sizeof(McState));
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
