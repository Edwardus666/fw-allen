#ifndef __METER_OD_H_
#define __METER_OD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "Od.h"
#include "BaseOd.h"
	#define METER_OD_SIZE 21


	/*******************************************************************************
	   STRUCTURES FOR VARIABLES IN DIFFERENT MEMORY LOCATIONS
	*******************************************************************************/
#define  METER_CO_OD_FIRST_LAST_WORD     0x55 //Any value from 0x01 to 0xFE. If changed, EEPROM will be reinitialized.

	/***** Structure for RAM variables ********************************************/
	struct sMETER_CO_OD_RAM {
		UNSIGNED32     FirstWord;

		///*1001      */ UNSIGNED8      errorRegister;
		///*1002      */ UNSIGNED32     manufacturerStatusRegister;
		///*1003      */ UNSIGNED32     preDefinedErrorField[8];
		///*1010      */ UNSIGNED32     storeParameters[1];
		///*1011      */ UNSIGNED32     restoreDefaultParameters[1];
		///*2100      */ OCTET_STRING   errorStatusBits[10];
		///*2103      */ UNSIGNED16     SYNCCounter;
		///*2104      */ UNSIGNED16     SYNCTime;
		///*2107      */ UNSIGNED16     performance[5];
		///*2108      */ INTEGER16      temperature[1];
		///*2109      */ INTEGER16      voltage[1];
		///*2110      */ INTEGER32      variableInt32[16];
		///*2120      */ PMS_OD_testVar_t   testVar;
		///*2130      */ PMS_OD_time_t      time;
		///*6000      */ UNSIGNED8      readInput8Bit[8];
		///*6200      */ UNSIGNED8      writeOutput8Bit[8];
		///*6401      */ INTEGER16      readAnalogueInput16Bit[12];
		///*6411      */ INTEGER16      writeAnalogueOutput16Bit[8];

		UNSIGNED32     LastWord;
	};

	/***** Structure for EEPROM variables *****************************************/
	struct sMETER_CO_OD_EEPROM {
		UNSIGNED32     FirstWord;

		/*2106      */ UNSIGNED32     powerOnCounter;
		/*2112      */ INTEGER32      variableNVInt32[16];

		UNSIGNED32     LastWord;
	};


	/***** Structure for ROM variables ********************************************/
	struct sMETER_CO_OD_ROM {
		UNSIGNED32     FirstWord;

		///*1000      */ UNSIGNED32     deviceType;
		///*1005      */ UNSIGNED32     COB_ID_SYNCMessage;
		///*1006      */ UNSIGNED32     communicationCyclePeriod;
		///*1007      */ UNSIGNED32     synchronousWindowLength;
		/*1008      */ VISIBLE_STRING manufacturerDeviceName[8];
		/*1009      */ VISIBLE_STRING manufacturerHardwareVersion[4];
		/*100A      */ VISIBLE_STRING manufacturerSoftwareVersion[12];
		///*1014      */ UNSIGNED32     COB_ID_EMCY;
		///*1015      */ UNSIGNED16     inhibitTimeEMCY;
		///*1016      */ UNSIGNED32     consumerHeartbeatTime[4];
		///*1017      */ UNSIGNED16     producerHeartbeatTime;
		/*1018      */ OD_identity_t  identity;
		///*1019      */ UNSIGNED8      synchronousCounterOverflowValue;
		///*1029      */ UNSIGNED8      errorBehavior[6];
		///*1200[1]   */ OD_SDOServerParameter_t SDOServerParameter[1];
		///*1400[4]   */ OD_RPDOCommunicationParameter_t RPDOCommunicationParameter[4];
//		/*1600[4]   */ OD_RPDOMappingParameter_t RPDOMappingParameter[4];
		///*1800[4]   */ OD_TPDOCommunicationParameter_t TPDOCommunicationParameter[4];
		/*1A00[4]   */ OD_TPDOMappingParameter_t TPDOMappingParameter[2];
		///*1F80      */ UNSIGNED32     NMTStartup;
		///*2101      */ UNSIGNED8      CANNodeID;
		///*2102      */ UNSIGNED16     CANBitRate;
		///*2111      */ INTEGER32      variableROMInt32[16];

		UNSIGNED32     LastWord;
	};


	/***** Declaration of Object Dictionary variables *****************************/
	extern struct sMETER_CO_OD_RAM METER_CO_OD_RAM;

	extern struct sMETER_CO_OD_EEPROM METER_CO_OD_EEPROM;

	extern struct sMETER_CO_OD_ROM METER_CO_OD_ROM;
	extern const CO_OD_entry_t g_MeterOd[];
#ifdef __cplusplus
}
#endif

#endif

