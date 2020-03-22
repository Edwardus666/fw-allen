
#include "Common.h"
#include "DriverIo.h"
#include "CO_OD.h"
#include "Led.h"

static Bool g_isIoStart = False;
uint8 g_ToggleValue = 0;

static DrvIo g_SwitchIOs[] =
{
	 {DET_SWITCH_BIT3, "SWITCH_BIT3", GPIOD, GPIO_PIN_4, GPIO_MODE_IN_FLOATING}
	,{DET_SWITCH_BIT2, "SWITCH_BIT2", GPIOD, GPIO_PIN_5, GPIO_MODE_IN_FLOATING}
	,{DET_SWITCH_BIT1, "SWITCH_BIT1", GPIOD, GPIO_PIN_6, GPIO_MODE_IN_FLOATING}
	,{DET_SWITCH_BIT0, "SWITCH_BIT0", GPIOD, GPIO_PIN_7, GPIO_MODE_IN_FLOATING}
};

static DrvIo g_InIOs[] =
{
	 {DET_PUMP_OFF	 , "PUMP_OFF", GPIOA, GPIO_PIN_15, GPIO_MODE_IN_FLOATING, EXTI_15, GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_15, EXTI_TRIG_BOTH , MSG_IO_PUMP, &OD_pumpPowerOn}
	,{DET_BUCK		 , "BUCK"    , GPIOA, GPIO_PIN_0 , GPIO_MODE_IN_FLOATING, EXTI_0 , GPIO_PORT_SOURCE_GPIOA, GPIO_PIN_SOURCE_0 , EXTI_TRIG_BOTH , MSG_IO_BUCK}
	,{DET_AC_ON		 , "AC_ON"  , GPIOB, GPIO_PIN_3 , GPIO_MODE_IN_FLOATING, EXTI_3 , GPIO_PORT_SOURCE_GPIOB, GPIO_PIN_SOURCE_3 , EXTI_TRIG_BOTH , MSG_IO_AC  }

	,{DET_FUN1_FAULT, "FUN1_FAULT", GPIOD, GPIO_PIN_14, GPIO_MODE_IN_FLOATING, EXTI_14, GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_14, EXTI_TRIG_BOTH, MSG_IO_FUN, &OD_fun1Fault}
	,{DET_FUN2_FAULT, "FUN2_FAULT", GPIOD, GPIO_PIN_13, GPIO_MODE_IN_FLOATING, EXTI_13, GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_13, EXTI_TRIG_BOTH, MSG_IO_FUN, &OD_fun2Fault}
	,{DET_FUN3_FAULT, "FUN3_FAULT", GPIOD, GPIO_PIN_12, GPIO_MODE_IN_FLOATING, EXTI_12, GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_12, EXTI_TRIG_BOTH, MSG_IO_FUN, &OD_fun3Fault}
	,{DET_FUN4_FAULT, "FUN4_FAULT", GPIOD, GPIO_PIN_11, GPIO_MODE_IN_FLOATING, EXTI_11, GPIO_PORT_SOURCE_GPIOD, GPIO_PIN_SOURCE_11, EXTI_TRIG_BOTH, MSG_IO_FUN, &OD_fun4Fault}
	,{DET_FUN5_FAULT, "FUN5_FAULT", GPIOD, GPIO_PIN_10, GPIO_MODE_IN_FLOATING, (exti_line_enum)0, 0, 0, (exti_trig_type_enum)0, MSG_IO_FUN, &OD_fun5Fault}
	,{DET_FUN6_FAULT, "FUN6_FAULT", GPIOB, GPIO_PIN_7, GPIO_MODE_IN_FLOATING , (exti_line_enum)0, 0, 0, (exti_trig_type_enum)0, MSG_IO_FUN, &OD_fun6Fault}

	,{DET_BU_PRESENT, "BU_PRESENT", GPIOD, GPIO_PIN_9, GPIO_MODE_IN_FLOATING , (exti_line_enum)0, 0, 0, (exti_trig_type_enum)0, 0, &BU_isPresent}

	,{DET_BLE_LOCK	 ,"BleLock", GPIOE, GPIO_PIN_4 , GPIO_MODE_IN_FLOATING, EXTI_4 , GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_4 , EXTI_TRIG_BOTH}
	,{DET_WATER_IN	 ,"Cabinet WaterLevel", GPIOE, GPIO_PIN_5 , GPIO_MODE_IN_FLOATING, EXTI_5 , GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_5 , EXTI_TRIG_BOTH, MSG_IO_WATER , &OD_shelfWaterIn}
	,{DET_BUCKET	 ,"Bucket WaterLevel" , GPIOE, GPIO_PIN_6 , GPIO_MODE_IN_FLOATING, EXTI_6 , GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_6 , EXTI_TRIG_BOTH, MSG_IO_BUCKET, &OD_waterLevel}
	,{DET_12V_BP	 ,"12V-BP"			  , GPIOE, GPIO_PIN_12 , GPIO_MODE_IN_FLOATING, (exti_line_enum)0, 0, 0, (exti_trig_type_enum)0, MSG_IO_12VBP}
	,{DET_12V_18650BP,"18650BP"			  , GPIOE, GPIO_PIN_14 , GPIO_MODE_IN_FLOATING, (exti_line_enum)0, 0, 0, (exti_trig_type_enum)0, MSG_IO_18650BP}

	,{DET_12V_1		 ,"ACC1_DET", GPIOE, GPIO_PIN_7 , GPIO_MODE_IN_FLOATING, EXTI_7 , GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_7 , EXTI_TRIG_BOTH, MSG_IO_12V, &OD_relay1PowerOn}
	,{DET_12V_2		 ,"ACC2_DET", GPIOE, GPIO_PIN_8 , GPIO_MODE_IN_FLOATING, EXTI_8 , GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_8 , EXTI_TRIG_BOTH, MSG_IO_12V, &OD_relay2PowerOn}
	,{DET_12V_3		 ,"ACC3_DET", GPIOE, GPIO_PIN_9 , GPIO_MODE_IN_FLOATING, EXTI_9 , GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_9 , EXTI_TRIG_BOTH, MSG_IO_12V, &OD_relay3PowerOn}
	,{DET_12V_4		 ,"ACC4_DET", GPIOE, GPIO_PIN_10, GPIO_MODE_IN_FLOATING, EXTI_10, GPIO_PORT_SOURCE_GPIOE, GPIO_PIN_SOURCE_10, EXTI_TRIG_BOTH, MSG_IO_12V, &OD_relay4PowerOn}
};

