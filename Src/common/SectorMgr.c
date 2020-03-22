#include "Common.h"
#include "SectorMgr.h"

void SectorMgr_Dump(const SectorMgr* pSectorMgr)
{
	Printf("SectorMgr(@%x) Dump:\n", pSectorMgr);
	
//	Printf("\t m_Version=%d.\n"		, pSectorMgr->m_Version);
	Printf("\t m_startAddr=%d.\n"	, pSectorMgr->m_startAddr);
	Printf("\t m_writeOffset=%d.\n"	, pSectorMgr->m_writeOffset);
	Printf("\t m_readOffset=%d.\n"	, pSectorMgr->m_readOffset);
	
	Printf("\t m_pItem=%x.\n"	, pSectorMgr->m_pItem);
	Printf("\t m_itemSize=%d.\n"	, pSectorMgr->m_itemSize);
	Printf("\t m_ItemCount=%d.\n"	, pSectorMgr->m_ItemCount);
}

Bool SectorMgr_Test()
{
#define BUF_SIZE 512
	int i = 0;
	Bool bRet = False;
	static uint8 tempBuf[BUF_SIZE];
	static uint8 buf1[BUF_SIZE];
	SectorMgr sector[2];

	for(i = 0; i < sizeof(buf1); i++)
	{
		buf1[i]= i;
	}

	SectorMgr_Init(sector, 512, 0, tempBuf, sizeof(buf1));

	for(i = 0; i < (sector->m_sectorSize / BUF_SIZE); i++)
	{
		SectorMgr_WriteEx(sector, buf1);
	}

	
	return bRet;
}

//数据块是否被写过
Bool SectorMgr_isWritten(uint8* pByte, int len)
{
	int i = 0;

	for(i = 0; i < len; i++)
	{
		//数据中有一个非0xFF值，说明该区块已经被写过
		if(pByte[i] != 0xFF)
		{
			return True;
		}
	}

	return False;
}

void SectorMgr_CalcOffset(SectorMgr* pSector)
{
	uint16 offset = 0;
	uint8 readBuf[MAX_ITEM_SIZE];
	uint32 itemsPerSector = pSector->m_sectorSize / pSector->m_itemSize;

	//先检测最后一个Item是否已经被写过
	offset = (itemsPerSector - 1) * pSector->m_itemSize;
	FmcRead(pSector->m_startAddr + offset, readBuf, pSector->m_itemSize);
	if (SectorMgr_isWritten(readBuf, pSector->m_itemSize))
	{
		pSector->m_writeOffset = 0;
		pSector->m_readOffset = offset;
		pSector->m_ItemCount = itemsPerSector;
		return;
	}

	//检测第一个位置是否已经被写过
	FmcRead(pSector->m_startAddr, readBuf, pSector->m_itemSize);
	if(!SectorMgr_isWritten(readBuf, pSector->m_itemSize))
	{
		pSector->m_writeOffset = 0;
		pSector->m_readOffset = 0;
		pSector->m_ItemCount = 0;
		return;
	}

	//定位可写范围
	int writeInd = 0;
	int unWriteInd = 0;
	int mid;
	int minInd = 0;
	int maxInd = itemsPerSector;
	for (; (maxInd - minInd) != 1;)
	{
		mid = (maxInd + minInd) >> 1;//mid是n/2；
		FmcRead(pSector->m_startAddr + mid * pSector->m_itemSize, readBuf, pSector->m_itemSize);
		if (SectorMgr_isWritten(readBuf, pSector->m_itemSize))
		{
			minInd = mid;
			writeInd = mid;
		}
		else
		{
			maxInd = mid;
			unWriteInd = mid;
		}
	}

	//找出可写位置
	writeInd *= pSector->m_itemSize;
	unWriteInd *= pSector->m_itemSize;
	for (int i = writeInd; i <= unWriteInd; i += pSector->m_itemSize)
	{
		FmcRead(pSector->m_startAddr + i, readBuf, pSector->m_itemSize);
		if (!SectorMgr_isWritten(readBuf, pSector->m_itemSize))
		{
			pSector->m_readOffset = i - pSector->m_itemSize;
			pSector->m_writeOffset = i;
			pSector->m_ItemCount = i / pSector->m_itemSize;
			break;
		}
	}
}

