#ifndef __OD_GLOBAL__H_
#define __OD_GLOBAL__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "HwFwVer.h"
#include "CO_OD_Def.h"

	typedef struct
	{
		uint8				OD_max_subindex;
		HwFwVer				version;
		CO_OD_entryRecord_t OD_record[4];
	}FwFileRecord;


#ifdef __cplusplus
}
#endif

#endif

