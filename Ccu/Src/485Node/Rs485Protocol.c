
#include "common.h"
#include "CheckSum.h"
#include "Rs485Protocol.h"
#include "UtpPdo.h"

#define UTP_PRINTF Printf


UtpCmd* Rs485Protocol_FindCmd(const UtpCmd* pCmdArray, uint8 cmd)
{
	for (const UtpCmd* pUtpCmd = pCmdArray; pUtpCmd->cmd; pUtpCmd++)
	{
		if (pUtpCmd->cmd == cmd)
		{
			return (UtpCmd*)pUtpCmd;
		}
	}

	return Null;
}

/*
把Payload数据加上帧头和帧为，并且转码。
参数说明：
	pSrc:指向Payload数据，包含检验和，不包括帧头帧尾
	srcLen：Src数据长度。
	srcInd: 处理位置
	pDst: 目标数据Buff
	dstLen：目标Buff数据长度
*/
Bool Rs485Protocol_FramePkt(const uint8* pSrc, uint16 srcLen, int* srcInd, uint8* pDst, int* dstSize)
{
	int j = 0;
	int i = 0;
	int bRet = True;
	
	if(*srcInd == 0)
		pDst[j++] = UTP_FRAME_HEAD;
	
	for(i = *srcInd; i < srcLen; i++)
	{		
		switch(pSrc[i])
		{
			case UTP_FRAME_HEAD:
				pDst[j++] = UTP_FRAME_ESCAPE;
				pDst[j++] = UTP_FRAME_ESCAPE_HREAD;
				break;
			case UTP_FRAME_TAIL:
				pDst[j++] = UTP_FRAME_ESCAPE;
				pDst[j++] = UTP_FRAME_ESCAPE_TAIL;
				break;
			case UTP_FRAME_ESCAPE:
				pDst[j++] = UTP_FRAME_ESCAPE;
				pDst[j++] = UTP_FRAME_ESCAPE_ESCAPE;
				break;
			default:	
				pDst[j++] = pSrc[i];
				break;
		}

		//pDst buffer 满
		if(j >= (*dstSize) - 2)
		{
			++i;
			if(i == srcLen)
			{
				break;
			}
			else
			{
				bRet = False;
				goto End;
			}
		}
	}
	
	pDst[j++] = UTP_FRAME_TAIL;
	
End:	
	*srcInd = i;
	*dstSize = j;
	return bRet;
}

//每次发送一帧字节
uint16 Rs485Protocol_SendFrame(Rs485Protocol* pRs485Protocol, const void* pData, uint16 len)
{
	#define BUF_SIZE 64
	int i = 0;
	uint8 byte[BUF_SIZE];
	int j = 0;

	while(i < len)
	{
		j = sizeof(byte);
		Rs485Protocol_FramePkt((uint8*)pData, len, &i, byte, &j);
		pRs485Protocol->TxFn(byte, j);
	}
	
	return len;
}

Bool Rs485Protocol_isBusy(Rs485Protocol* pRs485Protocol)
{
	return (pRs485Protocol->m_state != RS_INIT);
}

void Rs485Protocol_ResetReq(Rs485Protocol* pRs485Protocol)
{
	pRs485Protocol->m_state = RS_INIT;
	pRs485Protocol->m_ReqLen = 0;
	pRs485Protocol->m_reTxCount = 0;
	pRs485Protocol->m_pCurrentCmd = Null;
	SwTimer_Stop(&pRs485Protocol->m_Timer);
}

void Rs485Protocol_ResetRsp(Rs485Protocol* pRs485Protocol)
{
	pRs485Protocol->m_RspLen = 0;
}

void Rs485Protocol_Reset(Rs485Protocol* pRs485Protocol)
{
	Rs485Protocol_ResetRsp(pRs485Protocol);
	Rs485Protocol_ResetReq(pRs485Protocol);
}

