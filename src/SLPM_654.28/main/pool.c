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
#include "drawcb.h"

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


typedef struct {
    f32 v[4];
} F4;

extern f32 D_00306338[3];       /* the camera */
f32 func_001916E0(const f32 *a, const f32 *b);

/* Takes a callback from the pool (cleared), NULL when none is left. */
DrawCb *func_001B1810(void) {
    DrawCb *e;
    u16 id;

    if (D_0023BB78 == 0) {
        return NULL;
    }
    id = *D_0023BB70++;
    D_0023BB78--;
    e = &((DrawCb *)D_002F8AE0)[id];
    memset(e, 0, sizeof(DrawCb));
    return e;
}

/* Gives back the one just taken. */
void func_001B1880(void) {
    if (D_0023BB78 < 0x100) {
        D_0023BB70--;
        D_0023BB78++;
    }
}

/* Links e into the list at head, before the first farther-or-equal one. */
void func_001B18B0(DrawCb *e, u32 *head) {
    u32 t;
    DrawCb *n;

    for (;;) {
        t = *head;
        if (t == 0 || !(t & 1)) {
            *head = (u32)e | 1;
            e->next = t;
            return;
        }
        n = (DrawCb *)(t & ~1);
        if (n->pos[2] <= e->pos[2]) {
            *head = (u32)e | 1;
            e->next = t;
            return;
        }
        head = &n->next;
    }
}

/* Queues fn(arg) to be drawn back to front by the distance of pos from the camera (within
   50000): its bucket, or -1. */
s32 func_001B1460(void *fn, void *arg, f32 *pos) {
    F4 d;
    f32 c[3];
    f32 p[3];
    DrawCb *e;
    s32 far = 50000;

    if ((e = func_001B1810()) == NULL) {
        return -1;
    }
    c[0] = D_00306338[0];
    c[1] = D_00306338[1];
    c[2] = D_00306338[2];
    p[0] = pos[0];
    p[1] = pos[1];
    p[2] = pos[2];
    d.v[2] = func_001916E0(c, p);
    if (d.v[2] < 0.0f) {
        d.v[2] = 0.0f;
    }
    if (!(d.v[2] <= (f32)far)) {
        func_001B1880();
        return -1;
    }
    e->fn = (void (*)(DrawCb *))fn;
    e->arg = arg;
    *(F4 *)e->pos = d;
    e->depth = 128.0f * (d.v[2] / (f32)far);
    func_001B18B0(e, &D_002FAAE0[127 - e->depth]);
    return e->depth;
}
