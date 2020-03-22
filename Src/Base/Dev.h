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
#define DEV_STATE_ERR_PERIOD_SEC 5	//�豸����STATE_ERR�ķ����������ڣ��룩����̽�豸�Ƿ�ָ�ͨ��

	//typedef void (*DevFn)()
	//����������Ϣ��
	typedef struct _CmdDefTbl
	{
		uint32 stateMask;	//״̬���룬��ʾ��������ʲô״̬�¿��Է��ͣ�����϶��״̬��
		uint8 req;			//������
		uint8 dataLen;		//�����������
		uint8 data[CMD_DATA_SIZE];		//�������ֻ,����������ȴ���CMD_DATA_SIZE���������һ��ָ������������ָ�롣

		uint8* pIsSendCmd;	//�Ƿ���Ҫ��������ı�־
		uint8 initValue;	//* pIsSendCmd�ĳ�ʼֵ

		uint32 nextState;	//����ͳɹ����Ƿ��л�����һ��״ֵ̬��0��ʾ���л�

		uint32 periodSec;   //����ִ��ʱ�䣬0��ʾ���������
		uint32 initTicks;   //��ʱ������ʱ�䡣
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
		DevErrCode_CommErr = BIT_0	//ͨ�Ŵ�����������Ӧ
		, DevErrCode_Rsp_Err	//ͨ����������������Ӧ����Ӧ���ݴ���
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

