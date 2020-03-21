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
	MSG_TIMEOUT = 1,	//消息格式：(MSG_TIMEOUT, TIMEID, 0)
	MSG_48V_BAT_ON,

	MSG_RUN,
	MSG_RX_FRAME,
	MSG_SDO_WAIT_RSP_TIMEOUT,
	MSG_ENTRY_CHANGED,
	MSG_TX_DONE,	//消息格式：(MSG_TX_DONE, SendMgr, 0) Can bus 发送数据结束

	MSG_PMS_BATTERY_PRESENT,	 //电池插入或者拔出，(MSG_PMS_BATTERY_PRESENT    , BAT_ADDR, isPlugIn)
	MSG_PMS_BATTERY_SOC_CHANGED, //电池电量改变，    (MSG_PMS_BATTERY_SOC_CHANGED, BAT_ADDR, SOC)
	MSG_PMS_BATTERY_FAULT,		 //电池故障，        (MSG_PMS_BATTERY_FAULT      , BAT_ADDR, FAULT)

	//IO状态转换消息定义
	MSG_IO_PUMP,	//消息格式：(MSG_IO_AC, IO_PUMP, PUMP_ON/PUMP_OFF)
	MSG_IO_BUCK,	//消息格式：(MSG_IO_AC, IO_BUCK, IO_LOW/IO_HIGH)
	MSG_IO_AC,		//消息格式：(MSG_IO_AC, IO_NAME, AC_ON/AC_OFF)

	MSG_IO_FUN   ,
	MSG_IO_12VBP   ,	//备电电源砖 12V输出检测消息，低电平表示正常
	MSG_IO_18650BP   ,	//18650升压12V输出检测消息，低电平表示正常

	MSG_IO_WATER,		//机柜进水检测：进水
	MSG_IO_BUCKET,		//水桶水量足够

	MSG_IO_12V,			//消息格式：(MSG_IO_12V, IO_NAME, RELAY_12V_ON/RELAY_12V_OFF)
	MSG_IO_12V_FAULT,	//消息格式：(MSG_IO_12V_FAULT, IO_NAME, isFault)

	MSG_TEMP_CHANGED,	//温度变化，消息格式：(MSG_TEMP_CHANGED, sensorId, THRESHOLD_EVENT)

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

