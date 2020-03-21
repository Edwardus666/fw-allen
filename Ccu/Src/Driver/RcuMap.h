

#ifndef _RESOURCE_MAP_H
#define _RESOURCE_MAP_H

#include "typedef.h"
#include "gd32f403_rcu.h"

rcu_periph_enum Rcu_Get(uint32 periph);
IRQn_Type IRQn_Get(uint32 com);
uint32 Dma_GetIRQn(uint32 dma, dma_channel_enum channel);

uint32 EXTI_GetIRQn(exti_line_enum line);

#endif //_DRIVER_UART_H
