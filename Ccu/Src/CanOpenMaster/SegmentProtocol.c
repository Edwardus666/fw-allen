#include <common.h>
#include "SegmentProtocol.h"

void SegmentMgr_Reset(SdoTransMgr* pMgr)
{
	memset(pMgr, 0, sizeof(SdoTransMgr));
}

SDO_ERR Segment_TransDone(SdoTransMgr* pMgr)
{
	SDO_ERR ret = CO_SDO_AB_NONE;
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;

	DUMP_BYTE_EX_LEVEL(DL_OD, ("SOD[%04X.%02X] =", arg->index, arg->subIndex), arg->data, arg->dataLength);
	if (pMgr->OnEvent)
	{
		arg->dataLength = pMgr->offsetOfData;
		ret = pMgr->OnEvent(pMgr->pObj, TRANS_DONE, CO_SDO_AB_NONE, arg);
	}
	//数据字典更新成功
	if (CO_SDO_AB_NONE == ret && arg->reading)
	{
		memcpy(arg->ODdataStorage, arg->data, pMgr->offsetOfData);
	}
	return ret;
}

//build init download segment request msg
void InitSegDownloadReqPkt_Build(const SdoTransMgr* pMgr, InitSegDownloadReqPkt* pPktOut)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;

	pPktOut->ccs = SDO_MSG_TYPE_SEGMENT_DOWNLOAD_INIT_REQ;
	pPktOut->s = (arg->dataLength > 0);
	pPktOut->index = arg->index;
	pPktOut->sub = arg->subIndex;
	pPktOut->e = (arg->dataLength <= 4);

	if (arg->dataLength <= 4)
	{
		pPktOut->n = 4 - arg->dataLength;
		//Copy 数据
		memcpy(pPktOut->data, arg->ODdataStorage, arg->dataLength);
	}
	else
	{
		//Copy data length
		memcpy(pPktOut->data, &arg->dataLength, 4);
	}
}

//build download segment request msg
void Sdo_BuildDownloadSegmentReqPkt(SdoTransMgr* pMgr, SegDownloadReqPkt* pPktOut)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	//生成下一个请求帧
	int len = arg->dataLength - arg->offset;
	if (len > 7) len = 7;

	pPktOut->n = 7 - len;
	pPktOut->ccs = SDO_MSG_TYPE_SEGMENT_DOWNLOAD_REQ;
	pPktOut->t = pMgr->toggle;

	if (pMgr->offsetOfData + len > SDO_BUF_SIZE)
	{
		SdoTransMgr_CopyOdDataToBuff(pMgr);
	}

	memcpy(pPktOut->data, &arg->data[pMgr->offsetOfData], len);
	arg->offset += len;
	pMgr->offsetOfData += len;
	pPktOut->c = (arg->offset == arg->dataLength);
}

SDO_ERR Segment_ProcInitDownloadRsp(SdoTransMgr* pMgr, const InitSegDownloadRspPkt* pRspPkt, SegDownloadReqPkt* pPktOut, SdoState* state)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	//帧校验
	if (pRspPkt->index != arg->index
		|| pRspPkt->sub != arg->subIndex)
	{
		return CO_SDO_AB_NOT_EXIST;
	}

	if (arg->offset < arg->dataLength)
	{
		pMgr->toggle = 0;
		Sdo_BuildDownloadSegmentReqPkt(pMgr, pPktOut);
		*state = SDO_SEGMENT_DOWNLOAD;
		pMgr->isSend = True;
		pMgr->waitForRsp = True;
	}
	else
	{
		*state = SDO_IDLE;
	}

	return CO_SDO_AB_NONE;
}

SDO_ERR Segment_ProcDownloadRsp(SdoTransMgr* pMgr, const SegDownloadRspPkt* pRspPkt, SegDownloadReqPkt* pPktOut, SdoState* state)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	if (pRspPkt->t != pMgr->toggle)	//Toggle error;
	{
		return CO_SDO_AB_TOGGLE_BIT;
	}
	if (arg->offset < arg->dataLength)
	{
		pMgr->toggle = !pMgr->toggle;
		Sdo_BuildDownloadSegmentReqPkt(pMgr, pPktOut);
		pMgr->isSend = True;
		pMgr->waitForRsp = True;
	}
	else
	{
		*state = SDO_IDLE;
	}
	return CO_SDO_AB_NONE;
}

