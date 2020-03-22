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

/*功能:转换相对扇区为实际扇区
参数说明:
	sec:扇区值。
	ind:扇区的相对记录位置
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

/*函数功能：计算扇区号和扇区内偏移地址
	参数:itemInd,元素索引位置.
	参数:sec,扇区号.
	参数:addrOfSecOffset,扇区内部偏移地址.
	返回值:绝对地址.
*/
uint32 Record_CalcWriteSecAddr(Record* pRecord, uint32 itemInd, uint16* sec, uint32* addrOfSecOffset)
{
	uint32 addr = 0;
	int offsetSec = 0;
	
	if(itemInd >= pRecord->maxItems) return RECORD_INVALID_ADD;

	//当前所有扇区都有内容
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

	if(pRecord->sectorCount == 0) 	//还没有初始化完毕
		return False;
	
	if(SectorMgr_Write(&pRecord->sector))
	{
		pRecord->itemCount++;
	}
	
	uint16 sectorInd = pRecord->writeSectorInd;
	uint32 addr = Record_CalcWriteSecAddr(pRecord, pRecord->itemCount % pRecord->maxItems, &pRecord->writeSectorInd, &addrOfSec);
	if(addr == RECORD_INVALID_ADD) return False;
	if (pRecord->writeSectorInd != sectorInd)	//当前扇区写满,切换到下一个新的扇区
	{
		//切换到下一个新的扇区
		pRecord->sector.m_startAddr = addr;
		SectorMgr_Reset(&pRecord->sector);
		if(pRecord->itemCount >= pRecord->maxItems)	
		{
			//擦除新扇区内容
			SectorMgr_Erase(&pRecord->sector);
			//更正有效元素总数
			pRecord->itemCount -= pRecord->itemsPerSec;
			//移动读指针游标，如果当前正指向删除的扇区，要移动到第一个可读位置
			pRecord->readPointer = (pRecord->readPointer > pRecord->itemsPerSec) ? (pRecord->readPointer-pRecord->itemsPerSec) : 0;
		}
		
		//更新读起始扇区号
		Record_CalcuReadSecAddr(pRecord, 0, &pRecord->readStartSectorInd, Null);
	}
	return True;
}

/*函数功能：计算扇区号和扇区内偏移地址
	参数:itemInd,元素索引位置.
	参数:sec,扇区号.
	参数:addrOfSecOffset,扇区内部偏移地址.
	返回值:绝对地址.
*/
uint32 Record_CalcuReadSecAddr(Record* pRecord, uint32 itemInd, uint16* sec, uint32* addrOfSecOffset)
{
	uint32 addr = 0;
	uint8 offsetSec = 0;
	
	if(itemInd >= pRecord->maxItems) return RECORD_INVALID_ADD;

	if(pRecord->itemCount >= pRecord->maxItems - pRecord->itemsPerSec)	//全部扇区的数据都是有效的
	{
		offsetSec = pRecord->writeSectorInd + 1;	//读指针为于写指针的下一个扇区
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
	if(pRecord->sectorCount == 0) 	//还没有初始化完毕
		return;
	uint16 SECTOR_SIZE = pRecord->sector.m_sectorSize;
	//擦除所有扇区
	for(i = 0; i < pRecord->sectorCount; i++)
	{
		FmcErase(pRecord->startAddr + i * SECTOR_SIZE, SECTOR_SIZE);
	}

	//重新初始化扇区管理器
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
	
	//备份默认值
	memcpy(temp,  pItem, itemSize);
	
	//从Flash读取设置值
	bRet = SectorMgr_Init(pSector, sectorSize, startAddr, pItem , itemSize);
	
	//检验数据内容是否有效，如果校验失败，说明数据过时，被清除
	if(bRet && verify && !verify(pItem, 0))	//校验版本号
	{
		//Printf("Ver is not match.\n");
		SectorMgr_Erase(pSector);
		//恢复默认值
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

	//遍历所有扇区，统计有效的记录总数, 找出可写扇区。
	SectorMgr* pMgr = &pRecord->sector;
	for(i = 0; i < sectorCount; i++)
	{
		uint32 addr = pRecord->startAddr + i * sectorSize;
		Record_InitSectorMgr(pMgr, sectorSize, addr, pItem, itemSize, verify);

		//如果中间有一个扇区没有写满，则之后的所有扇区，要不全满，要不全空
		if(!SectorMgr_isFull(pMgr))
		{		
			if(writeSecInd < 0)	//第一个
			{
				writeSecInd = i;
			}
			else if(pMgr->m_ItemCount > 0)	//发生不可预期的错误，删除后面的日志数据。
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
	
	//找不到可写扇区
	if(writeSecInd == -1) 
	{
		writeSecInd = 0;
		if(pRecord->itemCount)	//发生不可预期的错误
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
