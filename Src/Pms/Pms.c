#include "Common.h"
#include "Pms.h"
//#include "Pdo.h"
#include "MasterNode.h"
#include "PmsOd.h"

#ifdef CFG_EX_FRAME
//#include "CANopen.h"
#include "UnitOd.h"
#endif

#define SMOKE_RAW_TO_VOLTAGE(_val) (int)((_val) * 3000 * 1.67 / 4096)
void Pms_DumpTestData(Pms* pPms)
{
	MasterNode* pNode = (MasterNode*)pPms;

	if (pPms->dischargeMosTemp == Null)  pPms->dischargeMosTemp = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x20, Null);
	if (pPms->volMeterTemp == Null)      pPms->volMeterTemp  = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x21, Null);
	if (pPms->connectorTemp == Null)     pPms->connectorTemp = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x22, Null);
	if (pPms->batTemp1 == Null)     pPms->batTemp1 = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x23, Null);
	if (pPms->batTemp2 == Null)     pPms->batTemp2 = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x24, Null);

	if (pPms->batHighestVolReg == Null)     pPms->batHighestVolReg = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x0D, Null);
	if (pPms->batHighestVolNo == Null)     pPms->batHighestVolNo = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x0E, Null);
	if (pPms->batLowestVolReg == Null)     pPms->batLowestVolReg = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x0F, Null);
	if (pPms->batLowestVolNo == Null)     pPms->batLowestVolNo = OD_GetValue(pNode->pPrivateOd, PMS_OD_SIZE, 0x3001, 0x10, Null);
	
#define TEMP_CONVERT(_val) (SWAP16(_val) - 400)
	PFL(DL_TEST, "PMS[%02X]: MOS=[% 4d] Voltameter=[% 4d], Connector=[% 4d], Bat=[% 4d]-[% 4d], "
		, pPms->base.addr
		, TEMP_CONVERT(*pPms->dischargeMosTemp)
		, TEMP_CONVERT(*pPms->volMeterTemp)
		, TEMP_CONVERT(*pPms->connectorTemp)
		, TEMP_CONVERT(*pPms->batTemp1)
		, TEMP_CONVERT(*pPms->batTemp2)
	);
	int OD_smoke_sensor1_value = SMOKE_RAW_TO_VOLTAGE(pPms->OD_smoke_sensor1_value);
	int OD_smoke_sensor2_value = SMOKE_RAW_TO_VOLTAGE(pPms->OD_smoke_sensor2_value);
	uint16 val[4];
	val[0] = *pPms->batHighestVolReg;
	val[1] = *pPms->batHighestVolNo;
	val[2] = *pPms->batLowestVolReg;
	val[3] = *pPms->batLowestVolNo;
	PFL(DL_TEST, "Smoking=[% 5d->% 5d]-[% 5d->% 5d]mV, UnitVoltage=[% 6d,% 6d,% 6d,% 6d]\n"
		, OD_smoke_sensor1_value, OD_smoke_sensor1_value - pPms->smoke1Value
		, OD_smoke_sensor2_value, OD_smoke_sensor2_value - pPms->smoke2Value
		, SWAP16(val[0])
		, SWAP16(val[1])
		, SWAP16(val[2])
		, SWAP16(val[3])
	);
	pPms->smoke1Value = OD_smoke_sensor1_value;
	pPms->smoke2Value = OD_smoke_sensor2_value;
}

