#include "Common.h"
#include "MeterOd.h"
#include "BaseOd.h"
#include "WhMeter.h"

/*******************************************************************************
   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

/***** Definition for RAM variables *******************************************/
struct sMETER_CO_OD_RAM METER_CO_OD_RAM = {
		   METER_CO_OD_FIRST_LAST_WORD,

		   ///*1001*/ 0x0,
		   ///*1002*/ 0x0L,
		   ///*1003*/ {0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		   ///*1010*/ {0x3L},
		   ///*1011*/ {0x1L},
		   ///*2100*/ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		   ///*2103*/ 0x0,
		   ///*2104*/ 0x0,
		   ///*2107*/ {0x3E8, 0x0, 0x0, 0x0, 0x0},
		   ///*2108*/ {0},
		   ///*2109*/ {0},
		   ///*2110*/ {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
		   ///*2120*/ {0x5, 0x1234567890ABCDEFLL, 0x234567890ABCDEF1LL, 12.345, 456.789, 0},
		   ///*2130*/ {0x3, {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0x0L},
		   ///*6000*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		   ///*6200*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
		   ///*6401*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		   ///*6411*/ {0, 0, 0, 0, 0, 0, 0, 0},

					  METER_CO_OD_FIRST_LAST_WORD,
};

/***** Definition for EEPROM variables ****************************************/
//struct sMETER_CO_OD_EEPROM METER_CO_OD_EEPROM = {
//		   METER_CO_OD_FIRST_LAST_WORD,
//
//		   /*2106*/ 0x0L,
//		   /*2112*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
//
//					  METER_CO_OD_FIRST_LAST_WORD,
//};

/***** Definition for ROM variables *******************************************/
#define PMS_EVENT_TIME 0 //0xEA60
struct sMETER_CO_OD_ROM METER_CO_OD_ROM = {    //constant variables, stored in flash
		METER_CO_OD_FIRST_LAST_WORD,

		///*1000*/ 0x0L,
		///*1005*/ 0x80L,
		///*1006*/ 0x0L,
		///*1007*/ 0x0L,
		/*1008*/ {'W', 'H', 'M', 'E', 'T', 'E', 'T'},
		/*1009*/ {'1', '.', '0'},
		/*100A*/ {'1', '.', '0', '.','0', '.', '0', '1'},
		///*1014*/ 0x80L,
		///*1015*/ 0x64,
		///*1016*/ {0x0L, 0x0L, 0x0L, 0x0L},
		///*1017*/ 0x3E8,
		/*1018*/ {0x4, 0x0L, 0x0L, 0x0L, 0x0L},
		///*1019*/ 0x0,
		///*1029*/ {0x0, 0x0, 0x1, 0x0, 0x0, 0x0},
		///*1200*/{{0x2, 0x600L, 0x580L}},

		///*1400*/{{0x2, CO_CAN_ID_RPDO_1, 0xFF},
		///*1401*/ {0x2, CO_CAN_ID_RPDO_2, 0xFE},
		///*1402*/ {0x2, CO_CAN_ID_RPDO_3, 0xFE},
		///*1403*/ {0x2, CO_CAN_ID_RPDO_4, 0xFE}},

//		/*1600*/{{0x5, 0x30110208L, 0x30110308L, 0x30110410L, 0x30110508L, 0x30110610L, 0x0L, 0x0L, 0x0L},

		///*1800*/{{0x6, CO_CAN_ID_TPDO_1 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}, 
		///*1801*/ {0x6, CO_CAN_ID_TPDO_2 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
		///*1802*/ {0x6, CO_CAN_ID_TPDO_3 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
		///*1803*/ {0x6, CO_CAN_ID_TPDO_4 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}},

		/*1A00*/{{0x6, 0x20000120L, 0x20000220L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
		/*1A01*/ {0x4, 0x20000110L, 0x20000220L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},

///*1F80*/ 0x0L,
///*2101*/ 0x30,
///*2102*/ 0xFA,
///*2111*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},

				   METER_CO_OD_FIRST_LAST_WORD
};

/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
/*0x1018*/ const CO_OD_entryRecord_t METER_OD_record1018[5] = {
		   {(void*)& METER_CO_OD_ROM.identity.maxSubIndex, 0x05,  1},
		   {(void*)& METER_CO_OD_ROM.identity.vendorID, 0x85,  4},
		   {(void*)& METER_CO_OD_ROM.identity.productCode, 0x85,  4},
		   {(void*)& METER_CO_OD_ROM.identity.revisionNumber, 0x85,  4},
		   {(void*)& METER_CO_OD_ROM.identity.serialNumber, 0x85,  4} };

/*0x1A00*/ const CO_OD_entryRecord_t METER_OD_record1A00[9] = {
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8D,  4} };
/*0x1A01*/ const CO_OD_entryRecord_t METER_OD_record1A01[9] = {
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject1, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject2, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject3, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject4, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject5, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject6, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject7, 0x8D,  4},
		   {(void*)& METER_CO_OD_ROM.TPDOMappingParameter[1].mappedObject8, 0x8D,  4} };


