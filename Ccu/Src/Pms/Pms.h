#ifndef __PMS__H_
#define __PMS__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "MasterNode.h"
#include "Charger.h"
#include "Bms.h"
#include <DriverIo.h>
#include "PmsOd.h"

#define PMS_CMD_UNLOCK			0x31
#define PMS_CMD_ENABLE_CHARGE	0x32
#define PMS_CMD_READ_BATTERY	0x33
#define PMS_CMD_DISABLE_CHARGE	0x37
#define PMS_CMD_READ_DEV_ID		0x20
#define PMS_CMD_DEV_CFG			0x24

#ifdef CFG_EX_FRAME
#else
#define PMS_OD_SIZE 2
#endif

	//电池容量测试运行状态
	typedef enum
	{
		CTS_INIT 				= 0,//未接收到测试指令
		CTS_RX_TESTCMD			= 1,//接收到测试指令
		CTS_DISCHARGE_DELAY		= 2,//放电前的延迟间隔
		CTS_DISCHARGEING		= 3,//正在放电
		CTS_DISCHARGE_ABNORMAL	= 4,//放电异常处理
		CTS_CHARGE_DELAY		= 5,//充电电前的延迟间隔
		CTS_CHARGING			= 6,//正在充电
		CTS_TEST_ABNORMAL		= 7,//测试异常
		CTS_TEST_DONE			= 8,//测试完成
	}BatCapTestState;

	//电池容量测试异常的原因
	typedef enum
	{
		CTE_CHARGER_FAULT		= BIT_0,//充电器故障
		CTE_DISCHARGER_FAULT	= BIT_1,//放电设备故障
		CTE_CMD_OPEN_DOOR		= BIT_2,//正在放电
		CTE_CMD_STOP			= BIT_3,//接收到测试结束命令
		CTE_SMOCKING_ALARM		= BIT_4,//烟雾告警
		CTE_BAT_NOT_PRESENT		= BIT_5,//电池不在位
		CTE_BAT_FAULT			= BIT_6,//电池故障
		CTE_BAT_CONNECTOR_ERR	= BIT_7,//电池连接器故障
		CTE_NOT_ALLOW_DISCHARGE	= BIT_8,//上位机不允许充电
	}BatCapTestErr;

	//设置电池放电函数返回值
	typedef enum
	{
		DSCHG_OK = 0,		//设置电池放电成功
		DSCHG_NO_BATTERY,	//备电仓没有电池
		DSCHG_SOC_LESS,		//电池电量少
		DSCHG_BAT_FAULT,	//电池故障
	}DSCHG_RC;

	typedef struct
	{
		uint8 batIsIn : 1;			//电池是否在线；1：In；0：Not in
		uint8 chargerIsIn : 1;		//充电器是否在线；1：In；0：Not in
		uint8 isBatteryLock : 1;	//电池锁(电磁锁)状态,0:Lock, 1:unLock。
		uint8 doorLockPosition : 2;	//门锁(电机锁)位置状态；0=表示都没有到位;1=表示开（上）到位;2=表示锁电池（下）到位;3=表示都到位。
		uint8 batteryLockRunning : 1;		//电池锁(电机锁)运行状态，0：没有开锁动作；1：开锁动作过程中。
		uint8 isFunPower : 1;		//风扇状态，1：Power On； 0：Power off。
		uint8 isFirex : 1;			//火警状态，1：起火； 0：没起火。
	}PmsState1;

	typedef struct
	{
		uint8 chargerEnable : 1;	//充电器使能；1：enable；0：disable
		uint8 chargerSwitchIsOn : 1;//PMS板上连通充电器和电池的功率开关状态；1：On；0：Off
		uint8 warmRSwitchIsOn : 1;	//板上连通充电器和加热电阻的功率开关状态；
		uint8 bp_SwitchIsOn : 1;	//PMS板上连通电池和负载设备板/备份电源控制板的功率开关状态。
		uint8 Pms_VerifyIsPass : 1;	//PMS板认证电池的结果。
		uint8 Bms_VerifyIsPass : 1;	//电池认证PMS板的结果。
		uint8 warmREnable : 1;		//加热电阻的加热功能使能。
	}PmsState2;

	typedef struct
	{
		uint8 warmUnitFuseBreak : 1;	//加热器件保险断开
		uint8 warmUnitOvpBreak : 1;		//加热器件过温断开
	}PmsState3;

	//PMS供电模式
	typedef enum {
		PowerMode_unknown = 0, 
		PowerMode_220V, 
		PowerMode_48V,
		PowerMode_18650,
	}PmsPoweredMode;

	typedef struct
	{
		uint32 unlock				: 1;	//BIT[0]:开锁
		uint32 chargeEnable			: 1;	//BIT[1]:充电使能
		uint32 chargeEnableByTemp	: 1;	//BIT[2]:根据温度动态充电使能；
		uint32 _5VPowerOn			: 1;	//BIT[3]:5V电源输出使能
		uint32 _12VPowerOn			: 1;	//BIT[4]:12V电源输出使能
		uint32 batVerifyEnable		: 1;	//BIT[5]:认证功能是否启动。
		uint32 batCapTestEnable		: 1;	//BIT[6]:电池电量测试功能使能。
		uint32 warmingEnable		: 1;	//BIT[7]:电池加热功能使能。

		uint32 poweredMode			: 2;	//BIT[8-9]:机柜供电模式，0-不确定，1-市电，2-备电，3-18650电池供电。
		uint32 dischargeEnable		: 1;	//BIT[10]备电模式下，上位机设置电池输出使能
		uint32 passthroughRspReady	: 1;	//BIT[11]应答透传命令的数据已准备好
	}PmsCmd;

