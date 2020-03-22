
#include "gd32f403.h"
#include "Common.h"
#include "DriverUart.h"

//USART0 Driver ===================================================================
//static Uart g_Usart[] =
//{
//#ifdef GD32F30X_HD
//	 {CCU_RESERVED_COM , USART0_IRQn, RCU_GPIOA, RCU_USART0, GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_10 , 256 , 64 , DMA0, RCU_DMA0, DMA_CH3, DMA0_Channel3_IRQn}
////	 {CCU_RESERVED_COM , USART0_IRQn, RCU_GPIOA, RCU_USART0, GPIOA, GPIO_PIN_9, GPIOA, GPIO_PIN_10 , 256 , 64 , DMA0, RCU_DMA0, DMA_CH3, DMA0_Channel3_IRQn}
//#else
//	 {CCU_RESERVED_COM , USART0_IRQn, RCU_GPIOB, RCU_USART0, GPIOB, GPIO_PIN_6 , GPIOB, GPIO_PIN_7 , 64  , 64 , DMA0, RCU_DMA0, DMA_CH3, DMA0_Channel3_IRQn}
//#endif
//	,{CCU_MISC_COM     , USART1_IRQn, RCU_GPIOA, RCU_USART1, GPIOA, GPIO_PIN_2 , GPIOA, GPIO_PIN_3 , 256 , 256, DMA0, RCU_DMA0, DMA_CH6, DMA0_Channel6_IRQn}
//	,{CCU_METER_COM      , USART2_IRQn, RCU_GPIOD, RCU_USART2, GPIOD, GPIO_PIN_8 , GPIOD, GPIO_PIN_9 , 256 , 256, DMA0, RCU_DMA0, DMA_CH1, DMA0_Channel1_IRQn}
//	,{CCU_SM_COM       , UART3_IRQn, RCU_GPIOC, RCU_UART3 , GPIOC, GPIO_PIN_10, GPIOC, GPIO_PIN_11, 256 , 256 , DMA1, RCU_DMA1, DMA_CH4, DMA1_Channel3_Channel4_IRQn}
//	,{CCU_DEBUG_COM    , UART4_IRQn, RCU_GPIOC, RCU_UART4 , GPIOC, GPIO_PIN_12, GPIOD, GPIO_PIN_2 , 256 , 64}	//UART4 不支持DMA传输
//};
static Uart g_Usart[] =
{
	 {CCU_MISC_COM     , CTRL_485_LOCK_ENABLE	, GPIOB, GPIO_PIN_10, GPIOB, GPIO_PIN_11, 32 , 64}//, DMA0, DMA_CH6 }
	,{CCU_METER_COM    , CTRL_485_METER_ENABLE	, GPIOA, GPIO_PIN_2 , GPIOA, GPIO_PIN_3 , 32 , 64}//, DMA0, DMA_CH1 }	//
	,{CCU_COOLING_COM  , CTRL_485_COOLING_ENABLE, GPIOA, GPIO_PIN_9 , GPIOA, GPIO_PIN_10, 32 , 32}//, DMA1, DMA_CH4 }
	,{CCU_DEBUG_COM    , 0						, GPIOC, GPIO_PIN_12, GPIOD, GPIO_PIN_2 , 256 , 64}	//UART4 不支持DMA传输
};


#if 1
#define DMA_CHANNEL_IRQ_HANDLER(dma, dmaChnl) \
	static Uart* pUart = Null;	\
	if (pUart == Null)	\
	{	\
		pUart = Uart_GetByDma(dma, dmaChnl);	\
	}	\
	if (dma_interrupt_flag_get(pUart->txDma, pUart->txDmaChl, DMA_INT_FLAG_FTF))	\
	{	\
		dma_interrupt_flag_clear(pUart->txDma, pUart->txDmaChl, DMA_INT_FLAG_FTF);	\
		DMA_Channel_IRQHandler(pUart);	\
	}
