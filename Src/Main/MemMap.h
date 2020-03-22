#ifndef __EP_ROM_H_
#define __EP_ROM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "DriverFmc.h"

//存储映射，定义本项目的存储器映射区域关系
#define  BOOT_LOADER_ADDR		0
#define  BOOT_LOADER_SIZE		8192

#define  APP_ADDR				(BOOT_LOADER_ADDR + BOOT_LOADER_SIZE)
#define  APP_SIZE				(200*1024)

#define  VERSION_INFO_ADDR		(APP_ADDR + APP_SIZE)
#define  VERSION_INFO_SIZE		BANK0_PAGE_SIZE

#define  CFG_INFO_ADDR			(VERSION_INFO_SIZE + VERSION_INFO_ADDR)
#define  CFG_INFO_SIZE			BANK0_PAGE_SIZE

#define  DBG_INFO_ADDR			(CFG_INFO_ADDR + CFG_INFO_SIZE)
#define  DBG_INFO_SIZE			BANK0_PAGE_SIZE

#define  APP_BACKUP_ADDR		(DBG_INFO_ADDR + DBG_INFO_SIZE)
#define  APP_BACKUP_SIZE		APP_SIZE

#define  BANK0_RESERVED_ADDR		(APP_BACKUP_ADDR + APP_BACKUP_SIZE)
#define  BANK0_RESERVED_SIZE		(0x80000 - BANK0_RESERVED_ADDR)

#define  PMS_CODE_ADDR		(BANK0_RESERVED_ADDR + BANK0_RESERVED_SIZE)
#define  PMS_CODE_SIZE		(307200)	//300K
#define  CHARGER_CODE_ADDR	(PMS_CODE_ADDR + PMS_CODE_SIZE)
#define  CHARGER_CODE_SIZE	(32768)		//32K
#define  BMS_CODE_ADDR		(CHARGER_CODE_ADDR + CHARGER_CODE_SIZE)
#define  BMS_CODE_SIZE		(65536)		//64K


#define  RESERVED_AREA_ADDR		(BMS_CODE_ADDR + BMS_CODE_SIZE)
#define  RESERVED_AREA_SIZE		(0x200000 - RESERVED_AREA_ADDR)

#define	 LOG_AREA_ADDR			(RESERVED_AREA_ADDR + RESERVED_AREA_SIZE)
#define	 LOG_AREA_SIZE			0x100000	//1M

	/*******************************************************************************
	//MCU的数据和代码存储区域映射定义
	*/
	typedef struct
	{
		uint8 bootLoader[BOOT_LOADER_SIZE];		//Boot loader 代码区

		uint8 app[APP_SIZE];					//应用程序区

		uint8 verInfo[VERSION_INFO_SIZE];		//版本信息区
		uint8 cfgInfo[CFG_INFO_SIZE];			//配置信息区
		uint8 dbgInfo[DBG_INFO_SIZE];			//调试信息区

		uint8 appBackUp[APP_BACKUP_SIZE];		//应用程序区升级备份区
		uint8 bank0Reserved[BANK0_RESERVED_SIZE];	//bank0保留区 - 512K

		uint8 pmsCode[PMS_CODE_SIZE];			//pms固件代码区，bank1 512K-812K，300K
		uint8 chargerCode[CHARGER_CODE_SIZE];	//充电器固件代码区，bank1 812K-844K，32K
		uint8 bmsCode[BMS_CODE_SIZE];			//bms固件代码bank1 844K-908K，64K

		uint8 reserved[RESERVED_AREA_SIZE];		//保留区

		uint8 logArea[LOG_AREA_SIZE];			//日志区,2M-3M
	}MemMap;

	extern const MemMap* g_pMemMap;

#ifdef __cplusplus
}
#endif

#endif

