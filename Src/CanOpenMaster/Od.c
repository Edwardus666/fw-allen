
#include "Common.h"
#include "Od.h"
#include "Emcy.h"
#include "CO_OD_Def.h"

/******************************************************************************/
const CO_OD_entry* OD_find(const CO_OD_entry* pOdArray, int odSize, uint16_t index, uint16* entryNo) {
	/* Fast search in ordered Object Dictionary. If indexes are mixed, this won't work. */
	/* If Object Dictionary has up to 2^N entries, then N is max number of loop passes. */
	uint16_t cur, min, max;
	const CO_OD_entry_t* object;

	min = 0U;
	max = odSize - 1U;
	while (min < max) {
		cur = (min + max) / 2;
		object = &pOdArray[cur];
		/* Is object matched */
		if (index == object->index) {
			if(entryNo) *entryNo = cur;
			return object;
		}
		if (index < object->index) {
			max = cur;
			if (max) max--;
		}
		else
			min = cur + 1U;
	}

	if (min == max) {
		object = &pOdArray[min];
		/* Is object matched */
		if (index == object->index) {
			if (entryNo) *entryNo = min;
			return object;
		}
	}

	*entryNo = ENTRY_NOT_EXST;
	return Null;  /* object does not exist in OD */
}

const CO_OD_entry* OD_findNext(const CO_OD_entry* pOdArray, int odSize, uint16_t* index, uint8* subIndex)
{
	/*
	CO_OD_entry* p = Null;
	uint16 entryNo = 0;
	if (*index == 0 && *subIndex == 0)
	{
		*index = p->index;
		*subIndex = p->maxSubIndex;
		p = pOdArray;
		return p;
	}
	else
	{
		p = OD_find(pOdArray, odSize, *index, &entryNo);
	}
*/
	uint16 entryNo = 0;
	const CO_OD_entry* p = OD_find(pOdArray, odSize, *index, &entryNo);
	if (p->maxSubIndex > *subIndex)
	{
		(*subIndex)++;
		return p;
	}
	else if(entryNo < odSize - 1)
	{
		(entryNo)++;
		p = &pOdArray[entryNo];
		*index = p->index;

		if (p->maxSubIndex == 0U) {    /* Object type is Var or domain */
			*subIndex = 0;
		}
		else if (p->attribute != 0U) { /* Object type is Array or domain */
			*subIndex = 0;
		}
		else {                            /* Object type is Record */
			*subIndex = 1;
		}

		return p;
	}

	return Null;
}

void* OD_GetValue(const CO_OD_entry* pOdArray, int odSize, uint16_t index, uint8_t subIndex, uint16_t* len)
{
	const CO_OD_entry* p = OD_find(pOdArray, odSize, index, Null);
	if (p == Null) return 0;

	if(len)
		*len = Entry_getLength(p, subIndex);

	return Entry_getDataPointer(p, subIndex);
}

void Entry_ResetValue(const CO_OD_entry* object)
{
	if (object->maxSubIndex == 0U) {    /* Object type is Var */
		if (object->pData == 0) { /* data type is domain */
			return ;
		}
		else {
			memset(object->pData, 0, object->length);
		}
	}
	else if (object->attribute != 0U) { /* Object type is Array */
		if (object->pData == 0) {
			/* data type is domain */
		}
		else {/* Object type is Array */
			memset(object->pData, 0, object->length);
		}
	}
	else {                            /* Object type is Record */
		CO_OD_entryRecord* pRecord = (CO_OD_entryRecord*)object->pData;
		for (int i = 1; i < object->maxSubIndex; i++, pRecord++)
		{
			if (pRecord->pData)
			{
				memset(pRecord->pData, 0, pRecord->length);
			}
		}
	}
}

void Entry_SetValue(const CO_OD_entry* object, uint8 subIndex, uint8* pData, int len)
{
	len = (len > object->length) ? object->length : len;
	if (object->maxSubIndex == 0U) {    /* Object type is Var */
		if (object->pData == 0) { /* data type is domain */
			return;
		}
		else {
			memcpy(object->pData, pData, object->length);
		}
	}
	else if (object->attribute != 0U) { /* Object type is Array */
		if (object->pData == 0) {
			/* data type is domain */
		}
		else {/* Object type is Array */
			memcpy(object->pData, pData, object->length);
		}
	}
	else {                            /* Object type is Record */
		CO_OD_entryRecord* pRecord = (CO_OD_entryRecord*)object->pData;
		if(subIndex < object->maxSubIndex)
		{
			pRecord = &pRecord[subIndex];
			memcpy(pRecord->pData, pData, pRecord->length);
		}
	}
}

