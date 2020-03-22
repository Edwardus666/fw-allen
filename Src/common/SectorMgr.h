#ifndef __SECTOR_MGR__H_
#define __SECTOR_MGR__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
/******************************************************************/
#define MAX_ITEM_SIZE 	128		//���Ԫ���ֽ���

	extern Bool FmcRead(uint32 addr, void* pData, int len);
	extern Bool FmcWrite(uint32 addr, void* pData, int len);
	extern Bool FmcErase(uint32 addr, uint32 dataLength);
/******************************************************************/

typedef struct _Sector
{
	uint32 m_startAddr;		//���Գ�ʼ��������д��������ʼ��ַ
	void*  m_pItem;			//���Գ�ʼ��������ָ��Ҫ���µ�����
	uint16 m_itemSize;		//���Գ�ʼ��������Item��С
	uint16 m_sectorSize;	//���Գ�ʼ��������������С��
	
	uint32 m_writeOffset;	//�����ڵ�дƫ�Ƶ�ַ
	uint32 m_readOffset;	//�����ڵĶ�ƫ�Ƶ�ַ
	uint32 m_ItemCount;		//��Ч��Item����
}SectorMgr;

Bool SectorMgr_Init(SectorMgr* pSector, uint16 sectorSize
	, uint32 startAddr, void* pItem, uint16 itemSize);
	void SectorMgr_InitFullSector(SectorMgr* pSector, uint32 startAddr, uint16 itemSize);
Bool SectorMgr_WriteEx(SectorMgr* pSector, const void* pData);
Bool SectorMgr_Write(SectorMgr* pSector);
int SectorMgr_Read(SectorMgr* pSector, void* pBuff);
void SectorMgr_CalcOffset(SectorMgr* pSector);
Bool SectorMgr_isFull(SectorMgr* pSector);
Bool SectorMgr_isEmpty(SectorMgr* pSector);
Bool SectorMgr_ReadItem(SectorMgr* pSector, int itemInd, void* pBuff);
Bool SectorMgr_Erase(SectorMgr* pSector);
Bool SectorMgr_Copy(SectorMgr* pDst, const SectorMgr* pSrc, uint8* pageBuf, int bufLen);
void SectorMgr_Dump(const SectorMgr* pSectorMgr);
void SectorMgr_Reset(SectorMgr* pSector);
Bool SectorMgr_IsChanged(const SectorMgr* pSector);

#ifdef __cplusplus
}
#endif

#endif

