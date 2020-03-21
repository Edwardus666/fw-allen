
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
����˵��:�����ڲ���ʱ��
����ֵ:
	True:ͨ��״̬�����ı䡣
	False:ͨ��״̬û�иı䡣
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
����˵��:�����ָ����ʱ���ڣ��Ƿ�����ʱ�¼�
����ֵ:
	True:ͨ��״̬�����ı䡣
	False:ͨ��״̬û�иı䡣
*/
Bool CommChecker_IsOn(CommChecker* pCommChecker, uint32 timeOutTicks)
{
	Bool isChanged = False;
	
	if(!pCommChecker->m_isStart) return False;
	
	if(0 == pCommChecker->m_InitTicks)	//��ʼ��������g_initTicks
	{
		pCommChecker->m_InitTicks = GET_TICKS();
	}
	else if(SwTimer_isTimerOutEx(pCommChecker->m_InitTicks, timeOutTicks))	//�����ʱ��˵����5��ÿû�и���g_initTicks
	{
		if(!pCommChecker->m_isTimeOut)
		{
			isChanged = True;
			pCommChecker->m_isTimeOut = True;
		}
	}
	return isChanged;
}

