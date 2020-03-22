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
	//�汾��Ϣ������,�����ڽṹ������firstByte��latestByte,����У���Flash�ж�ȡ���������ݵ�������
	//�ṹ�ֽ�������Ϊż��(2�ֽڶ���)�������޷�д��Flash
	typedef struct
	{
		uint8 firstByte;	//�����ж��ṹ��С�Ƿ��޸�

		HwFwVer	version;
		char manufacturerHardwareVersion[4];
		char manufacturerSoftwareVersion[11];

		uint8 latestByte;	//�Ӵ洢���������ֽڲ����� EEPROM_LATEST_BYTE��˵���ô洢�����޸ģ��Ѿ�ʧЧ
	}VersionInfo;

	//������Ϣ������,�����ڽṹ������firstByte��latestByte,����У���Flash�ж�ȡ���������ݵ�������
	//�ṹ�ֽ�������Ϊż��(2�ֽڶ���)�������޷�д��Flash
	typedef struct
	{
		uint8 firstByte;

		uint16 temp_thresholdLow;	//�¶�-����ֵ���¶��½���������ֵʱֹͣ���ȣ���λ�����϶�
		uint16 temp_thresholdHigh;	//�¶�-����ֵ���¶�������������ֵʱ�������ȣ���λ�����϶�

		uint8 latestByte;	//�Ӵ洢���������ֽڲ����� EEPROM_LATEST_BYTE��˵���ô洢�����޸ģ��Ѿ�ʧЧ
	}CfgInfo;

	//������Ϣ������,�����ڽṹ������firstByte��latestByte,����У���Flash�ж�ȡ���������ݵ�������
	//�ṹ�ֽ�������Ϊż��(2�ֽڶ���)�������޷�д��Flash
	typedef struct
	{
		uint8 firstByte;
		uint32 debugLevel;
		uint8 latestByte;	//�Ӵ洢���������ֽڲ����� EEPROM_LATEST_BYTE��˵���ô洢�����޸ģ��Ѿ�ʧЧ
	}DebugInfo;

#pragma pack() 
	/*******************************************************************************/
	/*Nvds�¼�����*/
	typedef enum
	{
		BE_DATA_ERROR,	//�洢�����ݼ���ʧ�ܣ�������Ч
		BE_DATA_OK,		//�洢�����ݼ���ͨ����������Ч
		BE_ON_WRITE_BEFORE,	//д���ݲ���֮ǰ
	}BlockEventID;
	//�¼���������
	typedef void (*BlockEventFn)(void* pData, BlockEventID eventId);
	typedef struct
	{
		NvdsID		id;					//����
		SectorMgr	sectorMgr;			//��������			
		BlockEventFn Event;	//���洢����Ϣ��Чʱ������Ĭ��ֵ�ĺ���
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

