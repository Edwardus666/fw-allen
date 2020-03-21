
#include "Common.h"
#include "CostTime.h"

void CostTime_Init(CostTime* pCostTime, const char* m_pDesc)
{
	memset(pCostTime, 0, sizeof(CostTime));

	pCostTime->m_pDesc = m_pDesc;
	pCostTime->m_MinTime = 10000;
}

void CostTime_CaptureStart(CostTime* pCostTime)
{
	pCostTime->m_StartTime = GET_TICKS();
}

void CostTime_CaptureEnd(CostTime* pCostTime)
{
	uint32 timeInterval = 0;
	
	pCostTime->m_EndTime = GET_TICKS();
	pCostTime->m_Counter++;

	if(pCostTime->m_EndTime < pCostTime->m_StartTime)
	{
		//tick counter overflow
		timeInterval = 0xFFFFFFFF - pCostTime->m_EndTime + pCostTime->m_StartTime;
	}
	else
	{
		timeInterval  = pCostTime->m_EndTime - pCostTime->m_StartTime;
	}

	pCostTime->m_MaxTime = MAX(timeInterval, pCostTime->m_MaxTime);
	pCostTime->m_MinTime = MIN(timeInterval, pCostTime->m_MinTime);

	if(0xFFFFFFFF - pCostTime->m_TotalTime < timeInterval)
	{
		Printf("TotalTime overflow!\n");
	}
	
	pCostTime->m_TotalTime += timeInterval;

	pCostTime->m_AverageTime = (double)pCostTime->m_TotalTime / pCostTime->m_Counter;
}

void CostTime_Dump(CostTime* pCostTime)
{
	Printf("%s: ", pCostTime->m_pDesc);
	Printf("\t Counter=%d."	, pCostTime->m_Counter);
	Printf("\t Total=%d."		, pCostTime->m_TotalTime);
	Printf("\t MaxTime=%d."	, pCostTime->m_MaxTime);
	Printf("\t MinTime=%d."	, pCostTime->m_MinTime);
	Printf("\t Average=%.2f.\n"	, pCostTime->m_AverageTime);
}

