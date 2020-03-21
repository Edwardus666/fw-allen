
#include "Common.h"
#include "UtpDef.h"
#include "Rs485ProtocolNode.h"
#include "sbwMgr.h"

void Rs485ProtocolNode_SwitchState(Rs485ProtocolNode* pNode, Rs485ProtocolNodeState newState)
{
	const static char* stateName[] = {"NODE_INIT", "PRE_OPERATION", "PRE_OPERATION", "NODE_STOP"};
	if (pNode->state == newState)
	{
		return;
	}
	PFL(DL_MAIN, "%s state switch to [%s]\n", pNode->name, stateName[newState]);
	pNode->state = newState;
	LOG_TRACE1(OBJID_SHELF_LOCK, ET_SHELF_LOCK_OP_STATE_CHANGED, newState);

	SwTimer_Stop(&pNode->sendCmdTimer);

	//切换到PRE_OPERATION状态，清楚所有待发送的命令
	if (newState == UTP_NODE_PRE_OPERATION)
	{
		for (UtpCmd* pUtpCmd = pNode->utpCmdArray; pUtpCmd->cmd && pUtpCmd->isForceSend; pUtpCmd++)
		{
			pUtpCmd->isForceSend = False;
		}
	}
}

void Rs485ProtocolNode_TransferDone(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd)
{
	pNode->transferUtpCmd = Null;
	if (pUtpCmd->lastError == UTP_ERROR_TIMEOUT)
	{
		Rs485ProtocolNode_SwitchState(pNode, UTP_NODE_PRE_OPERATION);
	}
	else if (pUtpCmd->lastError == UTP_ERROR_OK)
	{
		Rs485ProtocolNode_SwitchState(pNode, UTP_NODE_OPERATION);
	}
}

void Rs485ProtocolNode_SendCmdEx(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd)
{
	pNode->transferUtpCmd = pUtpCmd;
	SbwMgr_Send(pUtpCmd);
}

void Rs485ProtocolNode_SendCmd(Rs485ProtocolNode* pNode, uint8 cmd)
{
	UtpCmd* p = Rs485Protocol_FindCmd(pNode->utpCmdArray, cmd);
	if (p)
	{
		Rs485ProtocolNode_SendCmdEx(pNode, p);
	}
	else
	{
		PFL_ERROR("BT Lock no cmd[%X]\n", cmd);
	}
}

UTP_EVENT_RC Rs485ProtocolNode_RspProc_Common(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd, UTP_EVENT ev)
{
	if (ev == UTP_TRANSFER_DONE)
	{
		Rs485ProtocolNode_TransferDone(pNode, pUtpCmd);
		if (pUtpCmd->lastError == UTP_ERROR_OK)
		{
			pUtpCmd->isForceSend = False;
		}
		Rs485ProtocolNode_Run_WhenProtocolIdle(pNode);

		if (pUtpCmd->storageLen)
		{
			DUMP_BYTE_EX_LEVEL(DL_SHELF_LOCK, ("%s:", pUtpCmd->cmdName), (uint8*)pUtpCmd->pStorage, pUtpCmd->storageLen);
		}
		else
		{
			PFL(DL_SHELF_LOCK, "%s [%s]", pUtpCmd->cmdName, pUtpCmd->lastError ? "Success" : "Failed");
		}
	}

	return UTP_RC_SUCCESS;
}

UTP_EVENT_RC Rs485ProtocolNode_RspProc_ReadVersion(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd, UTP_EVENT ev)
{
	struct rspPkt
	{
		uint8 ver[9];
		uint8 sn[14];
	} *p = (struct rspPkt*)pUtpCmd->transferData;

	if (ev == UTP_EVENT_CHANGED_AFTER)
	{
		memcpy(&pNode->ver, &p->ver, sizeof(p->ver));
		memcpy(&pNode->sn, &p->sn, sizeof(p->sn));
		Rs485ProtocolNode_SwitchState(pNode, UTP_NODE_OPERATION);
	}
	else if (ev == UTP_TRANSFER_DONE)
	{
		Rs485ProtocolNode_TransferDone(pNode, pUtpCmd);
	}
	return UTP_RC_SUCCESS;
}
void Rs485ProtocolNode_Stop(Rs485ProtocolNode* pNode)
{
	pNode->state = UTP_NODE_STOP;
}

