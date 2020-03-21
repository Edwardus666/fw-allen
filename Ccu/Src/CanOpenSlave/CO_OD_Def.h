/*
 * CANopen Object Dictionary.
 *
 * This file was automatically generated with CANopenNode Object
 * Dictionary Editor. DON'T EDIT THIS FILE MANUALLY !!!!
 * Object Dictionary Editor is currently an older, but functional web
 * application. For more info see See 'Object_Dictionary_Editor/about.html' in
 * <http://sourceforge.net/p/canopennode/code_complete/ci/master/tree/>
 * For more information on CANopen Object Dictionary see <CO_SDO.h>.
 *
 * @file        CO_OD.h
 * @author      Janez Paternoster
 * @copyright   2010 - 2016 Janez Paternoster
 *
 * This file is part of CANopenNode, an opensource CANopen Stack.
 * Project home page is <https://github.com/CANopenNode/CANopenNode>.
 * For more information on CANopen see <http://www.can-cia.org/>.
 *
 * CANopenNode is free and open source software: you can redistribute
 * it and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Following clarification and special exception to the GNU General Public
 * License is included to the distribution terms of CANopenNode:
 *
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library. Thus, the terms and
 * conditions of the GNU General Public License cover the whole combination.
 *
 * As a special exception, the copyright holders of this library give
 * you permission to link this library with independent modules to
 * produce an executable, regardless of the license terms of these
 * independent modules, and to copy and distribute the resulting
 * executable under terms of your choice, provided that you also meet,
 * for each linked independent module, the terms and conditions of the
 * license of that module. An independent module is a module which is
 * not derived from or based on this library. If you modify this
 * library, you may extend this exception to your version of the
 * library, but you are not obliged to do so. If you do not wish
 * to do so, delete this exception statement from your version.
 */


#ifndef CO_OD_DEF_H
#define CO_OD_DEF_H

#include "typedef.h"

/*******************************************************************************
   CANopen DATA DYPES
*******************************************************************************/
   typedef uint8      UNSIGNED8;
   typedef uint16     UNSIGNED16;
   typedef uint32     UNSIGNED32;
   typedef uint64     UNSIGNED64;
   typedef int8       INTEGER8;
   typedef int16      INTEGER16;
   typedef int32      INTEGER32;
   typedef int64      INTEGER64;
   typedef float		REAL32;
   typedef long double    REAL64;
   typedef char			 VISIBLE_STRING;
   typedef unsigned char      OCTET_STRING;
   typedef unsigned char     DOMAIN;

    typedef unsigned char           bool_t;     /**< bool_t */
    typedef float                   float32_t;  /**< float32_t */
    typedef long double             float64_t;  /**< float64_t */
    typedef char                    char_t;     /**< char_t */
    typedef unsigned char           oChar_t;    /**< oChar_t */
    typedef unsigned char           domain_t;   /**< domain_t */


/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/

/*******************************************************************************
   TYPE DEFINITIONS FOR RECORDS
*******************************************************************************/
/*1018      */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     vendorID;
               UNSIGNED32     productCode;
               UNSIGNED32     revisionNumber;
               UNSIGNED32     serialNumber[3];
               }              OD_identity_t;

/*1200[1]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDClientToServer;
               UNSIGNED32     COB_IDServerToClient;
               }              OD_SDOServerParameter_t;

/*1400[4]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDUsedByRPDO;
               UNSIGNED8      transmissionType;
               }              OD_RPDOCommunicationParameter_t;

/*1600[4]   */ typedef struct{
               UNSIGNED8      numberOfMappedObjects;
               UNSIGNED32     mappedObject1;
               UNSIGNED32     mappedObject2;
               UNSIGNED32     mappedObject3;
               UNSIGNED32     mappedObject4;
               UNSIGNED32     mappedObject5;
               UNSIGNED32     mappedObject6;
               UNSIGNED32     mappedObject7;
               UNSIGNED32     mappedObject8;
               }              OD_RPDOMappingParameter_t;

/*1800[4]   */ typedef struct{
               UNSIGNED8      maxSubIndex;
               UNSIGNED32     COB_IDUsedByTPDO;
               UNSIGNED8      transmissionType;
               UNSIGNED16     inhibitTime;
               UNSIGNED8      compatibilityEntry;
               UNSIGNED16     eventTimer;
               UNSIGNED8      SYNCStartValue;
               }              OD_TPDOCommunicationParameter_t;

