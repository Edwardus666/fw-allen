#ifndef __UTP_DEF_H_
#define __UTP_DEF_H_

#ifdef __cplusplus
extern "C"{
#endif

#define UTP_PROTOCOL_VER	1

#define UTP_NO_RESULT_BYTE_IN_RSP		0xFF	//无效的数据索引位

#define UTP_PURE_DATA_SIZE 128	//UTP纯数据大小
#define UTP_REMOTE_REQ_BUF_SIZE (UTP_PURE_DATA_SIZE + 20)	//考虑在固件OTA时，最大包纯数据为128个字节。
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

#define UTP_WAIT_RSP_TIME_MS	1000	//发送请求等待响应时间
#define UTP_RETX_REQ_COUNT		3		//请求重发次数

//UTP 错误码定义
typedef enum _OP_CODE
{
	 OP_SUCCESS 		= 0		
	,OP_PARAM_INVALID	= 1 //该参数无效
	,OP_UNSUPPORTED		= 2 //该命令不支持
	,OP_CRC_ERROR		= 3	//校验错误
	,OP_NOT_READY		= 4	//设备没准备好
	,OP_USERID_ERROR	= 5	//USER_ID错误
	,OP_HW_ERROR		= 6	//执行失败
	,OP_18650BAT_V_OVER_LOW	= 7	//18650电压太低
	,OP_NOT_ALLOW_DISCHARGE	= 8	//不支持放电
	
	,OP_PENDING			= 0xF0	//执行等待
	,OP_NO_RSP			= 0xF1	//No response
	,OP_FAILED			= 0xFF	//执行失败
}OP_CODE;

//Latest Error
typedef enum
{
	UTP_ERROR_OK = 0,			//接收到成功的响应
	UTP_ERROR_RX_RSP_FAILED,	//接收到失败的响应
	UTP_ERROR_TIMEOUT,			//等待响应超时
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

//UTP 传输状态
typedef enum
{
	RS_INIT = 0		//初始化
	, RS_RX_REQ		//接收到请求
	, RS_WAIT_RSP	//等待响应
}UTP_TRANSFER_STATE;

struct _UtpCmd;
typedef UTP_EVENT_RC(*UtpEventFn)(void* pOwnerObj, struct _UtpCmd* pRspItem, UTP_EVENT ev);
typedef struct _UtpCmd
{
	uint8 cmd;			//命令码
	const char* cmdName;	//命令名称

	uint8* pCmdData;	//请求数据指针，来自初始化函数
	int cmdDataLen;		//请求数据指针长度，来自初始化函数

	uint8* pStorage;		//指向存储数据指针，来自初始化函数，如果非Null,则接收到响应会自动更新
	int storageLen;			//指向存储数据指针长度，来自初始化函数

	UtpEventFn Event;

	uint8 resultIndOfRspData;	//应答码在响应帧中数据域中的位置,用来判定命令是否执行成功,  无效值：UTP_NO_RESULT_BYTE_IN_RSP
	//////////////////////////////////////////////////////////////////
	Bool isForceSend;		//是否需要发送
	uint8* transferData;	//传输数据指针
	uint8 transferLen;		//传输数据长度

	uint32 nextCmdIntervalMs;	//自动发送下个命令的时间间隔，0-不发送下个命令。

	UTP_ERROR lastError;			//最后错误
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


