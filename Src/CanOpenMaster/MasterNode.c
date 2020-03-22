#include "Common.h"
#include "MasterNode.h"
#include "Frame.h"
#include "SegmentProtocol.h"
#ifdef CFG_BLOCK_TRANS
#include "BlockProtocol.h"
#endif
#include "Pdo.h"
#include "PmsOd.h"

#ifdef CFG_RPDO
#include "Pdo.h"
#endif

//可读并且没有被TPDO映射
#define IS_NEED_SDO_UPLOAD(attr) (attri & CO_ODA_READABLE) && (!(attri & CO_ODA_TPDO_MAPABLE))

void MasterNode_TxDoneCallBack(MasterNode* pMasterNode, CanTxMsg* pCanTxMsg, TxState state)
{

}

void CanTxMsg_Init(CanTxMsg* p, void* pMasterNode, uint8 type, uint8 nodeId)
{
	memset(p, 0, sizeof(CanTxMsg));
	p->state = TX_INIT;

	p->msg.tx_ft = CAN_FT_DATA;
	p->msg.tx_ff = CAN_FF_STANDARD;
	p->msg.tx_sfid = (type << 7) | nodeId;

	//p->callBackParam = pMasterNode;
	//p->TxDone = (CanBusTxDoneFn)MasterNode_TxDoneCallBack;
}

void CanTxMsg_Free(CanTxMsg* p)
{
	p->state = TX_INIT;
}

Bool CanTxMsg_IsFree(const CanTxMsg* p)
{
	return p->state == TX_INIT;
}

void MasterNode_Dump(MasterNode* p)
{
	Printf("\t%s[%d], state=%d, hbTime=%d", p->name, p->addr, p->state, p->HeartbeatTime);
}

void MasterNode_PostFrame(MasterNode* p, uint8 msgId, const Frame* pFrame)
{
	NodeMsg* msg = (NodeMsg*)QUEUE_getNew(&p->msgQueue);
	if (msg == Null)
	{
		PFL_WARNING("%s queue full\n", p->name);
		return;
	}
	msg->msgId = msgId;
	memcpy(&msg->frame, pFrame, pFrame->len + FRAME_HEAD_SIZE);
}
void MasterNode_Post(MasterNode* p, uint8 msgId, uint32 param1, uint32 param2)
{
	NodeMsg* msg = (NodeMsg*)QUEUE_getNew(&p->msgQueue);
	if (msg == Null)
	{
		PFL_WARNING("%s queue full\n", p->name);
		return;
	}
	msg->msgId = msgId;
	msg->param1 = param1;
	msg->param2 = param2;
}

#define CS_COUNT 5
const static uint8 mapItems[CS_COUNT][2] =
{
	SET_NMT_STOPPED			,NMT_STOPPED		,
	SET_NMT_OPERATION		,NMT_OPERATION		,
	SET_NMT_PRE_OPERATION	,NMT_PRE_OPERATION	,
	SET_NMT_RESET			,NMT_INIT	,
	SET_NMT_RESET_COMM		,NMT_INIT	,
};

Bool MasterNode_CsIsValid(uint8 cs)
{
	for (int i = 0; i < CS_COUNT; i++)
	{
		if (mapItems[i][0] == cs) return True;
	}
	return False;
}

NMT_STATE MasterNode_CsToState(uint8 cs)
{
	int i = 0;
	for (i = 0; i < CS_COUNT; i++)
	{
		if (mapItems[i][0] == cs) return (NMT_STATE)mapItems[i][1];
	}

	//程序不应该跑到这里；
	Printf("cs=%x\n", cs);
	Assert(False);
	return NMT_INIT;
}

uint8 MasterNode_GetFrameType(uint32 ident)
{
	//符合标准的帧类型, 高 4 bit.
	return (ident >> 7) & 0xF;
}

//获取下一个SDO读取的数据字典对象
//在数据字典中，没有被TPDO映射的项，都要通过SDO获取值
const CO_OD_entry* MasterNode_GetNextSdo(MasterNode* pMasterNode, uint16* index, uint8* subIndex)
{
	const CO_OD_entry* p = OD_findNext(pMasterNode->pPrivateOd, pMasterNode->privateOdSize, index, subIndex);
	while (p)
	{
		uint32 attri = Entry_getAttribute(p, *subIndex);
		void* pData = Entry_getDataPointer(p, *subIndex);

		if (IS_NEED_SDO_UPLOAD(attri) && pData)
		{
			return p;
		}
		else
		{
			//被映射，获取下一个字典项
			p = OD_findNext(pMasterNode->pPrivateOd, pMasterNode->privateOdSize, index, subIndex);
		}
	}
	return Null;
}

void MasterNode_Fsm_DoNothing(MasterNode* pMasterNode)
{
	Printf("Node[%d] Do Nothing\n", pMasterNode->addr);
}

