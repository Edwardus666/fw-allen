#ifndef __DEBUG_PARAM__H_
#define __DEBUG_PARAM__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "SectorMgr.h"

#define DEBUG_PARAM_AREA_VER	1	//���洢���ݵĽṹ���ģ��Ҳ��ܼ��ݾɰ汾���ͱ����޸İ汾�ţ�Flash�оɰ汾���ݱ�������
#define DEBUG_PARAM_AREA_SIZE	128	//����Ϊ4������������ΪFlashÿ��д����4���ֽ�Ϊ��λ��

#pragma pack(1)

	typedef struct
	{
		Nvds  base;	//�ڵ�һ��λ�ñ������NvdsBase

		uint32 debugLevel;

		uint8 reserved[DEBUG_PARAM_AREA_SIZE - sizeof(Nvds) - 4];
	}DebugParam;

#pragma pack()

	extern DebugParam* g_pDebugParam;
#ifdef __cplusplus
}
#endif

#endif

