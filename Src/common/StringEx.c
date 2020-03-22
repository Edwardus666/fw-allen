
#include "Common.h"
#include "StringEx.h"

//1111 0101
char* ByteToBinStr(uint8 byte, char* str)
{
	#if 1
	for(int i = 0; i < 4; i++)
	{
		str[i] = (byte >> i & 0x01) ? '1' : '0';
	}
	str[4] = ' ';
	for(int i = 4; i < 8; i++)
	{
		str[i+1] = (byte >> i & 0x01) ? '1' : '0';
	}
	str[9] = 0;
	#else
	for(int i = 7; i >= 4; i--)
	{
		str[7-i] = (byte >> i & 0x01) ? '1' : '0';
	}
	str[4] = ' ';
	for(int i = 3; i >= 0; i--)
	{
		str[7-i+1] = (byte >> i & 0x01) ? '1' : '0';
	}
	str[9] = 0;
	#endif
	
	return str;
}

char* ToBinStr(void* pData, int len, char* str)
{
	int ind = 0;
	uint8* pByte = (uint8*)pData;
	for(int i = 0; i < len; i++)
	{
		ByteToBinStr(pByte[i], &str[ind]);
		ind = (int)strlen(str);
		if(i < len - 1)
		{
			str[ind++] = ' ';
		}
	}

	return str;
}

Bool IsPrintChar(uint8 byte)
{
	return (byte >= 0x20 && byte <= 0x7E) || byte == '\r' || byte == '\n';
}

int GetBitInd(uint32 bitMask)
{
	int i = 0; 
	for(i = 0; i < 32; i++)
	{
		if(bitMask == (1<<i)) 
			return i;
	}

	return 0;
}

uint32 SetMaskBits(uint32 dst, uint32 mask, uint32 value)
{
    int i;
//	uint8 count = 0;

    for (i=0; i<32; i++) 
	{
        if (mask & (1 << i)) 
		{
			dst &= ~(1 << i);
			dst |= ((value & 0x01) << i);
			value >>= 1;
		}
    }
	return dst;
}

void convertMac(const char* strMac, uint8* buff)
{
	int i = 0;
	
	for(i=0;i<6;i++)
	{
		int tempHex;
		sscanf(&strMac[i*2],"%02x",&tempHex);	

		buff[i] = (uint8)tempHex;
	}
}

int mem_mov(void* pDst, uint16 dstLen, void* pSrc, uint16 srcLen, int movLen)
{
	uint8* pByte = (uint8*)pDst;
	uint16 remainLen = srcLen;
	
	movLen = MIN(movLen, srcLen);
	movLen = MIN(movLen, dstLen);
	
	memcpy(pByte, pSrc, movLen);

	remainLen -= movLen;

	pByte = (uint8*)pSrc;
	if(remainLen)
	{
		memcpy(pSrc, &pByte[movLen], remainLen);
	}
	memset(&pByte[remainLen], 0, srcLen - remainLen);

	return movLen;
}

int str_htoi(const char *s)
{
	int n = 0;

	if( !s ) return 0;

	if( *s == '0' )
	{
		s++;
		if( *s == 'x' || *s == 'X' )s++;
	}

	while(*s != '\0')
	{
		if( *s <= '9' && *s >= '0')
		{
			n <<= 4;
			n |= ( *s & 0xf );
		}
		else if(( *s <= 'F' && *s >= 'A') || ( *s <= 'f' && *s >= 'a'))
		{
			n <<= 4;
			n |= ( (*s & 0xf) + 9 );
		}
		else 
			break;
		s++;
	}
	return n;
}

char* strcpyEx(char* pSrc, const char* pDst, const char* startStr, const char* endStr)
{
	const char* p = pDst; 
	const char* p1 = Null;

	if(startStr) 
	{
		p = strstrex(pDst, startStr);
		if(p == Null) 
			return Null;
	}

	if(endStr)
	{
		p1 = strstr(p, endStr);
		if(p1 == Null)
			return Null;
		
		memcpy(pSrc, p, p1-p);
		pSrc[p1-p] = 0;
	}
	else
	{
		strcpy(pSrc, p);
	}
	
	
	return 	pSrc;
}

