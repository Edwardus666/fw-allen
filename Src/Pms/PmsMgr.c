#include "Common.h"
#include "PmsMgr.h"
#include "Pms.h"
#include "DriverUart.h"
#include "Master.h"

Pms g_Pms[PMS_COUNT];

int PmsMgr_Tx(const uint8* pData, int len)
{
	return len;
}

Pms* PmsMgr_GetByAddr(uint8 addr)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Pms); i++)
	{
		//if (g_Pms[i].base.id == addr)
		{
			return &g_Pms[i];
		}
	}
	return Null;
}

void PmsMgr_Stop()
{
	Printf("Pms stop.\n");
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Pms); i++)
	{
		Pms_Stop(&g_Pms[i]);
	}
}

void PmsMgr_Start()
{
	Printf("Pms start.\n");
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Pms); i++)
	{
		Pms_Start(&g_Pms[i]);
	}

}

//设置PMS的供电模式
void PmsMgr_SetPmsPoweredMode(PmsPoweredMode mode)
{
	Pms* p = g_Pms;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Pms); i++, p++)
	{
		PmsCmd* localCmdSet = (PmsCmd*)&p->OD_pms_cmd_set;
		localCmdSet->poweredMode = mode;
	}
}

void PmsMgr_Run()
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Pms); i++)
	{
		Pms_Run(&g_Pms[i]);
	}
}

void PmsMgr_Init()
{
//	static PmsMgr g_PmsMgr;
	static const uint8 g_pmsAddr[] =
	{
		PMS_ADDR_0 ,
		PMS_ADDR_1 ,
		PMS_ADDR_2 ,
		PMS_ADDR_3 ,
		PMS_ADDR_4 ,
		PMS_ADDR_5 ,
		PMS_ADDR_6 ,
		PMS_ADDR_7 ,
		PMS_ADDR_8 ,
		PMS_ADDR_9 ,
		PMS_ADDR_10,
		PMS_ADDR_11,
		PMS_ADDR_12,
	};

	for (int i = 0; i < GET_ELEMENT_COUNT(g_pmsAddr) && i < GET_ELEMENT_COUNT(g_Pms); i++)
	{
		Pms_Init(&g_Pms[i], g_pmsAddr[i]);
		Master_AddNode((MasterNode*)&g_Pms[i]);
	}

	static Obj pmsMgr;
	Obj_Register(&pmsMgr, "PmsMgr", PmsMgr_Start, PmsMgr_Stop, PmsMgr_Run);
}
