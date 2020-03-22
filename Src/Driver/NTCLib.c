#include "ntclib.h"
#include "Math.h"
#include "String.h"

//热敏电阻JAS103J410JA的“阻值-温度”对应表
//const static unsigned long NTC_JAS103J410JA_RESTAB[] =
//{
//	/*-40*/ 342803, 323156, 304557, 286966, 270346, 254654, 239851, 225895, 212745, 200361,
//	/*-30*/ 188703, 177734, 167415, 157711, 148588, 140011, 131951, 124375, 117256, 110567,
//	/*-20*/ 104281, 98375,  92824,  87608,  82706,  78097,  73765,  69692,  65862,  62260,
//	/*-10*/ 58872,  55684,  52684,  49860,  47201,  44698,  42339,  40118,  38024,  36050,
//	/*  0*/ 34190,  32435,  30779,  29217,  27742,  26350,  25035,  23792,  22618,  21509,
//	/* 10*/ 20460,  19467,  18529,  17640,  16799,  16003,  15249,  14535,  13857,  13216,
//	/* 20*/ 12607,  12030,  11482,  10962,  10469,  10000,  9555,   9132,   8730,   8348,
//	/* 30*/ 7985,   7640,   7311,   6998,   6700,   6417,   6147,   5890,   5645,   5411,
//	/* 40*/ 5189,   4976,   4774,   4581,   4397,   4221,   4053,   3892,   3739,   3593,
//	/* 50*/ 3453,   3319,   3191,   3069,   2952,   2840,   2733,   2631,   2533,   2439,
//	/* 60*/ 2349,   2263,   2180,   2101,   2025,   1953,   1883,   1816,   1752,   1690,
//	/* 70*/ 1631,   1575,   1520,   1468,   1418,   1370,   1323,   1279,   1236,   1195,
//	/* 80*/ 1155,   1117,   1081,   1046,   1012,   979,    948,    918,    889,    861,
//	/* 90*/ 834,    808,    783,    759,    735,    713,    691,    671,    650,    631,
//	/*100*/ 612,    594,    577,    560,    544,    528,    513,    499,    484,    471,
//	/*110*/ 458,    445,    433,    421,    409,    398,    387,    377,    367,    357,
//	/*120*/ 348,    338,    330,    321,    313,    305,
//};

/*******************************************************************************
**函数名称：longArray_Find
**功    能：在long数组中查出指定数据在数组中的位置,例如longArray_Find([1,3,4],3,2) = 0
**参    数： resistance : 查询的目标数据。
			 tab     : Long数组
			 count   : 数组长度
**返 回 值： 目标值在数组中最接近的位置minInd, 取值范围：0 - (count-2)。
*******************************************************************************/
unsigned short longArray_Find(const unsigned long* tab, int count, long resistance)
{
	int mid;
	int minInd = 0;
	int maxInd = count - 1;

	if (resistance > tab[minInd]) return TEM_LOWEST;
	if (resistance < tab[maxInd]) return TEM_HIGHEST;

	for (; (maxInd - minInd) != 1;)
	{
		mid = (maxInd + minInd) >> 1;//mid是n/2；
		if (resistance < tab[mid])
		{
			minInd = mid;
		}
		else
		{
			maxInd = mid;
		}
	}
	if (resistance == tab[minInd]) return minInd;
	if (resistance == tab[maxInd]) return maxInd;
	return minInd;
}

/*******************************************************************************
**函数功能：根据ADC的采样值，计算光敏电阻值
**	参数1： refResistance，参考阻值。
**	参数2： adcVal，adc原始采样值。
**	参数3： adcAccuracy，adc采样精度。
**	返回值：光敏电阻值。
*******************************************************************************/
unsigned long Ntc_VoltageToResistance(unsigned long refResistance, unsigned long adcVal, unsigned short adcAccuracy)
{
	return refResistance * adcVal / (adcAccuracy - adcVal);
}


/*******************************************************************************
**函数功能：根据光敏阻值，计算温度值
**	参数1： pNtc，Ntc对象。
**	参数2： resistance，光敏电阻值
**	返回值：温度。
*******************************************************************************/
int Ntc_CalcTemp(Ntc * pNtc, unsigned long resistance)
{
	int minInd = longArray_Find(pNtc->tab, pNtc->tablen, resistance);
	int temp = pNtc->lowestTemp + minInd;	//表格精度为1°，如果非1°则不适合使用该公司计算温度
	float per = (pNtc->tab[minInd] - resistance);
	per = per / (pNtc->tab[minInd] - pNtc->tab[minInd + 1]);
	per += temp;
	return round(per);	//四舍五入计算温度，精确到1°
}

/*******************************************************************************
**函数功能：初始话Ntc对象
**	参数1： pNtc，Ntc对象。
**	参数2： tab，光敏电阻值-温度对应表
**	参数3： lowestTemp，最低温度值
**	返回值：温度。
*******************************************************************************/
void Ntc_Init(Ntc* pNtc, const unsigned long* tab, int count, int lowestTemp)
{
	memset(pNtc, 0, sizeof(Ntc));
	pNtc->tab = tab;
	pNtc->tablen = count;
	pNtc->lowestTemp = lowestTemp;
}
