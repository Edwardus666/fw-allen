
#include "Common.h"
#include "Fsm.h"
#include "DriverIo.h"
#include "DriverAdc.h"
#include "DriverFun.h"
#include "Sm.h"
#include "BleLock.h"
#include "PmsMgr.h"

#define FSM_TIMER_ID_AC_DET_DELAY	1
#define FSM_TIMER_ID_S2_START		2
#define FSM_TIMER_ID_BP_DELAY		3

//备电电池仓地址
#define BP_PMS_ADDR				PMS_ADDR_0
#define BP_BAT_SOC_THRESHOLD	10

static Queue g_Queue;
static Message* g_pMsg = Null;
static FsmState g_FsmState = S0_18650POWERED;
static SwTimer g_FsmTimer;
static 	Pms* g_pBpPms;
static Bool g_isBpStart = False;

const char* stateStr[] =
{
	"S0_18650POWERED",
	"S1_48V_POWER",
	"S2_AC_POWERED",
};
void Fsm_Switch(FsmState state);
void PostMsgEx(uint8 msgId, uint32 param1, uint32 param2)
{
	MUTEX_LOCK();
	Message* pMsg = (Message*)QUEUE_getNew(&g_Queue);
	if (pMsg)
	{
		pMsg->m_MsgID = msgId;
		pMsg->m_Param1 = param1;
		pMsg->m_Param2 = param2;
	}
	else
	{
		PFL_ERROR("Msg queue full!\n");
	}
	MUTEX_UNLOCK();
}

void PostMsg(uint8 msgId)
{
	PostMsgEx(msgId, 0, 0);
}

void Fsm_Dump()
{
	Printf("State=%s\n", stateStr[g_FsmState]);
}

void Fsm_TimerStart(uint32 ms, uint32 timeId)
{
	SwTimer_Start(&g_FsmTimer, ms, timeId);
}

//备电系统启动
void Fsm_bpStop(BpEventType et)
{
	if (g_isBpStart)
	{
		LCD_POWER_OFF();

		g_isBpStart = False;
		LOG_TRACE1(OBJID_CCU, ET_PMS_BP_STOP, et);
		Fsm_Switch(S0_18650POWERED);
	}
}

Bool Fsm_bpStart()
{
	if (!g_isBpStart && Pms_GetBmsSoc(g_pBpPms) >= BP_BAT_SOC_THRESHOLD)
	{
		if (Pms_SetDischarge(g_pBpPms, 10) == DSCHG_OK)
		{
			g_isBpStart = True;
			LOG_TRACE1(OBJID_CCU, ET_PMS_BP_START, True);
			RunLed_SetMode(LED_RUN_MODE);
			return True;
		}
	}
	return g_isBpStart;
}
void Fsm_S0_18650Powered_Do()
{
	//关闭风扇电源
	FUN_POWER_OFF();

	LIGHT_POWER_OFF();
	LCD_POWER_OFF();
	PUMP_POWER_OFF();

	RM_POWER_OFF(CTRL_ACC_3);
	RM_POWER_OFF(CTRL_ACC_4);
	RM_POWER_OFF(CTRL_ACC_5);
	PmsMgr_SetPmsPoweredMode(PowerMode_18650);
	
	Fsm_TimerStart(10, DET_AC_ON);
}

void Fsm_S1_48vPowered_Do()
{
	//关闭风扇电源
	FUN_IO_POWER_OFF();
	LCD_POWER_OFF();
	LIGHT_POWER_OFF();
	PUMP_POWER_OFF();

	//SET 18650BP ON
	IO_Set(CTRL_18650BP, IO_HIGH);

	//IO_AllStateChanged();
	//Adc_AllStateChanged();
	Fsm_TimerStart(500, DET_12V_1);
}

void Fsm_S2_acPowered_Do()
{
	//触发一次IO状态改变消息，检测继电器1和2是否有电
	Adc_Stop();
	Fsm_TimerStart(1000, FSM_TIMER_ID_S2_START);
	PmsMgr_SetPmsPoweredMode(PowerMode_220V);
}

void Fsm_Switch(FsmState state)
{
	if (g_FsmState == state) return;

	RunLed_SetMode(LED_RUN_MODE);

	//PFL(DL_FSM, "State Switch: %s->%s\n", stateStr[g_FsmState], stateStr[state]);
	LOG_TRACE1(OBJID_CCU, ET_FSM_SWITCH, state);
	g_FsmState = state;

	SwTimer_Stop(&g_FsmTimer);

	if (g_FsmState == S0_18650POWERED)
	{
		Fsm_S0_18650Powered_Do();
	}
	else if (g_FsmState == S1_48V_POWER)
	{
		Fsm_S1_48vPowered_Do();
	}
	else if (g_FsmState == S2_AC_POWERED)
	{
		Fsm_S2_acPowered_Do();
	}
}

