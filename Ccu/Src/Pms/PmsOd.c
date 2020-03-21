#include "Common.h"
#include "PmsOd.h"

/*******************************************************************************
   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

/***** Definition for RAM variables *******************************************/
struct sPMS_CO_OD_RAM PMS_CO_OD_RAM = {
		   PMS_CO_OD_FIRST_LAST_WORD,

		   /*1001*/ 0x0,
		   /*1002*/ 0x0L,
		   /*1003*/ {0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		   /*1010*/ {0x3L},
		   /*1011*/ {0x1L},
		   /*2100*/ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		   /*2103*/ 0x0,
		   /*2104*/ 0x0,
		   /*2130*/ {0x3, {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0x0L},

					  PMS_CO_OD_FIRST_LAST_WORD,
};

/***** Definition for EEPROM variables ****************************************/
struct sPMS_CO_OD_EEPROM PMS_CO_OD_EEPROM = {
		   PMS_CO_OD_FIRST_LAST_WORD,

		   /*2106*/ 0x0L,
		   /*2112*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},

					  PMS_CO_OD_FIRST_LAST_WORD,
};

/***** Definition for ROM variables *******************************************/
#define PMS_EVENT_TIME 0 //0xEA60
struct sPMS_CO_OD_ROM PMS_CO_OD_ROM = {    //constant variables, stored in flash
		PMS_CO_OD_FIRST_LAST_WORD,

		/*1000*/ 0x0L,
		/*1005*/ 0x80L,
		/*1006*/ 0x0L,
		/*1007*/ 0x0L,
		/*1008*/ {'C', 'A', 'N', 'o', 'p', 'e', 'n', 'N', 'o', 'd', 'e'},
/*1009*/ {'2', '.', '0'},
/*100A*/ {'1', '.', '0', '.','0', '.', '0', '1'},
		/*1014*/ 0x80L,
		/*1015*/ 0x64,
		/*1016*/ {0x0L, 0x0L, 0x0L, 0x0L},
		/*1017*/ 0x3E8,
		/*1018*/ {0x4, 0x0L, 0x0L, 0x0L, 0x0L},
		/*1019*/ 0x0,
		/*1029*/ {0x0, 0x0, 0x1, 0x0, 0x0, 0x0},
		/*1200*/{{0x2, 0x600L, 0x580L}},

		/*1400*/{{0x2, CO_CAN_ID_RPDO_1, 0xFF},
		/*1401*/ {0x2, CO_CAN_ID_RPDO_2, 0xFE},
		/*1402*/ {0x2, CO_CAN_ID_RPDO_3, 0xFE},
		/*1403*/ {0x2, CO_CAN_ID_RPDO_4, 0xFE}},

		/*1600*/{{0x3, 0x30110410L, 0x30110508L, 0x30110610L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		/*1601*/ {0x2, 0x30110220L, 0x30110320L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		/*1602*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		/*1603*/ {0x0, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},

/*1800*/{{0x6, CO_CAN_ID_TPDO_1 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}}, /*Modified by Kim; set eventimer from 0x3E8 to 0xEA60*/
///*1801*/ {0x6, CO_CAN_ID_TPDO_2 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1802*/ {0x6, CO_CAN_ID_TPDO_3 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1803*/ {0x6, CO_CAN_ID_TPDO_4 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1804*/ {0x6, CO_CAN_ID_TPDO_5 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1805*/ {0x6, CO_CAN_ID_TPDO_6 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1806*/ {0x6, CO_CAN_ID_TPDO_7 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1807*/ {0x6, CO_CAN_ID_TPDO_8 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1808*/ {0x6, CO_CAN_ID_TPDO_9 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*1809*/ {0x6, CO_CAN_ID_TPDO_10, 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*18010*/{0x6, CO_CAN_ID_TPDO_11, 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
///*18011*/{0x6, CO_CAN_ID_TPDO_12, 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}},

/*1A00*/{{0x6, 0x30100108L, 0x30100208L, 0x30150608L, 0x30110108L, 0x30140110L, 0x30140210L, 0x0L, 0x0L},
/*1A01*/ {0x4, 0x30140310L, 0x30140410L, 0x30140510L, 0x30140610L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A02*/ {0x6, 0x30150108L, 0x30150208L, 0x30150408L, 0x30150508L, 0x30170110L, 0x30170210L, 0x0L, 0x0L},
/*1A03*/ {0x5, 0x30180108L, 0x30180220L, 0x30180308L, 0x30180408L, 0x30180508L, 0x0L, 0x0L, 0x0L},
/*1A04*/ {0x4, 0x30010510L, 0x30010610L, 0x30010710L, 0x30010810L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A05*/ {0x4, 0x30011910L, 0x30011A10L, 0x30011B10L, 0x30011C10L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A06*/ {0x4, 0x30012310L, 0x30013F10L, 0x30014010L, 0x30014110L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A07*/ {0x4, 0x30012410L, 0x30013E10L, 0x30014210L, 0x30014310L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A08*/ {0x4, 0x30011F10L, 0x30012010L, 0x30012110L, 0x30012210L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A09*/ {0x4, 0x30010D10L, 0x30010E10L, 0x30010F10L, 0x30011010L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0A*/ {0x3, 0x30001010L, 0x30001110L, 0x30001210L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1A0B*/ {0x3, 0x30001310L, 0x30110120L, 0x31000108L, 0x30100308L, 0x0L, 0x0L, 0x0L, 0x0L}},

/*1F80*/ 0x0L,
/*2101*/ 0x30,
/*2102*/ 0xFA,
/*2111*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},

				   PMS_CO_OD_FIRST_LAST_WORD
};

/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
/*0x1018*/ const CO_OD_entryRecord_t PMS_OD_record1018[5] = {
		   {(void*)& PMS_CO_OD_ROM.identity.maxSubIndex, 0x05,  1},
		   {(void*)& PMS_CO_OD_ROM.identity.vendorID, 0x85,  4},
		   {(void*)& PMS_CO_OD_ROM.identity.productCode, 0x85,  4},
		   {(void*)& PMS_CO_OD_ROM.identity.revisionNumber, 0x85,  4},
		   {(void*)& PMS_CO_OD_ROM.identity.serialNumber, 0x85,  4} };
/*0x1200*/ const CO_OD_entryRecord_t PMS_OD_record1200[3] = {
		   {(void*)& PMS_CO_OD_ROM.SDOServerParameter[0].maxSubIndex, 0x05,  1},
		   {(void*)& PMS_CO_OD_ROM.SDOServerParameter[0].COB_IDClientToServer, 0x85,  4},
		   {(void*)& PMS_CO_OD_ROM.SDOServerParameter[0].COB_IDServerToClient, 0x85,  4} };
/*0x1400*/ const CO_OD_entryRecord_t PMS_OD_record1400[3] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[0].maxSubIndex, 0x05,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[0].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[0].transmissionType, 0x0D,  1} };
/*0x1401*/ const CO_OD_entryRecord_t PMS_OD_record1401[3] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[1].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[1].transmissionType, 0x0D,  1} };
/*0x1402*/ const CO_OD_entryRecord_t PMS_OD_record1402[3] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[2].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[2].transmissionType, 0x0D,  1} };
/*0x1403*/ const CO_OD_entryRecord_t PMS_OD_record1403[3] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[3].COB_IDUsedByRPDO, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOCommunicationParameter[3].transmissionType, 0x0D,  1} };
/*0x1600*/ const CO_OD_entryRecord_t PMS_OD_record1600[9] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[0].mappedObject8, 0x8D,  4} };
/*0x1601*/ const CO_OD_entryRecord_t PMS_OD_record1601[9] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[1].mappedObject8, 0x8D,  4} };
/*0x1602*/ const CO_OD_entryRecord_t PMS_OD_record1602[9] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[2].mappedObject8, 0x8D,  4} };
/*0x1603*/ const CO_OD_entryRecord_t PMS_OD_record1603[9] = {
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.RPDOMappingParameter[3].mappedObject8, 0x8D,  4} };

