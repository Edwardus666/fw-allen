#ifndef __PMS_OD_H_
#define __PMS_OD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "Od.h"

	#define PMS_OD_SIZE 30
	extern int PmsOd_GetSize();


	/*******************************************************************************
	   TYPE DEFINITIONS FOR RECORDS
	*******************************************************************************/

	/*2120      */ typedef struct {
		UNSIGNED8      maxSubIndex;
		INTEGER64      I64;
		UNSIGNED64     U64;
		REAL32         R32;
		REAL64         R64;
		DOMAIN         domain;
	}              PMS_OD_testVar_t;

	/*2130      */ typedef struct {
		UNSIGNED8      maxSubIndex;
		VISIBLE_STRING string[30];
		UNSIGNED64     epochTimeBaseMs;
		UNSIGNED32     epochTimeOffsetMs;
	}              PMS_OD_time_t;


	/*******************************************************************************
	   STRUCTURES FOR VARIABLES IN DIFFERENT MEMORY LOCATIONS
	*******************************************************************************/
#define  PMS_CO_OD_FIRST_LAST_WORD     0x55 //Any value from 0x01 to 0xFE. If changed, EEPROM will be reinitialized.

	/***** Structure for RAM variables ********************************************/
	struct sPMS_CO_OD_RAM {
		UNSIGNED32     FirstWord;

		/*1001      */ UNSIGNED8      errorRegister;
		/*1002      */ UNSIGNED32     manufacturerStatusRegister;
		/*1003      */ UNSIGNED32     preDefinedErrorField[8];
		/*1010      */ UNSIGNED32     storeParameters[1];
		/*1011      */ UNSIGNED32     restoreDefaultParameters[1];
		/*2100      */ OCTET_STRING   errorStatusBits[10];
		/*2103      */ UNSIGNED16     SYNCCounter;
		/*2104      */ UNSIGNED16     SYNCTime;
		/*2130      */ PMS_OD_time_t      time;

		UNSIGNED32     LastWord;
	};

	/***** Structure for EEPROM variables *****************************************/
	struct sPMS_CO_OD_EEPROM {
		UNSIGNED32     FirstWord;

		/*2106      */ UNSIGNED32     powerOnCounter;
		/*2112      */ INTEGER32      variableNVInt32[16];

		UNSIGNED32     LastWord;
	};


	/***** Structure for ROM variables ********************************************/
	struct sPMS_CO_OD_ROM {
		UNSIGNED32     FirstWord;

		/*1000      */ UNSIGNED32     deviceType;
		/*1005      */ UNSIGNED32     COB_ID_SYNCMessage;
		/*1006      */ UNSIGNED32     communicationCyclePeriod;
		/*1007      */ UNSIGNED32     synchronousWindowLength;
		/*1008      */ VISIBLE_STRING manufacturerDeviceName[11];
		/*1009      */ VISIBLE_STRING manufacturerHardwareVersion[4];
		/*100A      */ VISIBLE_STRING manufacturerSoftwareVersion[10];
		/*1014      */ UNSIGNED32     COB_ID_EMCY;
		/*1015      */ UNSIGNED16     inhibitTimeEMCY;
		/*1016      */ UNSIGNED32     consumerHeartbeatTime[4];
		/*1017      */ UNSIGNED16     producerHeartbeatTime;
		/*1018      */ OD_identity_t  identity;
		/*1019      */ UNSIGNED8      synchronousCounterOverflowValue;
		/*1029      */ UNSIGNED8      errorBehavior[6];
		/*1200[1]   */ OD_SDOServerParameter_t SDOServerParameter[1];
		/*1400[4]   */ OD_RPDOCommunicationParameter_t RPDOCommunicationParameter[4];
		/*1600[4]   */ OD_RPDOMappingParameter_t RPDOMappingParameter[4];
		/*1800[4]   */ OD_TPDOCommunicationParameter_t TPDOCommunicationParameter[1];	//12个TPDO公用一个参数
		/*1A00[4]   */ OD_TPDOMappingParameter_t TPDOMappingParameter[12];
		/*1F80      */ UNSIGNED32     NMTStartup;
		/*2101      */ UNSIGNED8      CANNodeID;
		/*2102      */ UNSIGNED16     CANBitRate;
		/*2111      */ INTEGER32      variableROMInt32[16];

		UNSIGNED32     LastWord;
	};


	/***** Declaration of Object Dictionary variables *****************************/
	extern struct sPMS_CO_OD_RAM PMS_CO_OD_RAM;

	extern struct sPMS_CO_OD_EEPROM PMS_CO_OD_EEPROM;

	extern struct sPMS_CO_OD_ROM PMS_CO_OD_ROM;
	extern const CO_OD_entry_t g_PmsOd[];
	extern const CO_OD_entry_t g_Pms_Od[15][PMS_OD_SIZE];
#ifdef __cplusplus
}
#endif

#endif