void CO_OD_ResetValue(const CO_OD_entry* pOdArray, int odSize)
{
	for (int i = 0; i < odSize; i++, pOdArray++)
	{
		Entry_ResetValue(pOdArray);
	}
}

/******************************************************************************/
uint16_t Entry_getLength(const CO_OD_entry* object, uint8_t subIndex) {
	if (object->maxSubIndex == 0U) {    /* Object type is Var */
		if (object->pData == 0) { /* data type is domain */
			return CO_SDO_BUFFER_SIZE;
		}
		else {
			return object->length;
		}
	}
	else if (object->attribute != 0U) { /* Object type is Array */
		if (subIndex == 0U) {
			return 1U;
		}
		else if (object->pData == 0) {
			/* data type is domain */
			return CO_SDO_BUFFER_SIZE;
		}
		else {
			return object->length;
		}
	}
	else {                            /* Object type is Record */
		if (((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData == 0) {
			/* data type is domain */
			return CO_SDO_BUFFER_SIZE;
		}
		else {
			return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].length;
		}
	}
}

/******************************************************************************/
uint16_t Entry_getAttribute(const CO_OD_entry* object, uint8_t subIndex) {
	if (object->maxSubIndex == 0U) {   /* Object type is Var */
		return object->attribute;
	}
	else if (object->attribute != 0U) {/* Object type is Array */
		Bool exception_1003 = False;
		uint16_t attr = object->attribute;

		/* Special exception: Object 1003,00 should be writable */
		if (object->index == 0x1003 && subIndex == 0) {
			exception_1003 = True;
			attr |= CO_ODA_WRITEABLE;
		}

		if (subIndex == 0U && exception_1003 == False) {
			/* First subIndex is readonly */
			attr &= ~(CO_ODA_WRITEABLE | CO_ODA_RPDO_MAPABLE);
			attr |= CO_ODA_READABLE;
		}
		return attr;
	}
	else {                            /* Object type is Record */
		const CO_OD_entryRecord_t* p = (const CO_OD_entryRecord_t*)(object->pData);
		p += subIndex;
		return p->attribute;
		//return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].attribute;
	}
}

/******************************************************************************/
void* Entry_getDataPointer(const CO_OD_entry_t* object, uint8_t subIndex) {
	if (object->maxSubIndex == 0U) {   /* Object type is Var */
		return object->pData;
	}
	else if (object->attribute != 0U) {/* Object type is Array */
		if (subIndex == 0) {
			/* this is the data, for the subIndex 0 in the array */
			return (void*)& object->maxSubIndex;
		}
		else if (object->pData == 0) {
			/* data type is domain */
			return 0;
		}
		else {
			return (void*)(((int8_t*)object->pData) + ((subIndex - 1) * object->length));
		}
	}
	else {                            /* Object Type is Record */
		return ((const CO_OD_entryRecord_t*)(object->pData))[subIndex].pData;
	}
}

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
	uint8_t* pIsMultibyteVar)
{
	uint16_t entryNo;
	uint16_t index;
	uint8_t subIndex;
	uint8_t dataLen;
	uint8_t objectLen;
	uint8_t attr;

	index = (uint16_t)(map >> 16);
	subIndex = (uint8_t)(map >> 8);
	dataLen = (uint8_t)map;   /* data length in bits */

	/* data length must be byte aligned */
	if (dataLen & 0x07) return CO_SDO_AB_NO_MAP;   /* Object cannot be mapped to the PDO. */

	dataLen >>= 3;    /* new data length is in bytes */
	*pLength += dataLen;

	/* total PDO length can not be more than 8 bytes */
	if (*pLength > 8) return CO_SDO_AB_MAP_LEN;  /* The number and length of the objects to be mapped would exceed PDO length. */

	/* is there a reference to dummy entries */
	if (index <= 7 && subIndex == 0) {
		static uint32_t dummyTX = 0;
		static uint32_t dummyRX;
		uint8_t dummySize = 4;

		if (index < 2) dummySize = 0;
		else if (index == 2 || index == 5) dummySize = 1;
		else if (index == 3 || index == 6) dummySize = 2;

		/* is dataLength of variable big enough for map */
		if (dummySize < dataLen) return CO_SDO_AB_NO_MAP;   /* Object cannot be mapped to the PDO. */

		/* Data and ODE pointer */
		if (R_T == 0) * ppData = (uint8_t*)& dummyRX;
		else         *ppData = (uint8_t*)& dummyTX;

		return 0;
	}

	/* find object in Object Dictionary */
	const CO_OD_entry* entry = OD_find(pOdArray, odSize, index, &entryNo);

	/* Does object exist in OD? */
	if (entry == Null)
		return CO_SDO_AB_NOT_EXIST;   /* Object does not exist in the object dictionary. */

	attr = Entry_getAttribute(entry, subIndex);
	/* Is object Mappable for RPDO? */
	if (R_T == 0 && !((attr & CO_ODA_RPDO_MAPABLE) && (attr & CO_ODA_WRITEABLE))) return CO_SDO_AB_NO_MAP;   /* Object cannot be mapped to the PDO. */
	/* Is object Mappable for TPDO? */
	if (R_T != 0 && !((attr & CO_ODA_TPDO_MAPABLE) && (attr & CO_ODA_READABLE))) return CO_SDO_AB_NO_MAP;   /* Object cannot be mapped to the PDO. */

	/* is dataLength of variable big enough for map */
	objectLen = Entry_getLength(entry, subIndex);
	if (objectLen < dataLen) return CO_SDO_AB_NO_MAP;   /* Object cannot be mapped to the PDO. */

	/* mark multibyte variable */
	*pIsMultibyteVar = (attr & CO_ODA_MB_VALUE) ? 1 : 0;

	/* pointer to data */
	*ppData = (uint8_t*)Entry_getDataPointer(entry, subIndex);
#ifdef CO_BIG_ENDIAN
	/* skip unused MSB bytes */
	if (*pIsMultibyteVar) {
		*ppData += objectLen - dataLen;
	}
#endif

	/* setup change of state flags */
	if (attr & CO_ODA_TPDO_DETECT_COS) {
		int16_t i;
		for (i = *pLength - dataLen; i < *pLength; i++) {
			*pSendIfCOSFlags |= 1 << i;
		}
	}

	return 0;
}

