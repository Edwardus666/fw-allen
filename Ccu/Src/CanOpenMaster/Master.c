#include <common.h>
#include "Master.h"
#include "Frame.h"
#include "DriverCan.h"
#include "Pms.h"
#include "PmsMgr.h"

Master* g_pMaster;
CanBus* g_pMasterCanBus;

void Master_Dump(uint8 addr)
{
	MasterNode* p = Master_FindMasterNode(addr);
	if (p == Null)
	{
		Printf("No node addr[%x]\n", addr);
		return;
	}
	p->Dump(p);
}

void Master_SetNodeValue(uint8 addr, int index, uint32 val)
{
	MasterNode* pMasterNode = Master_FindMasterNode(addr);
	if (pMasterNode == Null)
	{
		Printf("No node addr[%x]\n", addr);
	}
	if (index == 0)
	{
		pMasterNode->manualControl = val;
		Printf("%s[%X].manualControl=%d\n", pMasterNode->name, pMasterNode->addr, pMasterNode->manualControl);
	}
}

void Master_SetNmtState(uint8 addr, uint8 cs)
{
	MasterNode* pMasterNode = Master_FindMasterNode(addr);
	if (pMasterNode == Null)
	{
		Printf("No node addr[%x]\n", addr);
	}
	if (MasterNode_CsIsValid(cs))
	{
		pMasterNode->manualControl = True;
		MasterNode_SendNmt(pMasterNode, (SET_NMT_STATE)(cs));
	}
	else
	{
		Printf("Param cs[%d] is invalid.\n", cs);
	}
}

void Master_AddNode(MasterNode* pMasterNode)
{
	for (int i = 0; i < MAX_LIST_COUNT; i++)
	{
		if (g_pMaster->nodes[i] == Null)
		{
			g_pMaster->nodes[i] = pMasterNode;
			return;
		}
	}
	
	Printf("Master node array is full.!");
	Assert(False);
}

MasterNode* Master_FindMasterNode(uint8 id)
{
	MasterNode* pMasterNode = Null;
	for (int i = 0; i < MAX_LIST_COUNT && g_pMaster->nodes[i]; i++)
	{
		pMasterNode = g_pMaster->nodes[i];
		if (pMasterNode->addr == id)
		{
			return pMasterNode;
		}
	}

	return Null;
}

void Master_ConvertToCanOpenFrame(const can_receive_message_struct* msg, Frame* pFrame)
{
	pFrame->ident = msg->rx_sfid;
	pFrame->rtr = msg->rx_ft;
	pFrame->len = msg->rx_dlen;
	memcpy(pFrame->data, msg->rx_data, pFrame->len);
}

void Master_RxData(const can_receive_message_struct* pMsg)
{
	uint16 nodeId = pMsg->rx_sfid & 0x3F;
	Frame frame = { 0 };
	MasterNode* pMasterNode;

	Master_ConvertToCanOpenFrame(pMsg, &frame);

	pMasterNode = (MasterNode*)Master_FindMasterNode(nodeId);
	if (pMasterNode)
	{
		pMasterNode->FrameProc(pMasterNode, &frame);
	}
}

void Master_TimerProc()
{
}

void Master_Stop()
{
	CanBus_Stop(g_pMasterCanBus);
	g_pMaster->isStart = False;

	MasterNode* pMasterNode = g_pMaster->nodes[0];
	for (int i = 0; i < MAX_LIST_COUNT && pMasterNode; i++, pMasterNode = g_pMaster->nodes[i])
	{
		pMasterNode->Stop(pMasterNode);
	}
}


void Master_Start()
{
	g_pMaster->isStart = True;

	MasterNode* pMasterNode = g_pMaster->nodes[0];
	for (int i = 0; i < MAX_LIST_COUNT && pMasterNode; i++, pMasterNode = g_pMaster->nodes[i])
	{
		pMasterNode->Start(pMasterNode);
	}

	CanBus_Start(g_pMasterCanBus);
}

void Master_Run()
{
	if (!g_pMaster->isStart) return;

	CanBus_Run(g_pMasterCanBus);
	//Master_ProcRxBuf();
	Master_TimerProc();

	MasterNode* pMasterNode = g_pMaster->nodes[0];
	for (int i = 0; i < MAX_LIST_COUNT && pMasterNode; i++, pMasterNode = g_pMaster->nodes[i])
	{
		pMasterNode->Run(pMasterNode);
	}
}

void Master_Init()
{
	static Master g_Master;
	g_pMaster = &g_Master;
	g_pMasterCanBus = CanBus_Init(CAN_BUS_0);

	Obj_Register((Obj*)g_pMaster, "Master", Master_Start, Master_Stop, Master_Run);

	//Master初始化必须在PmsMgr_Init()之前
	PmsMgr_Init();
}
