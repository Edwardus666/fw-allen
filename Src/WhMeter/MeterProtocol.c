
#include "common.h"
#include "CheckSum.h"
#include "MeterProtocol.h"

#define MP_PRINTF Printf

//每次发送一帧字节
static uint16 MeterProtocol_SendFrame(MeterProtocol* pMeterProtocol, const uint8* pData, uint16 len)
{
	//MeterProtocolFrame* pFrame = (MeterProtocolFrame*)pData;
	return pMeterProtocol->TxFn(pData, len);
}

Bool MeterProtocol_isBusy(MeterProtocol* pMeterProtocol)
{
	return (pMeterProtocol->m_state != RS_INIT);
}

void MeterProtocol_ResetReq(MeterProtocol* pMeterProtocol)
{
	pMeterProtocol->m_state = RS_INIT;
	pMeterProtocol->m_ReqLen = 0;
	pMeterProtocol->m_reTxCount = 0;
	pMeterProtocol->m_pCurrentCmd = Null;
	SwTimer_Stop(&pMeterProtocol->m_WaitForRspTimer);
}

void MeterProtocol_ResetRsp(MeterProtocol* pMeterProtocol)
{
	pMeterProtocol->m_RspLen = 0;
}

void MeterProtocol_Reset(MeterProtocol* pMeterProtocol)
{
	MeterProtocol_ResetRsp(pMeterProtocol);
	MeterProtocol_ResetReq(pMeterProtocol);
}

MP_FRAME_RESULT MeterProtocol_VerifyFrame(MeterProtocolFrame* pRspFrame, int frameLen)
{
	MP_FRAME_RESULT result = MP_FRAME_OK;
	uint16 calcCheckSum = 0;

	if (frameLen < MP_MIN_LEN) //数据没有接受完整
	{
		 return MP_FRAME_INCOMPLETE;
	}

	//检验帧头特征字符
	if (pRspFrame->head != MP_FRAME_HEAD 
		|| pRspFrame->startChar != MP_FRAME_HEAD
		)
	{
		MP_PRINTF("Frame flag error\n");
		result = MP_FRAME_FLAG_ERROR;
		goto Error;
	}

	//Verify len
	if(pRspFrame->len + MP_MIN_LEN > frameLen)
	{
		return MP_FRAME_INCOMPLETE;
	}
	else if (pRspFrame->len + MP_MIN_LEN < frameLen)
	{
		MP_PRINTF("Len[%d,%d] error\n", pRspFrame->len + MP_MIN_LEN, frameLen);
		result = MP_FRAME_LEN_ERROR;
		goto Error;
	}

	//检验帧尾特征字符
	if (pRspFrame->data[pRspFrame->len + 1] != MP_FRAME_TAIL)
	{
		MP_PRINTF("Frame flag error\n");
		result = MP_FRAME_FLAG_ERROR;
		goto Error;
	}
	
	//verify checksum.
	calcCheckSum = CheckSum_Get(&calcCheckSum, pRspFrame, frameLen - 2);
	if(pRspFrame->data[pRspFrame->len] != (uint8)calcCheckSum)
	{
		MP_PRINTF("Crc[0x%02X, 0x%02X] error\n", pRspFrame->data[pRspFrame->len], (uint8)calcCheckSum);
		result = MP_FRAME_CHECKSUM_ERROR;
		goto Error;
	}

	//检验帧方向
	if (pRspFrame->direction != SLAVE_TO_MASTER_RSP)
	{
		MP_PRINTF("MPFrame direction error\n");
		result = MP_FRAME_ERROR;
		goto Error;
	}

	//转换为非BCD码
	for (int i = 0; i < pRspFrame->len; i++)
	{
		pRspFrame->data[i] -= 0x33;	
	}
	return result;
	
Error:
	//DUMP_BYTE_LEVEL(DL_METER, pRspFrame, frameLen);
	return result;
}

static Bool MeterProtocol_RspProc(MeterProtocol* pMeterProtocol, const MeterProtocolFrame* pRsp, UTP_RSP_RC rspCode)
{
	ASRT(pMeterProtocol->RspHandler);
	MeterProtocolFrame* pReqFrame = (MeterProtocolFrame*)pMeterProtocol->m_Req;
	pMeterProtocol->m_state = RS_INIT;
	if (pRsp)
	{
		pMeterProtocol->RspHandler(pMeterProtocol, pReqFrame, rspCode, pRsp);
	}
	else
	{
		pMeterProtocol->RspHandler(pMeterProtocol, pReqFrame, rspCode, Null);
	}

	if(RS_INIT == pMeterProtocol->m_state)
	{
		MeterProtocol_ResetReq(pMeterProtocol);
		MeterProtocol_ResetRsp(pMeterProtocol);
	}
	return True;
}

