#include "Common.h"
#include "BaseOd.h"

/*******************************************************************************
   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

/***** Definition for RAM variables *******************************************/
//struct sBASE_CO_OD_RAM BASE_CO_OD_RAM = {
//		   BASE_CO_OD_FIRST_LAST_WORD,
//
//		   /*1001*/ 0x0,
//		   /*1002*/ 0x0L,
//		   /*1003*/ {0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
//		   ///*1010*/ {0x3L},
//		   ///*1011*/ {0x1L},
//		   /*2100*/ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//		   ///*2103*/ 0x0,
//		   ///*2104*/ 0x0,
//		   ///*2107*/ {0x3E8, 0x0, 0x0, 0x0, 0x0},
//		   ///*2108*/ {0},
//		   ///*2109*/ {0},
//		   ///*2110*/ {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
//		   ///*2120*/ {0x5, 0x1234567890ABCDEFLL, 0x234567890ABCDEF1LL, 12.345, 456.789, 0},
//		   ///*2130*/ {0x3, {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0x0L},
//		   ///*6000*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
//		   ///*6200*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
//		   ///*6401*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
//		   ///*6411*/ {0, 0, 0, 0, 0, 0, 0, 0},
//
//					  BASE_CO_OD_FIRST_LAST_WORD,
//};

/***** Definition for EEPROM variables ****************************************/
//struct sBASE_CO_OD_EEPROM BASE_CO_OD_EEPROM = {
//		   BASE_CO_OD_FIRST_LAST_WORD,
//
//		   /*2106*/ 0x0L,
//		   /*2112*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
//
//					  BASE_CO_OD_FIRST_LAST_WORD,
//};

/***** Definition for ROM variables *******************************************/
#define PMS_EVENT_TIME 0 //0xEA60
struct sBASE_CO_OD_ROM BASE_CO_OD_ROM = {    //constant variables, stored in flash
		BASE_CO_OD_FIRST_LAST_WORD,

		///*1000*/ 0x0L,
		///*1005*/ 0x80L,
		///*1006*/ 0x0L,
		///*1007*/ 0x0L,
		///*1008*/ {'W', 'H', 'M', 'E', 'T', 'E', 'T'},
		///*1009*/ {'1', '.', '0'},
		///*100A*/ {'1', '.', '0', '.','0', '.', '0', '1'},
		///*1014*/ 0x80L,
		///*1015*/ 0x64,
		///*1016*/ {0x0L, 0x0L, 0x0L, 0x0L},
		/*1017*/ 0x3E8,
		///*1018*/ {0x4, 0x0L, 0x0L, 0x0L, 0x0L},
		///*1019*/ 0x0,
		///*1029*/ {0x0, 0x0, 0x1, 0x0, 0x0, 0x0},
		///*1200*/{{0x2, 0x600L, 0x580L}},

		/*1400*/{{0x2, CO_CAN_ID_RPDO_1, 0xFF},
		/*1401*/ {0x2, CO_CAN_ID_RPDO_2, 0xFE},
		/*1402*/ {0x2, CO_CAN_ID_RPDO_3, 0xFE},
		/*1403*/ {0x2, CO_CAN_ID_RPDO_4, 0xFE}},

		///*1600*/{{0x5, 0x30110208L, 0x30110308L, 0x30110410L, 0x30110508L, 0x30110610L, 0x0L, 0x0L, 0x0L},
		///*1601*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		///*1602*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		///*1603*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},

