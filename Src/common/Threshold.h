
#ifndef __THRESHOLD__H_
#define __THRESHOLD__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

#define THRESHOLD_INNVALID (0xFFFFFFFF>>1)

	/*
							MORE_THEN_THRESHOLD AREA（大于阈值区）
	THRESHOLD_HIGH：--------------------------------------------
							THRESHOLD AREA（阈值区内）
	THRESHOLD_LOW： --------------------------------------------
							LASS_THEN_THRESHOLD AREA（小于阈值区）
	*/

	//值变化事件定义
	typedef enum
	{
		NOT_OVER_THRESHOLD,			//没有越过任何阈值
		UP_OVER_THRESHOLD_HIGH,		//向上越过阈值上线
		UP_OVER_THRESHOLD_LOW,		//向上越过阈值下线
		DOWN_OVER_THRESHOLD_HIGH,	//向下越过阈值上线
		DOWN_OVER_THRESHOLD_LOW,	//向下越过阈值下线
	}THRESHOLD_EVENT;

	//阈值区定义，采样值位域那个区域
	typedef enum
	{
		UNKNOWN_AREA,				//未知区域
		MORE_THEN_THRESHOLD_AREA,	//大于阈值区
		IN_THRESHOLD_AREA,			//阈值区内
		LASS_THEN_THRESHOLD_AREA,	//小于阈值区
	}THRESHOLD_AREA;

	THRESHOLD_EVENT Threshold_GetEvent(int oldVal, int newVal, int thresholdHigh, int thresholdLow, THRESHOLD_AREA* pArea);

#ifdef __cplusplus
}
#endif

#endif

