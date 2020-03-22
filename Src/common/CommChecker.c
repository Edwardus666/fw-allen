
#include "common.h"
#include "CommChecker.h"

void CommChecker_Init(CommChecker* pCommChecker)
{
	memset(pCommChecker, 0, sizeof(CommChecker));
}

void CommChecker_Start(CommChecker* pCommChecker)
{
	pCommChecker->m_isStart = True;
	pCommChecker->m_isTimeOut = False;
	pCommChecker->m_InitTicks = GET_TICKS();
}

void CommChecker_Stop(CommChecker* pCommChecker)
{
	pCommChecker->m_isStart = False;
	pCommChecker->m_isTimeOut = False;
}
/*
函数说明:重置内部定时器
返回值:
	True:通信状态发生改变。
	False:通信状态没有改变。
*/
Bool CommChecker_On(CommChecker* pCommChecker)
{
	Bool isChanged = False;

	if(pCommChecker->m_isTimeOut)
	{
		isChanged = True;
		pCommChecker->m_isTimeOut = False;
	}
	pCommChecker->m_InitTicks = GET_TICKS();

	return isChanged;
}

/*
函数说明:检测在指定的时间内，是否发生超时事件
返回值:
	True:通信状态发生改变。
	False:通信状态没有改变。
*/
Bool CommChecker_IsOn(CommChecker* pCommChecker, uint32 timeOutTicks)
{
	Bool isChanged = False;
	
	if(!pCommChecker->m_isStart) return False;
	
	if(0 == pCommChecker->m_InitTicks)	//初始化计数器g_initTicks
	{
		pCommChecker->m_InitTicks = GET_TICKS();
	}
	else if(SwTimer_isTimerOutEx(pCommChecker->m_InitTicks, timeOutTicks))	//如果超时，说明在5秒每没有更新g_initTicks
	{
		if(!pCommChecker->m_isTimeOut)
		{
			isChanged = True;
			pCommChecker->m_isTimeOut = True;
		}
	}
	return isChanged;
}

