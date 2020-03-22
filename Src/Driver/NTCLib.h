#ifndef HEADER_NTC_LIB
#define HEADER_NTC_LIB

#define TEM_HIGHEST 0x7ffe	//温度最大值
#define TEM_LOWEST  0x7ffd	//温度最小值


typedef struct
{
	unsigned char type;
	//温度传感器热敏电阻的阻值数组，阻值最大值排再前，最小值排在后
    const unsigned long *tab;
    unsigned short tablen;//表数据长度-1
	int lowestTemp;	//最低温值,tab[0]对应的温度值
}Ntc;

/*******************************************************************************
**函数功能：根据光敏阻值，计算温度值
**	参数1： pNtc，Ntc对象。
**	参数2： resistance，光敏电阻值
**	返回值：温度。
*******************************************************************************/
int Ntc_CalcTemp(Ntc* pNtc, unsigned long rValue);

/*******************************************************************************
**函数功能：根据ADC的采样值，计算光敏电阻值
**	参数1： refResistance，参考阻值。
**	参数2： adcVal，adc原始采样值。
**	参数3： adcAccuracy，adc采样精度。
**	返回值：光敏电阻值。
*******************************************************************************/
unsigned long Ntc_VoltageToResistance(unsigned long refResistance, unsigned long adcVal, unsigned short adcAccuracy);

#endif