/*0x1800*/ const CO_OD_entryRecord_t PMS_OD_record1800[7] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[0].maxSubIndex		, 0x05,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[0].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[0].transmissionType	, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[0].inhibitTime		, 0x8D,  2},
		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[0].compatibilityEntry	, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[0].eventTimer			, 0x8D,  2},
		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[0].SYNCStartValue		, 0x0D,  1} };
///*0x1801*/ const CO_OD_entryRecord_t PMS_OD_record1801[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[1].COB_IDUsedByTPDO, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[1].transmissionType, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[1].inhibitTime, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[1].compatibilityEntry, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[1].eventTimer, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[1].SYNCStartValue, 0x0D,  1} };
///*0x1802*/ const CO_OD_entryRecord_t PMS_OD_record1802[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[2].COB_IDUsedByTPDO, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[2].transmissionType, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[2].inhibitTime, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[2].compatibilityEntry, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[2].eventTimer, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[2].SYNCStartValue, 0x0D,  1} };
///*0x1803*/ const CO_OD_entryRecord_t PMS_OD_record1803[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[3].COB_IDUsedByTPDO, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[3].transmissionType, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[3].inhibitTime, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[3].compatibilityEntry, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[3].eventTimer, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[3].SYNCStartValue, 0x0D,  1} };
///*0x1804*/ const CO_OD_entryRecord_t PMS_OD_record1804[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[4].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[4].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[4].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[4].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[4].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[4].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[4].SYNCStartValue		, 0x0D,  1} };
///*0x1805*/ const CO_OD_entryRecord_t PMS_OD_record1805[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[5].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[5].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[5].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[5].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[5].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[5].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[5].SYNCStartValue		, 0x0D,  1} };
///*0x1806*/ const CO_OD_entryRecord_t PMS_OD_record1806[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[6].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[6].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[6].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[6].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[6].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[6].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[6].SYNCStartValue		, 0x0D,  1} };
///*0x1807*/ const CO_OD_entryRecord_t PMS_OD_record1807[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[7].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[7].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[7].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[7].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[7].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[7].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[7].SYNCStartValue		, 0x0D,  1} };
///*0x1808*/ const CO_OD_entryRecord_t PMS_OD_record1808[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[8].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[8].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[8].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[8].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[8].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[8].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[8].SYNCStartValue		, 0x0D,  1} };
///*0x1809*/ const CO_OD_entryRecord_t PMS_OD_record1809[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[9].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[9].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[9].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[9].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[9].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[9].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[9].SYNCStartValue		, 0x0D,  1} };
///*0x180A*/ const CO_OD_entryRecord_t PMS_OD_record180A[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[10].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[10].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[10].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[10].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[10].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[10].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[10].SYNCStartValue		, 0x0D,  1} };
///*0x180B*/ const CO_OD_entryRecord_t PMS_OD_record180B[7] = {
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[11].maxSubIndex		, 0x05,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[11].COB_IDUsedByTPDO	, 0x8D,  4},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[11].transmissionType	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[11].inhibitTime		, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[11].compatibilityEntry	, 0x0D,  1},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[11].eventTimer			, 0x8D,  2},
//		   {(void*)& PMS_CO_OD_ROM.TPDOCommunicationParameter[11].SYNCStartValue		, 0x0D,  1} };

