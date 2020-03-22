#ifndef __STRING_H_
#define __STRING_H_

#ifdef __cplusplus
extern "C"{
#endif
	typedef struct
	{
		uint8 ind;
		const char* str;
	}Value_Str;
int str_htoi(const char *s);
char* strstrex(const char* pSrc, const char* pDst);
int str_tok(char* strDst, const char* separator, int startInd, char* strArray[], int count);
char* strstr_remove(char* pSrc, const char* pDst, char**pValue);
int strtokenValueByIndex(char* strDst, const char* separator, int index);
Bool Stream_IsIncludeTag(uint8* tag, int len, uint8 byte);
int mem_mov(void* pDst, uint16 dstLen, void* pSrc, uint16 srcLen, int movLen);
uint32 SetMaskBits(uint32 dst, uint32 mask, uint32 value);
void convertMac(const char* strMac, uint8* buff);
char* strcpyEx(char* pSrc, const char* pDst, const char* startStr, const char* endStr);
int GetBitInd(uint32 bitMask);
Bool IsPrintChar(uint8 byte);
int IsPktEnd(uint8 byte, char tag, uint8* buf, int* ind, int buflen);
uint8* bytesSearch(const uint8* pSrc, int len, const char* pDst);
uint8* bytesSearchEx(const uint8* pSrc, int len, const char* pDst);
char* ToBinStr(void* pByte, int len, char* str);
#define strtoken(strDst, separator, strArray, count) str_tok(strDst, separator, 0, strArray, count)

#ifdef __cplusplus
}
#endif

#endif