void Pms_Dump(Pms* pPms)
{
	char buff[128];
	MasterNode_Dump((MasterNode*)pPms);
	Printf("\t Hw(%s); FW(%s); SN", pPms->manufacturerHardwareVersion, pPms->manufacturerSoftwareVersion);
	DUMP_BYTE(pPms->OD_PMS_SN, sizeof(pPms->OD_PMS_SN));
	Printf("\t Hw(%s); FW(%s)\n", pPms->manufacturerHardwareVersion, pPms->manufacturerSoftwareVersion);

	SprintfBit(buff, "funPower[%6B],doorLockState[%5B],bolt[%3-4B],doorLock[%2B],chargerIn[%1B],batIn[%0B]", pPms->OD_pms_status1, Null);
	Printf("\t status1=%02X (%s)\n", pPms->OD_pms_status1, buff);

	SprintfBit(buff, "B-Verify[%5B],P-Verify[%4B],B-Power[%3B],R-Power[%2B],chargerPower[%1B],chargeEnable[%0B]", pPms->OD_pms_status2, Null);
	Printf("\t status2=%02X (%s)\n", pPms->OD_pms_status2, buff);

	Printf("\t charging max current=%d, swapping_soc=%d, max vol=%d\n", pPms->OD_charging_current_max, pPms->OD_swapping_soc, pPms->OD_charging_voltage_max);
	Printf("Smoking=[% 5dmV]-[% 5dmV], Ntc=[% 2d], WaterLevel=[%d]-[%d]\n"
		, SMOKE_RAW_TO_VOLTAGE(pPms->OD_smoke_sensor1_value)
		, SMOKE_RAW_TO_VOLTAGE(pPms->OD_smoke_sensor2_value)
		, pPms->OD_cabinet_ntc_temperature - 40
		, g_pWaterIn->value
		, g_pBucket->value
	);
}

void RPdo1_Done(Pms* pPms, RPdo* pRPdo)
{
	//这两个命令清0
	RPdo_CopySentData(pRPdo);
}

