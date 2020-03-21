
#include "Common.h"
#include "DriverFmc.h"

//Extern Api for SectorMgr using
Bool FmcRead(uint32 addr, void* pData, int len)
{
	memcpy(pData, (void*)addr, len);
	return True;
}

Bool FmcErase(uint32 addr, uint32 dataLength)
{
	fmc_erase_pages(addr, addr + dataLength);
	return True;
}

Bool FmcWrite(uint32 addr, void* pData, int len)
{

	//长度必须为2的整数倍
	if (len % 2 != 0) return False;

	uint32* pUint32 = (uint32*)pData;
	uint16* pUint16 = (uint16*)pData;

	uint32 i = 0;
	fmc_state_enum ret = FMC_READY;

	int dataLength = (len >> 2) << 2;	//对齐4字节边界

	/* unlock the flash program/erase controller */
	fmc_unlock();

	for (i = 0; i < dataLength; i += 4, pUint32++)
	{
		ret = fmc_word_program(addr + i, *pUint32); 
		fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
		if (ret != FMC_READY)
		{
			goto End;
		}
	}

	if (len - i == 2)
	{
		pUint16 = (uint16*)pUint32;
		ret = fmc_halfword_program(addr + i, *pUint16);
		fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
		if (ret != FMC_READY)
		{
			goto End;
		}
	}
End:
	fmc_lock();

	return ret == FMC_READY;
}

void fmc_erase_pages(uint32 startAddr, uint32 endAddr)
{
	uint32 EraseCounter;
	uint32 FMC_PAGE_SIZE = FMC_BANK0_SIZE;
	/* calculate the number of page to be programmed/erased */
	uint32 startPageAddr = 0;

	if (startAddr < FMC_BANK0_END_ADDRESS)
	{
		FMC_PAGE_SIZE = BANK0_PAGE_SIZE;
		startPageAddr = startAddr / BANK0_PAGE_SIZE * BANK0_PAGE_SIZE;
	}
	else
	{
		FMC_PAGE_SIZE = BANK1_PAGE_SIZE;
		startPageAddr = startAddr / BANK1_PAGE_SIZE * BANK1_PAGE_SIZE;
	}

	/* unlock the flash program/erase controller */
	fmc_unlock();

	/* erase the flash pages */
	for (uint32 pageAddr = startPageAddr; pageAddr <= endAddr; pageAddr += FMC_PAGE_SIZE) {
		fmc_page_erase(pageAddr);
		/* clear all pending flags */
		fmc_flag_clear(FMC_FLAG_BANK0_END | FMC_FLAG_BANK0_WPERR | FMC_FLAG_BANK0_PGERR);
	}

	/* lock the main FMC after the erase operation */
	fmc_lock();
}
