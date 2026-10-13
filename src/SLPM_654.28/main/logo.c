#include "common.h"

/* The boot logos: full-screen pictures from demo_com.tex (texture ids 1-3). */

typedef struct {
    s16 x, y, w, h;
    u32 color;
    s16 u0, v0, u1, v1;
} Sprite2D;

extern Sprite2D D_00217340[2];  /* 512 wide (logo 1) and 640 wide (logos 2, 3) */

s32 func_001D04D0(s32 id);
void func_001AE290(void);
void func_001AE9C0(s32 cmd, u32 arg);
void func_001AE430(s32 a, s32 b);
s32 func_0019CD60(s32 n, s32 *handles);
s32 func_001AAB10(Sprite2D *s);

void func_001BAD50(s32 logo) {
    s32 tex;
    s32 wide = logo >= 2;

    tex = func_001D04D0(logo);
    if (tex != 0) {
        func_001AE290();
        func_001AE9C0(0xD, 0);
        func_001AE9C0(0x5F, 5);
        func_001AE9C0(0x60, 0);
        func_001AE9C0(0x63, 0);
        func_001AE9C0(0x6C, 0);
        func_0019CD60(1, &tex);
        func_001AE9C0(4, tex);
        func_001AE430(4, 5);
        func_001AAB10(&D_00217340[wide]);
    }
}

/* The "now loading" picture (texture 0, a 256x128 sprite at 368,320) drawn for 3 frames. */
extern Sprite2D D_00216880;
void func_001B9140(s32 a);

void func_001AF710(void) {
    s32 tex;
    s32 i;

    tex = func_001D04D0(0);
    if (tex != 0) {
        for (i = 0; i < 3; i++) {
            func_001B9140(1);
            func_001AE430(4, 5);
            func_001AE9C0(0x63, 0x10000);
            func_001AE9C0(0x6C, 0);
            func_0019CD60(1, &tex);
            func_001AE9C0(4, tex);
            func_001AAB10(&D_00216880);
        }
    }
}
