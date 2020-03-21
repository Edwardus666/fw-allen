
#include "Common.h"
#include "DriverTimer.h"
#include "DriverFun.h"

/*
1. 所有风扇一起工作
2. 风扇实现3档
	1）停止档：停止，关电，0%占空比。
	2）低档：50%占空比
	3）高档：90%占空比
*/

static TimerDrv g_Timer[] =
{
	{TIMER_PWM_1, TIMER2, TIMER_CH_3, GPIOC, GPIO_PIN_9 , 3},
	{TIMER_PWM_2, TIMER2, TIMER_CH_2, GPIOC, GPIO_PIN_8 , 3},
	{TIMER_PWM_3, TIMER2, TIMER_CH_1, GPIOC, GPIO_PIN_7 , 3},
	{TIMER_PWM_4, TIMER2, TIMER_CH_0, GPIOC, GPIO_PIN_6 , 3},
	{TIMER_PWM_5, TIMER3, TIMER_CH_3, GPIOD, GPIO_PIN_15, 3},
};

/* -----------------------------------------------------------------------
TIMERCLK = SystemCoreClock / g_TimerPrescaler = 25K
TIMERx channelx duty cycle = (pulse / g_TimerPeriod)* 100
----------------------------------------------------------------------- */
const static uint32 g_TimerPrescaler = 168;	//g_TimerPrescaler = SystemCoreClock / 1M = 168
const static uint32 g_TimerPeriod = 40;		//1M/40 = 25K

TimerDrv* TimerDrv_Get(PwmTimer tid)
{
	TimerDrv* p = g_Timer;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Timer); i++, p++)
	{
		if (p->tid == tid)
		{
			return p;
		}
	}

	Assert(False);
	return Null;
}

void TimerDrv_SetClock(PwmTimer tid, uint32 prescaler, uint32 period, uint8 dutyCycle)
{
	TimerDrv* p = TimerDrv_Get(tid);

	if(prescaler)
		TIMER_PSC(p->periph) = (uint16_t)prescaler;

	if (period)
		TIMER_CAR(p->periph) = (uint32_t)period;

	if (dutyCycle)
	{
		TimerDrv_SetDutyCycle(tid, dutyCycle);
	}
}

/*
功能：调整PWM的占空比，风扇转速通过占空比来控制。
dutyCycle，风扇转速，有效转速：5~100, < 3转速为0
*/
void TimerDrv_SetDutyCycle(PwmTimer tid, uint8 dutyCycle)
{
	TimerDrv* p = TimerDrv_Get(tid);
	p->dutyCycle = dutyCycle % 100;
	uint32 pulse = g_TimerPeriod * p->dutyCycle / 100;

	if (dutyCycle == 0)
	{
		timer_disable(p->periph);
	}
	else
	{
		timer_enable(p->periph);
		pulse = (pulse > 2) ? (pulse - 1) : 1;	//最小值为1，
		timer_channel_output_pulse_value_config(p->periph, p->timerChnl, pulse);
	}
}

void TimerDrv_Stop()
{
	/* disable TIMERx */
	TimerDrv* p = g_Timer;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Timer); i++, p++)
	{
		timer_disable(p->periph);
	}

	//风扇关电
	IO_Set(CTRL_FUN_POWER, 0);
}

void TimerDrv_Start()
{
	/* enable TIMERx */
	TimerDrv* p = g_Timer;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Timer); i++, p++)
	{
		timer_enable(p->periph);
	}

	//风扇上电,所有风扇共用一个电源开关
	FUN_POWER_SWITCH(True);
}

void TimerDrv_InitEx(TimerDrv* pTimer)
{
	///* enable TIMER clock */
	//rcu_periph_clock_enable(Rcu_Get(pTimer->periph));
	///* enable GPIO clock */
	//rcu_periph_clock_enable(Rcu_Get(pTimer->ioPort));


	//timer_parameter_struct timer_initpara;
	//timer_initpara.prescaler = g_TimerPrescaler - 1;
	//timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	//timer_initpara.counterdirection = TIMER_COUNTER_UP;
	//timer_initpara.period = g_TimerPeriod - 1;
	//timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	//timer_initpara.repetitioncounter = 0;
	//timer_deinit(pTimer->periph);
	//timer_init(pTimer->periph, &timer_initpara);

	gpio_init(pTimer->ioPort, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pTimer->pin);

	/* CH0 configuration in PWM mode0 */
	timer_oc_parameter_struct timer_ocintpara;
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_LOW;
	timer_ocintpara.ocnpolarity = TIMER_OCN_POLARITY_LOW;
	timer_ocintpara.ocidlestate = TIMER_OC_IDLE_STATE_HIGH;
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_HIGH;

	timer_channel_output_config				(pTimer->periph, pTimer->timerChnl, &timer_ocintpara);
	timer_channel_output_pulse_value_config (pTimer->periph, pTimer->timerChnl, 1);
	timer_channel_output_mode_config		(pTimer->periph, pTimer->timerChnl, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config		(pTimer->periph, pTimer->timerChnl, TIMER_OC_SHADOW_DISABLE);

	/* TIMERx primary output enable */
	//timer_primary_output_config(pTimer->periph, ENABLE);

	/* auto-reload preload enable */
	//timer_auto_reload_shadow_enable(pTimer->periph);

}

void TimerDrv_Init()
{
	static Obj g_TimerObj;
	Obj_Register(&g_TimerObj, "PWM", TimerDrv_Start, TimerDrv_Stop, Null);

	/* enable TIMER clock */
	rcu_periph_clock_enable(RCU_TIMER2);
	rcu_periph_clock_enable(RCU_TIMER3);
	/* enable GPIO clock */
	rcu_periph_clock_enable(RCU_GPIOC);
	rcu_periph_clock_enable(RCU_GPIOD);

	/* enable AF clock */
	rcu_periph_clock_enable(RCU_AF);

	/* TIMERx configuration */
	/* -----------------------------------------------------------------------
	TIMER configuration: generate 3 PWM signals with 3 different duty cycles:
	TIMERCLK = SystemCoreClock / 168 = 1MHz

	TIMERx channelx duty cycle = (4000 / 16000)* 100  = 25%
	----------------------------------------------------------------------- */
	timer_parameter_struct timer_initpara;
	timer_initpara.prescaler = g_TimerPrescaler - 1;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = g_TimerPeriod - 1;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_deinit(TIMER2);
	timer_deinit(TIMER3);
	timer_init(TIMER2, &timer_initpara);
	timer_init(TIMER3, &timer_initpara);

	TimerDrv* p = g_Timer;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Timer); i++, p++)
	{
		TimerDrv_InitEx(p);
	}

	gpio_pin_remap_config(GPIO_TIMER2_FULL_REMAP, ENABLE);
	gpio_pin_remap_config(GPIO_TIMER3_REMAP, ENABLE);

	///* auto-reload preload enable */
	timer_auto_reload_shadow_enable(TIMER2);
	timer_auto_reload_shadow_enable(TIMER3);
}
