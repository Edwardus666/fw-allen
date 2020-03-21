
#include "Common.h"
#include "UtpPdo.h"


void UtpPdo_Dump(UtpPdo* pUtpPdo)
{
}

Bool UtpPdo_isChange(UtpPdo* pUtpPdo)
{
	return (memcmp(pUtpPdo->pReqData, pUtpPdo->pReqDataOlder, pUtpPdo->reqLen) != 0);
}

void UtpPdo_UpdateOlder(UtpPdo* pUtpPdo, const void* pReq)
{
	memcpy(pUtpPdo->pReqDataOlder, pReq, pUtpPdo->reqLen);
}

void UtpPdo_Init(UtpPdo* pUtpPdo, const uint8* pReqData, uint8* pReqDataCmp, int reqLen, uint8* pRsp, int rspLen)
{
	memset(pUtpPdo, 0, sizeof(UtpPdo));

	pUtpPdo->reqLen = reqLen;
	pUtpPdo->pReqData = pReqData;
	pUtpPdo->pReqDataOlder = pReqDataCmp;

	pUtpPdo->pRsp = pRsp;
	pUtpPdo->rspLen = rspLen;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UtpSdo_Init(UtpSdo* pUtpSdo, uint8 cmd, const uint8* pReqData, int reqLen, uint8* pRsp, int rspLen)
{
	memset(pUtpSdo, 0, sizeof(UtpPdo));

	pUtpSdo->cmd = cmd;

	pUtpSdo->reqLen = reqLen;
	pUtpSdo->pReqData = pReqData;

	pUtpSdo->pRsp = pRsp;
	pUtpSdo->rspLen = rspLen;
}

void UtpSdo_UpdateRsp(UtpPdo* pUtpPdo, void* pRsp, int len)
{
	memcpy(pUtpPdo->pRsp, pRsp, MIN(pUtpPdo->rspLen, len));
}
