#ifndef __WHMP__H_
#define __WHMP__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "SafeBuf.h"
#include "SwTimer.h"
#include "UtpDef.h"

#define MP_ENDLESS 0xFF

#define MP_RSP_SUCCESS			0x91	//从站应答成功
#define MP_RSP_SUCCESS_NEXT		0xB1	//从站应答成功,有后续数据
#define MP_RSP_FAILED			0xD1	//从站应答异常

#define MP_FRAME_HEAD 		0x68	//0x8C 0x81
#define MP_FRAME_TAIL 		0x16	//0x8C 0x00

#define MASTER_TO_SLAVE_CMD 0	//主站发出的命令帧
#define SLAVE_TO_MASTER_RSP 1	//从站发出的应答帧
#define MP_ADDR_SIZE		6	//电表节点的地址字节数

	//frame state;
typedef enum _MP_FRAME_STATE
{
	 MP_FRAME_INIT = 0
	,MP_FRAME_WAIT_ADDR
	,MP_FRAME_WAIT_START_CH
	,MP_FRAME_WAIT_CTRL
	,MP_FRAME_WAIT_DLC
	,MP_FRAME_WAIT_DATA
	,MP_FRAME_WAIT_CRC
	,MP_FRAME_WAIT_END_CH
	,MP_FRAME_DONE
}MP_FRAME_STATE;

#pragma anon_unions
#pragma pack(1)

typedef struct _MeterProtocolFrame
{
	uint8  head;
	uint8  addr[MP_ADDR_SIZE];
	uint8  startChar;
	union
	{
		struct
		{
			uint8 cmd : 5;				//功能码
			uint8 moreData : 1;			//是否传输结束,0-没有，1-有
			uint8 slaveResult : 1;		//从站应答标志，0=正确， 1=错误
			uint8 direction : 1;		//传送方向，MASTER_TO_SLAVE_CMD=主站发出的命令帧，SLAVE_TO_MASTER_RSP-从站发出的应答帧。
		};
		uint8  Ctrl;
	};
	uint8  len;
	uint8  data[1];
	//uint8 checkSum;
	//uint8 tail
}MeterProtocolFrame;

#pragma pack()

#define MP_HEAD_LEN 	10
#define MP_MIN_LEN 		12	//最小帧长度，包含帧头和帧尾

#define MP_RSP_BUF_SIZE 32
#define MP_REQ_BUF_SIZE 32
typedef struct _MeterProtocol
{
	uint8 m_state:3; 
	uint8 m_Reserved:5; 

	uint8 m_Addr[6];

	uint8 m_FrameState;
	uint16 m_RspLen;
	uint16 m_ReqLen;
	uint8 m_Rsp[MP_RSP_BUF_SIZE];
	uint8 m_Req[MP_REQ_BUF_SIZE];

	uint8 m_reTxCount;	//重发次数
	uint8 m_maxTxCount;	//最大重发次数
	
	UtpTxFn   TxFn;			//发送数据函数，来自初始化函数MeterProtocol_Init
	UtpRcvReqFn	ReqProc;	//发送请求处理函数
	UtpRcvRspFn	RspHandler;	//接收响应处理函数

	UtpCmd* m_pCurrentCmd;		//当前发送并等待应答的命令，Null-表示当前没有发送命令

	SwTimer m_PendingReqTimer;	//待发请求定时器

	SwTimer m_RxDataTimer;
	SwTimer m_WaitForRspTimer;

	void* m_pOwerObj;
}MeterProtocol;

void MeterProtocol_Init(MeterProtocol* pMeterProtocol, const uint8* pAddr, UtpTxFn txFn, void* pOwerObj);
void MeterProtocol_Run(MeterProtocol* pMeterProtocol);
Bool MeterProtocol_isBusy(MeterProtocol* pMeterProtocol);
void MeterProtocol_Reset(MeterProtocol* pMeterProtocol);
void MeterProtocol_RcvFrameHandler(MeterProtocol* pMeterProtocol, MeterProtocolFrame* pFrame);

typedef enum { 
	MP_FRAME_OK,			//帧正确
	MP_FRAME_INCOMPLETE,	//帧不完整,没接收完毕
	MP_FRAME_LEN_ERROR,		//帧长度错误
	MP_FRAME_FLAG_ERROR,	//帧标志错误
	MP_FRAME_CHECKSUM_ERROR,//帧校验错误
	MP_FRAME_ERROR			//其他未定义错误
} MP_FRAME_RESULT;
MP_FRAME_RESULT MeterProtocol_VerifyFrame(MeterProtocolFrame* pRspFrame, int frameLen);

UtpCmd* MeterProtocol_FindRspItem(const UtpCmd* pRspArray, uint8 cmd);

Bool MeterProtocol_SendCmd(MeterProtocol* pMeterProtocol, UtpCmd* p, uint32 waitMs, uint8 maxReTxCount);

#ifdef __cplusplus
}
#endif

#endif


