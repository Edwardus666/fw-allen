#ifndef __SAFE_BUF__H_
#define __SAFE_BUF__H_

#ifdef __cplusplus
extern "C"{
#endif

#include "Typedef.h"

typedef struct _SafeBuf
{
	volatile uint16	m_WriteInd;	
	volatile uint16 m_ReadIndx;
	
	uint16 m_MaxSize;
	uint8*  m_pBuf;
}SafeBuf;

void SafeBuf_Init(SafeBuf* pSafeBuf, void* pBuf, uint16 bufSize);
int SafeBuf_WriteByte(SafeBuf* pSafeBuf, uint8 data);
int SafeBuf_Write(SafeBuf* pSafeBuf, const void* pData, uint16 len);
int SafeBuf_Read(SafeBuf* pSafeBuf, void* pBuf, int bufSize);
void SafeBuf_Reset(SafeBuf* pSafeBuf);
int SafeBuf_GetCount(const SafeBuf* pSafeBuf);
__inline uint8 SafeBuf_LatestByte(SafeBuf* pSafeBuf){return pSafeBuf->m_pBuf[pSafeBuf->m_WriteInd-1];};
Bool SafeBuf_IsEmpty(const SafeBuf* pSafeBuf);
Bool SafeBuf_IsFull(const SafeBuf* pSafeBuf);

/*************************************
函数功能：在接收数组中搜索指定的字节，并获取数据。
参数说明：
	pSafeBuf：搜索对象。
	byte：搜索的字符。
	* offset：
		输入参数：搜索的起始偏移位置，
		输出参数：下一次搜索的偏移位置(已经搜索完成的长度)
返回值：
	非Null：搜索到目标字符，返回指向该数据指针。
	Null：没搜索到目标字符。
*************************************/
uint8* SafeBuf_SearchByte(const SafeBuf* pSafeBuf, uint8 byte, int* offset);
uint8* SafeBuf_GetData(const SafeBuf* pSafeBuf, int* len);

#ifdef __cplusplus
}
#endif

#endif


