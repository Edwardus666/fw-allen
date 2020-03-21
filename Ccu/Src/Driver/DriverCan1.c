
#include "Common.h"
#include "DriverCan.h"

static CanBus g_CanBuses[2] =
{
	{CAN0, GPIOB, GPIO_PIN_9, GPIO_PIN_8},
	{CAN1, GPIOB, GPIO_PIN_6, GPIO_PIN_5}
};

void MailBox_Start(MailBox* pMailBox, CanBusTxDoneFn TxDone, void* pCallBackParam)
{
	pMailBox->state = TX_WAITTING;
	pMailBox->txState = TX_SUCCESS;
	pMailBox->TxDone = TxDone;
	pMailBox->pCallBackParam = pCallBackParam;

	//启动100ms定时器，如果超时则发送失败
	//SwTimer_Start(&pMailBox->timer, 2000, 0);
}
void MailBox_Free(MailBox* pMailBox)
{
	memset(pMailBox, 0, sizeof(MailBox));
	SwTimer_Stop(&pMailBox->timer);
}
/*!
	\brief      this function handles CAN0 RX0 exception
	\param[in]  none
	\param[out] none
	\retval     none
*/
void CAN0_RxFromFifo(CanBus* pCanBus, can_receive_message_struct* pRxMsg, int fifoNo)
{
	/* check the receive message */
	int depth = can_receive_message_length_get(pCanBus->canPort, fifoNo);
	for (int i = 0; i < depth; i++)
	{
		can_message_receive(pCanBus->canPort, fifoNo, pRxMsg);

		if (CAN_FF_STANDARD == pRxMsg->rx_ff)
		{
			if (!QUEUE_add(&pCanBus->rxBufQueue, pRxMsg, sizeof(can_receive_message_struct)))
			{
				PFL_ERROR("CAN rx buffer full! \n");
			}
		}
	}
}

void CAN0_RX0_IRQHandler(void)
{
	static can_receive_message_struct rxMsg;
	CAN0_RxFromFifo(&g_CanBuses[0], &rxMsg, CAN_FIFO0);
}

void CAN0_RX1_IRQHandler(void)
{
	static can_receive_message_struct rxMsg;
	CAN0_RxFromFifo(&g_CanBuses[0], &rxMsg, CAN_FIFO1);
}

void CAN0_TXIRQHandler(CanBus* pCanBus, uint8 mailBox, TxState txState)
{
	MailBox* pMailBox = &pCanBus->mailBox[mailBox];

	if (pMailBox->state == TX_WAITTING)
	{
		pMailBox->state = TX_DONE;
		pMailBox->txState = txState;
	}
}

void CAN0_ERR_IRQHandler(void)
{
	//int errNo = 0;
	uint32 can_periph = g_CanBuses[0].canPort;
	MailBox* pMailBox = g_CanBuses[0].mailBox;
	//检查是否有error interrupt flag
	if (SET == can_interrupt_flag_get(can_periph, CAN_INT_FLAG_ERRIF))
	{
		//errNo = CAN_ERR(can_periph) & (CAN_ERR_BOERR | CAN_ERR_PERR | CAN_ERR_WERR);
		//当CAN口没接线时，canErr = 1， errRCnt++;
		can_error_enum canErr = can_error_get(can_periph);
		uint32 errRCnt = can_receive_error_number_get(can_periph);
		uint32 errTCnt = can_transmit_error_number_get(can_periph);
		if (can_flag_get(can_periph, CAN_FLAG_BOERR))
			can_flag_clear(can_periph, CAN_FLAG_BOERR);
		if (can_flag_get(can_periph, CAN_FLAG_PERR))
			can_flag_clear(can_periph, CAN_FLAG_PERR);
		if (can_flag_get(can_periph, CAN_FLAG_WERR))
			can_flag_clear(can_periph, CAN_FLAG_WERR);
		if (errRCnt >= 128)
		{
			//Printf("ERRIF=0x%x,errTCnt=%d,errRCnt=%d, canErr=%d\n", errNo, errTCnt, errRCnt, canErr);
			for (int i = CAN_MAILBOX0; i <= CAN_MAILBOX2; i++, pMailBox++)
			{
				if (TX_WAITTING == pMailBox->state)
				{
					//CAN0_TXIRQHandler(&g_CanBuses[0], i, TX_FAILED);
					can_transmission_stop(can_periph, pMailBox->number);
				}
			}
		}

		can_interrupt_flag_clear(can_periph, CAN_INT_FLAG_ERRIF);
	}
}

