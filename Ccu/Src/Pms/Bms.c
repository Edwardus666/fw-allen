#include "Common.h"
#include "Bms.h"

void Bms_Stop(Bms* pBms)
{
}

void Bms_Start(Bms* pBms)
{	
}

void Bms_Run(Bms* pBms)
{
}

SDO_ERR Bms_OnEntryChanged(Bms* pBms, EntryUpdateSrc updateSrc
	, uint16 index, uint8 subIndex
	, uint8* oldValue, uint8* newValue, int len)
{
	PFL(DL_MAIN, "OD[%04X.%02X] =", index, subIndex);DUMP_BYTE(newValue, len);
	return CO_SDO_AB_NONE;
}

CO_SDO_abortCode_t Bms_OnChanged_Common (CO_ODF_arg_t* p)
{
	Printf("PMS OD[%04X.%02X]=", p->index, p->subIndex); DUMP_BYTE(p->data, p->dataLength);
	return CO_SDO_AB_NONE;
}

void Bms_OdExtenInit(Bms* pBms, uint8 addr)
{
}

void Bms_Init(Bms* pBms)
{
	CO_OD_entryRecord_t* pRec = pBms->OD_record2026;
	memset(pBms, 0, sizeof(Bms));

	RECORD_INIT(pBms->OD_record2026, pBms->OD_2026_max_subindex);
	OdRecord_Init(pRec++, &pBms->active_detected_fault, 0x76, 1);
	OdRecord_Init(pRec++, &pBms->detected_fault, 0x76, 4);

	FwFileRecord* p = pBms->fwFile;
	for (int i = 0; i < MAX_BMS_FWFILE_COUNT; i++, p++)
	{
		RECORD_INIT(p->OD_record, p->OD_max_subindex);
		OdRecord_Init(pRec++, &p->version.m_HwMainVer, 0x76, 2);
		OdRecord_Init(pRec++, &p->version.m_AppMainVer, 0x76, 7);
		OdRecord_Init(pRec++, Null, 0x0F, 0);
	}
}
