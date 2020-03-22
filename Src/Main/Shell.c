
#include "common.h"
#ifdef CONFIG_CMDLINE
#include "cmdline.h"
#include "Obj.h"
#include "DriverIo.h"
#include "DriverTimer.h"
#include "Master.h"
#include "SegmentProtocol.h"
#include "CANopen.h"
#include "Slave.h"
#include "LogUser.h"
#include "OtaFile.h"

#define SHELL_RX_BUF_SIZE 64
#define DBL_IS_TRUE(dl) ((dl & g_dwDebugLevel) > 0)
#define SET_DBL_BIT(dl, isTrue) \
	if(isTrue)	\
		g_dwDebugLevel |= (1 << GetBitInd(dl));	\
	else	\
		g_dwDebugLevel &= ~(1 << GetBitInd(dl));


static SafeBuf* g_pShellBuf = Null;

void Shell_BuildSFID(uint8 type, uint8 sfid)
{
	CAN_SFID id;
	id.nodeId = sfid;
	id.type = type;
	Printf("CANID=0x%08x (type=0x%x, sfid=0x%x)\n", id.ident, type, sfid);
}

void Shell_BuildEFID(uint8 type, uint8 sfid, uint16 efid)
{
	CAN_EFID id;
	id.sfid = sfid;
	id.efid = efid;
	id.type = type;
	Printf("CANID=0x%08x (type=0x%x, sfid=0x%x, efid=0x%x)\n", id.ident, type, sfid, efid);
}

void Shell_CanIdParser(uint32 ident, Bool isSfid)
{
	if (isSfid)
	{
		CAN_SFID* p = (CAN_SFID*)& ident;
		Printf("CANID=0x%08x (type=0x%x, sfid=%x)\n", ident, p->type, p->nodeId);
	}
	else
	{
		CAN_EFID* p = (CAN_EFID*)& ident;
		Printf("CANID=0x%08x (type=0x%x, sfid=%x, efid=%x)\n", ident, p->type, p->sfid, p->efid);
	}
}
void Shell_EFIDP(uint32 ident) { Shell_CanIdParser(ident, false); }
void Shell_SFIDP(uint32 ident) { Shell_CanIdParser(ident, true); }

void Shell_RPDO(uint8 rpdoInd, uint8 efid, uint8 dlc, uint32 data, uint32 data1)
{
	int copyLen = dlc;

	FrameB rpdo;
	if (rpdoInd >= CO_NO_RPDO)
	{
		Printf("Param rpdoInd (%d) error, must be < %d\n", rpdoInd, CO_NO_RPDO);
		return;
	}
	if (dlc > 8)
	{
		Printf("Param dlc (%d) error, must be < 8\n", dlc);
		return;
	}

	static const uint8 RPDO_TYPE[] = { PDO1rx, PDO2rx, PDO3rx, PDO4rx };
	
	rpdo.len = dlc;

	rpdo.type = RPDO_TYPE[rpdoInd];
	rpdo.efid = efid;
	rpdo.sfid = g_ToggleValue;

	copyLen = (dlc > 4) ? 4 : dlc;
	memcpy(rpdo.data, &data, copyLen);
	dlc -= copyLen;

	copyLen = (dlc > 4) ? 4 : dlc;
	memcpy(rpdo.data, &data1, copyLen);

	DUMP_BYTE_EX(("Ident=0x%08X, dlc=%d, data=", rpdo.ident, rpdo.len), rpdo.data, rpdo.len);

}

void Shell_UploadFile(uint8 addr, char* name)
{
	MasterNode* pNode = Master_FindMasterNode(addr);
	if (pNode == Null)
	{
		Printf("No Node[%X]\n", addr);
		return;
	}

	OtaFile* p = OtaFiles_FindByName(name);
	if (p == Null)
	{
		Printf("No file[%s]\n", name);
		return;
	}

	Printf("Upload %s OD[%04X.%02X], len=0x%X(%d)\n", p->fileName, p->index, p->subIndex, p->fileLen, p->fileLen);
	MasterNode_ReadEntry(pNode, p->index, p->subIndex);
}

