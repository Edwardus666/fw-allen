
#ifndef _SDO_DEF_H_
#define _SDO_DEF_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "CO_OD_Def.h"
#define SDO_NORMAL_TRANS    0	//普通传输
#define SDO_EXPEDITED_TRANS 1	//快速传输

#pragma pack(1)
	typedef struct
	{
		uint8 cmd;
		uint16 ind;
		uint8 subInd;
		uint8 data[4];
	}SdoPkt;

#pragma pack()

	typedef enum
	{
		SDO_IDLE,
		SDO_DONE,
		SDO_FAILED,
		SDO_SEGMENT_DOWNLOAD_INIT,
		SDO_SEGMENT_DOWNLOAD,
		SDO_SEGMENT_UPLOAD_INIT,
		SDO_SEGMENT_UPLOAD,
		SDO_BLOCK_DOWNLOAD_INIT,
		SDO_BLOCK_DOWNLOAD,
		SDO_BLOCK_DOWNLOAD_END,
		SDO_BLOCK_UPLOAD_INIT,
		SDO_BLOCK_UPLOAD,
		SDO_BLOCK_UPLOAD_END,
	}SdoState;

	typedef enum
	{
		SDO_TRANS_TYPE_UNKNOWN,
		SDO_TRANS_TYPE_DOWNLOAD,
		SDO_TRANS_TYPE_UPLOAD,
		SDO_TRANS_TYPE_SEGMENT_DOWNLOAD,
		SDO_TRANS_TYPE_SEGMENT_UPLOAD,
		SDO_TRANS_TYPE_BLOCK_DOWNLOAD,
		SDO_TRANS_TYPE_BLOCK_UPLOAD,
	}SdoTransType;

	//typedef enum
	//{
	//	CO_SDO_AB_NONE,
	//	SDO_FALIED,
	//	SDO_STATE_ERROR,

	//	CO_SDO_AB_NONE                  = 0x00000000UL, /**< 0x00000000, No abort */
	//	CO_SDO_AB_TOGGLE_BIT            = 0x05030000UL, /**< 0x05030000, Toggle bit not altered */
	//	CO_SDO_AB_TIMEOUT               = 0x05040000UL, /**< 0x05040000, SDO protocol timed out */
	//	CO_SDO_AB_CMD                   = 0x05040001UL, /**< 0x05040001, Command specifier not valid or unknown */
	//	CO_SDO_AB_BLOCK_SIZE            = 0x05040002UL, /**< 0x05040002, Invalid block size in block mode */
	//	CO_SDO_AB_SEQ_NUM               = 0x05040003UL, /**< 0x05040003, Invalid sequence number in block mode */
	//	CO_SDO_AB_CRC                   = 0x05040004UL, /**< 0x05040004, CRC error (block mode only) */
	//	CO_SDO_AB_OUT_OF_MEM            = 0x05040005UL, /**< 0x05040005, Out of memory */
	//	CO_SDO_AB_UNSUPPORTED_ACCESS    = 0x06010000UL, /**< 0x06010000, Unsupported access to an object */
	//	CO_SDO_AB_WRITEONLY             = 0x06010001UL, /**< 0x06010001, Attempt to read a write only object */
	//	CO_SDO_AB_READONLY              = 0x06010002UL, /**< 0x06010002, Attempt to write a read only object */
	//	CO_SDO_AB_NOT_EXIST             = 0x06020000UL, /**< 0x06020000, Object does not exist */
	//	CO_SDO_AB_NO_MAP                = 0x06040041UL, /**< 0x06040041, Object cannot be mapped to the PDO */
	//	CO_SDO_AB_MAP_LEN               = 0x06040042UL, /**< 0x06040042, Number and length of object to be mapped exceeds PDO length */
	//	CO_SDO_AB_PRAM_INCOMPAT         = 0x06040043UL, /**< 0x06040043, General parameter incompatibility reasons */
	//	CO_SDO_AB_DEVICE_INCOMPAT       = 0x06040047UL, /**< 0x06040047, General internal incompatibility in device */
	//	CO_SDO_AB_HW                    = 0x06060000UL, /**< 0x06060000, Access failed due to hardware error */
	//	CO_SDO_AB_TYPE_MISMATCH         = 0x06070010UL, /**< 0x06070010, Data type does not match, length of service parameter does not match */
	//	CO_SDO_AB_DATA_LONG             = 0x06070012UL, /**< 0x06070012, Data type does not match, length of service parameter too high */
	//	CO_SDO_AB_DATA_SHORT            = 0x06070013UL, /**< 0x06070013, Data type does not match, length of service parameter too short */
	//	CO_SDO_AB_SUB_UNKNOWN           = 0x06090011UL, /**< 0x06090011, Sub index does not exist */
	//	CO_SDO_AB_INVALID_VALUE         = 0x06090030UL, /**< 0x06090030, Invalid value for parameter (download only). */
	//	CO_SDO_AB_VALUE_HIGH            = 0x06090031UL, /**< 0x06090031, Value range of parameter written too high */
	//	CO_SDO_AB_VALUE_LOW             = 0x06090032UL, /**< 0x06090032, Value range of parameter written too low */
	//	CO_SDO_AB_MAX_LESS_MIN          = 0x06090036UL, /**< 0x06090036, Maximum value is less than minimum value. */
	//	CO_SDO_AB_NO_RESOURCE           = 0x060A0023UL, /**< 0x060A0023, Resource not available: SDO connection */
	//	CO_SDO_AB_GENERAL               = 0x08000000UL, /**< 0x08000000, General error */
	//	CO_SDO_AB_DATA_TRANSF           = 0x08000020UL, /**< 0x08000020, Data cannot be transferred or stored to application */
	//	CO_SDO_AB_DATA_LOC_CTRL         = 0x08000021UL, /**< 0x08000021, Data cannot be transferred or stored to application because of local control */
	//	CO_SDO_AB_DATA_DEV_STATE        = 0x08000022UL, /**< 0x08000022, Data cannot be transferred or stored to application because of present device state */
	//	CO_SDO_AB_DATA_OD               = 0x08000023UL, /**< 0x08000023, Object dictionary not present or dynamic generation fails */
	//	CO_SDO_AB_NO_DATA               = 0x08000024UL  /**< 0x08000024, No data available */
	//}SDO_ERR;

	typedef enum
	{
		SDO_MSG_TYPE_SEGMENT_DOWNLOAD_INIT_REQ = 1,
		SDO_MSG_TYPE_SEGMENT_DOWNLOAD_REQ = 0,
		SDO_MSG_TYPE_SEGMENT_UPLOAD_INIT_REQ = 2,
		SDO_MSG_TYPE_SEGMENT_UPLOAD_REQ = 3,
		SDO_MSG_TYPE_ABORT_SEGMENT_REQ = 4,

		SDO_MSG_TYPE_BLOCK_DOWNLOAD_INIT_REQ = 6,
		SDO_MSG_TYPE_BLOCK_DOWNLOAD_REQ = 5,
		SDO_MSG_TYPE_BLOCK_UPLOAD_INIT_REQ = 5,
		SDO_MSG_TYPE_BLOCK_UPLOAD_REQ = 5,
		SDO_MSG_TYPE_END_BLOCK_UPLOAD_REQ = 6,
	}SdoReqMsgType;

	typedef enum
	{
		SDO_MSG_TYPE_SEGMENT_DOWNLOAD_INIT_RSP = 3,
		SDO_MSG_TYPE_SEGMENT_DOWNLOAD_RSP = 1,
		SDO_MSG_TYPE_SEGMENT_UPLOAD_INIT_RSP = 2,
		SDO_MSG_TYPE_SEGMENT_UPLOAD_RSP = 0,

		SDO_MSG_TYPE_BLOCK_DOWNLOAD_INIT_RSP = 5,
		SDO_MSG_TYPE_BLOCK_DOWNLOAD_RSP = 5,
		SDO_MSG_TYPE_END_BLOCK_DOWNLOAD_RSP = 5,

		SDO_MSG_TYPE_BLOCK_UPLOAD_INIT_RSP = 6, 
		SDO_MSG_TYPE_BLOCK_UPLOAD_RSP = 5,
		SDO_MSG_TYPE_END_BLOCK_UPLOAD_RSP = 5,

		//自定义消息
		SDO_MSG_TYPE_BLOCK_DOWNLOAD_SEG = 7,
		SDO_MSG_TYPE_BLOCK_UPLOAD_SEG = 8,
	}SdoRspMsgType;

	#define SDO_BUF_SIZE 32
	typedef struct
	{
		CO_ODF_arg_t arg;

		/** SDO data buffer contains data, which are exchanged in SDO transfer.
		@ref CO_SDO_OD_function may verify or manipulate that data before (after)
		they are written to (read from) Object dictionary. Data have the same
		endianes as processor. Pointer must NOT be changed. (Data up to length
		can be changed.) */
		uint8           data[SDO_BUF_SIZE];

		/******************************************************/
		//For block transfer 
		uint8  seq;	//sequence number of segment 0 < seqno < 128
		uint8  blockSize;	//block size
		uint16 crc;

		uint16 crcEnable;	//
		uint16 sizeValid;	 //size indicator
		uint8 pstBytes;		//Protocol Switch Threshold 
		/******************************************************/
		//for segment transfer
		uint8 toggle;

		/******************************************************/
		uint8 isSend;		//是否需要发送帧
		//uint32 transferedLen;		//传输完成的长度
		uint8 waitForRsp;	//是否等待应答

		uint16 offsetOfData; //offset of data[SDO_BUF_SIZE] above

		/******************************************************/
		//事件回调函数
		EntryEventFn OnEvent;
		void* pObj;
	}SdoTransMgr;

	extern SDO_ERR SdoTransMgr_CopyOdDataToBuff(SdoTransMgr* pMgr);

#ifdef __cplusplus
}
#endif

#endif