typedef struct _Pms
{
	MasterNode base;
	CO_OD_extension_t odExtension[PMS_OD_SIZE];	//当OD被Master读或者写时触发内部函数调用
	EntryExt entryExt[PMS_OD_SIZE];				//当OD从Slave节点读或写时，触发内部函数调用

	uint8 manufacturerDeviceName[12];
	uint8 manufacturerHardwareVersion[5];
	uint8 manufacturerSoftwareVersion[11];

	//Only for test
	int smoke1Value;
	int smoke2Value;

	uint8 elock_status_new;
	uint8 elock_error_status;
	uint8 is_bat_detected;

	//Pms hw version
	uint8 hwVer[2];

	//Pms fw version
	uint8 fwVer[7];

	Charger charger;
	Bms		bms;

	uint16* dischargeMosTemp;	//放电MOS温度
	uint16* volMeterTemp;		//电量计温度
	uint16* connectorTemp;		//连接器温度
	uint16* batTemp1;			//电池温度1
	uint16* batTemp2;			//电池温度2

	uint16* batHighestVolReg;	//最高单体电压寄存器
	uint16* batHighestVolNo;	//最高单体电压编号寄存器
	uint16* batLowestVolReg ;	//最低单体电压寄存器
	uint16* batLowestVolNo;		//最低单体电压编号寄存器

	//flag定义**********************************************
	uint8 OD_3010_flag;

	//数据字典定义**********************************************
	uint8 OD_3010_max_subindex;
	uint8 OD_pms_status1;
	uint8 OD_pms_status2;
	uint8 OD_pms_status3;
	CO_OD_entryRecord_t OD_record3010[4];

	uint8  OD_3011_max_subindex;
	uint32 OD_pms_cmd;
	uint32 OD_pms_cmd_set;
	uint32 OD_pms_cmd_clear;
	uint16 OD_charging_current_max;
	uint8  OD_swapping_soc;
	uint16 OD_charging_voltage_max;
	CO_OD_entryRecord_t OD_record3011[7];

	uint8  OD_3013_max_subindex;
	uint8  OD_PMS_HW_version[2];
	uint8  OD_PMS_FW_version[7];
	uint8  OD_PMS_SN[14];
	CO_OD_entryRecord_t OD_record3013[4];

	uint8  OD_3014_max_subindex			;
	uint16 OD_heater_ntc_temperature	;
	uint16 OD_cabinet_ntc_temperature	;
	uint16 OD_smoke_sensor1_value		;
	uint16 OD_smoke_sensor2_value		;
	uint16 OD_smoke_senor_total_current	;
	uint16 OD_front_motor_working_current;
	CO_OD_entryRecord_t OD_record3014[7];

	uint8  OD_3018_max_subindex				;
	uint8  OD_pms_active_detected_err_of_bat;
	uint32 OD_pms_detected_err_of_bat		;
	uint8  OD_pms_other_detected_err		;
	uint8  OD_pms_fault						;
	uint8  OD_pms_damage					;
	CO_OD_entryRecord_t OD_record3018[6]	;

	uint8  OD_301A_max_subindex;
	uint8  OD_update_file_HW_version[2];
	uint8  OD_update_file_FW_version[7];
	CO_OD_entryRecord_t OD_record3100[3];

	FwFileRecord OD_record301A;
	FwFileRecord OD_record3021;

	uint8  OD_3100_max_subindex;
	uint8  OD_capTest_State;		//电池容量测试状态,取值范围参考 BatCapTestState
	uint32 OD_capTest_CancelReason;	//电池容量测试中断原因
}Pms;

void Pms_Init(Pms* pPms, uint8 addr);
void Pms_Start(Pms* pPms);
void Pms_Stop(Pms* pPms);
void Pms_Run(Pms* pPms);
void Pms_DumpTestData(Pms* pPms);

Bool Pms_IsBatIn(const Pms* pPms);
uint16 Pms_GetBmsSoc(const Pms* pPms);
uint32 Pms_GetBmsFault(const Pms* pPms);

/***************************************************************
函数功能：判断是否满足放电条件，如果满足，则触发PMS发送放电命令。
参数：
	pPms：操作对象。
	dischargeSocThreshold： SOC阈值，大于等于该值，才允许放电阈
返回值：
	参考DSCHG_RC定义
***************************************************************/
DSCHG_RC Pms_SetDischarge(Pms* pPms, uint8 dischargeSocThreshold);

//关闭放电
void Pms_StopDischarge(Pms* pPms);

#ifdef __cplusplus
}
#endif

#endif