#else
void DMA_CHANNEL_IRQ_HANDLER(uint32 dma, dma_channel_enum dmaChnl)
{
	static Usart* pUart = Null;
	if (pUart == Null)
	{
		pUart = Uart_GetByDma(dma, dmaChnl);
	}
	if (dma_interrupt_flag_get(pUart->txDma, pUart->txDmaChl, DMA_INT_FLAG_FTF))
	{
		dma_interrupt_flag_clear(pUart->txDma, pUart->txDmaChl, DMA_INT_FLAG_FTF);
		DMA_Channel_IRQHandler(pUart);
	}
}
#endif
/*!
	\brief      this function handles DMA_Channel1_2_IRQHandler interrupt
	\param[in]  none
	\param[out] none
	\retval     none
*/
void DMA0_Channel6_IRQHandler(void)
{
	DMA_CHANNEL_IRQ_HANDLER(DMA0, DMA_CH6);
}
void DMA0_Channel1_IRQHandler(void)
{
	DMA_CHANNEL_IRQ_HANDLER(DMA0, DMA_CH1);
}
void DMA0_Channel4_IRQHandler(void)
{
	DMA_CHANNEL_IRQ_HANDLER(DMA0, DMA_CH4);
}
void DMA0_Channel3_IRQHandler(void)
{
	DMA_CHANNEL_IRQ_HANDLER(DMA0, DMA_CH3);
}
void DMA1_Channel4_IRQHandler(void)
{
	DMA_CHANNEL_IRQ_HANDLER(DMA1, DMA_CH4);
}

void Uart_IrqHandler(Uart* pUart)
{
	if ((RESET != usart_interrupt_flag_get(pUart->com, USART_INT_FLAG_RBNE)) && (RESET != usart_flag_get(pUart->com, USART_FLAG_RBNE)))
	{
		//Rx irq
		Uart_RxIRQHandler(pUart);
	}
	if ((RESET != usart_flag_get(pUart->com, USART_FLAG_TBE)) && (RESET != usart_interrupt_flag_get(pUart->com, USART_INT_FLAG_TBE)))
	{	//Tx irq
		Uart_TxIRQHandler(pUart);
	}
#if 1
	if ((RESET != usart_flag_get(pUart->com, USART_FLAG_TC)) && (RESET != usart_interrupt_flag_get(pUart->com, USART_INT_FLAG_TC)))
	{
		DrvIo* enableIo = pUart->enableIo;
		if(pUart->com != CCU_DEBUG_COM)
		{
			Printf("TC\n");
		}
		if (enableIo) PortPin_Set(enableIo->periph, enableIo->pin, 0);
		usart_interrupt_disable(pUart->com, USART_INT_TC);

		//usart_interrupt_flag_clear(pUart->com, USART_INT_FLAG_TC);
	}
#endif
}
#define UART_IRQ_HANDLER(com)	\
	static Uart* pUart = Null;	\
	if (pUart == Null)	\
	{	\
		pUart = Uart_Get((com_typedef_enum)com);	\
	}	\
	Uart_IrqHandler(pUart);

void USART0_IRQHandler(void)
{
	UART_IRQ_HANDLER(USART0);
}

void USART1_IRQHandler(void)
{
	UART_IRQ_HANDLER(USART1);
}

void USART2_IRQHandler(void)
{
	UART_IRQ_HANDLER(USART2);
}

void UART3_IRQHandler(void)
{
	UART_IRQ_HANDLER(UART3);
}

void UART4_IRQHandler(void)
{
	UART_IRQ_HANDLER(UART4);
}

//该串口和Meter口复用
static Bool g_MultiCom = False;
void Uart_SwitchIO(MultiCom multiCom)
{
	static Uart g_MeterComCfg = { CCU_MISC_COM, GPIOD, GPIO_PIN_5 , GPIOD, GPIO_PIN_6};
	if (g_MultiCom == multiCom) return;

	//去配置原来的IO
	Uart* pUart = (g_MultiCom == MISC_COM) ? Uart_Get(CCU_MISC_COM) : &g_MeterComCfg;
	Uart_Stop(pUart);
	gpio_deinit(pUart->txPort);
	gpio_deinit(pUart->rxPort);

	//配置新的IO
	pUart = (multiCom == MISC_COM) ? Uart_Get(CCU_MISC_COM) : &g_MeterComCfg;
	gpio_init(pUart->txPort, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pUart->txPin);
	gpio_init(pUart->rxPort, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pUart->rxPin);
	Uart_Start(pUart);

	g_MultiCom = multiCom;
}

Uart* Uart_Get(com_typedef_enum com)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Usart); i++)
	{
		if (com == g_Usart[i].com)
		{
			return &g_Usart[i];
		}
	}

	return 0;
}

Uart* Uart_GetByDma(uint32 dma, dma_channel_enum channel)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Usart); i++)
	{
		if (dma == g_Usart[i].txDma && channel == g_Usart[i].txDmaChl)
		{
			return &g_Usart[i];
		}
	}

	return 0;
}

