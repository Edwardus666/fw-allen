
#ifndef _Rs485ProtocolNode_H_
#define _Rs485ProtocolNode_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "Rs485Protocol.h"
#include "HwFwVer.h"

	typedef void (*Rs485ProtocolNodeFun)(void* pObj, Bool isProtocolBusy);
	typedef Bool (*Rs485ProtocolNodeIsFun)(void* pObj);

	typedef enum
	{
		UTP_NODE_INIT = 0,			//Node init
		UTP_NODE_PRE_OPERATION,		//Node Pre-operation state
		UTP_NODE_OPERATION,			//Node operation state
		UTP_NODE_STOP,		//Node Pre-operation state
	}Rs485ProtocolNodeState;

	typedef struct
	{
		uint8 addr;
		const char* name;

		HwFwVer ver;
		uint8 sn[14];

		UtpSdo  verSdo;

		Rs485ProtocolNodeState state;

		UtpCmd* transferUtpCmd;		//当前正在传输的命令
		UtpCmd* utpCmdArray;
		const RxReqDispatch* reqDispatch;

		Rs485ProtocolNodeFun	Run;
		Rs485ProtocolNodeIsFun	IsBusy;
		SwTimer sendCmdTimer;

		UtpCmd* utpCmd_ReadVersion;	//读取版本帧
		UtpCmd* utpCmd_Heatbeat;	//心跳帧

	}Rs485ProtocolNode;

	void Rs485ProtocolNode_Init(Rs485ProtocolNode* pNode
		, uint8 addr
		, const char* name
		, UtpCmd* utpCmdArray
		, UtpCmd* utpCmdReadVersion
		, UtpCmd* utpCmdHb
		, Rs485ProtocolNodeFun run
		, Rs485ProtocolNodeIsFun IsBusyFn);
	
	void Rs485ProtocolNode_Start(Rs485ProtocolNode* pNode);
	void Rs485ProtocolNode_Stop(Rs485ProtocolNode* pNode);
	void Rs485ProtocolNode_Run(Rs485ProtocolNode* pNode, Bool isBusy);
	void Rs485ProtocolNode_TransferDone(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd);

	UTP_EVENT_RC Rs485ProtocolNode_RspProc_Common(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd, UTP_EVENT ev);
	UTP_EVENT_RC Rs485ProtocolNode_RspProc_ReadVersion(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd, UTP_EVENT ev);
	void Rs485ProtocolNode_Run_WhenProtocolIdle(Rs485ProtocolNode* pNode);
	void Rs485ProtocolNode_SendCmd(Rs485ProtocolNode* pNode, uint8 cmd);
	void Rs485ProtocolNode_SendCmdEx(Rs485ProtocolNode* pNode, UtpCmd* pUtpCmd);

#ifdef __cplusplus
}
#endif

#endif
