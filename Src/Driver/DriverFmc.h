

#ifndef _DRIVER_FMC_H
#define _DRIVER_FMC_H

#include "typedef.h"

#define BANK0_SECTOR_SIZE	2048	//bank0的闪存扇区字节数，2K
#define BANK0_PAGE_SIZE		2048	//bank0的闪存页大小为2KB，擦除的最小单位
#define BANK1_SECTOR_SIZE	4096	//bank1的闪存扇区字节数，4K
#define BANK1_PAGE_SIZE		4096    //bank1的闪存页大小为4KB，擦除的最小单位

Bool fmc_program(uint32 addr, void* pData, int len);
void fmc_erase_pages(uint32 startAddr, uint32 endAddr);

#endif //_DRIVER_FMC_H