void CAN0_TX_IRQHandler(void)
{
	uint32 can_periph = g_CanBuses[0].canPort;
	can_transmit_state_enum ret;
	const static can_interrupt_flag_enum intflag[] = { CAN_INT_FLAG_MTF0 ,CAN_INT_FLAG_MTF1 ,CAN_INT_FLAG_MTF2 };
//	const static can_flag_enum mteflag[] = { CAN_FLAG_MTE0 ,CAN_FLAG_MTE1 ,CAN_FLAG_MTE2 };
//	int errNo = 0;
	//检查是否mailbox transmit finished interrupt flag
	for (int mailbox = CAN_MAILBOX0; mailbox <= CAN_MAILBOX2; mailbox++)
	{
		if (RESET == can_interrupt_flag_get(can_periph, intflag[mailbox])) continue;
		
		ret = can_transmit_states(can_periph, mailbox);
		if (ret == CAN_TRANSMIT_OK)
		{
			CAN0_TXIRQHandler(&g_CanBuses[0], mailbox, TX_SUCCESS);
		}
		else if (ret == CAN_TRANSMIT_FAILED)
		{
			//获取失败原因
			uint32 errNo = CAN_ERR(can_periph);	// & (CAN_ERR_BOERR | CAN_ERR_PERR | CAN_ERR_WERR)
			uint32 errTCnt = can_transmit_error_number_get(can_periph);
			uint32 errRCnt = can_receive_error_number_get(can_periph);
			can_error_enum canErr = can_error_get(can_periph);
			PFL(DL_CAN_MASTER, "canErr=0x%x, ERRIF=0x%x,errTCnt=%d,errRCnt=%d\n", canErr, errNo, errTCnt, errRCnt);
			CAN0_TXIRQHandler(&g_CanBuses[0], mailbox, TX_FAILED);
		}

		can_interrupt_flag_clear(can_periph, intflag[mailbox]);
	}
}

void CAN0_EWMC_IRQHandler(void)
{
	CAN0_ERR_IRQHandler();
}

void CanBus_TxDone(CanBus* pCanBus, MailBox* pMailBox, TxState txState)
{
	if (pMailBox->state == MS_IDLE) return;

//	PFL(DL_CAN_MASTER, "Can[0] tx state = %d.\n", txState);

	pMailBox->txState = txState;
	if (pMailBox->TxDone)
	{
		pMailBox->TxDone(pMailBox->pCallBackParam, pMailBox->txState);
	}
	MailBox_Free(pMailBox);
}

Bool CanBus_isIdle(const CanBus* pCanBus)
{
	//for (int i = 0; i < GET_ELEMENT_COUNT(pCanBus->mailBox); i++)
	//{
	//	if (pCanBus->mailBox[i].state == MS_IDLE) return True;
	//}

	uint32 can_periph = pCanBus->canPort;
	uint8_t mailbox_number = CAN_MAILBOX0;

	/* select one empty mailbox */
	if (CAN_TSTAT_TME0 == (CAN_TSTAT(can_periph) & CAN_TSTAT_TME0)) {
		mailbox_number = CAN_MAILBOX0;
	}
	else if (CAN_TSTAT_TME1 == (CAN_TSTAT(can_periph) & CAN_TSTAT_TME1)) {
		mailbox_number = CAN_MAILBOX1;
	}
	else if (CAN_TSTAT_TME2 == (CAN_TSTAT(can_periph) & CAN_TSTAT_TME2)) {
		mailbox_number = CAN_MAILBOX2;
	}
	else {
		mailbox_number = CAN_NOMAILBOX;
	}

	return (CAN_NOMAILBOX != mailbox_number);
}

