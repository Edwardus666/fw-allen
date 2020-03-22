
#ifndef _SDO_MASTER_TRANS_MGR_H_
#define _SDO_MASTER_TRANS_MGR_H_

#ifdef __cplusplus
extern "C"{
#endif
#include "typedef.h"
#include "SdoDef.h"
#include "SwTimer.h"
#include "Od.h"

	//Master SDO 传输管理控制
	struct _MasterNode;
	typedef struct
	{
		SdoTransMgr	TransMgr;

		uint8	txCounter;		//错误重发次数
		Bool	isBlockEnable;	//Is block transfer

		//SdoPkt		Sdo;
		SdoState	sdoState;
		SdoTransType type;

		SwTimer		timer;

		const CO_OD_entry* pOd;
		int odSize;

		struct _MasterNode* pNode;
	}MasterSdo;

	void  MasterSdo_Init(MasterSdo* pSdo, struct _MasterNode* pNode);
	void MasterSdo_Reset(MasterSdo* pMgr);
	void MasterSdo_TimerStart(MasterSdo* pSdo, uint32 timeOutMs);
	Bool MasterSdo_IsTimerOut(MasterSdo* pSdo);
	SDO_ERR MasterSdo_TransInit(MasterSdo* pSdo, int index, uint8 subIndex, Bool isDownload, Bool isBlockEnable, SdoPkt* pOutPkt);
	SDO_ERR MasterSdo_ProcRsp(MasterSdo* pMasterSdo, SdoPkt* pSdoIn, SdoPkt* pSdoOut);
	void MasterSdo_Set(MasterSdo* pMgr, const uint8* pData, int len);

#ifdef __cplusplus
}
#endif

#endif
