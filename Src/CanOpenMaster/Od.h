/*
 * CANopen Object Dictionary.
 */


#ifndef CO_OD_H_
#define CO_OD_H_

#include "typedef.h"
#include "CO_OD_Def.h"

#define ENTRY_NOT_EXST 0xFFFFU
#define CO_SDO_BUFFER_SIZE  32	//domain buffer dataLength


//更新数据字典源定义，
typedef enum
{
	EUS_SLAVE,		//记录被SLAVE节点更新
	EUS_MASTER,		//记录被Master更新
	EUS_UNDEFINEED	//没定义
}EntryUpdateSrc;

//数据字典改变事件函数，当返回SDO_SUCCESS时，完成改变，否则放弃改变。
typedef int (*EntryChangedFn) (void* pObj, EntryUpdateSrc updateSrc
	, uint16 index, uint8 subIndex
	, const uint8* oldValue, uint8* newValue, int len);


void Entry_SetValue(const CO_OD_entry* object, uint8 subIndex, uint8* pData, int len);
void CO_OD_ResetValue(const CO_OD_entry* pOdArray, int odSize);
const CO_OD_entry* OD_find(const CO_OD_entry* pOdArray, int odSize, uint16_t index, uint16* entryNo);

uint16_t Entry_getLength(const CO_OD_entry* object, uint8_t subIndex);
uint16_t Entry_getAttribute(const CO_OD_entry* object, uint8_t subIndex);
void*    Entry_getDataPointer(const CO_OD_entry_t* object, uint8_t subIndex);
const CO_OD_entry* OD_findNext(const CO_OD_entry* pOdArray, int odSize, uint16_t* index, uint8* subIndex);

/*
 * Find mapped variable in Object Dictionary.
 *
 * Function is called from CO_R(T)PDOconfigMap or when mapping parameter changes.
 *
 * @param SDO SDO object.
 * @param map PDO mapping parameter.
 * @param R_T 0 for RPDO map, 1 for TPDO map.
 * @param ppData Pointer to returning parameter: pointer to data of mapped variable.
 * @param pLength Pointer to returning parameter: *add* length of mapped variable.
 * @param pSendIfCOSFlags Pointer to returning parameter: sendIfCOSFlags variable.
 * @param pIsMultibyteVar Pointer to returning parameter: true for multibyte variable.
 *
 * @return 0 on success, otherwise SDO abort code.
 */
uint32_t OD_PDOfindMap(const CO_OD_entry* pOdArray, int odSize,
	uint32_t                map,
	uint8_t                 R_T,
	uint8_t** ppData,
	uint8_t* pLength,
	uint8_t* pSendIfCOSFlags,
	uint8_t* pIsMultibyteVar);

void OD_configure(
	const CO_OD_entry* pOdArray, int odSize,
	CO_OD_extension_t* extArray,
	uint16_t                index,
	CO_SDO_abortCode_t(*pODFunc)(CO_ODF_arg_t* ODF_arg),
	void* object,
	uint8_t* flags,
	uint8_t                 flagsSize);

void* OD_GetValue(const CO_OD_entry* pOdArray, int odSize, uint16_t index, uint8_t subIndex, uint16_t* len);

typedef struct
{
	EntryEventFn OnEvent;
}EntryExt;

void OdRecord_Init(CO_OD_entryRecord_t* p, void* pData, uint16_t attribute, uint16_t length);
//void Entry_Init(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData);
//void Entry_Init1(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData
//	, EntryExt* pEntryExtArray, EntryEventFn OnChanged);
//
//EntryExt* EntryExt_Find(EntryExt* pEntryExtArray, int dataLength, uint16_t index);
//uint16 EntryExt_GetCount(EntryExt* pEntryExtArray, int dataLength);

//记录初始化
#define RECORD_INIT(recordArray, maxSubIndex) \
	maxSubIndex = GET_ELEMENT_COUNT(recordArray) - 1;	\
	pRec = recordArray;	\
	OdRecord_Init(pRec++, &maxSubIndex, 0x06, 1);

void OdExtension_Init(CO_OD_extension_t* p, ODExtenFunc pODFunc, void* object, uint8_t* flags);

//void Entry_Init2(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData
//	, CO_OD_extension_t* pEntryExt, ODExtenFunc OnChanged, void* object);
//
//void Entry_Init3(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData
//	, EntryExt* pEntryExt, EntryEventFn OnChanged
//	, CO_OD_extension_t* pOdExt, ODExtenFunc OnExternChanged, void* object);
//
//void Entry_CountReset();
//int Entry_GetCount();
void Entry_Dump(const CO_OD_entry_t* pEntry, uint8 addr, const char* name);
CO_OD_entry_t* Od_GetEntry(const CO_OD_entry* pOd, int dataLength, uint16 index, uint8 subInd);

uint32 Od_GetEntryValueAsUint32(const CO_OD_entry* pOd, int dataLength, uint16 index, uint8 subInd);

#endif

