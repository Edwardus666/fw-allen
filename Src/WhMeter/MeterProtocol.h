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

#define MP_RSP_SUCCESS			0x91	//��վӦ��ɹ�
#define MP_RSP_SUCCESS_NEXT		0xB1	//��վӦ��ɹ�,�к�������
#define MP_RSP_FAILED			0xD1	//��վӦ���쳣

#define MP_FRAME_HEAD 		0x68	//0x8C 0x81
#define MP_FRAME_TAIL 		0x16	//0x8C 0x00

#define MASTER_TO_SLAVE_CMD 0	//��վ����������֡
#define SLAVE_TO_MASTER_RSP 1	//��վ������Ӧ��֡
#define MP_ADDR_SIZE		6	//���ڵ�ĵ�ַ�ֽ���

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
			uint8 cmd : 5;				//������
			uint8 moreData : 1;			//�Ƿ������,0-û�У�1-��
			uint8 slaveResult : 1;		//��վӦ���־��0=��ȷ�� 1=����
			uint8 direction : 1;		//���ͷ���MASTER_TO_SLAVE_CMD=��վ����������֡��SLAVE_TO_MASTER_RSP-��վ������Ӧ��֡��
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
#define MP_MIN_LEN 		12	//��С֡���ȣ�����֡ͷ��֡β

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

	uint8 m_reTxCount;	//�ط�����
	uint8 m_maxTxCount;	//����ط�����
	
	UtpTxFn   TxFn;			//�������ݺ��������Գ�ʼ������MeterProtocol_Init
	UtpRcvReqFn	ReqProc;	//������������
	UtpRcvRspFn	RspHandler;	//������Ӧ������

	UtpCmd* m_pCurrentCmd;		//��ǰ���Ͳ��ȴ�Ӧ������Null-��ʾ��ǰû�з�������

	SwTimer m_PendingReqTimer;	//��������ʱ��

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
	MP_FRAME_OK,			//֡��ȷ
	MP_FRAME_INCOMPLETE,	//֡������,û�������
	MP_FRAME_LEN_ERROR,		//֡���ȴ���
	MP_FRAME_FLAG_ERROR,	//֡��־����
	MP_FRAME_CHECKSUM_ERROR,//֡У�����
	MP_FRAME_ERROR			//����δ�������
} MP_FRAME_RESULT;
MP_FRAME_RESULT MeterProtocol_VerifyFrame(MeterProtocolFrame* pRspFrame, int frameLen);

UtpCmd* MeterProtocol_FindRspItem(const UtpCmd* pRspArray, uint8 cmd);

Bool MeterProtocol_SendCmd(MeterProtocol* pMeterProtocol, UtpCmd* p, uint32 waitMs, uint8 maxReTxCount);

#ifdef __cplusplus
}
#endif

#endif


