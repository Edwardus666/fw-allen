
#include "CheckSum.h"

uint16 CheckSum_Get(uint16* pCheckSum, const void* pData, uint8 len)
{
	const uint8* pByte = (uint8*)pData;
	uint16 i = 0;

	for(i=0; i<len; i++)
	{
		*pCheckSum += pByte[i];
	}
	
	return *pCheckSum;
}