Bool CanBus_Tx(CanBus* pCanBus, uint16 sfid, uint8* pData, int len, CanBusTxDoneFn txDone, void* pCallBackParam)
{
	//Printf("Can[0] Tx Data: %02X", sfid); DUMP_BYTE(pData, len); Printf("\n");

	CanTxMsgObj msgObj;
	can_trasnmit_message_struct* pMsg = &msgObj.msg;
	msgObj.callBackParam = pCallBackParam;
	msgObj.TxDone = txDone;

	pMsg->tx_ft = CAN_FT_DATA;
	pMsg->tx_ff = CAN_FF_STANDARD;
	pMsg->tx_sfid = sfid;
	pMsg->tx_dlen = len;
	if(len) memcpy(pMsg->tx_data, pData, len);
	if (!QUEUE_add(&pCanBus->txBufQueue, &msgObj, sizeof(CanTxMsgObj)))
	{
		PFL_WARNING("Can[0] Tx buffer full.\n");
		return False;
	}
	return True;
}

void CanBus_GpioInit(CanBus* pCanBus)
{
	//Config can clock and gpio
	rcu_periph_clock_enable(Rcu_Get(pCanBus->canPort));
	rcu_periph_clock_enable(Rcu_Get(pCanBus->ioPort));
	rcu_periph_clock_enable(RCU_AF);

	gpio_init(pCanBus->ioPort, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, pCanBus->rxPin);
	gpio_init(pCanBus->ioPort, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pCanBus->txPin);

	gpio_pin_remap_config(GPIO_CAN0_PARTIAL_REMAP, ENABLE);	//pCanBus->canPort == CAN0 ? GPIO_CAN0_FULL_REMAP : GPIO_CAN1_REMAP
}

void CanBus_config(CanBus* pCanBus, can_parameter_struct* param, can_filter_parameter_struct* filter)
{
	/* initialize CAN register */
	can_deinit(pCanBus->canPort);
	can_init(pCanBus->canPort, param);
	can_filter_init(filter);
}

uint32 CanBus_CalcPrescaler(uint32 baudrate)
{
	struct
	{
		uint32 baudrate;
		uint32 prescaler;
	}
	static const bds[] =
	{
		 {1000, 7}	//1M
		,{500, 14}	//500k
		,{250, 28}	//250K
		,{125, 56}
		,{100, 70}
		,{50, 140}
		,{20, 350}
	};
	for (int i = 0; i < GET_ELEMENT_COUNT(bds); i++)
	{
		if (bds[i].baudrate == baudrate) return bds[i].prescaler;
	}

	Assert(False);
	return 0;
}

Bool CanBus_CopyMsgToMailBox(CanBus* pCanBus, const CanTxMsgObj* pMsg)
{
	if (CanBus_isIdle(pCanBus) && pMsg->msg.tx_dlen)	//不打印SYNC
	{
		PFL(DL_CAN_MASTER, "Can[0] Tx Data[%02X]: ", pMsg->msg.tx_sfid); DUMP_BYTE(pMsg->msg.tx_data, pMsg->msg.tx_dlen);
	}
	else return False;

	uint8 number = can_message_transmit(pCanBus->canPort, &pMsg->msg);
	MailBox* pMailBox = &pCanBus->mailBox[number];
	if (number != CAN_NOMAILBOX)
	{
		MailBox_Start(pMailBox, pMsg->TxDone, pMsg->callBackParam);
	}

	return (number != CAN_NOMAILBOX);
}

