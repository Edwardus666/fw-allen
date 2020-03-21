#include <common.h>
#include "Frame.h"

const static FrameTypeMap g_FrameTypeMap[] =
{
	 {FNMT		,0x0 , "NMT"		}
	,{FSYNC		,0x2 , "SYNC"		}
	,{TIME_STAMP,0x4 , "TIME_STAMP"	}

	,{PDO1tx	,0x6 , "TPDO1"		}
	,{PDO2tx	,0x7 , "TPDO2"		}

	,{PDO1rx	,0x8 , "RPDO1"		}

	,{PDO3tx	,0x9 , "TPDO3"		}
	,{PDO4tx	,0xA , "TPDO4"		}
	,{PDO5tx	,0xB , "TPDO5"		}

	,{PDO2rx	,0xC , "RPDO2"		}

	,{PDO6tx	,0xD , "TPDO6"		}
	,{PDO7tx	,0xE , "TPDO7"		}
	,{PDO8tx	,0xF,  "TPDO8"		}

	,{PDO3rx	,0x10 , "RPDO3"		}

	,{PDO9tx	,0x11, "TPDO9"		}
	,{PDO10tx	,0x12, "TPDO10"		}
	,{PDO11tx	,0x13, "TPDO11"		}

	,{PDO4rx	,0x14, "RPDO4"		}

	,{PDO12tx	,0x15, "TPDO12"		}

	,{SDOtx 	,0x16, "TSDO"		}
	,{SDOrx 	,0x18, "RSDO "		}
	,{FHEARTBEAT,0x1C, "HEARTBEAT "	}
};
 
const char* Frame_GetTypeString(uint8 type)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_FrameTypeMap); i++)
	{
		if (g_FrameTypeMap[i].type == type) return g_FrameTypeMap[i].typeStr;
	}
	return "FRAME_TYPE_ERROR";
}
const char* FrameEx_GetTypeString(uint8 type)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_FrameTypeMap); i++)
	{
		if (g_FrameTypeMap[i].value == type) return g_FrameTypeMap[i].typeStr;
	}
	return "FRAME_TYPE_ERROR";
}

uint8 FrameEx_GetType(uint32 ident)
{
	//PMS的帧类型定制, 高5bit.
	int value = (ident >> 6) & 0x1F;
	const FrameTypeMap * p = g_FrameTypeMap;
	for (int i = 0; i < GET_ELEMENT_COUNT(g_FrameTypeMap); i++, p++)
	{
		if (p->value == value) return p->type;
	}
	return 0;
}

void Frame_Dump(Frame* p, uint32 level, const char* tag)
{
	if ((level & g_dwDebugLevel) == 0) return;
	if (tag)
	{
		Printf("%s", tag);
	}
	DUMP_BYTE_EX(("%s \tident[0x%08X(%X.%X)] len[%d]:", Frame_GetTypeString(p->type), p->ident, p->type, p->nodeId, p->len), p->data, p->len);
}

void FrameEx_Dump(FrameEx* p, uint32 level, const char* tag)
{
	if ((level & g_dwDebugLevel) == 0) return;

	if (tag)
	{
		Printf("%s", tag);
	}
	DUMP_BYTE_EX(("%s \tident[0x%08X(%X.%X)] len[%d]:", FrameEx_GetTypeString(p->type), p->ident, p->type, p->nodeId, p->len), p->data, p->len);
}