void RPdo2_Done(Pms* pPms, RPdo* pRPdo)
{
	//这两个命令清0
	PmsCmd* localCmdSet = (PmsCmd*)& pPms->OD_pms_cmd_set;
	PmsCmd* localCmdClean = (PmsCmd*)& pPms->OD_pms_cmd_clear;
	PmsCmd* sentCmdSet = (PmsCmd*)& pRPdo->sentData[0];
	PmsCmd* sentCmdClean = (PmsCmd*)& pRPdo->sentData[4];

	//比较发送的数据和本地数据，
	//	如果发现非0且相等，说明该数据已经发送到PMS，清除本地的发送标志；
	//	如果发现非零不相等，说明本地数据再发送的过程中又有新的改变，不能清除本地的发送标志，留待下次继续发送。
#define CHECK_CMD_FIELD(_bitField) \
	if (localCmdSet->_bitField   == sentCmdSet->_bitField   && sentCmdSet->_bitField != 0)   {Printf("Set:%s=%d\n",#_bitField, sentCmdSet->_bitField); localCmdSet->_bitField = 0  ; sentCmdSet->_bitField = 0; }	\
	if (localCmdClean->_bitField == sentCmdClean->_bitField && sentCmdClean->_bitField != 0) {Printf("Clean:%s=%d\n",#_bitField, sentCmdSet->_bitField); localCmdClean->_bitField = 0; sentCmdClean->_bitField = 0; }

	CHECK_CMD_FIELD(unlock);
	CHECK_CMD_FIELD(chargeEnable);
	CHECK_CMD_FIELD(chargeEnableByTemp);
	CHECK_CMD_FIELD(_5VPowerOn);
	CHECK_CMD_FIELD(_12VPowerOn);
	CHECK_CMD_FIELD(batVerifyEnable);
	CHECK_CMD_FIELD(batCapTestEnable);
	CHECK_CMD_FIELD(warmingEnable);
	CHECK_CMD_FIELD(poweredMode);
	CHECK_CMD_FIELD(dischargeEnable);
	CHECK_CMD_FIELD(passthroughRspReady);
}


//判断电池是否在位
Bool Pms_IsBatIn(const Pms* pPms)
{
	const PmsState1* p =(PmsState1*) &pPms->OD_pms_status1;
	return p->batIsIn;

}

uint16 Pms_GetBmsSoc(const Pms* pPms)
{
	if (!Pms_IsBatIn(pPms)) return 0;

	uint16 soc = pPms->bms.reg_unit[READ_ONLY_REG_OFFSET + 6];
	soc = SWAP16(soc);
	return soc;
}

/***************************************************************
函数功能：判断是否满足放电条件，如果满足，则触发PMS发送放电命令。
参数：
	pPms：操作对象。
	dischargeSocThreshold： SOC阈值，大于等于该值，才允许放电阈
返回值：
	参考DSCHG_RC定义
***************************************************************/
DSCHG_RC Pms_SetDischarge(Pms* pPms, uint8 dischargeSocThreshold)
{
	PmsCmd* localCmdState = (PmsCmd*)& pPms->OD_pms_cmd;
	PmsCmd* localCmdSet = (PmsCmd*)& pPms->OD_pms_cmd_set;
	if (localCmdState->dischargeEnable || localCmdSet->dischargeEnable) return DSCHG_OK;

	//1. 备电仓是否有电池
	if (!Pms_IsBatIn(pPms)) return DSCHG_NO_BATTERY;
	//2. 电池电量是否满足要求。
	if (Pms_GetBmsSoc(pPms) < dischargeSocThreshold) return DSCHG_SOC_LESS;
	//3. 电池是否有其他故障。
	//if (!Pms_CanDischarge(pPms)) return DSCHG_BAT_FAULT;

	//设置电池放电
	PFL(DL_PMS, "PMS[%d] set discharge\n", pPms->base.addr);
	localCmdSet->dischargeEnable = 1;
	return DSCHG_OK;
}

void Pms_StopDischarge(Pms* pPms)
{
	PmsCmd* localCmdClean = (PmsCmd*)& pPms->OD_pms_cmd_clear;
	PFL(DL_PMS, "PMS[%d] stop discharge\n", pPms->base.addr);
	localCmdClean->dischargeEnable = 1;
}

uint32 Pms_GetBmsFault(const Pms* pPms)
{
	return 0;
}

SDO_ERR Pms_OnChanged(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	MasterNode* pNode = (MasterNode*)pPms;
	//如果该值被映射到RPDO，则更新RPDO的sentData值，避免发送多余的RPDO
	uint16 entryNo = 0;
	const CO_OD_entry_t* pEntry = OD_find(pNode->pPrivateOd, PMS_OD_SIZE, pArg->index, &entryNo);
	if (pEntry)
	{
		uint16 attr = Entry_getAttribute(pEntry, pArg->subIndex);
		if (attr & CO_ODA_RPDO_MAPABLE)
		{
			MasterNode_SetRPDOSentData((MasterNode*)pPms, pArg->ODdataStorage, pArg->data, pArg->dataLength);
		}
	}
	return CO_SDO_AB_NONE;
}

SDO_ERR Pms_OnChanged_1017(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	if (eventType != TRANS_DONE) return CO_SDO_AB_NONE;
	PFL(DL_MAIN, "Pms[%d] HBTime=%d ms\n", pPms->base.addr, *((uint16*)pArg->data));

	return CO_SDO_AB_NONE;
}

SDO_ERR Pms_OnChanged_3001(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	if (eventType != TRANS_DONE) return CO_SDO_AB_NONE;
	
	if ((pArg->subIndex >= 0x0D && pArg->subIndex <= 0x10)
		|| (pArg->subIndex >= 0x20 && pArg->subIndex <= 0x24))
	{
		Pms_DumpTestData(pPms);
	}
	return CO_SDO_AB_NONE;
}

SDO_ERR Pms_OnChanged_3010(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	if (eventType != TRANS_DONE) return CO_SDO_AB_NONE;

	LOG_TRACE1(pPms->base.addr, (pArg->subIndex == 1) ? ET_PMS_STATE1 : ET_PMS_STATE2, *pArg->data);
	static DrvIo* pPump = Null;
	if(pPump == Null) pPump = IO_Get(CTRL_PUMP);

	if (pArg->subIndex == 1)
	{
		PmsState1* pNew = (PmsState1*)pArg->data;
		PmsState1* pOld = (PmsState1*)pArg->ODdataStorage;
		if (pNew->batIsIn != pOld->batIsIn)
		{
			if (pNew->batIsIn)
			{
				PmsCmd* ps = (PmsCmd*)&pPms->OD_pms_cmd_set;
				ps->chargeEnable = 1;
			}
			else
			{
				PmsCmd* ps = (PmsCmd*)& pPms->OD_pms_cmd_clear;
				ps->chargeEnable = 1;
			}
		}
		if (pNew->isFirex != pOld->isFirex)
		{
			PortPin_Set(pPump->periph, pPump->pin, pNew->isFirex ? IO_HIGH : IO_LOW);

			if (pNew->isFirex)
			{
				Printf("FIREX start.\n");
			}
			else
			{
				Printf("FIREX stop.\n");
			}
		}
	}

	return CO_SDO_AB_NONE;
}

SDO_ERR Pms_OnChanged_3011(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	if (eventType == TRANS_DONE)
	{
	}
	else if (eventType == TRANS_CHANGE_AFTER)
	{
		if (pArg->subIndex == 1)
		{
			LOG_TRACE1(pPms->base.addr, ET_PMS_301101, *pArg->data);
			//PFL(DL_MAIN, "Pms[%d][%04X.%02X]=%X\n", pPms->base.addr, index, pArg->subIndex, (len == 1) ? (*data) : *(uint16*)data);
		}
	}
	return CO_SDO_AB_NONE;
}

SDO_ERR Pms_OnChanged_3014(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	if (eventType != TRANS_DONE) return CO_SDO_AB_NONE;
	memcpy(pArg->ODdataStorage, pArg->data, pArg->dataLength);
	//if (pArg->subIndex <= 4)
	{
		Pms_DumpTestData(pPms);
	}
	return CO_SDO_AB_NONE;
}

SDO_ERR Pms_OnChanged_301A(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	static OtaFile* pFile = Null;

	if (pArg->subIndex != 3) return CO_SDO_AB_NONE;

	if (pFile == Null || pFile->index != pArg->index)
	{
		pFile = OtaFiles_Find(pArg->index);
	}

	PFL(DL_FW_UPGRADE, "PMS[%d] OD[%X.%X] %s Offset[%d],crc=0x%X \n", pPms->base.addr, pArg->index, pArg->subIndex, pArg->reading ? "read" : "write"
		, pArg->offset, pPms->base.sdoMgr.TransMgr.crc);
	if (eventType == TRANS_DONE)
	{
		uint8 result = pArg->reading << 1 | (sdoErr == CO_SDO_AB_NONE);
		LOG_TRACE1(pPms->base.addr, ET_FW_UPGRADE, ((pArg->index << 16) | pArg->subIndex) << 8 | result);
		if (sdoErr == CO_SDO_AB_NONE)
		{
			PFL(DL_FW_UPGRADE, "success\n");
			if (pArg->reading)
			{
				//memcpy(&buff[pArg->offset], pArg->data, pArg->dataLength);
				//DUMP_BYTE_EX((""), buff, pArg->offset + pArg->dataLength);
			}
		}
		else
		{
			PFL(DL_FW_UPGRADE, "failed[%x]\n", sdoErr);
		}
	}
	else if (eventType == TRANS_BUF_FULL)
	{
		//memcpy(&buff[pArg->offset], pArg->data, pArg->dataLength);
	}
	else if (eventType == TRANS_GET_TOTAL_LEN)
	{
		if (pFile->pInfoSector)
		{
			//pArg->dataLengthTotal = pFile->pInfoSector->binDataLen;
		}
		else return CO_SDO_AB_NO_RESOURCE;
	}
	else if (eventType == TRANS_BUF_EMPTY)
	{
		int pmsCodeSize = pFile->fileLen;
		uint8* fileAddr = (uint8*)pFile->addr;
		pArg->dataLength = MIN(pmsCodeSize - pArg->offset, pArg->dataLength);
		if (pArg->offset + pArg->dataLength == pmsCodeSize)
		{
			pArg->lastSegment = True;
		}
		memcpy(pArg->data, &fileAddr[pArg->offset], pArg->dataLength);
	}
	
	return CO_SDO_AB_NONE;
}

SDO_ERR Pms_OnChanged_3100(Pms* pPms, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* pArg)
{
	MasterNode* pNode = (MasterNode*)pPms;

	if (eventType == TRANS_DONE)
	{
		CO_OD_entry_t* pEntry = Od_GetEntry(pNode->pPrivateOd, pNode->privateOdSize, pArg->index, pArg->subIndex);
		if(pEntry) Entry_Dump(pEntry, pNode->addr, pNode->name);

		if (pArg->subIndex == 1)
		{
			if (pPms->OD_capTest_State == CTS_TEST_ABNORMAL)
			{
				//读取失败原因
				MasterNode_ReadEntry((MasterNode*)pPms, 0x3100, 0x02);
			}
			else if (pPms->OD_capTest_State == CTS_TEST_DONE)
			{
				//读取测试结果
				MasterNode_ReadEntry((MasterNode*)pPms, 0x3100, 0x04);
			}
		}
	}
	else if(eventType == TRANS_CHANGE_AFTER)
	{
		LOG_TRACE2(pPms->base.addr, ET_PMS_3100, pPms->OD_capTest_State, pPms->OD_capTest_CancelReason);
	}
	
	return CO_SDO_AB_NONE;
}

CO_SDO_abortCode_t Pms_RPDO_OnChanged(CO_ODF_arg_t* p)
{
	Pms* pPms = p->object;
	uint8 state = pPms->base.state;

	if (p->index == 0x3011 && (p->subIndex == 1 || p->subIndex == 2))
	{
		uint8 cmd = *p->data;

		//如果不是OPERATION状态，则不能修改该字典项
		if (state != NMT_OPERATION)
		{
			LOG_WARNING2(pPms->base.addr, (p->subIndex == 1) ? ET_SM_CMD_SET : ET_SM_CMD_RESET, cmd, state);
			return CO_SDO_AB_GENERAL;
		}
		else
		{
			LOG_TRACE2(pPms->base.addr, (p->subIndex == 1) ? ET_SM_CMD_SET : ET_SM_CMD_RESET, cmd, state);
		}
	}

	DUMP_BYTE_EX_LEVEL(DL_MAIN, ("PMS[%d] OD[%x.%x]=", pPms->base.addr, p->index, p->subIndex), p->data, p->dataLength);
	return CO_SDO_AB_NONE;
}

void Pms_Stop(Pms* pPms)
{
	MasterNode_Stop((MasterNode*)pPms);
}

void Pms_Start(Pms* pPms)
{
	MasterNode_Start((MasterNode*)pPms);
}

void Pms_Run(Pms* pPms)
{
	if (!((MasterNode*)pPms)->isStart) return;

	MasterNode_Run((MasterNode*)pPms);

	//PmsState1* pmsState1 = (PmsState1*)&pPms->OD_pms_status1;
	//PmsState2* pmsState2 = (PmsState2*)&pPms->OD_pms_status2;
	//PmsCmd* pPmsCmd = (PmsCmd*)&pPms->OD_pms_cmd_set;
	//if (pmsState1->batIsIn && !pmsState2->chargeEnable)
	//{
	//	pPmsCmd->chargeEnable = 1;
	//}
}

void Pms_RecordInit(Pms* pPms)
{
	CO_OD_entryRecord_t* pRec = Null;

	pPms->OD_charging_current_max = 18;
	pPms->OD_swapping_soc = 80;
	pPms->OD_charging_voltage_max = 664;

	RECORD_INIT(pPms->OD_record3010, pPms->OD_3010_max_subindex);
	OdRecord_Init(pRec++, &pPms->OD_pms_status1		, 0x76, 1);
	OdRecord_Init(pRec++, &pPms->OD_pms_status2		, 0x76, 1);
	OdRecord_Init(pRec++, &pPms->OD_pms_status3		, 0x76, 1);

	RECORD_INIT(pPms->OD_record3011, pPms->OD_3011_max_subindex);
	OdRecord_Init(pRec++, &pPms->OD_pms_cmd					, 0x76, 4);
	OdRecord_Init(pRec++, &pPms->OD_pms_cmd_set				, 0x7E, 4);
	OdRecord_Init(pRec++, &pPms->OD_pms_cmd_clear			, 0x7E, 4);
	OdRecord_Init(pRec++, &pPms->OD_charging_current_max	, 0x7E, 2);
	OdRecord_Init(pRec++, &pPms->OD_swapping_soc			, 0x7E, 1);
	OdRecord_Init(pRec++, &pPms->OD_charging_voltage_max	, 0x7E, 2);

	RECORD_INIT(pPms->OD_record3013, pPms->OD_3013_max_subindex);
	OdRecord_Init(pRec++, &pPms->OD_PMS_HW_version, 0x0E, 2);
	OdRecord_Init(pRec++, &pPms->OD_PMS_FW_version, 0x0E, 7);
	OdRecord_Init(pRec++, &pPms->OD_PMS_SN		  , 0x0E, 14);

	RECORD_INIT(pPms->OD_record3014, pPms->OD_3014_max_subindex);
	OdRecord_Init(pRec++, &pPms->OD_heater_ntc_temperature	 , 0x76, 2);
	OdRecord_Init(pRec++, &pPms->OD_cabinet_ntc_temperature	 , 0x76, 2);
	OdRecord_Init(pRec++, &pPms->OD_smoke_sensor1_value		 , 0x76, 2);
	OdRecord_Init(pRec++, &pPms->OD_smoke_sensor2_value		 , 0x76, 2);
	OdRecord_Init(pRec++, &pPms->OD_smoke_senor_total_current	 , 0x76, 2);
	OdRecord_Init(pRec++, &pPms->OD_front_motor_working_current, 0x76, 2);


	RECORD_INIT(pPms->OD_record3018, pPms->OD_3018_max_subindex);
	OdRecord_Init(pRec++, &pPms->OD_pms_active_detected_err_of_bat, 0x76, 1);
	OdRecord_Init(pRec++, &pPms->OD_pms_detected_err_of_bat		, 0x76, 4);
	OdRecord_Init(pRec++, &pPms->OD_pms_other_detected_err		, 0x76, 1);
	OdRecord_Init(pRec++, &pPms->OD_pms_fault					, 0x76, 1);
	OdRecord_Init(pRec++, &pPms->OD_pms_damage					, 0x76, 1);

	FwFileRecord* pFwRec = &pPms->OD_record301A;
	RECORD_INIT(pFwRec->OD_record, pFwRec->OD_max_subindex);
	OdRecord_Init(pRec++, &pFwRec->version.m_HwMainVer, 0x06, 2);
	OdRecord_Init(pRec++, &pFwRec->version.m_AppMainVer, 0x06, 7);
	OdRecord_Init(pRec++, Null, 0x0F, 0);

	pFwRec = &pPms->OD_record3021;
	RECORD_INIT(pFwRec->OD_record, pFwRec->OD_max_subindex);
	OdRecord_Init(pRec++, &pFwRec->version.m_HwMainVer, 0x06, 2);
	OdRecord_Init(pRec++, &pFwRec->version.m_AppMainVer, 0x06, 7);
	OdRecord_Init(pRec++, Null, 0x0F, 0);

	RECORD_INIT(pPms->OD_record3100, pPms->OD_3100_max_subindex);
	OdRecord_Init(pRec++, &pPms->OD_capTest_State		, 0x76, 1);
	OdRecord_Init(pRec++, &pPms->OD_capTest_CancelReason, 0x76, 4);
}
#include "PmsMgr.h"
void Pms_OdExternInit(Pms* pPms, const CO_OD_entry_t* pPrivateOd)
{
	struct odExternCfg
	{
		uint16 index;
		ODExtenFunc func;
		EntryEventFn event;
		//uint8* flags;
		//uint8 flagsSize;
	} 
	const static odExten[] = 
	{
		{0x1017, Null, (EntryEventFn)Pms_OnChanged_1017},
		{0x3001, Null, (EntryEventFn)Pms_OnChanged_3001},
		{0x3010, Null, (EntryEventFn)Pms_OnChanged_3010},
		{0x3011, Null, (EntryEventFn)Pms_OnChanged_3011},
		{0x3014, Null, (EntryEventFn)Pms_OnChanged_3014},
		{0x301A, Null, (EntryEventFn)Pms_OnChanged_301A},
		{0x3100, Null, (EntryEventFn)Pms_OnChanged_3100},
	};

	const CO_OD_entry_t* pEntry = Null;
	const struct odExternCfg* pOdEx = odExten;
	uint16 indexOfArray = 0;
	for (int i = 0; i < GET_ELEMENT_COUNT(odExten); i++, pOdEx++)
	{
		pEntry = OD_find(pPrivateOd, PMS_OD_SIZE, pOdEx->index, &indexOfArray);
		Assert(pEntry);
		if (pEntry)
		{
			CO_OD_extension_t* ext = &pPms->odExtension[indexOfArray];
			ext->pODFunc = pOdEx->func;
			ext->object = pPms;
			ext->flags = NULL;

			pPms->entryExt[indexOfArray].OnEvent = pOdEx->event;
		}
	}
}

void Pms_Init(Pms* pPms, uint8 addr)
{
	MasterNode* pMasterNode = (MasterNode*)pPms;
	Charger* pCharger = &pPms->charger;
	Bms* pBms = &pPms->bms;
	const CO_OD_entry_t* pEntry = &g_Pms_Od[addr - PMS_ADDR_0][0];
//	CO_OD_extension_t* pOdExten = pPms->odExtension;
//	EntryExt* pEntryExt = pPms->entryExt;

	//PDO帧类型和Index映射表
	const static PdoTypeMapInd ptMapInd[] =
	{
		{0x1A00, PDO1tx},
		{0x1A01, PDO2tx},

		{0x1A02, PDO3tx},
		{0x1A03, PDO4tx},
		{0x1A04, PDO5tx},

		{0x1A05, PDO6tx},
		{0x1A06, PDO7tx},
		{0x1A07, PDO8tx},

		{0x1A08, PDO9tx},
		{0x1A09, PDO10tx},
		{0x1A0A, PDO11tx},

		{0x1A0B, PDO12tx},
	};

	memset(pPms, 0, sizeof(Pms));

	Pms_RecordInit(pPms);
	Bms_Init(pBms);
	Charger_Init(pCharger);

	//初始化之前必须调用该函数，清除计数器
	Pms_OdExternInit(pPms, pEntry);

	Assert(sizeof(g_Pms_Od)/sizeof(CO_OD_entry_t) == PMS_OD_SIZE * 15);
	/*********************************OD extension 初始化*****************************************************/
	MasterNode_Init(pMasterNode, addr, "Pms", g_PmsOd
		, pEntry, PMS_OD_SIZE
		, pPms->odExtension
		, ptMapInd
		, pPms->entryExt
	);

	pMasterNode->GetFrameType = FrameEx_GetType;
	pMasterNode->Dump = (NodeFn)Pms_Dump;

#ifdef CFG_EX_FRAME
	UnitOd_Init(addr, pMasterNode->pPrivateOd, PMS_OD_SIZE, pPms->odExtension,
		(CO_RPDOMapPar_t*)& PMS_CO_OD_ROM.RPDOMappingParameter, 4,
		(CO_TPDOMapPar_t*)& PMS_CO_OD_ROM.TPDOMappingParameter, 12,
		(CO_TPDOCommPar_t*)&PMS_CO_OD_ROM.TPDOCommunicationParameter
	);
#endif

#ifdef CFG_RPDO
	const static PdoCfg rPdoCfg[] =
	{
		{PDO1rx, 0x1400, 0x1600, (RPdoDone)RPdo1_Done, (CO_TPDOCommPar_t*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[0], (CO_TPDOMapPar_t*)&PMS_CO_OD_ROM.RPDOMappingParameter[0]},
		{PDO2rx, 0x1401, 0x1601, (RPdoDone)RPdo2_Done, (CO_TPDOCommPar_t*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[1], (CO_TPDOMapPar_t*)&PMS_CO_OD_ROM.RPDOMappingParameter[1]},
		//{PDO3rx, 0x1402, 0x1602, Null				 , (CO_TPDOCommPar_t*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[2], (CO_TPDOMapPar_t*)&PMS_CO_OD_ROM.RPDOMappingParameter[2]},
		//{PDO4rx, 0x1403, 0x1603, Null				 , (CO_TPDOCommPar_t*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[3], (CO_TPDOMapPar_t*)&PMS_CO_OD_ROM.RPDOMappingParameter[3]},
	};
	if (addr == PMS_ADDR_7)
	{
		addr = PMS_ADDR_7;
	}
	for (int i = 0; i < GET_ELEMENT_COUNT(pMasterNode->rpdo) && i < GET_ELEMENT_COUNT(rPdoCfg); i++)
	{
		CanTxMsg_Init(&pMasterNode->pdoCanTxMsg[i], pMasterNode, rPdoCfg[i].pdoType, addr);
		RPdo_Init(&pMasterNode->rpdo[i], &rPdoCfg[i], pMasterNode->pPrivateOd, pMasterNode->privateOdSize);
	}
#endif
}
