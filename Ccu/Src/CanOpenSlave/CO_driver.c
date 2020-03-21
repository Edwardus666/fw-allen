/*
 * CAN module object for generic microcontroller.
 *
 * This file is a template for other microcontrollers.
 *
 * @file        CO_driver.c
 * @ingroup     CO_driver
 * @author      Janez Paternoster
 * @copyright   2004 - 2015 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free and open source software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Following clarification and special exception to the GNU General Public
 * License is included to the distribution terms of CANopenNode:
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library. Thus, the terms and
 * conditions of the GNU General Public License cover the whole combination.
 *
 * As a special exception, the copyright holders of this library give
 * you permission to link this library with independent modules to
 * produce an executable, regardless of the license terms of these
 * independent modules, and to copy and distribute the resulting
 * executable under terms of your choice, provided that you also meet,
 * for each linked independent module, the terms and conditions of the
 * license of that module. An independent module is a module which is
 * not derived from or based on this library. If you modify this
 * library, you may extend this exception to your version of the
 * library, but you are not obliged to do so. If you do not wish
 * to do so, delete this exception statement from your version.
 */


#include "CO_driver.h"
#include "CO_Emergency.h"
#ifdef bool
#undef bool /*Because there is a definition in gd32f30x_can.h*/
#endif
#include "gd32f403_can.h"

//Modified by Allen
#include "Common.h"
static CO_CANmodule_t* g_pCANmodule;
static int g_offsetFilterNumber;
/******************************************************************************/
void CO_CANsetConfigurationMode(int32_t CANbaseAddress){
    /* Put CAN module in configuration mode */
}


/******************************************************************************/
void CO_CANsetNormalMode(CO_CANmodule_t *CANmodule){
    /* Put CAN module in normal mode */

    CANmodule->CANnormal = true;
	CAN1_Start();
}


/******************************************************************************/
CO_ReturnError_t CO_CANmodule_init(
        CO_CANmodule_t         *CANmodule,
        int32_t                 CANbaseAddress,
        CO_CANrx_t              rxArray[],
        uint16_t                rxSize,
        CO_CANtx_t              txArray[],
        uint16_t                txSize,
        uint16_t                CANbitRate)
{
    uint16_t i;
	g_pCANmodule = CANmodule;
    /* verify arguments */
    if(CANmodule==NULL || rxArray==NULL || txArray==NULL){
        return CO_ERROR_ILLEGAL_ARGUMENT;
    }

    /* Configure object variables */
    CANmodule->CANbaseAddress = CANbaseAddress;
    CANmodule->rxArray = rxArray;
    CANmodule->rxSize = rxSize;
    CANmodule->txArray = txArray;
    CANmodule->txSize = txSize;
    CANmodule->CANnormal = false;
    CANmodule->useCANrxFilters = (rxSize <= 32U) ? true : false;/* microcontroller dependent */
    CANmodule->bufferInhibitFlag = false;
    CANmodule->firstCANtxMessage = true;
    CANmodule->CANtxCount = 0U;
    CANmodule->errOld = 0U;
    CANmodule->em = NULL;

    for(i=0U; i<rxSize; i++){
        rxArray[i].ident = 0U;
        rxArray[i].pFunct = NULL;
    }
    for(i=0U; i<txSize; i++){
        txArray[i].bufferFull = false;
    }

    /**************************************canopen can init*****begin*******************************/
    /* Configure CAN module registers */
    /* initialize CAN and CAN filter*/
    /* Configure CAN timing */
    CAN1_Init();/*Ported by Allen*/
    /*************************************canopen can init****end**********************************/
    
    /* Configure CAN module hardware filters */
    if(CANmodule->useCANrxFilters){
        /* CAN module filters are used, they will be configured with */
        /* CO_CANrxBufferInit() functions, called by separate CANopen */
        /* init functions. */
        /* Configure all masks so, that received message must match filter */
    }
    else{
        /* CAN module filters are not used, all messages with standard 11-bit */
        /* identifier will be received */
        /* Configure mask 0 so, that all messages with standard identifier are accepted */
    }


    /* configure CAN interrupt registers */


    return CO_ERROR_NO;
}


