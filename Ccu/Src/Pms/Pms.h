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

	//���������������״̬
	typedef enum
	{
		CTS_INIT 				= 0,//δ���յ�����ָ��
		CTS_RX_TESTCMD			= 1,//���յ�����ָ��
		CTS_DISCHARGE_DELAY		= 2,//�ŵ�ǰ���ӳټ��
		CTS_DISCHARGEING		= 3,//���ڷŵ�
		CTS_DISCHARGE_ABNORMAL	= 4,//�ŵ��쳣����
		CTS_CHARGE_DELAY		= 5,//����ǰ���ӳټ��
		CTS_CHARGING			= 6,//���ڳ��
		CTS_TEST_ABNORMAL		= 7,//�����쳣
		CTS_TEST_DONE			= 8,//�������
	}BatCapTestState;

	//������������쳣��ԭ��
	typedef enum
	{
		CTE_CHARGER_FAULT		= BIT_0,//���������
		CTE_DISCHARGER_FAULT	= BIT_1,//�ŵ��豸����
		CTE_CMD_OPEN_DOOR		= BIT_2,//���ڷŵ�
		CTE_CMD_STOP			= BIT_3,//���յ����Խ�������
		CTE_SMOCKING_ALARM		= BIT_4,//����澯
		CTE_BAT_NOT_PRESENT		= BIT_5,//��ز���λ
		CTE_BAT_FAULT			= BIT_6,//��ع���
		CTE_BAT_CONNECTOR_ERR	= BIT_7,//�������������
		CTE_NOT_ALLOW_DISCHARGE	= BIT_8,//��λ����������
	}BatCapTestErr;

	//���õ�طŵ纯������ֵ
	typedef enum
	{
		DSCHG_OK = 0,		//���õ�طŵ�ɹ�
		DSCHG_NO_BATTERY,	//�����û�е��
		DSCHG_SOC_LESS,		//��ص�����
		DSCHG_BAT_FAULT,	//��ع���
	}DSCHG_RC;

	typedef struct
	{
		uint8 batIsIn : 1;			//����Ƿ����ߣ�1��In��0��Not in
		uint8 chargerIsIn : 1;		//������Ƿ����ߣ�1��In��0��Not in
		uint8 isBatteryLock : 1;	//�����(�����)״̬,0:Lock, 1:unLock��
		uint8 doorLockPosition : 2;	//����(�����)λ��״̬��0=��ʾ��û�е�λ;1=��ʾ�����ϣ���λ;2=��ʾ����أ��£���λ;3=��ʾ����λ��
		uint8 batteryLockRunning : 1;		//�����(�����)����״̬��0��û�п���������1���������������С�
		uint8 isFunPower : 1;		//����״̬��1��Power On�� 0��Power off��
		uint8 isFirex : 1;			//��״̬��1����� 0��û���
	}PmsState1;

	typedef struct
	{
		uint8 chargerEnable : 1;	//�����ʹ�ܣ�1��enable��0��disable
		uint8 chargerSwitchIsOn : 1;//PMS������ͨ������͵�صĹ��ʿ���״̬��1��On��0��Off
		uint8 warmRSwitchIsOn : 1;	//������ͨ������ͼ��ȵ���Ĺ��ʿ���״̬��
		uint8 bp_SwitchIsOn : 1;	//PMS������ͨ��غ͸����豸��/���ݵ�Դ���ư�Ĺ��ʿ���״̬��
		uint8 Pms_VerifyIsPass : 1;	//PMS����֤��صĽ����
		uint8 Bms_VerifyIsPass : 1;	//�����֤PMS��Ľ����
		uint8 warmREnable : 1;		//���ȵ���ļ��ȹ���ʹ�ܡ�
	}PmsState2;

	typedef struct
	{
		uint8 warmUnitFuseBreak : 1;	//�����������նϿ�
		uint8 warmUnitOvpBreak : 1;		//�����������¶Ͽ�
	}PmsState3;

	//PMS����ģʽ
	typedef enum {
		PowerMode_unknown = 0, 
		PowerMode_220V, 
		PowerMode_48V,
		PowerMode_18650,
	}PmsPoweredMode;

	typedef struct
	{
		uint32 unlock				: 1;	//BIT[0]:����
		uint32 chargeEnable			: 1;	//BIT[1]:���ʹ��
		uint32 chargeEnableByTemp	: 1;	//BIT[2]:�����¶ȶ�̬���ʹ�ܣ�
		uint32 _5VPowerOn			: 1;	//BIT[3]:5V��Դ���ʹ��
		uint32 _12VPowerOn			: 1;	//BIT[4]:12V��Դ���ʹ��
		uint32 batVerifyEnable		: 1;	//BIT[5]:��֤�����Ƿ�������
		uint32 batCapTestEnable		: 1;	//BIT[6]:��ص������Թ���ʹ�ܡ�
		uint32 warmingEnable		: 1;	//BIT[7]:��ؼ��ȹ���ʹ�ܡ�

		uint32 poweredMode			: 2;	//BIT[8-9]:���񹩵�ģʽ��0-��ȷ����1-�е磬2-���磬3-18650��ع��硣
		uint32 dischargeEnable		: 1;	//BIT[10]����ģʽ�£���λ�����õ�����ʹ��
		uint32 passthroughRspReady	: 1;	//BIT[11]Ӧ��͸�������������׼����
	}PmsCmd;

typedef struct _Pms
{
	MasterNode base;
	CO_OD_extension_t odExtension[PMS_OD_SIZE];	//��OD��Master������дʱ�����ڲ���������
	EntryExt entryExt[PMS_OD_SIZE];				//��OD��Slave�ڵ����дʱ�������ڲ���������

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

	uint16* dischargeMosTemp;	//�ŵ�MOS�¶�
	uint16* volMeterTemp;		//�������¶�
	uint16* connectorTemp;		//�������¶�
	uint16* batTemp1;			//����¶�1
	uint16* batTemp2;			//����¶�2

	uint16* batHighestVolReg;	//��ߵ����ѹ�Ĵ���
	uint16* batHighestVolNo;	//��ߵ����ѹ��żĴ���
	uint16* batLowestVolReg ;	//��͵����ѹ�Ĵ���
	uint16* batLowestVolNo;		//��͵����ѹ��żĴ���

	//flag����**********************************************
	uint8 OD_3010_flag;

	//�����ֵ䶨��**********************************************
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
	uint8  OD_capTest_State;		//�����������״̬,ȡֵ��Χ�ο� BatCapTestState
	uint32 OD_capTest_CancelReason;	//������������ж�ԭ��
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
�������ܣ��ж��Ƿ�����ŵ�������������㣬�򴥷�PMS���ͷŵ����
������
	pPms����������
	dischargeSocThreshold�� SOC��ֵ�����ڵ��ڸ�ֵ��������ŵ���
����ֵ��
	�ο�DSCHG_RC����
***************************************************************/
DSCHG_RC Pms_SetDischarge(Pms* pPms, uint8 dischargeSocThreshold);

//�رշŵ�
void Pms_StopDischarge(Pms* pPms);

#ifdef __cplusplus
}
#endif

#endif

