#ifndef __DEBUG_PARAM__H_
#define __DEBUG_PARAM__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "SectorMgr.h"

#define DEBUG_PARAM_AREA_VER	1	//当存储数据的结构更改，且不能兼容旧版本，就必须修改版本号，Flash中旧版本内容被擦除。
#define DEBUG_PARAM_AREA_SIZE	128	//必须为4的整数倍，因为Flash每次写入以4个字节为单位。

#pragma pack(1)

	typedef struct
	{
		Nvds  base;	//在第一个位置必须包含NvdsBase

		uint32 debugLevel;

		uint8 reserved[DEBUG_PARAM_AREA_SIZE - sizeof(Nvds) - 4];
	}DebugParam;

#pragma pack()

	extern DebugParam* g_pDebugParam;
#ifdef __cplusplus
}
#endif

#endif

