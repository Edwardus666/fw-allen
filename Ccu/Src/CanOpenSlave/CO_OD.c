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
 * @file        CO_OD.c
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

#include "Common.h"
#include "CO_driver.h"
#include "CO_OD.h"
#include "CO_SDO.h"
#include "DriverAdc.h"
#include "PmsMgr.h"
#include "OdGlobal.h"

/*******************************************************************************
   DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

//Allen add;
const uint16 TPDO_CAN_ID[CO_NO_TPDO] = {   CO_CAN_ID_TPDO_1, CO_CAN_ID_TPDO_2
										 , CO_CAN_ID_TPDO_3 , CO_CAN_ID_TPDO_4  
										 , CO_CAN_ID_TPDO_5 , CO_CAN_ID_TPDO_6
										 , CO_CAN_ID_TPDO_7 , CO_CAN_ID_TPDO_8
										 , CO_CAN_ID_TPDO_9 , CO_CAN_ID_TPDO_10 
										 , CO_CAN_ID_TPDO_11, CO_CAN_ID_TPDO_12 };

const uint16 RPDO_CAN_ID[CO_NO_RPDO] = {  CO_CAN_ID_RPDO_1, CO_CAN_ID_RPDO_2
										, CO_CAN_ID_RPDO_3, CO_CAN_ID_RPDO_4};

/***** Definition for RAM variables *******************************************/
struct sCO_OD_RAM CO_OD_RAM = {
           CO_OD_FIRST_LAST_WORD,

/*1001*/ 0x0,
/*1002*/ 0x0L,
/*1003*/ {0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1009*/ {'1', '.', '2', '3'},
/*100A*/ {'1', '.', '0', '1'},
/*1010*/ {0x3L},
/*1011*/ {0x1L},

/*2000*/ {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

/*2001*/ {0x03, 0x00, 0x00, 0x00},
/*2002*/ {0x03, 0x00, 0x00, 0x00},
/*2003*/ {0x03, 0x00, 0x00, 0x00},
/*2004*/ {0x03, 0x00, 0x00, 0x00},

/*2005*/ {0x02, 0x00, 0x00},
/*2007*/ {0x04, 0x00, 0x00, 0x00, 0x00},

/*2008*/ {0x03, 0x00, 0x00, 0x00},
/*2009*/ {0x03, 0x00, 0x00, 0x00},

/*2010*/ {0x02, 0x00, 0x00},
/*2011*/ {0x02, 0x00, 0x00},
/*2012*/ {0x02, 0x00, 0x00},
/*2013*/ {0x02, 0x00, 0x00},
/*2014*/ {0x02, 0x00, 0x00},

/*2020*/ 0x00,

/*2100*/ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
/*2103*/ 0x0,
/*2104*/ 0x0,
/*2107*/ {0x3E8, 0x0, 0x0, 0x0, 0x0},
/*2108*/ {0},
/*2109*/ {0},
/*2110*/ {0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
/*2120*/ {0x5, 0x1234567890ABCDEFLL, 0x234567890ABCDEF1LL, 12.345, 456.789, 0},
/*2130*/ {0x3, {'-', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, 0, 0x0L},
/*6000*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
/*6200*/ {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
/*6401*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
/*6411*/ {0, 0, 0, 0, 0, 0, 0, 0},

           CO_OD_FIRST_LAST_WORD,
};


/***** Definition for EEPROM variables ****************************************/
struct sCO_OD_EEPROM CO_OD_EEPROM = {
           CO_OD_FIRST_LAST_WORD,

/*2106*/ 0x0L,
/*2112*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},
/*301A*/ {3L, 1L, 2L, 3L, 4L, 5L, 6L},
		   CO_OD_FIRST_LAST_WORD,
};


#define CCU_EVENT_TIME 0 //0xEA60
/***** Definition for ROM variables *******************************************/
   struct sCO_OD_ROM CO_OD_ROM = {    //constant variables, stored in flash
           CO_OD_FIRST_LAST_WORD,

/*1000*/ 0x0L,
/*1005*/ 0x80L,
/*1006*/ 0x0L,
/*1007*/ 0x0L,
/*1008*/ {'I', 'M', 'T', '-', 'C', 'C', 'U'},
/*1014*/ 0x80L,
/*1015*/ 0x64,
/*1016*/ {0x0L, 0x0L, 0x0L, 0x0L},
/*1017*/ 0x1388,
/*1018*/ {0x4, 0x0L, 0x0L, 0x0L, 0x0L},
/*1019*/ 0x0,
/*1029*/ {0x0, 0x0, 0x1, 0x0, 0x0, 0x0},
/*1200*/{{0x2, 0x600L, 0x580L}},

/*1400*/{{0x2, 0x200L, 0xFE},
/*1401*/ {0x2, 0x300L, 0xFE},
/*1402*/ {0x2, 0x400L, 0xFE},
/*1403*/ {0x2, 0x500L, 0xFE}},
/*1600*/{{0x2, 0x62000108L, 0x62000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1601*/ {0x1, 0x62000108L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1602*/ {0x1, 0x62000108L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/*1603*/ {0x1, 0x62000108L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},

/*1800*/{{0x6, CO_CAN_ID_TPDO_1 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0}, /*Modified by Kim; set eventimer from 0x3E8 to 0xEA60*/
/*1801*/ {0x6, CO_CAN_ID_TPDO_2 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1802*/ {0x6, CO_CAN_ID_TPDO_3 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1803*/ {0x6, CO_CAN_ID_TPDO_4 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1804*/ {0x6, CO_CAN_ID_TPDO_5 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1805*/ {0x6, CO_CAN_ID_TPDO_6 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1806*/ {0x6, CO_CAN_ID_TPDO_7 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1807*/ {0x6, CO_CAN_ID_TPDO_8 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1808*/ {0x6, CO_CAN_ID_TPDO_9 , 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*1809*/ {0x6, CO_CAN_ID_TPDO_10, 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*180A*/ {0x6, CO_CAN_ID_TPDO_11, 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0},
/*180B*/ {0x6, CO_CAN_ID_TPDO_12, 0xFE, 0x64, 0x0, CCU_EVENT_TIME, 0x0}},

#ifdef CFG_EX_FRAME
/*1A00*/{{0x8, 0x20000108L, 0x20000208L, 0x20000308L, 0x20000408L, 0x20000508L, 0x20000608L, 0x20000708L, 0x20000808L},
/*1A01*/ {0x8, 0x20010108L, 0x20010208L, 0x20010108L, 0x20020208L, 0x20030108L, 0x20030208L, 0x20040108L, 0x20040208L},
/*1A02*/ {0x6, 0x20070110L, 0x20070210L, 0x20070308L, 0x20070408L, 0x20140108L, 0x20140208L, 0x00000000L, 0x00000000L},
/*1A03*/ {0x8, 0x20100108L, 0x20100208L, 0x20110108L, 0x20110208L, 0x20120108L, 0x20120208L, 0x20130108L, 0x20130208L},
/*1A04*/ {0x8, 0x20400108L, 0x20400208L, 0x20400308L, 0x20400408L, 0x20400508L, 0x20400608L, 0x20400708L, 0x20400808L},
/*1A05*/ {0x8, 0x20400908L, 0x20400A08L, 0x20400B08L, 0x20400C08L, 0x20400D08L, 0x20410108L, 0x20410208L, 0x20410308L},
/*1A06*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A07*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A08*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A09*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A0A*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A0B*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L}},
#else
/*1A00*/{{0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A01*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A02*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A03*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A04*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A05*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A06*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A07*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A08*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A09*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A0A*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L},
/*1A0B*/ {0x0, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L}},
#endif

/*1F80*/ 0x0L,
/*2101*/ 0x30,
/*2102*/ 0xFA,
/*2111*/ {1L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L, 0L},

           CO_OD_FIRST_LAST_WORD
};


/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
/*0x1018*/ const CO_OD_entryRecord_t OD_record1018[5] = {
           {(void*)&CO_OD_ROM.identity.maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.identity.vendorID, 0x85,  4},
           {(void*)&CO_OD_ROM.identity.productCode, 0x85,  4},
           {(void*)&CO_OD_ROM.identity.revisionNumber, 0x85,  4},
           {(void*)g_McuID, 0x85,  12}};
/*0x1200*/ const CO_OD_entryRecord_t OD_record1200[3] = {
           {(void*)&CO_OD_ROM.SDOServerParameter[0].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDClientToServer, 0x85,  4},
           {(void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDServerToClient, 0x85,  4}};
/*0x1400*/ const CO_OD_entryRecord_t OD_record1400[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].transmissionType, 0x0D,  1}};
/*0x1401*/ const CO_OD_entryRecord_t OD_record1401[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].transmissionType, 0x0D,  1}};
/*0x1402*/ const CO_OD_entryRecord_t OD_record1402[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].transmissionType, 0x0D,  1}};
/*0x1403*/ const CO_OD_entryRecord_t OD_record1403[3] = {
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[3].COB_IDUsedByRPDO, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOCommunicationParameter[3].transmissionType, 0x0D,  1}};
/*0x1600*/ const CO_OD_entryRecord_t OD_record1600[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject8, 0x8D,  4}};
/*0x1601*/ const CO_OD_entryRecord_t OD_record1601[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject8, 0x8D,  4}};
/*0x1602*/ const CO_OD_entryRecord_t OD_record1602[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject8, 0x8D,  4}};
/*0x1603*/ const CO_OD_entryRecord_t OD_record1603[9] = {
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject1, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject2, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject3, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject4, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject5, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject6, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject7, 0x8D,  4},
           {(void*)&CO_OD_ROM.RPDOMappingParameter[3].mappedObject8, 0x8D,  4}};

/*0x1800*/ const CO_OD_entryRecord_t OD_record1800[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[0].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[0].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[0].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[0].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[0].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[0].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[0].SYNCStartValue		, 0x0D,  1} };
/*0x1801*/ const CO_OD_entryRecord_t OD_record1801[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[1].maxSubIndex, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[1].COB_IDUsedByTPDO, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[1].transmissionType, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[1].inhibitTime, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[1].compatibilityEntry, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[1].eventTimer, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[1].SYNCStartValue, 0x0D,  1} };
/*0x1802*/ const CO_OD_entryRecord_t OD_record1802[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[2].maxSubIndex, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[2].COB_IDUsedByTPDO, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[2].transmissionType, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[2].inhibitTime, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[2].compatibilityEntry, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[2].eventTimer, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[2].SYNCStartValue, 0x0D,  1} };
/*0x1803*/ const CO_OD_entryRecord_t OD_record1803[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[3].maxSubIndex, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[3].COB_IDUsedByTPDO, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[3].transmissionType, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[3].inhibitTime, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[3].compatibilityEntry, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[3].eventTimer, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[3].SYNCStartValue, 0x0D,  1} };
/*0x1804*/ const CO_OD_entryRecord_t OD_record1804[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[4].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[4].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[4].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[4].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[4].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[4].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[4].SYNCStartValue		, 0x0D,  1} };
/*0x1805*/ const CO_OD_entryRecord_t OD_record1805[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[5].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[5].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[5].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[5].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[5].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[5].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[5].SYNCStartValue		, 0x0D,  1} };
/*0x1806*/ const CO_OD_entryRecord_t OD_record1806[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[6].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[6].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[6].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[6].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[6].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[6].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[6].SYNCStartValue		, 0x0D,  1} };
/*0x1807*/ const CO_OD_entryRecord_t OD_record1807[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[7].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[7].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[7].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[7].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[7].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[7].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[7].SYNCStartValue		, 0x0D,  1} };
/*0x1808*/ const CO_OD_entryRecord_t OD_record1808[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[8].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[8].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[8].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[8].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[8].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[8].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[8].SYNCStartValue		, 0x0D,  1} };
/*0x1809*/ const CO_OD_entryRecord_t OD_record1809[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[9].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[9].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[9].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[9].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[9].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[9].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[9].SYNCStartValue		, 0x0D,  1} };
/*0x180A*/ const CO_OD_entryRecord_t OD_record180A[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[10].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[10].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[10].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[10].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[10].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[10].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[10].SYNCStartValue		, 0x0D,  1} };
/*0x180B*/ const CO_OD_entryRecord_t OD_record180B[7] = {
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[11].maxSubIndex		, 0x05,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[11].COB_IDUsedByTPDO	, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[11].transmissionType	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[11].inhibitTime		, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[11].compatibilityEntry	, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[11].eventTimer			, 0x8D,  2},
		   {(void*)& CO_OD_ROM.TPDOCommunicationParameter[11].SYNCStartValue		, 0x0D,  1} };

