

#ifndef _DRIVER_LED_H
#define _DRIVER_LED_H

#include "common.h"

typedef enum
{
	TIMER_PWM_1,	//PC9, TIMER2_CH3, 风扇1调速，PWM控制
	TIMER_PWM_2,	//PC8, TIMER2_CH2, 风扇2调速，PWM控制
	TIMER_PWM_3,	//PC7, TIMER2_CH1, 风扇3调速，PWM控制
	TIMER_PWM_4,	//PC6, TIMER2_CH0, 风扇4调速，PWM控制
	TIMER_PWM_5,	//PD15, TIMER3_CH3, 风扇5调速，PWM控制
} PwmTimer;

typedef struct
{
	PwmTimer tid;

	/* TIMERx(x=0,2,3,5..13) definitions */
	uint32 periph;

	/*
	 TIMER_CH_0: TIMER channel0(TIMERx(x=0,2,3,7..13))
	 TIMER_CH_1: TIMER channel1(TIMERx(x=0,2,3,7,8,11))
	 TIMER_CH_2: TIMER channel2(TIMERx(x=0,2,3,7))
	 TIMER_CH_3: TIMER channel3(TIMERx(x=0,2,3,7))
	*/
	uint16  timerChnl;        
	/* GPIOx(x=A,B,C,D,E,F,G) definitions */
	uint32 ioPort;
	/* GPIO_PIN_x(x=0,...,15) definitions */
	uint32 pin;

	uint8 dutyCycle;

}TimerDrv;

void TimerDrv_Init();
void TimerDrv_SetDutyCycle(PwmTimer tid, uint8 dutyCycle);

#endif //_DRIVER_UART_H
