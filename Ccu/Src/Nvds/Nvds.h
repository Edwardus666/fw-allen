#ifndef __NVDS__H_
#define __NVDS__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "FlashMap.h"

	typedef struct _Nvds
	{
		uint8 state;
	}Nvds;

	void Nvds_Init();
	void Nvds_Start();

#ifdef __cplusplus
}
#endif

#endif

