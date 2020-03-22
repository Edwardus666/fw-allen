#ifndef __DRIVER_IO_H_
#define __DRIVER_IO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "MsgDef.h"
	
#define IS_OUT_MODE(trig_type) ((trig_type & 0xF0) == 0x10)
#define IS_IN_MODE(trig_type) ((trig_type & 0xF0) != 0x10)

#define FUN_IO_POWER_OFF() IO_Set(CTRL_FUN_POWER, 0)
#define FUN_IO_POWER_ON()  IO_Set(CTRL_FUN_POWER, 1)

#define LIGHT_POWER_OFF() IO_Set(CTRL_LIGHT, 0)
#define LIGHT_POWER_ON()  IO_Set(CTRL_LIGHT, 1)

//LCD 上/关电
#define LCD_POWER_OFF() IO_Set(CTRL_LCD_POWER, 0)
#define LCD_POWER_ON()  IO_Set(CTRL_LCD_POWER, 1)
//LCD复位脚
#define LCD_RESET_ON()  IO_Set(CTRL_LCD_RESET, 1)
#define LCD_RESET_OFF()  IO_Set(CTRL_LCD_RESET, 0)

#define PUMP_POWER_OFF() IO_Set(CTRL_PUMP, 0)
#define PUMP_POWER_ON()  IO_Set(CTRL_PUMP, 1)

//继电器上电加测，n = DET_12V_1，DET_12V_2，DET_12V_3，DET_12V_4
#define RM_POWER_IS_ON(n) (IO_Read(n) == 0)

//继电器模块上电，n = CTRL_ACC_3，CTRL_ACC_4，CTRL_ACC_5
#define RM_POWER_ON(n)	IO_Set(n, 1)
#define RM_POWER_OFF(n)	IO_Set(n, 0)

#define IO_LOW  0
#define IO_HIGH 1

#define AC_ON  1
#define AC_OFF 0

#define PUMP_ON  0
#define PUMP_OFF 1

#define FUN_OK	  1
#define FUN_FAULT 0

#define RELAY_12V_ON  0
#define RELAY_12V_OFF 1

#define BP_START_SOC_THRESHOLD 10	//备电电池工作电量阈值，必须大于该值才能正常工作

	//继电器错误码定义
	typedef enum
	{
		RELAY_ERR_OK = 0,
		RELAY_ERR_ON,		//继电器错误, 应该为打开，实际关闭
		RELAY_ERR_OFF,		//继电器错误，应该为关闭，实际打开
	}RELAY_ERR;

	//信号名称定义，其枚举值可能被写入LOG文件中，修改其枚举值肯能会导致log文件的事件ET_CCU_BOM_FAULT内容错配
	typedef enum
	{
		DET_BUCK = 0x01,	//PA0, ADC012_IN0 , 3V6 BUCK输出检测， 低电平正常
		DET_PUMP_OFF,		//PA15,水泵启动检测，低电平表示开启

		DET_AC_ON,			//PB3,市电断电指示，高电平表示正常，低电平断电

		DET_SWITCH_BIT3 = 0x10,	//PD4,拨码开关, BIT3
		DET_SWITCH_BIT2,	//PD5,拨码开关, BIT2
		DET_SWITCH_BIT1,	//PD6,拨码开关, BIT1
		DET_SWITCH_BIT0,	//PD7,拨码开关, BIT0

		DET_FUN1_FAULT = 0x20,	//PD14,风扇1故障检测，低电平表示故障，高电平正常
		DET_FUN2_FAULT,	//PD13,风扇2故障检测，低电平表示故障，高电平正常
		DET_FUN3_FAULT,	//PD12,风扇3故障检测，低电平表示故障，高电平正常
		DET_FUN4_FAULT,	//PD11,风扇4故障检测，低电平表示故障，高电平正常
		DET_FUN5_FAULT,	//PD10,风扇1故障检测，低电平表示故障，高电平正常
		DET_FUN6_FAULT,	//PB7, 风扇6故障检测，低电平表示故障，高电平正常

		DET_BLE_LOCK = 0x30,//PE4, 后门蓝牙锁反馈信号, 暂时不用
		DET_WATER_IN,		//PE5, 机柜进水检测，高电平表示进水
		DET_BUCKET,			//PE6, 水桶水位检测，低电平表示水量不足

		DET_12V_1 = 0x40,	//PE7, 12V_#1_DET, 1号12V继电器是否有电，低电平有电, LCD,蓝牙锁，其他设备电源，不可控
		DET_12V_2,			//PE8, 12V_#2_DET, 2号12V继电器是否有电，低电平有电, 视频监控系统，照明电源，不可控
		DET_12V_3,			//PE9, 12V_#3_DET, 3号12V继电器是否有电，低电平有电，风扇，电磁锁供电，可控
		DET_12V_4,			//PE10,12V_#4_DET, 4号12V继电器是否有电，低电平有电, PMS供电，可控

		DET_12V_BP,			//PE12, 备电电源砖 12V输出检测，低电平表示正常
		DET_12V_18650BP,	//PE14, 18650升压12V输出检测，低电平表示正常

		//Out put
		CTRL_FUN_POWER = 0x50,	//PA11, 风扇供电控制，高电平有效。硬件电路默认关闭
		CTRL_LIGHT,			//PA12, 照明灯控制，高电平有效。硬件电路默认开启

		CTRL_PUMP,			//PC10, 水泵供电控制，高电平有效。硬件电路默认关闭
		CTRL_LCD_POWER,		//PC11, LCD供电控制，高电平有效。硬件电路默认开启
		CTRL_LCD_RESET,		//PE2, LCD 复位，低电平复位至少n秒，再拉高

		CTRL_ACC_3,			//PE1, 继电器3上电，高电平有效，硬件电路默认关闭，风扇，电磁锁供电
		CTRL_ACC_4,			//PE0, 继电器4上电，高电平有效，硬件电路默认关闭，PMS供电
		CTRL_ACC_5,			//PB4, 继电器5上电, 高电平有效，硬件电路默认关闭, 空调供电

		CTRL_MCU_LED,		//PE11, MCU运行灯
		CTRL_CAN_RED,		//PD1, CAN OPEN状台灯，红色
		CTRL_CAN_GREEN,		//PD0, CAN OPEN状台灯，绿色

		CTRL_485_METER_ENABLE,		//PA1, 电表485使能控制
		CTRL_485_LOCK_ENABLE,	//PE15, 4后门蓝牙锁85使能控制
		CTRL_485_COOLING_ENABLE,	//PA8, 空调485使能控制

		CTRL_18650BP,		//PB15, BU_CTR2, 控制18650BP，高电平使能放电

		CTRL_HOT_FUN,		//PC13, 控制电源两个扇热风扇，高电平有效
		CTRL_BU_CHARGE,		//PB14, 控制back_up充电，高电平使能充电
		CTRL_BU_DISCHARGE , //PD8, 控制back_up，12V隔离模块控制，高电平使能放电
		DET_BU_PRESENT,		//PD9, 备电板在位检测，高电平表示在位
	}IO_ID;

