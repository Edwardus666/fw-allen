
#include "Common.h"
#include "SbwMgr.h"
#include "ShelfLock.h"

static SbwMgr* g_pSbwMgr;
static Rs485Protocol* g_pSbwUtp;
Uart* g_pSbwMgrUart;


int SbwMgr_TxData(const uint8* pData, int len)
{
	DUMP_BYTE_EX_LEVEL(DL_SHELF_LOCK, ("LockTx:"), pData, len);
	Uart_Tx(g_pSbwMgrUart, pData, len);

	return len;
}

void SbwMgr_RxDataProc()
{
	static uint8 buff[48];
	Rs485ProtocolFrame* pFrame = (Rs485ProtocolFrame*)&buff[1];

	int offset = 0;
	uint8* p = SafeBuf_SearchByte(&g_pSbwMgrUart->rxBuf, 0xFF, &offset);
	if (p)
	{
		uint16 ind = 0;
		FRAME_STATE state = FRAME_INIT;
		DUMP_BYTE_EX_LEVEL(DL_SHELF_LOCK, ("LockRx:"), p, offset);
		for (int i = 0; i < offset; i++, p++)
		{
			if (Rs485Protocol_ConvertToHost(buff, sizeof(buff), &ind, &state, *p))
			{
				if(Rs485Protocol_VerifyFrame(pFrame, ind - 2))
				{
					Rs485Protocol_RcvFrameHandler(g_pSbwUtp, pFrame);
				}
				break;
			}
		}
		SafeBuf_Read(&g_pSbwMgrUart->rxBuf, Null, offset);
	}
}

Rs485ProtocolNode* SbwMgr_GetNode(uint8 addr)
{
	for (int i = 0; i < g_pSbwMgr->nodeCount; i++)
	{
		if (g_pSbwMgr->node[i]->addr == addr) return g_pSbwMgr->node[i];
	}

	Assert(False);
	return Null;
}

Bool SbwMgr_Send(UtpCmd* p)
{
	return Rs485Protocol_SendCmd(g_pSbwUtp, p, 1000, 2);
}

Rs485ProtocolNode* SbwMgr_SwitchNextNode()
{
	Rs485ProtocolNode* node = g_pSbwMgr->node[g_pSbwMgr->nodeIndex];
	if (node->IsBusy(node) || g_pSbwMgr->nodeCount == 1) return node;

	if (++g_pSbwMgr->nodeIndex >= g_pSbwMgr->nodeCount) g_pSbwMgr->nodeIndex = 0;
	node = g_pSbwMgr->node[g_pSbwMgr->nodeIndex];

	g_pSbwUtp->m_Addr = node->addr;
	g_pSbwUtp->m_pOwerObj = node;
	g_pSbwUtp->m_reqDispatch = node->reqDispatch;
	g_pSbwUtp->m_cmdArray = node->utpCmdArray;

	return node;
}

void SbwMgr_Reset()
{
}

void SbwMgr_Run()
{
	Rs485ProtocolNode* node = SbwMgr_SwitchNextNode();

	Rs485Protocol_Run(g_pSbwUtp);
	SbwMgr_RxDataProc();

	node->Run(node, Rs485Protocol_isBusy(g_pSbwUtp));
}

void SbwMgr_Stop()
{
}

//SbwMgr 启动
void SbwMgr_Start()
{
	if (g_pSbwMgr == Null || g_pSbwMgr->state == SBW_PRE_OPERATION) return;

	g_pSbwMgr->state = SBW_PRE_OPERATION;
	Uart_Start(g_pSbwMgrUart);
}

void SbwMgr_Init()
{
	static SbwMgr g_SbwMgr;
	static Obj sbwObj;
	Rs485ProtocolNode* pNode = Null;

	Obj_Register(&sbwObj, "SbwMgr", SbwMgr_Start, SbwMgr_Stop, SbwMgr_Run);

	g_pSbwMgr = &g_SbwMgr;
	g_pSbwUtp = &g_SbwMgr.utp;
	memset(g_pSbwMgr, 0, sizeof(SbwMgr));

	//创建节点对象
	g_SbwMgr.node[g_SbwMgr.nodeCount++] = ShelfLock_Create(SHELF_LOCK_485_ADDR);
	Assert(g_SbwMgr.nodeCount <= MAX_SBW_NODE_SIZE);
	pNode = g_SbwMgr.node[0];
	//初始化协议对象
	Rs485Protocol_Init(&g_pSbwMgr->utp, pNode->addr, SbwMgr_TxData, Null, Null, pNode);
	//初始化串口通信对象
	g_pSbwMgrUart = Uart_InitEx(CCU_MISC_COM, 19200, USART_PM_EVEN);
}
