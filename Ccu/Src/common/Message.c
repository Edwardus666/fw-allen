#include "Message.h"

const char* Msg_GetName(MSG_ID id)
{
	struct
	{
		MSG_ID	id;
		const char*	name;
	}
	static const msgName[] =
	{
		{MSG_TIMEOUT,"MSG_TIMEOUT"},
		{MSG_48V_BAT_ON,"MSG_48V_BAT_ON"},

		{MSG_RUN				 ,"MSG_RUN"					},
		{MSG_RX_FRAME			 ,"MSG_RX_FRAME"			},
		{MSG_SDO_WAIT_RSP_TIMEOUT,"MSG_SDO_WAIT_RSP_TIMEOUT"},
		{MSG_ENTRY_CHANGED		 ,"MSG_ENTRY_CHANGED"		},
		{MSG_TX_DONE			 ,"MSG_TX_DONE"				},

		{MSG_PMS_BATTERY_PRESENT	,"MSG_PMS_BATTERY_PRESENT"		},
		{MSG_PMS_BATTERY_SOC_CHANGED,"MSG_PMS_BATTERY_SOC_CHANGED"	},
		{MSG_PMS_BATTERY_FAULT		,"MSG_PMS_BATTERY_FAULT"		},

		{MSG_IO_PUMP,"MSG_IO_PUMP"},
		{MSG_IO_BUCK,"MSG_IO_BUCK"},
		{MSG_IO_AC  ,"MSG_IO_AC"  },

		{MSG_IO_FUN		,"MSG_IO_FUN"},
		{MSG_IO_12VBP	,"MSG_IO_12VBP"},
		{MSG_IO_18650BP	,"MSG_IO_18650BP"},

		{MSG_IO_WATER	,"MSG_IO_WATER"},
		{MSG_IO_BUCKET	,"MSG_IO_BUCKET"},

		{MSG_IO_12V			,"MSG_IO_12V"},
		{MSG_IO_12V_FAULT	,"MSG_IO_12V_FAULT"},
		{MSG_TEMP_CHANGED	,"MSG_TEMP_CHANGED"},
	};

	for (int i = 0; i < sizeof(msgName) / sizeof(msgName[0]); i++)
	{
		if (msgName[i].id == id) return msgName[i].name;
	}
	return "Unknown";
}

void MsgIf_Init(MsgIf* pMsgIf, MsgProcFun MsgHandler, RunFun Run)
{
	pMsgIf->MsgHandler = MsgHandler;
	pMsgIf->Run = Run;
}

int MsgIf_MsgProc(void* pObj, uint8 msgId, uint32 param1, uint32 param2, const MsgMap* pMsgTbl, int nCount)
{
	int i = 0;
	
	for(i = 0; i < nCount; i++, pMsgTbl++)
	{
		if(msgId == pMsgTbl->m_MsgID)
		{
			return pMsgTbl->MsgHandler((MsgIf*)pObj, param1, param2);
		}
	}

	return -1;
}

