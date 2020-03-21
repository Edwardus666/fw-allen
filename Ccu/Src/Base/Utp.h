#ifndef __UTP__H_
#define __UTP__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "SafeBuf.h"
#include "SwTimer.h"

#include "UtpDef.h"

#define GET_CHECKSUM_POINTER(_pFrame) (uint16*)&(((uint8*)_pFrame)[ ((uint8*)_pFrame)[3] + 4 ])

#define UTP_ENDLESS 0xFF

//frame state;
typedef enum _FRAME_STATE
{
	FRAME_INIT = 0
	,FRAME_FOUND_HEAD
	,FRAME_FOUND_ESCAPE
	,FRAME_DONE
}FRAME_STATE;

typedef enum _RSP_RC
{
	 RSP_SUCCESS = 0
	,RSP_TIMEOUT
	,RSP_FAILED
}RSP_RC;

typedef OP_CODE(*UtpReqProcFn)(const uint8* pData, uint8 dataLen, uint8* rspData, uint8* pRspLen);
typedef struct _UtpCmdTbl
{
	uint8 cmd;
	UtpReqProcFn ReqProc;
}UtpCmdTbl;
typedef void (*UtpRspFn)(RSP_RC state, const void* pRsp, uint8 Len);
typedef struct _UtpRspItem
{
	uint8 cmd;
	UtpRspFn RspProc;

	uint8* pValue;
	int len;
}UtpRspItem;

struct _Utp;
typedef int (*UtpTxFn)(const uint8* pData, int len);
typedef OP_CODE (*UtpRcvReqFn)(struct _Utp* pUtp, uint8 cmd, const uint8* pData, uint8 dataLen, uint8* rspData, uint8* pRspLen);
typedef void (*UtpRcvRspFn)(struct _Utp* pUtp, uint8 req, RSP_RC state, const uint8* pRsp, uint8 Len);

typedef struct _UtpFrame
{
//	uint8  head;
	uint8  addr;
	uint8  cmd;
	uint8  len;
	uint8  data[1];
	//uint16 checkSum;
	//uint8 tail
}UtpFrame;
#define UTP_HEAD_LEN 	3
#define UTP_MIN_LEN 	5	//不包含帧头和帧尾

typedef struct _Utp
{
	uint8 m_state:3; 
	uint8 m_Reserved:5; 

	uint8 m_Addr;

	uint8 m_FrameState;
	uint16 m_RspLen;
	uint16 m_ReqLen;
	uint8 m_Rsp[UTP_RSP_BUF_SIZE];
	uint8 m_Req[UTP_REQ_BUF_SIZE];

	uint8 m_reTxCount;	//重发次数
	uint8 m_maxTxCount;	//最大重发次数
	
	UtpTxFn   TxFn;
	UtpRcvReqFn	ReqProc;
	UtpRcvRspFn	RspProc;
	//必须以一个Null元素结尾
	const UtpCmdTbl* m_UtpCmdTbl;
	const UtpRspItem* m_UtpRspItem;

	SwTimer m_Timer;

	void* m_pOwerObj;
}Utp;

void Utp_Init(Utp* pUtp, uint8 addr, UtpTxFn txFn, const UtpCmdTbl* pUtpCmdTbl, const UtpRspItem* pUtpRspItem, void* pOwerObj);
void Utp_Run(Utp* pUtp);
Bool Utp_isBusy(Utp* pUtp);
Bool Utp_SendReq(Utp* pUtp, uint8 cmd, const void* pData, int len, uint32 waitMs, uint8 maxReTxCount);
void Utp_Reset(Utp* pUtp);
Bool Utp_ConvertToHost(uint8* dst, uint16 dstSize, uint16* dstInd, FRAME_STATE *state, uint8 byte);
Bool Utp_FramePkt(const uint8* pSrc, uint16 srcLen, int* srcInd, uint8* pDst, int* dstLen);
uint16 Utp_SendFrame(Utp* pUtp, const void* pData, uint16 len);
void Utp_RcvRsp(Utp* pUtp, uint8 req, RSP_RC state, const uint8* pRsp, uint8 Len);
OP_CODE Utp_RcvReq(Utp* pUtp, uint8 cmd, const uint8* pData, uint8 dataLen, uint8* rspData, uint8* pRspLen);
void Utp_RcvFrameHandler(Utp* pUtp, const UtpFrame* pFrame);
Bool Utp_VerifyFrame(const UtpFrame* pFrame, int frameLen);

#ifdef __cplusplus
}
#endif

#endif


