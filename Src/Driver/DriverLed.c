
#include "Common.h"
#include "DriverLed.h"
#include "Led.h"

void  LedDriver_Init(LedDriver* pLedDriver)
{
	/* enable the led clock */
	rcu_periph_clock_enable(pLedDriver->clk);
	/* configure led GPIO port */
	gpio_init(pLedDriver->periph, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pLedDriver->pin);

	LedDriver_Off(pLedDriver);
}

Bool LedDriver_IsOn(LedDriver* pLedDriver)
{
	return pLedDriver->isOn;
}

void LedDriver_On(LedDriver* pLedDriver)
{
	GPIO_BOP(pLedDriver->periph) = pLedDriver->pin;
	pLedDriver->isOn = True;
}

void LedDriver_Off(LedDriver* pLedDriver)
{
	GPIO_BC(pLedDriver->periph) = pLedDriver->pin;
	pLedDriver->isOn = False;
}