CO_OD_entry_t* MasterNode_GetOd(MasterNode* pMasterNode, uint16 index, uint8 subInd)
{
	CO_OD_entry* pEntry = Od_GetEntry(pMasterNode->pPrivateOd, pMasterNode->privateOdSize, index, subInd);
	if (pEntry == Null)
	{
		pEntry = Od_GetEntry((CO_OD_entry*)pMasterNode->pGlobalOd, pMasterNode->globalOdSize, index, subInd);
	}

	return pEntry;
}

Bool MasterNode_SetOd(MasterNode* pMasterNode, uint16 index, uint8 subInd, uint8* pData, int len)
{
	const CO_OD_entry* pEntry = OD_find(pMasterNode->pPrivateOd, pMasterNode->privateOdSize, index, Null);
	if (pEntry)
	{
		Entry_SetValue(pEntry, subInd, pData, len);
	}
	return True;
}

Bool MasterNode_SendNmt(MasterNode* pMasterNode, SET_NMT_STATE state)
{
	CanTxMsg* pCanTxMsg = &pMasterNode->nmtCanTxMsg;
	if (!CanTxMsg_IsFree(pCanTxMsg)) return False;	//有一个帧正在发送

	can_trasnmit_message_struct* msg = &pCanTxMsg->msg;

	msg->tx_dlen = 2;
	msg->tx_data[0] = state;
	msg->tx_data[1] = pMasterNode->addr;

	pCanTxMsg->state = TX_PENDING;
	return True;
}

Bool MasterNode_SendSdo(MasterNode* pMasterNode, const SdoPkt* pPkt)
{
	CanTxMsg* pCanTxMsg = &pMasterNode->sdoCanTxMsg;
	if (!CanTxMsg_IsFree(pCanTxMsg)) return False;	//有一个帧正在发送

	can_trasnmit_message_struct* msg = &pCanTxMsg->msg;

	msg->tx_dlen = 8;
	memcpy(msg->tx_data, pPkt, sizeof(SdoPkt));

	pCanTxMsg->state = TX_PENDING;
	return True;
}

#ifdef CFG_RPDO

Bool MasterNode_SendPdo(MasterNode* pMasterNode, CanTxMsg* pCanTxMsg, RPdo* pdo)
{
	if (!CanTxMsg_IsFree(pCanTxMsg)) return False;	//有一个帧正在发送
	can_trasnmit_message_struct* msg = &pCanTxMsg->msg;

	msg->tx_dlen = 8;
	for (int i = 0; i < pdo->dataLength; i++)
	{
		msg->tx_data[i] = *pdo->mapPointer[i];
	}

	pCanTxMsg->state = TX_PENDING;
	return True;
}
#endif

Bool MasterNode_ReadEntryEx(MasterNode* pMasterNode, uint16 index, uint8 subIndex)
{
	MasterSdo* pSdo = &pMasterNode->sdoMgr;
	if (SDO_IDLE != pSdo->sdoState)
	{
		//pSdo is not free;
		return False;
	}
	if (NMT_PRE_OPERATION != pMasterNode->state && NMT_OPERATION != pMasterNode->state)
	{
		PFL(DL_CAN_MASTER, "%s[%X] state[%X] unsupport read SDO.\n", pMasterNode->name, pMasterNode->addr, pMasterNode->state);
		return False;
	}

	SdoPkt pkt;
	if (CO_SDO_AB_NONE == MasterSdo_TransInit(pSdo, index, subIndex, False, False, &pkt))
	{
		pSdo->txCounter++;
		return MasterNode_SendSdo(pMasterNode, &pkt);
	}
	return False;
}

//使用SDO读取字典项，如果总线忙，则放置到队列中
Bool MasterNode_ReadEntry(MasterNode* pMasterNode, uint16 index, uint8 subIndex)
{
	if (!MasterNode_ReadEntryEx(pMasterNode, index, subIndex))
	{
		OdInd* p = (OdInd*)QUEUE_getNew(&pMasterNode->sdoQueue);
		if (p)
		{
			p->index = index;
			p->sub = subIndex;
		}
		else
		{
			PFL_ERROR("sdoQueueFull.");
		}
		return False;
	}
	else
	{
		return True;
	}
}

Bool MasterNode_WriteEntry(MasterNode* pMasterNode, uint16 index, uint8 subIndex, uint8* pData, int len)
{
	MasterSdo* pSdo = &pMasterNode->sdoMgr;
	if (SDO_IDLE != pSdo->sdoState)
	{
		//pSdo is not free;
		return False;
	}
	if (NMT_PRE_OPERATION != pMasterNode->state && NMT_OPERATION != pMasterNode->state)
	{
		PFL(DL_CAN_MASTER, "%s[%X] state[%X] unsupport write SDO.\n", pMasterNode->name, pMasterNode->addr, pMasterNode->state);
		return False;
	}

	const CO_OD_entry* pEntry = OD_find(pSdo->pOd, pSdo->odSize, index, Null);
	if (pEntry)
	{
		Entry_SetValue(pEntry, subIndex, pData, len);
	}

	SdoPkt pkt;
	if (CO_SDO_AB_NONE == MasterSdo_TransInit(pSdo, index, subIndex, True, False, &pkt))
	{
		pSdo->txCounter++;
		return MasterNode_SendSdo(pMasterNode, (SdoPkt*)& pkt);
	}
	return False;
}

