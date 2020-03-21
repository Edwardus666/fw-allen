
#include "Common.h"
#include "DriverIo.h"
#include "CcuLed.h"

static LedState g_RunLedMode[] = 
{
	{LED_RUN_MODE					, 0, 1000, 1000},	//正常运行，呼吸灯
	{LED_RELAY_1_2_FAULT			, 0, 200 , 50},		//1#或者2#继电器故障
	{LED_BP_NO_BATTERY_OR_SOC_LESS	, 0, 200 , 200},	//备电仓没电池，或者电池电量不足
};
static DrvIo* g_pLedIO = Null;
LedState* g_pLedMode = g_RunLedMode;

//运行灯闪烁
LedState* RunLed_GetMode(LedID ledID)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_RunLedMode); i++)
	{
		if (g_RunLedMode[i].m_LedId == ledID) return &g_RunLedMode[i];
	}

	return Null;
}

void RunLed_SetMode(LedID mode)
{
	g_pLedMode = RunLed_GetMode(mode);
}

void RunLed_Run()
{
	static uint32 tick = 0;
	static Bool isOn = True;
	//初始化
	isOn = (g_pLedMode->m_State == LED_STATUS_ON);
	if (SwTimer_isTimerOutEx(tick, isOn ? g_pLedMode->m_onMs : g_pLedMode->m_offMs))
	{
		tick = GET_TICKS();
		g_pLedMode->m_State = isOn ? LED_STATUS_OFF : LED_STATUS_ON;
		PortPin_Set(g_pLedIO->periph, g_pLedIO->pin, isOn);
	}
}

void RunLed_Init()
{
	static Obj g_LedObj;
	Obj_Register(&g_LedObj, "LED", Null, Null, RunLed_Run);

	g_pLedMode = RunLed_GetMode(LED_RUN_MODE);
	g_pLedIO = IO_Get(CTRL_MCU_LED);
}
