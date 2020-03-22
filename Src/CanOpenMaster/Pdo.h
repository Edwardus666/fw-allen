
#ifndef _CAN_RPDO_H_
#define _CAN_RPDO_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "SdoDef.h"
#include "CO_PDO.h"

#pragma pack(1)
	typedef void (*RPdoDone)(void* pObj, void* pRPdo);
	typedef struct
	{
		uint8 pdoType;	//PDO1rx,PDO2rx,PDO3rx,PDO4rx
		uint16 pdoCommOdIndex;
		uint16 pdoMapOdIndex;
		RPdoDone Done;
		const CO_TPDOCommPar_t* TPDOCommPar;
		const CO_TPDOMapPar_t* TPDOMapPar;

	}PdoCfg;
#pragma pack()

	typedef struct {
		const PdoCfg* pPdoCfg;

			/** If application set this flag, PDO will be later sent by
		function CO_TPDO_process(). Depends on transmission type. */
		uint8_t             sendRequest;

		/** Pointers to 8 data objects, where PDO will be copied */
		uint8_t* mapPointer[8];

		/** Each flag bit is connected with one mapPointer. If flag bit
		is true, CO_TPDO_process() functiuon will send PDO if
		Change of State is detected on value pointed by that mapPointer */
		uint8_t             sendIfCOSFlags;

		/** Data length of the received PDO message. Calculated from mapping */
		uint8_t             dataLength;

		/** Pointers to 8 data objects, where PDO will be copied */
		uint8_t  sentData[8];
	}RPdo;

	int RPdo_Init(RPdo* pPdo
		, const PdoCfg* pPdoCfg
		, const CO_OD_entry* pPrivateOd, int privateOdodSize	//Ë½ÓÐÊý¾Ý×Öµä
	);

	uint8_t RPdo_isCOS(RPdo* TPDO);
	void TPOD_dump(CO_TPDO_t* TPDO);
	void RPOD_dump(RPdo* RPDO);
	void RPdo_CopySentData(RPdo* pRPdo);

#ifdef __cplusplus
}
#endif

#endif
