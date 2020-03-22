
#ifndef _SHELF_LOCK_H_
#define _SHELF_LOCK_H_

#ifdef __cplusplus
extern "C"{
#endif

/******************************
柜门锁单元头文件
*******************************/

#include "Rs485ProtocolNode.h"

#define BTLOCK_READ_SN		0x20

#define BTLOCK_SET_UNLOCK	0x01
#define BTLOCK_SET_LOCK		0x02
#define BTLOCK_READ_STATE	0x03

#define BTLOCK_CLR_INTERUPT	0x04

	typedef struct
	{
		Rs485ProtocolNode base;
	}ShelfLock;

	extern ShelfLock* g_pShelfLock;

	Rs485ProtocolNode* ShelfLock_Create(uint8 addr);

#ifdef __cplusplus
}
#endif

#endif
