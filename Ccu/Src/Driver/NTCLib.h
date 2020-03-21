#ifndef HEADER_NTC_LIB
#define HEADER_NTC_LIB

#define TEM_HIGHEST 0x7ffe	//�¶����ֵ
#define TEM_LOWEST  0x7ffd	//�¶���Сֵ


typedef struct
{
	unsigned char type;
	//�¶ȴ����������������ֵ���飬��ֵ���ֵ����ǰ����Сֵ���ں�
    const unsigned long *tab;
    unsigned short tablen;//�����ݳ���-1
	int lowestTemp;	//�����ֵ,tab[0]��Ӧ���¶�ֵ
}Ntc;

/*******************************************************************************
**�������ܣ����ݹ�����ֵ�������¶�ֵ
**	����1�� pNtc��Ntc����
**	����2�� resistance����������ֵ
**	����ֵ���¶ȡ�
*******************************************************************************/
int Ntc_CalcTemp(Ntc* pNtc, unsigned long rValue);

/*******************************************************************************
**�������ܣ�����ADC�Ĳ���ֵ�������������ֵ
**	����1�� refResistance���ο���ֵ��
**	����2�� adcVal��adcԭʼ����ֵ��
**	����3�� adcAccuracy��adc�������ȡ�
**	����ֵ����������ֵ��
*******************************************************************************/
unsigned long Ntc_VoltageToResistance(unsigned long refResistance, unsigned long adcVal, unsigned short adcAccuracy);

#endif


