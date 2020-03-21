
#include "Common.h"
#include "Nvds.h"
#include "SectorMgr.h"
#include "DebugParam.h"

static SectorMgr	g_DebugParamSectorMgr;
//static SectorMgr	g_cfgParamSectorMgr;
//static SectorMgr	g_hwFwVerSnSectorMgr;


Bool Nvds_WriteDebugParam()
{
	return SectorMgr_Write(&g_DebugParamSectorMgr);
}

void Nvds_Init()
{
	SectorMgr_Init(&g_DebugParamSectorMgr, (uint32)& g_pFlashMap->debugInfoArea, (NvdsBase*)g_pDebugParam, sizeof(DebugParam));
}
