
#ifndef _SM_H_
#define _SM_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

	typedef enum
	{
		SM_INIT = 0,	//System menagement init
		SM_START,		//System menagement start
		SM_STOP,		//System menagement start
	}SmState;

	typedef struct
	{
		SmState state;
	}Sm;

	void Sm_Init();
	void Sm_Start();

#ifdef __cplusplus
}
#endif

#endif