void Uart_Tx(Uart* pUart, const uint8 * pData, int len)
{
	DrvIo* enableIo = pUart->enableIo;
	if (enableIo) PortPin_Set(enableIo->periph, enableIo->pin, 1);
#if	0 // UART_ASYNC_TRANS
		if (SafeBuf_Write(&pUart->txBuf, pData, len) != len)
		{
			Printf("USART[%x] buffer full.", pUart->com);
			//Assert(False);
		}
		if (pUart->txDma)
		{
			Uart_TxByDma(pUart);
		}
		else
		{
			usart_interrupt_enable(pUart->com, USART_INT_TBE);
		}
#else
	if (pUart->txDma)
	{
		if (SafeBuf_Write(&pUart->txBuf, pData, len) != len)
		{
			Printf("USART[%x] buffer full.", pUart->com);
			//Assert(False);
		}
		Uart_TxByDma(pUart);
	}
	else
	{
		//usart_interrupt_enable(pUart->com, USART_INT_TC);
		for (int i = 0; i < len; i++, pData++)
		{
			usart_data_transmit(pUart->com, *pData);
			while (RESET == usart_flag_get(pUart->com, USART_FLAG_TBE));
		}
	}	
#endif
	
	while (RESET == usart_flag_get(pUart->com, USART_FLAG_TC));
	if (enableIo) PortPin_Set(enableIo->periph, enableIo->pin, 0);
}

//配置Dma模式
void Uart_InitDma(const Uart* pUart)
{
	uint32 dma = pUart->txDma;
	dma_channel_enum dmaChl = pUart->txDmaChl;

	rcu_periph_clock_enable(Rcu_Get(dma));

	dma_parameter_struct dma_init_struct = { 0 };
	memset(&dma_init_struct, 0, sizeof(dma_parameter_struct));
	dma_deinit(dma, dmaChl);
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.periph_addr = (uint32)&USART_DATA(pUart->com);// + 0x04;
	dma_init_struct.number = 0;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	dma_init(dma, dmaChl, &dma_init_struct);

	/* configure DMA mode */
	dma_circulation_disable(dma, dmaChl);
	dma_memory_to_memory_disable(dma, dmaChl);

	nvic_irq_enable(Dma_GetIRQn(dma, dmaChl), 0, 0);

	/* USART DMA enable for transmission and reception */
	usart_dma_transmit_config(pUart->com, USART_DENT_ENABLE);

	///* wait until USART0 TX DMA0 channel transfer complete */
	//while (RESET == dma_flag_get(DMA0, pUart->txDmaChl, DMA_INTF_FTFIF)) {}
}

void Uart_TxData(Uart* pUart)
{
	uint8 txByte = 0;
	/* transmit data */
	SafeBuf_Read(&pUart->txBuf, &txByte, sizeof(txByte));
	usart_data_transmit(pUart->com, txByte);
}

void Uart_TxByDma(Uart* pUart)
{
	uint32 dma = pUart->txDma;
	dma_channel_enum chnl = pUart->txDmaChl;

	if (dma_transfer_number_get(dma, chnl))
	{
		//正在传输
		return;
	}

	pUart->txLen = SafeBuf_GetCount(&pUart->txBuf);

	DMA_CHCTL(dma, chnl) &= ~(DMA_CHXCTL_CHEN | DMA_CHXCTL_FTFIE);
	//DMA_CHCTL(dma, chnl) = DMA_CHCTL_RESET_VALUE;

	/* configure memory base address */
	//DMA_CHMADDR(dma, chnl) = (uint32)SafeBuf_GetData(&pUart->txBuf, (int *)&pUart->txLen);
	DMA_CHMADDR(dma, chnl) = (uint32)&pUart->txBuf.m_pBuf[pUart->txBuf.m_ReadIndx];

	/* configure the number of remaining data to be transferred */
	DMA_CHCNT(dma, chnl) = (pUart->txLen & DMA_CHANNEL_CNT_MASK);

	///* enable DMA channel3 */
	dma_channel_enable(dma, chnl);
	dma_interrupt_enable(dma, chnl, DMA_INT_FLAG_FTF);

	/* wait DMA Channel transfer complete */
	//while (RESET == dma_flag_get(DMA0, DMA_CH3, DMA_INTF_FTFIF));
}

void DMA_Channel_IRQHandler(Uart* pUart)
{
	//int len = dma_transfer_number_get(pUart->txDma, pUart->txDmaChl);
	SafeBuf_Read(&pUart->txBuf, Null, pUart->txLen);

	//如果缓冲区中还有数据，则继续发送
	if (!SafeBuf_IsEmpty(&pUart->txBuf))
	{
		Uart_TxByDma(pUart);
	}
}

