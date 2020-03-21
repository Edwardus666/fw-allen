
#ifndef __COST_TIME__H_
#define __COST_TIME__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

typedef struct _CostTime
{
	uint32 m_Counter;
	
	uint32 m_TotalTime;
	double  m_AverageTime;
	
	uint32 m_StartTime;
	uint32 m_EndTime;
	
	uint32 m_MaxTime;
	uint32 m_MinTime;
	
	const char* m_pDesc;
}CostTime;

void CostTime_Init(CostTime* pCostTime, const char* m_pDesc);
void CostTime_CaptureStart(CostTime* pCostTime);
void CostTime_CaptureEnd(CostTime* pCostTime);
void CostTime_Dump(CostTime* pCostTime);


#ifdef __cplusplus
}
#endif

#endif

