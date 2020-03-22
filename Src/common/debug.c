
#include "Common.h"
#include "ArchDef.h"
#include "Debug.h"
//#include "Nvds.h"

unsigned int g_dwDebugLevel = 0;
#ifdef XDEBUG



void Debug_Init()
{
	g_dwDebugLevel = 0;
/*	
	g_dwDebugLevel = DL_MAIN;
	g_dwDebugLevel |= DL_ERROR;
	g_dwDebugLevel |= DL_WARNING;
	g_dwDebugLevel |= DL_ASSERT;	
	
	g_dwDebugLevel |= DL_BLE;
	g_dwDebugLevel |= DL_GPS;
	g_dwDebugLevel |= DL_GPRS;	
	g_dwDebugLevel |= DL_ATCCMD;	
	g_dwDebugLevel |= DL_SIM;
	*/
	
}

void Debug_SetLevel(uint8 level)
{
	if(level == 0)
	{
		g_dwDebugLevel = 0;
	}
	else 
	{
		g_dwDebugLevel = DL_MAIN;
		g_dwDebugLevel |= DL_ERROR;
		g_dwDebugLevel |= DL_WARNING;
		g_dwDebugLevel |= DL_ASSERT;
		if(level > 1)
		{
		}
		if(level > 2)
		{
		}
	}
	
	Printf("g_dwDebugLevel = 0x%08x\n", g_dwDebugLevel);	
	Nvds_Write(NDDS_DBG_INFO);
}
	
void Debug_SetBit(uint32 nIndex, Bool isEnable)
{
	#define PFOUT(value) Printf("\tBIT[%02d].%s\t = %d.\n", GetBitInd(value), #value, (g_dwDebugLevel >> GetBitInd(value)) & 0x01)

	if(isEnable)
	{
		g_dwDebugLevel |= (1 << nIndex);
	}
	else
	{
		g_dwDebugLevel &= ~(1 << nIndex);
	}

	PFOUT(DL_MAIN);
	PFOUT(DL_ERROR	);
	PFOUT(DL_WARNING);
	PFOUT(DL_ASSERT	);

	PFOUT(DL_IO);
	PFOUT(DL_ADC);
	PFOUT(DL_OD);
	PFOUT(DL_CAN0_RX);
	PFOUT(DL_LOG);

	PFOUT(DL_CAN_MASTER);
	PFOUT(DL_TPDO);
	PFOUT(DL_RPDO);

	PFOUT(DL_PMS);
	PFOUT(DL_CAN_SLAVE_RX);

	PFOUT(DL_TEST);

	PFOUT(DL_SHOW_CALLED);
	PFOUT(DL_SHOW_TICKS	);

	Printf("g_dwDebugLevel = ");	
	for(int i = 0; i < 32; i++)
	{
		Printf("%d", (g_dwDebugLevel >> i) & 0x01);
		if((i+1) % 4 == 0)
		{
			Printf(" ");
		}
	}
	Printf("\n");
	
	//Printf("g_dwDebugLevel = 0x%08x\n", g_dwDebugLevel);	
	Nvds_Write(NDDS_DBG_INFO);
}

void Debug_DumpByteEx(uint32 level, const uint8* pData, uint16 len, uint8 cols, const char* str, int line) 
{
    int i; 
    int counter = 0;	

	if(!(level & g_dwDebugLevel)) return;
	
	if(str && line)
	{
		Printf("%s,line[%d],Len[%d]:",str,line,len); 
	}
	else if(str)
	{
		Printf("%s,Len[%d]:",str,len); 
	}
	else if(line)
	{
		Printf("line[%d],Len[%d]:",line,len); 
	}
	
    while(counter < len) 
    { 
        for(i=0; i < cols; i++) 
        { 
            Printf("%02x ",*pData); 
            pData++; 
            if(++counter >= len) break;	
        } 
    } 
    Printf("\n"); 
}

void ShowWait(int ticksPerDot, int dotCountPerRow)
{
	static int i = 0;
	static int count = 0;

	if( (i++) % ticksPerDot == 0)
	{
		if(count++ >= dotCountPerRow)
		{
			count = 0;
			i = 0;
			Printf("\n\t");
		}
		else
		{
			Printf(".");
		}
	}
}

void Debug_DumpByte(const uint8* pData, uint16 len, uint8 cols, int max) 
{
    int i; 
    int counter = 0;	
	if(len == 0) Printf("\n");
	//Printf("Dump Data, addr=[0x%04x], Len=%d\n", pData, len); 
    while(counter < len) 
    { 
        //Printf("[%04x]:",counter); 
        for(i=0; i < cols; i++) 
        { 
            Printf("%02X ",*pData); 
            pData++; 
			++counter;
			if (counter >= len)
			{
				break;
			}
			else if (counter > max)
			{
				Printf("... \n");
				return;
			}
        } 
        Printf("\n"); 
    } 
}

void Debug_DumpDword(const uint32* pData, uint16 len, uint8 cols) 
{
    int i; 
    int counter = 0;	
    while(counter < len) 
    { 
        Printf("[%04X]:",counter); 
        for(i=0; i < cols; i++) 
        { 
            Printf("%08X ",*pData); 
            pData++; 
			
            if(++counter >= len) break;	
        } 
        Printf("\n"); 
    } 
}

#endif

