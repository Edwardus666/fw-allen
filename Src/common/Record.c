#include "common.h"
#include "Record.h"

#ifdef CFG_LOG
extern Bool FmcRead(uint32 addr, void* pData, int len);
extern Bool FmcWrite(uint32 addr, void* pData, int len);
extern Bool FmcErase(uint32 addr, uint32 dataLength);

void Record_DumpByCount(Record* pRecord, int count, uint8 objId)
{
	int ind = pRecord->itemCount - count;
	if (ind < 0) ind = 0;

	Record_DumpByInd(pRecord, ind, count, objId);
}

void Record_DumpByInd(Record* pRecord, int ind, int count, uint8 objId)
{
	LogItem item = { 0 };

	Record_Seek(pRecord, ind);

	while (Record_isValid(pRecord) && count--)
	{
		if (Record_Read(pRecord, &item, sizeof(LogItem)) && (item.objId == objId || objId == LOG_ALL_OBJ_ID))
		{
			if (item.eventId == ET_CCU_RESET) Printf("\n");
			Printf("%04d:", pRecord->readPointer - 1);
			Log_Dump(&item, Null, Null);
		}
	}
}

//2008/04/03 11:12:00
void Record_DumpByTimeStr(Record* pRecord, const char* pDataTime, int count)
{
	int year, month, day, hour, minute, sec;
	if (6 == sscanf(pDataTime, "%d/%d/%d %d:%d:%d", &year, &month, &day, &hour, &minute, &sec))
	{
		uint32 seconds = DateTime_GetSeconds(DateTime_MakeRtc(year, month, day, hour, minute, sec));
		Record_DumpByTime(pRecord, seconds, count);
	}
	else
	{
		Printf("Date time format error\n");
	}
}

Bool Record_FilterTime(LogItem * pItem, uint32 param)
{
	return pItem->dateTime >= param;
}

int Record_Find(Record* pRecord, int startInd, LogItemFilterFn verify, uint32 param)
{
	int i = 0;
	LogItem item = { 0 };

	Record_Seek(pRecord, startInd);

	for (i = startInd; i < pRecord->itemCount; i++)
	{
		if (Record_Read(pRecord, &item, sizeof(LogItem)))
		{
			if (verify(&item, param))
			{
				return i;
			}
		}
		else
		{
			break;
		}
	}

	return LOG_INVALID_IND;
}

void Record_DumpByTime(Record* pRecord, uint32 seconds, int count)
{
	int ind = Record_Find(pRecord, 0, (LogItemFilterFn)Record_FilterTime, seconds);

	Record_DumpByInd(pRecord, ind, count, LOG_ALL_OBJ_ID);
}

void Record_Dump(Record* pRecord)
{
	Printf("Record Dump:\n");
	
	Printf("\t startAddr=%d.\n"		, pRecord->startAddr);
	Printf("\t sectorCount=%d.\n"	, pRecord->sectorCount);
	
	Printf("\t total=%d.\n"			, pRecord->itemCount);
	
	Printf("\t writeSectorInd=%d.\n", pRecord->writeSectorInd);
	Printf("\t readPointer=%d.\n"	, pRecord->readPointer);
	
	SectorMgr_Dump(&pRecord->sector);
}

/*
Bool Record_WritePointerUpdate(Record* pRecord)
{
	uint32 sectorAddr = 0;
	Bool isFull = SectorMgr_isFull(&pRecord->recordMgr);
	
	if(isFull)
	{
		//
		if(pRecord->writeSectorInd + 1 >= pRecord->sectorCount)
		{
			pRecord->writeSectorInd = 0;
			pRecord->total -= pRecord->recordMgr.m_ItemCount;
		}
		else
		{
			pRecord->writeSectorInd++;
		}
		
		Assert(pRecord->total >= pRecord->recordMgr.m_ItemCount);
		
		sectorAddr = pRecord->startAddr + pRecord->writeSectorInd * SECTOR_SIZE;
		FmcErase(sectorAddr);
		
	}
	
	return isFull;
}
*/

/*����:ת���������Ϊʵ������
����˵��:
	sec:����ֵ��
	ind:��������Լ�¼λ��
*/
void Record_ConvertSector(Record* pRecord, uint16* sec, int* ind)
{
	int logInd = * ind;
	if(pRecord->readStartSectorInd > *sec)
	{
		logInd -= (pRecord->readStartSectorInd - *sec) * pRecord->itemsPerSec;
	}
	else if(pRecord->readStartSectorInd < *sec)
	{
		logInd += (*sec - pRecord->readStartSectorInd) * pRecord->itemsPerSec;
	}
	*sec = pRecord->readStartSectorInd;

	if(logInd < 0) logInd = 0;
	if(logInd > pRecord->itemCount) logInd = pRecord->itemCount;
	
	*ind = logInd;
}

