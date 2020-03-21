#include <common.h>
#include "BlockProtocol.h"
#include "crc16-ccitt.h"

#define BLOCK_NO_CRC 0
#define BLOCK_CRC 1

//cs: client subcommand
#define INIT_UPLOAD_REQ 0
#define START_UPLOAD    3

void Block_InitDownloadReq(SdoTransMgr* pMgr, InitBlockDownloadReqPkt* pReqPkt)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	pReqPkt->ccs = SDO_MSG_TYPE_BLOCK_DOWNLOAD_INIT_REQ;
	pReqPkt->cc = BLOCK_CRC;
	pReqPkt->s = (arg->dataLengthTotal > 0);	//initiate download response

	pReqPkt->index = arg->index;
	pReqPkt->sub   = arg->subIndex;

	pReqPkt->size = arg->dataLength;	//initiate download response
}

SDO_ERR Block_DownloadReqPkt(SdoTransMgr* pMgr, BlockDownloadReqPkt* pReq)
{
	SDO_ERR ret = CO_SDO_AB_NONE;
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	const static uint8 copyLen = 7;
	memset(pReq, 0, sizeof(BlockDownloadReqPkt));
	pReq->seq = ++pMgr->seq;

	if ((pMgr->offsetOfData + copyLen) > arg->dataLength && !arg->lastSegment)
	{
		ret = SdoTransMgr_CopyOdDataToBuff(pMgr);
	}

	memcpy(pReq->data, &arg->data[pMgr->offsetOfData], copyLen);
	pMgr->offsetOfData += copyLen;

	pReq->c = arg->lastSegment && (pMgr->seq == pMgr->blockSize);

	return ret;
}

SDO_ERR Block_InitDownloadRsp(SdoTransMgr* pMgr, InitBlockDownloadRspPkt* pRsp, BlockDownloadReqPkt* pReq, SdoState* state)
{
	SDO_ERR ret = CO_SDO_AB_NONE;
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	pMgr->isSend = True;
	if (arg->index != pRsp->index || arg->subIndex != pRsp->sub)
	{
		return CO_SDO_AB_TYPE_MISMATCH;
	}

	pMgr->crcEnable = pRsp->sc;
	pMgr->seq = 0;
	pMgr->blockSize = pRsp->blockSize;

	ret = SdoTransMgr_CopyOdDataToBuff(pMgr);
	if (ret != CO_SDO_AB_NONE)
	{
		return ret;
	}
	ret = Block_DownloadReqPkt(pMgr, pReq);
	if (ret == CO_SDO_AB_NONE)
	{
		*state = SDO_BLOCK_DOWNLOAD;
	}
	return ret;
}

SDO_ERR Block_DownloadRsp(SdoTransMgr* pMgr, const BlockDownloadRspPkt* pRsp, BlockDownloadReqPkt* pReq, SdoState* state)
{
	SDO_ERR ret = CO_SDO_AB_NONE;
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;

	pMgr->isSend = True;
	if (pRsp->ss != 2)	//Toggle error;
	{
		//命令错误
		return CO_SDO_AB_CMD;
	}
	else if(pMgr->seq != pRsp->seq)
	{
		//seq错误，重传
		int xLen = (pMgr->seq - pRsp->seq) * 7;

		//直接回滚Buff指针
		pMgr->offsetOfData -= xLen;
		pMgr->seq = pRsp->seq;

		Block_DownloadReqPkt(pMgr, pReq);
		return ret;
	}

	if (ret == CO_SDO_AB_NONE)
	{
		if (pMgr->offsetOfData >= arg->dataLength && !arg->lastSegment)
		{
			pMgr->seq = 0;
			pMgr->blockSize = pRsp->blockSize;
			ret = Block_DownloadReqPkt(pMgr, pReq);
		}
		else
		{
			//Transfer done, go next state
			EndBlockDownloadReqPkt* req = (EndBlockDownloadReqPkt*)pReq;
			req->ccs = SDO_MSG_TYPE_END_BLOCK_UPLOAD_REQ;
			req->n = pMgr->offsetOfData - arg->dataLength;
			req->cs = 1;
			req->crc = pMgr->crc;
			*state = SDO_BLOCK_DOWNLOAD_END;
		}
	}
	return ret;
}

SDO_ERR Block_DownloadEndRsp(SdoTransMgr* pMgr, const EndBlockDownloadRspPkt* pRsp, BlockDownloadReqPkt* pReq, SdoState* state)
{
	if (pMgr->OnEvent) pMgr->OnEvent(pMgr->pObj, TRANS_DONE, CO_SDO_AB_NONE, (CO_ODF_arg_t*)pMgr);
	*state = SDO_IDLE;
	return CO_SDO_AB_NONE;
}

/***********************************************************************************************/
void Block_InitUploadReq(SdoTransMgr* pMgr, InitBlockUploadReqPkt* pReq)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	pReq->ccs = SDO_MSG_TYPE_BLOCK_UPLOAD_INIT_REQ;
	pReq->cc = BLOCK_CRC;
	pReq->cs = INIT_UPLOAD_REQ;

	pReq->index = arg->index;
	pReq->sub = arg->subIndex;
	int size = MIN(arg->dataLength, SDO_BUF_SIZE);
	pReq->blockSize = (size / 7) % 127;
	pReq->pst = 0;

	pMgr->blockSize = pReq->blockSize;
	pMgr->waitForRsp = True;
}