static Bool MeterProtocol_SendReq(MeterProtocol* pMeterProtocol, uint8 cmd, const uint8* pData, int len, uint32 waitMs, uint8 maxReTxCount)
{
	uint16 checkSum = 0;
	MeterProtocolFrame* pReq = (MeterProtocolFrame*)pMeterProtocol->m_Req;

	if (pMeterProtocol->m_state != RS_INIT)
	{
		//Printf("MeterProtocol is busy.\n");
		return False;
	}
	if (len > MP_REQ_BUF_SIZE - 2)
	{
		return False;
	}

	MeterProtocol_ResetReq(pMeterProtocol);
	MeterProtocol_ResetRsp(pMeterProtocol);

	ASRT(len <= MP_REQ_BUF_SIZE - 2);

	pReq->Ctrl = 0;
	pReq->direction = MASTER_TO_SLAVE_CMD;
	pReq->moreData = 0;
	pReq->cmd = cmd;
	pReq->len = len;
	for (int i = 0; i < len; i++)
	{
		pReq->data[i] = pData[i] + 0x33;
	}
	//memcpy(pReq->data, pData, len);

	checkSum = CheckSum_Get(&checkSum, pReq, pReq->len + MP_HEAD_LEN);
	pReq->data[pReq->len] = (uint8)checkSum;
	pReq->data[pReq->len + 1] = MP_FRAME_TAIL;

	pMeterProtocol->m_reTxCount = 1;
	pMeterProtocol->m_maxTxCount = maxReTxCount;
	pMeterProtocol->m_ReqLen = pReq->len + MP_MIN_LEN;

	MeterProtocol_SendFrame(pMeterProtocol, (uint8*)pReq, pMeterProtocol->m_ReqLen);

	if (waitMs)
	{
		pMeterProtocol->m_state = RS_WAIT_RSP;
		SwTimer_Start(&pMeterProtocol->m_WaitForRspTimer, waitMs, 0);
	}
	else
	{
		MeterProtocol_ResetReq(pMeterProtocol);
	}

	return True;
}

Bool MeterProtocol_SendCmd(MeterProtocol* pMeterProtocol, UtpCmd* p, uint32 waitMs, uint8 maxReTxCount)
{
	if(MeterProtocol_SendReq(pMeterProtocol, p->cmd, p->pCmdData, p->cmdDataLen, waitMs, maxReTxCount))
	{
		pMeterProtocol->m_pCurrentCmd = p;
		return True;
	}
	return False;
}

//接收帧处理
void MeterProtocol_RcvFrameHandler(MeterProtocol* pMeterProtocol, MeterProtocolFrame* pRcvFrame)
{
	//判断响应命令码是否匹配
	MeterProtocolFrame* pReqFrame = (MeterProtocolFrame*)pMeterProtocol->m_Req;
	if (RS_WAIT_RSP == pMeterProtocol->m_state && pReqFrame->cmd == pRcvFrame->cmd)
	{
		MeterProtocol_RspProc(pMeterProtocol, pRcvFrame, RSP_SUCCESS);	//响应处理
	}

	pMeterProtocol->m_FrameState = MP_FRAME_INIT;
	return;
}

int MeterProtocol_SendReqSync(MeterProtocol* pMeterProtocol, uint8 addr, uint8 cmd, const void* pData, int len, uint32 waitMs, uint8 maxReTxCount, uint8* pRsp)
{
	if (pMeterProtocol->m_state != RS_INIT)
	{
		//Printf("MeterProtocol is busy.\n");
		return 0;
	}

	MeterProtocol_SendReq(pMeterProtocol, cmd, pData, len, waitMs, maxReTxCount);
	return 0;
}

void MeterProtocol_RxData(MeterProtocol* pMeterProtocol, uint8* pData, int len)
{

}

void MeterProtocol_Run(MeterProtocol* pMeterProtocol)
{
	if(RS_WAIT_RSP == pMeterProtocol->m_state)	//判断等待响应是否超时
	{
		if(SwTimer_isTimerOut(&pMeterProtocol->m_WaitForRspTimer))
		{
			if(pMeterProtocol->m_reTxCount >= pMeterProtocol->m_maxTxCount && pMeterProtocol->m_maxTxCount != MP_ENDLESS)
			{
				MeterProtocol_RspProc(pMeterProtocol, Null, RSP_TIMEOUT);
			}
			else
			{
				MeterProtocol_SendFrame(pMeterProtocol, pMeterProtocol->m_Req, pMeterProtocol->m_ReqLen);
				
				pMeterProtocol->m_reTxCount++;
				SwTimer_ReStart(&pMeterProtocol->m_WaitForRspTimer);
			}
		}
	}

	//是否有待发请求，如果有，则发送
	if (!MeterProtocol_isBusy(pMeterProtocol) && SwTimer_isTimerOut(&pMeterProtocol->m_PendingReqTimer) && pMeterProtocol->m_PendingReqTimer.m_Id)
	{
		const UtpCmd* p = (UtpCmd*)pMeterProtocol->m_PendingReqTimer.m_Id;
		MeterProtocol_SendReq(pMeterProtocol, p->cmd, p->pCmdData, p->cmdDataLen, 1000, 1);
	}
}

