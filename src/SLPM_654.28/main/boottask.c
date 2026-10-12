#include "task.h"

/* The first task: started by the frame loop, it loads SUBMAIN.BIN and starts the game. */

void func_001BACB0(Task *t);
void func_001BAE10(Task *t);

void func_001BAC60(Task *t) {
    switch (t->step) {
    case 0:
        func_001BACB0(t);
        break;
    case 1:
        func_001BAE10(t);
        break;
    }
}

void func_001A8990(u32 ovl);
void func_001B67A0(s16 slot, void (*onKill)(void));
void func_001B65D0(void (*fn)(Task *), s16 slot);
void func_001BB200(void);
void func_001BD8F0(void);
void func_001B8D00(Task *t);
void func_001B2C90(void);
void func_001D6CC0(void);
extern s8 D_0023BB90;

void func_001BACB0(Task *t) {
    t->step++;
    t->sub = 0;
    func_001A8990(0);
    func_001B67A0(0, func_001BB200);
    func_001BD8F0();
    func_001B65D0(func_001B8D00, 14);
    D_0023BB90 = 1;
    func_001B2C90();
    func_001D6CC0();
}
