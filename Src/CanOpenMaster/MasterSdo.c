#include <common.h>
#include "MasterSdo.h"
#include "SegmentProtocol.h"
#include "MasterNode.h"
#ifdef CFG_BLOCK_TRANS
#include "BlockProtocol.h"
#endif
#include "crc16-ccitt.h"

void MasterSdo_Reset(MasterSdo* pSdo)
{
	SdoTransMgr* pMgr = &pSdo->TransMgr;
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;

	pSdo->txCounter = 0;
	pSdo->sdoState = SDO_IDLE;
	pSdo->type = SDO_TRANS_TYPE_UNKNOWN;

	SwTimer_Stop(&pSdo->timer);

	arg->ODdataStorage = 0;
	arg->dataLength = 0;
	pMgr->toggle = 0;
	arg->offset = 0;
}

void MasterSdo_TimerStart(MasterSdo* pSdo, uint32 timeOutMs)
{
	SwTimer_Start(&pSdo->timer, timeOutMs, 0);
}

SDO_ERR SdoTransMgr_CopyOdDataToBuff(SdoTransMgr* pMgr)
{
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	SDO_ERR ret = CO_SDO_AB_NONE;

	arg->dataLength = pMgr->blockSize * 7;

	/* verify blksize and if SDO data buffer is large enough */
	if ( arg->dataLength > SDO_BUF_SIZE
		|| pMgr->blockSize < 1
		|| pMgr->blockSize > 127
		)
	{
		Printf("Error: dataLength=%d, block size=%x, lastSegment=%d\n", arg->dataLength, pMgr->blockSize, arg->lastSegment);
		return CO_SDO_AB_BLOCK_SIZE;
	}

	if (pMgr->OnEvent)
	{
		ret = pMgr->OnEvent(pMgr->pObj, TRANS_BUF_EMPTY, CO_SDO_AB_NONE, arg);
		if(pMgr->crcEnable)
		{
			//DUMP_BYTE_EX(("offset=%d, crcIn=0x%04X: ", arg->offset, pMgr->crc),arg->data, arg->dataLength);
			pMgr->crc = crc16_ccitt(arg->data, arg->dataLength, pMgr->crc);
			//Printf(" crcOut=0x%04X\n", pMgr->crc);
		}
		arg->offset += arg->dataLength;
		pMgr->offsetOfData = 0;

		//重新计算pMgr->blockSize
		pMgr->blockSize = (arg->dataLength + 6)/ 7;
	}

	//Not domain type
	if (ret == CO_SDO_AB_NONE && arg->ODdataStorage)
	{
		memcpy(arg->data, &arg->ODdataStorage[pMgr->offsetOfData], arg->dataLength);
	}


	return ret;
}