/*0x1A00*/ const CO_OD_entryRecord_t OD_record1A00[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8D,  4} };
/*0x1A01*/ const CO_OD_entryRecord_t OD_record1A01[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[1].mappedObject8, 0x8D,  4} };
/*0x1A02*/ const CO_OD_entryRecord_t OD_record1A02[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[2].mappedObject8, 0x8D,  4} };
/*0x1A03*/ const CO_OD_entryRecord_t OD_record1A03[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[3].mappedObject8, 0x8D,  4} };
/*0x1A04*/ const CO_OD_entryRecord_t OD_record1A04[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[4].mappedObject8, 0x8D,  4} };
/*0x1A05*/ const CO_OD_entryRecord_t OD_record1A05[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[5].mappedObject8, 0x8D,  4} };
/*0x1A06*/ const CO_OD_entryRecord_t OD_record1A06[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[6].mappedObject8, 0x8D,  4} };
/*0x1A07*/ const CO_OD_entryRecord_t OD_record1A07[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[7].mappedObject8, 0x8D,  4} };
/*0x1A08*/ const CO_OD_entryRecord_t OD_record1A08[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[8].mappedObject8, 0x8D,  4} };
/*0x1A09*/ const CO_OD_entryRecord_t OD_record1A09[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[9].mappedObject8, 0x8D,  4} };
/*0x1A0A*/ const CO_OD_entryRecord_t OD_record1A0A[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[10].mappedObject8, 0x8D,  4} };
/*0x1A0B*/ const CO_OD_entryRecord_t OD_record1A0B[9] = {
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].numberOfMappedObjects, 0x0D,  1},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject1, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject2, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject3, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject4, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject5, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject6, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject7, 0x8D,  4},
		   {(void*)& CO_OD_ROM.TPDOMappingParameter[11].mappedObject8, 0x8D,  4} };