SDO_ERR Block_InitUploadRsp(SdoTransMgr* pMgr, const InitBlockUploadRspPkt* pRsp, InitBlockUploadReq2Pkt* pReq, SdoState* state)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	if (arg->index != pRsp->index || arg->subIndex != pRsp->sub)
	{
		return CO_SDO_AB_TYPE_MISMATCH;
	}

	pMgr->seq = 0;
	pMgr->crcEnable = pRsp->sc;
	pMgr->sizeValid = pRsp->s;

	arg->dataLengthTotal = (pMgr->sizeValid) ? pRsp->size : 0 ;

	pReq->ccs = SDO_MSG_TYPE_BLOCK_UPLOAD_REQ;
	pReq->cs = 3;

	*state = SDO_BLOCK_UPLOAD;
	pMgr->isSend = True;
	pMgr->waitForRsp = True;
	return CO_SDO_AB_NONE;
}

SDO_ERR Block_UploadReq(SdoTransMgr* pMgr, const BlockDownloadReqPkt* pReq, BlockUploadSeqReqPkt* pRsp, SdoState* state)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	SDO_ERR ret = CO_SDO_AB_NONE;
	int segLen = 7;

	if (pMgr->seq == pReq->seq)	//可能是重发帧
	{
		return CO_SDO_AB_NONE;
	}
	else if (pMgr->seq + 1 == pReq->seq)
	{
		pMgr->seq++;
	}
	else 
	{
		//Seq 错误，终止传输
		pRsp->ccs = SDO_MSG_TYPE_BLOCK_UPLOAD_RSP;
		pRsp->cs  = 2;
		pRsp->seq = pMgr->seq;

		int len = SDO_BUF_SIZE - pMgr->offsetOfData;
		pRsp->blockSize = (len > (7 * 127)) ? 127 : (len / 7);

		pMgr->isSend = True; 
		pMgr->waitForRsp = True;
		return CO_SDO_AB_NONE;
	}

	if (arg->dataLengthTotal)	//指定长度
	{
		if (arg->offset > arg->dataLengthTotal)
		{
			ret = CO_SDO_AB_OUT_OF_MEM;
			goto Error;
		}
	}

	memcpy(&arg->data[pMgr->offsetOfData], pReq->data, segLen);
	pMgr->offsetOfData += segLen;
	arg->dataLength = pMgr->offsetOfData;
	//最后一个包，可能长度不确定，不计算CRC，在Block_UploadEndReq()函数中计算
	if (!pReq->c)
	{
		pMgr->crc = crc16_ccitt(pReq->data, segLen, pMgr->crc);
	}

	//判断buff是否满
	if (pMgr->seq == pMgr->blockSize && !pReq->c)
	{
		if (pMgr->OnEvent)
		{
			pMgr->OnEvent(pMgr->pObj, TRANS_BUF_FULL, CO_SDO_AB_NONE , arg);
		}
		else
		{
			PFL_ERROR("SDO buffer full.");
		}
		arg->offset += pMgr->offsetOfData;
		pMgr->offsetOfData = 0;
	}


	//段传输结束
	if (pMgr->seq >= pMgr->blockSize || pReq->c)
	{
		int size = 0;
		size = pReq->c ? 0 : SDO_BUF_SIZE;

		size = MIN(SDO_BUF_SIZE, size);
		pRsp->blockSize = (size / 7) % 127;
		pMgr->blockSize = pRsp->blockSize;

		//段传输结束
		pRsp->seq = pMgr->seq;

		pMgr->seq = 0;

		pRsp->ccs = SDO_MSG_TYPE_BLOCK_UPLOAD_RSP;
		pRsp->cs = 2;

		pMgr->isSend = True;
		pMgr->waitForRsp = True;
	}
	return CO_SDO_AB_NONE;

Error:
	if (pMgr->OnEvent)
	{
		arg->dataLength = pMgr->offsetOfData;
		pMgr->OnEvent(pMgr->pObj, TRANS_DONE, ret, arg);
	}
	return ret;
}

SDO_ERR Block_UploadEndReq(SdoTransMgr* pMgr, const EndBlockUploadReqPkt* pReq, EndBlockUploadRspPkt* pRsp, SdoState* state)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	SDO_ERR ret = CO_SDO_AB_NONE;
	pRsp->ccs = SDO_MSG_TYPE_END_BLOCK_UPLOAD_RSP;
	pRsp->cs = 1;

	int remainLen = 7 - pReq->n;
	pMgr->offsetOfData -= pReq->n;

	arg->dataLength = pMgr->offsetOfData;
	pMgr->crc = crc16_ccitt(&arg->data[pMgr->offsetOfData - remainLen], remainLen, pMgr->crc);

	if (arg->dataLengthTotal)
	{
		if (arg->dataLengthTotal != arg->offset + pMgr->offsetOfData)
		{
			pMgr->isSend = True;
			ret = (arg->dataLengthTotal > arg->offset + pMgr->offsetOfData) ? CO_SDO_AB_DATA_SHORT : CO_SDO_AB_DATA_LONG;
			goto Error;
		}
	}

	if (pMgr->crcEnable && pReq->crc != pMgr->crc)
	{
		//Upload failed.
		pMgr->isSend = True;
		ret = CO_SDO_AB_CRC;
		goto Error;
	}

	//Upload success.
	if (pMgr->OnEvent)
	{
		ret = pMgr->OnEvent(pMgr->pObj, TRANS_DONE, CO_SDO_AB_NONE, arg);
		arg->offset += pMgr->offsetOfData;
	}

	*state = SDO_DONE;
	pMgr->isSend = True;
	return CO_SDO_AB_NONE;

Error: 
	if (pMgr->OnEvent)
	{
		ret = pMgr->OnEvent(pMgr->pObj, TRANS_DONE, ret, arg);
	}

	return ret;
}
