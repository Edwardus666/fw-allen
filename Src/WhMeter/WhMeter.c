
#include "Common.h"
#include "WhMeter.h"
#include "SbwMgr.h"
#include "MeterProtocol.h"
#include "MeterOd.h"
#include "UnitOD.h"

WhMeter* g_pWhMeter;
Uart* g_pMeterUart = Null;
MeterProtocol* g_pMeterProtocol;

static MpReadCmdArg* g_pMpReadCmdArg = Null;
static SwTimer g_MeterTimer;
//static uint8 verCmd[] = { 0x00, 0x00 };

static MpReadCmdArg g_mpReadCmdArg[] =
{
	{ {0x00, 0x00, 0x01, 0x00}, 4, "XXXXXX.XX", "Total kWH"},	//有功总电能，电表读数，4字节，单位kWH
	{ {0x00, 0x00, 0x03, 0x02}, 3, "XX.XXXX"  , "Now KW"},		//瞬时总有功功率，3字节，单位kW 
	{ {0x00, 0x01, 0x01, 0x02}, 2, "XXX.X"    , "A-Voltage"},	//A相电压       ，2字节，单位V  
	{ {0x00, 0x01, 0x02, 0x02}, 3, "XXX.XXX"  , "A-Current"},	//A相电流       ，3字节，单位A  
	{ 0 }
};

#define METER_FRAME_HEAD	0x68
#define METER_FRAME_TAIL	0x16

#define METER_CMD_READ		0x11

static UtpCmd g_UtpCmds[] =
{
	{METER_CMD_READ, "ReadCmd", (uint8*)&g_mpReadCmdArg[0].di, 4, g_mpReadCmdArg[0].val, 0, (UtpEventFn)WhMeter_ReadCmd_RspProc}	//
};

/**************************************************
	函数功能：把字节数组的每个元素值（类似BCD码）转换为数字字符串，例如0x78 0x56 0x34 0x12， XXXXXX.XX => 123456.78
	参数说明：
		pData：字节数组。
		len  : 字节数组长度，最大为4.
		fmt  : 转换的格式掩码，例如 XXXX.XX 表示4位整数，2位小数。
		* asUint32 ： 转换结果-整数值。 上例：12345678
		* div      ： 转换结果-被除数。	上例：100
	返回值： 数字字符串， 上例返回：“123456.78”。
**************************************************/
int8* WhMeter_XXXtoVal(uint8* pData, int len, const char* fmt, unsigned int* asUint32, int* div)
{
	int strInd = 0;
	static int8 strVal[10];

	int dotInd = 0xFF;
	uint8 dst[2];		//保存0x1234=》0x01,0x02,0x03,0x04

	if (len > 4) return 0;
	if (asUint32)* asUint32 = 0;

	memset(dst, 0, sizeof(dst));
	memset(strVal, 0, sizeof(strVal));

	//整数位数
	for (int i = 0; fmt[i]; i++)
	{
		if (fmt[i] == '.')
		{
			dotInd = i;
			break;
		}
	}
	//检查len和fmt是否匹配
	if (len * 2 != strlen(fmt) - (dotInd != 0xFF)) return 0;

	//根据整数位数，计算被除数
	if (div) * div = (dotInd == 0xFF) ? 1 : pow(10, len * 2 - dotInd);

	//转换数字为字符串
	for (int i = len; i > 0; i--)
	{
		dst[0] = (pData[i - 1] >> 4) & 0x0F;
		dst[1] = pData[i - 1] & 0x0F;

		//计算整数值，不包含小数点
		if (asUint32)
		{
			*asUint32 += dst[0] * pow(10, (i - 1) * 2 + 1);
			*asUint32 += dst[0 + 1] * pow(10, (i - 1) * 2);
		}

		strVal[strInd++] = dst[0] + 0x30;
		if (strInd == dotInd) strVal[strInd++] = '.';
		strVal[strInd++] = dst[0 + 1] + 0x30;
		if (strInd == dotInd) strVal[strInd++] = '.';
	}

	return strVal;
}

