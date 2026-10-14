#include "common.h"
#include "options.h"

/* The room's data (.rdt): up to 32 parts {offset, size}, each copied where its handler wants it
   and handed to it. */

void *memset(void *p, s32 c, u32 n);
s32 sprintf(char *dst, const char *fmt, ...);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 unused);
s32 func_001A96E0(const char *name, void *buf);
void func_0018D630(void *dst, const void *src, s32 n);
extern char D_00238590[];   /* "%s/r%03d%02d%02d.rdt" */
extern char D_002385A8[];   /* "data/rdt" */
extern char D_002385C0[];   /* "r%03d%02d%02d.rdt" */

void func_001D68C0(void) {
    char name[0x100];
    Options *o = &D_00306590;

    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238590, D_002385A8, o->unk1A, o->unk1C, o->unk1E);
        o->rdtSize = func_001CFBB0(name, o->unk60, 1, 0);
    } else {
        sprintf(name, D_002385C0, o->unk1A, o->unk1C, o->unk1E);
        o->rdtSize = func_001A96E0(name, o->unk60);
    }
}

typedef struct {
    void (*fn)();       /* 0x0 */
    s32 part;           /* 0x4: (99: none) */
    s32 slot;           /* 0x8: where it is copied, 0 or 1 (99: not) */
    s32 call;           /* 0xC: 0 fn(copy 0, size), 1 fn(copy 1), 2 fn(copy 0, copy 1), 99 fn(), 999 end */
} RoomPart;

extern RoomPart D_00218F20[];

void func_001D6960(void);
#ifdef NON_MATCHING   /* first draft */
void func_001D6960(void) {
    s32 size[2];
    u8 *copy[2];
    u32 *parts;
    u32 *e;
    RoomPart *p;
    u32 i;

    memset(D_00306590.unk60, 0, 0x300000);
    func_001D68C0();
    for (i = 0, e = (u32 *)D_00306590.unk60; i < 0x20; i++, e += 2) {
        e[0] += (u32)D_00306590.unk60;
    }
    parts = (u32 *)D_00306590.unk60;
    p = D_00218F20;
    size[0] = 0;
    copy[0] = D_00306590.unk60 + 0x180000;
    copy[0] = (u8 *)(((u32)copy[0] + 3) & ~3);
    size[1] = 0;
    for (; p->call != 999; p++) {
        if (p->part != 99 && p->slot != 99) {
            e = &parts[p->part * 2];
            if (e[0] != 0 && e[1] != 0) {
                if (p->slot == 1) {
                    copy[1] = copy[0] + size[0];
                    copy[1] = (u8 *)(((u32)copy[1] + 3) & ~3);
                }
                func_0018D630(copy[p->slot], (void *)e[0], e[1]);
                size[p->slot] = e[1];
            } else {
                size[p->slot] = 0;
            }
        }
        if (p->fn != NULL) {
            switch (p->call) {
            case 2:
                if (size[0] != 0 && size[1] != 0) {
                    p->fn(copy[0], copy[1]);
                }
                break;
            case 1:
                if (size[1] != 0) {
                    p->fn(copy[1]);
                }
                break;
            case 0:
                if (size[0] != 0) {
                    p->fn(copy[0], size[0]);
                }
                break;
            case 99:
                p->fn();
                break;
            }
        }
    }
}
#endif

extern char D_00238728[];       /* "%s/dm%03d.bin" */
extern char D_00238738[];       /* "data/edit/eve" */
extern char D_00238748[];       /* "dm%03d.bin" */
s32 sprintf(char *s, const char *fmt, ...);
s32 func_001CFBB0(const char *name, void *buf, s32 flags, s32 size);
s32 func_001A96E0(const char *name, void *buf);

/* Load the room's dm file (mode 0: to Options.speech, 1: to unk6C + 0x10000); a missing one
   leaves a zero word. */
void func_001D7260(s32 mode) {
    char name[0x80];
    Options *o = &D_00306590;
    s32 *buf;
    s32 size;

    if (mode != 0) {
        buf = (s32 *)((s32)D_00306590.unk6C + 0x10000);
    } else {
        buf = (s32 *)D_00306590.speech;
    }
    if (D_00306590.unk17 == 0) {
        sprintf(name, D_00238728, D_00238738, o->unk1A);
        size = func_001CFBB0(name, buf, 1, 0);
    } else {
        sprintf(name, D_00238748, o->unk1A);
        size = func_001A96E0(name, buf);
    }
    if (size <= 0) {
        *buf = 0;
    }
}
