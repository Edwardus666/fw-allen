#ifndef __CHECK_SUM__H_
#define __CHECK_SUM__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

uint16 CheckSum_Get(uint16* pCheckSum, const void* pData, uint8 len);

#ifdef __cplusplus
}
#endif

#endif


