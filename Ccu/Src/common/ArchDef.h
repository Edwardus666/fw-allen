#ifndef __ARCH_CFG_H_
#define __ARCH_CFG_H_

#ifdef __cplusplus
extern "C"{
#endif

#define CFG_LOG

#define MAX_MSG_COUNT		60	//Message queue deepth
#define MAX_TIMER_COUNT		4
#define MS_STICKS 			1	//Hot many ticks per ms, it is for timer
#define TIMER_TASK_INTERVAL 1
#define MAX_TEMP_BUF_SIZE 	512

#ifdef WIN32
	#define _CRT_SECURE_NO_WARNINGS
	
	#include <windows.h>
	#include <stdio.h>

	#define SLEEP 		Sleep
	#define GET_TICKS() GetTickCount()
	#define UDELAY(...)
	
	extern CRITICAL_SECTION g_CriticalSection;

#else
	#define MDELAY(mS) TIMER_Delay(TIMER0, (mS)*1000)
	#define UDELAY(uS) TIMER_Delay(TIMER0, (uS))
	#define MUTEX_LOCK() {__disable_irq()
	#define MUTEX_UNLOCK() __enable_irq();}
	extern volatile unsigned int system_ms_tick;
	#define GET_TICKS( ) system_ms_tick
#endif


#include "typedef.h"	
#include "Debug.h"	
#include "String.h"
#include "RetCode.h"
#include <stdio.h>
#include <stdlib.h>


typedef int (*EventProcFun)(void* pObj, uint8 eventId, void* pData, int len);
typedef struct _FsmMap
{
	uint8 			m_State;
	EventProcFun	Proc;
}FsmMap;


typedef struct _ArchDef
{
	uint8 id;
}ArchDef;

typedef void (*TaskFun)(void* pArg);
typedef void (*voidObjFun)(void* pObj);
typedef int (*ObjFun)(void* pObj);
typedef int (*ObjIsFun)(void* pObj);
typedef int (*ObjSetFun)(void* pObj, uint32 value);
typedef Bool (*EnableFun)(void* pObj, Bool isEnable);

#define READ_REG32(reg) (uint32)(*(vuint32*)(reg))
#define WRITE_REG32(reg, vvalue) (*(vuint32*)(reg)) = vvalue
#define MODIFY_REG32(reg, clear_mask, set_mask) ((*(vuint32*)(reg)) = ((*(vuint32*)(reg)) &~ clear_mask) | set_mask)
#define MIN(v1, v2) ((v1) > (v2) ? (v2) : (v1))
#define MAX(v1, v2) ((v1) < (v2) ? (v2) : (v1))
#define GET_ELEMENT_COUNT(array) (sizeof(array)/sizeof(array[0]))
#define Assert ASRT
#define AssertTrue(expr) if(expr) \
	{	\
		Printf("%s() line=%d Test passed.\n",_FUNC_, _LINE_);	\
	}	\
	else	\
	{	\
		Printf("%s() line=%d Test failed.\n",_FUNC_, _LINE_); \
		while(1);\
	}
#ifdef __cplusplus
}
#endif

#endif




