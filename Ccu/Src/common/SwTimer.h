#ifndef __SWTIMER_H_
#define __SWTIMER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

typedef struct _SwTimer
{
	Bool	m_isStart;
	uint32 	m_InitTicks;
	uint32 	m_TimeOutTicks;
	uint32 	m_Id;
}SwTimer;

void SwTimer_Init(SwTimer* pTimer, uint32 timeOutMs, uint32 nId);
void SwTimer_ReStart(SwTimer* pTimer);
void SwTimer_Start(SwTimer* pTimer, uint32 timeOutMs, uint32 nId);
void SwTimer_Stop(SwTimer* pTimer);
void SwTimer_StopEx(SwTimer* pTimer, uint32 timeId);
Bool SwTimer_isTimerOut(SwTimer* pTimer);
Bool SwTimer_isTimerOutEx(uint32 initTicks, uint32 timeOutTicks);
Bool SwTimer_IsStart(SwTimer* pTimer);
Bool SwTimer_isTimerOutId(SwTimer* pTimer, uint32 timeId);


#ifdef __cplusplus
}
#endif

#endif