void Fsm_S0_18650Powered(uint8 msgID, uint32 param1, uint32 param2)
{
	if (msgID == MSG_TIMEOUT)
	{
		if (g_FsmTimer.m_Id == DET_AC_ON)
		{
			//检测继电器1和2状态并触发消息MSG_IO_AC
			IO_TriggerStateChanged(DET_AC_ON);
		}
	}
	else if (g_FsmTimer.m_isStart)
	{
		//定时器启动时，忽略其他IO消息，实现延时检测功能。
		return;
	}
	else if (msgID == MSG_IO_AC)
	{
		if (param2 == AC_ON)
		{
			Fsm_Switch(S2_AC_POWERED);
		}
		else 
		{
			if (RM_POWER_IS_ON(DET_12V_1))
			{
				LOG_ERROR2(OBJID_CCU, ET_CCU_BOM_FAULT, DET_AC_ON, RELAY_ERR_ON);
				Fsm_Switch(S2_AC_POWERED);
			}
			else
			{
				if (BU_isPresent)
				{
					Fsm_Switch(S1_48V_POWER);
				}
				else
				{
					Printf("BU is not present.\n");
				}
			}
		}
	}
}

void Fsm_S1_48vPowered(uint8 msgID, uint32 param1, uint32 param2)
{
	if (msgID == MSG_TIMEOUT)
	{
		if (g_FsmTimer.m_Id == DET_12V_1)
		{
			//检测继电器1和2状态并触发消息MSG_IO_12V
			IO_TriggerStateChanged(DET_12V_1);
			IO_TriggerStateChanged(DET_12V_2);
		}
		else if (g_FsmTimer.m_Id == DET_12V_4)
		{
			//BU_CTR3_R输出低电平，启动240隔离砖模块
			IO_TriggerStateChanged(DET_12V_4);
		}
	}
	else if (g_FsmTimer.m_isStart)
	{
		//定时器启动时，忽略其他IO消息，实现延时检测功能。
		return;
	}
	else if (msgID == MSG_IO_AC)
	{
		if (param2 == AC_ON)
		{
			Fsm_bpStop(BP_AC_ON);
		}
	}
	else if (msgID == MSG_IO_18650BP)
	{
		if (param2 == RELAY_12V_ON)
		{
			PmsMgr_Start();
		}
		else
		{
			//错误
			LOG_ERROR2(OBJID_CCU, ET_CCU_BOM_FAULT, DET_12V_18650BP, RELAY_ERR_ON);
		}
	}
	else if (msgID == MSG_IO_12V)
	{
	}
	else if (msgID == MSG_PMS_BATTERY_PRESENT && param1 == BP_PMS_ADDR)
	{
		if (Pms_IsBatIn(g_pBpPms))	//电池插入
		{
			Fsm_bpStart();
		}
		else if (g_isBpStart)	//电池拔出
		{
			RunLed_SetMode(LED_BP_NO_BATTERY_OR_SOC_LESS);
			Fsm_bpStop(BP_BAT_PLUGOUT);
		}
	}
	else if (msgID == MSG_PMS_BATTERY_SOC_CHANGED && param1 == BP_PMS_ADDR)
	{
		//启动备电系统
		if (Pms_GetBmsSoc(g_pBpPms) >= BP_BAT_SOC_THRESHOLD)
		{
			Fsm_bpStart();
		}
		else if(g_isBpStart)
		{
			RunLed_SetMode(LED_BP_NO_BATTERY_OR_SOC_LESS);
			Fsm_bpStop(BP_BAT_SOC_LESS);
		}
	}
	else if (msgID == MSG_IO_12VBP)
	{
		if (param2 != RELAY_12V_ON)
		{
			//错误
			LOG_ERROR2(OBJID_CCU, ET_CCU_BOM_FAULT, DET_12V_BP, RELAY_ERR_ON);
			RunLed_SetMode(LED_RELAY_1_2_FAULT);
			Fsm_bpStop(BP_12VBP_DET_OFF);
		}
		else
		{
			LCD_POWER_ON();
			LCD_RESET_ON();
			PmsMgr_SetPmsPoweredMode(PowerMode_48V);
			Sm_Start();
		}
	}
	else if (msgID == MSG_PMS_BATTERY_FAULT && param1 == BP_PMS_ADDR)
	{
		//判定故障是否影响备点功能，是则停止备电功能
	}
}

