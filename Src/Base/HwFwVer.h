#ifndef __HW_FW_VER_H_
#define __HW_FW_VER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

#pragma pack(1) 
	typedef struct _HwFwVer
	{
		uint8 m_HwMainVer;
		uint8 m_HwSubVer;

		uint8  m_AppMainVer;
		uint8  m_AppSubVer;
		uint8  m_AppMinorVer;
		uint32 m_AppBuildeNum;
	}HwFwVer;
#pragma pack() 

	void HwFwVer_Dump(const HwFwVer* pHwFwVer, const char* pTag);

#ifdef __cplusplus
}
#endif

#endif