static DrvIo g_OutputIOs[] =
{
	//输出配置
	 {CTRL_FUN_POWER, "FunPower"	, GPIOA, GPIO_PIN_11 ,GPIO_MODE_OUT_PP}
	,{CTRL_LIGHT	, "Light"		, GPIOA, GPIO_PIN_12 ,GPIO_MODE_OUT_PP}
	,{CTRL_PUMP		, "Pump"		, GPIOC, GPIO_PIN_10 ,GPIO_MODE_OUT_PP}
	,{CTRL_LCD_POWER, "LCD_POWER"	, GPIOC, GPIO_PIN_11 ,GPIO_MODE_OUT_PP}
	,{CTRL_LCD_RESET, "LCD_RESET"   , GPIOE, GPIO_PIN_2  ,GPIO_MODE_OUT_PP}
	,{CTRL_ACC_3    , "ACC_3"		, GPIOE, GPIO_PIN_1  ,GPIO_MODE_OUT_PP}
	,{CTRL_ACC_4	, "ACC_4"		, GPIOE, GPIO_PIN_0  ,GPIO_MODE_OUT_PP, 0, 0, 0, 0, 0, 0, 0}
	,{CTRL_ACC_5	, "ACC_5"		, GPIOB, GPIO_PIN_4  ,GPIO_MODE_OUT_PP, 0, 0, 0, 0, 0, 0, 0}
	
	,{CTRL_MCU_LED	, "MCU_LED"		, GPIOE, GPIO_PIN_11 ,GPIO_MODE_OUT_PP}
	,{CTRL_CAN_RED	, "CAN_RED"		, GPIOD, GPIO_PIN_1  ,GPIO_MODE_OUT_PP}
	,{CTRL_CAN_GREEN, "CAN_GREEN"	, GPIOD, GPIO_PIN_0  ,GPIO_MODE_OUT_PP}

	,{CTRL_485_METER_ENABLE	, "485_METER"		, GPIOA, GPIO_PIN_1  ,GPIO_MODE_OUT_PP}
	,{CTRL_485_LOCK_ENABLE	, "485_LOCK"		, GPIOE, GPIO_PIN_15 ,GPIO_MODE_OUT_PP}
	,{CTRL_485_COOLING_ENABLE, "485_COOLING"	, GPIOA, GPIO_PIN_8  ,GPIO_MODE_OUT_PP}	//和TIME0_CH0的PA8复用，用于触发ADC检测

	,{CTRL_BU_CHARGE	, "BU_CHARGE"	 , GPIOB, GPIO_PIN_14 ,GPIO_MODE_OUT_PP}
	,{CTRL_18650BP		, "18650BP"		 , GPIOB, GPIO_PIN_15 ,GPIO_MODE_OUT_PP}
	,{CTRL_BU_DISCHARGE , "BU_DISCHARGE" , GPIOD, GPIO_PIN_8  ,GPIO_MODE_OUT_PP}
	,{CTRL_HOT_FUN		, "HOT_FUN"		 , GPIOC, GPIO_PIN_13 ,GPIO_MODE_OUT_PP}
};

