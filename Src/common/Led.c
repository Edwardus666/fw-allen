#include "Led.h"
#include "SwTimer.h"

#define DEFAULT_TIME_OUT  500

void Led_Set(Led* pLed, uint8 status)
{
	if(pLed->m_State != status)
	{
		pLed->SetPower(pLed, status);
		pLed->m_State = status;
	}
	pLed->m_StartTimeMs = GET_TICKS();
	pLed->m_TimeOutMs = (pLed->m_State == LED_STATUS_ON) ? pLed->m_pWorkMode->m_onMs: pLed->m_pWorkMode->m_offMs;
}

void Led_Shine(Led* pLed, uint32 onMs, uint32 offMs)
{
	if(0 == pLed->m_StartTimeMs)
	{
		//The first power on LED.
		pLed->m_isDelay = False;
		Led_Set(pLed, LED_STATUS_ON);
	}
	else if(SwTimer_isTimerOutEx(pLed->m_StartTimeMs, pLed->m_TimeOutMs))
	{
		if(pLed->m_State == LED_STATUS_OFF)
		{
			pLed->m_count++;
			if(pLed->m_MaxCount == 0 || (pLed->m_count >= pLed->m_MaxCount))
			{
				//如果中间间隔小于500ms，则必须延长到500ms
				if(pLed->m_TimeOutMs < 500 && !pLed->m_isDelay)
				{
					pLed->m_isDelay = True;
					Led_Set(pLed, LED_STATUS_OFF);
					pLed->m_TimeOutMs = 1000-pLed->m_TimeOutMs;
					return;
				}
				pLed->m_pWorkMode = Null;	//结束当前闪烁过程
				return;
			}
		}
		
		Led_Set(pLed, pLed->m_State == LED_STATUS_ON ? LED_STATUS_OFF : LED_STATUS_ON);
	}
}

void Led_SetWorkMode(Led* pLed, const LedState* pLedState)
{
	if(pLed->m_pWorkMode != pLedState || pLedState == Null)
	{
		pLed->SetPower(pLed, LED_STATUS_OFF);
		pLed->m_State = LED_STATUS_OFF;
		pLed->m_pWorkMode = pLedState;
		pLed->m_StartTimeMs = 0;
		pLed->m_TimeOutMs 	= 0;
		pLed->m_count	 	= 0;
		pLed->m_MaxCount 	= 0;
		pLed->m_isDelay 	= False;
	}
}

void Led_SetWorkModeEx(Led* pLed, const LedState* pLedState, uint8 count)
{
	if(pLed->m_pWorkMode)	//如果当前有WorkMode正在运行还没有完成。
	{
		pLed->m_pPendingWorkMode = pLedState;
		pLed->m_PeindingMaxCount = count;
	}
	else
	{
		Led_SetWorkMode(pLed, pLedState);
		pLed->m_MaxCount = count;
	}
}

void Led_Reset(Led* pLed)
{
	pLed->SetPower(pLed, LED_STATUS_OFF);
	pLed->m_State = LED_STATUS_OFF;
	pLed->m_pWorkMode = Null;
	pLed->m_StartTimeMs = 0;
	pLed->m_TimeOutMs 	= 0;
	pLed->m_count	 	= 0;
	pLed->m_MaxCount 	= 0;
}

void Led_Run(Led* pLed)
{
	const LedState* pLedState = pLed->m_pWorkMode;
	if(pLedState == Null)
	{
		if(pLed->m_pPendingWorkMode)
		{
			Led_SetWorkModeEx(pLed, pLed->m_pPendingWorkMode, pLed->m_PeindingMaxCount);
		}
		pLed->m_pPendingWorkMode = Null;
		pLed->m_PeindingMaxCount = 0;
		
		pLedState = pLed->m_pWorkMode;
		if(pLedState == Null) 
			return;
	}

	if(pLedState->m_State == LED_STATUS_OFF || pLedState->m_State == LED_STATUS_ON)
	{
		Led_Set(pLed, pLedState->m_State);
	}
	else if(pLedState->m_State == LED_STATUS_SHINE)
	{
		Led_Shine(pLed, pLedState->m_onMs, pLedState->m_offMs);
	}
}

void Led_Init(Led* pLed, uint8 ledID, LedSetPowerFn LedSetPower)
{
	memset(pLed, 0, sizeof(Led));

	pLed->m_LedID = ledID;
	pLed->SetPower = LedSetPower;
	
	pLed->SetPower(pLed, LED_STATUS_OFF);
	pLed->m_State = LED_STATUS_OFF;
}


