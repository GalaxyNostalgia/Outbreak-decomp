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

#endif
