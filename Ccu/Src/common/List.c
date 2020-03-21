
#include "Common.h"
#include "List.h"

void List_Add(List* pListHead, List* newNode)
{
	List* node = pListHead->next;
	for (; node->next != Null; node = node->next);
	node->next = newNode;
}

List* List_Remove(List* pListHead, const List* removeNode)
{
	List* node = pListHead;
	if (node == removeNode)
	{
		return node->next;
	}

	for (; node->next != Null; node = node->next)
	{
		if (node->next == removeNode)
		{
			node->next = removeNode->next;
		}
	}

	return pListHead;
}

