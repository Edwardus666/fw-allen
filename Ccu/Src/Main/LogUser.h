
#ifndef _LOG_USER_H_
#define _LOG_USER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "Log.h"

	typedef enum
	{
		OBJID_CCU = 1,
		OBJID_SLAVE,
		OBJID_PMS,
		OBJID_SM,
		OBJID_SHELF_LOCK,
		OBJID_METER,
	}LogObjID;

//����޸Ĺ�LOG�ṹ�Ķ��壬֮ǰ������FLASH��LOGֵ���ϣ���Ҫ�޸�LOG_VERSION��֮ǰ��ͬ
#define LOG_VERSION 0

	typedef enum _LogEvent
	{
		//system event
		ET_CCU_RESET = 1,
		ET_STATE_SWITCH,		//״̬�л�����ʽ (ET_STATE_SWITCH, new-operationState)
		ET_FW_UPGRADE,			//ET_FW_UPGRADE, ��ʽ (ET_FW_UPGRADE, index, subIndex, rw|result)

		ET_CCU_BOM_FAULT = 0x10,	//Ԫ��������, ��ʽ (ET_CCU_FAULT, BOM-ID, FaultID)
		ET_CCU_TEMP2,
		ET_CCU_TEMP3,
		ET_CCU_TEMP4,

		ET_PMS_301101 = 0x20,	//PMS����״̬, ��ʽ (ET_PMS_301101, addr, cmd)
		ET_PMS_STATE1,			//PMS STATE1, ��ʽ (ET_PMS_301001, 3010.01)
		ET_PMS_STATE2,			//PMS STATE2, ��ʽ (ET_PMS_301002, 3010.02)
		ET_PMS_3100,			//PMS����״̬, ��ʽ (ET_PMS_301101, addr, cmd)

		ET_PMS_BP_START = 0x28,		//����ϵͳ�����¼�, ��ʽ (ET_PMS_BP_START)
		ET_PMS_BP_STOP,				//����ϵͳ��ֹ�¼�, ��ʽ (ET_PMS_BP_STOP, reason)

		ET_SM_CMD_SET = 0x30,	//SM���������ʽ (ET_SM_CMD_CHARGE, cmd, operationState)
		ET_SM_CMD_RESET,		//SM���������ʽ (ET_SM_CMD_CHARGE, cmd, operationState)
		ET_SM_CMD_LOCK,

		ET_FSM_SWITCH = 0x40,		//FSM״̬�л�����ʽ (ET_FSM_SWITCH, newFsmState)

		ET_SHELF_LOCK_LOCK_STATE_CHANGED = 0x50,//������״̬�ı䣬��ʽ (ET_SHELF_LOCK_LOCK_STATE_CHANGED, newState)
		ET_SHELF_LOCK_OP_STATE_CHANGED		,	//����������״̬�ı䣬��ʽ (ET_SHELF_LOCK_OP_STATE_CHANGED, newState)

		ET_METER_OP_STATE_CHANGED = 0x60,		//������״̬�ı䣬��ʽ (ET_METER_OP_STATE_CHANGED, newState)
	}LogEvent;

	typedef enum
	{
		MODULE_SYS = 0
	}LOG_MODULE_ID;

#define LOG_TRACE1(objId, eventId, _val) 			{ uint32 val = (_val)					; LogUser_Write(objId, LT_TRACE , eventId, val);}
#define LOG_TRACE2(objId, eventId, b0, b1) 			{ uint32 val = AS_UINT16(b1, b0)		; LogUser_Write(objId, LT_TRACE , eventId, val);}
#define LOG_TRACE3(objId, eventId, asU16H, asU16L) 	{ uint32 val = (asU16H << 16) | asU16L	; LogUser_Write(objId, LT_TRACE , eventId, val);}
#define LOG_TRACE4(objId, eventId, b0, b1, b2, b3) 	{ uint32 val = AS_UINT32(b3, b2, b1, b0); LogUser_Write(objId, LT_TRACE , eventId, val);}

#define LOG_WARNING1(objId, eventId, _val) 			{ uint32 val = (_val)					; LogUser_Write(objId, LT_WARNING , eventId, val);}
#define LOG_WARNING2(objId, eventId, b0, b1) 		{ uint32 val = AS_UINT16(b1, b0)		; LogUser_Write(objId, LT_WARNING , eventId, val);}
#define LOG_WARNING3(objId, eventId, asU16H, asU16L) { uint32 val = (asU16H << 16) | asU16L	; LogUser_Write(objId, LT_WARNING , eventId, val);}
#define LOG_WARNING4(objId, eventId, b0, b1, b2, b3) { uint32 val = AS_UINT32(b3, b2, b1, b0); LogUser_Write(objId, LT_WARNING , eventId, val);}

#define LOG_ERROR1(objId, eventId, _val) 			{ uint32 val = (_val)					; LogUser_Write(objId, LT_ERROR , eventId, val);}
#define LOG_ERROR2(objId, eventId, b0, b1) 			{ uint32 val = AS_UINT16(b1, b0)		; LogUser_Write(objId, LT_ERROR , eventId, val);}
#define LOG_ERROR3(objId, eventId, asU16H, asU16L)	{ uint32 val = (asU16H << 16) | asU16L	; LogUser_Write(objId, LT_ERROR , eventId, val);}
#define LOG_ERROR4(objId, eventId, b0, b1, b2, b3)	{ uint32 val = AS_UINT32(b3, b2, b1, b0); LogUser_Write(objId, LT_ERROR , eventId, val);}

	extern LogItem* g_pLogItem;

	void LogUser_Init();
	void LogUser_Write(uint8 moduleId, LogType logType, uint8 eventId, uint32 val);
	void LogUser_DumpByCount(int count, uint8 objId);
	void LogUser_DumpByInd(int ind, int count, uint8 objId);
	void LogUser_Tester();

#ifdef __cplusplus
}
#endif

#endif
