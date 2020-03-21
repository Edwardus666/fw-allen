
#include "SwTimer.h"
#include "Archdef.h"

void SwTimer_Init(SwTimer* pTimer, uint32 timeOutMs, uint32 nId)
{
	memset(pTimer, 0, sizeof(SwTimer));
	
	pTimer->m_TimeOutTicks  = timeOutMs;
	pTimer->m_Id 			= nId;
}

void SwTimer_Start(SwTimer* pTimer, uint32 timeOutMs, uint32 nId)
{
	pTimer->m_Id 			= nId;
	pTimer->m_TimeOutTicks 	= timeOutMs;
	
	pTimer->m_InitTicks = GET_TICKS();
	pTimer->m_isStart 	= True;
}

Bool SwTimer_isTimerOutEx(uint32 initTicks, uint32 timeOutTicks)
{
	uint32 totalTicks = 0;
	uint32 newTicks = GET_TICKS();
	
	if(newTicks < initTicks)
	{
		//tick counter overflow
		totalTicks = 0xFFFFFFFF - initTicks + newTicks;
	}
	else
	{
		totalTicks = newTicks - initTicks;
	}

	return (totalTicks >= timeOutTicks);
}

Bool SwTimer_isTimerOutId(SwTimer* pTimer, uint32 timeId)
{
	return (timeId == pTimer->m_Id && SwTimer_isTimerOut(pTimer));
}

Bool SwTimer_isTimerOut(SwTimer* pTimer)
{
	uint32 totalTicks = 0;
	uint32 newTicks = GET_TICKS();

	if(!pTimer->m_isStart) 
		return False;
	
	if(newTicks < pTimer->m_InitTicks)
	{
		//tick counter overflow
		totalTicks = 0xFFFFFFFF - pTimer->m_InitTicks + newTicks;
	}
	else
	{
		totalTicks = newTicks - pTimer->m_InitTicks;
	}

	if(totalTicks >= pTimer->m_TimeOutTicks)
	{
		pTimer->m_isStart = False;
		return True;
	}
	return False;
}

Bool SwTimer_IsStart(SwTimer* pTimer)
{
	return pTimer->m_isStart;
}

void SwTimer_StopEx(SwTimer* pTimer, uint32 timeId)	
{
	if(timeId == pTimer->m_Id)
	{
		pTimer->m_isStart = False ;
	}
}

void SwTimer_Stop(SwTimer* pTimer)	
{
	pTimer->m_isStart = False ;
}

void SwTimer_ReStart(SwTimer* pTimer)
{
	if(pTimer->m_TimeOutTicks == 0) return;
	pTimer->m_InitTicks = GET_TICKS(); pTimer->m_isStart = True;
}

