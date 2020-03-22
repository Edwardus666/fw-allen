
#include "common.h"
#include "Log.h"
#include "Time.h"

#ifdef CFG_LOG

const char* g_LogTypeStr[] = { "TRACE","WARNING","ERROR" };

time_t __attribute__((weak)) DateTime_GetCurrentSeconds()
{
	return 0;
	//return time(Null);
}

const char* __attribute__((weak)) DateTime_FmtStr(uint32 seconds)
{
	//2019-12-16 11:32:20
	static char timeStr[20];
	struct tm* pTm = localtime((time_t*)&seconds);
	strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", pTm);
	return timeStr;
}

const char* __attribute__((weak)) ObjId_ToStr(uint8 objId)
{
	static char str[5];

	sprintf(str, "OBJ[%d]", objId);

	return str;
}

const char* __attribute__((weak)) EventId_ToStr(uint8 eventId)
{
	static char str[5];

	sprintf(str, "%d", eventId);

	return str;
}

const char* __attribute__((weak)) LogVal_ToStr(uint8 eventId, uint32 val)
{
	static char str[25];

	sprintf(str, "0x%08X (%d)", val, val);

	return str;
}

const char* __attribute__((weak)) LogType_ToStr(LogType type)
{
	if (type == LT_WARNING) return "WARNING ";
	if (type == LT_ERROR) return "ERROR ";
	return "";
}

//把整数按位解析打印到字符串中
//str = "batIsIn[%0Bx],chargerIsIn[%0-1BX]..."
int SprintfBit(char buf[], const char* str, uint32 val, uint8* numberOfBit)
{
	Bool isMatch = False;
	uint8 byte,byte1 = 0;
	uint8 bits = 0;
	int v1, v2, len = 0;
	const char* s = str;
	const char* p = strstr(str, "%");

	while (p)
	{
		static int8 strFmt[] = { '%', 'X', 0 };
		memcpy(&buf[len], s, (uint32)p - (uint32)s);
		len += (uint32)p - (uint32)s;
		s = p;

		p++;
		if ((3 == sscanf(p, "%d%c%c", &v1, &byte, &byte1)) && (byte == 'B' || byte == 'b'))	//Search string "%0B"
		{
			strFmt[1] = (byte1 == 'X' || byte1 == 'x') ? byte1 : 'd';

			len += sprintf(&buf[len], strFmt, GET_BITS(val, v1, v1));
			isMatch = True;
			s = strstr(p, "B") + 1;
			bits = MAX(bits, v1);
		}
		else if ((4 == sscanf(p, "%d-%d%c%c", &v1, &v2, &byte, &byte1)) && (byte == 'B' || byte == 'b'))	//Search string "%0-1B"
		{
			strFmt[1] = (byte1 == 'X' || byte1 == 'x') ? byte1 : 'd';
			if (v1 > v2)
			{
				int temp = v1;
				v1 = v2;
				v2 = temp;
			}
			
			len += sprintf(&buf[len], strFmt, GET_BITS(val, v1, v2));
			isMatch = True;
			s = strstr(p, "B") + 1;
			bits = MAX(bits, v1);
			bits = MAX(bits, v2);
		}

		//Search next "%..B"
		p = strstr(p, "%");
	}

	//Copy tail of string to buff
	p = (char*)(str + strlen(str));
	memcpy(&buf[len], s, (uint32)p - (uint32)s);
	len += (uint32)p - (uint32)s;
	buf[len++] = 0;

	if (numberOfBit) *numberOfBit = bits;
	return isMatch ? len : 0;
}
void Log_Dump(LogItem* pItem, const char* head, const char* tail)
{
	if (head == Null) head = "";
	if (tail == Null) tail = "";
	//uint32* pAsUint32 = (uint32*)pItem->data;

	Printf("%s[%s]%s %s.%s=%s%s\n"
		, head
		, DateTime_FmtStr(pItem->dateTime)
		, LogType_ToStr(pItem->logType)
		, ObjId_ToStr(pItem->objId)
		, EventId_ToStr(pItem->eventId)
		, LogVal_ToStr(pItem->eventId, *(uint32*)pItem->data)
		, tail
	);
}


void Log_Init(LogItem* item, uint8 objId, LogType logType, uint8 eventId, uint32 val)
{
	item->dateTime = DateTime_GetCurrentSeconds();
	item->objId = objId;
	item->eventId = eventId;
	item->logType = logType;
	memcpy(item->data, &val, 4);
}

#endif