		/*1800*/{{0x6, CO_CAN_ID_TPDO_1 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}, 
		/*1801*/ {0x6, CO_CAN_ID_TPDO_2 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
		/*1802*/ {0x6, CO_CAN_ID_TPDO_3 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
		/*1803*/ {0x6, CO_CAN_ID_TPDO_4 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}},

		///*1A00*/{{0x6, 0x30100108L, 0x30100208L, 0x30150608L, 0x30110108L, 0x30140110L, 0x30140210L, 0x0L, 0x0L},
		///*1A01*/ {0x4, 0x30140310L, 0x30140410L, 0x30140510L, 0x30140610L, 0x0L, 0x0L, 0x0L, 0x0L},
		///*1A02*/ {0x6, 0x30150108L, 0x30150208L, 0x30150408L, 0x30150508L, 0x30170110L, 0x30170210L, 0x0L, 0x0L},
		///*1A03*/ {0x5, 0x30180108L, 0x30180220L, 0x30180308L, 0x30180408L, 0x30180508L, 0x0L, 0x0L, 0x0L}},

///*1F80*/ 0x0L,
///*2101*/ 0x30,
///*2102*/ 0xFA,
///*2111*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},

				   BASE_CO_OD_FIRST_LAST_WORD
};

/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
///*0x1018*/ const CO_OD_entryRecord_t BASE_OD_record1018[5] = {
//		   {(void*)& BASE_CO_OD_ROM.identity.maxSubIndex, 0x05,  1},
//		   {(void*)& BASE_CO_OD_ROM.identity.vendorID, 0x85,  4},
//		   {(void*)& BASE_CO_OD_ROM.identity.productCode, 0x85,  4},
//		   {(void*)& BASE_CO_OD_ROM.identity.revisionNumber, 0x85,  4},
//		   {(void*)& BASE_CO_OD_ROM.identity.serialNumber, 0x85,  4} };
///*0x1200*/ const CO_OD_entryRecord_t BASE_OD_record1200[3] = {
//		   {(void*)& BASE_CO_OD_ROM.SDOServerParameter[0].maxSubIndex, 0x05,  1},
//		   {(void*)& BASE_CO_OD_ROM.SDOServerParameter[0].COB_IDClientToServer, 0x85,  4},
//		   {(void*)& BASE_CO_OD_ROM.SDOServerParameter[0].COB_IDServerToClient, 0x85,  4} };
/*0x1400*/ const CO_OD_entryRecord_t BASE_OD_record1400[3] = {
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[0].maxSubIndex, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[0].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[0].transmissionType, 0x0D,  1} };
/*0x1401*/ const CO_OD_entryRecord_t BASE_OD_record1401[3] = {
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[1].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[1].transmissionType, 0x0D,  1} };
/*0x1402*/ const CO_OD_entryRecord_t BASE_OD_record1402[3] = {
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[2].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[2].transmissionType, 0x0D,  1} };
/*0x1403*/ const CO_OD_entryRecord_t BASE_OD_record1403[3] = {
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[3].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.RPDOCommunicationParameter[3].transmissionType, 0x0D,  1} };
///*0x1600*/ const CO_OD_entryRecord_t BASE_OD_record1600[9] = {
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[0].mappedObject8, 0x8D,  4} };
///*0x1601*/ const CO_OD_entryRecord_t BASE_OD_record1601[9] = {
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[1].mappedObject8, 0x8D,  4} };
///*0x1602*/ const CO_OD_entryRecord_t BASE_OD_record1602[9] = {
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[2].mappedObject8, 0x8D,  4} };
///*0x1603*/ const CO_OD_entryRecord_t BASE_OD_record1603[9] = {
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.RPDOMappingParameter[3].mappedObject8, 0x8D,  4} };

/*0x1800*/ const CO_OD_entryRecord_t BASE_OD_record1800[7] = {
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[0].maxSubIndex		, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[0].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[0].transmissionType	, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[0].inhibitTime		, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[0].compatibilityEntry	, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[0].eventTimer			, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[0].SYNCStartValue		, 0x0D,  1} };
/*0x1801*/ const CO_OD_entryRecord_t BASE_OD_record1801[7] = {
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[1].COB_IDUsedByTPDO, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[1].transmissionType, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[1].inhibitTime, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[1].compatibilityEntry, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[1].eventTimer, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[1].SYNCStartValue, 0x0D,  1} };
/*0x1802*/ const CO_OD_entryRecord_t BASE_OD_record1802[7] = {
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[2].COB_IDUsedByTPDO, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[2].transmissionType, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[2].inhibitTime, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[2].compatibilityEntry, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[2].eventTimer, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[2].SYNCStartValue, 0x0D,  1} };
/*0x1803*/ const CO_OD_entryRecord_t BASE_OD_record1803[7] = {
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[3].COB_IDUsedByTPDO, 0x8D,  4},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[3].transmissionType, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[3].inhibitTime, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[3].compatibilityEntry, 0x0D,  1},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[3].eventTimer, 0x8D,  2},
		   {(void*)& BASE_CO_OD_ROM.TPDOCommunicationParameter[3].SYNCStartValue, 0x0D,  1} };