Bool Rs485Protocol_VerifyFrame(const Rs485ProtocolFrame* pFrame, int frameLen)
{
	uint16 checkSum = 0;
	uint16 dstCheckSum = 0;

	//Verify len
	if(frameLen < UTP_MIN_LEN || pFrame->len + UTP_MIN_LEN != frameLen)
	{
		UTP_PRINTF("Len[%d,%d] error:", pFrame->len + UTP_MIN_LEN, frameLen);
		goto Error;
	}
	
	//verify checksum.
	checkSum = pFrame->data[pFrame->len] | (pFrame->data[pFrame->len + 1] << 8);
	dstCheckSum = CheckSum_Get(&dstCheckSum, pFrame, pFrame->len + UTP_HEAD_LEN);
	if(checkSum != dstCheckSum)
	{
		UTP_PRINTF("Crc[0x%02x, 0x%02x] error:", checkSum, dstCheckSum);
		goto Error;
	}

	return True;
	
Error:
	DUMP_BYTE(pFrame, frameLen);
	return False;
}

static Bool Rs485Protocol_RcvRspProc(Rs485Protocol* pRs485Protocol, const Rs485ProtocolFrame* pRsp, UTP_RSP_RC rspCode)
{
	Assert(pRs485Protocol->RspHandler);
	pRs485Protocol->m_state = RS_INIT;
	if (pRsp)
	{
		pRs485Protocol->RspHandler(pRs485Protocol, (Rs485ProtocolFrame*)pRs485Protocol->m_Req, rspCode, pRsp);
	}
	else
	{
		pRs485Protocol->RspHandler(pRs485Protocol, (Rs485ProtocolFrame*)pRs485Protocol->m_Req, rspCode, Null);
	}

	if(RS_INIT == pRs485Protocol->m_state)
	{
		Rs485Protocol_ResetReq(pRs485Protocol);
		Rs485Protocol_ResetRsp(pRs485Protocol);
	}
	return True;
}
//网络字节转换为
Bool Rs485Protocol_ConvertToHost(uint8* dst, uint16 dstSize, uint16* dstInd, FRAME_STATE *state, uint8 byte)
{
	Bool bRet = False;
	int i = *dstInd;
	#define FRAME_RESET() {i = 0; *state = FRAME_INIT;}

	
	if(UTP_FRAME_HEAD == byte)
	{
		i = 0;
		*state = FRAME_FOUND_HEAD;
		
		dst[i++] = byte;
	}
	else if(*state == FRAME_FOUND_HEAD)
	{
		if(UTP_FRAME_ESCAPE == byte)
		{
			*state = FRAME_FOUND_ESCAPE;
		}
		else if(UTP_FRAME_TAIL == byte)
		{
			dst[i++] = byte;
			*state = FRAME_DONE;
			bRet = True;
			goto End;
		}
		else
		{
			dst[i++] = byte;
		}
	}
	else if(*state == FRAME_FOUND_ESCAPE)
	{
		switch(byte)
		{
			case UTP_FRAME_ESCAPE_HREAD : byte = UTP_FRAME_HEAD  ; break;
			case UTP_FRAME_ESCAPE_TAIL  : byte = UTP_FRAME_TAIL  ; break;
			case UTP_FRAME_ESCAPE_ESCAPE: byte = UTP_FRAME_ESCAPE; break;
			default:
			{	//Data is invalid, discard.
				//Printf("Frame error, byte = 0x%x\n", byte);
				FRAME_RESET();
				goto End;
			}
		}
		dst[i++] = byte;
		*state = FRAME_FOUND_HEAD;
	}
	
	if(i >= dstSize)
	{
		//Printf("*Buf is full*.\n");
//		ASRT(i < dstSize);
		FRAME_RESET();
	}

End:
	
	* dstInd = i;
	return bRet;
}

//接收帧处理
void Rs485Protocol_RcvFrameHandler(Rs485Protocol* pRs485Protocol, const Rs485ProtocolFrame* pFrame)
{
	Rs485ProtocolFrame* pReqFrame = (Rs485ProtocolFrame*)pRs485Protocol->m_Req;
	//判断响应命令码是否匹配
	if (RS_WAIT_RSP == pRs485Protocol->m_state)
	{
		//检查响应的命令码是否匹配
		if (pReqFrame->cmd == pFrame->cmd
			&& pReqFrame->addr == pFrame->addr
			&& pRs485Protocol->m_pCurrentCmd)
		{
			Rs485Protocol_RcvRspProc(pRs485Protocol, pFrame, RSP_SUCCESS);	//响应处理
			return;
		}
	}

	//Rs485Protocol_RcvReqProc(pRs485Protocol, pFrame);
	return;
}

