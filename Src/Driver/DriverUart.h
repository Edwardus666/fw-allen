

#ifndef _DRIVER_UART_H
#define _DRIVER_UART_H

#include <stdint.h>
#include "SafeBuf.h"
#include "DriverIo.h"

//复用的COM定义
typedef enum
{
	METER_COM = 0,
	MISC_COM = 1
}MultiCom;


typedef enum
{
	CCU_COOLING_COM = USART0,	//USART0 DMA0 Tx: PA9,  DMA_CH3; Rx: PA10, DMA_CH4
	CCU_METER_COM    = USART1,	//USART2 DMA0 Tx: PA2,  DMA_CH1; Rx: PA3,  DMA_CH2
	CCU_MISC_COM	 = USART2,	//USART1 DMA0 Tx: PB10, DMA_CH6; Rx: PB11, DMA_CH5
	CCU_RESERVED_COM = UART3,	//UART3  DMA1 Tx: PC10, DMA_CH4; Rx: PC11, DMA_CH2, 
	CCU_DEBUG_COM1	= UART4,	//UART4  Tx: PC12; Rx: PD2, Unsupported DMA
} com_typedef_enum;
#define CCU_DEBUG_COM  CCU_DEBUG_COM1	

#define Uart_TX(com, pData, len) Uart_Tx(Uart_Get(com), pData, len)

typedef struct
{
	//以下部分是Uart的配置信息，在运行过程中不能修改
	com_typedef_enum com;
	uint32 enablePin;		//使能IO，可用于485，来自于初始化； 0：表示不需要使能IO	
	//uint32 uartIRQn;
	//rcu_periph_enum gpioClk;
	//rcu_periph_enum comClk;

	uint32 txPort;
	uint32 txPin;

	uint32 rxPort;
	uint32 rxPin;

	int txBufSize;
	int rxBufSize;

	uint32 txDma;
	//rcu_periph_enum rcu;
	dma_channel_enum txDmaChl;
	//uint32 dmaIRQn;

	/******************************************************************/
	//以下部分是Uart的运行信息，在运行过程中可以修改
	DrvIo* enableIo;	//指向enablePin的对象, 来自初始化函数， Null：表示无效

	uint16 txLen;
	SafeBuf txBuf;
	SafeBuf rxBuf;
}Uart;

Uart* Uart_Get(com_typedef_enum com);
Uart* Uart_GetByDma(uint32 dma, dma_channel_enum channel);
void DMA_Channel_IRQHandler(Uart* pUart);
void Uart_RxIRQHandler(Uart* pUart);
void Uart_TxIRQHandler(Uart* pUart);

void Uart_TxByDma(Uart* pUart);
void Uart_Tx(Uart* pUart, const uint8* pData, int len);
void Uart_Start(const Uart* pUart);
void Uart_Stop(Uart* pUart);
Uart* Uart_Init(com_typedef_enum com);

/*!
	\brief     configure USART parity
	\param[in] usart_periph: USARTx(x=0,1,2)/UARTx(x=3,4)
	\param[in]  baudval: baud rate value
	\param[in] paritycfg: configure USART parity
			   only one parameter can be selected which is shown as below:
	  \arg       USART_PM_NONE: no parity
	  \arg       USART_PM_ODD:  odd parity
	  \arg       USART_PM_EVEN: even parity
	\param[out] none
	\retval     none
*/
Uart* Uart_InitEx(com_typedef_enum com, uint32_t baudval, uint32_t paritycfg);

void Uart_SwitchIO(MultiCom multiCom);
#endif //_DRIVER_UART_H