/****************************************************************************************************************/
void Segment_CopyData(SdoTransMgr* pMgr, const uint8* pData, int len)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	if (pMgr->offsetOfData + len > SDO_BUF_SIZE)
	{
		if (pMgr->OnEvent)
		{
			pMgr->OnEvent(pMgr->pObj, TRANS_BUF_FULL, CO_SDO_AB_NONE, arg);
		}
		else
		{
			PFL_ERROR("SDO buffer full.");
		}
		pMgr->offsetOfData = 0;
	}

	if (memcpy(&arg->data[pMgr->offsetOfData], pData, len) != 0)
	{
		SDO_ERR err = CO_SDO_AB_NONE;
		if (pMgr->OnEvent) err = pMgr->OnEvent(pMgr->pObj, TRANS_CHANGE_BEFORE, CO_SDO_AB_NONE, arg);
		if (err == CO_SDO_AB_NONE)
		{
			memcpy(&arg->data[pMgr->offsetOfData], pData, len);
			if (pMgr->OnEvent) pMgr->OnEvent(pMgr->pObj, TRANS_CHANGE_AFTER, CO_SDO_AB_NONE, arg);
		}
	}

	pMgr->offsetOfData += len;
	arg->offset += len;
}

void InitSegUploadReqPkt_Build(InitSegUploadReqPkt* pPktOut, uint16 index, uint8 subIndex)
{
	pPktOut->scs = SDO_MSG_TYPE_SEGMENT_UPLOAD_INIT_REQ;

	pPktOut->index = index;
	pPktOut->sub = subIndex;
}

SDO_ERR Segment_ProcInitUploadRsp(SdoTransMgr* pMgr, const InitSegUploadRspPkt* pRspPkt, SegUploadReqPkt* pPktOut, SdoState* state)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	//帧校验
	if (pRspPkt->index != arg->index
		|| pRspPkt->sub != arg->subIndex)
	{
		pMgr->isSend = True;
		return CO_SDO_AB_NOT_EXIST;
	}

	if (pRspPkt->e == SDO_EXPEDITED_TRANS)
	{
		int len = (pRspPkt->s == 1) ? (4 - pRspPkt->n) : 4;
		Segment_CopyData(pMgr, pRspPkt->data, len);
		*state = SDO_IDLE;
		Segment_TransDone(pMgr);
	}
	else
	{
		//处理接收到的信息
		if (pRspPkt->s == 1)
		{
			arg->dataLength = *(uint32*)pRspPkt->data;
		}

		//生成下一个请求帧
		pMgr->toggle = 0;
		pPktOut->t = pMgr->toggle;
		pPktOut->scs = SDO_MSG_TYPE_SEGMENT_UPLOAD_REQ;
		*state = SDO_SEGMENT_UPLOAD;
		pMgr->isSend = True;
		pMgr->waitForRsp = True;
	}
	return CO_SDO_AB_NONE;
}

SDO_ERR Segment_ProcUploadRsp(SdoTransMgr* pMgr, const SegUploadRspPkt* pRspPkt, SegUploadReqPkt* pPktOut, SdoState* state)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;

	//	SDO_ERR ret = CO_SDO_AB_NONE;
	if (pRspPkt->t != pMgr->toggle)	//Toggle error;
	{
		return CO_SDO_AB_TOGGLE_BIT;
	}

	int len = 7 - pRspPkt->n;
	Segment_CopyData(pMgr, pRspPkt->data, len);

	pMgr->toggle = !pMgr->toggle;
	pPktOut->t = pMgr->toggle;

	if (pRspPkt->c)
	{
		//Transfer done, no more segments to be uploaded
		Segment_TransDone(pMgr);
		*state = SDO_IDLE;
		return CO_SDO_AB_NONE;
	}
	else
	{
		if (arg->dataLength == 0 || pMgr->offsetOfData < arg->dataLength)
		{
			//生成下一个请求帧,more segments to be uploaded
			pPktOut->scs = SDO_MSG_TYPE_SEGMENT_UPLOAD_REQ;
			pMgr->isSend = True;
			pMgr->waitForRsp = True;
			return CO_SDO_AB_NONE;
		}
		else
		{
			PFL_WARNING("SDO[%04X.%02X] len error.\n", arg->index, arg->subIndex);
			pMgr->isSend = True;
			return CO_SDO_AB_DATA_LONG;
		}
	}
}
