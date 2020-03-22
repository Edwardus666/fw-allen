
#ifndef _BLOK_PROTOCOL_H_
#define _BLOK_PROTOCOL_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "SdoDef.h"

#pragma pack(1)
	//Init SDO Download Protocol Request
	typedef struct
	{
		uint8 s : 1;	//dataLength Indicator, 1: data set dataLength is indicated
		uint8 e : 1;	//transfer Type, 1: expedited transfer
		uint8 n : 2;	//Only valid if e = 1 and s = 1, otherwise 0,If valid it indicates the number of bytes in d 
		uint8 reserved : 1;	//not used, always 0
		uint8 ccs : 3;

		uint16 index;
		uint8 sub;
		uint8 data[4];
	}InitSegDownloadReqPkt;

	//Init SDO Download Protocol Response
	typedef struct
	{
		uint8 reserved : 5;	//not used, always 0
		uint8 scs : 3;

		uint16 index;
		uint8 sub;
		uint8 data[4];
	}InitSegDownloadRspPkt;


	//Init SDO Upload Protocol Rsp
	typedef struct
	{
		uint8 s : 1;	//sizeIndicator
		uint8 e : 1;	//transferType
		uint8 n : 2;	//Only valid if e = 1 and s = 1, otherwise 0,If valid it indicates the number of bytes in d 
		uint8 reserved : 1;	//not used, always 0
		uint8 ccs : 3;

		uint16 index;
		uint8 sub;
		uint8 data[4];
	}InitSegUploadRspPkt;

	//Download SDO Segment Protocol Req
	typedef struct
	{
		uint8 c : 1;	//sizeIndicator
		uint8 n : 3;	//Only valid if e = 1 and s = 1, otherwise 0,If valid it indicates the number of bytes in d 
		uint8 t : 1;	//not used, always 0
		uint8 ccs : 3;

		uint8 data[7];
	}SegDownloadReqPkt;

	//Download SDO Segment Protocol Rsp
	typedef struct
	{
		uint8 x : 4;	//Only valid if e = 1 and s = 1, otherwise 0,If valid it indicates the number of bytes in d 
		uint8 t : 1;	//not used, always 0
		uint8 scs : 3;

		uint16 index;
		uint8 sub;
		uint8 data[4];
	}SegDownloadRspPkt;
#pragma pack()


	//Init SDO Upload Protocol
	//typedef InitSegDownloadReqPkt InitSegUploadRspPkt;
	typedef InitSegDownloadRspPkt InitSegUploadReqPkt;
	typedef SegDownloadReqPkt	  SegUploadRspPkt;
	typedef SegDownloadRspPkt	  SegUploadReqPkt;

	void InitSegDownloadReqPkt_Build(const SdoTransMgr* pMgr, InitSegDownloadReqPkt* pPktOut);
	void InitSegUploadReqPkt_Build(InitSegUploadReqPkt* pPktOut, uint16 index, uint8 subIndex);

	SDO_ERR Segment_ProcInitDownloadRsp(SdoTransMgr* pMgr, const InitSegDownloadRspPkt* pRspPkt, SegDownloadReqPkt* pPktOut, SdoState* state);
	SDO_ERR Segment_ProcDownloadRsp(SdoTransMgr* pMgr, const SegDownloadRspPkt* pRspPkt, SegDownloadReqPkt* pPktOut, SdoState* state);
	SDO_ERR Segment_ProcInitUploadRsp(SdoTransMgr* pMgr, const InitSegUploadRspPkt* pRspPkt, SegUploadReqPkt* pPktOut, SdoState* state);
	SDO_ERR Segment_ProcUploadRsp(SdoTransMgr* pMgr, const SegUploadRspPkt* pRspPkt, SegUploadReqPkt* pPktOut, SdoState* state);

#ifdef __cplusplus
}
#endif

#endif
