#ifndef HEADER_NTC
#define HEADER_NTC

#include "NTCLib.h"

typedef enum
{
	NTC_JAS103J410JA
}NTC_TYPE;

//����NTC����
Ntc* Ntc_Create(NTC_TYPE type);


#endif