SDO_ERR MasterSdo_TransInit(MasterSdo* pSdo, int index, uint8 subIndex, Bool isDownload, Bool isBlockEnable, SdoPkt* pOutPkt)
{
	SDO_ERR ret = CO_SDO_AB_NONE;
	SdoTransMgr* pMgr = &pSdo->TransMgr;
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pMgr;
	uint16 indOfArray = 0;
	const CO_OD_entry* pEntry = OD_find(pSdo->pOd, pSdo->odSize, index, &indOfArray);
	if(pEntry == Null)
	{
		PFL_WARNING("%s[%02X] SDO[%04X.%02X] not exist.\n", pSdo->pNode->name, pSdo->pNode->addr, index, subIndex);
		return CO_SDO_AB_NOT_EXIST;
	}
	void* pData = Entry_getDataPointer(pEntry, subIndex);
	memset(pOutPkt, 0, sizeof(SdoPkt));
	memset(pMgr, 0, sizeof(SdoTransMgr));

	EntryExt* pEntryExt = &pSdo->pNode->pEntryExtArray[indOfArray];
	if (pEntryExt && pEntryExt->OnEvent)
	{
		pMgr->OnEvent = pEntryExt->OnEvent;
		pMgr->pObj = pSdo->pNode;
	}

	isBlockEnable = (pData == Null);	//domain type
	arg->data = pMgr->data;
	arg->index = index;
	arg->subIndex = subIndex;
	pMgr->toggle = 0;
	arg->reading = !isDownload;
	arg->offset = 0;

	//pSdo->isBlockEnable = isBlockEnable;
	arg->ODdataStorage = Entry_getDataPointer(pEntry, subIndex);
	arg->dataLength = Entry_getLength(pEntry, subIndex);
	arg->attribute = Entry_getAttribute(pEntry, subIndex);

#ifdef CFG_BLOCK_TRANS
	if (isBlockEnable)
	{
		if (isDownload)
		{
			if (pMgr->OnEvent) ret == pMgr->OnEvent(pMgr->pObj, TRANS_GET_TOTAL_LEN, CO_SDO_AB_NONE, arg);
			if (ret == CO_SDO_AB_NONE)
			{
				Block_InitDownloadReq(pMgr, (InitBlockDownloadReqPkt*)pOutPkt);
				pSdo->sdoState = SDO_BLOCK_DOWNLOAD_INIT;
			}
			else
			{
				PFL_WARNING("%s[%02X] SDO[%04X.%02X] not ready.\n", pSdo->pNode->name, pSdo->pNode->addr, arg->index, arg->subIndex);
			}
		}
		else
		{
			Block_InitUploadReq(pMgr, (InitBlockUploadReqPkt*)pOutPkt);
			pSdo->sdoState = SDO_BLOCK_UPLOAD_INIT;
		}
	}
	else
#endif
	{
		if (isDownload)
		{
			if (pMgr->OnEvent) ret == pMgr->OnEvent(pMgr->pObj, TRANS_GET_TOTAL_LEN, CO_SDO_AB_NONE, arg);
			if (ret == CO_SDO_AB_NONE)
			{
				InitSegDownloadReqPkt_Build(pMgr, (InitSegDownloadReqPkt*)pOutPkt);
				pSdo->sdoState = SDO_SEGMENT_DOWNLOAD_INIT;
			}
		}
		else
		{
			InitSegUploadReqPkt_Build((InitSegUploadReqPkt*)pOutPkt, index, subIndex);
			pSdo->sdoState = SDO_SEGMENT_UPLOAD_INIT;
		}
	}
	return CO_SDO_AB_NONE;
}