uint8* bytesSearch(const uint8* pSrc, int len, const char* pDst)
{
	int i = 0;
	int dstLen = (int)strlen(pDst);
	
	for(i = 0; i < len - dstLen; i++, pSrc++)
	{
		if(memcmp(pSrc, pDst, dstLen) == 0) return (uint8*)pSrc;
	}
	
	return Null;
}

uint8* bytesSearchEx(const uint8* pSrc, int len, const char* pDst)
{
	int i = 0;
	int dstLen = (int)strlen(pDst);
	
	for(i = 0; i < len - dstLen; i++, pSrc++)
	{
		if(memcmp(pSrc, pDst, dstLen) == 0) return (uint8*)(pSrc + dstLen);
	}
	
	return Null;
}

/****************************************************
功能：查找字符串，找到后会把指针移到关键字的后面第一个字符
*****************************************************/
char* strstrex(const char* pSrc, const char* pDst)
{
	char* p = strstr(pSrc, pDst);
	
	if(p)
	{
		p += strlen(pDst);
	}

	return p;
}

//功能: 找到目标字符串之后，用空格替换它，使之下次找不到(仅处理一次)。
char* strstr_remove(char* pSrc, const char* pDst, char**pValue)
{
	char* p = strstr(pSrc, pDst);
	
	if(p)	//找到该字符串之后,
	{
		*pValue =p+strlen(pDst);
		memset(p, ' ', strlen(pDst));

	}
	
	return p;
}

/*
功能:把一个字符串根据separator分割成为多个字符串。和strtok不同的是，每一个分隔符都会被分割成为字符串。
参数说明:
	strDst		: 目标字符串。
	separator	: 分割符,可以为多个字符。
	startInd	: 开始索引
	strArray[]	: 输出参数，分割结果字符串数组。
	count		: 字符串数组元素个数。
返回值:实际分割的字符串数组个数。
*/	
int str_tok(char* strDst, const char* separator, int startInd, char* strArray[], int count)
{
	int i = 0;
	int index = 0;
	char* p = strDst;
	char* pRet = p;

	while(*p)
	{
		const char* pByte = Null;
		for(pByte = separator; *pByte != 0; pByte++)
		{
			if(*p == *pByte)
			{
				if(index++ >= startInd)
				{
					*p = 0;
					strArray[i++] = pRet;
					if(i >= count) return i;
				}
				pRet = p+1;
				break;
			}
		}
		p++;
	}

	if(strlen(pRet) && i < count && index >= startInd)
	{
		strArray[i++] = pRet;
	}

	return i;

}

int strtokenValueByIndex(char* strDst, const char* separator, int index)
{
	int value = 0;
	char* p = Null;
	
	str_tok(strDst, separator, index, &p, 1);

	if(p && strlen(p))
	{
		sscanf(p, "%d",&value);
	}
	
	return value;
}


Bool Stream_IsIncludeTag(uint8* tag, int len, uint8 byte)
{
	int i = 0;
	static uint8 g_state = 0;
	
	for(i = 0; i < len; i++)
	{
		if(i == g_state)
		{
			if(byte == tag[g_state])
				g_state++;
			else
				g_state = 0;
			break;
		}
	}

	if(g_state == len)
	{
		g_state = 0;
		return True;
	}
	
	return False;
}

int IsPktEnd(uint8 byte, char tag, uint8* buf, int* ind, int buflen)
{
	buf[(*ind)++] = byte;
	return (byte == tag || *ind == buflen) ? *ind : 0;
}

/****************************************************
功能：把一个字符串，按照关键字隔开成为若干个字符串。
例如："val1|val2|val3" => list[0] = "val1", list[1] = "val2", list[3] = "val3"
*****************************************************/
int StrToVSList(char* str, char tok, Value_Str vs[])
{
	int len = strlen(str);
	int j = 0;
	int ind = 0;
	char* p = str;

	vs[ind].str = p;
	vs[ind].ind = j++;
	ind++;

	for (int i = 0; i < len; i++)
	{
		if (p[i] == tok)
		{
			p[i] = 0;
			vs[ind].str = &p[++i];
			vs[ind].ind = j++;
			ind++;
		}
	}

	return ind;
}