/*1A00[4]   */ typedef struct{
               UNSIGNED8      numberOfMappedObjects;
               UNSIGNED32     mappedObject1;
               UNSIGNED32     mappedObject2;
               UNSIGNED32     mappedObject3;
               UNSIGNED32     mappedObject4;
               UNSIGNED32     mappedObject5;
               UNSIGNED32     mappedObject6;
               UNSIGNED32     mappedObject7;
               UNSIGNED32     mappedObject8;
               }              OD_TPDOMappingParameter_t;


/*******************************************************************************
   STRUCTURES FOR VARIABLES IN DIFFERENT MEMORY LOCATIONS
*******************************************************************************/
#define  CO_OD_FIRST_LAST_WORD     0x55 //Any value from 0x01 to 0xFE. If changed, EEPROM will be reinitialized.

/**
 * Default CANopen identifiers.
 *
 * Default CANopen identifiers for CANopen communication objects. Same as
 * 11-bit addresses of CAN messages. These are default identifiers and
 * can be changed in CANopen. Especially PDO identifiers are confgured
 * in PDO linking phase of the CANopen network configuration.
 */
typedef enum {
	CO_CAN_ID_NMT_SERVICE = 0x000,   /**< 0x000, Network management */
	CO_CAN_ID_SYNC = 0x080,   /**< 0x080, Synchronous message */
	CO_CAN_ID_EMERGENCY = 0x080,   /**< 0x080, Emergency messages (+nodeID) */
	CO_CAN_ID_TIME_STAMP = 0x100,   /**< 0x100, Time stamp message */

	CO_CAN_ID_TPDO_1 = 0x180,   /**< 0x180, Default TPDO1 (+nodeID) */
	CO_CAN_ID_TPDO_2 = 0x1C0,   /**< 0x180, Default TPDO1 (+nodeID) */

	CO_CAN_ID_RPDO_1 = 0x200,   /**< 0x200, Default RPDO1 (+nodeID) */

	CO_CAN_ID_TPDO_3 = 0x240,   /**< 0x200, Default RPDO1 (+nodeID) */
	CO_CAN_ID_TPDO_4 = 0x280,   /**< 0x280, Default TPDO2 (+nodeID) */
	CO_CAN_ID_TPDO_5 = 0x2C0,   /**< 0x280, Default TPDO2 (+nodeID) */

	CO_CAN_ID_RPDO_2 = 0x300,   /**< 0x300, Default RPDO2 (+nodeID) */

	CO_CAN_ID_TPDO_6 = 0x340,   /**< 0x380, Default TPDO3 (+nodeID) */
	CO_CAN_ID_TPDO_7 = 0x380,   /**< 0x380, Default TPDO3 (+nodeID) */
	CO_CAN_ID_TPDO_8 = 0x3C0,   /**< 0x380, Default TPDO3 (+nodeID) */

	CO_CAN_ID_RPDO_3 = 0x400,   /**< 0x400, Default RPDO3 (+nodeID) */

	CO_CAN_ID_TPDO_9 = 0x440,   /**< 0x480, Default TPDO4 (+nodeID) */
	CO_CAN_ID_TPDO_10 = 0x480,   /**< 0x480, Default TPDO4 (+nodeID) */
	CO_CAN_ID_TPDO_11 = 0x4C0,   /**< 0x480, Default TPDO4 (+nodeID) */

	CO_CAN_ID_RPDO_4 = 0x500,   /**< 0x500, Default RPDO5 (+nodeID) */

	CO_CAN_ID_TPDO_12 = 0x540,   /**< 0x480, Default TPDO4 (+nodeID) */

	CO_CAN_ID_TSDO = 0x580,   /**< 0x580, SDO response from server (+nodeID) */
	CO_CAN_ID_RSDO = 0x600,   /**< 0x600, SDO request from client (+nodeID) */
	CO_CAN_ID_HEARTBEAT = 0x700    /**< 0x700, Heartbeat message */
}CO_Default_CAN_ID_t;

/**
 * Object Dictionary attributes. Bit masks for attribute in CO_OD_entry_t.
 */