void Shell_DownloadFile(uint8 addr, char* name)
{
	MasterNode* pNode = Master_FindMasterNode(addr);
	if (pNode == Null)
	{
		Printf("No Node[%X]\n", addr);
		return;
	}

	OtaFile* p = OtaFiles_FindByName(name);
	if (p == Null)
	{
		Printf("No file[%s]\n", name);
		return;
	}

	Printf("Download %s OD[%04X.%02X], len=0x%X(%d)\n", p->fileName, p->index, p->subIndex, p->fileLen, p->fileLen);
	MasterNode_WriteEntry(pNode, p->index, p->subIndex, Null, 0);
}

void Shell_ReadFile(char* name)
{
	//Printf("CANID=0x%08x (type=0x%x, sfid=0x%x, efid=0x%x)\n", id->ident, id->type, id->sfid, efid);
	OtaFile* p = OtaFiles_FindByName(name);
	if (p == Null)
	{
		Printf("No file[%s]\n", name);
		return;
	}

	OtaFile_Dump(p);
}

void Shell_WriteFile(char* name, uint32 val)
{
	OtaFile* p = OtaFiles_FindByName(name);
	if (p == Null)
	{
		Printf("No file[%s]\n", name);
		return;
	}

	if (!OtaFile_Create(p, val))
	{
		Printf("file[%s] length[%d] error.\n", name, val);
	}
	else
	{
		Printf("file[%s] create success.\n", name);
		OtaFile_Dump(p);
	}
}

void Shell_SetSlaveNmtState(uint8 cs)
{
	CO_OD_RAM.errorRegister = 0;
	CO->NMT->operatingState = cs;
	Printf("Slave Nmt state=0x%x\n", cs);
}

CO_OD_entry_t* Shell_GetOd(uint8 addr, uint16 index, uint8 subInd)
{
	CO_OD_entry_t* pEntry = Null;
	if (addr == CCU_ADDR)
	{
		pEntry = Slave_GetOd(index, subInd);
	}

	MasterNode* pNode = Master_FindMasterNode(addr);
	if (pNode)
	{
		pEntry = MasterNode_GetOd(pNode, index, subInd);
	}
	return pEntry;
}

void Shell_ReadOd(uint8 addr, uint16 index, uint8 subInd)
{
	CO_OD_entry_t* pEntry = Shell_GetOd(addr, index, subInd);
	if (pEntry)
	{
		if (pEntry->pData)
		{
			Entry_Dump(pEntry, addr, Null);
		}
		else
		{
			OtaFile* pFile = OtaFiles_Find(index);
			if (pFile)
			{
				OtaFile_Dump(pFile);
			}
			else
			{
				Printf("Node[%d] File[%04X] not exist.\n", addr, index);
			}
		}
	}
	else
	{
		Printf("Node[%d] OD[%04X.%02X] not found.\n", addr, index, subInd);
	}
}

void Shell_WriteOd(uint8 addr, uint16 index, uint8 subInd, uint32 val)
{
	CO_OD_entry_t* pEntry = Shell_GetOd(addr, index, subInd);
	if (pEntry)
	{
		if (pEntry->attribute & CO_ODA_WRITEABLE)
		{
			if (pEntry->pData)
			{
				Entry_SetValue(pEntry, subInd, (uint8*)& val, 4);
				Entry_Dump(pEntry, addr, Null);
				return;
			}
			else //Domain type
			{
				OtaFile* pFile = OtaFiles_Find(index);
				if (pFile)
				{
					OtaFile_Create(pFile, val);
					return;
				}
				else
				{
					Printf("Node[%d] file[%04X] not exist.\n", addr, index);
				}
			}
		}
		else
		{
			Printf("Node[%d] OD[%04X.%02X] not writeable.\n", addr, index, subInd);
		}
	}
	else
	{
		Printf("No node[%d]\n", addr);
	}
}

void Shell_Run()
{
	static int offset = 0;
	char* buf = (char*)SafeBuf_SearchByte(g_pShellBuf, '\n', &offset);
	if (buf)
	{
		CmdLine_AddStrEx(buf, offset);
		SafeBuf_Read(g_pShellBuf, Null, offset);
		offset = 0;
	}
	//Only for test
	//static Uart* pUart1 = Null;
	//if(pUart1 == Null) Uart_Get(CCU_MISC_COM);
	//g_pShellBuf = &pUart1->rxBuf;
	//buf = (char*)SafeBuf_GetData(g_pShellBuf, '\n', &len);
	//if (buf)
	//{
	//	CmdLine_AddStrEx(buf, len);
	//	SafeBuf_Read(g_pShellBuf, Null, len);
	//}
}

