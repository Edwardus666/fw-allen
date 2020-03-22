
#include "Common.h"
#include "DebugParam.h"
#include "FlashMap.h"

DebugParam* g_pDebugParam;
static SectorMgr g_DebugParamSectorMgr;

void DebugParam_Init()
{
	static DebugParam g_DebugParam;
	g_pDebugParam = &g_DebugParam;

	memset(g_pDebugParam, 0, sizeof(DebugParam));

	Nvds_Init(&g_pDebugParam->base, DEBUG_PARAM_AREA_VER);

	SectorMgr_Init(&g_DebugParamSectorMgr, (uint32)& g_pFlashMap->debugInfoArea, (Nvds*)g_pDebugParam, sizeof(DebugParam));
}

Bool DebugParam_Write()
{
	return SectorMgr_Write(&g_DebugParamSectorMgr);
}