///*0x1A00*/ const CO_OD_entryRecord_t BASE_OD_record1A00[9] = {
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8D,  4} };
///*0x1A01*/ const CO_OD_entryRecord_t BASE_OD_record1A01[9] = {
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[1].mappedObject8, 0x8D,  4} };
///*0x1A02*/ const CO_OD_entryRecord_t BASE_OD_record1A02[9] = {
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[2].mappedObject8, 0x8D,  4} };
///*0x1A03*/ const CO_OD_entryRecord_t BASE_OD_record1A03[9] = {
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject1, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject2, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject3, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject4, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject5, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject6, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject7, 0x8D,  4},
//		   {(void*)& BASE_CO_OD_ROM.TPDOMappingParameter[3].mappedObject8, 0x8D,  4} };

///*0x2120*/ const CO_OD_entryRecord_t BASE_OD_record2120[6] = {
//		   {(void*)& BASE_CO_OD_RAM.testVar.maxSubIndex, 0x06,  1},
//		   {(void*)& BASE_CO_OD_RAM.testVar.I64, 0xBE,  8},
//		   {(void*)& BASE_CO_OD_RAM.testVar.U64, 0xBE,  8},
//		   {(void*)& BASE_CO_OD_RAM.testVar.R32, 0xBE,  4},
//		   {(void*)& BASE_CO_OD_RAM.testVar.R64, 0xBE,  8},
//		   {0, 0x0E,  0} };
///*0x2130*/ const CO_OD_entryRecord_t BASE_OD_record2130[4] = {
//		   {(void*)& BASE_CO_OD_RAM.time.maxSubIndex, 0x06,  1},
//		   {(void*)& BASE_CO_OD_RAM.time.string[0], 0x06, 30},
//		   {(void*)& BASE_CO_OD_RAM.time.epochTimeBaseMs, 0x8E,  8},
//		   {(void*)& BASE_CO_OD_RAM.time.epochTimeOffsetMs, 0xBE,  4} };

/**************************************************************************************************/

/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
//const CO_OD_entry_t g_PmeterOd[] = {
////{0x1000, 0x00, 0x85,  4, (void*)& BASE_CO_OD_ROM.deviceType},
////{0x1001, 0x00, 0x36,  1, (void*)& BASE_CO_OD_RAM.errorRegister},
////{0x1002, 0x00, 0xB6,  4, (void*)& BASE_CO_OD_RAM.manufacturerStatusRegister},
////{0x1003, 0x08, 0x8E,  4, (void*)& BASE_CO_OD_RAM.preDefinedErrorField[0]},
////{0x1005, 0x00, 0x8D,  4, (void*)& BASE_CO_OD_ROM.COB_ID_SYNCMessage},
////{0x1006, 0x00, 0x8D,  4, (void*)& BASE_CO_OD_ROM.communicationCyclePeriod},
////{0x1007, 0x00, 0x8D,  4, (void*)& BASE_CO_OD_ROM.synchronousWindowLength},
//{0x1008, 0x00, 0x05, 11, (void*)& BASE_CO_OD_ROM.manufacturerDeviceName[0]},
//{0x1009, 0x00, 0x05,  4, (void*)& BASE_CO_OD_ROM.manufacturerHardwareVersion[0]},
//{0x100A, 0x00, 0x05, 10, (void*)& BASE_CO_OD_ROM.manufacturerSoftwareVersion[0]},
////{0x1010, 0x01, 0x8E,  4, (void*)& BASE_CO_OD_RAM.storeParameters[0]},
////{0x1011, 0x01, 0x8E,  4, (void*)& BASE_CO_OD_RAM.restoreDefaultParameters[0]},
////{0x1014, 0x00, 0x85,  4, (void*)& BASE_CO_OD_ROM.COB_ID_EMCY},
////{0x1015, 0x00, 0x8D,  2, (void*)& BASE_CO_OD_ROM.inhibitTimeEMCY},
////{0x1016, 0x04, 0x8D,  4, (void*)& BASE_CO_OD_ROM.consumerHeartbeatTime[0]},
////{0x1017, 0x00, 0x8D,  2, (void*)& BASE_CO_OD_ROM.producerHeartbeatTime},
//{0x1018, 0x04, 0x00,  0, (void*)& BASE_OD_record1018},
////{0x1019, 0x00, 0x0D,  1, (void*)& BASE_CO_OD_ROM.synchronousCounterOverflowValue},
////{0x1029, 0x06, 0x0D,  1, (void*)& BASE_CO_OD_ROM.errorBehavior[0]},
////{0x1200, 0x02, 0x00,  0, (void*)& BASE_OD_record1200},
//
//{0x1400, 0x02, 0x00,  0, (void*)& BASE_OD_record1400},
//{0x1401, 0x02, 0x00,  0, (void*)& BASE_OD_record1401},
//{0x1402, 0x02, 0x00,  0, (void*)& BASE_OD_record1402},
//{0x1403, 0x02, 0x00,  0, (void*)& BASE_OD_record1403},
//
////{0x1600, 0x08, 0x00,  0, (void*)& BASE_OD_record1600},
////{0x1601, 0x08, 0x00,  0, (void*)& BASE_OD_record1601},
////{0x1602, 0x08, 0x00,  0, (void*)& BASE_OD_record1602},
////{0x1603, 0x08, 0x00,  0, (void*)& BASE_OD_record1603},
//
//{0x1800, 0x06, 0x00,  0, (void*)& BASE_OD_record1800},
//{0x1801, 0x06, 0x00,  0, (void*)& BASE_OD_record1801},
//{0x1802, 0x06, 0x00,  0, (void*)& BASE_OD_record1802},
//{0x1803, 0x06, 0x00,  0, (void*)& BASE_OD_record1803},
//
////{0x1A00, 0x08, 0x00,  0, (void*)& BASE_OD_record1A00},
////{0x1A01, 0x08, 0x00,  0, (void*)& BASE_OD_record1A01},
////{0x1A02, 0x08, 0x00,  0, (void*)& BASE_OD_record1A02},
////{0x1A03, 0x08, 0x00,  0, (void*)& BASE_OD_record1A03},
//
////{0x1F80, 0x00, 0x8D,  4, (void*)& BASE_CO_OD_ROM.NMTStartup},