/*0x1A00*/ const CO_OD_entryRecord_t PMS_OD_record1A00[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8D,  4} };
/*0x1A01*/ const CO_OD_entryRecord_t PMS_OD_record1A01[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[1].mappedObject8, 0x8D,  4} };
/*0x1A02*/ const CO_OD_entryRecord_t PMS_OD_record1A02[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[2].mappedObject8, 0x8D,  4} };
/*0x1A03*/ const CO_OD_entryRecord_t PMS_OD_record1A03[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[3].mappedObject8, 0x8D,  4} };
/*0x1A04*/ const CO_OD_entryRecord_t PMS_OD_record1A04[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[4].mappedObject8, 0x8D,  4} };
/*0x1A05*/ const CO_OD_entryRecord_t PMS_OD_record1A05[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[5].mappedObject8, 0x8D,  4} };
/*0x1A06*/ const CO_OD_entryRecord_t PMS_OD_record1A06[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[6].mappedObject8, 0x8D,  4} };
/*0x1A07*/ const CO_OD_entryRecord_t PMS_OD_record1A07[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[7].mappedObject8, 0x8D,  4} };
/*0x1A08*/ const CO_OD_entryRecord_t PMS_OD_record1A08[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[8].mappedObject8, 0x8D,  4} };
/*0x1A09*/ const CO_OD_entryRecord_t PMS_OD_record1A09[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[9].mappedObject8, 0x8D,  4} };
/*0x1A0A*/ const CO_OD_entryRecord_t PMS_OD_record1A0A[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[10].mappedObject8, 0x8D,  4} };
/*0x1A0B*/ const CO_OD_entryRecord_t PMS_OD_record1A0B[9] = {
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject1, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject2, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject3, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject4, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject5, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject6, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject7, 0x8D,  4},
		   {(void*)& PMS_CO_OD_ROM.TPDOMappingParameter[11].mappedObject8, 0x8D,  4} };

