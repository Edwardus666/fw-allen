#include "Common.h"
#include "Charger.h"

void Charger_Stop(Charger* pCharger)
{
}

void Charger_Start(Charger* pCharger)
{	
}

void Charger_Run(Charger* pCharger)
{
}

SDO_ERR Charger_OnEntryChanged(Charger* pCharger, EntryUpdateSrc updateSrc
	, uint16 index, uint8 subIndex
	, uint8* oldValue, uint8* newValue, int len)
{
	PFL(DL_MAIN, "OD[%04X.%02X] =", index, subIndex);DUMP_BYTE(newValue, len);
	return CO_SDO_AB_NONE;
}

CO_SDO_abortCode_t Charger_OnChanged_Common (CO_ODF_arg_t* p)
{
	Printf("PMS OD[%04X.%02X]=", p->index, p->subIndex); DUMP_BYTE(p->data, p->dataLength);
	return CO_SDO_AB_NONE;
}

void Charger_OdExtenInit(Charger* pCharger, uint8 addr)
{
	MasterNode* pNode = (MasterNode*)pCharger;
	OD_configure(pNode->pPrivateOd, pNode->privateOdSize, pNode->ODExtensions, 0x2120, Charger_OnChanged_Common, Null, 0, 0);
}

void Charger_Init(Charger* pCharger)
{
	CO_OD_entryRecord_t* pRec = Null;
	memset(pCharger, 0, sizeof(Charger));

	RECORD_INIT(pCharger->OD_record2043, pCharger->OD_2043_max_subindex);
	OdRecord_Init(pRec++, &pCharger->charger_online_status, 0x76, 1) ;
	OdRecord_Init(pRec++, &pCharger->charger_output_C[0]	, 0x76, 2) ; 
	OdRecord_Init(pRec++, &pCharger->charger_output_V[0]	, 0x76, 2) ; 
	OdRecord_Init(pRec++, &pCharger->charger_output_P[0]	, 0x76, 2) ; 
	OdRecord_Init(pRec++, &pCharger->charger_status[0]	, 0x76, 4) ; 
	OdRecord_Init(pRec++, &pCharger->charger_sn[0]		, 0x76, 14); 

	RECORD_INIT(pCharger->OD_record20FF, pCharger->OD_20FF_max_subindex);
	OdRecord_Init(pRec++, &pCharger->heater_NTC_temperature	, 0x76, 2) ;
	OdRecord_Init(pRec++, &pCharger->cabinet_NTC_temperature	, 0x76, 2) ;
	OdRecord_Init(pRec++, &pCharger->charger_temperature		, 0x76, 2) ;

	RECORD_INIT(pCharger->OD_record3015, pCharger->OD_3015_max_subindex);
	OdRecord_Init(pRec++, &pCharger->OD_charger_C			 , 0x76, 2);
	OdRecord_Init(pRec++, &pCharger->OD_charger_V			 , 0x76, 2);
	OdRecord_Init(pRec++, &pCharger->OD_charger_P			 , 0x76, 2);
	OdRecord_Init(pRec++, &pCharger->OD_charger_error1	 , 0x76, 1);
	OdRecord_Init(pRec++, &pCharger->OD_charger_error2	 , 0x76, 1);
	OdRecord_Init(pRec++, &pCharger->OD_charger_temperature, 0x76, 1);
	OdRecord_Init(pRec++, &pCharger->OD_charger_reserved	 , 0x76, 1);

	RECORD_INIT(pCharger->OD_record3016, pCharger->OD_3016_max_subindex);
	OdRecord_Init(pRec++, &pCharger->OD_charger_HW_version, 0x76, 1);
	OdRecord_Init(pRec++, &pCharger->OD_charger_FW_version, 0x76, 2);
	OdRecord_Init(pRec++, &pCharger->OD_charger_SN		, 0x76, 7);

	RECORD_INIT(pCharger->OD_record3017, pCharger->OD_3017_max_subindex);
	OdRecord_Init(pRec++, &pCharger->OD_charger_voltage_setting_value, 0x76, 2);
	OdRecord_Init(pRec++, &pCharger->OD_charger_current_setting_value, 0x76, 2);
}