typedef enum{
    CO_ODA_MEM_ROM          = 0x0001U,  /**< Variable is located in ROM memory */
    CO_ODA_MEM_RAM          = 0x0002U,  /**< Variable is located in RAM memory */
    CO_ODA_MEM_EEPROM       = 0x0003U,  /**< Variable is located in EEPROM memory */
    CO_ODA_READABLE         = 0x0004U,  /**< SDO server may read from the variable */
    CO_ODA_WRITEABLE        = 0x0008U,  /**< SDO server may write to the variable */
    CO_ODA_RPDO_MAPABLE     = 0x0010U,  /**< Variable is mappable for RPDO */
    CO_ODA_TPDO_MAPABLE     = 0x0020U,  /**< Variable is mappable for TPDO */
    CO_ODA_TPDO_DETECT_COS  = 0x0040U,  /**< If variable is mapped to any PDO, then
                                             PDO is automatically send, if variable
                                             changes its value */
    CO_ODA_MB_VALUE         = 0x0080U   /**< True when variable is a multibyte value */
}CO_SDO_OD_attributes_t;


/**
 * Common DS301 object dictionary entries.
 */
typedef enum{
    OD_H1000_DEV_TYPE             = 0x1000U,/**< Device type */
    OD_H1001_ERR_REG              = 0x1001U,/**< Error register */
    OD_H1002_MANUF_STATUS_REG     = 0x1002U,/**< Manufacturer status register */
    OD_H1003_PREDEF_ERR_FIELD     = 0x1003U,/**< Predefined error field */
    OD_H1004_RSV                  = 0x1004U,/**< Reserved */
    OD_H1005_COBID_SYNC           = 0x1005U,/**< Sync message cob-id */
    OD_H1006_COMM_CYCL_PERIOD     = 0x1006U,/**< Communication cycle period */
    OD_H1007_SYNC_WINDOW_LEN      = 0x1007U,/**< Sync windows length */
    OD_H1008_MANUF_DEV_NAME       = 0x1008U,/**< Manufacturer device name */
    OD_H1009_MANUF_HW_VERSION     = 0x1009U,/**< Manufacturer hardware version */
    OD_H100A_MANUF_SW_VERSION     = 0x100AU,/**< Manufacturer software version */
    OD_H100B_RSV                  = 0x100BU,/**< Reserved */
    OD_H100C_GUARD_TIME           = 0x100CU,/**< Guard time */
    OD_H100D_LIFETIME_FACTOR      = 0x100DU,/**< Life time factor */
    OD_H100E_RSV                  = 0x100EU,/**< Reserved */
    OD_H100F_RSV                  = 0x100FU,/**< Reserved */
    OD_H1010_STORE_PARAM_FUNC     = 0x1010U,/**< Store parameter in persistent memory function */
    OD_H1011_REST_PARAM_FUNC      = 0x1011U,/**< Restore default parameter function */
    OD_H1012_COBID_TIME           = 0x1012U,/**< Timestamp message cob-id */
    OD_H1013_HIGH_RES_TIMESTAMP   = 0x1013U,/**< High resolution timestamp */
    OD_H1014_COBID_EMERGENCY      = 0x1014U,/**< Emergency message cob-id */
    OD_H1015_INHIBIT_TIME_MSG     = 0x1015U,/**< Inhibit time message */
    OD_H1016_CONSUMER_HB_TIME     = 0x1016U,/**< Consumer heartbeat time */
    OD_H1017_PRODUCER_HB_TIME     = 0x1017U,/**< Producer heartbeat time */
    OD_H1018_IDENTITY_OBJECT      = 0x1018U,/**< Identity object */
    OD_H1019_SYNC_CNT_OVERFLOW    = 0x1019U,/**< Sync counter overflow value */
    OD_H1020_VERIFY_CONFIG        = 0x1020U,/**< Verify configuration */
    OD_H1021_STORE_EDS            = 0x1021U,/**< Store EDS */
    OD_H1022_STORE_FORMAT         = 0x1022U,/**< Store format */
    OD_H1023_OS_CMD               = 0x1023U,/**< OS command */
    OD_H1024_OS_CMD_MODE          = 0x1024U,/**< OS command mode */
    OD_H1025_OS_DBG_INTERFACE     = 0x1025U,/**< OS debug interface */
    OD_H1026_OS_PROMPT            = 0x1026U,/**< OS prompt */
    OD_H1027_MODULE_LIST          = 0x1027U,/**< Module list */
    OD_H1028_EMCY_CONSUMER        = 0x1028U,/**< Emergency consumer object */
    OD_H1029_ERR_BEHAVIOR         = 0x1029U,/**< Error behaviour */
    OD_H1200_SDO_SERVER_PARAM     = 0x1200U,/**< SDO server parameters */
    OD_H1280_SDO_CLIENT_PARAM     = 0x1280U,/**< SDO client parameters */

    OD_H1400_RXPDO_1_PARAM        = 0x1400U,/**< RXPDO communication parameter */
    OD_H1401_RXPDO_2_PARAM        = 0x1401U,/**< RXPDO communication parameter */
    OD_H1402_RXPDO_3_PARAM        = 0x1402U,/**< RXPDO communication parameter */
    OD_H1403_RXPDO_4_PARAM        = 0x1403U,/**< RXPDO communication parameter */

    OD_H1600_RXPDO_1_MAPPING      = 0x1600U,/**< RXPDO mapping parameters */
    OD_H1601_RXPDO_2_MAPPING      = 0x1601U,/**< RXPDO mapping parameters */
    OD_H1602_RXPDO_3_MAPPING      = 0x1602U,/**< RXPDO mapping parameters */
    OD_H1603_RXPDO_4_MAPPING      = 0x1603U,/**< RXPDO mapping parameters */

    OD_H1800_TXPDO_1_PARAM        = 0x1800U,/**< TXPDO communication parameter */
    OD_H1801_TXPDO_2_PARAM        = 0x1801U,/**< TXPDO communication parameter */
    OD_H1802_TXPDO_3_PARAM        = 0x1802U,/**< TXPDO communication parameter */
    OD_H1803_TXPDO_4_PARAM        = 0x1803U,/**< TXPDO communication parameter */
    OD_H1804_TXPDO_5_PARAM        = 0x1804U,/**< TXPDO communication parameter */
    OD_H1805_TXPDO_6_PARAM        = 0x1805U,/**< TXPDO communication parameter */
    OD_H1806_TXPDO_7_PARAM        = 0x1806U,/**< TXPDO communication parameter */
    OD_H1807_TXPDO_8_PARAM        = 0x1807U,/**< TXPDO communication parameter */
    OD_H1808_TXPDO_9_PARAM        = 0x1808U,/**< TXPDO communication parameter */
	OD_H1809_TXPDO_10_PARAM		  = 0x1809U,/**< TXPDO communication parameter */
	OD_H180A_TXPDO_11_PARAM       = 0x180AU,/**< TXPDO communication parameter */
	OD_H180B_TXPDO_12_PARAM       = 0x180BU,/**< TXPDO communication parameter */

	OD_H1A00_TXPDO_1_MAPPING      = 0x1A00U,/**< TXPDO mapping parameters */
    OD_H1A01_TXPDO_2_MAPPING      = 0x1A01U,/**< TXPDO mapping parameters */
    OD_H1A02_TXPDO_3_MAPPING      = 0x1A02U,/**< TXPDO mapping parameters */
    OD_H1A03_TXPDO_4_MAPPING      = 0x1A03U, /**< TXPDO mapping parameters */
	OD_H1A04_TXPDO_5_MAPPING      = 0x1A04U,/**< TXPDO mapping parameters */
    OD_H1A05_TXPDO_6_MAPPING      = 0x1A05U,/**< TXPDO mapping parameters */
    OD_H1A06_TXPDO_7_MAPPING      = 0x1A06U,/**< TXPDO mapping parameters */
    OD_H1A07_TXPDO_8_MAPPING      = 0x1A07U, /**< TXPDO mapping parameters */
	OD_H1A08_TXPDO_9_MAPPING      = 0x1A08U,/**< TXPDO mapping parameters */
    OD_H1A0A_TXPDO_10_MAPPING     = 0x1A09U,/**< TXPDO mapping parameters */
    OD_H1A0B_TXPDO_11_MAPPING     = 0x1A010U,/**< TXPDO mapping parameters */
    OD_H1A0C_TXPDO_12_MAPPING     = 0x1A011U /**< TXPDO mapping parameters */
}CO_ObjDicId_t;


