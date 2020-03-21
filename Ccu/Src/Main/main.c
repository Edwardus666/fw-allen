/*!
    \file  main.c
    \brief led spark with systick, USART print and key example
*/

/*
    Copyright (C) 2017 GigaDevice

    2017-02-10, V1.0.0, firmware for GD32F403
*/
#include "common.h"
#include "main.h"
#include "DriverIo.h"
#include "Fsm.h"
#include "DriverTimer.h"
#include "Master.h"
#include "Slave.h"
#include "MemMap.h"
#include "crc32.h"
#include "OtaFile.h"
#include "SbwMgr.h"
#include "WhMeter.h"

extern void Shell_Init();

uint8	g_McuID[12];

int main(void)
{    
	memcpy(&g_McuID, (void*)(0x1FFFF7E8), 12);
	/* configure systick */
    systick_config();
	Nvds_Init();

	IO_Init();
	RunLed_Init();

	Uart_Start(Uart_Init(CCU_DEBUG_COM));
	//Uart_Start(Uart_Init(CCU_MISC_COM));
	Printf("\r\n**********************************************\r\n");
	Printf("\t Ccu Start up \r\n");
	Printf("\t Hardware Ver: %s \r\n", g_pVerInfo->manufacturerHardwareVersion);
	Printf("\t Firmware Ver: %s \r\n", g_pVerInfo->manufacturerSoftwareVersion);

	Printf("\t CcuAddr=0x%02X \r\n", g_ToggleValue);
	Printf("**********************************************\r\n");

	OtaFiles_Init();
	
	LogUser_Init();
	LOG_TRACE1(OBJID_CCU, ET_CCU_RESET, 0);

	Adc_init();

	Fsm_Init();
	TimerDrv_Init();
	Shell_Init();

	Master_Init();
	Slave_Init();

	SbwMgr_Init();
	//WhMeter_Init();

	//Objedt start********************
	ObjList_Start();

	while(1)
	{
		ObjList_Run();
    }
}