void MasterNode_ProcSdo(MasterNode* pMasterNode, Frame* pFrame)
{
	//CO_SDOclient_receive(Null, );
	//SDO_ERR ret = SDO_FALIED;
	SdoPkt sdoPktOut; memset(&sdoPktOut, 0, sizeof(SdoPkt));
	MasterSdo* pSdoMgr = &pMasterNode->sdoMgr;
	CO_ODF_arg_t* arg = &pMasterNode->sdoMgr.TransMgr.arg;

	pSdoMgr->TransMgr.isSend = 0;
	SDO_ERR err = MasterSdo_ProcRsp(pSdoMgr, (SdoPkt*)pFrame->data, &sdoPktOut);

	if (pSdoMgr->TransMgr.isSend)
	{
		MasterNode_SendSdo(pMasterNode, &sdoPktOut);
		pSdoMgr->TransMgr.isSend = False;
	}
	
	if(SDO_IDLE ==pSdoMgr->sdoState)
	{
		MasterSdo_Reset(pSdoMgr);
	}
	else if (SDO_FAILED == pSdoMgr->sdoState)
	{
		MasterSdo_Reset(pSdoMgr);
		//重新启动读取数据字典项
		if (err == CO_SDO_AB_TIMEOUT || CO_SDO_AB_CRC == err)
		{
			PFL(DL_CAN_MASTER, "%s[%02X] SDO[0x%04X.%02X] failed(%x).\n", pMasterNode->name, pMasterNode->addr, arg->index, arg->subIndex, err);
			//MasterNode_ReadEntry(pMasterNode, arg->index, arg->subIndex);
		}
	}
}

void MasterNode_Test(MasterNode* pMasterNode)
{
}

void MasterNode_DumpNeedUpload(MasterNode* pMasterNode)
{
	const CO_OD_entry* p = pMasterNode->pPrivateOd;
	uint16 index = p->index;
	uint8 subIndex = 0;
	uint32 attri = Entry_getAttribute(p, subIndex);
	for (p = pMasterNode->pPrivateOd; p; p = MasterNode_GetNextSdo(pMasterNode, &index, &subIndex))
	{
		uint32 attri = Entry_getAttribute(p, subIndex);
		Printf("%s[%02X] SDO[0x%04X.%02X] %d.\n", pMasterNode->name, pMasterNode->addr, index, subIndex, IS_NEED_SDO_UPLOAD(attri) ? "Need upload" : "");
	}
}

void MasterNode_PreOperation_Do(MasterNode* pMasterNode)
{
	//清除所有的数据字典值
	//CO_OD_ResetValue(pMasterNode->pPrivateOd, pMasterNode->privateOdSize);
	if (pMasterNode->manualControl) return;

	//读取第一个数据字典
	const CO_OD_entry* p = pMasterNode->pPrivateOd;
	uint16 index = p->index;
	uint8 subIndex = 0;
	uint32 attri = Entry_getAttribute(p, subIndex);
	if (!IS_NEED_SDO_UPLOAD(attri))
	{
		p = MasterNode_GetNextSdo(pMasterNode, &index, &subIndex);
	}
	if(p)
		MasterNode_ReadEntry(pMasterNode, p->index, subIndex);
}

void MasterNode_Stopped_Do(MasterNode* pMasterNode)
{

}

void MasterNode_Init_Do(MasterNode* pMasterNode)
{
}

void MasterNode_Operation_Do(MasterNode* pMasterNode)
{

}

