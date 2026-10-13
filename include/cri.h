#ifndef CRI_H
#define CRI_H

#include "common.h"

/* CRI Middleware (ADX sound, Sofdec movies) calls the game makes. Names come from CRI's own
   error messages in the library code (see config/SLPM_654.28/symbol_addrs.txt). */

typedef void *ADXT;

void ADXPS2_SetupVoice(s32 nStereo, s32 nMono);

void ADXT_Init(void);
ADXT ADXT_Create(s32 maxnch, void *work, s32 worksize);
void ADXT_Stop(ADXT adxt);
s32 ADXT_GetStat(ADXT adxt);           /* 3: playing */
void ADXT_SetOutVol(ADXT adxt, s32 vol);
void ADXT_SetSvrFreq(ADXT adxt, s32 freq);
void ADXT_SetReloadSct(ADXT adxt, s32 sct);
void ADXT_Pause(ADXT adxt, s32 on);
void ADXT_StartAfs(ADXT adxt, s32 ptid, s32 fid);
void ADXT_SetWaitPlayStart(ADXT adxt, s32 flag);
s32 ADXT_IsReadyPlayStart(ADXT adxt);

void ADXF_LoadPartitionNw(s32 ptid, const char *fname, void *dir, void *ptinfo);
void ADXF_LoadPtFromAfsNw(s32 ptid, s32 afs_ptid, s32 fid, void *ptinfo);
s32 ADXF_GetPtStat(s32 ptid);          /* 3: loaded */
typedef void *ADXF;
ADXF ADXF_Open(const char *fname, void *atr);
ADXF ADXF_OpenAfs(s32 ptid, s32 fid);
void ADXF_Close(ADXF adxf);
s32 ADXF_ReadNw(ADXF adxf, s32 nsct, void *buf);
s32 ADXF_GetFsizeSct(ADXF adxf);
s32 ADXF_GetStat(ADXF adxf);           /* 3: read finished */

/* Sofdec: the movie player (mwPly handles; the game only plays from AFS archives) */
typedef struct MwPly MwPly;
typedef struct {
    u8 unk0[0x14];
    void (*destroy)(MwPly *ply);        /* 0x14 */
    void *unk18;
    void (*stop)(MwPly *ply);           /* 0x1C */
    s32 (*getStat)(MwPly *ply);         /* 0x20: 0 stop, 1 prep, 2 playing, 3 end, 4 error */
    void (*getTime)(MwPly *ply, s32 *time, s32 *tunit); /* 0x24 */
} MwPlyVtbl;

struct MwPly {
    MwPlyVtbl *vtbl;
};

typedef struct {
    s32 ftype;          /* 0x00: 3 */
    s32 maxBps;         /* 0x04 */
    s32 maxWidth;       /* 0x08 */
    s32 maxHeight;      /* 0x0C */
    s32 nfrmPool;       /* 0x10 */
    s32 unk14;          /* 0x14: 2 */
    u32 work;           /* 0x18 */
    u32 workSize;       /* 0x1C */
    s32 unk20;          /* 0x20: 0x11 */
    u8 unk24[0xC];
} MwsfdCrePrm;

typedef struct {
    u32 bufadr;         /* 0x00: the picture */
    s32 frmid;
    s32 width;          /* 0x08 */
    s32 height;         /* 0x0C */
    u8 unk10[0x10];
    s32 frmno;          /* 0x20 */
    u8 unk24[0x4C - 0x24];
} MwsfdFrmObj;

typedef struct {
    s32 dropFrm;
    s32 skipDec;
    s32 skipDisp;
    s32 unkC;
    s32 unk10;
    u8 unk14[0x20 - 0x14];
} MwsfdPlyInf;

u32 mwPlyCalcWorkCprmSfd(MwsfdCrePrm *cprm);
MwPly *mwPlyCreateSofdec(MwsfdCrePrm *cprm);
void mwPlyGetCurFrm(MwPly *ply, MwsfdFrmObj *frm);
void mwPlyRelCurFrm(MwPly *ply, MwsfdFrmObj *frm);
void mwPlyStartAfs(MwPly *ply, s32 ptid, s32 fid);
void mwPlyGetPlyInf(MwPly *ply, MwsfdPlyInf *inf);
void mwPlyStartSeamless(MwPly *ply);
void mwPlySetLpFlg(MwPly *ply, s32 flag);
void mwPlyEntryAfs(MwPly *ply, s32 ptid, s32 fid);

#endif