static Bool Rs485Protocol_SendReq(Rs485Protocol* pRs485Protocol, uint8 cmd, const void* pData, int len, uint32 waitMs, uint8 maxReTxCount)
{
	uint16 checkSum = 0;
	Rs485ProtocolFrame* pReq = (Rs485ProtocolFrame*)pRs485Protocol->m_Req;
	pRs485Protocol->pUtpPdo = Null;

	if (pRs485Protocol->m_state != RS_INIT)
	{
		//Printf("Rs485Protocol is busy.\n");
		return False;
	}
	if (len > UTP_REQ_BUF_SIZE - 2)
	{
		return False;
	}

	Rs485Protocol_ResetReq(pRs485Protocol);
	Rs485Protocol_ResetRsp(pRs485Protocol);

	ASRT(len <= UTP_REQ_BUF_SIZE - 2);


	pReq->addr = pRs485Protocol->m_Addr;
	pReq->cmd = cmd;
	pReq->len = len;
	memcpy(pReq->data, pData, len);

	checkSum = CheckSum_Get(&checkSum, pReq, pReq->len + UTP_HEAD_LEN);
	pReq->data[pReq->len] = (uint8)checkSum;
	pReq->data[pReq->len + 1] = (uint8)(checkSum >> 8);

	pRs485Protocol->m_reTxCount = 1;
	pRs485Protocol->m_maxTxCount = maxReTxCount;
	pRs485Protocol->m_ReqLen = pReq->len + UTP_MIN_LEN;

	Rs485Protocol_SendFrame(pRs485Protocol, pReq, pRs485Protocol->m_ReqLen);

	if (waitMs)
	{
		pRs485Protocol->m_state = RS_WAIT_RSP;
		SwTimer_Start(&pRs485Protocol->m_Timer, waitMs, 0);
	}
	else
	{
		Rs485Protocol_ResetReq(pRs485Protocol);
	}

	return True;
}

Bool Rs485Protocol_SendCmd(Rs485Protocol* pRs485Protocol, UtpCmd* p, uint32 waitMs, uint8 maxReTxCount)
{
	if (Rs485Protocol_SendReq(pRs485Protocol, p->cmd, p->pCmdData, p->cmdDataLen, waitMs, maxReTxCount))
	{
		pRs485Protocol->m_pCurrentCmd = p;
		return True;
	}
	return False;
}
int Rs485Protocol_SendReqSync(Rs485Protocol* pRs485Protocol, uint8 addr, uint8 cmd, const void* pData, int len, uint32 waitMs, uint8 maxReTxCount, uint8* pRsp)
{
	if (pRs485Protocol->m_state != RS_INIT)
	{
		//Printf("Rs485Protocol is busy.\n");
		return 0;
	}

	Rs485Protocol_SendReq(pRs485Protocol, cmd, pData, len, waitMs, maxReTxCount);
	return 0;
}

Bool Rs485Protocol_SendPdo(Rs485Protocol* pRs485Protocol, UtpPdo* pUtpPdo)
{
	Bool bRet = Rs485Protocol_SendReq(pRs485Protocol, pUtpPdo->cmd, pUtpPdo->pReqData, pUtpPdo->reqLen, 1000, 1);
	if (bRet)
	{
		pRs485Protocol->pUtpPdo = pUtpPdo;
	}

	return bRet;
}


//周期发送状态变化的UtpPdo
void Rs485Protocol_TxPdoRun(Rs485Protocol* pRs485Protocol)
{
	if (pRs485Protocol->pUtpPdo && SwTimer_isTimerOutEx(pRs485Protocol->m_TPdoTicks, 500) && !Rs485Protocol_isBusy(pRs485Protocol))
	{
		pRs485Protocol->m_TPdoTicks = GET_TICKS();
		if (UtpPdo_isChange(pRs485Protocol->pUtpPdo) || pRs485Protocol->pUtpPdo->forceSend)
		{
			Rs485Protocol_SendPdo(pRs485Protocol, pRs485Protocol->pUtpPdo);
		}
	}
}


