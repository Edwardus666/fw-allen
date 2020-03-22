#include "Common.h"
#include "TlvPair.h"

void TlvPair_UpdateLocalTlv(TlvPair* pTlvPair, uint8 tag, const void* pData)
{
	Tlv* pLocalTlv = Tlv_GetByTag((Tlv*)pTlvPair->pLocalBuf, pTlvPair->tlvTotalLen, tag);
	ASRT(pLocalTlv);
	memcpy(pLocalTlv->data, pData, pLocalTlv->len);
}

void TlvPair_UpdateSentTlv(TlvPair* pTlvPair, const Tlv* pTlv, int len)
{
	TLV_LOOP_START(pTlv, len);
		Tlv* pSentTlv = Tlv_GetByTag((Tlv*)pTlvPair->pSentBuf, pTlvPair->tlvTotalLen, pTlv->tag);
		if (pSentTlv)
		{
			memcpy(pSentTlv->data, pTlv->data, pTlv->len);
		}
	TLV_LOOP_END();
}

void TlvPair_ResetSentTlv(TlvPair* pTlvPair)
{
	Tlv* pLocalTlv, * pSentTlv;
	for (int i = 0; i < pTlvPair->tlvTotalLen; i += pLocalTlv->len + TLV_HEAD_SIZE)
	{
		pLocalTlv = (Tlv*)& pTlvPair->pLocalBuf[i];
		pSentTlv = (Tlv*)& pTlvPair->pSentBuf[i];
		//修改pSentTlvBuf的Data内容,时pSentTlvBuf和pLocalTlvBuf不一致，触发一次改变上传
		pSentTlv->data[0] = pLocalTlv->data[0] + 1;
	}
}

int TlvPair_GetChangedTlv(TlvPair* pTlvPair, uint8* pBuf, int dataLength)
{
	int j = 0;
	TlvSeed* pTlvSeedArray = pTlvPair->pTlvSeed;
	for (pTlvSeedArray = pTlvPair->pTlvSeed; pTlvSeedArray->len; pTlvSeedArray++)
	{
		Tlv* pLocalTlv,* pSentTlv;
		for (int i = 0; i < pTlvPair->tlvTotalLen; i += pLocalTlv->len + TLV_HEAD_SIZE)
		{
			pLocalTlv = (Tlv*)& pTlvPair->pLocalBuf[i];
			pSentTlv = (Tlv*)& pTlvPair->pSentBuf[i];

			if (memcmp(pLocalTlv->data, pSentTlv->data, pSentTlv->len) != 0)
			{
				int tlvTotalLen = pLocalTlv->len + TLV_HEAD_SIZE;
				if (j + tlvTotalLen <= dataLength)
				{
					memcpy(&pBuf[j], pLocalTlv, tlvTotalLen);
					j += tlvTotalLen;
				}
				else
				{
					break;
				}
			}
		}
	}

	return j;
}

void TlvPair_Init(TlvPair* pTlvPair, uint8* pLocalTlvBuf, uint8* pSentTlvBuf, int tlvBufSize, const TlvSeed* pTlvSeedArray)
{
	memset(pTlvPair, 0, sizeof(TlvPair));

	pTlvPair->pTlvSeed = pTlvSeedArray;
	pTlvPair->bufSize = tlvBufSize;
	pTlvPair->pLocalBuf = pLocalTlvBuf;
	pTlvPair->pSentBuf = pSentTlvBuf;

	//使用pTlvSeedArray初始化LocalTlv
	for (; pTlvSeedArray->len; pTlvSeedArray++)
	{
		Assert(pTlvPair->tlvTotalLen + pTlvSeedArray->len <= tlvBufSize);
		pTlvPair->tlvTotalLen = Tlv_Append(pLocalTlvBuf, tlvBufSize, pTlvPair->tlvTotalLen, (Tlv*)pTlvSeedArray);
	}

	//复制pLocalTlvBuf数据到pSentTlvBuf，使他们保持一致。
	memcpy(pSentTlvBuf, pLocalTlvBuf, pTlvPair->tlvTotalLen);

	TlvPair_ResetSentTlv(pTlvPair);
}