/*-***********************############-*/
/*-******############-*/
/*-******##-*/
/*-**##-*/

//void SectorMgr_CalcOffset(SectorMgr* pSector)
//{
//	uint16 offset = 0;
//	uint8 readBuf[MAX_ITEM_SIZE];
//	uint32 itemsPerSector = pSector->m_sectorSize/pSector->m_itemSize;
//
//	//先检测最后一个Item是否已经被写过
//	offset  = (itemsPerSector - 1) * pSector->m_itemSize ;
//	FmcRead(pSector->m_startAddr + offset, readBuf, pSector->m_itemSize);
//
//	if(SectorMgr_isWritten(readBuf, pSector->m_itemSize))
//	{
//		pSector->m_writeOffset = 0;
//		pSector->m_readOffset  = offset;
//		pSector->m_ItemCount   = itemsPerSector;
//		return;
//	}
//
//	//从0开始, 定位空白位置
//	pSector->m_writeOffset = offset;
//	pSector->m_readOffset  = 0;
//	pSector->m_ItemCount   = 0;
//
//	//找出第一个没有被写的地方
//	for(int i = 0; i <= offset; i += pSector->m_itemSize)
//	{
//		FmcRead(pSector->m_startAddr + i, readBuf, pSector->m_itemSize);
//		
//		if(SectorMgr_isWritten(readBuf, pSector->m_itemSize))
//		{
//			pSector->m_ItemCount++;
//			pSector->m_readOffset = i;
//		}
//		else
//		{
//			pSector->m_writeOffset = i;
//			break;
//		}
//	}
//}

//在扇区空闲部位写入数据，如果发现Flash某一位置损坏，则自动跳过去.
Bool SectorMgr_WriteEx(SectorMgr* pSector, const void* pData)
{
	uint8 startWriteoffset = pSector->m_writeOffset;
	uint8 readBuf[MAX_ITEM_SIZE];
	
	//写到新位置上
	while(True)
	{
		if(0 == pSector->m_writeOffset)
		{
			//检查该扇区是否被写过，如果是，执行擦除操作
			for (int i = 0; i < pSector->m_sectorSize; i += MAX_ITEM_SIZE)
			{
				if (FmcRead(pSector->m_startAddr + i, readBuf, MAX_ITEM_SIZE)
					&& SectorMgr_isWritten(readBuf, MAX_ITEM_SIZE))
				{
					SectorMgr_Erase(pSector);
					break;
				}
			}
		}
		
		//写数据
		if(!FmcWrite(pSector->m_startAddr + pSector->m_writeOffset, (uint8*)pData, pSector->m_itemSize)) 
		{
			return False;
		}
		pSector->m_ItemCount++;

		//移动读指针
		pSector->m_readOffset  = pSector->m_writeOffset;
		
		//移动写指针,检查可写位置的长度是否超过pSector->m_sectorSize
		pSector->m_writeOffset += pSector->m_itemSize;
		if((pSector->m_writeOffset + pSector->m_itemSize) > pSector->m_sectorSize)
		{
			pSector->m_writeOffset = 0;
		}

		//读取写入的数据
		if(!SectorMgr_Read(pSector, readBuf))
		{
			return False;
		}
		
		//通过比较数据判断是否写成功,如果失败，说明Flash可能损坏,再写下一个区块。
		if(memcmp(pData, readBuf, pSector->m_itemSize) == 0)
		{
			//比较成功
			break;
		}

		//当两者相等时，说明全部区块都写失败，Flash这个Sector全部损坏。
		if(startWriteoffset == pSector->m_writeOffset)
		{
			return False;
		}		
	}

	return True;
}

