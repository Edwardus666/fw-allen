
#include "Common.h"
#include "DriverAdc.h"
#include "Ntc.h"
#include "CO_OD.h"


static Obj g_AdcObj;
static Ntc* g_pNtc;
//ADC转换之后会把数据通过DMA自动保存在该变量中
#define ADC_VAL_COUNT 7
static uint16 g_AdcRawValue[ADC_VAL_COUNT] = { 0 };
int adc0Count = 0;
int adc1Count = 0;

// ADC_CHANNEL_x(x=0..17)(x=16 and x=17 are only for ADC0): ADC Channelx 

#define ADC_REFV 3.0f
static Adc g_Adc[] =
{
	{ADC_18650_CHARGE_I	,"18650_CHARGE_I" ,GPIOC, GPIO_PIN_5, ADC0, ADC_CHANNEL_15 ,&g_AdcRawValue[0], ADC_REFV, Null, &OD_18650Current},
	{ADC_18650_VOLTAGE	,"18650_VOLTAGE"  ,GPIOB, GPIO_PIN_0, ADC0, ADC_CHANNEL_8  ,&g_AdcRawValue[1], ADC_REFV, Null, &OD_18650Voltage},
	{ADC_18650_TEMP		,"18650_TEMP"     ,GPIOB, GPIO_PIN_1, ADC0, ADC_CHANNEL_9  ,&g_AdcRawValue[2], ADC_REFV, Null },
	
	{ADC_TEMP_1	,"TEMP_1" ,GPIOC, GPIO_PIN_0, ADC0, ADC_CHANNEL_10, &g_AdcRawValue[3], ADC_REFV, &OD_ccuTemp1},
	{ADC_TEMP_2 ,"TEMP_2" ,GPIOC, GPIO_PIN_1, ADC0, ADC_CHANNEL_11, &g_AdcRawValue[4], ADC_REFV, &OD_ccuTemp2},
	{ADC_TEMP_3	,"TEMP_3" ,GPIOC, GPIO_PIN_2, ADC0, ADC_CHANNEL_12, &g_AdcRawValue[5], ADC_REFV, &OD_ccuTemp3},
	{ADC_TEMP_4 ,"TEMP_4" ,GPIOC, GPIO_PIN_3, ADC0, ADC_CHANNEL_13, &g_AdcRawValue[6], ADC_REFV, &OD_ccuTemp4},
};

Adc* Adc_Get(ADC_ID adcId)
{
	Adc* pAdc = g_Adc;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Adc); i++, pAdc++)
	{
		if (pAdc->adcId == adcId)
		{
			return pAdc;
		}
	}
	return Null;
}

void Adc_ItemDump(const Adc* pAdc, const char* pTitle)
{
	if (pTitle == Null) pTitle = "";

	PFL(DL_ADC, "%s%s = %d->%d => %d->%d \n", pTitle, pAdc->name, pAdc->rawValue, *pAdc->pRawValue, pAdc->oldValue, pAdc->newValue);
}

void Adc_Dump()
{
	const Adc* pAdc = g_Adc;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Adc); i++, pAdc++)
	{
		Adc_ItemDump(pAdc, Null);
	}
}

void Adc_EventRegister(ADC_ID field, int thresholdHigh, int thresholdLow, AdcChangedFn changed)
{
	Adc* pAdc = Adc_Get(field);

	pAdc->thresholdHigh = thresholdHigh;
	pAdc->thresholdLow = thresholdLow;
	pAdc->thresholdArea = UNKNOWN_AREA;

	pAdc->OnChanged = changed;
}

int Adc_Convert(Adc* pAdc, uint16 rawVal, float refVoltage)
{
	ADC_ID adcId = pAdc->adcId;
	float val = 0.0;
	if (adcId >= ADC_TEMP_1 && adcId <= ADC_TEMP_4)
	{
		uint32 rVal = Ntc_VoltageToResistance(10000, rawVal, 4096);
		val = Ntc_CalcTemp(g_pNtc, rVal) + 40;
	}
	else if (adcId == ADC_18650_VOLTAGE)
	{
		val = (float)rawVal / 4096 * refVoltage ;
		val *= 1000 * 2;
	}
	else
	{
		val = (float)rawVal / 4096 * refVoltage ;
		val *= 1000;
	}

	if (pAdc->odValue8)  *pAdc->odValue8   = (uint8)val;
	if (pAdc->odValue16) * pAdc->odValue16 = (uint16)val;
	return (int)val;
}

