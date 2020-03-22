#ifndef __PAGE_H_
#define __PAGE_H_

#ifdef __cplusplus
extern "C"{
#endif

typedef struct _Page
{
	unsigned int offset;
	unsigned int len;
}Page;

#define PAGE_ALLIGNED(len, pageSize) (((len + pageSize - 1) / pageSize) * pageSize)
void Page_Alligned(unsigned int offset
	, unsigned int len
	, unsigned short pageSize
	, Page* pageHead
	, Page* pages
	, Page* pageTail);
	
#ifdef __cplusplus
}
#endif

#endif


