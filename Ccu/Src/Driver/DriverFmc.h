

#ifndef _DRIVER_FMC_H
#define _DRIVER_FMC_H

#include "typedef.h"

#define BANK0_SECTOR_SIZE	2048	//bank0�����������ֽ�����2K
#define BANK0_PAGE_SIZE		2048	//bank0������ҳ��СΪ2KB����������С��λ
#define BANK1_SECTOR_SIZE	4096	//bank1�����������ֽ�����4K
#define BANK1_PAGE_SIZE		4096    //bank1������ҳ��СΪ4KB����������С��λ

Bool fmc_program(uint32 addr, void* pData, int len);
void fmc_erase_pages(uint32 startAddr, uint32 endAddr);

#endif //_DRIVER_FMC_H
