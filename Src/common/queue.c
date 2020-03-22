
#include "Common.h"
#include "ArchDef.h"
#include "queue.h"

void QUEUE_Dump(Queue* queue)
{
	Printf("Queue Dump(@0x%x)\n"	, queue);
	Printf("\t m_ReadPointer=%d\n"	, queue->m_ReadPointer);
	Printf("\t m_WritePointer=%d\n"	, queue->m_WritePointer);
	Printf("\t m_nElementSize=%d\n"	, queue->m_nElementSize);
	Printf("\t m_nBufferSize=%d\n"	, queue->m_nBufferSize);
	Printf("\t m_pBuffer=0x%x\n"	, queue->m_pBuffer);
}

void* QUEUE_getNew(Queue* queue)
{
	void* pElement = Null;
	
	if (queue->m_WritePointer == queue->m_ReadPointer)
	{
		queue->m_WritePointer = 0;
		queue->m_ReadPointer = 0;
	}
	else if (queue->m_WritePointer >= queue->m_nBufferSize)
	{
		return Null;	//Full
	}

	pElement = &queue->m_pBuffer[queue->m_WritePointer];
	queue->m_WritePointer += queue->m_nElementSize;
	return pElement;
}

Bool QUEUE_add(Queue* queue, const void* element, int len)
{
	if (len <= queue->m_nElementSize)
	{
		void* pData = QUEUE_getNew(queue);
		if (pData)
		{
			memcpy(pData, element, len);
			return True;
		}
	}
	return False;
}

void* QUEUE_getHead(Queue* queue)
{
	if (queue->m_ReadPointer == queue->m_WritePointer)
	{
		return Null;
	}
	
	return &queue->m_pBuffer[queue->m_ReadPointer];
}

//Pop the head element;
void QUEUE_removeHead(Queue* queue)
{
	if(queue->m_ReadPointer == queue->m_WritePointer)
	{
		return;
	}
	
	queue->m_ReadPointer += queue->m_nElementSize;
}

void QUEUE_removeAll(Queue* queue)
{
	queue->m_ReadPointer = 0;
	queue->m_WritePointer = 0;
}

//Get the head element and pop it
void* QUEUE_popGetHead(Queue* queue)
{
	void* pvalue = QUEUE_getHead(queue);
	QUEUE_removeHead(queue);
	return pvalue;
}


//Is queue empty 
//return: 1=Empty 0=Not Empty
Bool QUEUE_isEmpty(Queue* queue)
{
	return queue->m_WritePointer == queue->m_nElementSize;
}

Bool QUEUE_isFull(Queue* queue)
{
	return queue->m_WritePointer >= queue->m_nElementSize;
}

Bool QUEUE_init(Queue* queue, void* pBuffer, unsigned short itemSize, unsigned int itemCount)
{
	memset(queue, 0, sizeof(Queue));

	queue->m_pBuffer = pBuffer;	
	queue->m_nElementSize = itemSize;
	queue->m_nBufferSize = itemSize * itemCount;

	return True;
}

void QUEUE_reset(Queue* queue)
{
	queue->m_ReadPointer = 0;
	queue->m_WritePointer = 0;
	memset(queue->m_pBuffer, 0, queue->m_nBufferSize);
}