OP_CODE Rs485Protocol_RcvReqProc(Rs485Protocol* pRs485Protocol, const Rs485ProtocolFrame* pRxReqFrame)
{
	uint16 calcCheckSum = 0;
	OP_CODE rc = OP_UNSUPPORTED;
	const RxReqDispatch* pCmd = Null;

	Rs485ProtocolFrame* pFrame = (Rs485ProtocolFrame*)pRs485Protocol->m_Rsp;
	pFrame->data[0] = pRxReqFrame->data[0];
	pFrame->data[1] = pRxReqFrame->data[1];
	pFrame->data[2] = OP_UNSUPPORTED;

	for (pCmd = pRs485Protocol->m_reqDispatch; pCmd->Proc; pCmd++)
	{
		if (pCmd->cmd == pRxReqFrame->cmd)
		{
			rc = pCmd->Proc(pRxReqFrame->data, pRxReqFrame->len, &pFrame->data[3], &pFrame->len);
			pFrame->len += 3;
			pFrame->data[2] = rc;

			if (rc != OP_NO_RSP && pFrame->len)
			{
				calcCheckSum = CheckSum_Get(&calcCheckSum, &pFrame->addr, pFrame->len + 3);

				pFrame->cmd = pRxReqFrame->cmd;
				pFrame->addr = pRxReqFrame->addr;
				pFrame->data[pFrame->len] = (uint8)calcCheckSum;
				pFrame->data[pFrame->len + 1] = (uint8)(calcCheckSum >> 8);
			}
			break;
		}
	}

	if (rc != OP_NO_RSP)
	{
		Rs485Protocol_SendFrame(pRs485Protocol, pRs485Protocol->m_Rsp, pFrame->len + UTP_MIN_LEN);
	}

	return rc;
}

UTP_EVENT_RC Rs485Protocol_Event(Rs485Protocol* pRs485Protocol, UtpCmd* pUtpCmd, UTP_EVENT ev, UTP_ERROR error)
{
	pUtpCmd->lastError = error;
	if (pUtpCmd->Event)
	{
		if (ev == UTP_TRANSFER_DONE)
		{
			pRs485Protocol->m_state = RS_INIT;
		}
		return pUtpCmd->Event(pRs485Protocol->m_pOwerObj, pUtpCmd, ev);
	}
	else
		return UTP_RC_SUCCESS;
}