/*0x2130*/ const CO_OD_entryRecord_t PMS_OD_record2130[4] = {
		   {(void*)& PMS_CO_OD_RAM.time.maxSubIndex, 0x06,  1},
		   {(void*)& PMS_CO_OD_RAM.time.string[0], 0x06, 30},
		   {(void*)& PMS_CO_OD_RAM.time.epochTimeBaseMs, 0x8E,  8},
		   {(void*)& PMS_CO_OD_RAM.time.epochTimeOffsetMs, 0xBE,  4} };

/**************************************************************************************************/

/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
const CO_OD_entry_t g_PmsOd[] = {
{0x1000, 0x00, 0x85,  4, (void*)& PMS_CO_OD_ROM.deviceType},
{0x1001, 0x00, 0x36,  1, (void*)& PMS_CO_OD_RAM.errorRegister},
{0x1002, 0x00, 0xB6,  4, (void*)& PMS_CO_OD_RAM.manufacturerStatusRegister},
{0x1003, 0x08, 0x8E,  4, (void*)& PMS_CO_OD_RAM.preDefinedErrorField[0]},
{0x1005, 0x00, 0x8D,  4, (void*)& PMS_CO_OD_ROM.COB_ID_SYNCMessage},
{0x1006, 0x00, 0x8D,  4, (void*)& PMS_CO_OD_ROM.communicationCyclePeriod},
{0x1007, 0x00, 0x8D,  4, (void*)& PMS_CO_OD_ROM.synchronousWindowLength},
{0x1008, 0x00, 0x05, 11, (void*)& PMS_CO_OD_ROM.manufacturerDeviceName[0]},
{0x1009, 0x00, 0x05,  4, (void*)& PMS_CO_OD_ROM.manufacturerHardwareVersion[0]},
{0x100A, 0x00, 0x05, 10, (void*)& PMS_CO_OD_ROM.manufacturerSoftwareVersion[0]},
{0x1010, 0x01, 0x8E,  4, (void*)& PMS_CO_OD_RAM.storeParameters[0]},
{0x1011, 0x01, 0x8E,  4, (void*)& PMS_CO_OD_RAM.restoreDefaultParameters[0]},
{0x1014, 0x00, 0x85,  4, (void*)& PMS_CO_OD_ROM.COB_ID_EMCY},
{0x1015, 0x00, 0x8D,  2, (void*)& PMS_CO_OD_ROM.inhibitTimeEMCY},
{0x1016, 0x04, 0x8D,  4, (void*)& PMS_CO_OD_ROM.consumerHeartbeatTime[0]},
{0x1017, 0x00, 0x8D,  2, (void*)& PMS_CO_OD_ROM.producerHeartbeatTime},
{0x1018, 0x04, 0x00,  0, (void*)& PMS_OD_record1018},
{0x1019, 0x00, 0x0D,  1, (void*)& PMS_CO_OD_ROM.synchronousCounterOverflowValue},
{0x1029, 0x06, 0x0D,  1, (void*)& PMS_CO_OD_ROM.errorBehavior[0]},
{0x1200, 0x02, 0x00,  0, (void*)& PMS_OD_record1200},

{0x1400, 0x02, 0x00,  0, (void*)& PMS_OD_record1400},
{0x1401, 0x02, 0x00,  0, (void*)& PMS_OD_record1401},
{0x1402, 0x02, 0x00,  0, (void*)& PMS_OD_record1402},
{0x1403, 0x02, 0x00,  0, (void*)& PMS_OD_record1403},

{0x1600, 0x08, 0x00,  0, (void*)& PMS_OD_record1600},
{0x1601, 0x08, 0x00,  0, (void*)& PMS_OD_record1601},
{0x1602, 0x08, 0x00,  0, (void*)& PMS_OD_record1602},
{0x1603, 0x08, 0x00,  0, (void*)& PMS_OD_record1603},

{0x1800, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1801, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1802, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1803, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1804, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1805, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1806, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1807, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1808, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x1809, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x180A, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},
{0x180B, 0x06, 0x00,  0, (void*)& PMS_OD_record1800},

{0x1A00, 0x08, 0x00,  0, (void*)& PMS_OD_record1A00},
{0x1A01, 0x08, 0x00,  0, (void*)& PMS_OD_record1A01},
{0x1A02, 0x08, 0x00,  0, (void*)& PMS_OD_record1A02},
{0x1A03, 0x08, 0x00,  0, (void*)& PMS_OD_record1A03},
{0x1A04, 0x08, 0x00,  0, (void*)& PMS_OD_record1A04},
{0x1A05, 0x08, 0x00,  0, (void*)& PMS_OD_record1A05},
{0x1A06, 0x08, 0x00,  0, (void*)& PMS_OD_record1A06},
{0x1A07, 0x08, 0x00,  0, (void*)& PMS_OD_record1A07},
{0x1A08, 0x08, 0x00,  0, (void*)& PMS_OD_record1A08},
{0x1A09, 0x08, 0x00,  0, (void*)& PMS_OD_record1A09},
{0x1A0A, 0x08, 0x00,  0, (void*)& PMS_OD_record1A0A},
{0x1A0B, 0x08, 0x00,  0, (void*)& PMS_OD_record1A0B},

{0x1F80, 0x00, 0x8D,  4, (void*)& PMS_CO_OD_ROM.NMTStartup},

/**************************************************************************************************/
{0x2100, 0x00, 0x36, 10, (void*)& PMS_CO_OD_RAM.errorStatusBits[0]},
{0x2101, 0x00, 0x0D,  1, (void*)& PMS_CO_OD_ROM.CANNodeID},
{0x2102, 0x00, 0x8D,  2, (void*)& PMS_CO_OD_ROM.CANBitRate},
{0x2103, 0x00, 0x8E,  2, (void*)& PMS_CO_OD_RAM.SYNCCounter},
{0x2104, 0x00, 0x86,  2, (void*)& PMS_CO_OD_RAM.SYNCTime},
{0x2106, 0x00, 0x87,  4, (void*)& PMS_CO_OD_EEPROM.powerOnCounter},
{0x2111, 0x10, 0xFD,  4, (void*)& PMS_CO_OD_ROM.variableROMInt32[0]},
{0x2112, 0x10, 0xFF,  4, (void*)& PMS_CO_OD_EEPROM.variableNVInt32[0]},
{0x2130, 0x03, 0x00,  0, (void*)& PMS_OD_record2130},
};
//#define PMS_CO_OD_NoOfElements  sizeof(PMS_CO_OD)/sizeof(CO_OD_entry_t)