#define RM_POWER_FUN DET_12V_3
#define RM_POWER_PMS DET_12V_4

	typedef void (*DrvIoStateChanged)(IO_ID ioID, Bool isHigh);
	typedef struct
	{
		IO_ID ioID;
		const char* desc;	//IO的名称，方便调试查看

		/* GPIOx(x=A,B,C,D,E,F,G) definitions */
		uint32 periph;	

		/* GPIO pin definitions 
		GPIO_PIN_0 
		GPIO_PIN_1 
		GPIO_PIN_2 
		GPIO_PIN_3 
		GPIO_PIN_4 
		GPIO_PIN_5 
		GPIO_PIN_6 
		GPIO_PIN_7 
		GPIO_PIN_8 
		GPIO_PIN_9 
		GPIO_PIN_10
		GPIO_PIN_11
		GPIO_PIN_12
		GPIO_PIN_13
		GPIO_PIN_14
		GPIO_PIN_15
		*/
		uint32 pin;

		/* GPIO mode definitions 
		GPIO_MODE_AIN              
		GPIO_MODE_IN_FLOATING      
		GPIO_MODE_IPD              
		GPIO_MODE_IPU              
		GPIO_MODE_OUT_OD           
		GPIO_MODE_OUT_PP           
		GPIO_MODE_AF_OD            
		GPIO_MODE_AF_PP        
		*/    
		uint8 mode;

		exti_line_enum line;	//EXTI重点触发线，0 表示无效值。

		/*portSource: gpio event output port
		GPIO_PORT_SOURCE_GPIOA: output port source A
		GPIO_PORT_SOURCE_GPIOB: output port source B
		GPIO_PORT_SOURCE_GPIOC: output port source C
		GPIO_PORT_SOURCE_GPIOD: output port source D
		GPIO_PORT_SOURCE_GPIOE: output port source E 
		GPIO_PORT_SOURCE_GPIOF: output port source F
		GPIO_PORT_SOURCE_GPIOG: output port source G
		*/
		uint8 portSource;

		/*pinSource: GPIO_PIN_SOURCE_x(x=0..15)*/
		uint8 pinSource;

		/* trig_type: interrupt trigger type, refer to exti_trig_type_enum
			only one parameter can be selected which is shown as below :
			EXTI_TRIG_RISING : rising edge trigger
			EXTI_TRIG_FALLING : falling trigger
			EXTI_TRIG_BOTH : rising and falling trigger
		*/
		exti_trig_type_enum trig_type;	//触发类型

		MSG_ID stateChangedMsg;	//IO状态改变时触发的消息值
		uint8* pOdValue;

		//IO的状态值，0或者1
		uint8 value;

		//Output Mode，用于检测故障计时。
		//Input Mode，用于防抖计时。
		uint32 ticks;

		//是否故障，由相关反馈IO的状态确定
		Bool isFault;

		Bool isEnable;
	}DrvIo;
	extern uint8 g_ToggleValue;
	extern DrvIo* g_pBucket;			//水桶水位开关状态
	extern DrvIo* g_pWaterIn;			//机柜水位开关状态

	void IO_Init();
	
	DrvIo* IO_Get(IO_ID io);

	uint8 PortPin_Read(const DrvIo* pDrvIo);
	//portPinName = "PA1","PB2"..."PE15"
	uint8 IODesc_Read(const char* portPinName);
	uint8 IO_Read(IO_ID pin);

	void PortPin_Set(uint32 port, uint32 pin, uint8 value);
	//portPinName = "PA1","PB2"...
	//value = 0;1
	void IODesc_Set(const char* portPinName, uint8 value);
	void IO_Set(IO_ID pin, uint8 value);
	void IO_AllStateChanged();
	void IO_TriggerStateChanged(IO_ID pin);

	void IO_SetRunLed(uint16 onMs, uint16 offMs);

#ifdef __cplusplus
}
#endif

#endif