void Uart_RxIRQHandler(Uart* pUart)
{
#if 1
	SafeBuf_WriteByte(&pUart->rxBuf, (uint8)usart_data_receive(pUart->com));
#else
	static uint8 len = 0;
	static uint8 buf[64];

	buf[len] = (usart_data_receive(pUart->com) & 0x7F);
	if (buf[len] == '\r' || len == sizeof(buf) - 1)
	{
		len = 0;
		SafeBuf_Write(&pUart->rxBuf, buf, len);
	}
	else
	{
		len++;
	}
#endif
}

void Uart_TxIRQHandler(Uart* pUart)
{
	if (!SafeBuf_IsEmpty(&pUart->txBuf))
	{
		uint8 txByte = 0;
		/* transmit data */
		SafeBuf_Read(&pUart->txBuf, &txByte, sizeof(txByte));
		usart_data_transmit(pUart->com, txByte);
	}
	else
	{
		usart_interrupt_disable(pUart->com, USART_INT_TBE);
	}
}

void Uart_Start(const Uart* pUart)
{
	nvic_irq_enable(IRQn_Get(pUart->com), 0, 0);
	usart_interrupt_enable(pUart->com, USART_INT_TBE);
	usart_interrupt_enable(pUart->com, USART_INT_RBNE);
	//usart_interrupt_enable(pUart->com, USART_INT_TC);
	usart_enable(pUart->com);
}

void Uart_Stop(Uart* pUart)
{
	uint32 dma = pUart->txDma;
	dma_channel_enum dmaChl = pUart->txDmaChl;

	usart_disable(pUart->com);

	if (dma)
	{
		dma_interrupt_disable(dma, dmaChl, DMA_INT_FLAG_FTF);
		DMA_CHCNT(dma, dmaChl) = (0 & DMA_CHANNEL_CNT_MASK);
		dma_channel_disable(dma, dmaChl);
	}
	else
	{
		usart_interrupt_disable(pUart->com, USART_INT_RBNE);
		usart_interrupt_disable(pUart->com, USART_INT_TBE);
	}
	SafeBuf_Reset(&pUart->rxBuf);
	SafeBuf_Reset(&pUart->txBuf);
}

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
Uart* Uart_InitEx(com_typedef_enum com, uint32_t baudval, uint32_t paritycfg)
{
	Uart* pUart = Uart_Get(com);

	void* rxBuff = malloc(pUart->rxBufSize);
	void* txBuff = malloc(pUart->txBufSize);
	Assert(rxBuff != Null);
	Assert(txBuff != Null);

	//Config usart tx buffer and rx buffer
	SafeBuf_Init((SafeBuf*)&pUart->rxBuf, rxBuff, pUart->rxBufSize);
	SafeBuf_Init((SafeBuf*)&pUart->txBuf, txBuff, pUart->txBufSize);

	//Config com IO and click
	rcu_periph_clock_enable(Rcu_Get(pUart->txPort));
	if (pUart->txPort != pUart->rxPort)
	{
		rcu_periph_clock_enable(Rcu_Get(pUart->rxPort));
	}

	/* enable USART clock */
	rcu_periph_clock_enable(Rcu_Get(pUart->com));

	/* connect port to USARTx_Tx */
	gpio_init(pUart->txPort, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pUart->txPin);

	/* connect port to USARTx_Rx */
	gpio_init(pUart->rxPort, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pUart->rxPin);

	/* USART configure */
	usart_deinit(com);
	usart_baudrate_set(com, baudval);	//pUart->baudval
	usart_parity_config(com, paritycfg);

	//如果使能校验位，数据位必须设置位9BIT
	if (paritycfg != USART_PM_NONE)
	{
		usart_word_length_set(com, USART_WL_9BIT);
	}
	usart_receive_config(com, USART_RECEIVE_ENABLE);
	usart_transmit_config(com, USART_TRANSMIT_ENABLE);

	//Config usart tx dma
	if (pUart->txDma)
	{
		Uart_InitDma(pUart);
	}
	
	pUart->enableIo = pUart->enablePin ? IO_Get(pUart->enablePin) : 0;
	return pUart;
}

Uart* Uart_Init(com_typedef_enum com)
{
	return Uart_InitEx(com, 115200U, USART_PM_NONE);
	
}