//Rs485ProtocolNode 启动
void Rs485ProtocolNode_Start(Rs485ProtocolNode* pNode)
{
	if (pNode->state == UTP_NODE_PRE_OPERATION) return;

	pNode->state = UTP_NODE_PRE_OPERATION;
}

void Rs485ProtocolNode_Run_WhenProtocolIdle(Rs485ProtocolNode* pNode)
{
	SwTimer* pTimer = &pNode->sendCmdTimer;
	uint8 state = pNode->state;

	if (state == UTP_NODE_PRE_OPERATION)
	{
		if (!pTimer->m_isStart)
		{
			SwTimer_Start(pTimer, 2000, 0);
		}
		else if (SwTimer_isTimerOut(pTimer))
		{
			Rs485ProtocolNode_SendCmdEx(pNode, pNode->utpCmd_ReadVersion);
			return;
		}
	}
	else if (state == UTP_NODE_OPERATION)
	{
		for (UtpCmd* pUtpCmd = pNode->utpCmdArray; pUtpCmd->cmd && pUtpCmd->isForceSend; pUtpCmd++)
		{
			Rs485ProtocolNode_SendCmdEx(pNode, pUtpCmd);
			return;
		}

		//启动心跳帧
		if (!pTimer->m_isStart)
		{
			SwTimer_Start(pTimer, 2000, 0);
		}
		else if (SwTimer_isTimerOut(pTimer))
		{
			Rs485ProtocolNode_SendCmdEx(pNode, pNode->utpCmd_Heatbeat);
			return;
		}
	}
}

void Rs485ProtocolNode_Run(Rs485ProtocolNode* pNode, Bool isProtocolBusy)
{
	if (!isProtocolBusy)
	{
		Rs485ProtocolNode_Run_WhenProtocolIdle(pNode);
	}
}

int Rs485ProtocolNode_IsBusy(Rs485ProtocolNode* pNode)
{
	//使用transferUtpCmd是否有效来判定是否忙
	return pNode->transferUtpCmd != Null;
}

void Rs485ProtocolNode_Init(Rs485ProtocolNode* pNode
	, uint8 addr
	, const char* name
	, UtpCmd* utpCmdArray
	, UtpCmd* utpCmdReadVersion
	, UtpCmd* utpCmdHb
	, Rs485ProtocolNodeFun run
	, Rs485ProtocolNodeIsFun IsBusyFn)
{
	static uint8 verSdoData[2];
	verSdoData[0] = 0x00;
	verSdoData[1] = addr;
	memset(pNode, 0, sizeof(Rs485ProtocolNode));

	pNode->verSdo.cmd = 0x20;
	pNode->verSdo.reqLen = sizeof(verSdoData);
	pNode->verSdo.pReqData = verSdoData;

	pNode->verSdo.rspLen = sizeof(HwFwVer);
	pNode->verSdo.pRsp = (uint8*)&pNode->ver;

	pNode->utpCmdArray = utpCmdArray;

	pNode->addr = addr;
	pNode->name = name;
	pNode->state = UTP_NODE_PRE_OPERATION;

	pNode->utpCmd_ReadVersion = utpCmdReadVersion;
	pNode->utpCmd_Heatbeat = utpCmdHb;

	pNode->Run = (run == Null) ? (Rs485ProtocolNodeFun)Rs485ProtocolNode_Run : run;
	pNode->IsBusy = (IsBusyFn == Null) ? (Rs485ProtocolNodeIsFun)Rs485ProtocolNode_IsBusy : IsBusyFn;
}
