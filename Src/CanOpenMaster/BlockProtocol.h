
#ifndef _SEGMENT_PROTOCOL_H_
#define _SEGMENT_PROTOCOL_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "SdoDef.h"

#pragma pack(1)
	//Init SDO Block Download Protocol
	typedef struct
	{
		uint8 cs : 1;	//client subcommand
		uint8 s : 1;	// size indicator
		uint8 cc : 1;	//Only valid if e = 1 and s = 1, otherwise 0,If valid it indicates the number of bytes in d 
		uint8 reserved : 2;	//not used, always 0
		uint8 ccs : 3;	// client command specifier

		uint16 index;
		uint8 sub;
		uint32 size;
	}InitBlockDownloadReqPkt;

	typedef struct
	{
		uint8 ss : 1;	// server subcommand
		uint8 sc : 2;	// server CRC support
		uint8 reserved : 2;	//not used, always 0
		uint8 scs : 3;	//server command specifier

		uint16 index;
		uint8 sub;
		uint8 blockSize;
		uint8 reserved1[3];
	}InitBlockDownloadRspPkt;

	typedef struct
	{
		uint8 seq : 7;	// seq
		uint8 c : 1;	// indicates whether there are still more segments to be downloaded
		uint8 data[7];
	}BlockDownloadReqPkt;

	typedef struct
	{
		uint8 ss : 2;	// server subcommand
		uint8 reserved : 3;	//not used, always 0
		uint8 scs : 3;	//server command specifier

		uint8 seq;		// sequence number of segment 0 < seqno < 128
		uint8 blockSize;	//Number of segments per block
	}BlockDownloadRspPkt;

	typedef struct
	{
		uint8 cs : 1;	// client subcommand
		uint8 x : 1;	// reserved
		uint8 n : 3;	// indicates the number of bytes in the last segment
		uint8 ccs : 3;	//client command specifier

		uint16 crc;		// 
		uint8 data[5];	//reserved
	}EndBlockDownloadReqPkt;

	typedef struct
	{
		uint8 ss : 2;	// server subcommand
		uint8 x : 3;	// reserved
		uint8 scs : 3;	// server command specifier

		uint8 data[7];	//reserved
	}EndBlockDownloadRspPkt;

	//////////////////////////////////////////////////////////////////////////////////////
	//Init SDO Block Upload Protocol
	typedef struct
	{
		uint8 cs : 2;	//client subcommand
		uint8 cc : 1;	//Only valid if e = 1 and s = 1, otherwise 0,If valid it indicates the number of bytes in d 
		uint8 reserved : 2;	//not used, always 0
		uint8 ccs : 3;	// client command specifier

		uint16 index;
		uint8 sub;
		uint32 blockSize;
		uint8 pst;
		uint8 reserved1;
	}InitBlockUploadReqPkt;

	typedef struct
	{
		uint8 ss : 1;	// server subcommand
		uint8 s : 1;	// server CRC support
		uint8 sc : 1;	// server CRC support
		uint8 reserved : 2;	//not used, always 0
		uint8 scs : 3;	//server command specifier

		uint16 index;
		uint8 sub;
		uint32 size;
	}InitBlockUploadRspPkt;

	typedef struct
	{
		uint8 cs : 2;	//client subcommand
		uint8 reserved : 3;	//not used, always 0
		uint8 ccs : 3;	// client command specifier

		uint8 data[7];	//Reserved
	}InitBlockUploadReq2Pkt;

	/**********************************************************************/
	//Init SDO Upload Protocol
	//typedef InitBlockDownloadReqPkt InitBlockUploadRspPkt;
	//typedef InitBlockDownloadRspPkt InitBlockUploadReqPkt;
	//typedef BlockDownloadReqPkt BlockUploadRspPkt;
	typedef struct
	{
		uint8 cs : 1;	// cliet subcommand
		uint8 cc : 1;	// client CRC support
		uint8 reserved : 3;	//not used, always 0
		uint8 ccs : 3;	//server command specifier

		uint16 index;
		uint8 sub;
		uint8 blockSize;	//Number of segments per block
		uint8 pst;			//Protocol Switch Threshold in bytes to change the SDO transfer protocol
		uint8 data[2];		//Reserved
	}BlockUploadReqPkt;

	typedef struct
	{
		uint8 cs : 2;	// client CRC support
		uint8 reserved : 3;	//not used, always 0
		uint8 ccs : 3;	//server command specifier

		uint8 seq;
		uint8 blockSize;	//Number of segments per block
		uint8 data[5];		//Reserved
	}BlockUploadSeqReqPkt;

	typedef struct
	{
		uint8 ss : 2;	// server subcommand
		uint8 reserved : 3;	//not used, always 0
		uint8 scs : 3;	//server command specifier
	}BlockUploadRspPkt;

	typedef struct
	{
		uint8 ss : 2;	// client subcommand
		uint8 n : 3;	// indicates the number of bytes in the last segment
		uint8 scs : 3;	//client command specifier

		uint16 crc;		// 
		uint8 data[5];	//reserved
	}EndBlockUploadReqPkt;
	typedef struct
	{
		uint8 cs : 1;	// client subcommand
		uint8 x : 4;	// indicates the number of bytes in the last segment
		uint8 ccs : 3;	//client command specifier

		uint8 data[7];	//reserved
	}EndBlockUploadRspPkt;
#pragma pack()

	void Block_InitDownloadReq(SdoTransMgr* pSdoMgr, InitBlockDownloadReqPkt* pRspPkt);
	SDO_ERR Block_InitDownloadRsp(SdoTransMgr* pMgr, InitBlockDownloadRspPkt* pRsp, BlockDownloadReqPkt* pReq, SdoState* state);
	SDO_ERR Block_DownloadRsp(SdoTransMgr* pMgr, const BlockDownloadRspPkt* pRsp, BlockDownloadReqPkt* pReq, SdoState* state);
	SDO_ERR Block_DownloadEndRsp(SdoTransMgr* pMgr, const EndBlockDownloadRspPkt* pRsp, BlockDownloadReqPkt* pReq, SdoState* state);

	SDO_ERR Block_DownloadReqPkt(SdoTransMgr* pMgr, BlockDownloadReqPkt* pReq);
	void Block_InitUploadReq(SdoTransMgr* pMgr, InitBlockUploadReqPkt* pReq);
	SDO_ERR Block_InitUploadRsp(SdoTransMgr* pMgr, const InitBlockUploadRspPkt* pRsp, InitBlockUploadReq2Pkt* pReq, SdoState* state);
	SDO_ERR Block_UploadReq(SdoTransMgr* pMgr, const BlockDownloadReqPkt* pRsp, BlockUploadSeqReqPkt* pReq, SdoState* state);
	SDO_ERR Block_UploadEndReq(SdoTransMgr* pMgr, const EndBlockUploadReqPkt* pRsp, EndBlockUploadRspPkt* pReq, SdoState* state);

#ifdef __cplusplus
}
#endif

#endif
