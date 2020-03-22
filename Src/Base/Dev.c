#include "Common.h"
#include "Dev.h"
#include "Utp.h"

#define MAX_DEV_COUNT 30
static int g_DevCount = 0;
static Dev* g_pDevs[MAX_DEV_COUNT] = { 0 };

static void DevArray_Add(Dev* pDev)
{
	//����Ƿ��Ѿ�������������
	for (int i = 0; i < MAX_DEV_COUNT; i++)
	{
		if (g_pDevs[i] == pDev) return;
	}

	if (g_DevCount < MAX_DEV_COUNT)
	{
		g_pDevs[g_DevCount++] = pDev;
	}
	else
	{
		PFL_ERROR("Device array is full\n");
		Assert(False);
	}
}

void DevArray_GetByID(uint8 ObjectId)
{
	for (int i = 0; i < g_DevCount; i++)
	{
		if (g_pDevs[i]->ObjectId == ObjectId)
		{
			return g_pDevs[i];
		}
	}
	return Null;
}

///////////////////////////////////////////////////////////////////

void Dev_Dump(Dev* pDev)
{

}

void Dev_Err(Dev* pDev, uint32 errMask, Bool isSet)
{
	if (isSet)
	{
		pDev->errMask |= errMask;
	}
	else
	{
		pDev->errMask &= ~errMask;
	}
}

void Dev_SetState(Dev* pDev, uint8 newState)
{
	pDev->state = newState;
}

void Dev_UtpRcvRsp(Dev* pDev, Utp* pUtp, uint8 req, RSP_RC state, const uint8* pRsp, uint8 Len)
{
//	UtpFrame* pReq = (UtpFrame*)pUtp->m_Req;

	//����/������ϱ�־
	Dev_Err(pDev, DevErrCode_CommErr, state != RSP_SUCCESS);

	if (state == RSP_SUCCESS)
	{
		//������ͱ�־��������Ӧ����
		for (CmdDefTbl* pCmd = pDev->pCmdDefTbl; pCmd->req; pCmd++)
		{
			if (pCmd->req == req && pCmd->pIsSendCmd && *pCmd->pIsSendCmd)
			{
				*pCmd->pIsSendCmd = False;
				//�Ƿ��л�����һ״̬��
				if (pCmd->nextState)
				{
					Dev_SetState(pDev, pCmd->nextState);
				}

				//������ʱ��
				if (pCmd->periodSec)
				{
					pCmd->initTicks = GET_TICKS();
				}
			}
		}
	}

	//Utp_RcvRsp(pUtp, req, state, pRsp, Len);
}

Bool Dev_CheckSend(Dev* pDev)
{
	//����������еĵķ��ͱ�־��pIsSendCmd��
	for (CmdDefTbl* pCmd = pDev->pCmdDefTbl; pCmd->req; pCmd++)
	{
		//���pIsSendCmdָ���Null������ֵΪTrue
		if (pDev->state & pCmd->stateMask
			&& pCmd->pIsSendCmd
			&& *pCmd->pIsSendCmd)
		{
			return Dev_SendCmd(pDev, pCmd->req);
		}
	}
	return False;
}

