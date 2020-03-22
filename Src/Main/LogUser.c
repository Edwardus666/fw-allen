
#include "Common.h"
#include "LogUser.h"
#include "Record.h"
#include "Log.h"
#include "Master.h"

/********************************************************
日志
*******************************************************/

LogItem* g_pLogItem = Null;
static Record g_LogRecord;

#define BOM_NAME "1=BOM1|2=BOM2"

//sample: str = "BOM ID[1], FAULT[%8-15B]"
//char* str_Insert(char* srcStr, const char* fmt, const char* insertStr)
//{
//
//}
//char* LogEvent_BOM_FAULT_Fmt(char* str)
//{
//	struct
//	{
//		int ind;
//		int desc;
//	}
//	static const map[] = 
//	{
//		{1, "COM1"}
//	};
//
//	int val = 0;
//	int startInd = -1;
//	int endInd = -1;
//
//	for (int i = 0; str[i] != 0; i++)
//	{
//		if (str[i] == '[') startInd = i;
//		if (str[i] == ']')
//		{
//			Assert(startInd != -1);
//			endInd = i;
//			break;
//		}
//	}
//
//	if (sscanf(str[startInd], "[%x]", &val) == 1)
//	{
//		for (int i = 0; i < GET_ELEMENT_COUNT(map); i++)
//		{
//			if (map[i].ind == val)
//			{
//				memcpy();
//				break;
//			}
//		}
//	}
//	return str;
//}

const static LogEventMap g_pStrLogEvent[] =
{
	 {ET_CCU_RESET  , "CCU_RESET"},
	 {ET_STATE_SWITCH , "OPERATION-STATE"},
	 {ET_CCU_BOM_FAULT, "BOM-FAULT", "BOM ID[%0-7BX], FAULT[%8-15B]"},

	 {ET_FW_UPGRADE , "FW_UPGRADE", "Index[%16-31BX.%8-15BX],isRead[%1B],Result[%0B]"},

	 {ET_PMS_STATE1 , "PMS_STATE1", "funPower[%6B],doorLockState[%5B],bolt[%3-4B],doorLock[%2B],chargerIn[%1B],batIn[%0B]"},
	 {ET_PMS_STATE2 , "PMS_STATE2", "B-Verify[%5B],P-Verify[%4B],B-Power[%3B],R-Power[%2B],chargerPower[%1B],chargeEnable[%0B]"},

	 {ET_FSM_SWITCH , "FSM-STATE"},

	 {ET_SHELF_LOCK_LOCK_STATE_CHANGED , "LOCK_STATE_CHANGED"},
	 {ET_SHELF_LOCK_OP_STATE_CHANGED , "OP_STATE_CHANGED"},

	 {ET_METER_OP_STATE_CHANGED	  , "OP_STATE_CHANGED"},
};

const char* EventId_ToStr(uint8 eventId)
{
	static char str[32];
	int i = 0;

	for (i = 0; i < GET_ELEMENT_COUNT(g_pStrLogEvent); i++)
	{
		if (eventId == g_pStrLogEvent[i].eventId) return g_pStrLogEvent[i].pStr;
	}

	sprintf(str, "%02X", eventId);
	return str;
}

const char* ObjId_ToStr(uint8 objId)
{
	static char str[16];
	
	if(objId == OBJID_CCU)
	{
		return "CCU";
	}
	else if (objId == OBJID_SLAVE)
	{
		return "SLAVE";
	}
	else
	{
		MasterNode* p = Master_FindMasterNode(objId);
		if (p && p->addr >= PMS_ADDR_0 && p->addr <= PMS_ADDR_14)
		{
			sprintf(str, "%s[%02X]", p->name, objId);
		}
		else
		{
			sprintf(str, "OBJ[%02X]", objId);
		}
	}

	return str;
}