/**
 * Bit masks for flags associated with variable from @ref CO_SDO_objectDictionary.
 *
 * This additional functionality of any variable in @ref CO_SDO_objectDictionary can be
 * enabled by function CO_OD_configure(). Location of the flag byte can be
 * get from function CO_OD_getFlagsPointer().
 */
typedef enum{
    /** Variable was written by RPDO. Flag can be cleared by application */
    CO_ODFL_RPDO_WRITTEN        = 0x01U,
    /** Variable is mapped to TPDO */
    CO_ODFL_TPDO_MAPPED         = 0x02U,
    /** Change of state bit, initially copy of attribute from CO_OD_entry_t.
    If set and variable is mapped to TPDO, TPDO will be automatically send,
    if variable changed */
    CO_ODFL_TPDO_COS_ENABLE     = 0x04U,
    /** PDO send bit, can be set by application. If variable is mapped into
    TPDO, TPDO will be send and bit will be cleared. */
    CO_ODFL_TPDO_SEND           = 0x08U,
    /** Variable was accessed by SDO download */
    CO_ODFL_SDO_DOWNLOADED      = 0x10U,
    /** Variable was accessed by SDO upload */
    CO_ODFL_SDO_UPLOADED        = 0x20U,
    /** Reserved */
    CO_ODFL_BIT_6               = 0x40U,
    /** Reserved */
    CO_ODFL_BIT_7               = 0x80U
}CO_SDO_OD_flags_t;