Bool Record_isValid(Record* pRecord)
{
	return pRecord->itemCount > 0 && pRecord->readPointer < pRecord->itemCount;
}

int Record_GetTotal(Record* pRecord)
{
	return pRecord->itemCount;
}

/*�������ܣ����������ź�������ƫ�Ƶ�ַ
	����:itemInd,Ԫ������λ��.
	����:sec,������.
	����:addrOfSecOffset,�����ڲ�ƫ�Ƶ�ַ.
	����ֵ:���Ե�ַ.
*/
uint32 Record_CalcWriteSecAddr(Record* pRecord, uint32 itemInd, uint16* sec, uint32* addrOfSecOffset)
{
	uint32 addr = 0;
	int offsetSec = 0;
	
	if(itemInd >= pRecord->maxItems) return RECORD_INVALID_ADD;

	//��ǰ����������������
	if(pRecord->itemCount == pRecord->maxItems)
	{
		offsetSec = (pRecord->writeSectorInd + 1) % pRecord->sectorCount;
	}
	else if(pRecord->itemCount > pRecord->maxItems - pRecord->itemsPerSec)
	{
		offsetSec = pRecord->writeSectorInd;
	}
	else
	{
		offsetSec = itemInd / pRecord->itemsPerSec;
	}
	
	addr = (itemInd % pRecord->itemsPerSec) * pRecord->sector.m_itemSize;

	if(sec) *sec = offsetSec;
	if(addrOfSecOffset) *addrOfSecOffset = addr;
	
	return pRecord->startAddr + (offsetSec * pRecord->sector.m_sectorSize) + addr;
}

Bool Record_Write(Record* pRecord, void* pRecData)
{
	uint32 addrOfSec = 0;
	memcpy(pRecord->sector.m_pItem, pRecData, pRecord->sector.m_itemSize);

	if(pRecord->sectorCount == 0) 	//��û�г�ʼ�����
		return False;
	
	if(SectorMgr_Write(&pRecord->sector))
	{
		pRecord->itemCount++;
	}
	
	uint16 sectorInd = pRecord->writeSectorInd;
	uint32 addr = Record_CalcWriteSecAddr(pRecord, pRecord->itemCount % pRecord->maxItems, &pRecord->writeSectorInd, &addrOfSec);
	if(addr == RECORD_INVALID_ADD) return False;
	if (pRecord->writeSectorInd != sectorInd)	//��ǰ����д��,�л�����һ���µ�����
	{
		//�л�����һ���µ�����
		pRecord->sector.m_startAddr = addr;
		SectorMgr_Reset(&pRecord->sector);
		if(pRecord->itemCount >= pRecord->maxItems)	
		{
			//��������������
			SectorMgr_Erase(&pRecord->sector);
			//������ЧԪ������
			pRecord->itemCount -= pRecord->itemsPerSec;
			//�ƶ���ָ���α꣬�����ǰ��ָ��ɾ����������Ҫ�ƶ�����һ���ɶ�λ��
			pRecord->readPointer = (pRecord->readPointer > pRecord->itemsPerSec) ? (pRecord->readPointer-pRecord->itemsPerSec) : 0;
		}
		
		//���¶���ʼ������
		Record_CalcuReadSecAddr(pRecord, 0, &pRecord->readStartSectorInd, Null);
	}
	return True;
}

/*�������ܣ����������ź�������ƫ�Ƶ�ַ
	����:itemInd,Ԫ������λ��.
	����:sec,������.
	����:addrOfSecOffset,�����ڲ�ƫ�Ƶ�ַ.
	����ֵ:���Ե�ַ.
*/
uint32 Record_CalcuReadSecAddr(Record* pRecord, uint32 itemInd, uint16* sec, uint32* addrOfSecOffset)
{
	uint32 addr = 0;
	uint8 offsetSec = 0;
	
	if(itemInd >= pRecord->maxItems) return RECORD_INVALID_ADD;

	if(pRecord->itemCount >= pRecord->maxItems - pRecord->itemsPerSec)	//ȫ�����������ݶ�����Ч��
	{
		offsetSec = pRecord->writeSectorInd + 1;	//��ָ��Ϊ��дָ�����һ������
	}
	else
	{
		offsetSec = 0;
	}
	
	offsetSec = (offsetSec + itemInd / pRecord->itemsPerSec) % pRecord->sectorCount;
	addr = (itemInd % pRecord->itemsPerSec) * pRecord->sector.m_itemSize;

	if(sec) *sec = offsetSec;
	if(addrOfSecOffset) *addrOfSecOffset = addr;

	return pRecord->startAddr + (offsetSec * pRecord->sector.m_sectorSize) + addr;
}

void Record_Seek(Record* pRecord, uint32 pos)
{
	pRecord->readPointer = (pos > pRecord->itemCount) ? pRecord->itemCount : pos;
}