extern WhMeter g_WhMeter;
const static uint8 record2000_numberOfMappedObjects = 5;
/*0x2000*/ const CO_OD_entryRecord_t METER_OD_record2000[5] = {
		   {(void*)& record2000_numberOfMappedObjects, 0x0D,  1},
		   {(void*)& g_WhMeter.totalWh, 0x8E,  4},
		   {(void*)& g_WhMeter.nowTotalWh, 0x8E,  4},
		   {(void*)& g_WhMeter.aVoltage, 0x8E,  2},
		   {(void*)& g_WhMeter.aCurrent, 0x8E,  4},

};

/**************************************************************************************************/

/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
const CO_OD_entry_t g_MeterOd[METER_OD_SIZE] = {
//{0x1000, 0x00, 0x85,  4, (void*)& METER_CO_OD_ROM.deviceType},
//{0x1001, 0x00, 0x36,  1, (void*)& METER_CO_OD_RAM.errorRegister},
//{0x1002, 0x00, 0xB6,  4, (void*)& METER_CO_OD_RAM.manufacturerStatusRegister},
//{0x1003, 0x08, 0x8E,  4, (void*)& METER_CO_OD_RAM.preDefinedErrorField[0]},
//{0x1005, 0x00, 0x8D,  4, (void*)& METER_CO_OD_ROM.COB_ID_SYNCMessage},
//{0x1006, 0x00, 0x8D,  4, (void*)& METER_CO_OD_ROM.communicationCyclePeriod},
//{0x1007, 0x00, 0x8D,  4, (void*)& METER_CO_OD_ROM.synchronousWindowLength},
{0x1008, 0x00, 0x05, 11, (void*)& METER_CO_OD_ROM.manufacturerDeviceName[0]},
{0x1009, 0x00, 0x05,  4, (void*)& METER_CO_OD_ROM.manufacturerHardwareVersion[0]},
{0x100A, 0x00, 0x05, 10, (void*)& METER_CO_OD_ROM.manufacturerSoftwareVersion[0]},
//{0x1010, 0x01, 0x8E,  4, (void*)& METER_CO_OD_RAM.storeParameters[0]},
//{0x1011, 0x01, 0x8E,  4, (void*)& METER_CO_OD_RAM.restoreDefaultParameters[0]},
//{0x1014, 0x00, 0x85,  4, (void*)& METER_CO_OD_ROM.COB_ID_EMCY},
//{0x1015, 0x00, 0x8D,  2, (void*)& METER_CO_OD_ROM.inhibitTimeEMCY},
//{0x1016, 0x04, 0x8D,  4, (void*)& METER_CO_OD_ROM.consumerHeartbeatTime[0]},
{0x1017, 0x00, 0x8D,  2, (void*)& BASE_CO_OD_ROM.producerHeartbeatTime},
{0x1018, 0x04, 0x00,  0, (void*)& METER_OD_record1018},
//{0x1019, 0x00, 0x0D,  1, (void*)& METER_CO_OD_ROM.synchronousCounterOverflowValue},
//{0x1029, 0x06, 0x0D,  1, (void*)& METER_CO_OD_ROM.errorBehavior[0]},
//{0x1200, 0x02, 0x00,  0, (void*)& METER_OD_record1200},

{0x1800, 0x06, 0x00,  0, (void*)& BASE_OD_record1800},
{0x1801, 0x06, 0x00,  0, (void*)& BASE_OD_record1801},

{0x1A00, 0x08, 0x00,  0, (void*)& METER_OD_record1A00},
{0x1A01, 0x08, 0x00,  0, (void*)& METER_OD_record1A01},

{0x2000, 0x05, 0x00,  0, (void*)& METER_OD_record2000},
};