//=============================================
//控制IO和反馈IO状态匹配定义,
typedef struct
{
	IO_ID ctrlPin;		//控制IO
	uint8 ctrlPinState;	//控制IO状态
	IO_ID detPin;			//反馈IO
	uint8 detPinState;	//反馈IO状态
	MSG_ID msgId;	//如果发现ctrlPinState和detPinState的状态不一致，则发送消息

	DrvIo* pCtrlIo;		//指向控制IO对象
	DrvIo* pDetIo;		//指向反馈IO对象
}IOPair;

static IOPair g_IOPairs[] =
{
	{CTRL_ACC_3, IO_HIGH, DET_12V_3, IO_LOW, MSG_IO_12V_FAULT},
	{CTRL_ACC_4, IO_HIGH, DET_12V_4, IO_LOW, MSG_IO_12V_FAULT},
};

const IOPair* IOPair_Get(IO_ID pin)
{
	const IOPair* p = g_IOPairs;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_IOPairs); i++, p++)
	{
		if (p->ctrlPin == pin) return p;
	}
	return Null;
}

void IOPair_CheckFault(const IOPair* pPair)
{
	DrvIo* pCtrlIo = pPair->pCtrlIo;
	DrvIo* pDet = pPair->pDetIo;

	if (pCtrlIo->ticks && SwTimer_isTimerOutEx(pCtrlIo->ticks, 1000))
	{
		pCtrlIo->ticks = 0;

		Bool isFault = False;
		if (pPair->ctrlPinState == pPair->detPinState)
		{
			//判断设置和反馈是否匹配
			isFault = (pDet->value != pCtrlIo->value);
		}
		else
		{
			//判断设置和反馈是否匹配
			isFault = (pDet->value == pCtrlIo->value);
		}
		if (isFault)
		{
			PostMsgEx(pPair->msgId, pPair->ctrlPin, isFault);
			Printf("%s is fault, \n", pCtrlIo->desc);
		}
	}
}
//===========================================================================
//perith和名称匹配定义
typedef struct
{
	uint32 periph;
	const char* Name;
}IOPortName;
const static IOPortName g_IOPortName[] =
{
	{GPIOA,"PA"},
	{GPIOB,"PB"},
	{GPIOC,"PC"},
	{GPIOD,"PD"},
	{GPIOE,"PE"},
	{GPIOF,"PF"},
	{GPIOG,"PG"},
};

//GPIOA->"PA"
const char* GPIOxToPx(uint32 port)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_IOPortName); i++)
	{
		if (g_IOPortName[i].periph == port)
		{
			return g_IOPortName[i].Name;
		}
	}
	return "No";
}

//ch = 'A', 'B', 'C', 'D', 'E', 'F', 'G',
uint32 ABCDEFG_ToPort(char ch)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_IOPortName); i++)
	{
		if (g_IOPortName[i].Name[1] == ch)
		{
			return g_IOPortName[i].periph;
		}
	}
	return 0;
}

//根据port和pin参数，转换为字符串，例如 PortNameToStr(GPIOA, GPIO_PIN_3) -> "PA3"
//返回值："PA0";"PA1"..."PG15"
char* PortPinToPxx(uint32 port, uint32 pin)
{
	static char retStr[5];
	memset(retStr, 0, sizeof(retStr));
	strcpy(retStr, "No");

	for (int j = 0; j < 32; j++)
	{
		if (pin & (1 << j))
		{
			sprintf(retStr, "%s%d", GPIOxToPx(port), j);
			break;
		}
	}

	return retStr;
}

//str = "PA1";"PB2";...
Bool PxxToPortPin(const char* Pxx, uint32* port, uint32* pin)
{
	char ch;
	if (2 == sscanf(Pxx, "P%c%ud", &ch, pin))
	{
		*pin = BIT(*pin);
		*port = ABCDEFG_ToPort(ch);
		return *port != 0;
	}

	return False;
}

