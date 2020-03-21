#ifndef __ADC_DRIVER_H_
#define __ADC_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "Threshold.h"


	typedef enum
	{
		  ADC_18650_CHARGE_I	//PC5, ADC01_IN15 , 18650������������� ADC
		, ADC_18650_VOLTAGE 	//PB0, ADC01_IN8 , 18650��ص�ѹ��� ADC
		, ADC_18650_TEMP	 	//PB1, ADC01_IN9 , 18650����¶� ADC

		, ADC_TEMP_1			//PC0, ADC012_IN10, ���NTC���¶Ȳ�������ADC��
		, ADC_TEMP_2			//PC1, ADC012_IN11, ���NTC���¶Ȳ�������ADC��
		, ADC_TEMP_3			//PC2, ADC012_IN12, ���NTC���¶Ȳ�������ADC��
		, ADC_TEMP_4			//PC3, ADC012_IN13, ���NTC���¶Ȳ�������ADC��

		, ADC_TEMP_MCU			//ADC0_CH16, �ڲ��¶ȴ�����
		, ADC_VERF_MCU			//ADC0_CH17, �ڲ��ο���ѹ VREFINT
	}ADC_ID;

	struct _Adc;
	typedef void (*AdcChangedFn)(struct _Adc* pAdc, THRESHOLD_EVENT event, int oldVal, int newVal);
	typedef struct _Adc
	{
		//���������������в������޸�////////////////////////////////////
		ADC_ID adcId;	//�ɼ���
		const char* name;	//����
		uint32 ioPort;		//Gpio Port
		uint16 ioPin;		//Gpio Pin
		uint32_t adcPort;	//Adc port��  ADCx,x=0,1,2
		uint8 adcChnl;		//Adcͨ��	, ADC_CHANNEL_x(x=0..17)(x=16 and x=17 are only for ADC0)
		uint16* pRawValue;	//ָ��Rawֵ��ָ��
		float refVoltage;	//�ο���ѹ

		//ָ�������ֵ������ָ�룬���Գ�ʼ�����ã���������һ
		uint8* odValue8;	//���OD������������Uint8ʱ ��ʹ�ô�ָ��
		uint16* odValue16;	//���OD������������Uint16ʱ��ʹ�ô�ָ��

		//���������������п����޸�////////////////////////////////////
		int newValue;	//��ת��ֵ
		int rawValue;	//����ԭʼֵ
		int oldValue;	//��ת��ֵ

		//OnChanged�¼���ֵ, ��convertVal�����ı�ʱ����Ҫ�͸�ֵ���Ƚϣ��ж��Ƿ񴥷�OnChanged�¼�
		// ��ֵ�����Խ��lowThreshold �������¼�UP_OVER_THRESHOLD_LOW�� 
		// ��ֵ�����Խ��thresholdHigh�������¼�UP_OVER_THRESHOLD_HIGH�� 
		// ��ֵ��С��Խ��lowThreshold �������¼�DOWN_OVER_THRESHOLD_LOW�� 
		// ��ֵ��С��Խ��thresholdHigh�������¼�DOWN_OVER_THRESHOLD_HIGH�� 
		int thresholdHigh;	//��ֵ���ߣ���Чֵ��THRESHOLD_INNVALID
		int thresholdLow;	//��ֵ����, ��Чֵ��THRESHOLD_INNVALID
		THRESHOLD_AREA thresholdArea;	//����ֵconvertValλ���Ǹ���ֵ����
		AdcChangedFn OnChanged;	//ֵ�ı��¼�
	}Adc;

	//typedef struct
	//{
	//	AdcCfg adc0Cfg;
	//	AdcCfg adc1Cfg;

	//	uint32 port;
	//}Adc;

	void Adc_init();
	void Adc_Stop();
	void Adc_Start();
	void Adc_AllStateChanged();
	void Adc_EventRegister(ADC_ID field, int thresholdHigh, int thresholdLow, AdcChangedFn changed);

#ifdef __cplusplus
}
#endif

#endif

