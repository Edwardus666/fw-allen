#ifndef __CONNECT_CHECKER__H_
#define __CONNECT_CHECKER__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "SwTimer.h"

/*
模块功能:检测通信状态是否发生改变
1. 在有通信的地方调用 CommChecker_Update()
2. 在检查的地方调用CommChecker_Check()
*/
typedef struct _CommChecker
{
	uint32  m_InitTicks;
	
	uint8	m_isTimeOut:1;
	uint8	m_isStart:1;
	uint8	m_Reserved:6;
}CommChecker;

void CommChecker_Init(CommChecker* pCommChecker);
Bool CommChecker_On(CommChecker* pCommChecker);
Bool CommChecker_IsOn(CommChecker* pCommChecker, uint32 timeOutTicks);
void CommChecker_Start(CommChecker* pCommChecker);
void CommChecker_Stop(CommChecker* pCommChecker);

#ifdef __cplusplus
}
#endif

#endif

