
#ifndef _FSM_H_
#define _FSM_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

	//BP事件定义
	typedef enum
	{
		BP_BAT_PLUGIN = 1,	//BP电池插入
		BP_BAT_PLUGOUT,		//BP电池拔出
		BP_BAT_SOC_LESS,	//电池电量不足
		BP_AC_ON,			//市电供电
		BP_12VBP_DET_OFF,	//12VBP_DET_OFF
	}BpEventType;

	typedef enum
	{
		S_INIT = 0,				//初始化
		S0_18650POWERED,	//18650供电
		S1_48V_POWER,		//电池供电
		S2_AC_POWERED,		//交流供电
	}FsmState;

	typedef struct
	{
		FsmState state;

		Bool isBpStart;	//BP系统是否启动。
	}Fsm;
	void Fsm_Init();

#ifdef __cplusplus
}
#endif

#endif