const char* LogVal_ToStr(uint8 eventId, uint32 val)
{
	uint8 bites;
	const char* fmt = "0x%08X (%d)";
	static char str[256];
	int i = 0;
	const LogEventMap* pLogEvent = g_pStrLogEvent;
	for (i = 0; i < GET_ELEMENT_COUNT(g_pStrLogEvent); i++, pLogEvent++)
	{
		if (eventId == pLogEvent->eventId)
		{
			if (pLogEvent->pFmt)
			{
				fmt = pLogEvent->pFmt;
			}
			break;
		}
	}

	char buf[128]; Assert(strlen(fmt) < sizeof(buf));

	//sample: fmt = "BOM ID[%0-7BX], FAULT[%8-15B]"
	if(SprintfBit(buf, fmt, val, &bites))
	{
		if (bites > 24)      fmt = "0x%08X (%s)";
		else if (bites > 16) fmt = "0x%06X (%s)";
		else if (bites > 8)  fmt = "0x%04X (%s)";
		else                 fmt = "0x%02X (%s)";
		sprintf(str, fmt, val, buf);
	}
	else
	{
		sprintf(str, fmt, val, val);
	}

	if (i < GET_ELEMENT_COUNT(g_pStrLogEvent))
	{
		if (pLogEvent->Format) pLogEvent->Format(str);
	}

	return str;
}

void LogUser_DumpByCount(int count, uint8 objId)
{
	Record_DumpByCount(&g_LogRecord, count, objId);
}

void LogUser_DumpByInd(int ind, int count, uint8 objId)
{
	Record_DumpByInd(&g_LogRecord, ind, count, objId);
}


void LogUser_Write(uint8 moduleId, LogType logType, uint8 eventId, uint32 val)
{
	LogItem item;
	LogItem* p = g_pLogItem;
	if (g_pLogItem == Null)	//g_pLogItem 没被初始化
	{
		p = &item;
	}

	Log_Init(p, moduleId, logType, eventId, val);

	if (g_dwDebugLevel & DL_LOG)
	{
		Printf("%04d:", g_LogRecord.itemCount);
		Log_Dump(p, Null, Null);
	}

	if (g_pLogItem)
	{
		Record_Write(&g_LogRecord, p);
	}

}

//检验LOG的内容是否过时，如果版本号不一致，说明数据过时，会被丢弃
Bool LogUser_Verify(const LogItem* pItem, uint32 pParam)
{
	return (pItem->version == LOG_VERSION && pItem->tail == LOG_LATEST_BYTE);
}

void LogUser_Init()
{
	static LogItem	g_LogItem;
	g_LogItem.version = LOG_VERSION;
	g_LogItem.tail = LOG_LATEST_BYTE;

	//初始化日志记录模块
	g_pLogItem = &g_LogItem;
	Record_Init(&g_LogRecord
		, BANK1_SECTOR_SIZE
		, LOG_AREA_SIZE / BANK1_SECTOR_SIZE
		, (void*)g_pMemMap->logArea
		, g_pLogItem
		, sizeof(LogItem)
		, (RecordVerifyItemFn)LogUser_Verify);
}

//以下代码仅用于LogUser模块测试
#if 0
void LogUser_TestInit(int sectorCount)
{
	static LogItem	g_LogItem;
	g_LogItem.version = LOG_VERSION;
	g_LogItem.tail = LOG_LATEST_BYTE;

	//初始化日志记录模块
	g_pLogItem = &g_LogItem;
	Record_Init(&g_LogRecord
		, BANK1_SECTOR_SIZE
		, BANK1_PAGE_SIZE
		, 2
		, (void*)g_pMemMap->logArea
		, g_pLogItem
		, sizeof(LogItem)
		, (RecordVerifyItemFn)LogUser_Verify);
}

//测试2个扇区
void LogUser_TesterFor2Sec()
{
	int i = 0;
	int size = 4096 / sizeof(LogItem);
	Record_RemoveAll(&g_LogRecord);

	LogUser_TestInit(2);

	//第0扇区：写g_LogRecord.itemsPerSec - 1个元素
	size = g_LogRecord.itemsPerSec - 1;
	for (i = 0; i < size; i++)
	{
		LogUser_Write(1, 1, 1, i);
	}
	Assert(g_LogRecord.readPointer == 0);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);

	LogUser_TestInit(2);
	Assert(g_LogRecord.readPointer == 0);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);

	//第0扇区：写1个元素，第0扇区满
	LogUser_TestInit(2);
	LogUser_Write(1, 1, 1, i++);
	size++;
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 1);
	Assert(g_LogRecord.totalCount == size);

	LogUser_TestInit(2);
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 1);
	Assert(g_LogRecord.totalCount == size);

	//第1扇区：写g_LogRecord.itemsPerSec个元素，满
	size += g_LogRecord.itemsPerSec;
	for (; i < size; i++)
	{
		LogUser_TestInit(2);
		LogUser_Write(1, 1, 1, i);
	}

	size -= g_LogRecord.itemsPerSec;
	i -= g_LogRecord.itemsPerSec;
	Assert(g_LogRecord.readStartSectorInd == 1);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);

	Printf("LogUser test passed\n.");
}