/******************************************************************************/
//uint8_t* Entry_getFlagsPointer(CO_SDO_t * SDO, uint16_t entryNo, uint8_t subIndex) {
//	CO_OD_extension_t* ext;
//
//	if ((entryNo == 0xFFFFU) || (SDO->ODExtensions == 0)) {
//		return 0;
//	}
//
//	ext = &SDO->ODExtensions[entryNo];
//
//	return &ext->flags[subIndex];
//}

/******************************************************************************/
void OD_configure(
	const CO_OD_entry* pOdArray, int odSize,
	CO_OD_extension_t* extArray,
	uint16_t                index,
	CO_SDO_abortCode_t(*pODFunc)(CO_ODF_arg_t* ODF_arg),
	void* object,
	uint8_t* flags,
	uint8_t                 flagsSize)
{
	uint16_t entryNo;

	const CO_OD_entry* pEntry = OD_find(pOdArray, odSize, index, &entryNo);
	if (entryNo < 0xFFFFU) {
		CO_OD_extension_t* ext = &extArray[entryNo];
		uint8_t maxSubIndex = pEntry->maxSubIndex;
		ext->pODFunc = pODFunc;
		ext->object = object;
		if ((flags != NULL) && (flagsSize != 0U) && (flagsSize == maxSubIndex)) {
			uint16_t i;
			ext->flags = flags;
			for (i = 0U; i <= maxSubIndex; i++) {
				ext->flags[i] = 0U;
			}
		}
		else {
			ext->flags = NULL;
		}
	}
}

void OdRecord_Init(CO_OD_entryRecord_t* p, void* pData, uint16_t attribute, uint16_t length)
{
	p->pData = pData;
	p->attribute = attribute;
	p->length = length;
}
//
//static int g_EntriesCount = 0;	//统计Entry数量
//void Entry_CountReset() { g_EntriesCount = 0; }
//int Entry_GetCount() {return g_EntriesCount; }
//void Entry_Init(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData)
//{
//	g_EntriesCount++;
//	p->index = index;
//	p->maxSubIndex = maxSubIndex;
//	p->attribute = attribute;
//	p->length = length;
//	p->pData = pData;
//}
//
//void Entry_Init1(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData
//	, EntryExt* pEntryExt, EntryEventFn OnChanged)
//{
//	Entry_Init(p, index, maxSubIndex, attribute, length, pData);
//
//	pEntryExt->index = index;
//	pEntryExt->OnEvent = OnChanged;
//}
//
//void Entry_Init2(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData
//	, CO_OD_extension_t* pEntryExt, ODExtenFunc OnEvent, void* object)
//{
//	pEntryExt += g_EntriesCount;
//	Entry_Init(p, index, maxSubIndex, attribute, length, pData);
//
//	pEntryExt->pODFunc = OnEvent;
//	pEntryExt->object = object;
//}
//
//void Entry_Init3(CO_OD_entry_t* p, uint16_t index, uint8_t maxSubIndex, uint16_t attribute, uint16_t length, void* pData
//	, EntryExt* pEntryExt, EntryEventFn OnEvent
//	, CO_OD_extension_t* pOdExt, ODExtenFunc OnExternChanged, void* object)
//{
//	pOdExt += g_EntriesCount;
//	Entry_Init(p, index, maxSubIndex, attribute, length, pData);
//
//	pEntryExt->index = index;
//	pEntryExt->OnEvent = OnEvent;
//
//	pOdExt->pODFunc = OnExternChanged;
//	pOdExt->object = object;
//}

