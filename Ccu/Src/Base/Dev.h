#ifndef __DEVICE_BASE_H_
#define __DEVICE_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"/#include "IdentifyInfo.h"
#include "Utp.h"
#include "SwTimer.h"
#include "CcuDef.h"

#define CMD_DATA_SIZE 4
#define DEV_STATE_ERR_PERIOD_SEC 5	//设备处于STATE_ERR的发送命令周期（秒），试探设备是否恢复通信

	//typedef void (*DevFn)()
	//定义命令信息表
	typedef struct _CmdDefTbl
	{
		uint32 stateMask;	//状态掩码，表示该命令在什么状态下可以发送，可组合多个状态。
		uint8 req;			//请求码
		uint8 dataLen;		//请求参数长度
		uint8 data[CMD_DATA_SIZE];		//请求参数只,如果参数长度大于CMD_DATA_SIZE，则该域是一个指向真正参数的指针。

		uint8* pIsSendCmd;	//是否需要发送请求的标志
		uint8 initValue;	//* pIsSendCmd的初始值

		uint32 nextState;	//命令发送成功后，是否切换到下一个状态值，0表示不切换

		uint32 periodSec;   //周期执行时间，0表示非周期命令。
		uint32 initTicks;   //定时器启动时间。
	}CmdDefTbl;

	typedef enum _DevState
	{
		DevState_Init = BIT_0
		, DevState_Start = BIT_1
		, DevState_Active = BIT_2
		, DevState_Error = BIT_3
	}DevState;

	typedef enum _DevErrMask
	{
		DevErrCode_CommErr = BIT_0	//通信错误，请求无响应
		, DevErrCode_Rsp_Err	//通信正常，请求有响应，响应内容错误
	}DevErrMask;

	typedef SM_UTP_OP_CODE(*ReqHandlerFn)(Dev* pDev, uint8* rspData, uint8* pRspLen);
	typedef struct _Dev
	{
		uint8  ObjectId;
		uint8  state;

		uint32 errMask;
		IdInfo idInfo;

		Utp*   pUtp;
		//SwTimer timer;
		CmdDefTbl* pCmdDefTbl;

		ReqHandlerFn GetDeviceId;
		ReqHandlerFn SetEventReceiver;
	}Dev;

	void Dev_Run(Dev* pDev);
	void Dev_Start(Dev* pDev);
	void Dev_Stop(Dev* pDev);
	void Dev_Init(Dev* pDev, Utp* pUtp, CmdDefTbl* pCmdDefTbl);
	void Dev_Dump(Dev* pDev);
	Bool Dev_SendCmd(Dev* pDev, uint8 req);
	void Dev_PostCmdEx(Dev* pDev, uint8 req, void* pData, int len);
	void Dev_UtpRcvRsp(Dev* pDev, Utp* pUtp, uint8 req, RSP_RC state, const uint8* pRsp, uint8 Len);
	void Dev_SetState(Dev* pDev, uint8 newState);

	Dev* DevArray_GetByID(uint8 ObjectId);

#ifdef __cplusplus
}
#endif

#endif

