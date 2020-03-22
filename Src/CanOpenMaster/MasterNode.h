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
		SET_NMT_OPERATION = 0x01,		//���ýڵ����״̬
		SET_NMT_STOPPED = 0x02,			//���ýڵ�ֹͣ״̬
		SET_NMT_PRE_OPERATION = 0x80,	//���ýڵ�Ԥ����״̬
		SET_NMT_RESET = 0x81,			//���ø�λ�ڵ�Ӧ�ò�
		SET_NMT_RESET_COMM = 0x82,		//���ø�λ�ڵ�ͨ��
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
		Bool isStart;	//�����Ƿ���ù�Start����

		//�Ƿ��˹�����Node�Ĳ���״̬�л���Ϊ�˷�����Խڵ㡣
		//��ΪTrueʱ����ʾNode�����Զ���Pre-Operation�л���Operation����Ҫ��Shell���������л�״̬
		//��ΪFalseʱ��Node���Զ���Pre-Operation�л���Operation��
		Bool manualControl;	
		uint16 HeartbeatTime;
		
		//֡���ͣ���ͬ�Ľڵ㣬���岻һ������׼�ڵ㣬4 Bit��Pms�ڵ�5 Bit
		//��ʼ��ʱ�趨

		//Pdo-Index��PDO֡����-�����ֵ�Indexӳ���������0��β
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

		CanTxMsg  nmtCanTxMsg;	//nmt��Ϣ����
		CanTxMsg  sdoCanTxMsg;	//sdo��Ϣ����

#ifdef CFG_RPDO
		RPdo rpdo[RPDO_COUNT];
		CanTxMsg  pdoCanTxMsg[RPDO_COUNT];	//pdo��Ϣ����
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

		//��Ϣ����
		Queue msgQueue;
		NodeMsg msgBuf[16];

		//Rpdo���м��
		uint32 rpdoIntervalMs;

		//SDO���У�����SOD��������
		Queue sdoQueue;
		//SDO����
		OdInd sdoBuf[3];
	}MasterNode;

	void MasterNode_Init(MasterNode* pMasterNode
		, uint8 addr
		, const char* name
		, const CO_OD_entry_t* pGlobalOd//�����������ֵ䣬���ܱ��޸ģ�����Slave���壬���Ա������ͬ���Ͷ�����
		, const CO_OD_entry_t* pPrivateOd		//����Ӱ�������ֵ���
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

