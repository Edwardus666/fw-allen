
#include "Common.h"
#include "Slave.h"
#include "CANopen.h"
#include "DriverIo.h"
#include "Od.h"

static DrvIo* g_pRedLed = Null, * g_pGreenLed = Null;

/*******************************************************************************/
void programStart(void){

}


/*******************************************************************************/
void communicationReset(void){

}


/*******************************************************************************/
void programEnd(void){

}


/*******************************************************************************/
void programAsync(uint16_t timer1msDiff){

}

void Slave_Dump()
{
	Printf("CCU addr: %X\n", g_ToggleValue);
	Printf("CCU State: %X\n", CO->NMT->operatingState);
}

void Slave_OnStateChanged(CO_NMT_internalState_t state)
{
	static uint8 localState = 0xFF;
	if (localState != state)
	{
		LOG_TRACE1(OBJID_SLAVE, ET_STATE_SWITCH, state);
		localState = state;
		//PFL(DL_MAIN, "CCU State switch to %X\n", localState);
	}
}
/*******************************************************************************/
CO_SDO_abortCode_t Slave_OnChanged_Common(CO_ODF_arg_t* p)
{
	Printf("PMS OD[%x.%x]=", p->index, p->subIndex); DUMP_BYTE(p->data, p->dataLength);
	return CO_SDO_AB_NONE;
}

CO_SDO_abortCode_t Slave_FileEvent(OtaFile* pFile, SdoTransEventType type, uint16 index, uint8 subIndex, CO_ODF_arg_t* p)
{
	uint32 remainSize = pFile->fileLen - p->offset;
	uint32 maxSize = CO_SDO_BUFFER_SIZE / 7 * 7;
	uint8* pByte = (uint8*)pFile->addr;

	if (TRANS_GET_TOTAL_LEN == type)
	{
		p->dataLengthTotal = pFile->fileLen;
		return CO_SDO_AB_NONE;
	}
	else if (TRANS_BUF_EMPTY == type)
	{
		p->dataLength = MIN(maxSize, remainSize);
		p->lastSegment = (remainSize <= maxSize);

		memcpy(p->data, &pByte[p->offset], p->dataLength);
	}
	else if (TRANS_BUF_FULL == type)
	{
	}
	else if (TRANS_DONE == type)
	{
		if (p->reading)
		{
		}
		else
		{
			maxSize = p->offset + p->dataLength;
			Printf("%s Write success.len=0x%X(%d)\n", pFile->fileName, maxSize, maxSize);
		}
	}
	return CO_SDO_AB_NONE;
}

CO_SDO_abortCode_t Slave_Entry_2FF0(CO_ODF_arg_t* p)
{
	if (p->subIndex != 3) return CO_SDO_AB_NONE;

	static OtaFile* pFile = Null;

	if (p->reading)
	{
		//读数据
		if (p->offset == 0)
		{
			pFile = OtaFiles_Find(p->index);
			if (pFile == Null || pFile->pInfoSector == Null)
			{
				Printf("%s invalid\n", pFile->fileName);
				return CO_SDO_AB_NO_RESOURCE;
			}
			Slave_FileEvent(pFile, TRANS_GET_TOTAL_LEN, p->index, p->subIndex, p);
		}
		return Slave_FileEvent(pFile, TRANS_BUF_EMPTY, p->index, p->subIndex, p);
	}
	else
	{
		return Slave_FileEvent(pFile, (p->lastSegment) ? TRANS_DONE : TRANS_BUF_FULL, p->index, p->subIndex, p);
	}
	//return CO_SDO_AB_NONE;
}

void Slave_Red_Green_Led()
{
	if (g_pGreenLed->value != LED_GREEN_RUN(CO->NMT))
	{
		g_pGreenLed->value = LED_GREEN_RUN(CO->NMT);
		PortPin_Set(g_pGreenLed->periph, g_pGreenLed->pin, g_pGreenLed->value);
	}

	if (g_pRedLed->value != LED_RED_ERROR(CO->NMT))
	{
		g_pRedLed->value = LED_RED_ERROR(CO->NMT);
		PortPin_Set(g_pRedLed->periph, g_pRedLed->pin, g_pRedLed->value);
	}
}

