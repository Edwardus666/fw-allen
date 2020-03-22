#ifndef __TLV_OBJ_H_
#define __TLV_OBJ_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "Tlv.h"

	typedef struct _TlvSeed
	{
		uint8 tag;
		uint8 len;
		uint8 data[4];
	}TlvSeed;

	typedef struct _TlvPair
	{
		int bufSize;
		int tlvTotalLen;

		uint8* pLocalBuf;
		uint8* pSentBuf;

		TlvSeed* pTlvSeed;
	}TlvPair;

	int TlvPair_GetChangedTlv(TlvPair* pTlvPair, uint8* pBuf, int dataLength);
	void TlvPair_UpdateSentTlv(TlvPair* pTlvPair, const Tlv* pTlv, int len);
	void TlvPair_UpdateLocalTlv(TlvPair* pTlvPair, uint8 tag, const void* pData);
	void TlvPair_Init(TlvPair* pTlvPair, uint8* pSrcTlvBuf, uint8* pDstTlvBuf, int tlvBufSize, const TlvSeed* pTlvSeedArray);
	void TlvPair_ResetSentTlv(TlvPair* pTlvPair);

#ifdef __cplusplus
}
#endif

#endif