#define USER_ATTRI 0xE6
/*0x2000*/ const CO_OD_entryRecord_t OD_record2000[10] = {
		   {(void*)& CO_OD_RAM.cabinet.maxSubIndex	   , 0x06, 1},
		   {(void*)& CO_OD_RAM.cabinet.shelfWaterIn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.cabinet.BucketWaterLevel, USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.cabinet.lightPowerOn    , USER_ATTRI, 1 },
		   {(void*)& CO_OD_RAM.cabinet.lcdPowerOn      , USER_ATTRI, 1 },
		   {(void*)& CO_OD_RAM.cabinet.ccuTemp1        , USER_ATTRI, 1 },
		   {(void*)& CO_OD_RAM.cabinet.ccuTemp2        , USER_ATTRI, 1 },
		   {(void*)& CO_OD_RAM.cabinet.ccuTemp3        , USER_ATTRI, 1 },
		   {(void*)& CO_OD_RAM.cabinet.ccuTemp4        , USER_ATTRI, 1 },
		   {(void*)& CO_OD_RAM.cabinet.funPowerOn      , USER_ATTRI, 1 },
};

/*0x2001*/ const CO_OD_entryRecord_t OD_record2001[4] = {
		   {(void*)& CO_OD_RAM.Fun1.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.Fun1.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun1.workState  , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun1.funFault   , USER_ATTRI, 1 },
};
/*0x2002*/ const CO_OD_entryRecord_t OD_record2002[4] = {
		   {(void*)& CO_OD_RAM.Fun2.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.Fun2.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun2.workState  , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun2.funFault   , USER_ATTRI, 1 },
};
/*0x2003*/ const CO_OD_entryRecord_t OD_record2003[4] = {
		   {(void*)& CO_OD_RAM.Fun3.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.Fun3.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun3.workState  , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun3.funFault   , USER_ATTRI, 1 },
};
/*0x2004*/ const CO_OD_entryRecord_t OD_record2004[4] = {
		   {(void*)& CO_OD_RAM.Fun4.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.Fun4.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun4.workState  , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.Fun4.funFault   , USER_ATTRI, 1 },
};

