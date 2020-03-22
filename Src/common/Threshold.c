
#include "Common.h"
#include "Threshold.h"

//获取阈值区
THRESHOLD_AREA Threshold_GetArea(int value, int thresholdHigh, int thresholdLow)
{
	if (value < thresholdLow) return LASS_THEN_THRESHOLD_AREA;
	else if (value > thresholdHigh) return MORE_THEN_THRESHOLD_AREA;
	else return IN_THRESHOLD_AREA;
}

//获取事件
THRESHOLD_EVENT Threshold_GetEvent(int oldVal, int newVal, int thresholdHigh, int thresholdLow, THRESHOLD_AREA* pArea)
{
	struct
	{
		THRESHOLD_AREA oldValArea;	//旧值区域
		THRESHOLD_AREA newValArea;	//新值区域
		THRESHOLD_EVENT event;		//触发事件
	}
	const static valChangedEvent[] =
	{
		{LASS_THEN_THRESHOLD_AREA, LASS_THEN_THRESHOLD_AREA, NOT_OVER_THRESHOLD},
		{LASS_THEN_THRESHOLD_AREA, IN_THRESHOLD_AREA	   , UP_OVER_THRESHOLD_LOW},
		{LASS_THEN_THRESHOLD_AREA, MORE_THEN_THRESHOLD_AREA, UP_OVER_THRESHOLD_HIGH},

		{IN_THRESHOLD_AREA, LASS_THEN_THRESHOLD_AREA, DOWN_OVER_THRESHOLD_LOW},
		{IN_THRESHOLD_AREA, IN_THRESHOLD_AREA	    , NOT_OVER_THRESHOLD},
		{IN_THRESHOLD_AREA, MORE_THEN_THRESHOLD_AREA, UP_OVER_THRESHOLD_HIGH},

		{MORE_THEN_THRESHOLD_AREA, LASS_THEN_THRESHOLD_AREA , DOWN_OVER_THRESHOLD_LOW},
		{MORE_THEN_THRESHOLD_AREA, IN_THRESHOLD_AREA	    , DOWN_OVER_THRESHOLD_HIGH},
		{MORE_THEN_THRESHOLD_AREA, MORE_THEN_THRESHOLD_AREA , NOT_OVER_THRESHOLD},
	};

	if (thresholdHigh == THRESHOLD_INNVALID)
	{
		if (thresholdLow == THRESHOLD_INNVALID)
		{
			return NOT_OVER_THRESHOLD;
		}
		else
		{
			thresholdHigh = thresholdLow;
		}
	}
	else
	{
		if (thresholdLow == THRESHOLD_INNVALID)
		{
			thresholdLow = thresholdHigh;
		}
	}

	THRESHOLD_AREA oldValueArea = Threshold_GetArea(oldVal, thresholdHigh, thresholdLow);
	THRESHOLD_AREA newValueArea = Threshold_GetArea(newVal, thresholdHigh, thresholdLow);
	for (int i = 0; i < GET_ELEMENT_COUNT(valChangedEvent); i++)
	{
		if (valChangedEvent[i].oldValArea == oldValueArea && valChangedEvent[i].newValArea == newValueArea)
		{
			*pArea = newValueArea;
			return valChangedEvent[i].event;
		}
	}
	
	//程序下不可能运行到这里。
	Printf("Threahold high=%d,low=%d,oldVal=%d,newVal=%d\n", thresholdHigh, thresholdLow, oldValueArea, newValueArea);
	Assert(False);
	return NOT_OVER_THRESHOLD;

}
