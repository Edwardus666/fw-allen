#ifndef __NVDS_H_
#define __NVDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "SectorMgr.h"
#include "HwFwVer.h"
#define EEPROM_FIRST_BYTE	0x55
#define EEPROM_LATEST_BYTE	0xAA


	typedef enum
	{
		NVDS_VER_INFO = 0,
		NVDS_CFG_INFO,
		NDDS_DBG_INFO,
	}NvdsID;

#pragma pack(1) 
	//版本信息区内容,必须在结构中增加firstByte和latestByte,用于校验从Flash中读取的区块内容的完整性
	//结构字节数必须为偶数(2字节对齐)，否则无法写入Flash
	typedef struct
	{
		uint8 firstByte;	//用于判定结构大小是否被修改

		HwFwVer	version;
		char manufacturerHardwareVersion[4];
		char manufacturerSoftwareVersion[11];

		uint8 latestByte;	//从存储区读出的字节不等于 EEPROM_LATEST_BYTE，说明该存储区被修改，已经失效
	}VersionInfo;

	//配置信息区内容,必须在结构中增加firstByte和latestByte,用于校验从Flash中读取的区块内容的完整性
	//结构字节数必须为偶数(2字节对齐)，否则无法写入Flash
	typedef struct
	{
		uint8 firstByte;

		uint16 temp_thresholdLow;	//温度-低阈值，温度下降超过该阈值时停止风扇，单位：摄氏度
		uint16 temp_thresholdHigh;	//温度-高阈值，温度上升超过该阈值时驱动风扇，单位：摄氏度

		uint8 latestByte;	//从存储区读出的字节不等于 EEPROM_LATEST_BYTE，说明该存储区被修改，已经失效
	}CfgInfo;

	//调试信息区内容,必须在结构中增加firstByte和latestByte,用于校验从Flash中读取的区块内容的完整性
	//结构字节数必须为偶数(2字节对齐)，否则无法写入Flash
	typedef struct
	{
		uint8 firstByte;
		uint32 debugLevel;
		uint8 latestByte;	//从存储区读出的字节不等于 EEPROM_LATEST_BYTE，说明该存储区被修改，已经失效
	}DebugInfo;

#pragma pack() 
	/*******************************************************************************/
	/*Nvds事件定义*/
	typedef enum
	{
		BE_DATA_ERROR,	//存储区数据检验失败，数据无效
		BE_DATA_OK,		//存储区数据检验通过，数据有效
		BE_ON_WRITE_BEFORE,	//写数据操作之前
	}BlockEventID;
	//事件函数定义
	typedef void (*BlockEventFn)(void* pData, BlockEventID eventId);
	typedef struct
	{
		NvdsID		id;					//索引
		SectorMgr	sectorMgr;			//扇区管理			
		BlockEventFn Event;	//当存储块信息无效时，设置默认值的函数
	}Nvds;

	extern VersionInfo  *g_pVerInfo;
	extern CfgInfo		*g_pCfgInfo;
	extern DebugInfo	*g_pDbgInfo;

	void Nvds_Write(NvdsID id);
	void Nvds_Init();

#ifdef __cplusplus
}
#endif

#endif