/*0x2005*/ const CO_OD_entryRecord_t OD_record2005[3] = {
		   {(void*)& CO_OD_RAM.pump.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.pump.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.pump.Fault      , USER_ATTRI, 1},
};

/*0x2007*/ const CO_OD_entryRecord_t OD_record2007[5] = {
		   {(void*)& CO_OD_RAM._18650Bat.maxSubIndex      , 0x06, 1},
		   {(void*)& CO_OD_RAM._18650Bat.DischargeCurrent , USER_ATTRI, 2},
		   {(void*)& CO_OD_RAM._18650Bat.Voltage          , USER_ATTRI, 2},
		   {(void*)& CO_OD_RAM._18650Bat.Temp             , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM._18650Bat.convertState     , USER_ATTRI, 1},
};

/*0x2010*/ const CO_OD_entryRecord_t OD_record2010[3] = {
		   {(void*)& CO_OD_RAM.relay1.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.relay1.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.relay1.Fault      , USER_ATTRI, 1},
};

/*0x2011*/ const CO_OD_entryRecord_t OD_record2011[3] = {
		   {(void*)& CO_OD_RAM.relay2.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.relay2.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.relay2.Fault      , USER_ATTRI, 1},
};

/*0x2012*/ const CO_OD_entryRecord_t OD_record2012[3] = {
		   {(void*)& CO_OD_RAM.relay3.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.relay3.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.relay3.Fault      , USER_ATTRI, 1},
};

