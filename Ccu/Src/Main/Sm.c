
#include "Common.h"
#include "Sm.h"
#include "DriverIo.h"

static Sm* g_pSm;

void Sm_Reset()
{
	static uint32 ticks = 0;

	if (ticks == 0)
	{
		ticks = GET_TICKS();
		IO_Set(CTRL_LCD_RESET, 0);
	}
	else if (SwTimer_isTimerOutEx(ticks, 2000))
	{
		ticks = 0;
		IO_Set(CTRL_LCD_RESET, 1);
	}
}

void Sm_Run()
{
	if (SM_START != g_pSm->state) return;
}

void Sm_Stop()
{
	g_pSm->state = SM_STOP;
	LCD_POWER_OFF();
}

//Sm Æô¶¯
void Sm_Start()
{
	if (g_pSm == Null || SM_START == g_pSm->state) return;
	g_pSm->state = SM_START;
	LCD_POWER_ON();
}

void Sm_Init()
{
	static Sm g_Sm;
	static Obj g_SmObj;
	g_pSm = &g_Sm;

	memset(g_pSm, 0, sizeof(Sm));
	g_pSm->state = SM_INIT;

	Obj_Register(&g_SmObj, "Sm", Sm_Start, Sm_Stop, Sm_Run);
}