Bool Dev_SendCmd(Dev* pDev, uint8 req)
{
	for (CmdDefTbl* pCmd = pDev->pCmdDefTbl; pCmd->req; pCmd++)
	{
		if (req == pCmd->req)
		{
			void* p = Null;
			if (pCmd->dataLen <= CMD_DATA_SIZE)
			{
				//�����������С��CMD_DATA_SIZE��ֱ�ӰѲ�����ֵ��pCmd->data�У�
				p = pCmd->data;
			}
			else
			{
				//�����pCmd->data����ָ�룬
				p = (void*)(*((uint32*)pCmd->data));
			}
			return Utp_SendReq(pDev->pUtp, pCmd->req, p, pCmd->dataLen, UTP_WAIT_RSP_TIME_MS, UTP_RETX_REQ_COUNT);
		}
	}

	return False;
}
void Dev_PostCmdEx(Dev* pDev, uint8 req, void* pData, int len)
{
	for (CmdDefTbl* pCmd = (CmdDefTbl*)pDev->pCmdDefTbl; pCmd->req; pCmd++)
	{
		if (req == pCmd->req)
		{
			ASRT(len <= CMD_DATA_SIZE);
			//������������������С��CMD_DATA_SIZE��ֱ�ӰѲ�����ֵ��pCmd->data�У�
			if (len <= CMD_DATA_SIZE)
			{
				memcpy(pCmd->data, pData, len);
			}
			else
			{
				//����������ȴ���CMD_DATA_SIZE��ֱ�ӰѲ���ָ�븳ֵ��pCmd->data�С�
				uint32* pU32 = (uint32*)pCmd->data;
				*pU32 = (uint32)pData;
			}
			pCmd->dataLen = len;
			*pCmd->pIsSendCmd = True;
			break;
		}
	}
}

void Dev_Fsm_Init(Dev* pDev)
{
	Dev_CheckSend(pDev);
}
void Dev_Fsm_Start(Dev* pDev)
{
	Dev_CheckSend(pDev);
}
void Dev_Fsm_Active(Dev* pDev)
{
	if (Dev_CheckSend(pDev))
	{
		return;
	}
}
void Dev_Fsm_Error(Dev* pDev)
{
	if (Dev_CheckSend(pDev))
	{
		return;
	}
}

void Dev_Run(Dev* pDev)
{
	Utp_Run(pDev->pUtp);
	if (Utp_isBusy(pDev->pUtp)) return;

	if (pDev->state == DevState_Init)
	{
		Dev_Fsm_Init(pDev);
	}
	else if (pDev->state == DevState_Start)
	{
		Dev_Fsm_Start(pDev);
	}
	else if (pDev->state == DevState_Active)
	{
		Dev_Fsm_Active(pDev);
	}
	else if (pDev->state == DevState_Error)
	{
		Dev_Fsm_Error(pDev);
	}

	//�ж�������ķ��������Ƿ�ʱ��
	for (CmdDefTbl* pCmd = pDev->pCmdDefTbl; pCmd->req; pCmd++)
	{
		if (pCmd->stateMask & pDev->state 
			&& pCmd->periodSec 
			&& SwTimer_isTimerOutEx(pCmd->initTicks, pCmd->periodSec * 1000))
		{
			*pCmd->pIsSendCmd = True;
			pCmd->initTicks = GET_TICKS();
		}
	}
}

void Dev_Stop(Dev* pDev)
{
}

void Dev_Start(Dev* pDev)
{
}

SM_UTP_OP_CODE Dev_ReqHandler(Dev* pDev, uint8* rspData, uint8* pRspLen)
{
	return SM_UTP_OP_SUCCESS;
}


void Dev_Init(Dev* pDev, Utp* pUtp, CmdDefTbl* pCmdDefTbl)
{
	memset(pDev, 0, sizeof(Dev));

	pDev->state = DevState_Init;
	pDev->pUtp = pUtp;

	//Utp_Init(pDev->pUtp, addr, txFn, Null, Null);
	//pDev->utp.RspProc = Dev_UtpRcvRsp;

	pDev->pCmdDefTbl = pCmdDefTbl;

	//��ʼ�����еķ��ͱ�־ΪTrue
	for (CmdDefTbl* pCmd = pDev->pCmdDefTbl; pCmd->req; pCmd++)
	{
		if (pCmd->pIsSendCmd)
		{
			*pCmd->pIsSendCmd = pCmd->initValue;
		}

		//��DevState_Init������������ΪDevState_Error���������DevState_Error������ķ���Ĭ������
		if (pCmd->stateMask & DevState_Init)
		{
			pCmd->stateMask |= DevState_Error;
			pCmd->periodSec = DEV_STATE_ERR_PERIOD_SEC;
		}
	}

	DevArray_Add(pDev);

	pDev->GetDeviceId = Dev_ReqHandler;
	pDev->SetEventReceiver = Dev_ReqHandler;
}