/**
 * Internal states of the SDO server state machine
 */
typedef enum {
    CO_SDO_ST_IDLE                  = 0x00U,
    CO_SDO_ST_DOWNLOAD_INITIATE     = 0x11U,
    CO_SDO_ST_DOWNLOAD_SEGMENTED    = 0x12U,
    CO_SDO_ST_DOWNLOAD_BL_INITIATE  = 0x14U,
    CO_SDO_ST_DOWNLOAD_BL_SUBBLOCK  = 0x15U,
    CO_SDO_ST_DOWNLOAD_BL_SUB_RESP  = 0x16U,
    CO_SDO_ST_DOWNLOAD_BL_END       = 0x17U,
    CO_SDO_ST_UPLOAD_INITIATE       = 0x21U,
    CO_SDO_ST_UPLOAD_SEGMENTED      = 0x22U,
    CO_SDO_ST_UPLOAD_BL_INITIATE    = 0x24U,
    CO_SDO_ST_UPLOAD_BL_INITIATE_2  = 0x25U,
    CO_SDO_ST_UPLOAD_BL_SUBBLOCK    = 0x26U,
    CO_SDO_ST_UPLOAD_BL_END         = 0x27U
} CO_SDO_state_t;

/**
 * SDO abort codes.
 *
 * Send with Abort SDO transfer message.
 *
 * The abort codes not listed here are reserved.
 */
