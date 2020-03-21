#ifndef __LOG_ITEM_H_
#define __LOG_ITEM_H_

#ifdef __cplusplus
extern "C"{
#endif

#ifdef CFG_LOG

#define LOG_INVALID_IND 0xFFFFFFFF
#define LOG_LATEST_BYTE 0x5A
#define LOG_ALL_OBJ_ID	0xFF

typedef Bool(*LogItemFilterFn)(void* pItem, uint32 pParam);

typedef enum _LogType
{
	  LT_TRACE
	, LT_WARNING
	, LT_ERROR
}LogType;

typedef const char* (*LogFmtFn)(const char* str);

typedef struct _LogEventMap
{
	uint8	eventId;	//
	const char* pStr;
	const char* pFmt;
	LogFmtFn Format;
}LogEventMap;

#pragma pack(1) 
//�ýṹ���ֽ��ձ���ż��������ᵼ�´洢�����½�
typedef struct _LogItem
{	
	time_t dateTime;

	uint8 moduleId:3;
	uint8 reserved : 2;
	uint8 version : 1;
	uint8 logType:2;

	uint8 eventId;	//�¼�ID
	uint8 objId;	//����ID����¼������log�Ķ���

	uint8 data[4];	//�¼�����
	uint8 tail;
}LogItem;
#pragma pack() 

void Log_Dump(LogItem* pItem, const char* head, const char* tail);

void Log_Init(LogItem*  pItem, uint8 objId, LogType logType, uint8 id, uint32 val);
int SprintfBit(char buf[], const char* str, uint32 val, uint8* numberOfBit);


#else
#define Log_Init(...)
#define LOG1(...)
#define LOG2(...)
#define Log_RemoveAll(...)
#endif

#ifdef __cplusplus
}
#endif

#endif

