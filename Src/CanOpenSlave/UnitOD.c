
#include "Common.h"
#include "UnitOD.h"
#include "CANopen.h"
#include "Master.h"

#ifdef CFG_EX_FRAME

#define UNIT_OD_COUNT CAN_NODE_COUNT
static UnitOd* g_pUnit = Null;
static UnitOd g_Units[UNIT_OD_COUNT] = 
{
	 {CCU_ADDR}
	,{METER_ADDR}
	,{SHELF_LOCK_ADDR}
	,{PMS_ADDR_0}
	,{PMS_ADDR_1}
	,{PMS_ADDR_2}
	,{PMS_ADDR_3}
	,{PMS_ADDR_4}
	,{PMS_ADDR_5}
	,{PMS_ADDR_6}
	,{PMS_ADDR_7}
	,{PMS_ADDR_8}
	,{PMS_ADDR_9}
	,{PMS_ADDR_10}
	,{PMS_ADDR_11}
	,{PMS_ADDR_12}
	,{BTU_ADDR}
};

void UnitOd_DumpTpdoEx(const UnitTPDO* pdo)
{
	Printf("\tsendRequest=%d\n", pdo->sendRequest);
	Printf("\tisValid=%d\n", pdo->isValid);
	Printf("\tsendIfCOSFlags=%d\n", pdo->sendIfCOSFlags);
	Printf("\tdataLength=%d\n", pdo->dataLength);

	DUMP_BYTE_EX(("\tSentBuf:"), pdo->txBufData, pdo->dataLength);
	Printf("\tmapPointer: ");
	for (int i = 0; i < pdo->dataLength; i++)
	{
		Printf("0x%02X ", *pdo->mapPointer[i]);
	}
	Printf("\n");
}

void UnitOd_DumpRpdoEx(const UnitRPDO* pdo)
{
	Printf("\tdataLength=%d\n", pdo->dataLength);
	Printf("\tmapPointer: ");
	for (int i = 0; i < pdo->dataLength; i++)
	{
		Printf("0x%02X ", *pdo->mapPointer[i]);
	}
	Printf("\n");
}

void UnitOd_Dump(uint8 addr)
{
	UnitOd* p = UnitOd_Find(addr);
	if (p == Null)
	{
		Printf("Unit address[%x] invalid\n.", addr);
		return;
	}

	Printf("\addr=%d\n", addr);
	for (int i = 0; i < p->tPDOCount; i++)
	{
		UnitTPDO* pdo = &p->tpdo[i];
		if (!pdo->isValid) break;

		Printf("Dump TPDO[%d]:\n", i);
		UnitOd_DumpTpdoEx(pdo);
	}

	for (int i = 0; i < p->rPDOCount; i++)
	{
		UnitRPDO* pdo = &p->rpdo[i];
		Printf("Dump RPDO[%d]:\n", i);
		if (pdo->dataLength)
		{
			UnitOd_DumpRpdoEx(pdo);
		}
		else
		{
			break;
		}
	}
}

void UnitOd_DumpTpdo(uint8 addr, uint8 pdoInd)
{
	UnitOd* p = UnitOd_Find(addr);
	UnitTPDO* pdo = &p->tpdo[pdoInd];
	if (pdoInd >= p->tPDOCount)
	{
		Printf("PDO[%d] invalid\n.", pdoInd);
		return;
	}
	UnitOd_DumpTpdoEx(pdo);
}

void UnitOd_DumpRpdo(uint8 addr, uint8 pdoInd)
{
	UnitOd* p = UnitOd_Find(addr);
	UnitRPDO* pdo = &p->rpdo[pdoInd];
	if (pdoInd >= p->rPDOCount)
	{
		Printf("PDO[%d] invalid\n.", pdoInd);
		return;
	}
	UnitOd_DumpRpdoEx(pdo);
}

UnitOd* UnitOd_Find(uint8 addr)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Units); i++)
	{
		UnitOd* p = &g_Units[i];
		if (addr == p->addr)
		{
			return p;
		}
	}
	return Null;
}

UnitTPDO* UnitOd_FindTPdo(uint8 addr, uint8 pdoNo)
{
	UnitOd*  p = UnitOd_Find(addr);
	if (p && pdoNo < p->tPDOCount) return &p->tpdo[pdoNo];
	else
		return Null;
}

void UnitOd_SetTPDOSendRequest(Bool isReq)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_Units); i++)
	{
		UnitTPDO* p = g_Units[i].tpdo;
		for (int j = 0; j < g_Units[i].tPDOCount; j++, p++)
		{
			if (p->isValid)
			{
				p->sendRequest = isReq;
			}
		}
	}
}

void UnitOd_Init(uint8 addr
	, const CO_OD_entry_t* OD, uint16_t ODSize
	, CO_OD_extension_t* ODExtensions
	, const CO_RPDOMapPar_t* RPDOMapPar, uint8 rPDOCount
	, const CO_TPDOMapPar_t* TPDOMapPar, uint8 tPDOCount
	, const CO_TPDOCommPar_t* TPDOCommPar
)
{
	UnitOd* p = UnitOd_Find(addr);
	p->addr = addr;
	p->OD = OD;
	p->ODSize = ODSize;
	p->ODExtensions = ODExtensions;
	p->rPDOCount = rPDOCount;
	p->tPDOCount = tPDOCount;

	UnitTPDO* ptpdo = p->tpdo;
	for (uint8 j = 0; j < tPDOCount; j++, ptpdo++)
	{
		ptpdo->TPDOMapPar = &TPDOMapPar[j];
		ptpdo->TPDOCommPar = &TPDOCommPar[j];
		ptpdo->sendRequest = True;
	}

	UnitRPDO* prpdo = p->rpdo;
	for (uint8 j = 0; j < rPDOCount; j++, prpdo++)
	{
		prpdo->RPDOMapPar = &RPDOMapPar[j];
	}
}