/******************************************************************************/
void CO_CANmodule_disable(CO_CANmodule_t *CANmodule){
    /* turn off the module */
}


/******************************************************************************/
uint16_t CO_CANrxMsg_readIdent(const CO_CANrxMsg_t *rxMsg){
    return (uint16_t) rxMsg->ident;
}


/******************************************************************************/
CO_ReturnError_t CO_CANrxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        uint16_t                mask,
        bool_t                  rtr,
        void                   *object,
        void                  (*pFunct)(void *object, const CO_CANrxMsg_t *message))
{
    CO_ReturnError_t ret = CO_ERROR_NO;

    if((CANmodule!=NULL) && (object!=NULL) && (pFunct!=NULL) && (index < CANmodule->rxSize)){
        /* buffer, which will be configured */
        CO_CANrx_t *buffer = &CANmodule->rxArray[index];

        /* Configure object variables */
        buffer->object = object;
        buffer->pFunct = pFunct;

        /* CAN identifier and CAN mask, bit aligned with CAN module. Different on different microcontrollers. */
        buffer->ident = ident & 0x07FFU;
        if(rtr){
            buffer->ident |= 0x0800U;
        }
        buffer->mask = (mask & 0x07FFU) | 0x0800U;

        /* Set CAN hardware module filter and mask. */
        if(CANmodule->useCANrxFilters){
            /*Ported by Kim*/
            canopen_can_filter_init(index, buffer->mask, buffer->ident);
        }
    }
    else{
        ret = CO_ERROR_ILLEGAL_ARGUMENT;
    }

    return ret;
}


/******************************************************************************/
CO_CANtx_t *CO_CANtxBufferInit(
        CO_CANmodule_t         *CANmodule,
        uint16_t                index,
        uint16_t                ident,
        bool_t                  rtr,
        uint8_t                 noOfBytes,
        bool_t                  syncFlag)
{
    CO_CANtx_t *buffer = NULL;

    if((CANmodule != NULL) && (index < CANmodule->txSize)){
        /* get specific buffer */
        buffer = &CANmodule->txArray[index];

        /* CAN identifier, DLC and rtr, bit aligned with CAN module transmit buffer.
         * Microcontroller specific. */
		buffer->sfid = ident;
		buffer->efid = CCU_ADDR;
		buffer->rtr = rtr;
		buffer->DLC = noOfBytes;
		//buffer->ident = ((uint32_t)ident & 0x07FFU)
        //              | ((uint32_t)(((uint32_t)noOfBytes & 0xFU) << 12U))
        //              | ((uint32_t)(rtr ? 0x8000U : 0U));

        buffer->bufferFull = false;
        buffer->syncFlag = syncFlag;
    }

    return buffer;
}


/******************************************************************************/
/*Ported by Allen*/
CO_ReturnError_t CO_CANsendToBus(CO_CANmodule_t* CANmodule, CO_CANtx_t* buffer)
{
	can_trasnmit_message_struct vl_transmit_message;
	CANmodule->bufferInhibitFlag = buffer->syncFlag;
	/* copy message and txRequest */
	vl_transmit_message.tx_dlen = buffer->DLC;/*buffer->DLC;*//*Defined in CO_CANtxBufferInit()*/
	memcpy(vl_transmit_message.tx_data, buffer->data, vl_transmit_message.tx_dlen);
#ifdef CFG_EX_FRAME
	vl_transmit_message.tx_ff = CAN_FF_EXTENDED;
	vl_transmit_message.tx_efid = buffer->ident;	//Allen
#else
	vl_transmit_message.tx_sfid = buffer->sfid;
	vl_transmit_message.tx_ff = CAN_FF_STANDARD;
#endif
	vl_transmit_message.tx_ft = CAN_FT_DATA;
	return (can_message_transmit(CAN1, &vl_transmit_message) != CAN_NOMAILBOX) ? CO_ERROR_NO : CO_ERROR_TX_OVERFLOW;
}

