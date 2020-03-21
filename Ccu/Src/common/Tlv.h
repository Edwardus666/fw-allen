#ifndef __TLV__H_
#define __TLV__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

#define TLV_HEAD_SIZE 2

//遍历连续的TLV Buffer
#define TLV_LOOP_START(pTlv, _totalLen)	\
	{	\
		const uint8* _pByte = (uint8*)pTlv;	\
		for (int i = 0; i < _totalLen; i += pTlv->len + TLV_HEAD_SIZE)	\
		{	\
			pTlv = (Tlv*)& _pByte[i];

#define TLV_LOOP_END()	}}

typedef struct _Tlv
{
	uint8 tag;
	uint8 len;
	uint8 data[1];
}Tlv;

void Tlv_Init(Tlv* pTlv, uint8 tag, uint8 len, const uint8* pData);
Tlv* Tlv_GetByTag(Tlv* pTlv, int len, uint8 tag);
int Tlv_Append(uint8* pTlvBuffer, int maxLen, int offset, const Tlv* pNewTlv);
const Tlv* Tlv_Next(const Tlv* pTlv, int len);

#ifdef __cplusplus
}
#endif

#endif

