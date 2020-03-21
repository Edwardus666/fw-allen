#include "Common.h"
#include "Obj.h"


Obj* g_ObjList[20] = { 0 };
static void ObjList_Add(Obj* pObj)
{
	for (int i = 0; i < GET_ELEMENT_COUNT(g_ObjList); i++)
	{
		if (g_ObjList[i] == Null)
		{
			g_ObjList[i] = pObj;
			return;
		}
	}

	Printf("Obj array is full.\n");
	Assert(False);
}

void ObjList_Start()
{
	Obj* pObj = g_ObjList[0];
	for (int i = 0; i < GET_ELEMENT_COUNT(g_ObjList) && pObj; i++, pObj = g_ObjList[i])
	{
		if (pObj->Start) pObj->Start();
	}
}

void ObjList_Stop()
{
	Obj* pObj = g_ObjList[0];
	for (int i = 0; i < GET_ELEMENT_COUNT(g_ObjList) && pObj; i++, pObj = g_ObjList[i])
	{
		if (pObj->Stop) pObj->Stop();
	}
}

void ObjList_Run()
{
	Obj* pObj = g_ObjList[0];
	for (int i = 0; i < GET_ELEMENT_COUNT(g_ObjList) && pObj; i++, pObj = g_ObjList[i])
	{
		if (pObj->Run) pObj->Run();
	}
}

void Obj_Register(Obj* pObj, const char* name, ObjFn Start, ObjFn Stop, ObjFn Run)
{
	pObj->name = name;

	pObj->Start = Start;
	pObj->Stop = Stop;
	pObj->Run = Run;

	Printf("Init %s\n", pObj->name);

	if (pObj->Start || pObj->Stop || pObj->Run)
	{
		ObjList_Add(pObj);
	}
}
