
#include "Common.h"
#include "AdcUse.h"

void AdcUse_TempChanged(Adc* pAdc, THRESHOLD_EVENT event, int oldVal, int newVal)
{
	PostMsgEx(MSG_TEMP_CHANGED, pAdc->adcId, event);
}

void AdcUse_Init()
{
	//ADC_TEMP_1和ADC_TEMP_2用于风扇开关参考
	Adc_EventRegister(ADC_TEMP_1, g_pCfgInfo->temp_thresholdHigh, g_pCfgInfo->temp_thresholdLow, AdcUse_TempChanged);
	Adc_EventRegister(ADC_TEMP_2, g_pCfgInfo->temp_thresholdHigh, g_pCfgInfo->temp_thresholdLow, AdcUse_TempChanged);

	//ADC_TEMP_3和ADC_TEMP_4用于空调开关参考
	Adc_EventRegister(ADC_TEMP_3, g_pCfgInfo->temp_thresholdHigh, g_pCfgInfo->temp_thresholdLow, AdcUse_TempChanged);
	Adc_EventRegister(ADC_TEMP_4, g_pCfgInfo->temp_thresholdHigh, g_pCfgInfo->temp_thresholdLow, AdcUse_TempChanged);
}
