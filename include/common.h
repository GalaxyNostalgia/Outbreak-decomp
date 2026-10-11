#ifndef COMMON_H
#define COMMON_H

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
typedef signed long long s64;
typedef unsigned long long u64;
typedef float f32;

/* 128-bit quadwords (GIF tags, DMA tags with VIF codes, VU vectors). The PS2 compiler has a
   128-bit integer; 32-bit x86 doesn't, so on PC a quadword is a 16-byte struct and code that
   clears one writes U128_ZERO. */
#ifdef PLATFORM_PC
typedef struct { u64 lo, hi; } __attribute__((aligned(16))) u128;
#define U128_ZERO ((u128){0, 0})
#else
typedef unsigned __int128 u128;
#define U128_ZERO 0
#endif

#define NULL 0

typedef struct Vec3 {
    f32 x, y, z;
} Vec3;

#endif