UnitOd* UnitOd_SwitchOd(CO_SDO_t* sdo, uint8 unitAddr)
{
	if(g_pUnit && g_pUnit->addr == unitAddr)
		return g_pUnit;
	else 
		g_pUnit = UnitOd_Find(unitAddr);

	if (g_pUnit)
	{
		sdo->OD = g_pUnit->OD;
		sdo->ODSize = g_pUnit->ODSize;
		sdo->ODExtensions = g_pUnit->ODExtensions;
	}
	else
	{
		PFL_ERROR("Switch OD(%x)\n", unitAddr);
	}

	sdo->CANtxBuff->efid = unitAddr;
	return g_pUnit;
}

Bool RPdo_SwitchOd(CO_RPDO_t* pdo, uint8 unitAddr)
{
	UnitOd* p = g_pUnit;
	if (p == UnitOd_SwitchOd(pdo->SDO, unitAddr)) return False;

	UnitRPDO* pPdo = &g_pUnit->rpdo[pdo->pdoNo];

	const CO_RPDOMapPar_t* RPDOMapPar = g_pUnit->rpdo[pdo->pdoNo].RPDOMapPar;
	pdo->RPDOMapPar = RPDOMapPar;

	if (pPdo->isInit)
	{
		pdo->dataLength = pPdo->dataLength;
		for (int j = 0; j < g_pUnit->rpdo[pdo->pdoNo].dataLength; j++) {
			pdo->mapPointer[j] = pPdo->mapPointer[j];
		}
	}
	else
	{
		pPdo->isInit = True;
		CO_RPDOconfigMap(pdo, RPDOMapPar->numberOfMappedObjects);
		pPdo->dataLength = pdo->dataLength;
		for (int j = 0; j < pdo->dataLength; j++) {
			pPdo->mapPointer[j] = pdo->mapPointer[j];
		}
	}
	return True;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t CmpMask(uint8* pSrc, uint8* pDst, int len, uint32 sendIfCOSFlags) {
//	switch (len) {
//	case 8: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x80)) return 1;
//	case 7: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x40)) return 1;
//	case 6: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x20)) return 1;
//	case 5: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x10)) return 1;
//	case 4: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x08)) return 1;
//	case 3: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x04)) return 1;
//	case 2: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x02)) return 1;
//	case 1: if (*(--pSrc) != *(--pDst) && (sendIfCOSFlags & 0x01)) return 1;
//	}
//}

Bool TPdo_SwitchOd(CO_TPDO_t* pdo, uint8 unitAddr)
{
	//切换之前要保存TPDO的txBuff内容
	UnitTPDO* pPdo = UnitOd_FindTPdo(pdo->CANtxBuff->efid, pdo->pdoNo);
	Assert(pPdo);
	pPdo->sendRequest = pdo->sendRequest;
	memcpy(pPdo->txBufData, pdo->CANtxBuff->data, pdo->dataLength);
	
	//切换数据字典
	g_pUnit = UnitOd_SwitchOd(pdo->SDO, unitAddr);
	pPdo = &g_pUnit->tpdo[pdo->pdoNo];

	pdo->TPDOMapPar = pPdo->TPDOMapPar;
	pdo->TPDOCommPar = pPdo->TPDOCommPar;
	pdo->sendRequest = pPdo->sendRequest;
	if (pPdo->isInit)
	{
		pdo->dataLength = pPdo->dataLength;
		pdo->sendIfCOSFlags = pPdo->sendIfCOSFlags;
		pdo->valid = pPdo->isValid;
		pdo->CANtxBuff->DLC = pdo->dataLength;

		for (int j = 0; j < pPdo->dataLength; j++) {
			pdo->mapPointer[j] = pPdo->mapPointer[j];
			pdo->CANtxBuff->data[j] = pPdo->txBufData[j];
		}
	}
	else
	{
		//重新配置映射参数
		CO_TPDOconfigMap(pdo, pdo->TPDOMapPar->numberOfMappedObjects);

		pdo->CANtxBuff->DLC = pdo->dataLength;
		pPdo->dataLength = pdo->dataLength;
		pPdo->sendIfCOSFlags = pdo->sendIfCOSFlags;
		pPdo->isValid = (pdo->dataLength > 0);
		pPdo->isInit = True;

		pdo->valid = pPdo->isValid;	//设置PDO的valid

		for (int j = 0; j < pdo->dataLength; j++) {
			pPdo->mapPointer[j] = pdo->mapPointer[j];
			pPdo->txBufData[j] = pdo->CANtxBuff->data[j];
		}
	}

	pdo->CANtxBuff->efid = unitAddr;
	return pPdo->dataLength > 0;

}
#endif
