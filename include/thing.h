#ifndef THING_H
#define THING_H

#include "common.h"
#include "model.h"
#include "enemy.h"

typedef struct {
    f32 v[4];
} Vec4f;

/* A push on a thing (0x12 bytes). */
typedef struct {
    s16 v; /* 0x00 */
    u8 unk2[0xF];
    u8 arg; /* 0x11 */
} ThingPush;

/* A motion layer of a thing. */
typedef struct {
    f32 unk0; /* 0x00: the frame it started on */
    u8 unk4[0xE - 0x4];
    s16 frame; /* 0x0E: its motion */
    u8 unk10[0x20 - 0x10];
} ThingLayer;

/* The room's things (0xD90 bytes, pool D_003FFCA0): the enemies its enemy sets place. */
typedef struct Thing {
    s8 active; /* 0x000 */
    s8 visible; /* 0x001 */
    u8 type; /* 0x002 */
    u8 kind; /* 0x003: the enemy type (D_00357500) */
    u8 unk4; /* 0x004 */
    u8 unk5; /* 0x005 */
    u8 index; /* 0x006: in D_003FFCA0 */
    u8 pad007;
    u8 act[4]; /* 0x008: action, its step and sub-steps */
    u8 unkC[8]; /* 0x00C */
    void *update; /* 0x014: (per kind) */
    void *draw; /* 0x018 */
    struct Thing *next; /* 0x01C */
    struct Thing *prev; /* 0x020 */
    void *target; /* 0x024: the player (or enemy) it goes for */
    u8 pad028[0x031 - 0x028];
    u8 slot; /* 0x031: its model (D_0042A2C0) */
    u8 unk32; /* 0x032 */
    u8 variant; /* 0x033 */
    Slot1468 *model; /* 0x034 */
    f32 pos[3]; /* 0x038 */
    f32 prevPos[3]; /* 0x044: pos when the frame began */
    f32 mtx[16]; /* 0x050: rotation (from rot) */
    s16 rot[3]; /* 0x090 */
    s16 unk96; /* 0x096 */
    u8 pad098[0x0A0 - 0x098];
    s16 prevRot[4]; /* 0x0A0: rot when the frame began */
    u8 pad0A8[0x0B4 - 0x0A8];
    f32 scale[3]; /* 0x0B4 */
    u8 pad0C0[0x0F0 - 0x0C0];
    f32 unkF0; /* 0x0F0 */
    u8 parts[0x80]; /* 0x0F4: per model part, drawn */
    u8 pad174[0x190 - 0x174];
    u32 unk190; /* 0x190 */
    u32 unk194; /* 0x194 */
    u8 pad198;
    s8 unk199; /* 0x199 */
    u8 pad19A[0x1A8 - 0x19A];
    void *unk1A8; /* 0x1A8 */
    Vec4f unk1AC; /* 0x1AC */
    Vec4f unk1BC; /* 0x1BC */
    u8 pad1CC[0x1D8 - 0x1CC];
    void *unk1D8; /* 0x1D8 */
    void *unk1DC; /* 0x1DC: -> unkAEC */
    u8 pad1E0[0x1E2 - 0x1E0];
    u16 scenario; /* 0x1E2 */
    ThingLayer layers[4]; /* 0x1E4: motion layers */
    u8 pad264[0x514 - 0x264];
    void *motions[2]; /* 0x514: (D_0048E620) */
    EnemyMotions *anims; /* 0x51C: (D_0048FDB0) */
    s16 unk520; /* 0x520 */
    u8 pad522[0x524 - 0x522];
    s8 unk524; /* 0x524 */
    u8 pad525[0x544 - 0x525];
    s16 hp; /* 0x544 */
    s16 maxHp; /* 0x546 */
    u8 pad548[0x54A - 0x548];
    u16 unk54A; /* 0x54A */
    u8 pad54C;
    u8 unk54D; /* 0x54D */
    u8 unk54E; /* 0x54E */
    u8 pad54F[0x551 - 0x54F];
    u8 unk551; /* 0x551 */
    u8 unk552; /* 0x552: the bone others look at */
    u8 shadow; /* 0x553 */
    u8 unk554; /* 0x554 */
    u8 pad555[0xAA8 - 0x555];
    f32 unkAA8; /* 0xAA8: gravity */
    u8 padAAC[0xAEC - 0xAAC];
    u8 unkAEC[4]; /* 0xAEC */
    u8 padAF0[0xB98 - 0xAF0];
    u8 set; /* 0xB98: its D_0071A030 entry */
    u8 padB99[0xB9A - 0xB99];
    u8 unkB9A; /* 0xB9A */
    u8 unkB9B; /* 0xB9B */
    u8 unkB9C; /* 0xB9C */
    u8 unkB9D; /* 0xB9D */
    u8 unkB9E; /* 0xB9E */
    s8 unkB9F; /* 0xB9F */
    u8 padBA0[0xBA1 - 0xBA0];
    u8 unkBA1; /* 0xBA1 */
    u8 padBA2[0xBA3 - 0xBA2];
    u8 unkBA3; /* 0xBA3 */
    u8 unkBA4; /* 0xBA4 */
    u8 unkBA5; /* 0xBA5 */
    s8 unkBA6; /* 0xBA6 */
    u8 unkBA7; /* 0xBA7 */
    u8 unkBA8; /* 0xBA8 */
    u8 unkBA9; /* 0xBA9 */
    u8 unkBAA; /* 0xBAA */
    u8 padBAB[0xBAC - 0xBAB];
    u8 unkBAC[10]; /* 0xBAC */
    u8 unkBB6[4]; /* 0xBB6 */
    u16 def; /* 0xBBA: its D_00357500[kind].defs entry */
    u8 padBBC[0xBCC - 0xBBC];
    f32 unkBCC; /* 0xBCC */
    f32 unkBD0; /* 0xBD0 */
    f32 unkBD4; /* 0xBD4: distance to its target */
    s16 unkBD8; /* 0xBD8: its target's direction, relative to its heading */
    s16 unkBDA; /* 0xBDA */
    s16 unkBDC; /* 0xBDC */
    u8 padBDE[0xBE8 - 0xBDE];
    u8 unkBE8[4]; /* 0xBE8 */
    u8 padBEC[0xC20 - 0xBEC];
    s8 unkC20; /* 0xC20 */
    s8 unkC21; /* 0xC21 */
    s8 unkC22; /* 0xC22 */
    s8 unkC23; /* 0xC23 */
    u8 unkC24; /* 0xC24 */
    s8 unkC25; /* 0xC25 */
    u8 unkC26; /* 0xC26 */
    u8 unkC27; /* 0xC27 */
    u8 unkC28; /* 0xC28 */
    u8 unkC29; /* 0xC29 */
    u8 unkC2A; /* 0xC2A */
    u8 unkC2B; /* 0xC2B */
    u8 unkC2C; /* 0xC2C: told the others (func_00576A70) */
    u8 padC2D;
    s16 unkC2E; /* 0xC2E */
    u8 padC30[0xC36 - 0xC30];
    s8 unkC36; /* 0xC36 */
    u8 unkC37; /* 0xC37 */
    u8 padC38[0xC3C - 0xC38];
    f32 unkC3C; /* 0xC3C: unkBD4 last frame */
    u8 padC40[0xC70 - 0xC40];
    Vec4f unkC70; /* 0xC70 */
    u8 unkC80; /* 0xC80 */
    u8 padC81;
    u8 unkC82; /* 0xC82 */
    u8 padC83;
    s8 unkC84; /* 0xC84 */
    u8 unkC85; /* 0xC85 */
    u8 unkC86; /* 0xC86 */
    u8 unkC87; /* 0xC87 */
    u8 unkC88; /* 0xC88 */
    u8 unkC89; /* 0xC89 */
    u8 unkC8A; /* 0xC8A */
    u8 unkC8B; /* 0xC8B */
    s32 unkC8C; /* 0xC8C */
    s32 unkC90; /* 0xC90 */
    s32 unkC94; /* 0xC94 */
    s8 unkC98; /* 0xC98 */
    u8 padC99;
    u8 unkC9A; /* 0xC9A */
    u8 unkC9B; /* 0xC9B */
    u8 padC9C[0xCA0 - 0xC9C];
    s8 unkCA0; /* 0xCA0 */
    u8 unkCA1; /* 0xCA1 */
    u8 padCA2[0xCCC - 0xCA2];
    u8 color[4]; /* 0xCCC */
    u8 padCD0[0xCF8 - 0xCD0];
    ThingPush pushA; /* 0xCF8 */
    ThingPush pushB; /* 0xD0A */
    u8 unkD1C[0x10]; /* 0xD1C */
    u8 unkD2C; /* 0xD2C */
    u8 padD2D[0xD40 - 0xD2D];
    u8 unkD40; /* 0xD40 */
    u8 padD41[0xD45 - 0xD41];
    s8 unkD45; /* 0xD45 */
    u8 padD46[0xD55 - 0xD46];
    u8 unkD55; /* 0xD55 */
    u8 padD56[0xD7D - 0xD56];
    u8 unkD7D; /* 0xD7D: standing on a kind-0x81 floor */
    u8 padD7E[0xD90 - 0xD7E];
} Thing;                /* 0xD90 */

#endif