SDO_ERR MasterNode_PdoPro(MasterNode* pMasterNode, uint8  pdoType, uint8* pData)
{
	uint16 pdoIndex = 0;
	for (const PdoTypeMapInd* p = pMasterNode->PdoTypeMapIndArray; p->index; p++)
	{
		if (p->pdoType == pdoType)
		{
			pdoIndex = p->index;
			break;
		}
	}
	const CO_OD_entry* pEntry = OD_find((CO_OD_entry*)pMasterNode->pGlobalOd, pMasterNode->globalOdSize, pdoIndex, Null);
	if (pEntry == Null)
	{
		PFL(DL_CAN_MASTER, "%s[%02X] ROM OD[0x%04X] not found.\n", pMasterNode->name, pMasterNode->addr, pdoIndex);
		return CO_SDO_AB_NO_MAP;
	}

	CO_OD_entryRecord_t* pRecord = (CO_OD_entryRecord_t*)pEntry->pData;
	OD_TPDOMappingParameter_t* pMapPar = (OD_TPDOMappingParameter_t*)pRecord->pData;
	uint32_t* map = &pMapPar->mappedObject1;

	uint8_t offset = 0;
	for (int i = 0; i < pMapPar->numberOfMappedObjects; i++)
	{
		int index = (uint16_t)(map[i] >> 16);
		int subIndex = (uint8_t)(map[i] >> 8);
		int dataLen = (uint8_t)map[i] >> 3;   /* data length in bits , and convert to in bytes*/
		uint16 indexOfArray = 0;
		pEntry = OD_find(pMasterNode->pPrivateOd, pMasterNode->privateOdSize, index, &indexOfArray);
		if (pEntry == Null || subIndex > pEntry->maxSubIndex)
		{
			PFL(DL_CAN_MASTER, "%s[%02X] Private OD[0x%04X.%02X] not found.\n", pMasterNode->name, pMasterNode->addr, index, subIndex);
			return CO_SDO_AB_NO_MAP;
		}
		uint8* pOdData = Entry_getDataPointer(pEntry, subIndex);

		EntryExt* pEntryExt = &pMasterNode->pEntryExtArray[indexOfArray];
		CO_SDO_abortCode_t ret = CO_SDO_AB_NONE;
		CO_ODF_arg_t arg = { pMasterNode, &pData[offset], pOdData, dataLen, 0, 0, index, subIndex , True, True, True, dataLen, 0 };
		EntryEventFn onEvent = (pEntryExt && pEntryExt->OnEvent) ? pEntryExt->OnEvent : Null;

		if (memcmp(pOdData, &pData[offset], dataLen) != 0)
		{
			PFL(DL_OD, "%s[%02X] PDO OD[%04X.%02X] =",pMasterNode->name, pMasterNode->addr, index, subIndex); DUMP_BYTE_LEVEL(DL_OD , &pData[offset], dataLen);

			if (onEvent) ret = onEvent(pMasterNode, TRANS_CHANGE_BEFORE, CO_SDO_AB_NONE, &arg);
			if (CO_SDO_AB_NONE == ret)
			{
				memcpy(pOdData, &pData[offset], dataLen);
				if (onEvent) ret = onEvent(pMasterNode, TRANS_CHANGE_AFTER, CO_SDO_AB_NONE, &arg);
			}
		}

		if (onEvent) ret = onEvent(pMasterNode, TRANS_DONE, CO_SDO_AB_NONE, &arg);

		offset += dataLen;
	}
	return CO_SDO_AB_NONE;
}

#ifdef CFG_RPDO
void MasterNode_SetRPDOSentData(MasterNode* pMasterNode, const void* pOdData, const void* pNewData, int len)
{
	RPdo* pdo = pMasterNode->rpdo;
	CanTxMsg* pCanTxMsg = pMasterNode->pdoCanTxMsg;

	for (int i = 0
		; i < GET_ELEMENT_COUNT(pMasterNode->pdoCanTxMsg)
		; i++, pCanTxMsg++, pdo++)
	{
		for (int j = 0; j < pdo->dataLength; j++)
		{
			if (pdo->mapPointer[j] == pOdData)
			{
				memcpy(&pdo->sentData[j], pNewData, len);
				break;
			}
		}
	}
}

void MasterNode_RPDOProc(MasterNode* pMasterNode)
{
	RPdo* pdo = pMasterNode->rpdo;
	CanTxMsg* pCanTxMsg = pMasterNode->pdoCanTxMsg;

	if (!SwTimer_isTimerOutEx(pMasterNode->rpdoIntervalMs, 100)) return;
	pMasterNode->rpdoIntervalMs = GET_TICKS();

	for (int i = 0
		; i < GET_ELEMENT_COUNT(pMasterNode->pdoCanTxMsg) && pCanTxMsg->state == TX_INIT
		; i++, pCanTxMsg++, pdo++)
	{
		if (!pdo->sendRequest) pdo->sendRequest = RPdo_isCOS(pdo);

		if (pdo->sendRequest)
		{
			if (MasterNode_SendPdo(pMasterNode, pCanTxMsg, pdo))
			{
				pdo->sendRequest = False;
			}
		}
	}
}
#endif
void MasterNode_SwitchState(MasterNode* pMasterNode, NMT_STATE newState)
{
	//如果状态没有发生变化
	if (newState == pMasterNode->state) return;
	
	//PFL(DL_CAN_MASTER, "Node[%d] State: [%X] -> [%X]\n", pMasterNode->addr, pMasterNode->state, newState);
	LOG_TRACE1(pMasterNode->addr, ET_STATE_SWITCH, newState);

	pMasterNode->state = newState;
	switch (newState)
	{
	case NMT_STOPPED:
		MasterNode_Stopped_Do(pMasterNode);
		//pMasterNode->Stopped(pMasterNode);
		break;
	case NMT_OPERATION:
		//pMasterNode->Operation(pMasterNode);
		MasterNode_Operation_Do(pMasterNode);
		break;
	case NMT_PRE_OPERATION:
		//pMasterNode->PreOperation(pMasterNode);
		MasterNode_PreOperation_Do(pMasterNode);
		break;
	case NMT_INIT:
		//pMasterNode->PreOperation(pMasterNode);
		MasterNode_Init_Do(pMasterNode);
		break;
	default:
		break;
	}
}

