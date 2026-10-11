#ifndef LIBPAD2_H
#define LIBPAD2_H

#include "common.h"

/* Sony controller library 2 (libpad2) and the DualShock/device manager (libdbc). */

typedef struct {
    s32 flag;
    s32 port;
    s32 slot;
    s32 number;
    u8 reserved[16];
} scePad2SocketParam;

s32 scePad2Init(s32 mode);
s32 scePad2End(void);
s32 scePad2CreateSocket(scePad2SocketParam *param, void *buffer);
s32 scePad2DeleteSocket(s32 socket);
s32 scePad2Read(s32 socket, void *data);
s32 scePad2GetState(s32 socket);
s32 sceDbcInit(void);
s32 sceDbcEnd(void);

#endif
