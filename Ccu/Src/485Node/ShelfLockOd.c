#include "Common.h"
#include "BaseOd.h"
#include "ShelfLockOd.h"

/*******************************************************************************
   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

/***** Definition for RAM variables *******************************************/
struct sSHELF_LOCK_OD_OD_RAM SHELF_LOCK_OD_OD_RAM = {
		   SHELF_LOCK_OD_FIRST_LAST_WORD,

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

					  SHELF_LOCK_OD_FIRST_LAST_WORD,
};

/***** Definition for EEPROM variables ****************************************/
//struct sSHELF_LOCK_OD_OD_EEPROM SHELF_LOCK_OD_OD_EEPROM = {
//		   SHELF_LOCK_OD_FIRST_LAST_WORD,
//
//		   /*2106*/ 0x0L,
//		   /*2112*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
//
//					  SHELF_LOCK_OD_FIRST_LAST_WORD,
//};

/***** Definition for ROM variables *******************************************/
#define PMS_EVENT_TIME 0 //0xEA60
struct sSHELF_LOCK_OD_ROM SHELF_LOCK_OD_ROM = {    //constant variables, stored in flash
		SHELF_LOCK_OD_FIRST_LAST_WORD,

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

		/*1600*/{{0x5, 0x20000108L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},

		///*1800*/{{0x6, CO_CAN_ID_TPDO_1 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}, 
		///*1801*/ {0x6, CO_CAN_ID_TPDO_2 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
		///*1802*/ {0x6, CO_CAN_ID_TPDO_3 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0},
		///*1803*/ {0x6, CO_CAN_ID_TPDO_4 , 0xFE, 0x64, 0x0, PMS_EVENT_TIME, 0x0}},

		/*1A00*/{{0x6, 0x20000108L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},

///*1F80*/ 0x0L,
///*2101*/ 0x30,
///*2102*/ 0xFA,
///*2111*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},

				   SHELF_LOCK_OD_FIRST_LAST_WORD
};

/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
/*0x1018*/ const CO_OD_entryRecord_t SHELF_LOCK_OD_record1018[5] = {
		   {(void*)& SHELF_LOCK_OD_ROM.identity.maxSubIndex, 0x05,  1},
		   {(void*)& SHELF_LOCK_OD_ROM.identity.vendorID, 0x85,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.identity.productCode, 0x85,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.identity.revisionNumber, 0x85,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.identity.serialNumber, 0x85,  4} };

/*0x1600*/ const CO_OD_entryRecord_t SHELF_LOCK_OD_record1600[9] = {
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject1, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject2, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject3, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject4, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject5, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject6, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject7, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.RPDOMappingParameter[0].mappedObject8, 0x8D,  4} };


/*0x1A00*/ const CO_OD_entryRecord_t SHELF_LOCK_OD_record1A00[9] = {
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8D,  4},
		   {(void*)& SHELF_LOCK_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8D,  4} };


const static uint8_t   record2000_numberOfMappedObjects = 1;
extern uint8  g_lockState;
/*0x2000*/ const CO_OD_entryRecord_t SHELF_LOCK_OD_record2000[2] = {
		   {(void*)& record2000_numberOfMappedObjects, 0x0D,  1},
		   {(void*)& g_lockState, 0x8E,  1} };
/**************************************************************************************************/

/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
const CO_OD_entry_t g_ShelfLockOd[SHELF_LOCK_OD_SIZE] = {
//{0x1000, 0x00, 0x85,  4, (void*)& SHELF_LOCK_OD_ROM.deviceType},
//{0x1001, 0x00, 0x36,  1, (void*)& SHELF_LOCK_OD_OD_RAM.errorRegister},
//{0x1002, 0x00, 0xB6,  4, (void*)& SHELF_LOCK_OD_OD_RAM.manufacturerStatusRegister},
//{0x1003, 0x08, 0x8E,  4, (void*)& SHELF_LOCK_OD_OD_RAM.preDefinedErrorField[0]},
//{0x1005, 0x00, 0x8D,  4, (void*)& SHELF_LOCK_OD_ROM.COB_ID_SYNCMessage},
//{0x1006, 0x00, 0x8D,  4, (void*)& SHELF_LOCK_OD_ROM.communicationCyclePeriod},
//{0x1007, 0x00, 0x8D,  4, (void*)& SHELF_LOCK_OD_ROM.synchronousWindowLength},
{0x1008, 0x00, 0x05, 11, (void*)& SHELF_LOCK_OD_ROM.manufacturerDeviceName[0]},
{0x1009, 0x00, 0x05,  4, (void*)& SHELF_LOCK_OD_ROM.manufacturerHardwareVersion[0]},
{0x100A, 0x00, 0x05, 10, (void*)& SHELF_LOCK_OD_ROM.manufacturerSoftwareVersion[0]},
//{0x1010, 0x01, 0x8E,  4, (void*)& SHELF_LOCK_OD_OD_RAM.storeParameters[0]},
//{0x1011, 0x01, 0x8E,  4, (void*)& SHELF_LOCK_OD_OD_RAM.restoreDefaultParameters[0]},
//{0x1014, 0x00, 0x85,  4, (void*)& SHELF_LOCK_OD_ROM.COB_ID_EMCY},
//{0x1015, 0x00, 0x8D,  2, (void*)& SHELF_LOCK_OD_ROM.inhibitTimeEMCY},
//{0x1016, 0x04, 0x8D,  4, (void*)& SHELF_LOCK_OD_ROM.consumerHeartbeatTime[0]},
{0x1017, 0x00, 0x8D,  2, (void*)& BASE_CO_OD_ROM.producerHeartbeatTime},
{0x1018, 0x04, 0x00,  0, (void*)& SHELF_LOCK_OD_record1018},
//{0x1019, 0x00, 0x0D,  1, (void*)& SHELF_LOCK_OD_ROM.synchronousCounterOverflowValue},
//{0x1029, 0x06, 0x0D,  1, (void*)& SHELF_LOCK_OD_ROM.errorBehavior[0]},
//{0x1200, 0x02, 0x00,  0, (void*)& SHELF_LOCK_OD_record1200},

{0x1400, 0x02, 0x00,  0, (void*)& BASE_OD_record1400},

{0x1600, 0x08, 0x00,  0, (void*)& SHELF_LOCK_OD_record1600},

{0x1800, 0x06, 0x00,  0, (void*)& BASE_OD_record1800},

{0x1A00, 0x08, 0x00,  0, (void*)& SHELF_LOCK_OD_record1A00},

{0x2000, 0x02, 0x00,  0, (void*)& SHELF_LOCK_OD_record2000},
};
