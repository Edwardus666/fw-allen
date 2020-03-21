
#include "Common.h"
#include "IdentifyInfo.h"
#include "FlashMap.h"

IdInfo* g_pIdInfo;
static SectorMgr g_IdInfoSectorMgr;

void IdInfo_Dump(const IdInfo* pHwFwVer, const char* pTag)
{
}

void IdInfo_Init()
{
	static IdInfo g_IdInfo;
	g_pIdInfo = &g_IdInfo;

	memset(g_pIdInfo, 0, sizeof(IdInfo));

	Nvds_Init(&g_pIdInfo->base, ID_INFO_VER);

	SectorMgr_Init(&g_IdInfoSectorMgr, (uint32)& g_pFlashMap->idInfoArea, (Nvds*)g_pIdInfo, sizeof(IdInfo));
}

void IdInfo_Save()
{
	SectorMgr_Write(&g_IdInfoSectorMgr);
}