void CanBus_TxMsgQueueToMailBox(CanBus* pCanBus)
{
	//是否有待发数据在队列中
	CanTxMsgObj* pMsg;
	Queue* pQueue = &pCanBus->txBufQueue;
	for (pMsg = (CanTxMsgObj*)QUEUE_getHead(pQueue)
		; pMsg != Null
		; QUEUE_removeHead(pQueue), pMsg = (CanTxMsgObj*)QUEUE_getHead(pQueue))
	{
		if (!CanBus_CopyMsgToMailBox(pCanBus, pMsg))
		{
			break;
		}
	}
}

//中断标志数组
const static uint32 g_CanIntFlag[] = {
	CAN_INT_TME,	//transmit mailbox empty interrupt enable ; ISR-> CAN0_TX_IRQHandler
	CAN_INT_RFNE0 ,	//receive FIFO0 not empty interrupt enable; ISR-> CAN0_RX0_IRQHandler
	CAN_INT_RFNE1 ,	//receive FIFO1 not empty interrupt enable; ISR-> CAN0_RX1_IRQHandler
	CAN_INT_BO,		//bus-off interrupt enable;					ISR-> CAN0_EWMC_IRQn		
	CAN_INT_ERRN,	//error number interrupt enable;			ISR-> CAN0_EWMC_IRQn
	CAN_INT_ERR		//error interrupt enable;					ISR-> CAN0_EWMC_IRQn
};

void CanBus_Stop(CanBus* pCanBus)
{
	uint32 can_periph = pCanBus->canPort;
	pCanBus->isStart = False;

	//失能中断
	nvic_irq_disable(IRQn_Get(can_periph));
	for (int i = 0; i < GET_ELEMENT_COUNT(g_CanIntFlag); i++)
	{
		can_interrupt_disable(can_periph, g_CanIntFlag[i]);//失能fifo_0 empty中断
	}

	//停止SYNC帧定时器
	SwTimer_Stop(&pCanBus->syncFrametimer);

	//清除接收FIFO
	can_fifo_release(can_periph, CAN_INTEN_RFNEIE0);
	can_fifo_release(can_periph, CAN_INTEN_RFNEIE1);

	//结束已经正在发送的数据
	MailBox* pMailBox = pCanBus->mailBox;
	for (int i = 0; i < GET_ELEMENT_COUNT(pCanBus->mailBox); i++, pMailBox++)
	{
		can_transmission_stop(can_periph, pMailBox->number);
		CanBus_TxDone(pCanBus, pMailBox, TX_ABORT);
	}

	//结束等待在队列中的数据
	CanTxMsgObj* pMsg;
	Queue* pQueue = &pCanBus->txBufQueue;
	for (pMsg = (CanTxMsgObj*)QUEUE_getHead(pQueue)
		; pMsg != Null
		; QUEUE_removeHead(pQueue), pMsg = (CanTxMsgObj*)QUEUE_getHead(pQueue))
	{
		if (pMsg->TxDone)
		{
			pMsg->TxDone(pMsg->callBackParam, TX_ABORT);
		}
	}

}
void CanBus_Start(CanBus* pCanBus)
{
	pCanBus->isStart = True;

	//使能中断
	nvic_irq_enable(CAN0_TX_IRQn, 0, 0);
	nvic_irq_enable(CAN0_EWMC_IRQn, 0, 1);
	nvic_irq_enable(CAN0_RX0_IRQn, 0, 2);
	nvic_irq_enable(CAN0_RX1_IRQn, 0, 3);

	for (int i = 0; i < GET_ELEMENT_COUNT(g_CanIntFlag); i++)
	{
		can_interrupt_enable(CAN0, g_CanIntFlag[i]);
	}

	//启动SYNC帧定时器
	SwTimer_Start(&pCanBus->syncFrametimer, 100, 0);
}