void MasterNode_Fsm_Init(MasterNode* pMasterNode, uint8 msgId, uint32 param1)
{
	if (msgId == MSG_RX_FRAME)
	{
		Frame* pFrame = (Frame*)param1;
		switch (pFrame->type)
		{
		case FHEARTBEAT:
			if (pFrame->data[0] == NMT_PRE_OPERATION)
			{
				MasterNode_SwitchState(pMasterNode, NMT_PRE_OPERATION);
			}
			else
			{
				MasterNode_SendNmt(pMasterNode, SET_NMT_PRE_OPERATION);
			}
			break;
		}
	}
}

void MasterNode_Fsm_Stop(MasterNode* pMasterNode, uint8 msgId, uint32 param1)
{
	if (msgId == MSG_RX_FRAME)
	{
		Frame* pFrame = (Frame*)param1;
		switch (pFrame->type)
		{
		case FHEARTBEAT:
			if (pFrame->data[0] != NMT_STOPPED)
			{
				MasterNode_SendNmt(pMasterNode, SET_NMT_STOPPED);
			}
			break;
		}
	}
}

void MasterNode_Fsm_PreOperation(MasterNode* pMasterNode, uint8 msgId, uint32 param1)
{
	MasterSdo* pSdo = &pMasterNode->sdoMgr;
	if (msgId == MSG_RX_FRAME)
	{
		Frame* pFrame = (Frame*)param1;
		switch (pFrame->type)
		{
		case FHEARTBEAT:
			if (pMasterNode->manualControl) return;
			if (pFrame->data[0] != NMT_PRE_OPERATION)
			{
				MasterNode_SendNmt(pMasterNode, SET_NMT_PRE_OPERATION);
			}
			else if (CanTxMsg_IsFree(&pMasterNode->sdoCanTxMsg))
			{
				CO_ODF_arg_t* arg = &pSdo->TransMgr.arg;
				PFL_WARNING("%s[%02X] SDO[%04X.%02X].\n", pMasterNode->name, pMasterNode->addr, arg->index, arg->subIndex);
				MasterSdo_Reset(pSdo);
				if (arg->index != 0)
				{
					MasterNode_ReadEntry(pMasterNode, arg->index, arg->subIndex);
				}
			}
			break;
		case SDOtx:
			//处理SDO
			MasterNode_ProcSdo(pMasterNode, pFrame);
			if (pSdo->sdoState == SDO_IDLE)
			{
				if (pMasterNode->manualControl) return;

				CO_ODF_arg_t* arg = &pSdo->TransMgr.arg;
				//读取数据字典下一个
				if (MasterNode_GetNextSdo(pMasterNode, &arg->index, &arg->subIndex))
				{
					MasterNode_ReadEntry(pMasterNode, arg->index, arg->subIndex);
				}
				else
				{
					//全部数据字典读取完毕,切换到OPERATION状态
					MasterNode_SendNmt(pMasterNode, SET_NMT_OPERATION);

				}
			}
			break;
		}
	}
}

void MasterNode_Fsm_Operation(MasterNode* pMasterNode, uint8 msgId, uint32 param1)
{
//	MasterSdo* pSdoMgr = &pMasterNode->sdoMgr;
//	SdoTransMgr* pSdo = &pSdoMgr->TransMgr;

	if (msgId == MSG_RX_FRAME)
	{
		Frame* pFrame = (Frame*)param1;

		//PFL(DL_MAIN, "Operation： rx %s\n", Frame_GetTypeString(pFrame->type));
		uint8 type = pMasterNode->GetFrameType(pFrame->ident);
		switch (type)
		{
		case FHEARTBEAT:
			if (pFrame->data[0] != NMT_OPERATION)	//节点可能复位，需要切换到NMT_PRE_OPERATION，重新读取数据字典
			{
				MasterNode_SendNmt(pMasterNode, SET_NMT_PRE_OPERATION);
			}
			break;
		case PDO1tx:
		case PDO2tx: 
		case PDO3tx: 
		case PDO4tx:
		case PDO5tx:
		case PDO6tx:
		case PDO7tx:
		case PDO8tx:
		case PDO9tx:
		case PDO10tx:
		case PDO11tx:
		case PDO12tx:
			MasterNode_PdoPro(pMasterNode, type, pFrame->data);
			break;
		case SDOtx:
			MasterNode_ProcSdo(pMasterNode, pFrame);
			break;
		default :
			Printf("%s() rx %s, do nothing\n", _FUNC_, Frame_GetTypeString(pFrame->type));
		}
	}
#ifdef CFG_RPDO
	MasterNode_RPDOProc(pMasterNode);
#endif
}

