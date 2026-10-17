#ifndef VU0_H
#define VU0_H

#include "common.h"

/* The VU0 math library (hand-written macro-mode assembly in the original; the PC port has C
   versions in its src/asm.c). Matrices are 4x4 f32, four rows; a vector goes through a matrix
   as x * row 0 + y * row 1 + z * row 2 (+ row 3). */

void func_00190C50(f32 *out, const f32 *a, const f32 *b);
void func_00191750(f32 *v);
f32 func_0018EDB0(f32 x);
f32 func_0018EED0(f32 x);
void func_0018EFC0(f32 *out, f32 x);
void func_0018F7C0(f32 *m);
void func_00190ED0(f32 *dst, const f32 *src);
void func_001903E0(f32 rx, f32 ry, f32 rz, f32 *m);
void func_001911F0(f32 *m, const f32 *dir, const f32 *up);
void func_00190BA0(f32 *m);
void func_001913A0(f32 *out, const f32 *v, const f32 *m);
void func_0018F7F0(f32 *m);
void func_0018FCA0(f32 rx, f32 ry, f32 rz, f32 *m);
f32 func_00191BF0(f32 x);
void func_0018F830(f32 x, f32 y, f32 z, f32 *m);
void func_0018FAD0(f32 a, f32 *m);
void func_00190E00(f32 *out, const f32 *m);
void func_00190F50(f32 *m, const f32 *a, const f32 *b);
void func_00191030(f32 *m, const f32 *a, const f32 *b);
f32 func_0018EC50(f32 t, f32 y0, f32 x0, f32 y1, f32 x1);
f32 func_0018ECD0(f32 t, f32 y0, f32 x0, f32 m0, f32 y1, f32 x1, f32 m1);
void func_0018F880(f32 x, f32 y, f32 z, f32 *m);
void func_0018F8E0(f32 x, f32 y, f32 z, f32 *m);
void func_0018F930(f32 s, f32 *m);
void func_0018F980(f32 s, f32 *m);
void func_0018FB70(f32 a, f32 *m);
void func_0018FC10(f32 a, f32 *m);
void func_0018FE70(f32 rx, f32 ry, f32 rz, f32 *m);
void func_00190040(f32 rx, f32 ry, f32 rz, f32 *m);
void func_00190210(f32 rx, f32 ry, f32 rz, f32 *m);
void func_00190570(f32 rx, f32 ry, f32 rz, f32 *m);
void func_00190700(f32 rx, f32 ry, f32 rz, f32 *m);
void func_001908A0(f32 rx, f32 ry, f32 rz, f32 *m);
void func_00190CD0(f32 *a, const f32 *b);
void func_00190D50(f32 *out, const f32 *a, const f32 *b);
void func_00190DB0(f32 *a, const f32 *b);
void func_00191110(f32 *m, const f32 *a, const f32 *b);
void func_001912D0(f32 s, f32 u, f32 *out, const f32 *a, const f32 *b);
void func_00191410(f32 *v, const f32 *m);
void func_00191480(f32 *out, const f32 *v, const f32 *m);
f32 func_00191690(const f32 *v);
f32 func_001916E0(const f32 *a, const f32 *b);
f32 func_001917C0(const f32 *a, const f32 *b);
void func_00191820(f32 *out, const f32 *a, const f32 *b);
/* SUBMAIN.BIN: out = v * s */
void func_003405D0(f32 s, f32 *out, const f32 *v);

#endif