void CanBus_Run(CanBus* pCanBus)
{
	if (!pCanBus->isStart) return;

	//检查所有的mailBox，是否有传输完成项
	for (int i = 0; i < GET_ELEMENT_COUNT(pCanBus->mailBox); i++)
	{
		MailBox* pMailBox = &pCanBus->mailBox[i];
		if (pMailBox->state == TX_DONE)
		{
			CanBus_TxDone(pCanBus, pMailBox, pMailBox->txState);
		}
		else if (SwTimer_isTimerOut(&pMailBox->timer))
		{
			//CanBus_TxDone(pCanBus, pMailBox, TX_TIMEOUT);
		}
	}

	//定时发送SYNC帧,最高优先级，优先发送
	if (SwTimer_isTimerOut(&pCanBus->syncFrametimer))
	{
		if (!CanBus_CopyMsgToMailBox(pCanBus, &pCanBus->syncMsg))
		{
			//发送失败,重启一个短时1MS定时器，快速重发。
			SwTimer_Start(&pCanBus->syncFrametimer, 1, 0);
		}
		else
		{
			SwTimer_Start(&pCanBus->syncFrametimer, SYNC_CYCLE_MS, 0);
		}
	}

	//从队列中发送帧
	CanBus_TxMsgQueueToMailBox(pCanBus);
}

CanBus* CanBus_Init(uint8 canId)
{
	CanBus* pCanBus = &g_CanBuses[canId];

	CanBus_GpioInit(pCanBus);

	/* configure CAN NVIC */
	//nvic_irq_enable(IRQn_Get(pCanBus->canPort), pCanBus->priority, pCanBus->priority);

	/* initialize CAN parameters */
	can_parameter_struct can_parameter;
	can_parameter.time_triggered = DISABLE;
	can_parameter.auto_bus_off_recovery = ENABLE;	//总线错误自动恢复
	can_parameter.auto_wake_up = DISABLE;
	can_parameter.no_auto_retrans = DISABLE;	//发送错误自动重传
	//can_parameter.auto_retrans = DISABLE;
	can_parameter.rec_fifo_overwrite = DISABLE;
	can_parameter.trans_fifo_order = DISABLE;
	can_parameter.working_mode = CAN_NORMAL_MODE;
	can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
	can_parameter.time_segment_1 = CAN_BT_BS1_6TQ;
	can_parameter.time_segment_2 = CAN_BT_BS2_5TQ;
	can_parameter.prescaler = CanBus_CalcPrescaler(1000);

	can_deinit(pCanBus->canPort);
	can_init(pCanBus->canPort, &can_parameter);

	/* initialize filter */
	//配置FIFO0
	can_filter_parameter_struct can_filter;
	can_filter.filter_number = 0;
	can_filter.filter_mode = CAN_FILTERMODE_MASK;
	can_filter.filter_bits = CAN_FILTERBITS_32BIT;
	can_filter.filter_list_high = 0x0000;
	can_filter.filter_list_low = 0x0000;
	can_filter.filter_mask_high = 0x0000;	
	can_filter.filter_mask_low = 0x0000;
	can_filter.filter_fifo_number = CAN_FIFO0;	
	can_filter.filter_enable = ENABLE;	//过滤
	can_filter_init(&can_filter);

	//配置FIFO1
	can_filter.filter_number = 1;
	can_filter.filter_fifo_number = CAN_FIFO1;
	//其他配置保持不变
	can_filter_init(&can_filter);

	QUEUE_init(&pCanBus->rxBufQueue, pCanBus->rxMsg, sizeof(can_receive_message_struct), GET_ELEMENT_COUNT(pCanBus->rxMsg));
	QUEUE_init(&pCanBus->txBufQueue, pCanBus->txMsg, sizeof(CanTxMsgObj), GET_ELEMENT_COUNT(pCanBus->txMsg));

	//初始化SYNC帧内容
	static const can_trasnmit_message_struct sync = { 0x80, 0, CAN_FF_STANDARD, 0, 0 };
	memcpy(&pCanBus->syncMsg.msg, &sync, sizeof(sync));

	return pCanBus;
}
