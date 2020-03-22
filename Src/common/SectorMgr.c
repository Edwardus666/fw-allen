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

//���ݿ��Ƿ�д��
Bool SectorMgr_isWritten(uint8* pByte, int len)
{
	int i = 0;

	for(i = 0; i < len; i++)
	{
		//��������һ����0xFFֵ��˵���������Ѿ���д��
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

	//�ȼ�����һ��Item�Ƿ��Ѿ���д��
	offset = (itemsPerSector - 1) * pSector->m_itemSize;
	FmcRead(pSector->m_startAddr + offset, readBuf, pSector->m_itemSize);
	if (SectorMgr_isWritten(readBuf, pSector->m_itemSize))
	{
		pSector->m_writeOffset = 0;
		pSector->m_readOffset = offset;
		pSector->m_ItemCount = itemsPerSector;
		return;
	}

	//����һ��λ���Ƿ��Ѿ���д��
	FmcRead(pSector->m_startAddr, readBuf, pSector->m_itemSize);
	if(!SectorMgr_isWritten(readBuf, pSector->m_itemSize))
	{
		pSector->m_writeOffset = 0;
		pSector->m_readOffset = 0;
		pSector->m_ItemCount = 0;
		return;
	}

	//��λ��д��Χ
	int writeInd = 0;
	int unWriteInd = 0;
	int mid;
	int minInd = 0;
	int maxInd = itemsPerSector;
	for (; (maxInd - minInd) != 1;)
	{
		mid = (maxInd + minInd) >> 1;//mid��n/2��
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

	//�ҳ���дλ��
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
//	//�ȼ�����һ��Item�Ƿ��Ѿ���д��
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
//	//��0��ʼ, ��λ�հ�λ��
//	pSector->m_writeOffset = offset;
//	pSector->m_readOffset  = 0;
//	pSector->m_ItemCount   = 0;
//
//	//�ҳ���һ��û�б�д�ĵط�
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

//���������в�λд�����ݣ��������Flashĳһλ���𻵣����Զ�����ȥ.
Bool SectorMgr_WriteEx(SectorMgr* pSector, const void* pData)
{
	uint8 startWriteoffset = pSector->m_writeOffset;
	uint8 readBuf[MAX_ITEM_SIZE];
	
	//д����λ����
	while(True)
	{
		if(0 == pSector->m_writeOffset)
		{
			//���������Ƿ�д��������ǣ�ִ�в�������
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
		
		//д����
		if(!FmcWrite(pSector->m_startAddr + pSector->m_writeOffset, (uint8*)pData, pSector->m_itemSize)) 
		{
			return False;
		}
		pSector->m_ItemCount++;

		//�ƶ���ָ��
		pSector->m_readOffset  = pSector->m_writeOffset;
		
		//�ƶ�дָ��,����дλ�õĳ����Ƿ񳬹�pSector->m_sectorSize
		pSector->m_writeOffset += pSector->m_itemSize;
		if((pSector->m_writeOffset + pSector->m_itemSize) > pSector->m_sectorSize)
		{
			pSector->m_writeOffset = 0;
		}

		//��ȡд�������
		if(!SectorMgr_Read(pSector, readBuf))
		{
			return False;
		}
		
		//ͨ���Ƚ������ж��Ƿ�д�ɹ�,���ʧ�ܣ�˵��Flash������,��д��һ�����顣
		if(memcmp(pData, readBuf, pSector->m_itemSize) == 0)
		{
			//�Ƚϳɹ�
			break;
		}

		//���������ʱ��˵��ȫ�����鶼дʧ�ܣ�Flash���Sectorȫ���𻵡�
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

//��ȡ����
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

//��ʼ��һ����������
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
	
	if(startAddr % sectorSize) return False;	//startAddr����Ҫ��pSector->m_sectorSize����
	if(itemSize > sectorSize) return False;	//dataLength����С��pSector->m_sectorSize
	
	pSector->m_startAddr = startAddr;
	pSector->m_itemSize	 = itemSize;
	pSector->m_pItem = pItem;
	pSector->m_sectorSize = sectorSize;

	SectorMgr_CalcOffset(pSector);

	if(pSector->m_ItemCount > 0 && pItem)
	{
		//��ȡ��һ�����ݣ��������ݳ���
		SectorMgr_Read(pSector, pItem);
	}

	//����
	return (pSector->m_ItemCount > 0);
}

