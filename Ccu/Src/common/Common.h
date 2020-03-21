#ifndef __COMMON_H_
#define __COMMON_H_

#ifdef __cplusplus
extern "C"{
#endif

//#define CFG_LOG
#define XDEBUG
#define CONFIG_CMDLINE
	
#include "gd32f403.h"
#include "gd32f403_gpio.h"
#include "systick.h"
#include <stdio.h>
#include "time.h"
	
#include "ArchDef.h"
#include "SwTimer.h"
#include "page.h"
#include "math.h"
#include "MsgDef.h"
//#include "GpioDef.h"
#include "Queue.h"
#include "Message.h"
#include "CheckSum.h"
#include "Debug.h"
#include "StringEx.h"
//#include "IdentifyInfo.h"
#include "DateTime.h"
#include "Tlv.h"
#include "Log.h"
#include "safebuf.h"
#include "Obj.h"
#include "CcuDef.h"
#include "MemMap.h"
#include "Nvds.h"
#include "LogUser.h"

//Driver
#include "RcuMap.h"
#include "DriverUart.h"
#include "DriverFmc.h"
#include "DriverAdc.h"
#include "OtaFile.h"
//#include "CanOpen.h"
//#include "SmUtpDef.h"
//#include "Frame.h"
#include "CcuLed.h"

#define ASRT_TURE(parenExpr) if(!(parenExpr))	\
		{                                   \
			Printf( "Assertion Failed! %s,%s,%s,line=%d\n", #parenExpr,__FILE__,__FUNCTION__,_LINE_);	\
			while(1){;}	\
		}
#define IS_FOUND(_str1, _str2) strstr((char*)(_str1), (char*)(_str2))


#ifdef CONFIG_CMDLINE
	extern void Shell_Run(void);
#else
	#define Shell_Run(...)
#endif

//复位原因
typedef enum _MCURST
{
	 MCURST_PWR				//
	,MCURST_MODIFY_FW_VER	//修改固件版本号
	,MCURST_MODIFY_HW_VER	//修改硬件版本号
	,MCURST_MODIFY_SVR_ADDR	//修改服务器地址
	,MCURST_FWUPG_ERROR		//固件升级失败
	,MCURST_GPRS			//网络远程复位
	,MCURST_UPG_OK			//升级固件成功

	,MCURST_BLE_CMD			//收到蓝牙复位命令
	,MCURST_FACTORY_SETTING	//恢复出厂设置
	,MCURST_18650_POWER_OFF	//切断18650供电
	,MCURST_DEV_ACTIVE		//设备激活
	,MCURST_ALL_BAT_PLUG_OUT//拔出所有电池
	,MCURST_SMS				//短息复位
	,MCURST_PMS_FW_UPGRADE	//pms固件升级成功
	 
	,MCURST_ADC_ISR_INVALID	//ADC中断故障，不能触发
}MCURST;

extern uint32 SystemCoreClock;
extern uint8 g_McuID[12];
extern uint32 g_TestFlag;

extern void PostMsg(uint8 msgId);
extern void PostMsgEx(uint8 msgId, uint32 param1, uint32 param2);

#ifdef __cplusplus
}
#endif

#endif


