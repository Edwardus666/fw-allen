#ifndef __CHARGER__H_
#define __CHARGER__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "MasterNode.h"

typedef struct _Charger
{
	uint8  OD_20FF_max_subindex;
	INTEGER16      heater_NTC_temperature;
	INTEGER16      cabinet_NTC_temperature;
	INTEGER16      charger_temperature;
	CO_OD_entryRecord_t OD_record20FF[4];

	uint8		   OD_2043_max_subindex;
	UNSIGNED8      charger_online_status; /*充电器是否在线*/
	UNSIGNED8      charger_output_C[2]; /*充电器目前输出的电流，单位0.1A*/
	UNSIGNED8      charger_output_V[2]; /*充电器目前输出的电压，单位0.1V*/
	UNSIGNED8      charger_output_P[2]; /*充电器目前输出的功率，单位0.1W*/
	UNSIGNED8      charger_status[4];	/*充电器目前的故障状态、信号状态、温度状态等；对应充电器协议解析*/
	UNSIGNED8      charger_sn[14];		/*充电器的SN号*/
	CO_OD_entryRecord_t OD_record2043[7];

	uint8  OD_3015_max_subindex	;
	uint16 OD_charger_C			;
	uint16 OD_charger_V			;
	uint16 OD_charger_P			;
	uint8  OD_charger_error1	;
	uint8  OD_charger_error2	;
	uint8  OD_charger_temperature;
	uint8  OD_charger_reserved	;
	CO_OD_entryRecord_t OD_record3015[8];

	uint8  OD_3016_max_subindex;
	uint8  OD_charger_HW_version[2];
	uint8  OD_charger_FW_version[7];
	uint8  OD_charger_SN[14];
	CO_OD_entryRecord_t OD_record3016[4];

	uint8  OD_3017_max_subindex;
	uint16 OD_charger_voltage_setting_value;
	uint8  OD_charger_current_setting_value;
	CO_OD_entryRecord_t OD_record3017[3];

}Charger;

void Charger_Init(Charger* pCharger);
void Charger_Start(Charger* pCharger);
void Charger_Stop(Charger* pCharger);
void Charger_Run(Charger* pCharger);

#ifdef __cplusplus
}
#endif

#endif

