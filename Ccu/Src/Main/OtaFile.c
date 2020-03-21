
#include "Common.h"
#include "OtaFile.h"
#include "crc32.h"

OtaFile g_ccuOtaFile;
OtaFile g_pmsOtaFile;
OtaFile g_bmsOtaFile;
OtaFile g_chgOtaFile;

static OtaFile* g_OtaFiles[4];

OtaFile* OtaFiles_FindByName(const char* name)
{
	int len = 0;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_OtaFiles); i++)
	{
		OtaFile* p = g_OtaFiles[i];
		len = MIN(strlen(name), strlen(p->fileName));
		if (p && (memcmp(p->fileName, name, len) == 0))
		{
			return p;
		}
	}
	return Null;
}
OtaFile* OtaFiles_Find(uint16 index)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_OtaFiles); i++)
	{
		OtaFile* p = g_OtaFiles[i];
		if (p && p->index == index)
		{
			return p;
		}
	}
	return Null;
}

void OtaFile_Dump(const OtaFile* pOtaFile)
{
	Printf("%s Dump:\n", pOtaFile->fileName);
	Printf("\t OD=0x%04X.%02X\n", pOtaFile->index, pOtaFile->subIndex);
	Printf("\t addr=0x%08X\n", pOtaFile->addr);
	Printf("\t fileLen=0x%X(%.1f K)\n", pOtaFile->fileLen, (float)pOtaFile->fileLen/1024);
	Printf("\t maxSize=0x%04X(%.1f K)\n", pOtaFile->maxSize, (float)pOtaFile->maxSize/1024);
	if (pOtaFile->pInfoSector)
	{
		HwFwVer_Dump(&pOtaFile->pInfoSector->ver, "\t ");
		Printf("\t binDataCrc=0x%08X\n", pOtaFile->pInfoSector->binDataCrc);
		Printf("\t binDataLen=0x%04X(%.1f K)\n", pOtaFile->pInfoSector->binDataLen, (float)pOtaFile->pInfoSector->binDataLen/1024);
		Printf("\t sectorCrc =0x%04X\n", pOtaFile->pInfoSector->sectorCrc);
	}
	else
	{
		Printf("\t file info invalid.\n");
	}
}

void OtaFile_Init(OtaFile* pOtaFile, uint16 index, const char* fileName, uint32 addr, uint32 maxSize)
{
	uint32 crc = 0;
	OtaFileInfoSector* p = (OtaFileInfoSector*)(addr + maxSize - OTA_FILE_SEC_SIZE);

	memset(pOtaFile, 0, sizeof(OtaFile));

	Assert(addr % OTA_FILE_SEC_SIZE == 0);
	Assert(maxSize % OTA_FILE_SEC_SIZE == 0);
	Assert(sizeof(OtaFileInfoSector) == 128);

	pOtaFile->addr = addr;
	pOtaFile->maxSize = maxSize;
	pOtaFile->fileName = fileName;
	pOtaFile->index = index;
	pOtaFile->subIndex = 3;	//Default value

	for (int i = maxSize; i >=0; i -= OTA_FILE_SEC_SIZE, p--)
	{
		if (p->binDataCrc != 0xFFFFFFFF
			&& p->binDataLen <= maxSize
			&& p->sectorCrc != 0xFFFFFFFF
			&& (*(uint32*)&p->reserved) == 0
			)
		{
			//检验扇区CRC，是否正确
			crc = 0;
			crc = Crc32_Calc(crc, (uint8*)p, SECTOR_CRC_DATA_LEN);
			if (crc == p->sectorCrc)
			{
				crc = 0;
				//检验扇区BIN数据CRC，是否正确
				crc = Crc32_Calc(crc, (uint8*)addr, p->binDataLen);
				if (crc == p->binDataCrc)
				{
					memcpy(&pOtaFile->ver, &p->ver, sizeof(HwFwVer));
					pOtaFile->pInfoSector = p;
					pOtaFile->fileLen = (p->binDataLen + (OTA_FILE_SEC_SIZE -1)) / OTA_FILE_SEC_SIZE * OTA_FILE_SEC_SIZE + OTA_FILE_SEC_SIZE;
					break;
				}
			}
		}
	}
}

