#ifndef __BMS__H_
#define __BMS__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "MasterNode.h"

//电池寄存器数据，设备信息
#define DEV_INFO_REG_BIGIN_ADDR 0
#define DEV_INFO_REG_END_ADDR   32
#define DEV_INFO_REG_OFFSET		0		
#define DEV_INFO_REG_SIZE	(DEV_INFO_REG_END_ADDR-DEV_INFO_REG_BIGIN_ADDR+1)

//电池寄存器数据，BMS只读数据
#define READ_ONLY_REG_BIGIN_ADDR 256
#define READ_ONLY_REG_END_ADDR   322//315//308
#define READ_ONLY_REG_OFFSET	(DEV_INFO_REG_OFFSET + DEV_INFO_REG_SIZE)
#define READ_ONLY_REG_SIZE		(READ_ONLY_REG_END_ADDR-READ_ONLY_REG_BIGIN_ADDR+1)

//电池寄存器数据，BMS只读数据
#define CTRL_REG_BIGIN_ADDR 512
#define CTRL_REG_END_ADDR 538//536
#define CTRL_REG_OFFSET			(READ_ONLY_REG_OFFSET + READ_ONLY_REG_SIZE)
#define CTRL_REG_SIZE			(CTRL_REG_END_ADDR-CTRL_REG_BIGIN_ADDR+1)

//电池寄存器数据，BMS运行控制
#define PARAM_REG_BIGIN_ADDR	768
#define PARAM_REG_END_ADDR		832//830
#define PARAM_REG_OFFSET		(CTRL_REG_OFFSET + CTRL_REG_SIZE)
#define PARAM_REG_SIZE			(PARAM_REG_END_ADDR-PARAM_REG_BIGIN_ADDR+1)

//电池寄存器数据，BMS参数配置
#define USER_REG_BIGIN_ADDR 4096
#define USER_REG_END_ADDR	4100
#define USER_REG_OFFSET		(PARAM_REG_OFFSET + PARAM_REG_SIZE)
#define USER_REG_SIZE		(USER_REG_END_ADDR-USER_REG_BIGIN_ADDR+1)

//电池寄存器数据，用户数据区
#define BMS_REG_COUNT		(USER_REG_OFFSET + USER_REG_SIZE)

#define	MAX_BMS_FWFILE_COUNT 64

typedef struct _Bms
{
	UNSIGNED8 OD_2026_max_subindex;
	UNSIGNED8      active_detected_fault; /*主动检测到的故障*/
	UNSIGNED32     detected_fault; /*被动检测到的故障。*/
	CO_OD_entryRecord_t OD_record2026[3];

	//BMS 寄存器
	uint16 reg_unit[BMS_REG_COUNT];

	FwFileRecord fwFile[MAX_BMS_FWFILE_COUNT];
}Bms;

void Bms_Init(Bms* pBms);
void Bms_Start(Bms* pBms);
void Bms_Stop(Bms* pBms);
void Bms_Run(Bms* pBms);

#ifdef __cplusplus
}
#endif

#endif

