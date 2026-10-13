#include "common.h"
#include "sce.h"

/* Free lists the game rebuilds every frame. */

void func_001A50F0(s32 count, u32 *items);   /* util.c */

extern u32 D_002FAAE0[0xA0];
extern u8 D_0023BB7C;
extern u16 *D_0023BB70;          /* top of the id stack */
extern u16 D_002F8AE0[];         /* the id stack ends here */
extern s16 D_0023BB78;           /* ids on the stack */

void func_001B1360(void) {
    s32 id;
    s32 i;

    func_001A50F0(0xA0, D_002FAAE0);
    D_0023BB7C = 1;
    D_0023BB70 = D_002F8AE0;
    id = 0xFF;
    i = 0;
    do {
        *--D_0023BB70 = id;
        i++;
        id--;
    } while (i < 0x100);
    D_0023BB78 = 0x100;
}

extern s32 D_0023BB30;           /* per-frame callbacks registered */
extern void (*D_0029B0D0[])(void);

void func_001AED60(void) {
    void (**cb)(void);
    s32 n;

    func_001B1360();
    n = D_0023BB30;
    cb = D_0029B0D0;
    while (n != 0) {
        (*cb)();
        n--;
        cb++;
    }
}

extern s16 D_0023BB74;

/* The same id stack, after clearing its first entry (the original clears the same 0x20 bytes
   256 times). */
void func_001B13D0(void) {
    s32 i;
    u16 id;

    for (i = 0; i < 0x100; i++) {
        memset(D_002F8AE0, 0, 0x20);
    }
    id = 0xFF;
    i = 0;
    D_0023BB70 = D_002F8AE0;
    do {
        *--D_0023BB70 = id;
        i++;
        id--;
    } while (i < 0x100);
    D_0023BB74 = 0;
    D_0023BB78 = 0x100;
}

/* Depth-sorted draw callbacks: 0xA0 buckets threaded into one list (D_002FAAE0); an entry is
   linked in after its bucket (func_001A5130), so walking the list visits them far to near. */
typedef struct DrawCb {
    u32 next;
    s32 depth;          /* 0x04 */
    f32 pos[4];         /* 0x08 */
    void *arg;          /* 0x18 */
    void (*fn)(struct DrawCb *cb); /* 0x1C */
} DrawCb;

u32 func_001A5150(u32 *list);  /* util.c */

/* Runs this frame's draw callbacks. */
void func_001B17C0(void) {
    u32 p = (u32)D_002FAAE0;

    for (;;) {
        p = func_001A5150(&p);
        if (p == 0) {
            break;
        }
        ((DrawCb *)p)->fn((DrawCb *)p);
    }
}
