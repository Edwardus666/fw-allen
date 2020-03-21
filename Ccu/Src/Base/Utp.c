
#include "common.h"
#include "CheckSum.h"
#include "Utp.h"

#define UTP_PRINTF Printf

/*
把Payload数据加上帧头和帧为，并且转码。
参数说明：
	pSrc:指向Payload数据，包含检验和，不包括帧头帧尾
	srcLen：Src数据长度。
	srcInd: 处理位置
	pDst: 目标数据Buff
	dstLen：目标Buff数据长度
*/
Bool Utp_FramePkt(const uint8* pSrc, uint16 srcLen, int* srcInd, uint8* pDst, int* dstSize)
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
uint16 Utp_SendFrame(Utp* pUtp, const void* pData, uint16 len)
{
	#define BUF_SIZE 64
	int i = 0;
	uint8 byte[BUF_SIZE];
	int j = 0;

	while(i < len)
	{
		j = sizeof(byte);
		Utp_FramePkt((uint8*)pData, len, &i, byte, &j);
		pUtp->TxFn(byte, j);
	}
	
	return len;
}

Bool Utp_isBusy(Utp* pUtp)
{
	return (pUtp->m_state != RS_INIT);
}

void Utp_ResetReq(Utp* pUtp)
{
	pUtp->m_state = RS_INIT;
	pUtp->m_ReqLen = 0;
	pUtp->m_reTxCount = 0;
	SwTimer_Stop(&pUtp->m_Timer);
}

void Utp_ResetRsp(Utp* pUtp)
{
	pUtp->m_RspLen = 0;
}

void Utp_Reset(Utp* pUtp)
{
	Utp_ResetRsp(pUtp);
	Utp_ResetReq(pUtp);
}

Bool Utp_VerifyFrame(const UtpFrame* pFrame, int frameLen)
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
	dstCheckSum = CheckSum_Get(&dstCheckSum, &pFrame, pFrame->len + UTP_HEAD_LEN);
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

static Bool Utp_RspProc(Utp* pUtp, const UtpFrame* pRsp, RSP_RC rspCode)
{
	ASRT(pUtp->RspProc);
	pUtp->m_state = RS_INIT;
	if (pRsp)
	{
		pUtp->RspProc(pUtp, pUtp->m_Req[0], rspCode, pRsp->data, pRsp->len);
	}
	else
	{
		pUtp->RspProc(pUtp, pUtp->m_Req[0], rspCode, Null, 0);
	}

	if(RS_INIT == pUtp->m_state)
	{
		Utp_ResetReq(pUtp);
		Utp_ResetRsp(pUtp);
	}
	return True;
}

static void Utp_ReqProc(Utp* pUtp, const UtpFrame* pReq)
{
	uint8 stateTemp = pUtp->m_state;
	OP_CODE rc = OP_NO_RSP;
	uint16 checkSum = 0;
	
	UtpFrame* pRsp = (UtpFrame*)pUtp->m_Rsp;

	pRsp->len = 0;
	
	pUtp->m_state = RS_RX_REQ;	//pUtp置忙标志，防止上层应用在函数ReqProc内部发起新的请求。
	rc = pUtp->ReqProc(pUtp, pReq->cmd, pReq->data, pReq->len, &pRsp->data[1], &pRsp->len);
	pUtp->m_state = stateTemp;
	
	pRsp->cmd = pReq->cmd;
	pRsp->data[0] = rc;	
	pRsp->len += 1;
	
	if(OP_NO_RSP != rc)
	{
		checkSum = 0;
		checkSum = CheckSum_Get(&checkSum, &pRsp, pRsp->len + UTP_HEAD_LEN);
		pRsp->data[pRsp->len] = (uint8)checkSum;
		pRsp->data[pRsp->len + 1] = (uint8)(checkSum >> 8);
		Utp_SendFrame(pUtp, &pRsp->cmd, pRsp->len + UTP_MIN_LEN);
	}
}

//网络字节转换为
Bool Utp_ConvertToHost(uint8* dst, uint16 dstSize, uint16* dstInd, FRAME_STATE *state, uint8 byte)
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
void Utp_RcvFrameHandler(Utp* pUtp, const UtpFrame* pFrame)
{
	//判断响应命令码是否匹配
	if (RS_WAIT_RSP == pUtp->m_state && pUtp->m_Req[0] == pFrame->cmd)
	{
		//pUtp->m_RspLen = pFrame->len + UTP_HEAD_LEN;
		//memcpy(pUtp->m_Rsp, pFrame, pUtp->m_RspLen);

		Utp_RspProc(pUtp, pFrame, RSP_SUCCESS);	//响应处理
	}
	else
	{
		Utp_ReqProc(pUtp, pFrame);	//请求处理
	}

	pUtp->m_FrameState = FRAME_INIT;
	return;
}

