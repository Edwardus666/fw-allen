#include "Common.h"
#ifdef CONFIG_CMDLINE
#include "CmdLine.h"
#include <stdarg.h>

extern int str_htoi(const char *s);
/*
支持的命令格式如下:
Test(1,2,3,4,5) 最多支持输入5个参数
Test(1,2,0x3,4,"str")
Test 1
Test 1 "str"

支持简写输入

*/

static CmdLine g_CmdLine;

#if 0
#define CmdLine_Strtok strtok
#else
char* CmdLine_Strtok(char* pSrc, const char* delim)
{
	static char* p = 0;
	char* pRet = Null;

	if(pSrc)
	{
		p = pSrc;
	}

	if(*p == 0)
	{
		pRet = Null;
		goto End;
	}

	pRet = p;

	//优先把字符串找出来
	if(*p == '"')	//第一个'"'
	{
		p++;
		while(*p++ != '"')
		{
			if(*p == 0)
			{
				pRet = Null;
				goto End;
			}
		}

		if(*p != 0)
		{
			*p++ = 0;
		}
		goto End;
	}

	while(*p)
	{
		const char* pByte = delim;
		for(pByte = delim; *pByte != 0; pByte++)
		{
			if(*p == *pByte)
			{
				*p++ = 0;
				goto End;
			}
		}
		p++;
	}

End:
	return pRet;
}

#endif

void CmdLine_Help()
{
	int i = 0;
	const FnDef* pFnDef = g_CmdLine.m_FnArray;
	
	for(i = 0; i < g_CmdLine.m_FnCount; i++, pFnDef++)
	{		
		Printf("\t %s\n", pFnDef->m_Title);
	}
}

ArgType CmdLine_GetArgType(const char* argStr)
{
	int nLen = strlen(argStr);

	if(Null == argStr) return ARGT_NONE;

	if(*argStr == '\"')
	{
		if('\"' == argStr[nLen-1])
		{
			return ARGT_STR;
		}
		else
		{
			return ARGT_ERROR;
		}
	}
	else if(argStr[0] == '0' && (argStr[1] == 'x' || argStr[1] == 'X'))
	{
		return ARGT_HEX;
	}
	else
	{
		return ARGT_DEC;
	}
}

Bool CmdLine_Parse(char* cmdLineStr, char** pFnName, char* pArgs[], int* argCount)
{
	int maxArgCount = *argCount;
	char *token;
	char fnNameseps[]   = " (\n";
	char argSeps[]   = ", )\n";

	//Find function name
	token = CmdLine_Strtok(cmdLineStr, fnNameseps);
	if(Null == token) return False;
	*pFnName = token;

	*argCount= 0;
	token = CmdLine_Strtok( NULL, argSeps);
	while( token != NULL )
	{
		pArgs[(*argCount)++] = token;
		if((*argCount) > maxArgCount)
		{
			Printf("PF_ERROR: Arg count is too many\n");
			return False;
		}
		token = CmdLine_Strtok( NULL, argSeps);
	}

	return True;
}

Bool CmdLine_ArgConvert(char* pArgs[], int argCount, uint32 arg[])
{
	int i = 0;
	ArgType at = ARGT_NONE;
	char* pChar = Null;

	for(i = 0; i < argCount; i++)
	{
		at = CmdLine_GetArgType(pArgs[i]);
		if(ARGT_DEC == at)
		{
			arg[i] = atoi(pArgs[i]);
		}
		else if(ARGT_HEX == at)
		{
			arg[i] = str_htoi(pArgs[i]);
		}
		else if(ARGT_STR == at)
		{
			pChar = pArgs[i];
			pChar[strlen(pChar) - 1] = 0;
			pChar++;
			arg[i] = (uint32)pChar;
		}
		else
		{
			Printf("\tArg[%d](%s) error. \n", i+1, pArgs[i]);
			return False;
		}
	}
	return True;
}