/*0x2013*/ const CO_OD_entryRecord_t OD_record2013[3] = {
		   {(void*)& CO_OD_RAM.relay4.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.relay4.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.relay4.Fault      , USER_ATTRI, 1},
};

/*0x2014*/ const CO_OD_entryRecord_t OD_record2014[3] = {
		   {(void*)& CO_OD_RAM.relay5.maxSubIndex, 0x06, 1},
		   {(void*)& CO_OD_RAM.relay5.powerOn    , USER_ATTRI, 1},
		   {(void*)& CO_OD_RAM.relay5.Fault      , USER_ATTRI, 1},
};

/*0x2120*/ const CO_OD_entryRecord_t OD_record2120[6] = {
           {(void*)&CO_OD_RAM.testVar.maxSubIndex, 0x06,  1},
           {(void*)&CO_OD_RAM.testVar.I64, 0xBE,  8},
           {(void*)&CO_OD_RAM.testVar.U64, 0xBE,  8},
           {(void*)&CO_OD_RAM.testVar.R32, 0xBE,  4},
           {(void*)&CO_OD_RAM.testVar.R64, 0xBE,  8},
           {0, 0x0E,  0}};
/*0x2130*/ const CO_OD_entryRecord_t OD_record2130[4] = {
           {(void*)&CO_OD_RAM.time.maxSubIndex, 0x06,  1},
           {(void*)&CO_OD_RAM.time.string[0], 0x06, 30},
           {(void*)&CO_OD_RAM.time.epochTimeBaseMs, 0x8E,  8},
           {(void*)&CO_OD_RAM.time.epochTimeOffsetMs, 0xBE,  4}};

