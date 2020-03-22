
#ifndef __REQ_ITEM_H_
#define __REQ_ITEM_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

typedef struct _UtpPdo
{
	//初始化变量*******************
	uint8 cmd;			//PDO的命令码，来自UtpPdo_Init

	int reqLen;
	const uint8* pReqData;
	uint8* pReqDataOlder;

	//运行变量*******************
	uint8 addr;
	Bool forceSend;		//是否强制发送请求
	int rspLen;
	uint8* pRsp;
}UtpPdo;

void UtpPdo_Init(UtpPdo* pUtpPdo, const uint8* pReqData, uint8* pReqDataCmp, int reqLen, uint8* pRsp, int rspLen);
void UtpPdo_Dump(UtpPdo* pUtpPdo);
void UtpPdo_UpdateOlder(UtpPdo* pUtpPdo, const void* pReq);

//////////////////////////////////////////////////////////
typedef struct _UtpSdo
{
	uint8 cmd;			//PDO的命令码，来自UtpPdo_Init

	int reqLen;
	const uint8* pReqData;

	int rspLen;
	uint8* pRsp;
}UtpSdo;
void UtpSdo_Init(UtpSdo* pUtpSdo,uint8 cmd, const uint8* pReqData, int reqLen, uint8* pRsp, int rspLen);
void UtpSdo_UpdateRsp(UtpPdo* pUtpPdo, void* pRsp, int len);
Bool UtpPdo_isChange(UtpPdo* pUtpPdo);

#ifdef __cplusplus
}
#endif

#endif