CO_ReturnError_t CO_CANsend(CO_CANmodule_t *CANmodule, CO_CANtx_t *buffer){
    CO_ReturnError_t err = CO_ERROR_NO;
//    can_trasnmit_message_struct vl_transmit_message;

    /* Verify overflow */
	//buffer->bufferFull = can_mailbox_is_full(CAN1);

    if(buffer->bufferFull){
        if(!CANmodule->firstCANtxMessage){
            /* don't set error, if bootup message is still on buffers */
            CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_CAN_TX_OVERFLOW, CO_EMC_CAN_OVERRUN, buffer->ident);
        }
        err = CO_ERROR_TX_OVERFLOW;
    }

    CO_LOCK_CAN_SEND();
    /* if CAN TX buffer is free, copy message to it */
    if(CANmodule->CANtxCount || CO_CANsendToBus(CANmodule, buffer) != CO_ERROR_NO)
	{
     /* if no buffer is free, message will be sent by interrupt */
        buffer->bufferFull = true;
        CANmodule->CANtxCount++;
		//Printf("CANtxCount = %d\n", CANmodule->CANtxCount);
    }
    CO_UNLOCK_CAN_SEND();

    return err;
}


/******************************************************************************/
void CO_CANclearPendingSyncPDOs(CO_CANmodule_t *CANmodule){
    uint32_t tpdoDeleted = 0U;

    CO_LOCK_CAN_SEND();
    /* Abort message from CAN module, if there is synchronous TPDO.
     * Take special care with this functionality. */
    if(/*messageIsOnCanBuffer && */CANmodule->bufferInhibitFlag){
        /* clear TXREQ */
        CANmodule->bufferInhibitFlag = false;
        tpdoDeleted = 1U;
    }
    /* delete also pending synchronous TPDOs in TX buffers */
    if(CANmodule->CANtxCount != 0U){
        uint16_t i;
        CO_CANtx_t *buffer = &CANmodule->txArray[0];
        for(i = CANmodule->txSize; i > 0U; i--){
            if(buffer->bufferFull){
                if(buffer->syncFlag){
                    buffer->bufferFull = false;
                    CANmodule->CANtxCount--;
                    tpdoDeleted = 2U;
                }
            }
            buffer++;
        }
    }
    CO_UNLOCK_CAN_SEND();


    if(tpdoDeleted != 0U){
        CO_errorReport((CO_EM_t*)CANmodule->em, CO_EM_TPDO_OUTSIDE_WINDOW, CO_EMC_COMMUNICATION, tpdoDeleted);
    }
}


