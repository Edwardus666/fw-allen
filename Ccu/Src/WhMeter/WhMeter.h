
#ifndef _AM_METER_H_
#define _AM_METER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "MeterProtocol.h"
#include "Rs485ProtocolNode.h"

#define MP_CMD_INTERVAL_MS 500	//������������������λMS
#define MP_CMD_WAIT_RSP_MS 500	//�ȴ���Ӧʱ�䣬��λMS
#define MP_CMD_RETX_COUNT	3	//�ط�����

#define METER_MAX_BUF_SIZE 32

#define MP_CMD_READ			0x11
#define MP_CMD_READ_NEXT	0x12

	//���������
	typedef struct
	{
		uint8 di[4];		//���ݱ�ʶ
		int len;			//ֵ��Ч����
		const char* fmt;	//ֵ�����ݸ�ʽ
		const char* name;	//����
		uint8 val[4];
	}MpReadCmdArg;

	typedef struct
	{
		Rs485ProtocolNodeState state;

		uint32 totalWh;		//�й��ܵ���
		uint32 nowTotalWh;	//˲ʱ���й�����
		uint16 aVoltage;	//A���ѹ, ��λ100mV.
		uint32 aCurrent;	//A���������λ1mA

		MeterProtocol  meterProtocol;
	}WhMeter;

	extern WhMeter* g_pWhMeter;

	UTP_EVENT_RC WhMeter_ReadCmd_RspProc(WhMeter* pWhMeter, UtpCmd* pUtpCmd, UTP_EVENT ev);
	void WhMeter_Init();

#ifdef __cplusplus
}
#endif

#endif