SDO_ERR MasterSdo_ProcRsp(MasterSdo* pMasterSdo, SdoPkt* pSdoIn, SdoPkt* pSdoOut)
{
	MasterNode* pMasterNode = pMasterSdo->pNode;
	typedef SDO_ERR (*SdoRspHandler) (SdoTransMgr * pMgr, const SdoPkt* pSdoIn, SdoPkt* pSdoOut, SdoState* state);
	SdoTransMgr* pMgr = &pMasterSdo->TransMgr;
	SDO_ERR ret = CO_SDO_AB_GENERAL;
	uint8 cmdType = pSdoIn->cmd >> 5;

	struct
	{
		SdoState state;	//当前SDO状态
		uint8 cmdType;	//接收到的消息类型
		SdoRspHandler Handler;	//处理函数
	}
	const static dispatchs[] =
	{
		 {SDO_SEGMENT_DOWNLOAD_INIT	, SDO_MSG_TYPE_SEGMENT_DOWNLOAD_INIT_RSP , (SdoRspHandler)Segment_ProcInitDownloadRsp}
		,{SDO_SEGMENT_DOWNLOAD		, SDO_MSG_TYPE_SEGMENT_DOWNLOAD_RSP		 , (SdoRspHandler)Segment_ProcDownloadRsp	 }

		,{SDO_SEGMENT_UPLOAD_INIT	, SDO_MSG_TYPE_SEGMENT_UPLOAD_INIT_RSP   , (SdoRspHandler)Segment_ProcInitUploadRsp	 }
		,{SDO_SEGMENT_UPLOAD		, SDO_MSG_TYPE_SEGMENT_UPLOAD_RSP		 , (SdoRspHandler)Segment_ProcUploadRsp		 }

#ifdef CFG_BLOCK_TRANS
		,{SDO_BLOCK_DOWNLOAD_INIT	, SDO_MSG_TYPE_BLOCK_DOWNLOAD_INIT_RSP	 , (SdoRspHandler)Block_InitDownloadRsp	}
		,{SDO_BLOCK_DOWNLOAD		, SDO_MSG_TYPE_BLOCK_DOWNLOAD_RSP		 , (SdoRspHandler)Block_DownloadRsp		}
		,{SDO_BLOCK_DOWNLOAD_END	, SDO_MSG_TYPE_END_BLOCK_DOWNLOAD_RSP	 , (SdoRspHandler)Block_DownloadEndRsp	}

		,{SDO_BLOCK_UPLOAD_INIT	, SDO_MSG_TYPE_BLOCK_UPLOAD_INIT_RSP , (SdoRspHandler)Block_InitUploadRsp	}
		,{SDO_BLOCK_UPLOAD		, SDO_MSG_TYPE_END_BLOCK_UPLOAD_REQ	 , (SdoRspHandler)Block_UploadEndReq	}
#endif
	};

	pMgr->waitForRsp = False;
	SwTimer_Stop(&pMasterSdo->timer);

	int i = 0;
	if (pMasterSdo->sdoState == SDO_BLOCK_UPLOAD && pMgr->seq < pMgr->blockSize)
	{
		ret = Block_UploadReq(pMgr, (BlockDownloadReqPkt*)pSdoIn, (BlockUploadSeqReqPkt*)pSdoOut, &pMasterSdo->sdoState);
		goto Handle;
	}
	
	if (cmdType == SDO_MSG_TYPE_ABORT_SEGMENT_REQ)
	{
		PFL_WARNING("%s[%02X] SDO[%04X.%02X] abort[%08X]\n", pMasterNode->name, pMasterNode->addr, pSdoIn->ind, pSdoIn->subInd, *(uint32*)pSdoIn->data);
		pMasterSdo->sdoState = SDO_FAILED;
		return *(uint32*)pSdoIn->data;
	}

	for (i = 0; i < GET_ELEMENT_COUNT(dispatchs); i++)
	{
		if (pMasterSdo->sdoState == dispatchs[i].state && cmdType == dispatchs[i].cmdType)
		{
			ret = dispatchs[i].Handler(pMgr, pSdoIn, pSdoOut, &pMasterSdo->sdoState);

			Handle:
			//如果返回失败
			if (CO_SDO_AB_NONE != ret)
			{
				PFL_WARNING("%s[%02X] SDO[%04X.%02X] failed=0x%X\n", pMasterNode->name, pMasterNode->addr, pSdoIn->ind, pSdoIn->subInd, ret);
				if (pMgr->isSend)
				{
					pSdoOut->ind = pSdoIn->ind;
					pSdoOut->subInd = pSdoIn->subInd;
					pSdoOut->cmd = (0x04 << 5);		//Abort SDO Transfer
					memcpy(pSdoOut->data, &ret, 4);

					pMasterSdo->sdoState = SDO_DONE;
				}
				else
				{
					goto Error;
				}
			}
			return ret;
		}
	}

Error:
	//没有处理，丢弃
	pMasterSdo->sdoState = SDO_IDLE;
	PFL_WARNING("%s[%02X] SDO[%04X.%02X] discard, error=0x%08X\n", pMasterNode->name, pMasterNode->addr, pSdoIn->ind, pSdoIn->subInd, ret);
	return ret;
}

Bool MasterSdo_IsTimerOut(MasterSdo* pSdo)
{
	return SwTimer_isTimerOut(&pSdo->timer);
	//if (SwTimer_isTimerOut(&pSdo->timer))
	//{
	//	switch (pSdo->sdoState)
	//	{
	//	case SDO_IDLE:
	//		return CO_SDO_AB_NONE;
	//	case SDO_SEGMENT_DOWNLOAD_INIT:
	//	case SDO_SEGMENT_DOWNLOAD:
	//	case SDO_SEGMENT_UPLOAD_INIT:
	//	case SDO_SEGMENT_UPLOAD:
	//	case SDO_BLOCK_DOWNLOAD_INIT:
	//	case SDO_BLOCK_DOWNLOAD:
	//	case SDO_BLOCK_UPLOAD_INIT:
	//	case SDO_BLOCK_UPLOAD:
	//		return CO_SDO_AB_TIMEOUT;
	//	}
	//}
}

void  MasterSdo_Init(MasterSdo* pSdo, MasterNode* pNode)
{
	memset(pSdo, 0, sizeof(MasterSdo));

	pSdo->pOd = pNode->pPrivateOd;
	pSdo->odSize = pNode->privateOdSize;
	pSdo->pNode = pNode;

}
