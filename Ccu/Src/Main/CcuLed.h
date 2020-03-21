
#ifndef _CCU_LED_H_
#define _CCU_LED_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "Led.h"

	typedef enum
	{
		LED_RUN_MODE,
		LED_RELAY_1_2_FAULT,
		LED_BP_NO_BATTERY_OR_SOC_LESS,
	}LedID;

	void RunLed_Init();
	void RunLed_SetMode(LedID mode);

#ifdef __cplusplus
}
#endif

#endif
