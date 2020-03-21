
#ifndef  _XDEBUG_H_
#define  _XDEBUG_H_

#ifdef __cplusplus
extern "C"{
#endif	//#ifdef __cplusplus

#include "typedef.h"
extern uint32 g_dwDebugLevel;
	
#ifdef XDEBUG
	void Debug_Init(void);
	void ShowWait(int printDotPerCount, int dotCountPerRow);
	void Debug_SetBit(uint32 bitInd, Bool isEnable);
	void Debug_SetLevel(uint8 level);
	extern int _Printf(const char* lpszFormat, ...);

	#define DL_MAIN 		BIT_0
	#define DL_ERROR		BIT_1
	#define DL_WARNING		BIT_2
	#define DL_ASSERT		BIT_3

	#define DL_IO			BIT_4
	#define DL_ADC			BIT_5
	#define DL_OD			BIT_6
	#define DL_CAN0_RX		BIT_7
	#define DL_LOG			BIT_8

	#define DL_CAN_MASTER	BIT_10
	#define DL_TPDO 		BIT_11
	#define DL_RPDO 		BIT_12

	#define DL_PMS			BIT_15
	#define DL_CAN_SLAVE_RX			BIT_16
	#define DL_FSM			BIT_17

	#define DL_METER		BIT_20
	#define DL_SHELF_LOCK	BIT_21

	#define DL_MSG			BIT_25
	#define DL_TEST			BIT_26
	#define DL_FW_UPGRADE	BIT_27
	#define DL_SHOW_CALLED	BIT_30
	#define DL_SHOW_TICKS	BIT_31
	#define DL_DEBUG		0xFFFFFFFF

	#define DUG_BIT_0(dl)	(g_dwDebugLevel &= ~dl)
	#define DUG_BIT_1(dl)	(g_dwDebugLevel |= dl)			
	
	#define Printf(...) {printf(__VA_ARGS__); while(RESET == usart_flag_get(CCU_DEBUG_COM, USART_FLAG_TC));}
	//#define Printf(...) {_Printf(__VA_ARGS__); while(RESET == usart_flag_get(CCU_DEBUG_COM, USART_FLAG_TC));}
	#define ASRT(parenExpr) if((g_dwDebugLevel & DL_ASSERT) && !(parenExpr))	\
			{                                   \
				Printf( "Assertion Failed! %s,%s,%s,line=%d\n", #parenExpr,__FILE__,__FUNCTION__,_LINE_);	\
				while(1){;}	\
			}
	#define ASSERT_FALSE(parenExpr) {	\
				Printf parenExpr;	\
				ASRT(FALSE);\
			}
	
	//#define Trace Printf
	#define PFL(level, ...) {if (g_dwDebugLevel & level) {Printf(__VA_ARGS__);}}

	void Debug_DumpByteEx(uint32 level, const uint8* pData, uint16 len, uint8 cols, const char* str, int line) ;
	void Debug_DumpByte(const uint8* pData, uint16 len, uint8 cols, int max);
	void Debug_DumpDword(const uint32* pData, uint16 len, uint8 cols);

	#define DUMP_BYTE(address,len)  Debug_DumpByte((uint8*)address, len, 32, len);
	#define DUMP_BYTE_EX(str,address,len)  {Printf str; Debug_DumpByte((uint8*)address, len, 32, len);}
	#define DUMP_DWORD(address,len) Debug_DumpDword(address, len, 8)

	#define DUMP_BYTE_LEVEL_EX(level, address, len, cols) {\
		if(((uint32)(level)) & g_dwDebugLevel) \
		{	\
    		Debug_DumpByte(address, len, cols, len);	\
		}	\
	}

	#define DUMP_BYTE_LEVEL(level, address, len){\
		if(((uint32)(level)) & g_dwDebugLevel) \
		{	\
    		DUMP_BYTE(address,len);	\
		}	\
	}
	
	#define DUMP_BYTE_EX_LEVEL(level, str, address, len){\
		if(((uint32)(level)) & g_dwDebugLevel) \
		{	\
    		DUMP_BYTE_EX(str, address,len);	\
		}	\
	}
	
	#define DUMP_DWORD_LEVEL(level, address,len){\
		if(((uint32)(level)) & g_dwDebugLevel) \
		{	\
    		DUMP_DWORD(address,len);	\
		}	\
	}

	#define SHELL(parenExpr) Printf parenExpr
	
	#define PFL_WARNING(...) 	PFL(DL_WARNING, "WARNING: %s(),line=%d: ",_FUNC_, _LINE_); PFL(DL_WARNING, __VA_ARGS__);
	#define PFL_ERROR(...) 	PFL(DL_ERROR, "ERROR: %s(),line=%d:",_FUNC_, _LINE_); PFL(DL_ERROR, __VA_ARGS__);
	//#define PFL_ERROR(parenExpr) 	PFL(DL_ERROR, "ERROR: line=%d:",_LINE_); PFL(DL_ERROR, parenExpr);

	#define PFL_FUN_LINE(level) PFL(level, "%s(),line=%d\n",_FUNC_, _LINE_);
	#define PFL_FILE_FUN_LINE(level) PFL(level, ("PFL Line. %s,%s(),line=%d\n", __FILE__,__FUNCTION__,_LINE_))
	#define PFL_VAR(V1) 		Printf("%s(),line=%d,%s=[0x%x](%d)\n",_FUNC_, _LINE_, #V1, V1, V1)
	#define PFL_FAILED() 		PFL(DL_MAIN|DL_WARNING, ("%s() FAILED,line=%d.\n",_FUNC_, _LINE_))
	#define PFL_FAILED_EXPR(parenExpr) 		PFL(DL_WARNING, ("%s() FAILED,line=%d:",_FUNC_, _LINE_)); PFL(DL_WARNING, parenExpr);
	
	#define WAIT(maxMS, parenExpr) {int ms = 0; while(!(parenExpr) && ms++ < (maxMS)) {SLEEP(1);}}

#else	//#ifdef XDEBUG
	#define DUG_BIT_0(...)
	#define DUG_BIT_1(...)			
	#define Debug_SetBit(...)
	#define Debug_SetLevel(...)
	#define Debug_Init(...)
	#define ShowWait(...)
	#define Debug_EnbaleLevel(...)
	#define Printf(...)
	#define ASRT(...)
	#define ASSERT_FALSE(...)
	#define ASSERT_EX(...)
	#define Trace(...)
	#define PFL_WARNING(...)
	#define PFL_ERROR(...)
	
	#define PFL(...)
	#define PFL_FUN_LINE(...)
	#define PFL_FILE_FUN_LINE(...)
	#define PFL_FAILED() (void)0
	#define PFL_FAILED_EXPR(...)

	#define Debug_DumpByteEx(...)
	#define Debug_DumpByte(...)
	#define Debug_DumpDword(...)

	#define DUMP_BYTE(...)
	#define DUMP_DWORD(...)

	#define DUMP_BYTE_EX(...)
	#define DUMP_BYTE_LEVEL(...)
	#define DUMP_DWORD_LEVEL(...)
	#define DUMP_BYTE_LEVEL_EX(...)
	#define Debug_Init(...)

	#define SHELL(...)
	#define PFL_VAR(...)
	#define WAIT(...)
	#define Debug_Set(...)
#endif	//#ifdef XDEBUG

#ifdef __cplusplus
}
#endif	//#ifdef __cplusplus

#endif //#ifndef  _DEBUG_H_


