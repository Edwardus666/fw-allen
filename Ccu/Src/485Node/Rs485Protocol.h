#ifndef __UTP__H_
#define __UTP__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "SafeBuf.h"
#include "SwTimer.h"

#include "UtpDef.h"
#include "UtpPdo.h"

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


typedef struct _Rs485ProtocolFrame
{
//	uint8  head;
	uint8  addr;
	uint8  cmd;
	uint8  len;
	uint8  data[1];
	//uint16 checkSum;
	//uint8 tail
}Rs485ProtocolFrame;
#define UTP_HEAD_LEN 	3
#define UTP_MIN_LEN 	5	//不包含帧头和帧尾

typedef struct _Rs485Protocol
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
	
	UtpTxFn     TxFn;		//发送数据函数，来自初始化函数Rs485Protocol_Init
	UtpRcvRspFn	RspHandler;	//接收响应处理函数
	//必须以一个Null元素结尾
	const RxReqDispatch* m_reqDispatch;
	const UtpCmd* m_cmdArray;
	UtpCmd* m_pCurrentCmd;	//当前的执行的命令，Null表示当前没有

	SwTimer m_PendingReqTimer;	//待发请求定时器

	SwTimer m_Timer;

	UtpPdo* pUtpPdo;
	uint32 m_TPdoTicks;

	void* m_pOwerObj;
}Rs485Protocol;

void Rs485Protocol_Init(Rs485Protocol* pRs485Protocol
	, uint8 addr
	, UtpTxFn txFn
	, const RxReqDispatch* pRxReqDispatch
	, const UtpCmd* cmdArray
	, void* pOwerObj);

void Rs485Protocol_Run(Rs485Protocol* pRs485Protocol);
Bool Rs485Protocol_isBusy(Rs485Protocol* pRs485Protocol);
void Rs485Protocol_Reset(Rs485Protocol* pRs485Protocol);
Bool Rs485Protocol_ConvertToHost(uint8* dst, uint16 dstSize, uint16* dstInd, FRAME_STATE *state, uint8 byte);
Bool Rs485Protocol_FramePkt(const uint8* pSrc, uint16 srcLen, int* srcInd, uint8* pDst, int* dstLen);
uint16 Rs485Protocol_SendFrame(Rs485Protocol* pRs485Protocol, const void* pData, uint16 len);
void Rs485Protocol_RspHandler(Rs485Protocol* pRs485Protocol, Rs485ProtocolFrame* reqFrame, UTP_RSP_RC state, const Rs485ProtocolFrame* pRsp);
OP_CODE Rs485Protocol_RcvReqProc(Rs485Protocol* pRs485Protocol, const Rs485ProtocolFrame* pRxReqFrame);
void Rs485Protocol_RcvFrameHandler(Rs485Protocol* pRs485Protocol, const Rs485ProtocolFrame* pFrame);
Bool Rs485Protocol_VerifyFrame(const Rs485ProtocolFrame* pFrame, int frameLen);
UtpCmd* Rs485Protocol_FindCmd(const UtpCmd* pRspArray, uint8 cmd);

Bool Rs485Protocol_SendPdo(Rs485Protocol* pRs485Protocol, UtpPdo* pUtpPdo);
Bool Rs485Protocol_SendCmd(Rs485Protocol* pRs485Protocol, UtpCmd* p, uint32 waitMs, uint8 maxReTxCount);

#ifdef __cplusplus
}
#endif

#endif


