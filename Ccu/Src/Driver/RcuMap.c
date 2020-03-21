
#include "Common.h"
#include "RcuMap.h"

struct
{
	uint32 periph;
	rcu_periph_enum rcu;
	IRQn_Type IRQn;
}
static const rcuPortMap[] =
{
	{GPIOA, RCU_GPIOA},
	{GPIOB, RCU_GPIOB},
	{GPIOC, RCU_GPIOC},
	{GPIOD, RCU_GPIOD},
	{GPIOE, RCU_GPIOE},
	{GPIOF, RCU_GPIOF},
	{GPIOG, RCU_GPIOG},
	{DMA0 , RCU_DMA0},
	{DMA1 , RCU_DMA1},
	{TIMER0 , RCU_TIMER0},
	{TIMER2 , RCU_TIMER2},
	{TIMER3 , RCU_TIMER3},
	{USART0, RCU_USART0, USART0_IRQn},
	{USART1, RCU_USART1, USART1_IRQn},
	{USART2, RCU_USART2, USART2_IRQn},
	{UART3 , RCU_UART3 , UART3_IRQn },
	{UART4 , RCU_UART4 , UART4_IRQn },
	{CAN0  , RCU_CAN0  , CAN0_RX0_IRQn},
	{CAN1  , RCU_CAN1  , CAN1_RX0_IRQn},
};

rcu_periph_enum Rcu_Get(uint32 periph)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(rcuPortMap); i++)
	{
		if (rcuPortMap[i].periph == periph) return rcuPortMap[i].rcu;
	}
	Assert(False);

	//欺骗编译器不高景，不可能运行到此
	return RCU_SPI0;
}

IRQn_Type IRQn_Get(uint32 port)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(rcuPortMap); i++)
	{
		if (rcuPortMap[i].periph == port) return rcuPortMap[i].IRQn;
	}

	Assert(False);
	return USBFS_IRQn;	//Force to No coplier warning，不可能运行到此
}

uint32 Dma_GetIRQn(uint32 dma, dma_channel_enum channel)
{
	struct
	{
		uint32 dma;
		dma_channel_enum channel;
		uint32 dmaIRQn;
	}
	static const dmaMap[] =
	{
		{DMA0, DMA_CH3, DMA0_Channel3_IRQn},
		{DMA0, DMA_CH6, DMA0_Channel6_IRQn},
		{DMA0, DMA_CH1, DMA0_Channel1_IRQn},
		{DMA1, DMA_CH4, DMA1_Channel4_IRQn},
	};
	for (int i = 0; i < GET_ELEMENT_COUNT(dmaMap); i++)
	{
		if (dmaMap[i].dma == dma && dmaMap[i].channel == channel)
		{
			return dmaMap[i].dmaIRQn;
		}
	}
	return 0;
}


uint32 EXTI_GetIRQn(exti_line_enum line)
{
	struct
	{
		exti_line_enum line;
		uint32 IRQn;
	}
	static const exti[] =
	{
		{EXTI_0, EXTI0_IRQn},
		{EXTI_1, EXTI1_IRQn},
		{EXTI_2, EXTI2_IRQn},
		{EXTI_3, EXTI3_IRQn},
		{EXTI_4, EXTI4_IRQn},
		{EXTI_5, EXTI5_9_IRQn},
		{EXTI_6, EXTI5_9_IRQn},
		{EXTI_7, EXTI5_9_IRQn},
		{EXTI_8, EXTI5_9_IRQn},
		{EXTI_9, EXTI5_9_IRQn},
		{EXTI_10, EXTI10_15_IRQn},
		{EXTI_11, EXTI10_15_IRQn},
		{EXTI_12, EXTI10_15_IRQn},
		{EXTI_13, EXTI10_15_IRQn},
		{EXTI_14, EXTI10_15_IRQn},
		{EXTI_15, EXTI10_15_IRQn},
	};
	for (int i = 0; i < GET_ELEMENT_COUNT(exti); i++)
	{
		if (exti[i].line == line)
		{
			return exti[i].IRQn;
		}
	}
	return 0;
}

