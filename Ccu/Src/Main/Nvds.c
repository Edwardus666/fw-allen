#include "Common.h"
#include "Nvds.h"
#include "Record.h"

VersionInfo *g_pVerInfo;
CfgInfo		*g_pCfgInfo;
DebugInfo	*g_pDbgInfo;

static Nvds  g_Nvds[3];

void CfgInfo_Event(CfgInfo* p, BlockEventID eventId)
{
	if (eventId == BE_DATA_ERROR)
	{
		//数据错误，设定默认值
		p->temp_thresholdLow  = 37;
		p->temp_thresholdHigh = 40;
	}
}

void DbgInfo_Event(DebugInfo* p, BlockEventID eventId)
{
	if (eventId == BE_DATA_ERROR)
	{
		//数据错误，设定默认值
		g_dwDebugLevel = 0xF;
	}
	else if (eventId == BE_DATA_OK)
	{
		g_dwDebugLevel = p->debugLevel;
	}
	else if (eventId == BE_ON_WRITE_BEFORE)
	{
		p->debugLevel = g_dwDebugLevel;
	}
}

void VerInfo_Event(VersionInfo* p, BlockEventID eventId)
{
	HwFwVer* pVer = &p->version;
	if (eventId == BE_DATA_ERROR)
	{
		//数据错误，设定默认值
		pVer->m_HwMainVer = 0;
		pVer->m_HwSubVer = 1;

		pVer->m_AppMainVer	= 1;
		pVer->m_AppSubVer	= 0;
		pVer->m_AppMinorVer = 0;
		pVer->m_AppBuildeNum = 1;
	}
	sprintf(p->manufacturerHardwareVersion, "%d.%d", pVer->m_HwMainVer, pVer->m_HwSubVer);
	sprintf(p->manufacturerSoftwareVersion, "%d.%d.%d.%d", pVer->m_AppMainVer, pVer->m_AppSubVer, pVer->m_AppMinorVer, pVer->m_AppBuildeNum);
}

void Nvds_Write(NvdsID id)
{
	Nvds* pNvds = &g_Nvds[id];
	if (pNvds->Event == Null) return;	//没被初始化

	Assert(id < GET_ELEMENT_COUNT(g_Nvds));

	SectorMgr* pMgr = &pNvds->sectorMgr;

	pNvds->Event(pMgr->m_pItem, BE_ON_WRITE_BEFORE);

	//内容没有变化，不执行写操作
	if (!SectorMgr_IsChanged(pMgr)) return;
	
	SectorMgr_Write(pMgr);
}

void Nvds_InitItem(Nvds* p, uint8 id, void* pData, int dataLength, void* eepRomAddr, BlockEventFn event)
{
	Assert(id < GET_ELEMENT_COUNT(g_Nvds));
	Assert(dataLength % 2 == 0);	//dataLength必须为偶数(2字节对齐)，否则无法写入Flash

	uint8* pByte = (uint8*)pData;
	p->id = id;
	p->Event = event;

	SectorMgr_Init(&p->sectorMgr, BANK0_SECTOR_SIZE, (uint32)eepRomAddr, pData, dataLength);

	//检验内容是否有效
	if (pByte[0] != EEPROM_FIRST_BYTE || pByte[dataLength - 1] != EEPROM_LATEST_BYTE)
	{
		//数据无效，重新调用默认的初始化函数
		p->Event(pByte, BE_DATA_ERROR);

		pByte[0] = EEPROM_FIRST_BYTE;
		pByte[dataLength - 1] = EEPROM_LATEST_BYTE;
		if(p->sectorMgr.m_ItemCount)	//如果被写过,则执行删除操作
		{
			SectorMgr_Erase(&p->sectorMgr);
		}
	}
	else
	{
		p->Event(pByte, BE_DATA_OK);
	}

}

void Nvds_Init()
{
	static VersionInfo  g_verInfo;
	static CfgInfo		g_cfgInfo;
	static DebugInfo	g_dbgInfo;

	g_pVerInfo = &g_verInfo;
	g_pCfgInfo = &g_cfgInfo;
	g_pDbgInfo = &g_dbgInfo;

	Nvds* p = g_Nvds;
	Nvds_InitItem(p++, NVDS_VER_INFO, &g_verInfo, sizeof(g_verInfo), &g_pMemMap->verInfo, (BlockEventFn)VerInfo_Event);
	Nvds_InitItem(p++, NVDS_CFG_INFO, &g_cfgInfo, sizeof(g_cfgInfo), &g_pMemMap->cfgInfo, (BlockEventFn)CfgInfo_Event);
	Nvds_InitItem(p++, NDDS_DBG_INFO, &g_dbgInfo, sizeof(g_dbgInfo), &g_pMemMap->dbgInfo, (BlockEventFn)DbgInfo_Event);

}
