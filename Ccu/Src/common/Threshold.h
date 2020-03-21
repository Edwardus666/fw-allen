
#ifndef __THRESHOLD__H_
#define __THRESHOLD__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

#define THRESHOLD_INNVALID (0xFFFFFFFF>>1)

	/*
							MORE_THEN_THRESHOLD AREA��������ֵ����
	THRESHOLD_HIGH��--------------------------------------------
							THRESHOLD AREA����ֵ���ڣ�
	THRESHOLD_LOW�� --------------------------------------------
							LASS_THEN_THRESHOLD AREA��С����ֵ����
	*/

	//ֵ�仯�¼�����
	typedef enum
	{
		NOT_OVER_THRESHOLD,			//û��Խ���κ���ֵ
		UP_OVER_THRESHOLD_HIGH,		//����Խ����ֵ����
		UP_OVER_THRESHOLD_LOW,		//����Խ����ֵ����
		DOWN_OVER_THRESHOLD_HIGH,	//����Խ����ֵ����
		DOWN_OVER_THRESHOLD_LOW,	//����Խ����ֵ����
	}THRESHOLD_EVENT;

	//��ֵ�����壬����ֵλ���Ǹ�����
	typedef enum
	{
		UNKNOWN_AREA,				//δ֪����
		MORE_THEN_THRESHOLD_AREA,	//������ֵ��
		IN_THRESHOLD_AREA,			//��ֵ����
		LASS_THEN_THRESHOLD_AREA,	//С����ֵ��
	}THRESHOLD_AREA;

	THRESHOLD_EVENT Threshold_GetEvent(int oldVal, int newVal, int thresholdHigh, int thresholdLow, THRESHOLD_AREA* pArea);

#ifdef __cplusplus
}
#endif

#endif

