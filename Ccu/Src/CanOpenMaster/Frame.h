
#ifndef _CAN_OPEN_FRAME_H_
#define _CAN_OPEN_FRAME_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
	/* Should not be modified */
#define Rx 0
#define Tx 1

/* TODO: remove this limitation. do bit granularity mapping */
#define PDO_MAX_LEN 8    

/** Status of the SDO transmission
 */
#define SDO_RESET                0x0      /* Transmission not started. Init state. */
#define SDO_FINISHED             0x1      /* data are available */                          
#define	SDO_ABORTED_RCV          0x80     /* Received an abort message. Data not available */
#define	SDO_ABORTED_INTERNAL     0x85     /* Aborted but not because of an abort message. */
#define	SDO_DOWNLOAD_IN_PROGRESS 0x2 
#define	SDO_UPLOAD_IN_PROGRESS   0x3   

 /* Status of the node during the SDO transfert : */
#define SDO_SERVER  0x1
#define SDO_CLIENT  0x2
#define SDO_UNKNOWN 0x3             

/*  Function Codes
   ---------------
  defined in the canopen DS301
*/
#define FNMT		   0x0
#define FSYNC      0x1
#define EMER       0x1
#define TIME_STAMP 0x2
#define PDO1tx     0x3
#define PDO1rx     0x4
#define PDO2tx     0x5
#define PDO2rx     0x6
#define PDO3tx     0x7
#define PDO3rx     0x8
#define PDO4tx     0x9
#define PDO4rx     0xA
#define SDOtx      0xB
#define SDOrx      0xC
#define NODE_GUARD 0xE
#define FHEARTBEAT  0xE

//PMS帧类型扩展
#define PDO5tx     0xF
#define PDO6tx     0x10
#define PDO7tx     0x11
#define PDO8tx     0x12
#define PDO9tx     0x13
#define PDO10tx    0x14
#define PDO11tx    0x15
#define PDO12tx    0x16

/* NMT Command Specifier, sent by master to change a slave state */
/* ------------------------------------------------------------- */
/* Should not be modified */
#define NMT_Start_Node              0x01
#define NMT_Stop_Node               0x02
#define NMT_Enter_PreOperational    0x80
#define NMT_Reset_Node              0x81
#define NMT_Reset_Comunication      0x82



/* constantes used in the different state machines */
/* ----------------------------------------------- */
/* Must not be modified */
#define state1  0x01
#define state2  0x02
#define state3  0x03
#define state4  0x04
#define state5  0x05
#define state6  0x06
#define state7  0x07
#define state8  0x08
#define state9  0x09
#define state10 0x0A
#define state11 0x0B
	
#pragma anon_unions
#pragma pack(1)
	typedef union
	{
		struct
		{
			uint16 nodeId : 7;
			uint16 type : 4;
		};
		uint16 ident;
	}CAN_SFID;

	typedef union
	{
		struct
		{
			uint32_t efid : 18;		//扩展帧ID
			uint32_t sfid : 7;		//标准帧ID
			uint32_t type : 4;		//标准帧ID
			uint32_t reserved : 3;	//保留
		};
		uint32_t			ident;	//扩展帧ID
	}CAN_EFID;

	typedef struct
	{
		union
		{
			struct
			{
				uint16 nodeId : 7;
				uint16 type : 4;
			};
			uint32 ident;
		};
		uint8  rtr;			/* remote transmission request. 0 if not rtr, 1 for a rtr message */
		uint8  len;			/* message length (0 to 8) */
		uint8  data[8];	/* data */
	}Frame;
	#define FRAME_HEAD_SIZE 6	//帧头长度，从ident到包含len的长度的

	typedef struct
	{
		union
		{
			struct
			{
				uint16 nodeId : 6;
				uint16 type : 5;
			};
			uint32 ident;
		};
		uint8  rtr;			/* remote transmission request. 0 if not rtr, 1 for a rtr message */
		uint8  len;			/* message length (0 to 8) */
		uint8  data[8];	/* data */
	}FrameEx;

	//CanOpen B format
	typedef struct
	{
		union
		{
			struct
			{
				uint32_t efid : 18;		//扩展帧ID
				uint32_t sfid : 7;		//标准帧ID
				uint32_t type : 4;		//标准帧ID
				uint32_t reserved : 3;	//保留
			};
			uint32 ident;
		};
		uint8  rtr;			/* remote transmission request. 0 if not rtr, 1 for a rtr message */
		uint8  len;			/* message length (0 to 8) */
		uint8  data[8];	/* data */
	}FrameB;
#pragma pack()

	typedef struct
	{
		uint8 type;
		uint8 value;
		const char* typeStr;
	}FrameTypeMap;

	const char* Frame_GetTypeString(uint8 type);
	uint8 FrameEx_GetType(uint32 ident);
	void Frame_Dump(Frame* p, uint32 level, const char* tag);
	void FrameEx_Dump(FrameEx* p, uint32 level, const char* tag);

#ifdef __cplusplus
}
#endif

#endif