Bool SectorMgr_IsChanged(const SectorMgr* pSector)
{
	return memcmp((void*)(pSector->m_startAddr + pSector->m_readOffset), pSector->m_pItem, pSector->m_itemSize) != 0;
}

Bool SectorMgr_Write(SectorMgr* pSector)
{
	return SectorMgr_WriteEx(pSector, pSector->m_pItem);
}

//读取数据
int SectorMgr_Read(SectorMgr* pSector, void* pBuff)
{
	if (!FmcRead(pSector->m_startAddr + pSector->m_readOffset, pBuff, pSector->m_itemSize)) return False;

	return pSector->m_itemSize;
}

Bool SectorMgr_ReadItem(SectorMgr* pSector, int itemInd, void* pBuff)
{
	//uint32 offset = itemInd * pSector->m_itemSize;

	if(itemInd >= pSector->m_ItemCount)
		return False;	//Offset is invalid.

	if (!FmcRead(pSector->m_startAddr + pSector->m_readOffset, pBuff, pSector->m_itemSize)) return False;

	return True;
}

Bool SectorMgr_isFull(SectorMgr* pSector)
{
	return (pSector->m_ItemCount == pSector->m_sectorSize / pSector->m_itemSize);
}

Bool SectorMgr_isEmpty(SectorMgr* pSector)
{
	return (pSector->m_ItemCount == 0);
}

Bool SectorMgr_Erase(SectorMgr* pSector)
{
	pSector->m_writeOffset = 0;
	pSector->m_readOffset  = 0;
	pSector->m_ItemCount   = 0;
	return FmcErase(pSector->m_startAddr, pSector->m_sectorSize);
}

void SectorMgr_Reset(SectorMgr* pSector)
{
	pSector->m_writeOffset = 0;
	pSector->m_readOffset = 0;
	pSector->m_ItemCount = 0;
}

Bool SectorMgr_Copy(SectorMgr* pDst, const SectorMgr* pSrc, uint8* pageBuf, int bufLen)
{
	uint32 offset = 0;
	
	if(!SectorMgr_isEmpty(pDst))
	{
		SectorMgr_Erase(pDst);
	}

	for(offset = 0; offset < pDst->m_sectorSize; offset += bufLen)
	{
		FmcRead (pSrc->m_startAddr + offset, pageBuf, bufLen);
		FmcWrite(pDst->m_startAddr + offset, pageBuf, bufLen);
	}

	SectorMgr_CalcOffset(pDst);
	return True;
}

//初始化一个满的扇区
void SectorMgr_InitFullSector(SectorMgr* pSector, uint32 startAddr, uint16 itemSize)
{
	pSector->m_startAddr = startAddr;
	pSector->m_itemSize	 = itemSize;
	
	pSector->m_writeOffset = 0;
	pSector->m_ItemCount   = pSector->m_sectorSize/itemSize;
	pSector->m_readOffset  = (pSector->m_ItemCount - 1) * itemSize ;
}

Bool SectorMgr_Init(SectorMgr* pSector, uint16 sectorSize
	, uint32 startAddr, void* pItem, uint16 itemSize)
{	
	memset(pSector, 0, sizeof(SectorMgr));

	if(itemSize > MAX_ITEM_SIZE) return False;
	
	if(startAddr % sectorSize) return False;	//startAddr必须要被pSector->m_sectorSize整除
	if(itemSize > sectorSize) return False;	//dataLength必须小于pSector->m_sectorSize
	
	pSector->m_startAddr = startAddr;
	pSector->m_itemSize	 = itemSize;
	pSector->m_pItem = pItem;
	pSector->m_sectorSize = sectorSize;

	SectorMgr_CalcOffset(pSector);

	if(pSector->m_ItemCount > 0 && pItem)
	{
		//读取第一项内容，检验数据长度
		SectorMgr_Read(pSector, pItem);
	}

	//检验
	return (pSector->m_ItemCount > 0);
}

