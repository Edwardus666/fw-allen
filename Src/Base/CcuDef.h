#ifndef __CCU_DEF_H_
#define __CCU_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

#define CAN_BUS_0 0
#define CAN_BUS_1 1
#define CAN_NODE_COUNT 17

	typedef enum _DevAddr
	{
		  CCU_ADDR = 0x01
		, METER_ADDR
		, SHELF_LOCK_ADDR = 0x03

		, PMS_ADDR_0 = 0x07
		, PMS_ADDR_1 = 0x08
		, PMS_ADDR_2 = 0x09
		, PMS_ADDR_3 = 0x0A
		, PMS_ADDR_4 = 0x0B
		, PMS_ADDR_5 = 0x0C
		, PMS_ADDR_6 = 0x0D
		, PMS_ADDR_7 = 0x0E
		, PMS_ADDR_8 = 0x0F
		, PMS_ADDR_9  = 0x10
		, PMS_ADDR_10 = 0x11
		, PMS_ADDR_11 = 0x12
		, PMS_ADDR_12 = 0x13
		, PMS_ADDR_13 = 0x14
		, PMS_ADDR_14 = 0x15

		, BTU_ADDR = 0x16

		, PMS_ADDR_MAX
	}DevAddr;

#ifdef __cplusplus
}
#endif

#endif