void Shell_SetTestVal(uint32 val)
{
	g_TestFlag = val;
	//MasterNode* pNode = Master_FindMasterNode(PMS_ADDR_0);
	//MasterNode_DumpNeedUpload(pNode);
	Printf("g_TestFlag = 0x%x(%d)\n", g_TestFlag, g_TestFlag)
}

void Shell_Init()
{
	static Obj g_ShellObj;
	static FnDef g_CmdTable[] =
	{
		   {(void*)NVIC_SystemReset		, "Reset()"}
		  ,{(void*)Shell_SetTestVal		, "SetTestVal(uint32 val)"}
		#ifdef XDEBUG
		  ,{(void*)Debug_SetBit  , "Debug_SetBit(int bitInd, Bool isEnable)"}
		  ,{(void*)Debug_SetLevel, "Debug_SetLevel(uint8 level)"}
		#endif
		  ,{(void*)LogUser_DumpByCount 	,"Log(int count, uint8 objId)"}
		  ,{(void*)LogUser_DumpByInd  	,"LogInd(int ind, int count, uint8 objId)"}

		  ,{(void*)IODesc_Set,  "SetIO(const char* Pxx, uint8 value)"}
		  ,{(void*)IODesc_Read, "ReadIO(char* Pxx)"}
		  ,{(void*)IO_Read, "ReadIO2(uint8 pin)"}
		  ,{(void*)Master_SetNmtState, "SetMasterNmt(uint8 addr, uint8 cs)"}
		  ,{(void*)Shell_SetSlaveNmtState, "SetSlaveNmt(uint8 state)"}
		  ,{(void*)Master_SetNodeValue, "SetNodeValue(uint8 addr, int index, uint32 val)"}

		  ,{(void*)Shell_BuildSFID, "SFID(uint8 type, uint8 sfid)"}
		  ,{(void*)Shell_BuildEFID, "EFID(uint8 type, uint8 sfid, uint16 efid)"}
		  ,{(void*)Shell_EFIDP, "EFIDP(uint32 ident)"}
		  ,{(void*)Shell_SFIDP, "SFIDP(uint32 ident)"}
		  ,{(void*)Shell_ReadFile,	"ReadFile(char* name)"}
		  ,{(void*)Shell_WriteFile,	"WriteFile(char* name, uint32 val)"}
		  ,{(void*)Shell_UploadFile,	"UploadFile(uint8 addr, char* name)"}
		  ,{(void*)Shell_DownloadFile,	"DownloadFile(uint8 addr, char* name)"}
		  ,{(void*)Shell_RPDO,		"RPDO(uint8 rpdoInd, uint8 efid, uint8 dlc, uint32 data, uint32 data1)"}
		  ,{(void*)Shell_WriteOd,	"WriteOd(uint8 addr, uint16 index, uint8 subInd, uint32 val)"}
		  ,{(void*)Shell_ReadOd,	"ReadOd(uint8 addr, uint16 index, uint8 subInd)"}
		  ,{(void*)OtaFile_Tester,	"OtaFile(char* name, Bool isWrite, uint32 fileLen)"}

#ifdef CFG_EX_FRAME
		  ,{(void*)UnitOd_Dump	,	"UnitOd_Dump()"}
		  ,{(void*)UnitOd_DumpTpdo,	"DumpTPDO(uint8 addr, uint8 pdoInd)"}
		  ,{(void*)UnitOd_DumpRpdo,	"DumpRPDO(uint8 addr, uint8 pdoInd)"}
#endif
		  ,{(void*)Slave_Dump	,	"Slave_Dump()"}
		  ,{(void*)Master_Dump	,	"Master_Dump(uint8 addr)"}
	};
	
	CmdLine_Init(g_CmdTable, GET_ELEMENT_COUNT(g_CmdTable), True);

	Obj_Register(&g_ShellObj, "Shell", Null, Null, Shell_Run);

	{
		Uart* pUart = Uart_Get(CCU_DEBUG_COM);
		Assert(pUart);
		g_pShellBuf = &pUart->rxBuf;
	}
}


#endif

