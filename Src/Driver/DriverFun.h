

#ifndef _DRIVER_FUN_H
#define _DRIVER_FUN_H

#include "DriverIo.h"
#include "DriverTimer.h"

#define FUN_SPEED_MAX() {IO_Set(CTRL_FUN_POWER, True); for(int i = TIMER_PWM_1; i <= TIMER_PWM_5; i++) TimerDrv_SetDutyCycle((PwmTimer)i, 99);}
#define FUN_SPEED_MIN()	{IO_Set(CTRL_FUN_POWER, True); for(int i = TIMER_PWM_1; i <= TIMER_PWM_5; i++) TimerDrv_SetDutyCycle((PwmTimer)i, 50);}
#define FUN_POWER_OFF()	{IO_Set(CTRL_FUN_POWER, False); for(int i = TIMER_PWM_1; i <= TIMER_PWM_5; i++) TimerDrv_SetDutyCycle((PwmTimer)i, 3);}

//·çÉÈ¿ª¹Ø
#define FUN_POWER_SWITCH(isOn)		IO_Set(CTRL_FUN_POWER, isOn)

#define FUN_SET_SPEED(fun, pulse)	TimerDrv_SetDutyCycle(fun, pulse)

#endif //_DRIVER_UART_H