void MasterNode_SdoTxDone(MasterNode* pMasterNode, MasterSdo* pSdoMgr, TxState txState, uint8 msgId)
{
	SdoTransMgr* pTransMgr = &pSdoMgr->TransMgr;
	CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pTransMgr;

	if (msgId == MSG_SDO_WAIT_RSP_TIMEOUT)
	{
		//重发
		MasterSdo_Reset(pSdoMgr);
		PFL_WARNING("%s[%02X] SDO[%04X.%02X] receive rsponse time out.\n", pMasterNode->name, pMasterNode->addr, arg->index, arg->subIndex);
		MasterNode_ReadEntry(pMasterNode, arg->index, arg->subIndex);
		return;
	}

	if (txState != TX_SUCCESS)
	{
		//PFL_WARNING("%s[%02X] SDO[%04X.%02X] failed(%d).\n", pMasterNode->name, pMasterNode->addr, pSdo->index, pSdo->subIndex, p->state);
		MasterNode_ReadEntry(pMasterNode, arg->index, arg->subIndex);
		return;
	}

	//启动定时器，等待SDO应答
	if (pTransMgr->waitForRsp && !pMasterNode->sdoMgr.timer.m_isStart)
	{
		//PFL_WARNING("%s[%02X] SDO[%04X.%02X] Timer start.\n", pMasterNode->name, pMasterNode->addr, pSdo->index, pSdo->subIndex);
		MasterSdo_TimerStart(&pMasterNode->sdoMgr, 1000);
	}

	if (pSdoMgr->sdoState == SDO_DONE)
	{
		MasterSdo_Reset(pSdoMgr);
	}
#ifdef CFG_BLOCK_TRANS
	else if (pSdoMgr->sdoState == SDO_BLOCK_DOWNLOAD)
	{
		if (txState == TX_SUCCESS)
		{
			if (pTransMgr->seq < pTransMgr->blockSize)	//段传输未结束
			{
				BlockDownloadReqPkt req;
				Block_DownloadReqPkt(pTransMgr, &req);
				MasterNode_SendSdo(pMasterNode, (SdoPkt*)& req);
			}
		}
		else
		{
			//传输失败，结束
			MasterSdo_Reset(pSdoMgr);
		}
	}
#endif

}

//任何状态下都必须处理的消息
void MasterNode_AnyStateDo(MasterNode* pMasterNode, uint8 msgId, uint32 param1)
{
	if (msgId == MSG_RX_FRAME)
	{
		Frame* pFrame = (Frame*)param1;
		uint8 type = pMasterNode->GetFrameType(pFrame->ident);
		if (type == FHEARTBEAT)
		{
			SwTimer_Start(&pMasterNode->heartbeatTimer, pMasterNode->HeartbeatTime * 2, 0);
		}
		else if (type == SDOtx)
		{
			MasterSdo* pSdo = &pMasterNode->sdoMgr;
			SdoTransMgr* pMgr = &pSdo->TransMgr;
			CO_ODF_arg_t* arg = &pMasterNode->sdoMgr.TransMgr.arg;

			if (pSdo->sdoState == SDO_BLOCK_UPLOAD && pMgr->seq < pMgr->blockSize)
			{
			}
			else if (!CanTxMsg_IsFree(&pMasterNode->sdoCanTxMsg))
			{
				//SdoTransMgr* pSdo = &pMasterNode->sdoMgr.TransMgr;
				PFL_WARNING("%s[%02X] SDO[%04X.%02X] free.\n", pMasterNode->name, pMasterNode->addr, arg->index, arg->subIndex);
				CanTxMsg_Free(&pMasterNode->sdoCanTxMsg);
			}
		}
	}
}


Bool MasterNode_ReadPendingEntry(MasterNode* pMasterNode)
{
	//发送缓存中的命令
	Queue* pQueue = &pMasterNode->sdoQueue;
	for (OdInd* p = (OdInd*)QUEUE_getHead(pQueue)
		; p != Null
		; p = (OdInd*)QUEUE_getHead(pQueue))
	{
		if (MasterNode_ReadEntryEx(pMasterNode, p->index, p->sub))
		{
			QUEUE_removeHead(pQueue);
		}
		break;
	}
}

void MasterNode_Fsm(MasterNode* pMasterNode, uint8 msgId, uint32 param1)
{
	NMT_STATE state = pMasterNode->state;
	MasterNode_AnyStateDo(pMasterNode, msgId, param1);

	if (state == NMT_OPERATION)
	{
		MasterNode_Fsm_Operation(pMasterNode, msgId, param1);
		MasterNode_ReadPendingEntry(pMasterNode);
	}
	else if (state == NMT_INIT || state == NMT_DISCONNECT)
	{
		MasterNode_Fsm_Init(pMasterNode, msgId, param1);
	}
	else if (state == NMT_STOPPED)
	{
		MasterNode_Fsm_Stop(pMasterNode, msgId, param1);
	}
	else if (state == NMT_PRE_OPERATION)
	{
		MasterNode_Fsm_PreOperation(pMasterNode, msgId, param1);
		MasterNode_ReadPendingEntry(pMasterNode);
	}
}

