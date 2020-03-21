
#include "Common.h"
#include "ShelfLock.h"
#include "SbwMgr.h"
#include "ShelfLockOd.h"
#include "UnitOD.h"
#include "DriverIo.h"

ShelfLock* g_pShelfLock;
Rs485ProtocolNode* g_pShelfLockNode;
static ShelfLock g_ShelfLock;
uint8  g_lockState;

//ShelfLock 启动
static uint8 g_lockVerCmd[] = { 0x00, 0x00 };
static uint8 g_unLockDelay = 1;

UTP_EVENT_RC ShelfLock_RspProc_SetLock(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd, UTP_EVENT ev);
static UtpCmd g_ShelfLockCmd[] =
{
	{BTLOCK_READ_SN		, "ReadVer"	 , g_lockVerCmd, sizeof(g_lockVerCmd), (uint8*)& g_ShelfLock.base.ver, sizeof(HwFwVer), (UtpEventFn)Rs485ProtocolNode_RspProc_ReadVersion, 0x02},
	{BTLOCK_READ_STATE	, "ReadState", Null		  , 0, (uint8*)& g_lockState, 1, (UtpEventFn)Rs485ProtocolNode_RspProc_Common, UTP_NO_RESULT_BYTE_IN_RSP},

	{BTLOCK_SET_UNLOCK	, "SetLock"  , &g_unLockDelay, 1, Null, 0, (UtpEventFn)ShelfLock_RspProc_SetLock},
	{BTLOCK_SET_LOCK	, "SetUnLock", Null			 , 0, Null, 0, (UtpEventFn)ShelfLock_RspProc_SetLock},
	{BTLOCK_CLR_INTERUPT, "ClrInt"   , Null			 , 0, Null, 0, (UtpEventFn)Rs485ProtocolNode_RspProc_Common},
	{0},
};

UTP_EVENT_RC ShelfLock_RspProc_ReadState(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd, UTP_EVENT ev)
{
	//状态改变
	if (ev == UTP_EVENT_CHANGED_AFTER)
	{
		LOG_TRACE1(OBJID_SHELF_LOCK, ET_SHELF_LOCK_LOCK_STATE_CHANGED, g_lockState);
	}
	return UTP_RC_SUCCESS;
}

UTP_EVENT_RC ShelfLock_RspProc_SetLock(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd, UTP_EVENT ev)
{
	if (ev == UTP_TRANSFER_DONE)
	{
		Rs485ProtocolNode_RspProc_Common(pNode, pUtpCmd, ev);
		if (pUtpCmd->lastError == 0)
			Rs485ProtocolNode_SendCmd(pNode, BTLOCK_READ_STATE);
	}
	return UTP_RC_SUCCESS;
}

void ShelfLock_SetLock(Bool isLock)
{
	UtpCmd* p = Rs485Protocol_FindCmd(g_ShelfLockCmd, isLock ? BTLOCK_SET_LOCK : BTLOCK_SET_UNLOCK);
	p->isForceSend = True;
}

Rs485ProtocolNode*  ShelfLock_Create(uint8 addr)
{
	CO_OD_extension_t odExtension[SHELF_LOCK_OD_SIZE];	//当OD被Master读或者写时触发内部函数调用

	g_pShelfLock = &g_ShelfLock;
	g_pShelfLockNode = (Rs485ProtocolNode*)g_pShelfLock;
	memset(g_pShelfLock, 0, sizeof(ShelfLock));

	Rs485ProtocolNode_Init(g_pShelfLockNode
		, addr
		, "BTLock"
		, g_ShelfLockCmd
		, &g_ShelfLockCmd[0]
		, &g_ShelfLockCmd[1]
		, Null, Null);

	UnitOd_Init(addr, g_ShelfLockOd, SHELF_LOCK_OD_SIZE, odExtension,
		(CO_RPDOMapPar_t*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter, 1, 
		(CO_TPDOMapPar_t*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter, 1,
		(CO_TPDOCommPar_t*)& BASE_CO_OD_ROM.TPDOCommunicationParameter
	);

	return (Rs485ProtocolNode*)g_pShelfLock;
}