int Utp_SendReqSync(Utp* pUtp, uint8 cmd, const void* pData, int len, uint32 waitMs, uint8 maxReTxCount, uint8* pRsp)
{
	if (pUtp->m_state != RS_INIT)
	{
		//Printf("Utp is busy.\n");
		return 0;
	}

	Utp_SendReq(pUtp, cmd, pData, len, waitMs, maxReTxCount);
	return 0;
}

Bool Utp_SendReq(Utp* pUtp, uint8 cmd, const void* pData, int len, uint32 waitMs, uint8 maxReTxCount)
{
	uint16 checkSum = 0;
	UtpFrame* pReq = (UtpFrame*)pUtp->m_Req;

	if(pUtp->m_state != RS_INIT) 
	{
		//Printf("Utp is busy.\n");
		return False;
	}
	if(len > UTP_REQ_BUF_SIZE - 2)
	{
		return False;
	}

	Utp_ResetReq(pUtp);
	Utp_ResetRsp(pUtp);
	
	ASRT(len <= UTP_REQ_BUF_SIZE - 2);


	pReq->addr = pUtp->m_Addr;
	pReq->cmd = cmd;
	pReq->len = len;
	memcpy(pReq->data, pData, len);
	
 	checkSum = CheckSum_Get(&checkSum, pReq, pReq->len + UTP_HEAD_LEN);
	pReq->data[pReq->len] 	= (uint8)checkSum;
	pReq->data[pReq->len+1] = (uint8)(checkSum>>8);

	pUtp->m_reTxCount = 1;
	pUtp->m_maxTxCount = maxReTxCount;
	pUtp->m_ReqLen 	   = pReq->len + UTP_MIN_LEN;
	
	Utp_SendFrame(pUtp, pReq, pUtp->m_ReqLen);
	
	if(waitMs)
	{
		pUtp->m_state = RS_WAIT_RSP;
		SwTimer_Start(&pUtp->m_Timer, waitMs, 0);
	}
	else
	{
		Utp_ResetReq(pUtp);
	}

	return True;
}

void Utp_Run(Utp* pUtp)
{	
	if(RS_WAIT_RSP == pUtp->m_state)	//判断等待响应是否超时
	{
		if(SwTimer_isTimerOut(&pUtp->m_Timer))
		{
			if(pUtp->m_reTxCount >= pUtp->m_maxTxCount && pUtp->m_maxTxCount != UTP_ENDLESS)
			{
				Utp_RspProc(pUtp, Null, RSP_TIMEOUT);
			}
			else
			{
				Utp_SendFrame(pUtp, pUtp->m_Req, pUtp->m_ReqLen);
				
				pUtp->m_reTxCount++;
				SwTimer_ReStart(&pUtp->m_Timer);
			}
		}
	}
}

OP_CODE Utp_RcvReq(Utp* pUtp, uint8 cmd, const uint8* pData, uint8 dataLen, uint8* rspData, uint8* pRspLen)
{
	OP_CODE rc = OP_UNSUPPORTED;
	const UtpCmdTbl* pCmd = Null;

	for (pCmd = pUtp->m_UtpCmdTbl; pCmd->ReqProc; pCmd++)
	{
		if (pCmd->cmd == cmd)
		{
			rc = pCmd->ReqProc(pData, dataLen, rspData, pRspLen);
			break;
		}
	}

	return rc;
}

void Utp_RcvRsp(Utp* pUtp, uint8 req, RSP_RC state, const uint8* pRsp, uint8 Len)
{
	const UtpRspItem* pRspItem = Null;

	for (pRspItem = pUtp->m_UtpRspItem; pRspItem->RspProc; pRspItem++)
	{
		if (pRspItem->cmd == req)
		{
			pRspItem->RspProc(state, pRsp, Len);
			break;
		}
	}
}

void Utp_Init(Utp* pUtp, uint8 addr, UtpTxFn txFn, const UtpCmdTbl* pUtpCmdTbl, const UtpRspItem* pUtpRspItem, void* pOwerObj)
{	
	memset(pUtp, 0, sizeof(Utp));

	pUtp->m_Addr = addr;

	pUtp->TxFn  	= txFn;
	pUtp->ReqProc = Utp_RcvReq;
	pUtp->RspProc = Utp_RcvRsp;
	pUtp->m_UtpCmdTbl = pUtpCmdTbl;
	pUtp->m_UtpRspItem = pUtpRspItem;
	pUtp->m_pOwerObj = pOwerObj;

	SwTimer_Init(&pUtp->m_Timer, 0, 0);
}