typedef enum {
	CO_SDO_AB_NONE                  = 0x00000000UL, /**< 0x00000000, No abort */

	SDO_FALIED,
	SDO_STATE_ERROR,
	SDO_BUFF_INIT,					//SDO buffer 初始化成功，

    CO_SDO_AB_TOGGLE_BIT            = 0x05030000UL, /**< 0x05030000, Toggle bit not altered */
    CO_SDO_AB_TIMEOUT               = 0x05040000UL, /**< 0x05040000, SDO protocol timed out */
    CO_SDO_AB_CMD                   = 0x05040001UL, /**< 0x05040001, Command specifier not valid or unknown */
    CO_SDO_AB_BLOCK_SIZE            = 0x05040002UL, /**< 0x05040002, Invalid block dataLength in block mode */
    CO_SDO_AB_SEQ_NUM               = 0x05040003UL, /**< 0x05040003, Invalid sequence number in block mode */
    CO_SDO_AB_CRC                   = 0x05040004UL, /**< 0x05040004, CRC error (block mode only) */
    CO_SDO_AB_OUT_OF_MEM            = 0x05040005UL, /**< 0x05040005, Out of memory */
    CO_SDO_AB_UNSUPPORTED_ACCESS    = 0x06010000UL, /**< 0x06010000, Unsupported access to an object */
    CO_SDO_AB_WRITEONLY             = 0x06010001UL, /**< 0x06010001, Attempt to read a write only object */
    CO_SDO_AB_READONLY              = 0x06010002UL, /**< 0x06010002, Attempt to write a read only object */
    CO_SDO_AB_NOT_EXIST             = 0x06020000UL, /**< 0x06020000, Object does not exist */
    CO_SDO_AB_NO_MAP                = 0x06040041UL, /**< 0x06040041, Object cannot be mapped to the PDO */
    CO_SDO_AB_MAP_LEN               = 0x06040042UL, /**< 0x06040042, Number and length of object to be mapped exceeds PDO length */
    CO_SDO_AB_PRAM_INCOMPAT         = 0x06040043UL, /**< 0x06040043, General parameter incompatibility reasons */
    CO_SDO_AB_DEVICE_INCOMPAT       = 0x06040047UL, /**< 0x06040047, General internal incompatibility in device */
    CO_SDO_AB_HW                    = 0x06060000UL, /**< 0x06060000, Access failed due to hardware error */
    CO_SDO_AB_TYPE_MISMATCH         = 0x06070010UL, /**< 0x06070010, Data type does not match, length of service parameter does not match */
    CO_SDO_AB_DATA_LONG             = 0x06070012UL, /**< 0x06070012, Data type does not match, length of service parameter too high */
    CO_SDO_AB_DATA_SHORT            = 0x06070013UL, /**< 0x06070013, Data type does not match, length of service parameter too short */
    CO_SDO_AB_SUB_UNKNOWN           = 0x06090011UL, /**< 0x06090011, Sub index does not exist */
    CO_SDO_AB_INVALID_VALUE         = 0x06090030UL, /**< 0x06090030, Invalid value for parameter (download only). */
    CO_SDO_AB_VALUE_HIGH            = 0x06090031UL, /**< 0x06090031, Value range of parameter written too high */
    CO_SDO_AB_VALUE_LOW             = 0x06090032UL, /**< 0x06090032, Value range of parameter written too low */
    CO_SDO_AB_MAX_LESS_MIN          = 0x06090036UL, /**< 0x06090036, Maximum value is less than minimum value. */
    CO_SDO_AB_NO_RESOURCE           = 0x060A0023UL, /**< 0x060A0023, Resource not available: SDO connection */
    CO_SDO_AB_GENERAL               = 0x08000000UL, /**< 0x08000000, General error */
    CO_SDO_AB_DATA_TRANSF           = 0x08000020UL, /**< 0x08000020, Data cannot be transferred or stored to application */
    CO_SDO_AB_DATA_LOC_CTRL         = 0x08000021UL, /**< 0x08000021, Data cannot be transferred or stored to application because of local control */
    CO_SDO_AB_DATA_DEV_STATE        = 0x08000022UL, /**< 0x08000022, Data cannot be transferred or stored to application because of present device state */
    CO_SDO_AB_DATA_OD               = 0x08000023UL, /**< 0x08000023, Object dictionary not present or dynamic generation fails */
    CO_SDO_AB_NO_DATA               = 0x08000024UL  /**< 0x08000024, No data available */
}CO_SDO_abortCode_t, SDO_ERR;

/**
 * Object for one entry with specific index in @ref CO_SDO_objectDictionary.
 */
typedef struct {
    /** The index of Object from 0x1000 to 0xFFFF */
    uint16_t            index;
    /** Number of (sub-objects - 1). If Object Type is variable, then
    maxSubIndex is 0, otherwise maxSubIndex is equal or greater than 1. */
    uint8_t             maxSubIndex;
    /** If Object Type is record, attribute is set to zero. Attribute for
    each member is then set in special array with members of type
    CO_OD_entryRecord_t. If Object Type is Array, attribute is common for
    all array members. See #CO_SDO_OD_attributes_t. */
    uint16_t            attribute;
    /** If Object Type is Variable, length is the length of variable in bytes.
    If Object Type is Array, length is the length of one array member.
    If Object Type is Record, length is zero. Length for each member is
    set in special array with members of type CO_OD_entryRecord_t.
    If Object Type is Domain, length is zero. Length is specified
    by application in @ref CO_SDO_OD_function. */
    uint16_t            length;
    /** If Object Type is Variable, pData is pointer to data.
    If Object Type is Array, pData is pointer to data. Data doesn't
    include Sub-Object 0.
    If object type is Record, pData is pointer to special array
    with members of type CO_OD_entryRecord_t.
    If object type is Domain, pData is null. */
    void               *pData;
}CO_OD_entry_t,CO_OD_entry;


/**
 * Object for record type entry in @ref CO_SDO_objectDictionary.
 *
 * See CO_OD_entry_t.
 */
typedef struct{
    /** See #CO_SDO_OD_attributes_t */
    void               *pData;
    /** Length of variable in bytes. If object type is Domain, length is zero */
    uint16_t            attribute;
    /** Pointer to data. If object type is Domain, pData is null */
    uint16_t            length;
}CO_OD_entryRecord_t,CO_OD_entryRecord;


/**
 * Object contains all information about the object being transferred by SDO server.
 *
 * Object is used as an argument to @ref CO_SDO_OD_function. It is also
 * part of the CO_SDO_t object.
 */
