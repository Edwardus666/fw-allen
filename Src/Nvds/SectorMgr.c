#include "Common.h"
#include "SectorMgr.h"
#include "DriverFmc.h"
#include "crc16.h"

#define FMC_READ(src, to, len) memcpy((to), (src), len)
#define FMC_WRITE(addr, pData, len) fmc_program(addr, pData, len)
#define FMC_ERASE(addr) fmc_erase_pages(addr, addr)

void Nvds_Init(Nvds* pNvds, uint8 version)
{
	memset(pNvds, 0, sizeof(Nvds));

	pNvds->version = version;
}

void SectorMgr_Dump(SectorMgr* pSectorMgr)
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

	SectorMgr_Init(sector, 0, (Nvds*)tempBuf, sizeof(buf1));

	for(i = 0; i < (SECTOR_SIZE / BUF_SIZE); i++)
	{
		SectorMgr_WriteEx(sector, buf1);
	}

	
	return bRet;
}

//数据块是否被写过
Bool SectorMgr_isDirty(uint8* pByte, int len)
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
	uint32 readBuf = 0;
	uint32 itemsPerSector = SECTOR_SIZE/pSector->m_itemSize;

	//先检测最后一个Item是否已经被写过
	offset  = (itemsPerSector - 1) * pSector->m_itemSize ;
	readBuf = pSector->m_startAddr + offset;
	//FMC_READ(pSector->m_startAddr + offset, readBuf, pSector->m_itemSize);

	if(SectorMgr_isDirty((uint8*)readBuf, pSector->m_itemSize))
	{
		pSector->m_writeOffset = 0;
		pSector->m_readOffset  = offset;
		pSector->m_ItemCount   = itemsPerSector;
		return;
	}

	//从0开始, 定位空白位置
	pSector->m_writeOffset = 0;
	pSector->m_readOffset  = 0;
	pSector->m_ItemCount   = 0;
	
	for(offset = 0; offset <= (SECTOR_SIZE - pSector->m_itemSize * 2); offset += pSector->m_itemSize)
	{
		//FMC_READ(pSector->m_startAddr + offset, readBuf, pSector->m_itemSize);
		readBuf = pSector->m_startAddr + offset;

		if(SectorMgr_isDirty((uint8*)readBuf, pSector->m_itemSize))
		{
			pSector->m_ItemCount++;
			pSector->m_readOffset = offset;
		}
		else
		{
			pSector->m_writeOffset = offset;
			break;
		}
	}
}

