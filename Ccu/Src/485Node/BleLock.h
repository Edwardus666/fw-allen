
#ifndef _BLE_LOCK_H_
#define _BLE_LOCK_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "Rs485Protocol.h"

	typedef enum
	{
		BLE_LOCK_INIT = 0,	//System menagement init
		BLE_LOCK_START,		//System menagement start
		BLE_LOCK_STOP,		//System menagement start
	}BleLockState;

	typedef struct
	{
		BleLockState state;
		Rs485Protocol utp;
	}BleLock;

	void BleLock_Init();
	void BleLock_Start();
	void BleLock_Stop();

#ifdef __cplusplus
}
#endif

#endif
