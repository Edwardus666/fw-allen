
#include "common.h"
#include "Tlv.h"

void Tlv_Init(Tlv* pTlv, uint8 tag, uint8 len, const uint8* pData)
{
	pTlv->tag = tag;
	pTlv->len = len;
	memcpy(pTlv->data, pData, len);
}

// 参数说明
//	pTlvBuffer：Tlv Buffer。
//	maxLen：pTlvBuffer的最大长度
//	offset：pTlvBuffer的偏移位置
//	pNewTlv：新的待增加的Tlv
// 返回值：pTlvBuffer的有效长度
int Tlv_Append(uint8* pTlvBuffer, int maxLen, int offset, const Tlv* pNewTlv)
{
	if(offset + pNewTlv->len + TLV_HEAD_SIZE <= maxLen)
	{
		memcpy(&pTlvBuffer[offset], pNewTlv, pNewTlv->len + TLV_HEAD_SIZE);
	}

	return offset + pNewTlv->len + TLV_HEAD_SIZE;
}

Tlv* Tlv_GetByTag(Tlv* pTlv, int len, uint8 tag)
{
	TLV_LOOP_START(pTlv, len);
		if(tag == pTlv->tag) return pTlv;
	TLV_LOOP_END();

	return Null;
}