int PmsOd_GetSize() { return sizeof(g_PmsOd) / sizeof(CO_OD_entry_t); }

//#define CFG_PMS_OD

#include "PmsMgr.h"
#define PMS_OD(ind)	\
	{0x1008, 0x00, 0x05, 11, &g_Pms[ind].manufacturerDeviceName[0]},									\
	{0x100A, 0x00, 0x05, 10, &g_Pms[ind].manufacturerSoftwareVersion[0]},								\
	{0x1017, 0x00, 0x05,  2, &g_Pms[ind].base.HeartbeatTime},											\
	/********************************PMS参数及状态***********************************************/		\
	{0x2003, 0x00, 0x05, 1, &g_Pms[ind].elock_status_new},												\
	{0x2004, 0x00, 0x05, 1, &g_Pms[ind].elock_error_status},											\
	{0x2005, 0x00, 0x05, 1, &g_Pms[ind].is_bat_detected},												\
	{0x2008, 0x00, 0x05, 2, &g_Pms[ind].hwVer},															\
	{0x2009, 0x00, 0x05, 7, &g_Pms[ind].fwVer},															\
	/*********************************电池参数*****************************************************/	\
	{0x2026, 0x02, 0x05, 7, &g_Pms[ind].bms.OD_record2026},												\
	/*********************************充电器参数**************************************************/		\
	{0x2040, 0x00, 0x76, 2, &g_Pms[ind].charger.OD_charger_HW_version},									\
	{0x2041, 0x00, 0x76, 7, &g_Pms[ind].charger.OD_charger_FW_version},									\
	{0x2043, 0x06, 0x00, 0, &g_Pms[ind].charger.OD_record2043},											\
	{0x20FF, 0x03, 0x00, 0, &g_Pms[ind].charger.OD_record20FF},											\
	/*********************************固件文件**************************************************/		\
	{0x2FF0, 0x00, 0x00, 0, Null},			/*Ccu固件*/													\
	/*********************************电池参数*****************************************************/	\
	{0x3000, DEV_INFO_REG_SIZE , 0xB6, 2,  &g_Pms[ind].bms.reg_unit[DEV_INFO_REG_OFFSET]},				\
	{0x3001, READ_ONLY_REG_SIZE, 0xB6, 2,  &g_Pms[ind].bms.reg_unit[READ_ONLY_REG_OFFSET]},				\
	{0x3002, CTRL_REG_SIZE	   , 0xB6, 2,  &g_Pms[ind].bms.reg_unit[CTRL_REG_OFFSET]},					\
	{0x3003, PARAM_REG_SIZE    , 0xB6, 2,  &g_Pms[ind].bms.reg_unit[PARAM_REG_OFFSET]},					\
	{0x3004, USER_REG_SIZE     , 0xB6, 2,  &g_Pms[ind].bms.reg_unit[USER_REG_OFFSET]},					\
	/*********************************PMS参数*****************************************************/		\
	{0x3010, 0x02, 0x00, 0, &g_Pms[ind].OD_record3010},													\
	{0x3011, 0x06, 0x00, 0, &g_Pms[ind].OD_record3011},													\
	{0x3013, 0x03, 0x00, 0, &g_Pms[ind].OD_record3013},													\
	{0x3014, 0x06, 0x00, 0, &g_Pms[ind].OD_record3014},													\
	{0x3015, 0x07, 0x00, 0, &g_Pms[ind].charger.OD_record3015},											\
	{0x3016, 0x03, 0x00, 0, &g_Pms[ind].charger.OD_record3016},											\
	{0x3017, 0x02, 0x00, 0, &g_Pms[ind].charger.OD_record3017},											\
	{0x3018, 0x05, 0x00, 0, &g_Pms[ind].OD_record3018},													\
	/*********************************固件文件**************************************************/		\
	{0x301A, 0x03, 0x00, 0, &g_Pms[ind].OD_record301A.OD_record},		/*Pms固件*/						\
	{0x3021, 0x03, 0x00, 0, &g_Pms[ind].OD_record3021.OD_record},		/*Charger固件*/					\
	/*********************************电池容量测试**************************************************/	\
	{0x3100, 0x03, 0x00, 0, &g_Pms[ind].OD_record3100},										

const CO_OD_entry_t g_Pms_Od[15][PMS_OD_SIZE] =
{
	{ PMS_OD(0) },
	{ PMS_OD(1) },
	{ PMS_OD(2) },
	{ PMS_OD(3) },
	{ PMS_OD(4) },
	{ PMS_OD(5) },
	{ PMS_OD(6) },
	{ PMS_OD(7) },
	{ PMS_OD(8) },
	{ PMS_OD(9) },
	{ PMS_OD(10) },
	{ PMS_OD(11) },
	{ PMS_OD(12) },
	{ PMS_OD(13) },
	{ PMS_OD(14) },
};
