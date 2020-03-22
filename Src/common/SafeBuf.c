#include "ArchDef.h"
#include "SafeBuf.h"

void SafeBuf_Reset(SafeBuf* pSafeBuf)
{
	pSafeBuf->m_WriteInd = 0;
	pSafeBuf->m_ReadIndx 	= 0;
}

Bool SafeBuf_IsEmpty(const SafeBuf* pSafeBuf)
{
	return (pSafeBuf->m_WriteInd == pSafeBuf->m_ReadIndx);
}

Bool SafeBuf_IsFull(const SafeBuf* pSafeBuf)
{
	return (pSafeBuf->m_WriteInd == pSafeBuf->m_MaxSize);
}

void SafeBuf_Init(SafeBuf* pSafeBuf, void* pBuf, uint16 bufSize)
{
	memset(pSafeBuf, 0, sizeof(SafeBuf));

	//Faking the complier to change the const value
	pSafeBuf->m_pBuf = pBuf;
	pSafeBuf->m_MaxSize = bufSize;	
}

//Push one byte to SafeBuf
int SafeBuf_WriteByte(SafeBuf* pSafeBuf, uint8 data)
{
	if(pSafeBuf->m_WriteInd == pSafeBuf->m_ReadIndx)
	{
		pSafeBuf->m_WriteInd = 0;
		pSafeBuf->m_ReadIndx = 0;
	}
	
	if(pSafeBuf->m_WriteInd < pSafeBuf->m_MaxSize)
	{
		pSafeBuf->m_pBuf[pSafeBuf->m_WriteInd++] = data;
		return 1;
	}
	else
	{
		//Printf("Buf full\n");
		return 0;
	}
}

//Push one or more bytes to SafeBuf
int SafeBuf_Write(SafeBuf* pSafeBuf, const void* pData, uint16 len)
{
	int copyLen = 0;
	int bytes = 0;
	if(pSafeBuf->m_WriteInd == pSafeBuf->m_ReadIndx)
	{
		pSafeBuf->m_WriteInd = 0;
		pSafeBuf->m_ReadIndx = 0;
	}

	bytes = pSafeBuf->m_MaxSize - pSafeBuf->m_WriteInd;
	copyLen = (len > bytes) ? bytes : len;
	memcpy(&pSafeBuf->m_pBuf[pSafeBuf->m_WriteInd], pData, copyLen);
	pSafeBuf->m_WriteInd += copyLen;
	
	return copyLen;
}

//Pop one or more bytes from SafeBuf
//���pBuf == Null,����ƶ���ָ�룬����ȡ����
int SafeBuf_Read(SafeBuf* pSafeBuf, void* pBuf, int bufSize)
{
	int copyLen = 0;
	int bytes = pSafeBuf->m_WriteInd - pSafeBuf->m_ReadIndx;

	if(bytes)
	{
		copyLen = (bytes > bufSize) ? bufSize : bytes;
		if (pBuf)
		{
			memcpy(pBuf, &pSafeBuf->m_pBuf[pSafeBuf->m_ReadIndx], copyLen);
		}
		pSafeBuf->m_ReadIndx += copyLen;
	}
	
	return copyLen;
}

int SafeBuf_GetCount(const SafeBuf* pSafeBuf)
{
	return (pSafeBuf->m_WriteInd - pSafeBuf->m_ReadIndx);
}

/*************************************
�������ܣ��ڽ�������������ָ�����ֽڣ�����ȡ���ݡ�
����˵����
	pSafeBuf����������
	byte���������ַ���
	* offset��
		��������������Ľ��ջ���ȥ����ʼƫ�ƣ�
		�����������һ����������ʼλ��(�Ѿ�������ɵĳ���)
����ֵ��
	��Null��������Ŀ���ַ�������ָ�������ָ�롣
	Null��û������Ŀ���ַ���
*************************************/

uint8* SafeBuf_SearchByte(const SafeBuf* pSafeBuf, uint8 byte, int* offset)
{
	int i = (offset ? (*offset) : 0) + pSafeBuf->m_ReadIndx;
	int end = pSafeBuf->m_WriteInd;
	for (; i < end; i++)
	{
		if (byte == pSafeBuf->m_pBuf[i])
		{
			if(offset) *offset = i - pSafeBuf->m_ReadIndx + 1;

			return &pSafeBuf->m_pBuf[pSafeBuf->m_ReadIndx];
		}
	}

	if (offset) *offset = i - pSafeBuf->m_ReadIndx;
	return Null;
}

uint8* SafeBuf_GetData(const SafeBuf* pSafeBuf, int* len)
{
	*len = (pSafeBuf->m_WriteInd - pSafeBuf->m_ReadIndx);
	return &pSafeBuf->m_pBuf[pSafeBuf->m_ReadIndx];
}
