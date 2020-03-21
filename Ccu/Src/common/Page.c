
#include "Page.h"
#include "string.h"

//��ҳ����
// ........*.....|................|.....*..........|
//         |<-           len          ->|
//       offset
void Page_Alligned(unsigned int offset
	, unsigned int len
	, unsigned short pageSize
	, Page* pageHead
	, Page* pages
	, Page* pageTail)
{
	pageHead->offset = offset;
	pageHead->len	 = ((offset + pageSize - 1) / pageSize) * pageSize - offset;
	if(len < pageHead->len)
	{
		pageHead->len = len;
		memset(pageTail	, 0, sizeof(Page));
		memset(pages	, 0, sizeof(Page));
		return;
	}

	pageTail->len	 = (offset + len) % pageSize;	
	pageTail->offset = offset + len - pageTail->len;

	pages->offset = pageHead->offset + pageHead->len;
	//pages->len�������pageHead->len���岻ͬ�������ʾ�ж���ҳ��
	pages->len	 = (len - pageHead->len - pageTail->len) / pageSize;	
}

