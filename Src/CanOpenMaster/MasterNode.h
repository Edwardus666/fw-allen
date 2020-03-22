#ifndef __NODE_CLIENT_H_
#define __NODE_CLIENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "typedef.h"
#include "MasterSdo.h"
#include "DriverCan.h"
#include "Frame.h"
#include "Pdo.h"
#include "OdGlobal.h"

#define HEARTBEAT_TIME_MS 6000
#define MAX_CALLBACK_PARAM_COUNT MAX_MAIL_BOX_COUNT

	typedef enum enum_nodeState {
		NMT_INIT = 0x00,
		NMT_DISCONNECT = 0x01,
		NMT_CONNECT = 0x02,

		NMT_STOPPED = 0x04,
		NMT_OPERATION = 0x05,
		NMT_PRE_OPERATION = 0x7F,
		NMT_UNKNOWN = 0x0F
	}NMT_STATE;

	typedef enum {
		SET_NMT_OPERATION = 0x01,		//设置节点操作状态
		SET_NMT_STOPPED = 0x02,			//设置节点停止状态
		SET_NMT_PRE_OPERATION = 0x80,	//设置节点预操作状态
		SET_NMT_RESET = 0x81,			//设置复位节点应用层
		SET_NMT_RESET_COMM = 0x82,		//设置复位节点通信
	}SET_NMT_STATE;

	typedef struct
	{
		uint16 index;
		uint8  pdoType;
	}PdoTypeMapInd;


	typedef struct
	{
		uint16 index;
		uint8  sub;
	}OdInd;

#pragma pack(1)
	typedef struct
	{
		uint8  msgId;
		union
		{
			struct
			{
				uint32	param1;
				uint32	param2;
			};
			Frame frame;
		};
	}NodeMsg;
#pragma pack()

	struct _MasterNode;
	void CanTxMsg_Init(CanTxMsg* p, void* pMasterNode, uint8 type, uint8 nodeId);

#define RPDO_COUNT 4
	typedef void (*NodeFrameProc)(void* pNode, Frame* pFrame);
	typedef void (*NodeFn)(void* pNode);
	typedef void (*NodeFsmFn)(void* pNode, uint8 msgId);
	typedef uint8 (*GetFrameTypeFn)(uint32 ident);
	typedef CO_OD_entry* (*GetPdoMapParamFn)(void* pMasterNode, uint8  pdoType);
	typedef struct _MasterNode
	{
		NMT_STATE state;
		uint8 addr;
		const char* name;
		Bool isStart;	//对象是否调用过Start函数

		//是否人工控制Node的操作状态切换，为了方便调试节点。
		//当为True时，表示Node不会自动从Pre-Operation切换到Operation，需要从Shell发送命令切换状态
		//当为False时，Node会自动从Pre-Operation切换到Operation。
		Bool manualControl;	
		uint16 HeartbeatTime;
		
		//帧类型，不同的节点，定义不一样，标准节点，4 Bit，Pms节点5 Bit
		//初始化时设定

		//Pdo-Index，PDO帧类型-数字字典Index映射表，必须以0结尾
		const PdoTypeMapInd* PdoTypeMapIndArray;

		NodeFsmFn Init;
		NodeFsmFn PreOperation;
		NodeFsmFn Operation;
		NodeFrameProc FrameProc;
		GetFrameTypeFn GetFrameType;

		NodeFn  Stop;
		NodeFn	Start;
		NodeFn	Run;

		MasterSdo sdoMgr;
		SwTimer heartbeatTimer;

		CanTxMsg  nmtCanTxMsg;	//nmt消息对象
		CanTxMsg  sdoCanTxMsg;	//sdo消息对象

#ifdef CFG_RPDO
		RPdo rpdo[RPDO_COUNT];
		CanTxMsg  pdoCanTxMsg[RPDO_COUNT];	//pdo消息对象
#endif

		const CO_OD_entry_t* pGlobalOd;
		int globalOdSize;

		const CO_OD_entry_t* pPrivateOd;
		int privateOdSize;

		int entryExtCount;
		EntryExt* pEntryExtArray;

		NodeFn		   Dump;
		EntryChangedFn Changed;
		void* pObjArg;

		/** Pointer to array of CO_OD_extension_t objects. Size of the array is
		equal to ODSize. */
		CO_OD_extension_t* ODExtensions;

		//消息队列
		Queue msgQueue;
		NodeMsg msgBuf[16];

		//Rpdo运行间隔
		uint32 rpdoIntervalMs;

		//SDO队列，保存SOD待发命令
		Queue sdoQueue;
		//SDO缓存
		OdInd sdoBuf[3];
	}MasterNode;

	void MasterNode_Init(MasterNode* pMasterNode
		, uint8 addr
		, const char* name
		, const CO_OD_entry_t* pGlobalOd//公共的数据字典，不能被修改，来自Slave定义，可以被多个相同类型对象公用
		, const CO_OD_entry_t* pPrivateOd		//重新影射数据字典项
		, int privateOdSize
		, CO_OD_extension_t* ODExtensions
		, const PdoTypeMapInd* PdoTypeMapIndArray
		, EntryExt* pEntryExtArray
	);

	void MasterNode_Dump(MasterNode* pMasterNode);
	void MasterNode_Start(MasterNode* pMasterNode);
	void MasterNode_Stop(MasterNode* pMasterNode);
	void MasterNode_Run(MasterNode* pMasterNode);

	Bool MasterNode_SendNmt(MasterNode* pMasterNode, SET_NMT_STATE state);
	void MasterNode_Fsm(MasterNode* pMasterNode, uint8 msgId, uint32 param1);
	Bool MasterNode_CsIsValid(uint8 cs);
	MasterNode* Master_FindMasterNode(uint8 id);
	Bool MasterNode_SetOd(MasterNode* pMasterNode, uint16 index, uint8 subInd, uint8* pData, int len);
	CO_OD_entry_t* MasterNode_GetOd(MasterNode* pMasterNode, uint16 index, uint8 subInd);
#ifdef CFG_RPDO
	void MasterNode_SetRPDOSentData(MasterNode* pMasterNode, const void* pOdData, const void* pNewData, int len);
#endif
	void MasterNode_PostFrame(MasterNode* p, uint8 msgId, const Frame* pFrame);
	Bool MasterNode_ReadEntry(MasterNode* pMasterNode, uint16 index, uint8 subIndex);
	Bool MasterNode_WriteEntry(MasterNode* pMasterNode, uint16 index, uint8 subIndex, uint8* pData, int len);
	void MasterNode_DumpNeedUpload(MasterNode* pMasterNode);

#ifdef __cplusplus
}
#endif

#endif