/**************************************************************************************************/
//{0x2100, 0x00, 0x36, 10, (void*)& BASE_CO_OD_RAM.errorStatusBits[0]},
//{0x2101, 0x00, 0x0D,  1, (void*)& BASE_CO_OD_ROM.CANNodeID},
//{0x2102, 0x00, 0x8D,  2, (void*)& BASE_CO_OD_ROM.CANBitRate},
//{0x2103, 0x00, 0x8E,  2, (void*)& BASE_CO_OD_RAM.SYNCCounter},
//{0x2104, 0x00, 0x86,  2, (void*)& BASE_CO_OD_RAM.SYNCTime},
//{0x2106, 0x00, 0x87,  4, (void*)& BASE_CO_OD_EEPROM.powerOnCounter},
//{0x2107, 0x05, 0xBE,  2, (void*)& BASE_CO_OD_RAM.performance[0]},
//{0x2108, 0x01, 0xB6,  2, (void*)& BASE_CO_OD_RAM.temperature[0]},
//{0x2109, 0x01, 0xB6,  2, (void*)& BASE_CO_OD_RAM.voltage[0]},
//{0x2110, 0x10, 0xFE,  4, (void*)& BASE_CO_OD_RAM.variableInt32[0]},
//{0x2111, 0x10, 0xFD,  4, (void*)& BASE_CO_OD_ROM.variableROMInt32[0]},
//{0x2112, 0x10, 0xFF,  4, (void*)& BASE_CO_OD_EEPROM.variableNVInt32[0]},
//{0x2120, 0x05, 0x00,  0, (void*)& BASE_OD_record2120},
//{0x2130, 0x03, 0x00,  0, (void*)& BASE_OD_record2130},

//{0x6000, 0x08, 0x76,  1, (void*)& BASE_CO_OD_RAM.readInput8Bit[0]},
//{0x6200, 0x08, 0x3E,  1, (void*)& BASE_CO_OD_RAM.writeOutput8Bit[0]},
//{0x6401, 0x0C, 0xB6,  2, (void*)& BASE_CO_OD_RAM.readAnalogueInput16Bit[0]},
//{0x6411, 0x08, 0xBE,  2, (void*)& BASE_CO_OD_RAM.writeAnalogueOutput16Bit[0]},
//};
//#define BASE_CO_OD_NoOfElements  sizeof(BASE_CO_OD)/sizeof(CO_OD_entry_t)
