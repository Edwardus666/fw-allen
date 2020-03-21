
#include "Common.h"
#include "BleLock.h"

static BleLock* g_pBleLock;

void BleLock_Reset()
{
}

void BleLock_Run()
{
}

void BleLock_Stop()
{
	g_pBleLock->state = BLE_LOCK_STOP;
}

//BleLock Æô¶¯
void BleLock_Start()
{
	if (g_pBleLock == Null || g_pBleLock->state == BLE_LOCK_START) return;

	g_pBleLock->state = BLE_LOCK_START;
}

void BleLock_Init()
{
	static BleLock g_BleLock;
	g_pBleLock = &g_BleLock;

	memset(g_pBleLock, 0, sizeof(BleLock));

}
