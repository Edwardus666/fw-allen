
#ifndef _FSM_H_
#define _FSM_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

	//BP�¼�����
	typedef enum
	{
		BP_BAT_PLUGIN = 1,	//BP��ز���
		BP_BAT_PLUGOUT,		//BP��ذγ�
		BP_BAT_SOC_LESS,	//��ص�������
		BP_AC_ON,			//�е繩��
		BP_12VBP_DET_OFF,	//12VBP_DET_OFF
	}BpEventType;

	typedef enum
	{
		S_INIT = 0,				//��ʼ��
		S0_18650POWERED,	//18650����
		S1_48V_POWER,		//��ع���
		S2_AC_POWERED,		//��������
	}FsmState;

	typedef struct
	{
		FsmState state;

		Bool isBpStart;	//BPϵͳ�Ƿ�������
	}Fsm;
	void Fsm_Init();

#ifdef __cplusplus
}
#endif

#endif