void Adc_OnChanged(Adc* pAdc)
{
	//如果设定阈值，则只有越过阈值才会触发OnChanged
	if (pAdc->thresholdHigh != THRESHOLD_INNVALID || pAdc->thresholdLow != THRESHOLD_INNVALID)
	{
		THRESHOLD_EVENT event = NOT_OVER_THRESHOLD;
		event = Threshold_GetEvent(pAdc->oldValue, pAdc->newValue, pAdc->thresholdHigh, pAdc->thresholdLow, &pAdc->thresholdArea);
		if(event != NOT_OVER_THRESHOLD)
			pAdc->OnChanged(pAdc, event, pAdc->oldValue, pAdc->newValue);
	}
	else
	{
		//否则只要值改变都会触发OnChanged
		pAdc->OnChanged(pAdc, NOT_OVER_THRESHOLD, pAdc->oldValue, pAdc->newValue);
	}

}

void Adc_CheckChanged(Adc* pAdc)
{
	int rawVal = *pAdc->pRawValue;
	if (rawVal != pAdc->rawValue)
	{
		//值转换
		pAdc->newValue = Adc_Convert(pAdc, rawVal, pAdc->refVoltage);

		//判断值是否改变
		if (pAdc->newValue != pAdc->oldValue)
		{
			Adc_ItemDump(pAdc, "");

			if (pAdc->OnChanged)
			{
				Adc_OnChanged(pAdc);
			}

			pAdc->oldValue = pAdc->newValue;
			pAdc->rawValue = rawVal;
		}
	}
}

void Adc_AllStateChanged()
{
	Adc* pAdc = g_Adc;
	for (int j = 0; j < GET_ELEMENT_COUNT(g_Adc); j++, pAdc++)
	{
		pAdc->rawValue = -1;
		Adc_CheckChanged(pAdc);
	}
}

void Adc_CheckValue()
{
	Adc* pAdc = g_Adc;
	for (int j = 0; j < GET_ELEMENT_COUNT(g_Adc); j++, pAdc++)
	{
		Adc_CheckChanged(pAdc);
	}
}

void rcu_config(void)
{
	/* enable GPIOA clock， FOR PA8 */
	rcu_periph_clock_enable(RCU_GPIOA);

	for (int i = 0; i < GET_ELEMENT_COUNT(g_Adc); i++)
	{
		if (g_Adc[i].ioPort)
		{
			rcu_periph_clock_enable(Rcu_Get(g_Adc[i].ioPort));
		}
	}
	//rcu_periph_clock_enable(RCU_GPIOA);
	//rcu_periph_clock_enable(RCU_GPIOB);
	//rcu_periph_clock_enable(RCU_GPIOC);
	/* enable DMA clock */
	rcu_periph_clock_enable(RCU_DMA0);
	/* enable TIMER0 clock */
	rcu_periph_clock_enable(RCU_TIMER0);
	/* enable ADC0 clock */
	rcu_periph_clock_enable(RCU_ADC0);

	if (adc1Count)
	{
		/* enable ADC1 clock */
		rcu_periph_clock_enable(RCU_ADC1);
	}
	/* config ADC clock */
	rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);
}

void gpio_config(void)
{
	/*configure PA8(TIMER0 CH0) as alternate function*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_8);

	/* config the GPIO as analog mode */
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Adc); i++)
	{
		if (g_Adc[i].ioPort)
		{
			gpio_init(g_Adc[i].ioPort, GPIO_MODE_AIN, GPIO_OSPEED_MAX, g_Adc[i].ioPin);
		}
	}
	//gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_6 | GPIO_PIN_7);
	//gpio_init(GPIOB, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0 | GPIO_PIN_1);
	//gpio_init(GPIOC, GPIO_MODE_AIN, GPIO_OSPEED_MAX, GPIO_PIN_0 | GPIO_PIN_1);
}


void dma_config(void)
{
	int number = GET_ELEMENT_COUNT(g_Adc);

	/* ADC_DMA_channel configuration */
	dma_parameter_struct dma_data_parameter;

	/* ADC_DMA_channel deinit */
	dma_deinit(DMA0, DMA_CH0);

	/* initialize DMA single data mode */
	dma_data_parameter.periph_addr = (uint32)(&ADC_RDATA(ADC0));
	dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_data_parameter.memory_addr = (uint32)(&g_AdcRawValue);
	dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
	dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
	dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_data_parameter.number = number;
	dma_data_parameter.priority = DMA_PRIORITY_HIGH;
	dma_init(DMA0, DMA_CH0, &dma_data_parameter);

	dma_circulation_enable(DMA0, DMA_CH0);

	/* enable DMA channel */
	dma_channel_enable(DMA0, DMA_CH0);

}

