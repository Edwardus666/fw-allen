
#include "crc32.h"
static unsigned int crc_table[256];  
    
/* 
**initialize the cre32 table 
*/  
void Crc32_Init(void)  
{  
    unsigned int c;  
    unsigned int i, j;  
      
    for (i = 0; i < 256; i++) 
	{  
        c = (unsigned int)i;  
        for (j = 0; j < 8; j++) 
		{  
            if (c & 1)  
                c = 0xedb88320L ^ (c >> 1);  
            else  
                c = c >> 1;  
        }  
        crc_table[i] = c;  
    }  
}  
  
/*compute the crc32 code for buffer*/  
unsigned int Crc32_Calc(unsigned int crc,unsigned char *buffer, unsigned int dataLength)  
{  
    unsigned int i;  
    for (i = 0; i < dataLength; i++) {  
        crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);  
    }  
    return crc ;  
}