int Record_Read(Record* pRecord, void* pBuf, int buflen)
{
	int i = 0;
	int readLen = 0;
	uint8* pByte = (uint8*)pBuf;
	uint32 addr = 0;
	uint16 itemSize = pRecord->sector.m_itemSize;

	for(i = pRecord->readPointer
		; i < pRecord->itemCount && buflen >= itemSize
		; i++)
	{
		addr = Record_CalcuReadSecAddr(pRecord, i, Null, Null);
		if(addr == RECORD_INVALID_ADD) break;
		
		FmcRead(addr, &pByte[readLen], itemSize);
		readLen += itemSize;
		buflen-= itemSize;
	}

	pRecord->readPointer = i;
	return readLen;
}

void Record_RemoveAll(Record* pRecord)
{
	int i = 0;
	if(pRecord->sectorCount == 0) 	//��û�г�ʼ�����
		return;
	uint16 SECTOR_SIZE = pRecord->sector.m_sectorSize;
	//������������
	for(i = 0; i < pRecord->sectorCount; i++)
	{
		FmcErase(pRecord->startAddr + i * SECTOR_SIZE, SECTOR_SIZE);
	}

	//���³�ʼ������������
	pRecord->writeSectorInd = 0;
	pRecord->itemCount = 0;
	pRecord->readPointer = 0;
	pRecord->readStartSectorInd = 0;
	SectorMgr_Reset(&pRecord->sector);
}

void Record_InitSectorMgr(SectorMgr* pSector, uint16 sectorSize
	, uint32 startAddr, void* pItem, uint16 itemSize, RecordVerifyItemFn verify)
{
	Bool bRet = True;
	uint8 temp[32];
	
	ASRT(itemSize < sizeof(temp));
	
	//����Ĭ��ֵ
	memcpy(temp,  pItem, itemSize);
	
	//��Flash��ȡ����ֵ
	bRet = SectorMgr_Init(pSector, sectorSize, startAddr, pItem , itemSize);
	
	//�������������Ƿ���Ч�����У��ʧ�ܣ�˵�����ݹ�ʱ�������
	if(bRet && verify && !verify(pItem, 0))	//У��汾��
	{
		//Printf("Ver is not match.\n");
		SectorMgr_Erase(pSector);
		//�ָ�Ĭ��ֵ
		memcpy(pItem, temp, itemSize);
	}
}

void Record_Init(Record* pRecord, uint16 sectorSize, uint16 sectorCount
	, void* pFlashAddr, void* pItem, int itemSize, RecordVerifyItemFn verify)
{
	int i = 0;
	int writeSecInd = -1;

	Assert((uint32)pFlashAddr % sectorSize == 0);

	memset(pRecord, 0, sizeof(Record));

	pRecord->itemsPerSec = sectorSize /itemSize;
	pRecord->maxItems = sectorCount * pRecord->itemsPerSec;

	pRecord->startAddr = (uint32)pFlashAddr;
	//pRecord->pItem 	   = pValue;
	//pRecord->itemSize  = itemSize;
	pRecord->sectorCount = sectorCount;

	//��������������ͳ����Ч�ļ�¼����, �ҳ���д������
	SectorMgr* pMgr = &pRecord->sector;
	for(i = 0; i < sectorCount; i++)
	{
		uint32 addr = pRecord->startAddr + i * sectorSize;
		Record_InitSectorMgr(pMgr, sectorSize, addr, pItem, itemSize, verify);

		//����м���һ������û��д������֮�������������Ҫ��ȫ����Ҫ��ȫ��
		if(!SectorMgr_isFull(pMgr))
		{		
			if(writeSecInd < 0)	//��һ��
			{
				writeSecInd = i;
			}
			else if(pMgr->m_ItemCount > 0)	//��������Ԥ�ڵĴ���ɾ���������־���ݡ�
			{
				for(int j =  writeSecInd + 1; j < sectorCount; j++)
				{
					addr = pRecord->startAddr + j * sectorSize;
					FmcErase(addr, sectorSize);
				}
				goto End;
			}
		}
		pRecord->itemCount += pMgr->m_ItemCount;
	}
	
	//�Ҳ�����д����
	if(writeSecInd == -1) 
	{
		writeSecInd = 0;
		if(pRecord->itemCount)	//��������Ԥ�ڵĴ���
		{
			Record_RemoveAll(pRecord);
		}
	}
	End:
	pRecord->writeSectorInd = writeSecInd;
	Record_InitSectorMgr(pMgr, sectorSize, pRecord->startAddr + pRecord->writeSectorInd * sectorSize, pItem, itemSize, verify);
	Record_CalcuReadSecAddr(pRecord, 0, &pRecord->readStartSectorInd, Null);
}

#endif
