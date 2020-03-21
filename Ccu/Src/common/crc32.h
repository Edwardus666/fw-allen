
#ifndef __CRC__H_
#define __CRC__H_

#ifdef __cplusplus
extern "C"{
#endif

#define CRC32_CHECK_CODE       0xa55aa55a

void 		 Crc32_Init(void);  
unsigned int Crc32_Calc(unsigned int crc, unsigned char * buffer, unsigned int dataLength);  


#ifdef __cplusplus
}
#endif

#endif //__CRC__H_
