
#include "Common.h"
#include "HwFwVer.h"

void HwFwVer_Dump(const HwFwVer* pHwFwVer, const char* pTag)
{
	Printf("%sHw(%d.%d), Fw(%d.%d.%d.%d)\n"
		, pTag
		, pHwFwVer->m_HwMainVer
		, pHwFwVer->m_HwSubVer

		, pHwFwVer->m_AppMainVer
		, pHwFwVer->m_AppSubVer
		, pHwFwVer->m_AppMinorVer
		, pHwFwVer->m_AppBuildeNum);
}