void Fsm_S2_acPowered(uint8 msgID, uint32 param1, uint32 param2)
{
	if (msgID == MSG_TIMEOUT)
	{
		if (FSM_TIMER_ID_S2_START == param1)
		{
			//检测继电器1和2状态并触发消息MSG_IO_12V
			IO_TriggerStateChanged(DET_12V_1);
			IO_TriggerStateChanged(DET_12V_2);
		}
	}
	else if (g_FsmTimer.m_isStart)
	{
		//定时器启动时，忽略其他IO消息，实现延时检测功能。
		return;
	}
	else if (msgID == MSG_IO_AC)
	{
		if (param2 == AC_OFF)
		{
			Fsm_Switch(S0_18650POWERED);
		}
	}
	else if (msgID == MSG_IO_12V)
	{
		if (param1 == DET_12V_1)
		{
			if (param2 == RELAY_12V_ON)
			{
				//启动系统管理(LCD)
				Sm_Start();

				//蓝牙锁启动
				//BleLock_Start();

				//风扇(电磁锁)继电器上电
				RM_POWER_ON(CTRL_ACC_3);

				//PMS供电继电器上电
				RM_POWER_ON(CTRL_ACC_4);

				//启动ADC检测温度
				Adc_Start();

				LCD_POWER_ON();
				LCD_RESET_ON();
			}
			else
			{
				//继电器1故障
				LOG_ERROR2(OBJID_CCU, ET_CCU_BOM_FAULT, DET_12V_1, RELAY_ERR_ON);
			}
		}
		else if (param1 == DET_12V_2)
		{
			if (param2 == RELAY_12V_OFF)
			{
				//继电器2故障
				LOG_ERROR2(OBJID_CCU, ET_CCU_BOM_FAULT, DET_12V_2, RELAY_ERR_ON);
			}
		}
		else if (param1 == DET_12V_3)
		{
			if (param2 == RELAY_12V_OFF)
			{
				//继电器3故障
				LOG_ERROR2(OBJID_CCU, ET_CCU_BOM_FAULT, DET_12V_3, RELAY_ERR_ON);
			}
		}
		else if (param1 == DET_12V_4)
		{
			if (param2 == RELAY_12V_ON)
			{
				PmsMgr_Start();
			}
			else
			{
				//继电器4故障
				LOG_ERROR2(OBJID_CCU, ET_CCU_BOM_FAULT, DET_12V_4, RELAY_ERR_ON);
			}
		}

	}
	else if (msgID == MSG_IO_AC)
	{

	}
}

void Fsm_allDo(uint8 msgID, uint32 param1, uint32 param2)
{
}

void Fsm_MsgProc(void)
{
	int i = 0;

	struct
	{
		FsmState state;
		void (*Handler)(uint8 msgID, uint32 param1, uint32 param2);
	}
	static const map[] =
	{
		 {S0_18650POWERED	, Fsm_S0_18650Powered}
		,{S1_48V_POWER		, Fsm_S1_48vPowered}
		,{S2_AC_POWERED		, Fsm_S2_acPowered}
	};

	//消息处理
	for (g_pMsg = (Message*)QUEUE_getHead(&g_Queue); g_pMsg; QUEUE_removeHead(&g_Queue), g_pMsg = (Message*)QUEUE_getHead(&g_Queue))
	{
		PFL(DL_MSG, "%s(0x%x(%d),0x%x(%d))\n", Msg_GetName(g_pMsg->m_MsgID), g_pMsg->m_Param1, g_pMsg->m_Param1, g_pMsg->m_Param2, g_pMsg->m_Param2);
		for (i = 0; i < GET_ELEMENT_COUNT(map); i++)
		{
			if (g_FsmState == map[i].state)
			{
				Fsm_allDo(g_pMsg->m_MsgID, g_pMsg->m_Param1, g_pMsg->m_Param2);
				map[i].Handler(g_pMsg->m_MsgID, g_pMsg->m_Param1, g_pMsg->m_Param2);
				break;
			}
		}
	}
}

void Fsm_Run()
{
	if (SwTimer_isTimerOut(&g_FsmTimer))
	{
		PostMsgEx(MSG_TIMEOUT, g_FsmTimer.m_Id, 0);
	}

	Fsm_MsgProc();
}

void Fsm_Start()
{
	g_pBpPms = PmsMgr_GetByAddr(BP_PMS_ADDR);
	Fsm_Switch(S0_18650POWERED);
}

void Fsm_Init()
{
	static Obj g_FsmObj;
	static Message 	g_QueueBuff[100];
	QUEUE_init(&g_Queue, g_QueueBuff, sizeof(Message), sizeof(g_QueueBuff) / sizeof(Message));

	Obj_Register(&g_FsmObj, "Fsm", Fsm_Start, Null, Fsm_Run);
	g_FsmState = S_INIT;
}
