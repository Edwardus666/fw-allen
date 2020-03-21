#ifndef __RECORD_H_
#define __RECORD_H_

#ifdef __cplusplus
extern "C"{
#endif

#ifdef CFG_LOG
	
#include "SectorMgr.h"
	
#define RECORD_INVALID_ADD 0xFFFFFFFF

typedef Bool (*RecordVerifyItemFn)(const void* pItem, uint32 pParam);

typedef struct _Record
{
	uint32 startAddr;		//��������ʼ��ַ�����Գ�ʼ������Record_Init()
	uint16  sectorCount;	//�������������Գ�ʼ������Record_Init()
		
	uint32 itemCount;			//��Ч��Ԫ������
	uint16 writeSectorInd;		//д������
	uint16 readStartSectorInd;	//��������

	uint32 	maxItems;		//���Ԫ������������������С��Ԫ�ش�С�������
	int 	itemsPerSec;	//ÿ������Ԫ������������������С��Ԫ�ش�С�������
	
	uint32 	readPointer;	//����¼�α�ָ��	

	SectorMgr sector;
}Record;

void Record_Seek(Record* pRecord, uint32 pos);
void Record_Init(Record* pRecord, uint16 sectorSize, uint16 sectorCount
	, void* pFlashAddr, void* pValue, int itemSize, RecordVerifyItemFn verify);

Bool Record_Write(Record* pRecord, void* pRecData);
int Record_Read(Record* pRecord, void* pBuf, int buflen);
void Record_Dump(Record* pRecord);
void Record_RemoveAll(Record* pRecord);
uint32 Record_CalcuReadSecAddr(Record* pRecord, uint32 itemInd, uint16* sec, uint32* addrOfSecOffset);
Bool Record_isValid(Record* pRecord);
void Record_ConvertSector(Record* pRecord, uint16* sec, int* ind);

void Record_DumpByCount(Record* pRecord, int count, uint8 objId);
void Record_DumpByInd(Record* pRecord, int ind, int count, uint8 objId);
void Record_DumpByTimeStr(Record* pRecord, const char* pDataTime, int count);
int Record_Find(Record* pRecord, int startInd, LogItemFilterFn verify, uint32 param);
void Record_DumpByTime(Record* pRecord, uint32 seconds, int count);

#endif

#ifdef __cplusplus
}
#endif

#endif


