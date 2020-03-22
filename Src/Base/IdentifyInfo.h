#ifndef __INDENTIFY_INFO_H_
#define __INDENTIFY_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "HwFwVer.h"
#include "SectorMgr.h"

#define SN_SIZE      14
#define ID_INFO_VER  1

#define ID_INFO_SIZE 128

#pragma pack(1) 
	typedef struct _IdInfo
	{
		Nvds base;

		HwFwVer hwFwVer;
		uint8 Sn[SN_SIZE];

		uint8 reserved[ID_INFO_SIZE - sizeof(Nvds) - sizeof(HwFwVer) - SN_SIZE];
	}IdInfo;
#pragma pack() 

	extern IdInfo* g_pIdInfo;

	void IdInfo_Dump(const IdInfo* pHwFwVer, const char* pTag);

#ifdef __cplusplus
}
#endif

#endif