void MasterNode_ProcFrame(MasterNode* pMasterNode, Frame* pFrame)
{
	MasterNode_PostFrame(pMasterNode, MSG_RX_FRAME, pFrame);
}

void MasterNode_TimerProc(MasterNode* pMasterNode)
{
	MasterSdo* pSdo = &pMasterNode->sdoMgr;

	if (MasterSdo_IsTimerOut(pSdo))
	{
		if (pSdo->sdoState != SDO_IDLE)
		{
			CanTxMsg_Free(&pMasterNode->sdoCanTxMsg);
			MasterSdo_Reset(pSdo);
			SdoTransMgr* pTransMgr = &pSdo->TransMgr;
			CO_ODF_arg_t* arg = (CO_ODF_arg_t*)pTransMgr;

			if (pSdo->txCounter <= 3)
			{
				PFL_WARNING("%s[%02X] SDO[%04X.%02X] receive rsponse time out[%d].\n", pMasterNode->name, pMasterNode->addr, arg->index, arg->subIndex, pSdo->txCounter);
				MasterNode_ReadEntry(pMasterNode, arg->index, arg->subIndex);
			}
			else
			{
				PFL_WARNING("%s[%02X] SDO[%04X.%02X] failed, time out=%d .\n", pMasterNode->name, pMasterNode->addr, arg->index, arg->subIndex, pSdo->txCounter);
			}
		}
	}

	//判断心跳定时器是否超时
	if (SwTimer_isTimerOut(&pMasterNode->heartbeatTimer))
	{
		MasterNode_SwitchState(pMasterNode, NMT_INIT);
	}
}

void MasterNode_Start(MasterNode* pMasterNode)
{
	if (pMasterNode->isStart) return;
	pMasterNode->isStart = True;
}

void MasterNode_Stop(MasterNode* pMasterNode)
{
	MasterSdo* pSdoMgr = &pMasterNode->sdoMgr;
	pMasterNode->isStart = False;

	SwTimer_Stop(&pSdoMgr->timer);
	SwTimer_Stop(&pMasterNode->heartbeatTimer);
}

extern CanBus* g_pMasterCanBus;
void MasterNode_TxFrameProc(MasterNode* pMasterNode)
{
	CanTxMsg* pMgr = &pMasterNode->nmtCanTxMsg;
	if (pMgr->state == TX_PENDING)
	{
		if (CanBus_CopyMsgToMailBox(g_pMasterCanBus, pMgr))
		{
			pMgr->state = TX_INPROCESS;
		}
	}
	else if (pMgr->state >= TX_SUCCESS)
	{
		if (pMgr->state == TX_SUCCESS)
		{
			MasterNode_SwitchState(pMasterNode, MasterNode_CsToState(pMgr->msg.tx_data[0]));
		}
		CanTxMsg_Free(pMgr);
	}

	//处理SDO对象
	pMgr = &pMasterNode->sdoCanTxMsg;
	if (pMgr->state == TX_PENDING)
	{
		if (CanBus_CopyMsgToMailBox(g_pMasterCanBus, pMgr))
		{
			pMgr->state = TX_INPROCESS;
		}
	}
	else if (pMgr->state >= TX_SUCCESS)
	{
		TxState txState = pMgr->state;
		CanTxMsg_Free(pMgr);
		MasterNode_SdoTxDone(pMasterNode, &pMasterNode->sdoMgr, txState, MSG_TX_DONE);
	}

	//处理RPDO对象
	RPdo* pdo = pMasterNode->rpdo;
	pMgr = pMasterNode->pdoCanTxMsg;
	for (int i = 0; i < RPDO_COUNT; i++, pMgr++, pdo++)
	{
		if (pMgr->state == TX_PENDING)
		{
			if (CanBus_CopyMsgToMailBox(g_pMasterCanBus, pMgr))
			{
				pMgr->state = TX_INPROCESS;
			}
		}
		else if (pMgr->state >= TX_SUCCESS)
		{
			if (pMgr->state == TX_SUCCESS)
			{
				memcpy(pdo->sentData, pMgr->msg.tx_data, pdo->dataLength);
				if (pdo->pPdoCfg->Done)
				{
					pdo->pPdoCfg->Done(pMasterNode, pdo);
				}
			}
			CanTxMsg_Free(pMgr);
		}
	}
}