int WhMeter_Uart_Tx(const uint8* pData, int len)
{
	DUMP_BYTE_EX_LEVEL(DL_METER, ("MeterTx:"), pData, len);
	Uart_Tx(g_pMeterUart, pData, len);

	//delay_1ms(g_TestFlag);
	//if (enableIo) PortPin_Set(enableIo->periph, enableIo->pin, 0);
	return len;
}

void WhMeter_SwitchState(WhMeter* pNode, Rs485ProtocolNodeState newState)
{
	const static char* stateName[] = { "NODE_INIT", "PRE_OPERATION", "OPERATION", "NODE_STOP" };
	if (pNode->state == newState)
	{
		return;
	}
	PFL(DL_MAIN, "Meter state switch to [%s]\n", stateName[newState]);
	pNode->state = newState;
	LOG_TRACE1(OBJID_METER, ET_METER_OP_STATE_CHANGED, newState);
}

void WhMeter_SendReadCmd(WhMeter* pWhMeter, UtpCmd* pUtpCmd, MpReadCmdArg* pDiVal)
{
	pUtpCmd->pCmdData = g_pMpReadCmdArg->di;
	pUtpCmd->cmdDataLen = 4;
	pUtpCmd->pStorage = g_pMpReadCmdArg->val;
	pUtpCmd->storageLen = g_pMpReadCmdArg->len;

	MeterProtocol_SendCmd(g_pMeterProtocol, pUtpCmd, 4000, 1);
}

UTP_EVENT_RC WhMeter_ReadCmd_RspProc(WhMeter* pWhMeter, UtpCmd* pUtpCmd, UTP_EVENT ev)
{
	UTP_EVENT_RC rc = UTP_RC_SUCCESS;

	if (ev == UTP_TRANSFER_DONE)
	{
		if (pUtpCmd->lastError == UTP_ERROR_OK)
		{
			WhMeter_SwitchState(pWhMeter, UTP_NODE_OPERATION);
		}
		else if (pUtpCmd->lastError == UTP_ERROR_TIMEOUT)
		{
			WhMeter_SwitchState(pWhMeter, UTP_NODE_PRE_OPERATION);
		}
		goto NEXT_CMD;
	}
	else if (ev == UTP_EVENT_CHANGED_AFTER)
	{
		int8* p = WhMeter_XXXtoVal(g_pMpReadCmdArg->val, g_pMpReadCmdArg->len, g_pMpReadCmdArg->fmt, Null, Null);
		if (p)
		{
			PFL(DL_METER, "%s:%s\n", g_pMpReadCmdArg->name, p);
		}
		else
		{
			PFL(DL_METER, "%s invalid. fmt=%s\n", g_pMpReadCmdArg->name, g_pMpReadCmdArg->fmt);
		}
		rc = UTP_RC_SUCCESS;
	}

	return rc;

NEXT_CMD:
	//发送下一个命令
	g_pMpReadCmdArg++;
	if (g_pMpReadCmdArg->len == 0)	//是否最后一条记录
	{
		g_pMpReadCmdArg = g_mpReadCmdArg;
	}
	rc = UTP_RC_SUCCESS;
	return rc;
}