const static uint8 OD_record2040_maxSubIndex = 13;
/*0x2040*/ const CO_OD_entryRecord_t OD_record2040[14] = {
		   {(void*)& OD_record2040_maxSubIndex, 0x06, 1},
		   {(void*)& g_Pms[0].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[1].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[2].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[3].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[4].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[5].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[6].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[7].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[8].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[9].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[10].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[11].base.state    , USER_ATTRI, 1},
		   {(void*)& g_Pms[12].base.state    , USER_ATTRI, 1},
};

const static MasterNode g_MasterNode;
const static uint8 OD_record2041_maxSubIndex = 4;
/*0x2041*/ const CO_OD_entryRecord_t OD_record2041[5] = {
		   {(void*)& OD_record2041_maxSubIndex, 0x06, 1},
		   {(void*)& g_MasterNode.state    , USER_ATTRI, 1},
		   {(void*)& g_MasterNode.state    , USER_ATTRI, 1},
		   {(void*)& g_MasterNode.state    , USER_ATTRI, 1},
		   {(void*)& g_MasterNode.state    , USER_ATTRI, 1},
};

//CCU fw file record
const static uint8 OD_record2FF0_maxSubIndex = 3;
/*0x2FF0*/ const CO_OD_entryRecord_t OD_record2FF0[4] = {
		   {(void*)& OD_record2FF0_maxSubIndex, 0x06, 1},
		   {(void*)& g_ccuOtaFile.ver.m_HwMainVer , 0x0F, 2},
		   {(void*)& g_ccuOtaFile.ver.m_AppMainVer, 0x0F, 7},
		   {(void*)Null , 0x0F, 0},	//Domain CCU fw file
};

//PMS fw file record
/*0x2FF1*/ const CO_OD_entryRecord_t OD_record2FF1[4] = {
		   {(void*)& OD_record2FF0_maxSubIndex, 0x06, 1},
		   {(void*)& g_pmsOtaFile.ver.m_HwMainVer , 0x0F, 2},
		   {(void*)& g_pmsOtaFile.ver.m_AppMainVer, 0x0F, 7},
		   {(void*)Null , 0x0F, 0},	//Domain PMS fw file
};

//BMS file record
/*0x2FF2*/ const CO_OD_entryRecord_t OD_record2FF2[4] = {
		   {(void*)& OD_record2FF0_maxSubIndex, 0x06, 1},
		   {(void*)& g_bmsOtaFile.ver.m_HwMainVer , 0x0F, 2},
		   {(void*)& g_bmsOtaFile.ver.m_AppMainVer, 0x0F, 7},
		   {(void*)Null , 0x0F, 0},	//Domain BMS fw file
};

//CHG file record
/*0x2FF3*/ const CO_OD_entryRecord_t OD_record2FF3[4] = {
		   {(void*)& OD_record2FF0_maxSubIndex, 0x06, 1},
		   {(void*)& g_chgOtaFile.ver.m_HwMainVer , 0x0F, 2},
		   {(void*)& g_chgOtaFile.ver.m_AppMainVer, 0x0F, 7},
		   {(void*)Null , 0x0F, 0},	//Domain CHG fw file
};

