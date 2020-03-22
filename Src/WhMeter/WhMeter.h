
#ifndef _AM_METER_H_
#define _AM_METER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "MeterProtocol.h"
#include "Rs485ProtocolNode.h"

#define MP_CMD_INTERVAL_MS 500	//连续发送命令间隔，单位MS
#define MP_CMD_WAIT_RSP_MS 500	//等待响应时间，单位MS
#define MP_CMD_RETX_COUNT	3	//重发次数

#define METER_MAX_BUF_SIZE 32

#define MP_CMD_READ			0x11
#define MP_CMD_READ_NEXT	0x12

	//读命令参数
	typedef struct
	{
		uint8 di[4];		//数据标识
		int len;			//值有效长度
		const char* fmt;	//值的数据格式
		const char* name;	//名称
		uint8 val[4];
	}MpReadCmdArg;

	typedef struct
	{
		Rs485ProtocolNodeState state;

		uint32 totalWh;		//有功总电能
		uint32 nowTotalWh;	//瞬时总有功能能
		uint16 aVoltage;	//A相电压, 单位100mV.
		uint32 aCurrent;	//A相电流，单位1mA

		MeterProtocol  meterProtocol;
	}WhMeter;

	extern WhMeter* g_pWhMeter;

	UTP_EVENT_RC WhMeter_ReadCmd_RspProc(WhMeter* pWhMeter, UtpCmd* pUtpCmd, UTP_EVENT ev);
	void WhMeter_Init();

#ifdef __cplusplus
}
#endif

#endif