/******************************************************************************/
void CO_CANverifyErrors(CO_CANmodule_t *CANmodule){
    uint16_t rxErrors, txErrors, overflow;

    CO_EM_t* em = (CO_EM_t*)CANmodule->em;
    uint32_t err;

    #if 0
    /* get error counters from module. Id possible, function may use different way to
     * determine errors. */
    rxErrors = CANmodule->txSize;
    txErrors = CANmodule->txSize;
    overflow = CANmodule->txSize;
    #else
    /* ? Portable here, Allen*/
    rxErrors = can_receive_error_number_get(CAN1); 
    txErrors = can_transmit_error_number_get(CAN1);
	overflow = (can_flag_get(CAN1, CAN_FLAG_RFO0) || can_flag_get(CAN1, CAN_FLAG_RFO1));
#endif

    err = ((uint32_t)txErrors << 16) | ((uint32_t)rxErrors << 8) | overflow;
	err = CAN_ERR(CAN1);

    if(CANmodule->errOld != err){
        CANmodule->errOld = err;

        if(txErrors >= 255U){                               /* bus off */
            CO_errorReport(em, CO_EM_CAN_TX_BUS_OFF, CO_EMC_BUS_OFF_RECOVERED, err);
        }
        else{                                               /* not bus off */
            CO_errorReset(em, CO_EM_CAN_TX_BUS_OFF, err);

            if((rxErrors >= 96U) || (txErrors >= 96U)){     /* bus warning */
                CO_errorReport(em, CO_EM_CAN_BUS_WARNING, CO_EMC_NO_ERROR, err);
            }

            if(rxErrors >= 127U){                           /* RX bus passive */
                CO_errorReport(em, CO_EM_CAN_RX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
            }
            else{
                CO_errorReset(em, CO_EM_CAN_RX_BUS_PASSIVE, err);
            }

            if(txErrors >= 127U){                           /* TX bus passive */
                if(!CANmodule->firstCANtxMessage){
                    CO_errorReport(em, CO_EM_CAN_TX_BUS_PASSIVE, CO_EMC_CAN_PASSIVE, err);
                }
            }
            else{
                bool_t isError = CO_isError(em, CO_EM_CAN_TX_BUS_PASSIVE);
                if(isError){
                    CO_errorReset(em, CO_EM_CAN_TX_BUS_PASSIVE, err);
                    CO_errorReset(em, CO_EM_CAN_TX_OVERFLOW, err);
                }
            }

            if((rxErrors < 96U) && (txErrors < 96U)){       /* no error */
                CO_errorReset(em, CO_EM_CAN_BUS_WARNING, err);
            }
        }

        if(overflow != 0U){                                 /* CAN RX bus overflow */
            CO_errorReport(em, CO_EM_CAN_RXB_OVERFLOW, CO_EMC_CAN_OVERRUN, err);
        }
    }
}

void CANrxMsg_Dump(const CO_CANrxMsg_t* pMsg, uint32 debugLevel, const char* tag)
{
	if (debugLevel & g_dwDebugLevel == 0) return;
	if (tag == Null) tag = "";
	DUMP_BYTE_EX(("%s id=%x(%x.%x): ", tag, pMsg->ident, pMsg->sfid, pMsg->efid),pMsg->data, pMsg->DLC);
}

/******************************************************************************/
/*
*Modified by Kim
*add input parameter, int_src: source of interrupt; May be CAN1 RX0, CAN1 RX1, CAN1 TX
*/
void CO_CANinterrupt(CO_CANmodule_t *CANmodule, CO_srcOfInt_t int_src){

    /* receive interrupt */
    if((CO_SRCOFINT_CAN_RX0 == int_src)||(CO_SRCOFINT_CAN_RX1 == int_src)){
        CO_CANrxMsg_t RcvMsgBuf;
        uint16_t index;             /* index of received message */
        uint32_t rcvMsgIdent;       /* identifier of the received message */
        CO_CANrx_t *buffer = NULL;  /* receive message buffer from CO_CANmodule_t object. */
		bool_t msgMatched = false;
		bool_t isSf = false;	//是否标准帧

        /*Ported by Kim*/
        can_receive_message_struct vl_can_receive_message;
        if(CO_SRCOFINT_CAN_RX0 == int_src)
        {
            can_message_receive(CAN1, CAN_FIFO0, &vl_can_receive_message);
        }
        else
        {
            can_message_receive(CAN1, CAN_FIFO1, &vl_can_receive_message);
        }

		isSf = (vl_can_receive_message.rx_ff == CAN_FF_STANDARD);

        RcvMsgBuf.DLC = vl_can_receive_message.rx_dlen;
        memcpy(RcvMsgBuf.data, vl_can_receive_message.rx_data, RcvMsgBuf.DLC);

		RcvMsgBuf.ident = isSf ? vl_can_receive_message.rx_sfid : vl_can_receive_message.rx_efid;
		rcvMsgIdent = isSf ? RcvMsgBuf.ident : (RcvMsgBuf.ident >> 18);	//Modified by Allen, 仅仅比较sfid部分
		rcvMsgIdent &= 0x7FF;	

		//CANrxMsg_Dump(&RcvMsgBuf, DL_CAN_SLAVE_RX, "CAN1-Rx");

		if(CANmodule->useCANrxFilters){
            /* CAN module filters are used. Message with known 11-bit identifier has */
            /* been received */
			index = vl_can_receive_message.rx_fi;  /* get index of the received message here. Or something similar *//*Ported by Kim*/
            if(index < CANmodule->rxSize){
                buffer = &CANmodule->rxArray[index];
                /* verify also RTR */
                if(((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U){
                    msgMatched = true;
                }
            }
        }
        else{
            /* CAN module filters are not used, message with any standard 11-bit identifier */
            /* has been received. Search rxArray form CANmodule for the same CAN-ID. */
            buffer = &CANmodule->rxArray[0];
            for(index = CANmodule->rxSize; index > 0U; index--){
                if(((rcvMsgIdent ^ buffer->ident) & buffer->mask) == 0U){
                    msgMatched = true;
                    break;
                }
                buffer++;
            }
        }

        /* Call specific function, which will process the message */
        if(msgMatched && (buffer != NULL) && (buffer->pFunct != NULL)){
            buffer->pFunct(buffer->object, &RcvMsgBuf);
        }

        /* Clear interrupt flag */
    }

    /* transmit interrupt *//*Ported by Kim*/
    else if(CO_SRCOFINT_CAN_TX == int_src){
        /* Clear interrupt flag */

        /* First CAN message (bootup) was sent successfully */
        CANmodule->firstCANtxMessage = false;
        /* clear flag from previous message */
        CANmodule->bufferInhibitFlag = false;
        /* Are there any new messages waiting to be send */
        if(CANmodule->CANtxCount > 0U){
            uint16_t i;             /* index of transmitting message */

            /* first buffer */
            CO_CANtx_t *buffer = &CANmodule->txArray[0];
            /* search through whole array of pointers to transmit message buffers. */
            for(i = CANmodule->txSize; i > 0U; i--){
                /* if message buffer is full, send it. */
                /******************************************************************************/
                /* canSend... */
                /* Ported by Allen, transmit message */
                /* if CAN TX buffer is free, copy message to it */
                if(buffer->bufferFull){
                    buffer->bufferFull = false;
                    CANmodule->CANtxCount--;
					CO_CANsendToBus(CANmodule, buffer);
					break;
				}
                /******************************************************************************/

				buffer++;
            }/* end of for loop */

            /* Clear counter if no more messages */
            if(i == 0U){
                CANmodule->CANtxCount = 0U;
            }
        }
    }
    else{
        /* some other interrupt reason */
    }
}

void CAN1_RX0_IRQHandler(void)
{
	CO_CANinterrupt(g_pCANmodule, CO_SRCOFINT_CAN_RX0);
}

void CAN1_RX1_IRQHandler(void)
{
	//Printf("CAN1_RX1_IRQHandler\n");
	CO_CANinterrupt(g_pCANmodule, CO_SRCOFINT_CAN_RX1);
}

void CAN1_TX_IRQHandler(void)
{
	//Printf("CAN1_TX_IRQHandler\n");
	//CO_CANinterrupt(g_pCANmodule, CO_SRCOFINT_CAN_TX);
	CO_CANverifyErrors(g_pCANmodule);

	uint32 can_periph = CAN1;
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
			//CAN0_TXIRQHandler(&g_CanBuses[0], mailbox, TX_SUCCESS);
			CO_CANinterrupt(g_pCANmodule, CO_SRCOFINT_CAN_TX);
		}
		else if (ret == CAN_TRANSMIT_FAILED)
		{
			//获取失败原因
			uint32 errNo = CAN_ERR(can_periph);	// & (CAN_ERR_BOERR | CAN_ERR_PERR | CAN_ERR_WERR)
			uint32 errTCnt = can_transmit_error_number_get(can_periph);
			uint32 errRCnt = can_receive_error_number_get(can_periph);
			can_error_enum canErr = can_error_get(can_periph);
			//PFL(DL_CAN_MASTER, "canErr=0x%x, ERRIF=0x%x,errTCnt=%d,errRCnt=%d\n", canErr, errNo, errTCnt, errRCnt);
			//CAN0_TXIRQHandler(&g_CanBuses[0], mailbox, TX_FAILED);
			CO_CANverifyErrors(g_pCANmodule);
		}

		can_interrupt_flag_clear(can_periph, intflag[mailbox]);
	}
}

void CAN1_EWMC_IRQHandler(void)
{
	uint32_t can_periph = CAN1;
	if (SET == can_interrupt_flag_get(CAN1, CAN_INT_FLAG_ERRIF))
	{
		CO_CANverifyErrors(g_pCANmodule);

		//errNo = CAN_ERR(can_periph) & (CAN_ERR_BOERR | CAN_ERR_PERR | CAN_ERR_WERR);
		//当CAN口没接线时，canErr = 1， errRCnt++;
		//can_error_enum canErr = can_error_get(can_periph);
		//uint32_t errRCnt = can_receive_error_number_get(can_periph);
		//uint32_t errTCnt = can_transmit_error_number_get(can_periph);
		//can_error_enum canErr = can_error_get(can_periph);
		//uint32_t errRCnt = can_receive_error_number_get(can_periph);
		//uint32_t errTCnt = can_transmit_error_number_get(can_periph);
		if (can_flag_get(can_periph, CAN_FLAG_BOERR)) can_flag_clear(can_periph, CAN_FLAG_BOERR);
		if (can_flag_get(can_periph, CAN_FLAG_PERR)) can_flag_clear(can_periph, CAN_FLAG_PERR);
		if (can_flag_get(can_periph, CAN_FLAG_WERR)) can_flag_clear(can_periph, CAN_FLAG_WERR);
		if (can_flag_get(can_periph, CAN_FLAG_RFO0)) can_flag_clear(can_periph, CAN_FLAG_RFO0);
		if (can_flag_get(can_periph, CAN_FLAG_RFO1)) can_flag_clear(can_periph, CAN_FLAG_RFO1);

		//uint32 errNo = CAN_ERR(can_periph) & (CAN_ERR_BOERR | CAN_ERR_PERR | CAN_ERR_WERR);
		//if(errTCnt >= 250)
		//	Printf("ERRIF=0x%x,errTCnt=%d,errRCnt=%d, canErr=%d\n", errNo, errTCnt, errRCnt, canErr);

		can_interrupt_flag_clear(can_periph, CAN_INT_FLAG_ERRIF);
	}
}

//中断标志使能
const static uint32_t g_Can1IntFlag[] = {
	CAN_INT_TME,	//transmit mailbox empty interrupt enable ; ISR-> CAN1_TX_IRQHandler
	CAN_INT_RFNE0 ,	//receive FIFO0 not empty interrupt enable; ISR-> CAN1_RX0_IRQHandler
	CAN_INT_RFNE1 ,	//receive FIFO1 not empty interrupt enable; ISR-> CAN1_RX1_IRQHandler
	CAN_INT_BO,		//bus-off interrupt enable;					ISR-> CAN1_EWMC_IRQn		
	CAN_INT_ERRN,	//error number interrupt enable;			ISR-> CAN1_EWMC_IRQn
	CAN_INT_ERR,	//error interrupt enable;					ISR-> CAN1_EWMC_IRQn
	CAN_INT_RFO0,	//< receive FIFO0 overfull interrupt enable ISR-> CAN1_EWMC_IRQn
	CAN_INT_RFO1,	//< receive FIFO1 overfull interrupt enable ISR-> CAN1_EWMC_IRQn
};
void CAN1_Stop()
{
	uint32_t can_periph = CAN1;

	//失能中断
	nvic_irq_disable(CAN1_TX_IRQn);
	nvic_irq_disable(CAN1_EWMC_IRQn);
	nvic_irq_disable(CAN1_RX0_IRQn);
	nvic_irq_disable(CAN1_RX1_IRQn);

	for (int i = 0; i < GET_ELEMENT_COUNT(g_Can1IntFlag); i++)
	{
		can_interrupt_disable(can_periph, g_Can1IntFlag[i]);//失能fifo_0 empty中断
	}

	//清除接收FIFO
	can_fifo_release(can_periph, CAN_INTEN_RFNEIE0);
	can_fifo_release(can_periph, CAN_INTEN_RFNEIE1);

}
void CAN1_Start()
{
	//使能中断
	nvic_irq_enable(CAN1_TX_IRQn, 0, 0);
	nvic_irq_enable(CAN1_EWMC_IRQn, 0, 1);
	nvic_irq_enable(CAN1_RX0_IRQn, 0, 2);
	nvic_irq_enable(CAN1_RX1_IRQn, 0, 3);

	for (int i = 0; i < GET_ELEMENT_COUNT(g_Can1IntFlag); i++)
	{
		can_interrupt_enable(CAN1, g_Can1IntFlag[i]);
	}
}

void CAN1_GpioInit()
{
	//Config can clock and gpio
	rcu_periph_clock_enable(RCU_CAN0);
	rcu_periph_clock_enable(RCU_CAN1);
	rcu_periph_clock_enable(RCU_GPIOB);
	rcu_periph_clock_enable(RCU_AF);

	gpio_init(GPIOB, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	gpio_pin_remap_config(GPIO_CAN1_REMAP, ENABLE);

	//gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_5);
	//gpio_init(GPIOB, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
	//PortPin_Set(GPIOB, GPIO_PIN_5)
}

void CAN1_Init()
{
	CAN1_GpioInit();

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
	can_parameter.prescaler = 7;

	can_deinit(CAN1);
	can_init(CAN1, &can_parameter);
	can1_filter_start_bank(CAN1, 0x05);
	g_offsetFilterNumber = (CAN_FCTL(CAN0) & CAN_FCTL_HBC1F) >> 8;
}


void canopen_can_filter_init(uint16_t ft_number, uint16_t mask, uint16_t ident)
{
	can_filter_parameter_struct can_filter;
	memset(&can_filter, 0, sizeof(can_filter));
	/* initialize filter */
	/* CAN0 filter number */
	can_filter.filter_number = ft_number *2;
	//必须要加这个偏移值，否则会导致配置出错
	can_filter.filter_number += g_offsetFilterNumber;
	//ident = 0x7FF;
	mask = 0x7FF;

	uint32 filter = can_build_filter(ident, 0, 0, 0);
	uint32 mask32 = can_build_filter(mask, 0, 0, 0);
	//CanFilter* p = (CanFilter*)& val;

	/* initialize filter */
	can_filter.filter_mode = CAN_FILTERMODE_MASK;
	can_filter.filter_bits = CAN_FILTERBITS_32BIT;
#if 1
	can_filter.filter_list_high = (uint16)(filter >> 16);	//((ident & 0x7FF) << 5);
	can_filter.filter_list_low  = (uint16)filter;			/*FF = 0, FT = 0*/
	can_filter.filter_mask_high = (uint16)(mask32 >> 16);	//((mask & 0x7FF) << 5);
	can_filter.filter_mask_low  = (uint16)mask32;			//0x0006;/*FF = 1, FT = 1*/
#else
	can_filter.filter_list_high = ((ident & 0x7FF) << 5);
	can_filter.filter_list_low  = 0; //FF = 0, FT = 0*/
	can_filter.filter_mask_high = ((mask & 0x7FF) << 5);
	can_filter.filter_mask_low  = 0x0006;/*FF = 1, FT = 1*/

	filter = can_filter.filter_list_high << 16 | can_filter.filter_list_low;
	mask32 = can_filter.filter_mask_high << 16 | can_filter.filter_mask_low;

#endif
	can_filter.filter_fifo_number = CAN_FIFO0;
	can_filter.filter_enable = ENABLE;

	//Printf("FIF00： filterNum=%d, ident=%04X, filter=%08X, mask=%08X\n", can_filter.filter_number, ident
	//	, filter
	//	, mask32);

	can_filter_init(CAN1, &can_filter);

	can_filter.filter_number = (ft_number * 2) + 1 + g_offsetFilterNumber;
	can_filter.filter_fifo_number = CAN_FIFO1;
	//Printf("FIF01： filterNum=%d, ident=%04X, filter=%08X, mask=%08X\n", can_filter.filter_number, ident, filter, mask32);
	can_filter_init(CAN1, &can_filter);
}

void DumpFilter()
{
	for (int i = 0; i < 28; i++)
	{
		uint32 filter = CAN_FDATA0(CAN0, i);
		Printf("REG[@%08X]:filterNum[%d], filter=%08X, mask=%08X\n"
			, (CAN0)+0x1B0U + ((i) * 0x10U)
			, i
			, CAN_FDATA0(CAN0, i)
			, CAN_FDATA1(CAN0, i));
	}
}
