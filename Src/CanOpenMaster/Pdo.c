#include <common.h>
#include "Pdo.h"
#include "Od.h"
#include "CO_NMT_Heartbeat.h"
#include <Frame.h>

void TPOD_dump(CO_TPDO_t* TPDO)
{
	uint8_t* pPDOdataByte;
	uint8_t** ppODdataByte;

	Printf("sendRequest = %d", TPDO->sendRequest);
	Printf("COSFlags = %X", TPDO->sendIfCOSFlags);
	Printf("Len = %d", TPDO->dataLength);

	pPDOdataByte = &TPDO->CANtxBuff->data[0];
	ppODdataByte = &TPDO->mapPointer[0];

	Printf("TxBuf:");
	for (int i = 0; i < TPDO->dataLength; i++)
	{
		Printf("%02X ", (*pPDOdataByte)++);
	}
	Printf("\n OdBuf:");
	for (int i = 0; i < TPDO->dataLength; i++)
	{
		Printf("%02X ", (**ppODdataByte)++);
	}
}

void RPOD_dump(RPdo* RPDO)
{
	uint8_t* pPDOdataByte;
	uint8_t** ppODdataByte;

	Printf("sendRequest = %d", RPDO->sendRequest);
	Printf("COSFlags = %X", RPDO->sendIfCOSFlags);
	Printf("Len = %d", RPDO->dataLength);

	pPDOdataByte = &RPDO->sentData[0];
	ppODdataByte = &RPDO->mapPointer[0];

	Printf("TxBuf:");
	for (int i = 0; i < RPDO->dataLength; i++)
	{
		Printf("%02X ", (*pPDOdataByte)++);
	}
	Printf("\n OdBuf:");
	for (int i = 0; i < RPDO->dataLength; i++)
	{
		Printf("%02X ", (**ppODdataByte)++);
	}
}

uint32_t PDO_configMap(RPdo* TPDO, CO_OD_entry* pOdArray, int odSize, const CO_TPDOMapPar_t* TPDOMapPar) {
	int16_t i;
	uint8_t length = 0;
	uint32_t ret = 0;
	const uint32_t* pMap = &TPDOMapPar->mappedObject1;

	TPDO->sendIfCOSFlags = 0;

	for (i = 0; i < TPDOMapPar->numberOfMappedObjects; i++) {
		int16_t j;
		uint8_t* pData;
		uint8_t prevLength = length;
		uint8_t MBvar;
		uint32_t map = *(pMap++);

		/* function do much checking of errors in map */
		ret = OD_PDOfindMap(
			pOdArray, odSize,
			map,
			1,
			&pData,
			&length,
			&TPDO->sendIfCOSFlags,
			&MBvar);
		if (ret) {
			length = 0;
			return 0x1AU;	//CO_EM_PDO_WRONG_MAPPING
		}

		/* write PDO data pointers */
		for (j = prevLength; j < length; j++) {
			TPDO->mapPointer[j] = pData++;
		}
	}

	TPDO->dataLength = length;

	return ret;
}

uint8_t RPdo_isCOS(RPdo* TPDO) {

	/* Prepare TPDO data automatically from Object Dictionary variables */
	uint8_t* pPDOdataByte;
	uint8_t** ppODdataByte;

	pPDOdataByte = &TPDO->sentData[TPDO->dataLength];
	ppODdataByte = &TPDO->mapPointer[TPDO->dataLength];

	switch (TPDO->dataLength) {
	case 8: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x80)) return 1;
	case 7: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x40)) return 1;
	case 6: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x20)) return 1;
	case 5: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x10)) return 1;
	case 4: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x08)) return 1;
	case 3: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x04)) return 1;
	case 2: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x02)) return 1;
	case 1: if (*(--pPDOdataByte) != **(--ppODdataByte) && (TPDO->sendIfCOSFlags & 0x01)) return 1;
	}

	return 0;
}

//int16_t CO_TPDOsend(RPdo* TPDO) {
//	int16_t i;
//	uint8_t* pPDOdataByte;
//	uint8_t** ppODdataByte;
//
//	i = TPDO->dataLength;
//	pPDOdataByte = &TPDO->sentData[0];
//	ppODdataByte = &TPDO->mapPointer[0];
//
//	/* Copy data from Object dictionary. */
//	for (; i > 0; i--) {
//		*(pPDOdataByte++) = **(ppODdataByte++);
//	}
//
//	//PFL(DL_TPDO, "TPDO[%X]:", TPDO->CANtxBuff->ident); DUMP_BYTE_LEVEL(DL_TPDO, TPDO->CANtxBuff->data, TPDO->dataLength);
//
//	TPDO->sendRequest = 0;
//
//	return CO_CANsend(TPDO->CANdevTx, TPDO->CANtxBuff);
//}
//
//void RPdo_process(RPdo* TPDO)
//{
//	if (*TPDO->operatingState == CO_NMT_OPERATIONAL) {
//
//		/* Send PDO by application request or by Event timer */
//		if (TPDO->TPDOCommPar->transmissionType >= 253) {
//			if (TPDO->sendRequest || TPDO->sendIfCOSFlags) {
//				if (CO_TPDOsend(TPDO) == CO_ERROR_NO) {
//					/* successfully sent */
//				}
//			}
//		}
//	}
//	else {
//		/* Not operational or valid. Force TPDO first send after operational or valid. */
//		if (TPDO->TPDOCommPar->transmissionType >= 254) TPDO->sendRequest = 1;
//		else											TPDO->sendRequest = 0;
//	}
//}

int RPdo_Init(RPdo* pPdo
	, const PdoCfg* pPdoCfg
	, const CO_OD_entry* pPrivateOd, int privateOdodSize	//Ë½ÓÐÊý¾Ý×Öµä
)
{
	memset(pPdo, 0, sizeof(RPdo));
	pPdo->pPdoCfg = pPdoCfg;

	return PDO_configMap(pPdo, (CO_OD_entry_t*)pPrivateOd, privateOdodSize, pPdoCfg->TPDOMapPar);
}

void RPdo_CopySentData(RPdo* pRPdo)
{
	for (int i = 0; i < pRPdo->dataLength; i++)
	{
		pRPdo->sentData[i] = *pRPdo->mapPointer[i];
	}
}