void OdExtension_Init(CO_OD_extension_t* p, ODExtenFunc pODFunc, void* object, uint8_t* flags)
{
	p->pODFunc = pODFunc;
	p->object = object;
	p->flags = flags;
}

//uint16 EntryExt_GetCount(EntryExt* pEntryExtArray, int dataLength)
//{
//	for (int i = 0; i < dataLength; i++, pEntryExtArray++)
//	{
//		if (pEntryExtArray->index == 0) return i;
//	}
//	return 0;
//}

//EntryExt* EntryExt_Find(EntryExt* pEntryExtArray, int dataLength, uint16_t index)
//{
//	int mid;
//	int minInd = 0;
//	int maxInd = dataLength - 1;
//
//	if (dataLength == 1) return (pEntryExtArray[0].index == index) ? pEntryExtArray : Null;
//	if (index < pEntryExtArray[minInd].index) return Null;
//	if (index > pEntryExtArray[maxInd].index) return Null;
//
//	if (index == pEntryExtArray[minInd].index) return &pEntryExtArray[minInd];
//	if (index == pEntryExtArray[maxInd].index) return &pEntryExtArray[maxInd];
//
//	for (; (maxInd - minInd) != 1;)
//	{
//		mid = (maxInd + minInd) >> 1;//mid是n/2；
//		if (index > pEntryExtArray[mid].index)
//		{
//			minInd = mid;
//		}
//		else if(index < pEntryExtArray[mid].index)
//		{
//			maxInd = mid;
//		}
//		else
//		{
//			return &pEntryExtArray[mid];
//		}
//	}
//
//	return Null;
//}

CO_OD_entry_t* Od_GetEntry(const CO_OD_entry* pOd, int odSize, uint16 index, uint8 subInd)
{
	static CO_OD_entry_t entry;
	const CO_OD_entry* pEntry = OD_find((CO_OD_entry*)pOd, odSize, index, Null);

	if (pEntry)
	{
		entry.index = index;
		entry.maxSubIndex = subInd;
		entry.length = Entry_getLength(pEntry, subInd);
		entry.attribute = Entry_getAttribute(pEntry, subInd);
		entry.pData = Entry_getDataPointer(pEntry, subInd);
		return &entry;
	}
	else
	{
		return Null;
	}
}

uint32 Od_GetEntryValueAsUint32(const CO_OD_entry* pOd, int dataLength, uint16 index, uint8 subInd)
{
	const CO_OD_entry* p = OD_find((CO_OD_entry*)pOd, dataLength, index, Null);
	if (p)
	{
		return *(uint32*)Entry_getDataPointer(p, subInd);
	}
	return 0;
}


void Entry_Dump(const CO_OD_entry_t* pEntry, uint8 addr, const char* name)
{
	uint32* pVal = (uint32*)pEntry->pData;
	if (name)
	{
		Printf("%s[%d] OD[%04X.%02X] Len[%d]=", name, addr, pEntry->index, pEntry->maxSubIndex, pEntry->length);
	}
	else
	{
		Printf("Node[%d] OD[%04X.%02X] Len[%d]=", addr, pEntry->index, pEntry->maxSubIndex, pEntry->length);
	}


	if (pEntry->length == 1)
	{
		Printf("0x%02X (%d)\n", (uint8)* pVal, (uint8)* pVal);
	}
	else if (pEntry->length == 2)
	{
		Printf("0x%04X (%d)\n", (uint16)* pVal, (uint16)* pVal);
	}
	else if (pEntry->length == 4)
	{
		Printf("0x%08X (%d)\n", (uint32)* pVal, (uint32)* pVal);
	}
	else
	{
		DUMP_BYTE(pEntry->pData, pEntry->length);
	}
}
