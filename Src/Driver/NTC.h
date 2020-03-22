#ifndef HEADER_NTC
#define HEADER_NTC

#include "NTCLib.h"

typedef enum
{
	NTC_JAS103J410JA
}NTC_TYPE;

//创建NTC对象
Ntc* Ntc_Create(NTC_TYPE type);


#endif


