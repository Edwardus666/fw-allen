/*
 * (C) Copyright 2000
 * Paolo Scaffardi, AIRVENT SAM s.p.a - RIMINI(ITALY), arsenio@tin.it
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <stdarg.h>
#include "console.h"
#include "DriverUart.h"

/** U-Boot INITIAL CONSOLE-COMPATIBLE FUNCTION *****************************/

/*
void __attribute__((weak)) _puts (const char *s)
{
	while(*s)
	{
		usart_data_transmit(CCU_DEBUG_COM, (uint8_t)*s++);
		while(RESET == usart_flag_get(CCU_DEBUG_COM, USART_FLAG_TBE));
	}
     // wait for completion of USART transmission
    while(RESET == usart_flag_get(CCU_DEBUG_COM, USART_FLAG_TC));
}
*/

/* retarget the C library printf function to the USART */
#if 0
int vsnprintfEx(char* buffer, int bufLen, const char* lpszFormat, va_list ptr)
{
	//if(DL_SHOW_TICKS & g_dwDebugLevel)
	if (0)
	{
		int i = 0;
		int nLen = 0;
		static Bool isNewLine = True;

		if (isNewLine)
		{
			if ('\n' == lpszFormat[0])
			{
				buffer[0] = '\n';
				i = 1;
			}
			nLen = sprintf(&buffer[i], "[%08x]:", GET_TICKS()) + i;
		}

		if (strlen(lpszFormat) >= 1)
		{
			isNewLine = (lpszFormat[strlen(lpszFormat) - 1] == '\n');
		}

		nLen = vsnprintf(&buffer[nLen], bufLen - nLen, &lpszFormat[i], ptr) + nLen;

		return nLen;
	}
	else
	{
		return vsnprintf(buffer, bufLen, lpszFormat, ptr);
	}
}

int _Printf(const char* lpszFormat, ...)
{
	int nLen = 0;
	va_list ptr;
	char g_Pfbuffer[128];

	//LOCK();

	memset(g_Pfbuffer, 0, sizeof(g_Pfbuffer));
	va_start(ptr, lpszFormat);
	nLen = vsnprintfEx(g_Pfbuffer, sizeof(g_Pfbuffer), lpszFormat, ptr);
	va_end(ptr);

	static Uart* pUart = Null;
	if (pUart == Null) pUart = Uart_Get(CCU_DEBUG_COM);
	if(pUart->txBuf.m_MaxSize == 0) return 0;
	Uart_Tx(pUart, (uint8*)& g_Pfbuffer, nLen);

	//static Uart* pUart1 = Null;
	//if (pUart1 == Null) pUart1 = Uart_Get(CCU_MISC_COM);
	//Uart_Tx(pUart1, (uint8*)& g_Pfbuffer, nLen);

	//UNLOCK();

	return nLen;
}

#else
int fputc(int ch, FILE *f)
{
#if 1
	static Uart* pUart = Null;
	if (pUart == Null) pUart = Uart_Get(CCU_DEBUG_COM);

	Uart_Tx(pUart, (uint8*)& ch, 1);
#else
    usart_data_transmit(CCU_DEBUG_COM, (uint8_t)ch);
    while(RESET == usart_flag_get(CCU_DEBUG_COM, USART_FLAG_TBE));
#endif
	return ch;
}
#endif