//根据exti号获取IO对象
DrvIo* IO_GetByExti(uint32 exti)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_InIOs); i++)
	{
		if (g_InIOs[i].line == exti) return &g_InIOs[i];
	}

	for (int i = 0; i < GET_ELEMENT_COUNT(g_OutputIOs); i++)
	{
		if (g_OutputIOs[i].line == exti) return &g_OutputIOs[i];
	}

	for (int i = 0; i < GET_ELEMENT_COUNT(g_SwitchIOs); i++)
	{
		if (g_SwitchIOs[i].line == exti) return &g_SwitchIOs[i];
	}

	return Null;
}

//根据pin号获取IO对象
DrvIo* IO_Get(IO_ID ioID)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_InIOs); i++)
	{
		if (g_InIOs[i].ioID == ioID) return &g_InIOs[i];
	}

	for (int i = 0; i < GET_ELEMENT_COUNT(g_SwitchIOs); i++)
	{
		if (g_SwitchIOs[i].ioID == ioID) return &g_SwitchIOs[i];
	}

	for (int i = 0; i < GET_ELEMENT_COUNT(g_OutputIOs); i++)
	{
		if (g_OutputIOs[i].ioID == ioID) return &g_OutputIOs[i];
	}

	return Null;
}

void IO_IRQHandler(DrvIo* p)
{
	uint8 isHigh = PortPin_Read(p);
	if (p->value != isHigh)
	{
		if (p->stateChangedMsg)
		{
			PostMsgEx(p->stateChangedMsg, p->ioID, isHigh);
		}
		if (p->pOdValue) *p->pOdValue = isHigh;
		p->value = isHigh;

		PFL(DL_IO, "%s(%s)=%d\n", p->desc, PortPinToPxx(p->periph, p->pin), isHigh);
	}
}

#define IO_IRQ_HANDLER(pin)	\
	static DrvIo* p = Null;	\
	if (p == Null)	\
	{	\
		p = IO_Get(pin);	\
	}	\
	p->ticks = GET_TICKS();	\
	exti_interrupt_flag_clear(p->line);

	/*IO_IRQHandler(p);	\	*/

void EXTI0_IRQHandler(void)  { IO_IRQ_HANDLER(DET_BUCK); }
void EXTI3_IRQHandler(void)  { IO_IRQ_HANDLER(DET_AC_ON);  }
void EXTI4_IRQHandler(void)  { IO_IRQ_HANDLER(DET_BLE_LOCK); }
void EXTIs_IRQHandler(int fromExti, int toExti)
{
	int ind = 0;
	exti_line_enum linex;

	//在函数运行第一次初始化p[]对象，优化中断执行时间，
	static Bool isFirst = 1;
	static DrvIo* p[11];	//指向对象DrvIo(EXTI_5...EXTI_15)，
	if (isFirst)	//仅初始化一次
	{
		isFirst = 0;
		//初始化对象指针p[11]
		for (int i = 0; i < 11; i++)
		{
			p[i] = IO_GetByExti(BIT(i + 5));
		}
	}

	for (int i = fromExti; i <= toExti; i++)
	{
		linex = (exti_line_enum)BIT(i);
		if (SET == exti_interrupt_flag_get(linex))
		{
			ind = i - 5;
			if (p[ind]) p[ind]->ticks = GET_TICKS();	//IO_IRQHandler(p[ind]);
			exti_interrupt_flag_clear(linex);
		}
	}
}
void EXTI5_9_IRQHandler(void)   { EXTIs_IRQHandler(5, 9); }
void EXTI10_15_IRQHandler(void) { EXTIs_IRQHandler(10, 15); }

