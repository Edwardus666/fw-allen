
#ifndef _OTA_FILE_H_
#define _OTA_FILE_H_

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"
#include "HwFwVer.h"
#define  OTA_FILE_SEC_SIZE 128
#pragma pack(1) 
	typedef struct
	{
		uint32 binDataCrc;
		uint32 binDataLen;
		HwFwVer ver;
		uint8 reserved[OTA_FILE_SEC_SIZE - sizeof(HwFwVer) - 12];
		uint32 sectorCrc;
	}OtaFileInfoSector;
	#define SECTOR_CRC_DATA_LEN (sizeof(HwFwVer) + 8)

#pragma pack() 
	typedef struct
	{
		const char* fileName;
		uint16 index;
		uint16 subIndex;
		uint32 addr;
		uint32 maxSize;
		OtaFileInfoSector* pInfoSector;
		uint32 fileLen;
		HwFwVer ver;		//Ó²¼þ/¹Ì¼þ°æ±¾ºÅ
	}OtaFile;

	extern OtaFile g_ccuOtaFile;
	extern OtaFile g_pmsOtaFile;
	extern OtaFile g_bmsOtaFile;
	extern OtaFile g_chgOtaFile;

	void OtaFiles_Init();
	void OtaFile_Tester(const char* name, uint8 isWrite, uint32 fileLen);
	Bool OtaFile_Create(OtaFile* pOtaFile, uint32 size);
	OtaFile* OtaFiles_Find(uint16 index);
	OtaFile* OtaFiles_FindByName(const char* name);
	void OtaFile_Dump(const OtaFile* pOtaFile);

#ifdef __cplusplus
}
#endif

#endif
