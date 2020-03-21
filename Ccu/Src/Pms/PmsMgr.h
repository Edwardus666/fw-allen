#ifndef __PMS_MGR_H_
#define __PMS_MGR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "Obj.h"
#include "Pms.h"

#define PMS_COUNT 15

typedef struct _PmsMgr
{
	Obj		base;
	uint8	state;
}PmsMgr;

extern Pms g_Pms[PMS_COUNT];

void PmsMgr_Init();
void PmsMgr_Start();
void PmsMgr_Run();
void PmsMgr_Start();
void PmsMgr_Stop();
Pms* PmsMgr_GetByAddr(uint8 addr);
void PmsMgr_SetPmsPoweredMode(PmsPoweredMode mode);

#ifdef __cplusplus
}
#endif

#endif