void WhMeter_RxDataProc()
{
	static int offset = 0;
	static uint32 tickMs = 0;

	uint8* pByte = Null;
	SafeBuf* pRxBuf = &g_pMeterUart->rxBuf;

	//判断接收帧是否超时,字节之间停顿时间500MS
	if (tickMs)
	{
		if (SwTimer_isTimerOutEx(tickMs, 500))
		{
			pByte = SafeBuf_GetData(pRxBuf, &offset);
			if (offset)
			{
				//接收帧超时
				DUMP_BYTE_EX_LEVEL(DL_METER, ("Meter rx timeout:"), pByte, offset);
			}
			goto RX_FRAME_RESET;
		}
	}
	else if (SafeBuf_GetCount(pRxBuf) > 0)
	{
		//启动定时器
		tickMs = GET_TICKS();
	}
	
	pByte = SafeBuf_SearchByte(pRxBuf, METER_FRAME_TAIL, &offset);
	if (pByte)
	{
		//收到的数据可能存在前置字节：FE FE FE FE 68 91 46 01 15 07 19 68 91 08 33 33 34 33 37 3A 33 33 1A 16 
		//忽略帧头的前置字节
		for (int i = 0; i < offset; i++, pByte++)
		{
			if (*pByte == 0x68)
			{
				offset -= i;
				SafeBuf_Read(pRxBuf, Null, offset);
				break;
			}
		}

		DUMP_BYTE_EX_LEVEL(DL_METER, ("Meter rx:"), pByte, offset);
		MP_FRAME_RESULT ret = MeterProtocol_VerifyFrame((MeterProtocolFrame*)pByte, offset);
		if (ret == MP_FRAME_OK)
		{
			MeterProtocol_RcvFrameHandler(g_pMeterProtocol, (MeterProtocolFrame*)pByte);
			goto RX_FRAME_RESET;
		}
		else if (ret != MP_FRAME_INCOMPLETE)
		{
			goto RX_FRAME_RESET;
		}
	}
	return;

RX_FRAME_RESET:
	SafeBuf_Read(pRxBuf, Null, offset);
	tickMs = 0;
	offset = 0;
}

//WhMeter 启动
void WhMeter_Start()
{
	if (g_pWhMeter->state == SBW_PRE_OPERATION) return;

	g_pWhMeter->state = UTP_NODE_PRE_OPERATION;
	Uart_Start(g_pMeterUart);
}

void WhMeter_RunWhenProtocolIdle()
{
	SwTimer* pTimer = &g_MeterTimer;

	if (g_pWhMeter->state == UTP_NODE_PRE_OPERATION)
	{
		//在与操作状态下，每2秒检测一下电表是否上线
		g_pMpReadCmdArg = g_mpReadCmdArg;
		if (!pTimer->m_isStart)
		{
			SwTimer_Start(pTimer, 2000, 0);
		}
	}
	else if (g_pWhMeter->state == UTP_NODE_OPERATION)
	{
		if (!pTimer->m_isStart)
		{
			SwTimer_Start(pTimer, 10000, 0);
		}
	}

	//每个10秒发送一个命令
	if (SwTimer_isTimerOut(pTimer))
	{
		WhMeter_SendReadCmd(g_pWhMeter, g_UtpCmds, g_pMpReadCmdArg);
	}
}

void WhMeter_Run()
{
	if (g_pWhMeter->state == UTP_NODE_INIT) return;

	WhMeter_RxDataProc();
	MeterProtocol_Run(g_pMeterProtocol);
	if (!MeterProtocol_isBusy(g_pMeterProtocol))
	{
		WhMeter_RunWhenProtocolIdle();
	}

}

WhMeter g_WhMeter;
void WhMeter_Init()
{
	//电表的地址，因为只有一个，所以使用通配符0xAA初始化
	static const uint8 addr[MP_ADDR_SIZE] = {0xAA, 0xAA ,0xAA ,0xAA ,0xAA ,0xAA };
	static Obj sbwObj;
	CO_OD_extension_t odExtension[METER_OD_SIZE];	//当OD被Master读或者写时触发内部函数调用

	Obj_Register(&sbwObj, "Meter", WhMeter_Start, Null, WhMeter_Run);

	g_pMpReadCmdArg = g_mpReadCmdArg;
	g_pWhMeter = &g_WhMeter;
	g_pMeterProtocol = &g_pWhMeter->meterProtocol;
	memset(g_pWhMeter, 0, sizeof(WhMeter));

	g_pMeterUart = Uart_InitEx(CCU_METER_COM, 2400, USART_PM_EVEN);
	Assert(g_pMeterUart);
	MeterProtocol_Init(g_pMeterProtocol, addr, WhMeter_Uart_Tx, g_pWhMeter);

	UnitOd_Init(METER_ADDR, g_MeterOd, METER_OD_SIZE, odExtension,
		 Null, 0,
		(CO_TPDOMapPar_t*)& METER_CO_OD_ROM.TPDOMappingParameter, 2,
		(CO_TPDOCommPar_t*)& BASE_CO_OD_ROM.TPDOCommunicationParameter
	);
}
