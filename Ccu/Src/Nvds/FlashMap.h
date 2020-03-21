#ifndef __FLASH_MAP_H_
#define __FLASH_MAP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"

//GD32F303xx_VK Flash代码和数据空间定义
#define FLASH_TOTAL_SIZE		(3*1024*1024)	//3M

#define FLASH_BANK0_SIZE		(512*1024)

#define BOOT_LOADER_AREA_SIZE	(8*1024)	
#define FLASH_CODE_AREA_SIZE	(200*1024)	
#define FLASH_BANK0_PAGE_SIZE	(2*1024)	

#define FLASH_BANK1_PAGE_SIZE	(4*1024)	

	typedef struct _FlashMap
	{
		uint8 bootLoaderArea[BOOT_LOADER_AREA_SIZE];

		uint8 appCodeArea1[FLASH_CODE_AREA_SIZE];
		uint8 appCodeArea2[FLASH_CODE_AREA_SIZE];

		uint8 idInfoArea    [FLASH_BANK0_PAGE_SIZE];
		uint8 debugInfoArea[FLASH_BANK0_PAGE_SIZE];
		uint8 cfgInfoArea  [FLASH_BANK0_PAGE_SIZE];
		uint8 runInfoArea  [FLASH_BANK0_PAGE_SIZE];

		uint8 reserved[FLASH_BANK0_SIZE 
			- BOOT_LOADER_AREA_SIZE 
			- FLASH_CODE_AREA_SIZE * 2 
			- FLASH_BANK0_PAGE_SIZE * 4];

		//512K
		uint8 dataArea[1];
	}FlashMap;

	extern const FlashMap* const  g_pFlashMap;
#ifdef __cplusplus
}
#endif

#endif