UTP_EVENT_RC MeterProtocol_Event(MeterProtocol* pMeterProtocol, UtpCmd* pUtpCmd, UTP_EVENT ev, UTP_ERROR error)
{
	pUtpCmd->lastError = error;
	if (pUtpCmd->Event)
	{
		if (ev == UTP_TRANSFER_DONE)
		{
			pMeterProtocol->m_state = RS_INIT;
		}
		return pUtpCmd->Event(pMeterProtocol->m_pOwerObj, pUtpCmd, ev);
	}
	else
		return UTP_RC_SUCCESS;
}

//pRsp指向
void MeterProtocol_RspHandler(MeterProtocol* pMeterProtocol, MeterProtocolFrame* pReqFrame, UTP_RSP_RC state, const MeterProtocolFrame* pRsp)
{
	UTP_EVENT_RC rc = UTP_RC_SUCCESS;
	UTP_ERROR latestErr = UTP_ERROR_OK;

	UtpCmd* pUtpCmd = pMeterProtocol->m_pCurrentCmd;

	if (pMeterProtocol->m_pCurrentCmd)
	{
		if (state == RSP_SUCCESS)
		{
			if (!pRsp->slaveResult)
			{
				//检验DI是否匹配
				for (int i = 0; i < 4; i++)
				{
					if(pReqFrame->data[i] !=  pRsp->data[i] + 0x33)
						return;
				}

				//忽略数据中的DI部分
				pUtpCmd->transferData = (uint8*)&pRsp->data[4];
				pUtpCmd->transferLen = pRsp->len - 4;

				if (pUtpCmd->pStorage && pUtpCmd->storageLen)
				{
					int size = MIN(pUtpCmd->storageLen, pUtpCmd->transferLen);
					if (memcmp(pUtpCmd->pStorage, pUtpCmd->transferData, size) != 0)
					{
						rc = MeterProtocol_Event(pMeterProtocol, pUtpCmd, UTP_EVENT_CHANGED_BEFORE, UTP_ERROR_OK);
						if (rc == UTP_RC_SUCCESS)
						{
							memcpy(pUtpCmd->pStorage, pUtpCmd->transferData, size);
						}
						MeterProtocol_Event(pMeterProtocol, pUtpCmd, UTP_EVENT_CHANGED_AFTER, UTP_ERROR_OK);
					}
				}
			}
			else
			{
				pUtpCmd->transferData = (uint8*)& pRsp->data[0];
				pUtpCmd->transferLen = pRsp->len;
				latestErr = UTP_ERROR_RX_RSP_FAILED;
			}

			MeterProtocol_Event(pMeterProtocol, pUtpCmd, UTP_TRANSFER_DONE, latestErr);
		}
		else if (state == RSP_TIMEOUT)
		{
			MeterProtocol_Event(pMeterProtocol, pUtpCmd, UTP_TRANSFER_DONE, UTP_ERROR_TIMEOUT);
		}
	}
	else
	{
		Printf("Not found rsp[%X]\n", pReqFrame->cmd);
	}
}

UtpCmd* MeterProtocol_FindRspItem(const UtpCmd* pRspArray, uint8 cmd)
{
	for (const UtpCmd* pCmd = pRspArray; pCmd->cmd; pCmd++)
	{
		if (pCmd->cmd == cmd)
		{
			return (UtpCmd*)pCmd;
		}
	}

	return Null;
}

void MeterProtocol_Init(MeterProtocol* pMeterProtocol, const uint8* pAddr, UtpTxFn txFn, void* pOwerObj)
{	
	MeterProtocolFrame* pReq = (MeterProtocolFrame*)pMeterProtocol->m_Req;
	memset(pMeterProtocol, 0, sizeof(MeterProtocol));

	//初始化发送帧
	memcpy(pReq->addr, pAddr, MP_ADDR_SIZE);
	pReq->head		= MP_FRAME_HEAD;
	pReq->startChar = MP_FRAME_HEAD;

	pMeterProtocol->TxFn = txFn;
	pMeterProtocol->RspHandler	= (UtpRcvRspFn)MeterProtocol_RspHandler;
	pMeterProtocol->m_pOwerObj	= pOwerObj;

	SwTimer_Init(&pMeterProtocol->m_WaitForRspTimer, 0, 0);
}