//计算调用一次花费多少uS，计算方法：统计每MS调用多次，最后计算平均值
uint32 Slave_costUsPerCallOneTimes()
{
	static uint16_t count = 0;
	static uint16_t timer1msPrevious = 0;
	static uint16_t returnVal = 25;

	count++;
	if (GET_TICKS() - timer1msPrevious >= 1)
	{
		timer1msPrevious = GET_TICKS();

		//计算每MS跑多少个循环,计算多次取平均值
		static int ind = 0;
		static uint16 callCount[9];

		callCount[ind++] = count;
		count = 0;

		if (ind == GET_ELEMENT_COUNT(callCount))
		{
			//不要计算第一个样品
			for (int i = 1; i < GET_ELEMENT_COUNT(callCount); i++)
			{
				count += callCount[i];
			}
			returnVal = 1000 / (count / (ind-1));
			ind = 0;
		}
	}
	return returnVal;
}

void Slave_Run()
{
	/* Global variables and objects */
	static uint16_t timer1usDiff = 0;
	static uint16_t timer1msPrevious = 0;
	uint16_t timer1msDiff = GET_TICKS() - timer1msPrevious;

	if (timer1msDiff >= 1)
	{
		timer1msPrevious = GET_TICKS();
	}
	timer1usDiff = Slave_costUsPerCallOneTimes();

	/* CANopen process */
	CO_process(CO, timer1msDiff, NULL);

	if (CO->CANmodule[0]->CANnormal) {
		bool_t syncWas;

		/* Process Sync and read inputs */
		syncWas = CO_process_SYNC_RPDO(CO, timer1usDiff);

		/* Further I/O or nonblocking application code may go here. */

		/* Write outputs */
		CO_process_TPDO(CO, syncWas, timer1usDiff);

		/* verify timer overflow */
		if (0) {
			CO_errorReport(CO->em, CO_EM_ISR_TIMER_OVERFLOW, CO_EMC_SOFTWARE_INTERNAL, 0U);
		}
	}

	//设置LED
	Slave_Red_Green_Led();
}

void Slave_Stop()
{

}

void Slave_Start()
{
	/* start CAN */
	CO_CANsetNormalMode(CO->CANmodule[0]);
}

CO_OD_entry_t* Slave_GetOd(uint16 index, uint8 subInd)
{
	return 	Od_GetEntry((CO_OD_entry*)CO_OD, CO_OD_NoOfElements, index, subInd);
}

void Slave_OdExtenInit()
{
	CO_OD_configure(CO->SDO[0], 0x2120, Slave_OnChanged_Common, Null, 0, 0);

	CO_OD_configure(CO->SDO[0], 0x2FF0, Slave_Entry_2FF0, Null, 0, 0);
	CO_OD_configure(CO->SDO[0], 0x2FF1, Slave_Entry_2FF0, Null, 0, 0);
	CO_OD_configure(CO->SDO[0], 0x2FF2, Slave_Entry_2FF0, Null, 0, 0);
	CO_OD_configure(CO->SDO[0], 0x2FF3, Slave_Entry_2FF0, Null, 0, 0);
}

void Slave_Init()
{
	static Obj g_SlaveObj;
	CO_ReturnError_t err;

	g_pRedLed = IO_Get(CTRL_CAN_RED);
	g_pGreenLed = IO_Get(CTRL_CAN_GREEN);
	Assert(g_pRedLed && g_pGreenLed);

	err = CO_init(0/* CAN module address */, g_ToggleValue/* NodeID */, 125 /* bit rate */);
	CO_NMT_initCallback(CO->NMT, Slave_OnStateChanged);

	Slave_OdExtenInit();

	if (err != CO_ERROR_NO) {
		while (1);
		/* CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err); */
	}

	Obj_Register(&g_SlaveObj, "CanSlave", Slave_Start, Slave_Stop, Slave_Run);

	memset(CO_OD_RAM.manufacturerHardwareVersion, 0, sizeof(CO_OD_RAM.manufacturerHardwareVersion));
	memset(CO_OD_RAM.manufacturerSoftwareVersion, 0, sizeof(CO_OD_RAM.manufacturerSoftwareVersion));
	strcpy(CO_OD_RAM.manufacturerHardwareVersion, g_pVerInfo->manufacturerHardwareVersion);
	strcpy(CO_OD_RAM.manufacturerSoftwareVersion, g_pVerInfo->manufacturerSoftwareVersion);
}
