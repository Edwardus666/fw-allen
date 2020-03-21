
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
	{ {0x00, 0x00, 0x01, 0x00}, 4, "XXXXXX.XX", "Total kWH"},	//�й��ܵ��ܣ���������4�ֽڣ���λkWH
	{ {0x00, 0x00, 0x03, 0x02}, 3, "XX.XXXX"  , "Now KW"},		//˲ʱ���й����ʣ�3�ֽڣ���λkW 
	{ {0x00, 0x01, 0x01, 0x02}, 2, "XXX.X"    , "A-Voltage"},	//A���ѹ       ��2�ֽڣ���λV  
	{ {0x00, 0x01, 0x02, 0x02}, 3, "XXX.XXX"  , "A-Current"},	//A�����       ��3�ֽڣ���λA  
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
	�������ܣ����ֽ������ÿ��Ԫ��ֵ������BCD�룩ת��Ϊ�����ַ���������0x78 0x56 0x34 0x12�� XXXXXX.XX => 123456.78
	����˵����
		pData���ֽ����顣
		len  : �ֽ����鳤�ȣ����Ϊ4.
		fmt  : ת���ĸ�ʽ���룬���� XXXX.XX ��ʾ4λ������2λС����
		* asUint32 �� ת�����-����ֵ�� ������12345678
		* div      �� ת�����-��������	������100
	����ֵ�� �����ַ����� �������أ���123456.78����
**************************************************/
int8* WhMeter_XXXtoVal(uint8* pData, int len, const char* fmt, unsigned int* asUint32, int* div)
{
	int strInd = 0;
	static int8 strVal[10];

	int dotInd = 0xFF;
	uint8 dst[2];		//����0x1234=��0x01,0x02,0x03,0x04

	if (len > 4) return 0;
	if (asUint32)* asUint32 = 0;

	memset(dst, 0, sizeof(dst));
	memset(strVal, 0, sizeof(strVal));

	//����λ��
	for (int i = 0; fmt[i]; i++)
	{
		if (fmt[i] == '.')
		{
			dotInd = i;
			break;
		}
	}
	//���len��fmt�Ƿ�ƥ��
	if (len * 2 != strlen(fmt) - (dotInd != 0xFF)) return 0;

	//��������λ�������㱻����
	if (div) * div = (dotInd == 0xFF) ? 1 : pow(10, len * 2 - dotInd);

	//ת������Ϊ�ַ���
	for (int i = len; i > 0; i--)
	{
		dst[0] = (pData[i - 1] >> 4) & 0x0F;
		dst[1] = pData[i - 1] & 0x0F;

		//��������ֵ��������С����
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
	//������һ������
	g_pMpReadCmdArg++;
	if (g_pMpReadCmdArg->len == 0)	//�Ƿ����һ����¼
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

	//�жϽ���֡�Ƿ�ʱ,�ֽ�֮��ͣ��ʱ��500MS
	if (tickMs)
	{
		if (SwTimer_isTimerOutEx(tickMs, 500))
		{
			pByte = SafeBuf_GetData(pRxBuf, &offset);
			if (offset)
			{
				//����֡��ʱ
				DUMP_BYTE_EX_LEVEL(DL_METER, ("Meter rx timeout:"), pByte, offset);
			}
			goto RX_FRAME_RESET;
		}
	}
	else if (SafeBuf_GetCount(pRxBuf) > 0)
	{
		//������ʱ��
		tickMs = GET_TICKS();
	}
	
	pByte = SafeBuf_SearchByte(pRxBuf, METER_FRAME_TAIL, &offset);
	if (pByte)
	{
		//�յ������ݿ��ܴ���ǰ���ֽڣ�FE FE FE FE 68 91 46 01 15 07 19 68 91 08 33 33 34 33 37 3A 33 33 1A 16 
		//����֡ͷ��ǰ���ֽ�
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

//WhMeter ����
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
		//�������״̬�£�ÿ2����һ�µ���Ƿ�����
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

	//ÿ��10�뷢��һ������
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
	//���ĵ�ַ����Ϊֻ��һ��������ʹ��ͨ���0xAA��ʼ��
	static const uint8 addr[MP_ADDR_SIZE] = {0xAA, 0xAA ,0xAA ,0xAA ,0xAA ,0xAA };
	static Obj sbwObj;
	CO_OD_extension_t odExtension[METER_OD_SIZE];	//��OD��Master������дʱ�����ڲ���������

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
