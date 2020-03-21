#ifndef __MSG_DEF_H_
#define __MSG_DEF_H_

#ifdef __cplusplus
extern "C"{
#endif

	typedef enum
	{
		OFF = 0,
		ON = 1
	}MSG_PARAM;

typedef enum _MSG_ID
{
	MSG_TIMEOUT = 1,	//��Ϣ��ʽ��(MSG_TIMEOUT, TIMEID, 0)
	MSG_48V_BAT_ON,

	MSG_RUN,
	MSG_RX_FRAME,
	MSG_SDO_WAIT_RSP_TIMEOUT,
	MSG_ENTRY_CHANGED,
	MSG_TX_DONE,	//��Ϣ��ʽ��(MSG_TX_DONE, SendMgr, 0) Can bus �������ݽ���

	MSG_PMS_BATTERY_PRESENT,	 //��ز�����߰γ���(MSG_PMS_BATTERY_PRESENT    , BAT_ADDR, isPlugIn)
	MSG_PMS_BATTERY_SOC_CHANGED, //��ص����ı䣬    (MSG_PMS_BATTERY_SOC_CHANGED, BAT_ADDR, SOC)
	MSG_PMS_BATTERY_FAULT,		 //��ع��ϣ�        (MSG_PMS_BATTERY_FAULT      , BAT_ADDR, FAULT)

	//IO״̬ת����Ϣ����
	MSG_IO_PUMP,	//��Ϣ��ʽ��(MSG_IO_AC, IO_PUMP, PUMP_ON/PUMP_OFF)
	MSG_IO_BUCK,	//��Ϣ��ʽ��(MSG_IO_AC, IO_BUCK, IO_LOW/IO_HIGH)
	MSG_IO_AC,		//��Ϣ��ʽ��(MSG_IO_AC, IO_NAME, AC_ON/AC_OFF)

	MSG_IO_FUN   ,
	MSG_IO_12VBP   ,	//�����Դש 12V��������Ϣ���͵�ƽ��ʾ����
	MSG_IO_18650BP   ,	//18650��ѹ12V��������Ϣ���͵�ƽ��ʾ����

	MSG_IO_WATER,		//�����ˮ��⣺��ˮ
	MSG_IO_BUCKET,		//ˮͰˮ���㹻

	MSG_IO_12V,			//��Ϣ��ʽ��(MSG_IO_12V, IO_NAME, RELAY_12V_ON/RELAY_12V_OFF)
	MSG_IO_12V_FAULT,	//��Ϣ��ʽ��(MSG_IO_12V_FAULT, IO_NAME, isFault)

	MSG_TEMP_CHANGED,	//�¶ȱ仯����Ϣ��ʽ��(MSG_TEMP_CHANGED, sensorId, THRESHOLD_EVENT)

	MSG_MAX

}MSG_ID;

//const char* g_pMsgIDStr[] =
//{
//	"MSG_TIMEOUT",
//	"48V_BAT_ON",
//};
//
#ifdef __cplusplus
}
#endif

#endif