typedef struct{
    /** Informative parameter. It may point to object, which is connected
    with this OD entry. It can be used inside @ref CO_SDO_OD_function, ONLY
    if it was registered by CO_OD_configure() function before. */
    void               *object;
    /** SDO data buffer contains data, which are exchanged in SDO transfer.
    @ref CO_SDO_OD_function may verify or manipulate that data before (after)
    they are written to (read from) Object dictionary. Data have the same
    endianes as processor. Pointer must NOT be changed. (Data up to length
    can be changed.) */
    uint8_t            *data;
    /** Pointer to location in object dictionary, where data are stored.
    (informative reference to old data, read only). Data have the same
    endianes as processor. If data type is Domain, this variable is null. */
    uint8         *ODdataStorage;
    /** Length of data in the above buffer. Read only, except for domain. If
    data type is domain see @ref CO_SDO_OD_function for special rules by upload. */
    uint16_t            dataLength;
    /** Attribute of object in Object dictionary (informative, must NOT be changed). */
    uint16_t            attribute;
    /** Pointer to the #CO_SDO_OD_flags_t byte. */
    uint8_t            *pFlags;
    /** Index of object in Object dictionary (informative, must NOT be changed). */
    uint16_t            index;
    /** Subindex of object in Object dictionary (informative, must NOT be changed). */
    uint8_t             subIndex;
    /** True, if SDO upload is in progress, false if SDO download is in progress. */
    bool_t              reading;
    /** Used by domain data type. Indicates the first segment. Variable is informative. */
    bool_t              firstSegment;
    /** Used by domain data type. If false by download, then application will
    receive more segments during SDO communication cycle. If uploading,
    application may set variable to false, so SDO server will call
    @ref CO_SDO_OD_function again for filling the next data. */
    bool_t              lastSegment;
    /** Used by domain data type. By upload @ref CO_SDO_OD_function may write total
    data length, so this information will be send in SDO upload initiate phase. It
    is not necessary to specify this variable. By download this variable contains
    total data dataLength, if dataLength is indicated in SDO download initiate phase */
	//domain data type，用于确定是否定长传输。0-不定长传输； >0定长传输
    uint32_t            dataLengthTotal;

    /** Used by domain data type. In case of multiple segments, this indicates the offset
    into the buffer this segment starts at. */
	//读操作时，已经读取到的数据,在调用pODFunc函数之后改变
	uint32_t            offset;
}CO_ODF_arg_t;

 typedef enum
 {
	 TRANS_DONE = 0,	//SDO传输结束，buff最后收到的数据，buff长度len
	 TRANS_BUF_EMPTY,	//用于SDO DOWNLOAD传输，当sdoBuf为空时触发该事件，sdoBuf：数据指针，len：需要的长度
	 TRANS_BUF_FULL,	//用于SDO UPLOAD传输  ，当sdoBuf为满时触发该事件，sdoBuf：数据指针，len：需要的长度
	 TRANS_GET_TOTAL_LEN,	//获取传输总长度，决定是否进行确定长度传输，如果确定长度，初始化arg->dataLengthTotal

	 TRANS_CHANGE_BEFORE,	//数字字典改变前
	 TRANS_CHANGE_AFTER,	//数字字典改变后
 }SdoTransEventType;
 //Sdo 传输事件函数，有SdoTransEventType定义的几种事件类型会触发该函数
 typedef SDO_ERR(*EntryEventFn)(void* pArgObj, SdoTransEventType eventType, SDO_ERR sdoErr, CO_ODF_arg_t* arg);

/**
 * Object is used as array inside CO_SDO_t, parallel to @ref CO_SDO_objectDictionary.
 *
 * Object is generated by function CO_OD_configure(). It is then used as
 * extension to Object dictionary entry at specific index.
 */
 typedef CO_SDO_abortCode_t(*ODExtenFunc)(CO_ODF_arg_t* ODF_arg);
 typedef struct{
     /** Pointer to @ref CO_SDO_OD_function */
	 ODExtenFunc pODFunc;
     /** Pointer to object, which will be passed to @ref CO_SDO_OD_function */
     void               *object;
     /** Pointer to #CO_SDO_OD_flags_t. If object type is array or record, this
     variable points to array with length equal to number of subindexes. */
     uint8_t            *flags;
 }CO_OD_extension_t;


#endif

