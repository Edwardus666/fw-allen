
#ifndef _CAN_OPEN_MASTER_H_
#define _CAN_OPEN_MASTER_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "Obj.h"
#include "MasterNode.h"

#define MAX_LIST_COUNT 20

	typedef struct
	{
		Obj base;
		Bool isStart;
		MasterNode* nodes[MAX_LIST_COUNT];
	}Master;
	extern Master* g_pMaster;

	void Master_Dump(uint8 addr);
	void Master_Stop();
	void Master_Run();
	void Master_Start();
	void Master_Init();
	void Master_AddNode(MasterNode* pNode);
	void Master_SetNmtState(uint8 addr, uint8 cs);
	void Master_SetNodeValue(uint8 addr, int index, uint32 val);

#ifdef __cplusplus
}
#endif

#endif
