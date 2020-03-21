

#ifndef _DRIVER_LED_H
#define _DRIVER_LED_H

#include <stdint.h>
#include "SafeBuf.h"
#include "common.h"

typedef enum
{
	RUN_LED = 0,
	LED3 = 1,
	LED4 = 2,
	LED5 = 3
} led_typedef_enum;

typedef struct
{
	uint32 periph;
	uint32 port;
	uint32 pin;
	rcu_periph_enum clk;

	Bool isOn;
}LedDriver;

void LedDriver_Init(LedDriver* pLedDriver);
void LedDriver_On(LedDriver* pLedDriver);
void LedDriver_Off(LedDriver* pLedDriver);
Bool LedDriver_IsOn(LedDriver* pLedDriver);

#endif //_DRIVER_UART_H
