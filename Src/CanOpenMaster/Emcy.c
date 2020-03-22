#include <common.h>
#include "Emcy.h"

static EmcyPkt g_emcyPkt;

void EmcyCode_Set(EmcyCode* p, uint8 errorBit, uint16 errorCode, uint32 infoCode)
{
	p->errorBit = errorBit;
	p->errorCode = errorCode;
	p->infoCode = infoCode;
}

EmcyPkt* Emcy_Build(uint8 nodeId, uint16 errCode, uint8 errReg)
{
	EmcyPkt* pPkt = &g_emcyPkt;
	memset(pPkt, 0, sizeof(EmcyPkt));

	pPkt->addr = nodeId;
	pPkt->len = 8;
	pPkt->errCode = errCode;
	pPkt->errReg = errReg;
	return pPkt;
}
