

#ifndef __QUEUE_H__
#define __QUEUE_H__    

#ifdef __cplusplus
extern "C"{
#endif

#include "typedef.h"

#define INVALID_POS		0xFFFFFFFF
#define THE_FIRST_POS	0xFFFFFFFE

typedef struct _Queue
{
    int m_ReadPointer;
    int m_WritePointer;
	unsigned short	m_nElementSize;		//Element dataLength
	
	int	m_nBufferSize;
    uint8* m_pBuffer;
}Queue;

void* QUEUE_getNew(Queue* queue);
Bool QUEUE_add(Queue* queue, const void* element, int len);

//Get the head element of queue
void* QUEUE_getHead(Queue* queue);

//Remove the head element;
void QUEUE_removeHead(Queue* queue);

//Get the head element and remove it
void* QUEUE_popGetHead(Queue* queue);

//Is queue empty 
//return: 1=Empty 0=Not Empty
Bool QUEUE_isEmpty(Queue* queue);
Bool QUEUE_isFull(Queue* queue);
void QUEUE_removeAll(Queue* queue);

Bool QUEUE_init(Queue* queue, void* pBuffer, unsigned short itemSize, unsigned int itemCount);
void QUEUE_reset(Queue* queue);
//void QUEUE_RemoveElements(Queue* queue, int nElements);
void QUEUE_Dump(Queue* queue);

#ifdef __cplusplus
}
#endif

#endif 