void IO_PinInit(DrvIo* pDrvIo)
{
	/* enable the pin clock */
	rcu_periph_clock_enable(Rcu_Get(pDrvIo->periph));

	//Remap for CTRL_ACC_5 PB4
	gpio_pin_remap_config(GPIO_SWJ_NONJTRST_REMAP, ENABLE);

	gpio_init(pDrvIo->periph, pDrvIo->mode, GPIO_OSPEED_50MHZ, pDrvIo->pin);

	if(IS_OUT_MODE(pDrvIo->mode))	//Output mode, 设置默认值
	{
		PortPin_Set(pDrvIo->periph, pDrvIo->pin, pDrvIo->value);
	}
	else //if(pDrvIo->mode == GPIO_MODE_IN_FLOATING)
	{
		if (pDrvIo->line) {
			rcu_periph_clock_enable(RCU_AF);
			/* enable and set pin EXTI interrupt to the lowest priority */
			nvic_irq_enable(EXTI_GetIRQn(pDrvIo->line), 2U, 0U);

			/* connect pin EXTI line to pin GPIO pin */
			gpio_exti_source_select(pDrvIo->portSource, pDrvIo->pinSource);

			/* configure pin EXTI line */
			exti_init(pDrvIo->line, EXTI_INTERRUPT, pDrvIo->trig_type);
			exti_interrupt_flag_clear(pDrvIo->line);
		}
		pDrvIo->value = PortPin_Read(pDrvIo);
		if (pDrvIo->pOdValue )*pDrvIo->pOdValue = pDrvIo->value;
	}
}

uint8 PortPin_Read(const DrvIo* pDrvIo)
{
	return gpio_input_bit_get(pDrvIo->periph, pDrvIo->pin);
}

//Pxx = "PA1","PB2"..."PE15"
//value = 0;1
uint8 IODesc_Read(const char* Pxx)
{
	char ch;
	int pin = 0;
	uint8 value = 0;
	if (2 == sscanf(Pxx, "P%c%d", &ch, &pin))
	{
		uint32 port = ABCDEFG_ToPort(ch);
		if (port && pin < 16)
		{
			pin = BIT(pin);
			value = gpio_input_bit_get(port, pin);
			Printf("%s[%X,%X]=%d\n", Pxx, port, pin, value);
		}
		else
		{
			Printf("Port[%d] is invalid\n", port);
		}
	}
	else
	{
		Printf("Param[%s] is invalid\n", Pxx);
	}
	return value;
}

uint8 IO_Read(IO_ID pin)
{
	DrvIo*  p = IO_Get(pin);
	return PortPin_Read(p);
}

//Pxx = "PA1","PB2"..."PE15"
void IODesc_Set(const char* Pxx, uint8 value)
{
	uint32 port, pin = 0;
	if (PxxToPortPin(Pxx, &port, &pin))
	{
		PortPin_Set(port, pin, value);
		Printf("%s[%X,%X]=%d\n", Pxx, port, pin, gpio_input_bit_get(port, pin));
	}
	else
	{
		Printf("Param[%s] is invalid\n", Pxx);
	}
}

void IO_Set(IO_ID pin, uint8 value)
{
	DrvIo* p = IO_Get(pin);
	PortPin_Set(p->periph, p->pin, value);
	p->value = PortPin_Read(p);

	//记录当前设置的时间戳
	p->ticks = GET_TICKS();
}


void PortPin_Set(uint32 port, uint32 pin, uint8 value)
{
	Bool isMatch = False;
	static DrvIo* p[3] = { Null, Null, Null };
	if (p[0] == Null) p[0] = IO_Get(CTRL_MCU_LED);
	if (p[1] == Null) p[1] = IO_Get(CTRL_CAN_RED);
	if (p[2] == Null) p[2] = IO_Get(CTRL_CAN_GREEN);
	for (int i = 0; i < 3; i++)
	{
		if (p[i]->periph == port && p[i]->pin == pin)
		{
			isMatch = True;
			break;
		}
	}
	if (!isMatch)
	{
		const char* pStr = PortPinToPxx(port, pin);
		PFL(DL_IO, "%s set to %d\n", pStr, value);
	}

	if (value)
	{
		GPIO_BOP(port) = pin;
	}
	else
	{
		GPIO_BC(port) = pin;
	}
}

//void IO_IRQEnable(IO_ID io, Bool isEnable)
//{
//	DrvIo* pDrvIo = IO_Get(io);
//	if (pDrvIo)
//	{
//		if (pDrvIo->mode == GPIO_MODE_IN_FLOATING)
//		{
//			pDrvIo->isEnable = isEnable;
//			if (pDrvIo->line)
//			{
//				if (isEnable)
//					exti_interrupt_enable(pDrvIo->line);
//				else
//					exti_interrupt_disable(pDrvIo->line);
//			}
//		}
//	}
//}

void IO_IRQEnable(Bool isEnable)
{
	DrvIo* pDrvIo = g_InIOs;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_InIOs); i++, pDrvIo++)
	{
		if (pDrvIo->mode == GPIO_MODE_IN_FLOATING && pDrvIo->line)
		{
			if(isEnable)
				exti_interrupt_enable(pDrvIo->line);
			else
				exti_interrupt_disable(pDrvIo->line);
		}
	}

}

