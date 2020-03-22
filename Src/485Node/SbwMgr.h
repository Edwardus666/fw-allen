
#ifndef _485_MGR_H_
#define _485_MGR_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "Rs485ProtocolNode.h"

#define SHELF_LOCK_485_ADDR 0x81

	typedef enum
	{
		SBW_INIT,
		SBW_PRE_OPERATION,
		SBW_OPERATION,
	}SbwState;

#define MAX_SBW_NODE_SIZE 3
	typedef struct
	{
		SbwState state;
		Rs485Protocol utp;

		uint8 nodeIndex;
		int nodeCount;
		Rs485ProtocolNode* node[MAX_SBW_NODE_SIZE];
	}SbwMgr;

	void SbwMgr_Init();
	void SbwMgr_Start();
	void SbwMgr_Stop();
	Rs485ProtocolNode* SbwMgr_GetNode(uint8 addr);
	Bool SbwMgr_Send(UtpCmd* p);

#ifdef __cplusplus
}
#endif

#endif