//测试3个扇区
void LogUser_TestFor3Sect()
{
	Record_RemoveAll(&g_LogRecord);

	LogUser_TestInit(3);
	int i = 0;
	int size = 4096 / sizeof(LogItem);
	Assert(g_LogRecord.itemsPerSec == size);

	//第0扇区：写g_LogRecord.itemsPerSec - 1个元素
	size = g_LogRecord.itemsPerSec - 1;
	for (i = 0; i < size; i++)
	{
		LogUser_Write(1, 1, 1, i);
	}
	Assert(g_LogRecord.readPointer == 0);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);

	LogUser_TestInit(3);
	Assert(g_LogRecord.readPointer == 0);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);

	//第0扇区：写1个元素，第0扇区满
	LogUser_TestInit(3);
	LogUser_Write(1, 1, 1, i++);
	size++;
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 1);
	Assert(g_LogRecord.totalCount == size);

	LogUser_TestInit(3);
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 1);
	Assert(g_LogRecord.totalCount == size);

	//第1扇区：写g_LogRecord.itemsPerSec个元素，满
	size += g_LogRecord.itemsPerSec;
	for (; i < size; i++)
	{
		LogUser_TestInit(3);
		LogUser_Write(1, 1, 1, i);
	}
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 2);
	Assert(g_LogRecord.totalCount == size);

	//第2扇区：写g_LogRecord.itemsPerSec-1个元素
	LogUser_TestInit(3);
	size += g_LogRecord.itemsPerSec-1;
	for (; i < size; i++)
	{
		LogUser_Write(1, 1, 1, i);
	}
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 2);
	Assert(g_LogRecord.totalCount == size);

	//第2扇区：写1一个元素，第2扇区满
	LogUser_Write(1, 1, 1, i++);
	size++;

	size -= g_LogRecord.itemsPerSec;
	i -= g_LogRecord.itemsPerSec;
	Assert(g_LogRecord.readStartSectorInd == 1);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);

	LogUser_TestInit(3);
	Assert(g_LogRecord.readStartSectorInd == 1);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);
	
	//第0扇区：写g_LogRecord.itemsPerSec - 1个元素
	size += g_LogRecord.itemsPerSec - 1;
	for (; i < size; i++)
	{
		LogUser_TestInit(3);
		LogUser_Write(1, 1, 1, i);
	}
	Assert(g_LogRecord.readStartSectorInd == 1);
	Assert(g_LogRecord.writeSectorInd == 0);
	Assert(g_LogRecord.totalCount == size);
	
	//第0扇区：写1个元素，满
	LogUser_TestInit(3);
	LogUser_Write(1, 1, 1, i++);
	size++;
	
	size -= g_LogRecord.itemsPerSec;
	i -= g_LogRecord.itemsPerSec;
	Assert(g_LogRecord.readStartSectorInd == 2);
	Assert(g_LogRecord.writeSectorInd == 1);
	Assert(g_LogRecord.totalCount == size);
	
	LogUser_TestInit(3);
	Assert(g_LogRecord.readStartSectorInd == 2);
	Assert(g_LogRecord.writeSectorInd == 1);
	Assert(g_LogRecord.totalCount == size);
	
	//第1扇区：写g_LogRecord.itemsPerSec-1元素
	size += g_LogRecord.itemsPerSec-1;
	for (; i < size; i++)
	{
		LogUser_TestInit(3);
		LogUser_Write(1, 1, 1, i);
	}
	Assert(g_LogRecord.readStartSectorInd == 2);
	Assert(g_LogRecord.writeSectorInd == 1);
	Assert(g_LogRecord.totalCount == size);

	//第1扇区：写1个元素，满
	LogUser_TestInit(3);
	LogUser_Write(1, 1, 1, i++);
	size++;
	size -= g_LogRecord.itemsPerSec;
	i -= g_LogRecord.itemsPerSec;
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 2);
	Assert(g_LogRecord.totalCount == size);

	LogUser_TestInit(3);
	Assert(g_LogRecord.readStartSectorInd == 0);
	Assert(g_LogRecord.writeSectorInd == 2);
	Assert(g_LogRecord.totalCount == size);

	Printf("LogUser test passed\n.");
}

#endif
