
#ifndef UNIT_OD_
#define UNIT_OD_

#include "CO_OD.h"
#include "CO_PDO.h"
#ifdef CFG_EX_FRAME

//��Ԫ�����ֵ����
typedef struct
{
	/** Data length of the received PDO message. Calculated from mapping */
	uint8_t isInit;		//�Ƿ��ʼ��
	uint8_t	 dataLength;
	uint8_t* mapPointer[8];
	const CO_RPDOMapPar_t* RPDOMapPar;	//���Գ�ʼ������
}UnitRPDO;

typedef struct
{
	/** Data length of the received PDO message. Calculated from mapping */
	uint8_t	 dataLength;
	uint8_t* mapPointer[8];
	uint8_t txBufData[8];

	const CO_TPDOMapPar_t* TPDOMapPar;	//���Գ�ʼ������
	const CO_TPDOCommPar_t* TPDOCommPar;	//���Գ�ʼ������

	uint8_t sendIfCOSFlags;
	uint8_t sendRequest;
	uint8_t isValid;
	uint8_t isInit;		//�Ƿ��ʼ��
}UnitTPDO;

typedef struct
{
	//MASTER�����CAN��Ԫ��ַ
	uint8 addr;

	/** Pointer to the @ref CO_SDO_objectDictionary (array) */
	const CO_OD_entry_t* OD;	//������OD
	/** Size of the @ref CO_SDO_objectDictionary */
	uint16_t            ODSize;//������OD Size
	/** Pointer to array of CO_OD_extension_t objects. Size of the array is
	equal to ODSize. */
	CO_OD_extension_t* ODExtensions;

	UnitTPDO tpdo[CO_NO_TPDO];
	UnitRPDO rpdo[CO_NO_RPDO];
	uint8 rPDOCount;
	uint8 tPDOCount;
	ODExtenFunc pODFunc;
}UnitOd;

void UnitOd_Init(uint8 addr
	, const CO_OD_entry_t* OD, uint16_t ODSize
	, CO_OD_extension_t* ODExtensions
	, const CO_RPDOMapPar_t* RPDOMapPar, uint8 rPDOCount
	, const CO_TPDOMapPar_t* TPDOMapPar, uint8 tPDOCount
	, const CO_TPDOCommPar_t* TPDOCommPar
);

UnitOd* UnitOd_Find(uint8 addr);
Bool RPdo_SwitchOd(CO_RPDO_t* RPDO, uint8 unitAddr);
Bool TPdo_SwitchOd(CO_TPDO_t* pdo, uint8 unitAddr);
UnitOd* UnitOd_SwitchOd(CO_SDO_t* sdo, uint8 unitAddr);
void UnitOd_SetTPDOSendRequest(Bool isReq);
void UnitOd_DumpTpdo(uint8 addr, uint8 pdoInd);
void UnitOd_DumpRpdo(uint8 addr, uint8 pdoInd);
void UnitOd_Dump(uint8 addr);
#else
#endif

#endif
