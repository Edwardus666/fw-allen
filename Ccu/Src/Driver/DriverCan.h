

#ifndef _DRIVER_CAN_DRIVER_H
#define _DRIVER_CAN_DRIVER_H

#include <stdint.h>
#include "SafeBuf.h"
#include "gd32f403_can.h"
#include "queue.h"

#define SYNC_CYCLE_MS	2000

#define CAN0_TX_LOCK()		nvic_irq_disable(CAN0_TX_IRQn)
#define CAN0_TX_UNLOCK()	nvic_irq_enable(CAN0_TX_IRQn, 0, 0)

typedef enum
{
	TX_INIT,	 //初始化
	TX_PENDING,	 //等待发送
	TX_INPROCESS,//发送到总线上，等待发送结果中断

	TX_SUCCESS,	//发送成功
	TX_FAILED,	//发送失败
	TX_TIMEOUT, //发送超时
	TX_ABORT,	//发送取消
}TxState;

struct _CanTxMsg;
typedef void (*CanBusTxDoneFn)(void* pCallBackParam, struct _CanTxMsg* pTxMsg, TxState state);

typedef enum
{
	MS_IDLE,
	TX_WAITTING,
	TX_DONE,
}MailBoxState;

typedef struct _CanTxMsg
{
	TxState state;	//发送状态
	can_trasnmit_message_struct msg;	//发送的消息

	void* callBackParam;	//回调函数参数
	CanBusTxDoneFn TxDone;	//回调函数
}CanTxMsg;

typedef struct
{
	uint8 number;			//nuber of mail box
	MailBoxState state;		//MailBox状态

	SwTimer timer;			//发送定时器，
	CanTxMsg* pTxObj;	//指向发送的内容对象
}MailBox;

#define MAX_MAIL_BOX_COUNT 3
typedef struct
{
	//Can config
	uint32 canPort;	//CAN0 or CAN1
	uint32 ioPort;
	uint32 txPin;
	uint32 rxPin;

	Bool isStart;
	uint8 isConnect;

	MailBox mailBox[MAX_MAIL_BOX_COUNT];

	//Tx msg buffer
	CanTxMsg syncMsg;	//SYNC帧消息,最高优先级，单独定义
	SwTimer syncFrametimer;	//用于MASTER模式得SYNC帧计时
}CanBus;

Bool CanBus_isIdle(const CanBus* pCanBus);
void CanBus_Start(CanBus* pCanBus);
void CanBus_Stop(CanBus* pCanBus);
void CanBus_Run(CanBus* pCanBus);
CanBus* CanBus_Init(uint8 canId);
void CanBus_TxDone(CanBus* pCanBus, MailBox* pMailBox, TxState txState);
Bool CanBus_CopyMsgToMailBox(CanBus* pCanBus, CanTxMsg* pMsg);

#endif //_DRIVER_UART_H