void MasterNode_MsgProc(MasterNode* pMasterNode)
{
	Queue* pQueue = &pMasterNode->msgQueue;
	for (NodeMsg* pMsg = (NodeMsg*)QUEUE_getHead(pQueue)
		; pMsg != Null
		; QUEUE_removeHead(pQueue), pMsg = (NodeMsg*)QUEUE_getHead(pQueue))
	{
		if (pMsg->msgId == MSG_RX_FRAME)
		{
			//if (g_TestFlag == pMasterNode->addr)
			//{
			//	Printf("%s[%02X] RX:", pMasterNode->name, pMasterNode->addr);
			//	FrameEx_Dump((FrameEx*)&pMsg->frame, DL_MAIN, "");
			//}
			MasterNode_Fsm(pMasterNode, MSG_RX_FRAME, (uint32)&pMsg->frame);
		}
		else
		{
			MasterNode_Fsm(pMasterNode, pMsg->msgId, pMsg->param1);
		}
	}
	MasterNode_Fsm(pMasterNode, MSG_RUN, 0);
}
void MasterNode_Run(MasterNode* pMasterNode)
{
	if (!pMasterNode->isStart) return;

	//这2个函数调用顺序有要求
	//1.发送处理函数MasterNode_TxFrameProc必须首先调用
	MasterNode_TxFrameProc(pMasterNode);

	//2.之后才能Call函数MasterNode_MsgProc
	MasterNode_MsgProc(pMasterNode);

	MasterNode_TimerProc(pMasterNode);

}

void MasterNode_Init(MasterNode* pMasterNode
	, uint8 addr
	, const char* name
	, const CO_OD_entry_t* pGlobalOd//公共的数据字典，不能被修改，来自Slave定义，可以被多个相同类型对象公用
	, const CO_OD_entry_t* pPrivateOd		//重新影射数据字典项
	, int privateOdSize
	, CO_OD_extension_t* ODExtensions
	, const PdoTypeMapInd* PdoTypeMapIndArray
	, EntryExt* pEntryExtArray
	)
{

	const static PdoTypeMapInd PtiMap[] =
	{
		{0x1A00, PDO1tx},
		{0x1A01, PDO2tx},
		{0x1A02, PDO3tx},
		{0x1A03, PDO4tx},
	};

	memset(pMasterNode, 0, sizeof(MasterNode));
	pMasterNode->addr = addr;
	pMasterNode->name = name;
	int odSize = PmsOd_GetSize();

	pMasterNode->Start	= (NodeFn)MasterNode_Start;
	pMasterNode->Stop	= (NodeFn)MasterNode_Stop;
	pMasterNode->Run	= (NodeFn)MasterNode_Run;

	pMasterNode->state = NMT_INIT;
	pMasterNode->pGlobalOd = pGlobalOd;
	pMasterNode->globalOdSize = odSize;
	pMasterNode->pPrivateOd = pPrivateOd;
	pMasterNode->privateOdSize = privateOdSize;
	pMasterNode->pObjArg = pMasterNode;
	pMasterNode->ODExtensions = ODExtensions;
	pMasterNode->pEntryExtArray = pEntryExtArray;
	pMasterNode->entryExtCount = privateOdSize;
	pMasterNode->PdoTypeMapIndArray = (PdoTypeMapIndArray == Null) ? PtiMap : PdoTypeMapIndArray;

	pMasterNode->FrameProc = (NodeFrameProc)MasterNode_ProcFrame;
	pMasterNode->GetFrameType = MasterNode_GetFrameType;
	pMasterNode->HeartbeatTime = HEARTBEAT_TIME_MS;
	pMasterNode->Dump = (NodeFn)MasterNode_Dump;

	MasterSdo_Init(&pMasterNode->sdoMgr, pMasterNode);

	//pMasterNode->FsmInit		  = MasterNode_Fsm_Init;
	//pMasterNode->FsmStopped	  = MasterNode_Fsm_Stop;
	//pMasterNode->FsmPreOperation= MasterNode_Fsm_PreOperation;
	//pMasterNode->FsmOperation	  = MasterNode_Fsm_Operation;
	//pMasterNode->Stopped		= MasterNode_Stopped_Do;
	//pMasterNode->Operation	= MasterNode_Operation_Do;
	//pMasterNode->PreOperation	= 33;

	CanTxMsg_Init(&pMasterNode->nmtCanTxMsg, pMasterNode, FNMT, 0);
	CanTxMsg_Init(&pMasterNode->sdoCanTxMsg, pMasterNode, SDOrx, addr);

	const static uint8 ft[] = { PDO1rx, PDO2rx, PDO3rx, PDO4rx};
	for (int i = 0; i < RPDO_COUNT; i++)
	{
		CanTxMsg_Init(&pMasterNode->pdoCanTxMsg[i], pMasterNode, ft[i], addr);
	}

	QUEUE_init(&pMasterNode->msgQueue, pMasterNode->msgBuf, sizeof(NodeMsg), GET_ELEMENT_COUNT(pMasterNode->msgBuf));
}



















