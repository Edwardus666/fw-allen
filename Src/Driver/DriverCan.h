

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
	TX_INIT,	 //��ʼ��
	TX_PENDING,	 //�ȴ�����
	TX_INPROCESS,//���͵������ϣ��ȴ����ͽ���ж�

	TX_SUCCESS,	//���ͳɹ�
	TX_FAILED,	//����ʧ��
	TX_TIMEOUT, //���ͳ�ʱ
	TX_ABORT,	//����ȡ��
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
	TxState state;	//����״̬
	can_trasnmit_message_struct msg;	//���͵���Ϣ

	void* callBackParam;	//�ص���������
	CanBusTxDoneFn TxDone;	//�ص�����
}CanTxMsg;

typedef struct
{
	uint8 number;			//nuber of mail box
	MailBoxState state;		//MailBox״̬

	SwTimer timer;			//���Ͷ�ʱ����
	CanTxMsg* pTxObj;	//ָ���͵����ݶ���
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
	CanTxMsg syncMsg;	//SYNC֡��Ϣ,������ȼ�����������
	SwTimer syncFrametimer;	//����MASTERģʽ��SYNC֡��ʱ
}CanBus;

Bool CanBus_isIdle(const CanBus* pCanBus);
void CanBus_Start(CanBus* pCanBus);
void CanBus_Stop(CanBus* pCanBus);
void CanBus_Run(CanBus* pCanBus);
CanBus* CanBus_Init(uint8 canId);
void CanBus_TxDone(CanBus* pCanBus, MailBox* pMailBox, TxState txState);
Bool CanBus_CopyMsgToMailBox(CanBus* pCanBus, CanTxMsg* pMsg);

#endif //_DRIVER_UART_H