/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
const CO_OD_entry_t CO_OD[CO_OD_NoOfElements] = {
{0x1000, 0x00, 0x85,  4, (void*)&CO_OD_ROM.deviceType},
{0x1001, 0x00, 0x36,  1, (void*)&CO_OD_RAM.errorRegister},
{0x1002, 0x00, 0xB6,  4, (void*)&CO_OD_RAM.manufacturerStatusRegister},
{0x1003, 0x08, 0x8E,  4, (void*)&CO_OD_RAM.preDefinedErrorField[0]},
{0x1005, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.COB_ID_SYNCMessage},
{0x1006, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.communicationCyclePeriod},
{0x1007, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.synchronousWindowLength},
{0x1008, 0x00, 0x0D, 11, (void*)&CO_OD_ROM.manufacturerDeviceName[0]},	//0X05
{0x1009, 0x00, 0x05,  4, (void*)& CO_OD_RAM.manufacturerHardwareVersion[0]},
{0x100A, 0x00, 0x05, 11, (void*)& CO_OD_RAM.manufacturerSoftwareVersion[0]},
{0x1010, 0x01, 0x8E,  4, (void*)&CO_OD_RAM.storeParameters[0]},
{0x1011, 0x01, 0x8E,  4, (void*)&CO_OD_RAM.restoreDefaultParameters[0]},
{0x1014, 0x00, 0x85,  4, (void*)&CO_OD_ROM.COB_ID_EMCY},
{0x1015, 0x00, 0x8D,  2, (void*)&CO_OD_ROM.inhibitTimeEMCY},
{0x1016, 0x04, 0x8D,  4, (void*)&CO_OD_ROM.consumerHeartbeatTime[0]},
{0x1017, 0x00, 0x8D,  2, (void*)&CO_OD_ROM.producerHeartbeatTime},
{0x1018, 0x04, 0x00,  0, (void*)&OD_record1018},
{0x1019, 0x00, 0x0D,  1, (void*)&CO_OD_ROM.synchronousCounterOverflowValue},
{0x1029, 0x06, 0x0D,  1, (void*)&CO_OD_ROM.errorBehavior[0]},
{0x1200, 0x02, 0x00,  0, (void*)&OD_record1200},
{0x1400, 0x02, 0x00,  0, (void*)&OD_record1400},
{0x1401, 0x02, 0x00,  0, (void*)&OD_record1401},
{0x1402, 0x02, 0x00,  0, (void*)&OD_record1402},
{0x1403, 0x02, 0x00,  0, (void*)&OD_record1403},
{0x1600, 0x08, 0x00,  0, (void*)&OD_record1600},
{0x1601, 0x08, 0x00,  0, (void*)&OD_record1601},
{0x1602, 0x08, 0x00,  0, (void*)&OD_record1602},
{0x1603, 0x08, 0x00,  0, (void*)&OD_record1603},

{0x1800, 0x06, 0x00,  0, (void*)& OD_record1800},
{0x1801, 0x06, 0x00,  0, (void*)& OD_record1801},
{0x1802, 0x06, 0x00,  0, (void*)& OD_record1802},
{0x1803, 0x06, 0x00,  0, (void*)& OD_record1803},
{0x1804, 0x06, 0x00,  0, (void*)& OD_record1804},
{0x1805, 0x06, 0x00,  0, (void*)& OD_record1805},
{0x1806, 0x06, 0x00,  0, (void*)& OD_record1806},
{0x1807, 0x06, 0x00,  0, (void*)& OD_record1807},
{0x1808, 0x06, 0x00,  0, (void*)& OD_record1808},
{0x1809, 0x06, 0x00,  0, (void*)& OD_record1809},
{0x180A, 0x06, 0x00,  0, (void*)& OD_record180A},
{0x180B, 0x06, 0x00,  0, (void*)& OD_record180B},

{0x1A00, 0x08, 0x00,  0, (void*)& OD_record1A00},
{0x1A01, 0x08, 0x00,  0, (void*)& OD_record1A01},
{0x1A02, 0x08, 0x00,  0, (void*)& OD_record1A02},
{0x1A03, 0x08, 0x00,  0, (void*)& OD_record1A03},
{0x1A04, 0x08, 0x00,  0, (void*)& OD_record1A04},
{0x1A05, 0x08, 0x00,  0, (void*)& OD_record1A05},
{0x1A06, 0x08, 0x00,  0, (void*)& OD_record1A06},
{0x1A07, 0x08, 0x00,  0, (void*)& OD_record1A07},
{0x1A08, 0x08, 0x00,  0, (void*)& OD_record1A08},
{0x1A09, 0x08, 0x00,  0, (void*)& OD_record1A09},
{0x1A0A, 0x08, 0x00,  0, (void*)& OD_record1A0A},
{0x1A0B, 0x08, 0x00,  0, (void*)& OD_record1A0B},

{0x1F80, 0x00, 0x8D,  4, (void*)&CO_OD_ROM.NMTStartup},

{0x2000, 0x08, 0x00,  0, (void*)& OD_record2000},	//ÎÂ¶È´«¸ÐÆ÷1-4
{0x2001, 0x03, 0x00,  0, (void*)& OD_record2001},
{0x2002, 0x03, 0x00,  0, (void*)& OD_record2002},
{0x2003, 0x03, 0x00,  0, (void*)& OD_record2003},
{0x2004, 0x03, 0x00,  0, (void*)& OD_record2004},
{0x2005, 0x02, 0x00,  0, (void*)& OD_record2005},
{0x2007, 0x04, 0x00,  0, (void*)& OD_record2007},

{0x2010, 0x02, 0x00,  0, (void*)& OD_record2010},
{0x2011, 0x02, 0x00,  0, (void*)& OD_record2011},
{0x2012, 0x02, 0x00,  0, (void*)& OD_record2012},
{0x2013, 0x02, 0x00,  0, (void*)& OD_record2013},
{0x2014, 0x02, 0x00,  0, (void*)& OD_record2014},

{0x2040, 0x0D, 0x00,  0, (void*)& OD_record2040},
{0x2041, 0x04, 0x00,  0, (void*)& OD_record2041},

{0x2100, 0x00, 0x36, 10, (void*)&CO_OD_RAM.errorStatusBits[0]},
{0x2101, 0x00, 0x0D,  1, (void*)&CO_OD_ROM.CANNodeID},
{0x2102, 0x00, 0x8D,  2, (void*)&CO_OD_ROM.CANBitRate},
{0x2103, 0x00, 0x8E,  2, (void*)&CO_OD_RAM.SYNCCounter},
{0x2104, 0x00, 0x86,  2, (void*)&CO_OD_RAM.SYNCTime},
{0x2106, 0x00, 0x87,  4, (void*)&CO_OD_EEPROM.powerOnCounter},
{0x2107, 0x05, 0xBE,  2, (void*)&CO_OD_RAM.performance[0]},
{0x2108, 0x01, 0xB6,  2, (void*)&CO_OD_RAM.temperature[0]},
{0x2109, 0x01, 0xB6,  2, (void*)&CO_OD_RAM.voltage[0]},
{0x2110, 0x10, 0xFE,  4, (void*)&CO_OD_RAM.variableInt32[0]},
{0x2111, 0x10, 0xFD,  4, (void*)&CO_OD_ROM.variableROMInt32[0]},
{0x2112, 0x10, 0xFF,  4, (void*)&CO_OD_EEPROM.variableNVInt32[0]},
{0x2120, 0x05, 0x00,  0, (void*)&OD_record2120},
{0x2130, 0x03, 0x00,  0, (void*)&OD_record2130},

{0x2FF0, 0x03, 0x00,  0, (void*)OD_record2FF0},	//ccu code space
{0x2FF1, 0x03, 0x00,  0, (void*)OD_record2FF1},	//pms code space
{0x2FF2, 0x03, 0x00,  0, (void*)OD_record2FF2},	//bms code space
{0x2FF3, 0x03, 0x00,  0, (void*)OD_record2FF3},	//chg code space

{0x6000, 0x08, 0x76,  1, (void*)&CO_OD_RAM.readInput8Bit[0]},
{0x6200, 0x08, 0x3E,  1, (void*)&CO_OD_RAM.writeOutput8Bit[0]},
{0x6401, 0x0C, 0xB6,  2, (void*)&CO_OD_RAM.readAnalogueInput16Bit[0]},
{0x6411, 0x08, 0xBE,  2, (void*)&CO_OD_RAM.writeAnalogueOutput16Bit[0]},
};

void OD_Init_Var(uint16 index, void* pData, uint8 len)
{

}

