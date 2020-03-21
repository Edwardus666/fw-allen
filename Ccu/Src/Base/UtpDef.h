#ifndef __UTP_DEF_H_
#define __UTP_DEF_H_

#ifdef __cplusplus
extern "C"{
#endif

#define UTP_PROTOCOL_VER	1

#define UTP_NO_RESULT_BYTE_IN_RSP		0xFF	//��Ч����������λ

#define UTP_PURE_DATA_SIZE 128	//UTP�����ݴ�С
#define UTP_REMOTE_REQ_BUF_SIZE (UTP_PURE_DATA_SIZE + 20)	//�����ڹ̼�OTAʱ������������Ϊ128���ֽڡ�
#define UTP_REQ_BUF_SIZE 	UTP_REMOTE_REQ_BUF_SIZE
#define UTP_RSP_BUF_SIZE 	UTP_REMOTE_REQ_BUF_SIZE
#define UTP_SAFEBUF_SIZE 	(UTP_REMOTE_REQ_BUF_SIZE)

#define UTP_FRAME_HEAD 		0x7E	//0x8C 0x81
#define UTP_FRAME_TAIL 		0xFF	//0x8C 0x00

#define UTP_FRAME_ESCAPE 	0x8C	//0x8C 0x73
#define UTP_RSP_CODE     	0x80

#define UTP_FRAME_ESCAPE_HREAD   0x81
#define UTP_FRAME_ESCAPE_TAIL    0x00
#define UTP_FRAME_ESCAPE_ESCAPE  0x73

#define UTP_WAIT_RSP_TIME_MS	1000	//��������ȴ���Ӧʱ��
#define UTP_RETX_REQ_COUNT		3		//�����ط�����

//UTP �����붨��
typedef enum _OP_CODE
{
	 OP_SUCCESS 		= 0		
	,OP_PARAM_INVALID	= 1 //�ò�����Ч
	,OP_UNSUPPORTED		= 2 //�����֧��
	,OP_CRC_ERROR		= 3	//У�����
	,OP_NOT_READY		= 4	//�豸û׼����
	,OP_USERID_ERROR	= 5	//USER_ID����
	,OP_HW_ERROR		= 6	//ִ��ʧ��
	,OP_18650BAT_V_OVER_LOW	= 7	//18650��ѹ̫��
	,OP_NOT_ALLOW_DISCHARGE	= 8	//��֧�ַŵ�
	
	,OP_PENDING			= 0xF0	//ִ�еȴ�
	,OP_NO_RSP			= 0xF1	//No response
	,OP_FAILED			= 0xFF	//ִ��ʧ��
}OP_CODE;

//Latest Error
typedef enum
{
	UTP_ERROR_OK = 0,			//���յ��ɹ�����Ӧ
	UTP_ERROR_RX_RSP_FAILED,	//���յ�ʧ�ܵ���Ӧ
	UTP_ERROR_TIMEOUT,			//�ȴ���Ӧ��ʱ
}UTP_ERROR;

typedef enum _UTP_RSP_RC
{
	RSP_SUCCESS = 0
	, RSP_TIMEOUT
	, RSP_FAILED
}UTP_RSP_RC;

typedef enum
{
	UTP_TRANSFER_DONE,
	UTP_EVENT_CHANGED_BEFORE,
	UTP_EVENT_CHANGED_AFTER,
	UTP_EVENT_RX_RSP_FAILED,

	//UTP_PROC_RSP_BEFORE,
	//UTP_PROC_RSP_AFTER,
	//UTP_PROC_RSP_DONE,
}UTP_EVENT;

typedef enum
{
	UTP_RC_SUCCESS,
	UTP_RC_FAILED,
	UTP_RC_DONE,
}UTP_EVENT_RC;

//UTP ����״̬
typedef enum
{
	RS_INIT = 0		//��ʼ��
	, RS_RX_REQ		//���յ�����
	, RS_WAIT_RSP	//�ȴ���Ӧ
}UTP_TRANSFER_STATE;

struct _UtpCmd;
typedef UTP_EVENT_RC(*UtpEventFn)(void* pOwnerObj, struct _UtpCmd* pRspItem, UTP_EVENT ev);
typedef struct _UtpCmd
{
	uint8 cmd;			//������
	const char* cmdName;	//��������

	uint8* pCmdData;	//��������ָ�룬���Գ�ʼ������
	int cmdDataLen;		//��������ָ�볤�ȣ����Գ�ʼ������

	uint8* pStorage;		//ָ��洢����ָ�룬���Գ�ʼ�������������Null,����յ���Ӧ���Զ�����
	int storageLen;			//ָ��洢����ָ�볤�ȣ����Գ�ʼ������

	UtpEventFn Event;

	uint8 resultIndOfRspData;	//Ӧ��������Ӧ֡���������е�λ��,�����ж������Ƿ�ִ�гɹ�,  ��Чֵ��UTP_NO_RESULT_BYTE_IN_RSP
	//////////////////////////////////////////////////////////////////
	Bool isForceSend;		//�Ƿ���Ҫ����
	uint8* transferData;	//��������ָ��
	uint8 transferLen;		//�������ݳ���

	uint32 nextCmdIntervalMs;	//�Զ������¸������ʱ������0-�������¸����

	UTP_ERROR lastError;			//������
}UtpCmd;

typedef int (*UtpTxFn)(const uint8* pData, int len);
typedef OP_CODE(*UtpRcvReqFn)(void* pObj, uint8 cmd, const uint8* pData, uint8 dataLen, uint8* rspData, uint8* pRspLen);
typedef void (*UtpRcvRspFn)(void* pObj, void* reqFrame, UTP_RSP_RC state, const void* pRspFrame);

typedef OP_CODE(*UtpDispatchFn)(const uint8* pData, uint8 dataLen, uint8* rspData, uint8* pRspLen);
typedef struct _RxReqDispatch
{
	uint8 cmd;
	UtpDispatchFn Proc;
}RxReqDispatch;

#ifdef __cplusplus
}
#endif

#endif