void CmdLine_Exe(CmdLine* pCmdLine, const char* pFnName, uint32 arg[], int argCount)
{
	Bool isFind = 0;
	int i = 0;
	const FnDef* pFnEntry = pCmdLine->m_FnArray;
	const FnDef* pFoundEntry = Null;
	
	#define FUN(n, funType, args) if(n == pFoundEntry->m_ArgCount)	\
		{	\
			((funType)pFoundEntry->pFn) args;	\
			return;	\
		}

	for(i = 0; i < pCmdLine->m_FnCount; i++, pFnEntry++)
	{
		if(strcmp(pFnName, "?") == 0)
		{
			CmdLine_Help();
			return;
		}

		//和函数名部分比较
		if(strstr(pFnEntry->m_Title, pFnName) == pFnEntry->m_Title)
		{
			char* str;
			
			isFind++;
			
			if(Null == pFoundEntry)
				pFoundEntry = pFnEntry;

			//查找函数名
			str	= strchr(pFnEntry->m_Title, '(');
			if(Null == str)
				str	= strchr(pFnEntry->m_Title, ' ');
			
			if(Null == str) continue;

			//和函数名完全比较
			if(memcmp(pFnEntry->m_Title, pFnName, str - pFnEntry->m_Title) == 0)
			{
				isFind = 1;
				pFoundEntry = pFnEntry;
				break;
			}
		}
	}

	if(0 == isFind)
	{
		Printf("Unknown: %s\n", pFnName);
		return;
	}
	else if(isFind > 1)
	{
		//如果找出的函数名多于一个，则打印所有的部分比较正确的函数名
		pFnEntry = pCmdLine->m_FnArray;
		for(i = 0; i < pCmdLine->m_FnCount; i++, pFnEntry++)
		{
			if(strstr(pFnEntry->m_Title, pFnName) == pFnEntry->m_Title)
			{
				Printf("%s\n", pFnEntry->m_Title);
			}
		}
		return;
	}

	FUN(0, FnArg0, ());
	FUN(1, FnArg01, (arg[0]));
	FUN(2, FnArg02, (arg[0], arg[1]));
	FUN(3, FnArg03, (arg[0], arg[1], arg[2]));
	FUN(4, FnArg04, (arg[0], arg[1], arg[2], arg[3]));
	FUN(5, FnArg05, (arg[0], arg[1], arg[2], arg[3], arg[4]));
}

int CmdLine_GetArgCount(const char* str)
{
	Bool bFlag = False;
	int nArgCount = 0;
	
	str	= strchr(str, '(');

	if(Null == str)
	{
		return 0;
	}
	
	while(*(++str) != '\0')
	{
		if(')' == *str)
		{
			break;
		}
		else if(!bFlag)
		{
			if(' ' != *str)
			{
				bFlag = True;
				nArgCount++;

				if(',' == *str)
				{
					nArgCount++;
				}
			}
		}
		else if(',' == *str)
		{
			nArgCount++;
		}
	}
	
	return *str == ')' ? nArgCount : -1;
}

void CmdLine_Reset(CmdLine* pCmdLine)
{
	memset(pCmdLine->m_CmdLineStr, 0, sizeof(pCmdLine->m_CmdLineStr));
	pCmdLine->m_CmdLineStrLen = 0;
}

void CmdLine_AddStr(const char* str)
{
	CmdLine_AddStrEx(str, strlen(str));
}

void CmdLine_AddStrEx(const char* str, int len)
{
	int i = 0;
	CmdLine* pCmdLine = &g_CmdLine;
	char* pBuf = pCmdLine->m_CmdLineStr;

	for(i = 0; i < len; i++, str++)
	{
		if(pCmdLine->m_CmdLineStrLen >= MAX_CMDLINE_LEN)
		{
			CmdLine_Reset(pCmdLine);
		}
		
		if(pCmdLine->m_isEcho)
		{
			if (pCmdLine->m_CmdLineStrLen == 0)
			{
				Printf("->");
			}
			Printf("%c", *str);
		}
		
		if(*str != KEY_CR && *str != KEY_LF)
		{
			pBuf[pCmdLine->m_CmdLineStrLen++] = *str;
		}
		if(KEY_CR == *str)// || ')' == *str)
		{
			char* pFnName = Null;
			char* argStr[MAX_ARG_COUNT] = {0};
			int argCount = MAX_ARG_COUNT;
			
			if(('\r' == pBuf[0] && pCmdLine->m_CmdLineStrLen == 1) || 0 == pCmdLine->m_CmdLineStrLen)
			{
				CmdLine_Reset(pCmdLine);
				return;
			}

			if(CmdLine_Parse(pBuf, &pFnName, argStr, &argCount))
			{
				uint32 arg[MAX_ARG_COUNT] = {0};

				if(CmdLine_ArgConvert(argStr, argCount, arg))
				{
					CmdLine_Exe(pCmdLine, pFnName, arg, argCount);
				}
			}
			CmdLine_Reset(pCmdLine);
	   	}
	}
}

void CmdLine_Init(FnDef* pCmdTable, uint8 cmdTableCount, Bool isEcho)
{
	int i = 0;
	FnDef* pFnEntry = pCmdTable;
	
	memset(&g_CmdLine, 0, sizeof(CmdLine));
	
	g_CmdLine.m_isEcho = isEcho;

	g_CmdLine.m_FnArray = pCmdTable;
	g_CmdLine.m_FnCount = cmdTableCount;
	//g_CmdLine.printf = printf;

	for(i = 0; i < cmdTableCount; i++, pFnEntry++)
	{
		int argCount = CmdLine_GetArgCount(pFnEntry->m_Title);
		if(argCount < 0 || argCount > MAX_ARG_COUNT)
		{
			Printf("[%s] error, get arg count[%d] error.\n", pFnEntry->m_Title, pFnEntry->m_ArgCount);
		}
		
		pFnEntry->m_ArgCount = (int8)argCount;
	}
}

#endif