void Rs485Protocol_RspHandler(Rs485Protocol* pRs485Protocol, Rs485ProtocolFrame* reqFrame, UTP_RSP_RC state, const Rs485ProtocolFrame* pRsp)
{
	UTP_EVENT_RC rc = UTP_RC_SUCCESS;
	UTP_ERROR latestErr = UTP_ERROR_OK;
	UtpCmd* pUtpCmd = pRs485Protocol->m_pCurrentCmd;

	if (pUtpCmd == Null) return;

	if(pRsp)
	{
		if (pUtpCmd)
		{
			//因为协议中没有定义明确的Result字，因此，需要判定是否存在Result字，使用变量resultIndOfRspData判定
			//假设应答的数据，是在命令执行结果码的后面跟随，如果没有应答码，说明数据位于位置0
			int dataInRspInd = (pUtpCmd->resultIndOfRspData == UTP_NO_RESULT_BYTE_IN_RSP) ? 0 : pUtpCmd->resultIndOfRspData + 1;
			pUtpCmd->transferData = (uint8*)&pRsp->data[dataInRspInd];
			pUtpCmd->transferLen = pRsp->len - dataInRspInd;

			uint8 rspResult = pRsp->data[pUtpCmd->resultIndOfRspData];

			if (pUtpCmd->transferData && pUtpCmd->transferLen && pUtpCmd->pStorage)
			{
				if (pUtpCmd->resultIndOfRspData != UTP_NO_RESULT_BYTE_IN_RSP)
				{
					//判定结果是否正确
					if (rspResult != 0)
					{
						Rs485Protocol_Event(pRs485Protocol, pUtpCmd, UTP_EVENT_RX_RSP_FAILED, rspResult);
						Rs485Protocol_Event(pRs485Protocol, pUtpCmd, UTP_TRANSFER_DONE, latestErr);
						return;
					}
				}

				int len = MIN(pUtpCmd->transferLen, pRsp->len);
				if (memcmp(pUtpCmd->pStorage, pUtpCmd->transferData, len) != 0)
				{
					rc = Rs485Protocol_Event(pRs485Protocol, pUtpCmd, UTP_EVENT_CHANGED_BEFORE, UTP_ERROR_OK);
					if (rc == UTP_RC_SUCCESS)
					{
						memcpy(pUtpCmd->pStorage, pUtpCmd->transferData, len);
						Rs485Protocol_Event(pRs485Protocol, pUtpCmd, UTP_EVENT_CHANGED_AFTER, UTP_ERROR_OK);
					}
				}
			}
		}
		else if (pRs485Protocol->pUtpPdo && pRs485Protocol->pUtpPdo->pReqDataOlder)
		{
			if (state == RSP_SUCCESS)
			{
				pRs485Protocol->pUtpPdo->forceSend = False;
				UtpPdo_UpdateOlder(pRs485Protocol->pUtpPdo, pRsp->data);
			}
		}

		Rs485Protocol_Event(pRs485Protocol, pUtpCmd, UTP_TRANSFER_DONE, latestErr);
	}
	else if (state == RSP_TIMEOUT)
	{
		Rs485Protocol_Event(pRs485Protocol, pUtpCmd, UTP_TRANSFER_DONE, UTP_ERROR_TIMEOUT);
	}
	else
	{
		Printf("Not found rsp[%X]\n", pRsp->cmd);
	}
}

void Rs485Protocol_Run(Rs485Protocol* pRs485Protocol)
{
	if (RS_WAIT_RSP == pRs485Protocol->m_state)	//判断等待响应是否超时
	{
		if (SwTimer_isTimerOut(&pRs485Protocol->m_Timer))
		{
			if (pRs485Protocol->m_reTxCount >= pRs485Protocol->m_maxTxCount && pRs485Protocol->m_maxTxCount != UTP_ENDLESS)
			{
				Rs485Protocol_RcvRspProc(pRs485Protocol, Null, RSP_TIMEOUT);
			}
			else
			{
				Rs485Protocol_SendFrame(pRs485Protocol, pRs485Protocol->m_Req, pRs485Protocol->m_ReqLen);

				pRs485Protocol->m_reTxCount++;
				SwTimer_ReStart(&pRs485Protocol->m_Timer);
			}
		}
	}

	Rs485Protocol_TxPdoRun(pRs485Protocol);

	//是否有待发请求，如果有，则发送
	if (!Rs485Protocol_isBusy(pRs485Protocol) && SwTimer_isTimerOut(&pRs485Protocol->m_PendingReqTimer) && pRs485Protocol->m_PendingReqTimer.m_Id)
	{
		const UtpCmd* p = (UtpCmd*)pRs485Protocol->m_PendingReqTimer.m_Id;
		Rs485Protocol_SendReq(pRs485Protocol, p->cmd, p->pCmdData, p->cmdDataLen, 1000, 1);
	}
}
void Rs485Protocol_Init(Rs485Protocol* pRs485Protocol
	, uint8 addr
	, UtpTxFn txFn
	, const RxReqDispatch* pRxReqDispatch
	, const UtpCmd* cmdArray
	, void* pOwerObj)
{	
	memset(pRs485Protocol, 0, sizeof(Rs485Protocol));

	pRs485Protocol->m_Addr		= addr;
	pRs485Protocol->TxFn  		= txFn;
	pRs485Protocol->RspHandler	= (UtpRcvRspFn)Rs485Protocol_RspHandler;
	pRs485Protocol->m_reqDispatch = pRxReqDispatch;
	pRs485Protocol->m_cmdArray = cmdArray;
	pRs485Protocol->m_pOwerObj = pOwerObj;

	SwTimer_Init(&pRs485Protocol->m_Timer, 0, 0);
}

