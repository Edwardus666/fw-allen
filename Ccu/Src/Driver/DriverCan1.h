

#ifndef _DRIVER_CAN1_DRIVER_H
#define _DRIVER_CAN1_DRIVER_H

#include <stdint.h>
#include "SafeBuf.h"

#define RX_MSG_BUF_SIZE_MAX 100
#define TX_MSG_BUF_SIZE_MAX 50

#define MAX_MAIL_BOX_COUNT 3
typedef struct
{
	//Can config
	uint32 canPort;	//CAN0 or CAN1
	uint32 ioPort;
	uint32 txPin;
	uint32 rxPin;

	uint8 priority;

	//Config end
	uint16 sfid;
}CanBus;

#endif //_DRIVER_UART_H