//在扇区空闲部位写入数据，如果发现Flash某一位置损坏，则自动跳过去.
Bool SectorMgr_WriteEx(SectorMgr* pSector, const void* pData)
{
	uint8 startWriteoffset = pSector->m_writeOffset;
	uint32 readBuf;
	
	//写到新位置上
	while(True)
	{
		if(0 == pSector->m_writeOffset)
		{
			//执行擦除操作
			SectorMgr_Erase(pSector);
		}
		
		//写数据
		if(!FMC_WRITE(pSector->m_startAddr + pSector->m_writeOffset, (uint8*)pData, pSector->m_itemSize)) 
		{
			return False;
		}
		pSector->m_ItemCount++;

		//移动读指针
		pSector->m_readOffset  = pSector->m_writeOffset;
		
		//移动写指针,检查可写位置的长度是否超过SECTOR_SIZE
		pSector->m_writeOffset += pSector->m_itemSize;
		if((pSector->m_writeOffset + pSector->m_itemSize) > SECTOR_SIZE)
		{
			pSector->m_writeOffset = 0;
		}

		readBuf = (pSector->m_startAddr + pSector->m_readOffset);
		//读取写入的数据
		//if(!SectorMgr_Read(pSector, readBuf))
		//{
		//	return False;
		//}
		
		//通过比较数据判断是否写成功,如果失败，说明Flash可能损坏,再写下一个区块。
		if(memcmp(pData, (void*)readBuf, pSector->m_itemSize) == 0)
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

Bool SectorMgr_Write(SectorMgr* pSector)
{
	Nvds* pData = SectorMgr_Read(pSector);
	uint8* pByte = (uint8*)pSector->m_pItem;

	//计算CRC
	pSector->m_pItem->crc = CRC16_calc(0, &pByte[sizeof(Nvds)], pSector->m_itemSize - sizeof(Nvds));

	//比较是否相等
	if (memcmp(pData, pSector->m_pItem, pSector->m_itemSize) != 0)
	{
		return SectorMgr_WriteEx(pSector, pSector->m_pItem);
	}
	return True;
}

//读取数据
Nvds* SectorMgr_Read(SectorMgr* pSector)
{
	return (Nvds*)(pSector->m_startAddr + pSector->m_readOffset);
}

void SectorMgr_ReadToBuf(SectorMgr* pSector, void* pBuf)
{
	memcpy(pBuf, (uint8*)(pSector->m_startAddr + pSector->m_readOffset), pSector->m_itemSize);
}


Bool SectorMgr_ReadItem(SectorMgr* pSector, int itemInd, void* pBuff)
{
	uint32 offset = itemInd * pSector->m_itemSize;

	if(itemInd >= pSector->m_ItemCount)
		return False;	//Offset is invalid.

	//FMC_READ(pSector->m_startAddr + offset, pBuff, pSector->m_itemSize);
	memcpy(pBuff, (uint8*)(pSector->m_startAddr + offset), pSector->m_itemSize);

	return True;
}

Bool SectorMgr_isFull(SectorMgr* pSector)
{
	return (pSector->m_ItemCount == SECTOR_SIZE / pSector->m_itemSize);
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

	FMC_ERASE(pSector->m_startAddr);
	//return FlashOperationSuccess == Mx25_EraseSector(pSector->m_startAddr);
	return True;
}

Bool SectorMgr_Copy(SectorMgr* pDst, const SectorMgr* pSrc)
{
	uint32 offset = 0;
	uint8* readBuf;
	
	if(!SectorMgr_isEmpty(pDst))
	{
		SectorMgr_Erase(pDst);
	}

	for(offset = 0; offset < SECTOR_SIZE; offset += PAGE_SIZE)
	{
		//FMC_READ (pSrc->m_startAddr + offset, readBuf, PAGE_SIZE);
		readBuf = (uint8*)(pSrc->m_startAddr + offset);
		FMC_WRITE(pDst->m_startAddr + offset, readBuf, PAGE_SIZE);
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
	pSector->m_ItemCount   = SECTOR_SIZE/itemSize;
	pSector->m_readOffset  = (pSector->m_ItemCount - 1) * itemSize ;
}

Bool SectorMgr_Init(SectorMgr* pSector, uint32 startAddr, Nvds* pItem, uint16 itemSize)
{	
	memset(pSector, 0, sizeof(SectorMgr));

	if(itemSize > MAX_ITEM_SIZE) return False;
	
	if(startAddr % SECTOR_SIZE) return False;	//startAddr必须要被SECTOR_SIZE整除
	if(itemSize > SECTOR_SIZE) return False;	//size必须小于SECTOR_SIZE
	
	pSector->m_startAddr = startAddr;
	pSector->m_itemSize	 = itemSize;
	pSector->m_pItem	 = pItem;

	SectorMgr_CalcOffset(pSector);

	if(pSector->m_ItemCount > 0 && pItem)
	{
		Nvds* p = SectorMgr_Read(pSector);
		//计算CRC
		uint8* pByte = (uint8*)p;
		uint16 crc = CRC16_calc(0, &pByte[sizeof(Nvds)], pSector->m_itemSize - sizeof(Nvds));

		//检验版本号和校验和
		if (p->version == pItem->version && crc == p->crc)
		{
			SectorMgr_ReadToBuf(pSector, pItem);
		}
		else
		{
			SectorMgr_Erase(pSector);
		}
	}

	//检验
	return (pSector->m_ItemCount > 0);
}