void OtaFiles_Init()
{
	int i = 0;
	#define OTA_FILE_INIT(pFile, index, name, add, maxSize) {OtaFile_Init(pFile, index, name, (uint32)add, maxSize);	g_OtaFiles[i++] = pFile;}

	Crc32_Init();

	OTA_FILE_INIT(&g_ccuOtaFile, 0x30FF, "CCU_File", &g_pMemMap->appBackUp	, 72 * 1024);
	OTA_FILE_INIT(&g_pmsOtaFile, 0x301A, "PMS_File", &g_pMemMap->pmsCode	, 128 * 1024);
	OTA_FILE_INIT(&g_chgOtaFile, 0x3021, "CHG_File", &g_pMemMap->chargerCode, 32 * 1024 );
	OTA_FILE_INIT(&g_bmsOtaFile, 0x3031, "BMS_File", &g_pMemMap->bmsCode	, 64 * 1024);
}

//Only for test
//OtaFile创建文件，Only for test
Bool OtaFile_Create(OtaFile* pOtaFile, uint32 size)
{
	uint32 addr = pOtaFile->addr;
	uint8 buff[128];
	for (int i = 0; i < sizeof(buff); i++)
	{
		buff[i] = i;
	}

	int allignLen = (size + OTA_FILE_SEC_SIZE - 1) / OTA_FILE_SEC_SIZE * OTA_FILE_SEC_SIZE;
	if (allignLen > pOtaFile->maxSize) return False;

	uint32 crc = 0;
	FmcErase(addr, pOtaFile->maxSize);
	int j = 0;
	for (j = sizeof(buff); j <= size; j += sizeof(buff))
	{
		crc = Crc32_Calc(crc, buff, sizeof(buff));
		FmcWrite(addr, buff, sizeof(buff));
		addr += sizeof(buff);
	}
	int len = size % sizeof(buff);
	if (len)
	{
		crc = Crc32_Calc(crc, buff, len);
		len = (len + 1) >> 1 << 1; //格式化为偶数
		FmcWrite(addr, buff, len);
		addr += sizeof(buff);
	}

	int testCrc = 0;
	testCrc = Crc32_Calc(testCrc, (uint8*)pOtaFile->addr, size);
	Assert(crc == testCrc);

	const static HwFwVer ver = { 1,2,3,4,5,7890 };
	OtaFileInfoSector * p = (OtaFileInfoSector*)buff;
	memset(buff, 0, sizeof(buff));

	p->binDataCrc = crc;
	p->binDataLen = size;
	memcpy(&p->ver, &ver, sizeof(ver));

	crc = 0;
	crc = Crc32_Calc(crc, (uint8*)p, SECTOR_CRC_DATA_LEN);
	p->sectorCrc = crc;
	FmcWrite(addr, buff, OTA_FILE_SEC_SIZE);

	pOtaFile->pInfoSector = (OtaFileInfoSector*)addr;
	pOtaFile->fileLen = allignLen + OTA_FILE_SEC_SIZE;
	return True;
}

void OtaFile_Tester(const char* name, uint8 isWrite, uint32 fileLen)
{
	OtaFile* p = g_OtaFiles[0];
	for (int i = 0; i < GET_ELEMENT_COUNT(g_OtaFiles); i++, p++)
	{
		p = g_OtaFiles[i];
		if (p && strcmp(name, p->fileName) == 0)
		{
			if (isWrite)
			{
				if (fileLen <= p->maxSize - OTA_FILE_SEC_SIZE)
				{
					OtaFile_Create(p, fileLen);
					Printf("%s create file len[%d] success.", p->fileName, fileLen);
				}
				else
				{
					Printf("%s file len[%d] error.", p->fileName, fileLen);
				}
				return;
			}
			else
			{
				OtaFile_Init(p, p->index, name, p->addr, p->maxSize);
			}
			break;
		}
	}

	AssertTrue(p->pInfoSector);
}