void adc_setting(uint32_t adc_periph, int chnlCount)
{
	/* ADC scan mode function enable */
	adc_special_function_config(adc_periph, ADC_SCAN_MODE, ENABLE);
	/* ADC trigger config */
	adc_external_trigger_source_config(adc_periph, ADC_REGULAR_CHANNEL, ADC0_1_EXTTRIG_REGULAR_T0_CH0);
	/* ADC data alignment config */
	adc_data_alignment_config(adc_periph, ADC_DATAALIGN_RIGHT);
	/* ADC external trigger enable */
	adc_external_trigger_config(adc_periph, ADC_REGULAR_CHANNEL, ENABLE);

	///* ADC channel length config */
	adc_channel_length_config(adc_periph, ADC_REGULAR_CHANNEL, chnlCount);

	/* enable ADC interface */
	adc_enable(adc_periph);
	delay_1ms(1);
	/* ADC calibration and reset calibration */
	adc_calibration_enable(adc_periph);

	/* ADC DMA function enable */
	adc_dma_mode_enable(adc_periph);
}

void adc_config(void)
{
	int adcChnlLen = GET_ELEMENT_COUNT(g_Adc);
	adc0Count = adc1Count = 0;

	for (int i = 0; i < adcChnlLen; i++)
	{
		if (ADC0 == g_Adc[i].adcPort)
		{
			/* ADC regular channel config */
			adc_regular_channel_config(g_Adc[i].adcPort, adc0Count, g_Adc[i].adcChnl, ADC_SAMPLETIME_239POINT5);
			adc0Count++;
		}
		else if (ADC1 == g_Adc[i].adcPort)
		{
			/* ADC regular channel config */
			adc_regular_channel_config(g_Adc[i].adcPort, adc1Count, g_Adc[i].adcChnl, ADC_SAMPLETIME_239POINT5);

			adc1Count++;
		}

	}
	/* configure the ADC sync mode */
	adc_mode_config(ADC_DAUL_REGULAL_PARALLEL);

	if (adc0Count)
	{	
		adc_setting(ADC0, adc0Count);
	}
	if (adc1Count)
	{
		adc_setting(ADC1, adc1Count);
	}
}

void timer_config(void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	/* -----------------------------------------------------------------------
	TIMER configuration: generate 3 PWM signals with 3 different duty cycles:
	TIMERCLK = SystemCoreClock(168000000) / 8400 = 20,000Hz = 50us

	TIMERx channelx duty cycle = (50us * 30000)  = 1.5s
	----------------------------------------------------------------------- */
	/* TIMER0 configuration */
	timer_initpara.prescaler = 8399;
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = 29999;
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	//(8399+1)*(29999+1) / 168M = 1.5s
	timer_init(TIMER0, &timer_initpara);

	/* CH0 configuration in PWM mode0 */
	timer_ocintpara.ocpolarity = TIMER_OC_POLARITY_HIGH;
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
	timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocintpara);

	timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 3999);
	timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
	timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

	/* TIMER0 primary output enable */
	timer_primary_output_config(TIMER0, ENABLE);
	/* auto-reload preload enable */
	timer_auto_reload_shadow_enable(TIMER0);

	/* enable TIMER0 */
	//timer_enable(TIMER0);
}

void Adc_Stop()
{
	PFL(DL_ADC, "Adc Stop.");
	timer_disable(TIMER0);
}

void Adc_Start()
{
	PFL(DL_ADC, "Adc Start.\n");
	/* enable TIMER0 */
	//timer_enable(TIMER0);
}

void Adc_Run()
{
	static uint32 ticks = 0;
	if (SwTimer_isTimerOutEx(ticks, 2000))
	{
		ticks = GET_TICKS();
		//DUMP_BYTE(g_AdcRawValue, sizeof(g_AdcRawValue));
		Adc_CheckValue();
	}
}

void Adc_init()
{
	Obj_Register(&g_AdcObj, "Adc", Adc_Start, Adc_Stop, Adc_Run);
	
	//统计ADC PORT
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Adc); i++)
	{
		if (ADC0 == g_Adc[i].adcPort)
		{
			adc0Count++;
		}
		else if (ADC1 == g_Adc[i].adcPort)
		{
			adc1Count++;
		}
	}

	/* system clocks configuration */
	rcu_config();
	/* configure systick */
	systick_config();
	/* GPIO configuration */
	gpio_config();
	/* TIMER configuration */
	timer_config();
	/* DMA configuration */
	dma_config();
	/* ADC configuration */
	adc_config();

	g_pNtc = Ntc_Create(NTC_JAS103J410JA);
}