//获取拨码开关值
void IO_ReadSwitchValue()
{
	g_ToggleValue  = IO_Read(DET_SWITCH_BIT3); g_ToggleValue <<= 1;
	g_ToggleValue |= IO_Read(DET_SWITCH_BIT2); g_ToggleValue <<= 1;
	g_ToggleValue |= IO_Read(DET_SWITCH_BIT1); g_ToggleValue <<= 1;
	g_ToggleValue |= IO_Read(DET_SWITCH_BIT0);
	
	//取反后取低四位的值
	g_ToggleValue = (~g_ToggleValue) & 0x0F +1;

}

void IO_Stop()
{
	g_isIoStart = False;
	IO_IRQEnable(False);
}

//所有IO对象触发一次状态改变	
void IO_AllStateChanged()
{
	DrvIo* p = g_InIOs;

	for (int i = 0; i < GET_ELEMENT_COUNT(g_InIOs); i++, p++)
	{
		if (!IS_OUT_MODE(p->trig_type))
		{
			p->value = 0xFF;
			IO_IRQHandler(p);
		}
	}
}

//触发一次状态改变
void IO_TriggerStateChanged(IO_ID pin)
{
	DrvIo* p = IO_Get(pin);

	p->value = 0xFF;
	IO_IRQHandler(p);
}

void IO_Start()
{
	g_isIoStart = True;
	IO_IRQEnable(True);
}

//检查IO状态，是否发生改变
void IO_CheckIOState()
{
	#define MAX_QUERY_IO_COUNT 5
	static uint8 queryIoCount = 0;
	static DrvIo* pQueryIo[MAX_QUERY_IO_COUNT];

	//初始化pQueryIo，仅初始化一次
	DrvIo* p = g_InIOs;
	if (queryIoCount == 0)
	{
		for (int i = 0; i < GET_ELEMENT_COUNT(g_InIOs); i++, p++)
		{
			if(p->line == 0)
			{
				Assert(queryIoCount <= MAX_QUERY_IO_COUNT);
				pQueryIo[queryIoCount++] = p;
			}
		}
	}
	else
	{
		//每秒检查一下IO的状态，是否发生改变
		static uint32 initTicks = 0;
		if (initTicks == 0 || SwTimer_isTimerOutEx(initTicks, 1000))
		{
			//pAcc1->value PortPin_Read(pAcc1)
			//if(pAcc1->value)
			initTicks = GET_TICKS();
			for (int i = 0; i < queryIoCount; i++, p++)
			{
				p = pQueryIo[i];
				if (PortPin_Read(p) != p->value)
				{
					p->ticks = GET_TICKS();
				}
			}
		}
	}

	p = g_InIOs;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_InIOs); i++, p++)
	{
		//去抖动
		if (p->ticks && SwTimer_isTimerOutEx(p->ticks, 100))
		{
			p->ticks = 0;
			IO_IRQHandler(p);
		}
	}

	for (int i = 0; i < GET_ELEMENT_COUNT(g_IOPairs); i++)
	{
		IOPair_CheckFault(&g_IOPairs[i]);
	}
}

void IO_Run()
{
	if(!g_isIoStart) return;

	IO_CheckIOState();
}

DrvIo* g_pBucket;
DrvIo* g_pWaterIn;
void IO_Init()
{
	static Obj g_IoObj;
	Obj_Register(&g_IoObj, "IODriver", IO_Start, IO_Stop, IO_Run);

	for (int i = 0; i < GET_ELEMENT_COUNT(g_InIOs); i++)
	{
		IO_PinInit(&g_InIOs[i]);
	}

	for (int i = 0; i < GET_ELEMENT_COUNT(g_OutputIOs); i++)
	{
		IO_PinInit(&g_OutputIOs[i]);
	}

	for (int i = 0; i < GET_ELEMENT_COUNT(g_SwitchIOs); i++)
	{
		IO_PinInit(&g_SwitchIOs[i]);
	}
	IO_ReadSwitchValue();

	for (int i = 0; i < GET_ELEMENT_COUNT(g_IOPairs); i++)
	{
		g_IOPairs[i].pCtrlIo = IO_Get(g_IOPairs[i].ctrlPin);
		g_IOPairs[i].pDetIo = IO_Get(g_IOPairs[i].detPin);
	}

	g_pBucket = IO_Get(DET_BUCKET);
	g_pWaterIn = IO_Get(DET_WATER_IN);
}
