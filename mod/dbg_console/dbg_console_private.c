/*
	M-Kernel - embedded RTOS
	Copyright (c) 2011-2012, Alexey Kramarenko
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
		list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice,
		this list of conditions and the following disclaimer in the documentation
		and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
	ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "dbg_console_private.h"
#include "dbg_console.h"
#include "types.h"
#include "dbg.h"
#include "console.h"
#include "kernel_config.h"
#include "thread.h"
#include "mem.h"
#include "irq.h"
#include "mutex.h"
#include "sys_call.h"
#include "sys_calls.h"
#include "error.h"

CONSOLE* _dbg_console										= NULL;

void dbg_console_thread(void* param)
{
	for (;;)
	{
		switch(console_getc(_dbg_console))
		{
		case 'h':
			printf("dbg console help\n\r");
			printf("h - this text\n\r");
#if (KERNEL_PROFILING)
			printf("m - memory usage statistics\n\r");
			printf("p - process list\n\r");
			printf("s - system stack usage\n\r");
#endif //KERNEL_PROFILING
			break;
#if (KERNEL_PROFILING)
		case 'm':
			mem_stat();
			break;
		case 'p':
			thread_stat();
			break;
		case 's':
			stack_stat();
			break;
#endif //KERNEL_PROFILING
		}
	}
}

void dbg_console_create()
{
	_dbg_console = console_create(DBG_CONSOLE_UART, DBG_CONSOLE_TX_FIFO, DBG_CONSOLE_IRQ_PRIORITY);
	UART_BAUD baud;
	baud.data_bits = 8;
	baud.parity = 'N';
	baud.stop_bits = 1;
	baud.baud = DBG_CONSOLE_BAUD;
	uart_set_baudrate(DBG_CONSOLE_UART, &baud);

	thread_create_and_run("DBG console", DBG_CONSOLE_THREAD_STACK_SIZE, DBG_CONSOLE_THREAD_PRIORITY, dbg_console_thread, NULL);
}

unsigned int svc_dbg_handler(unsigned int num, unsigned int param1, unsigned int param2)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT | SYSTEM_CONTEXT);
	unsigned int res = 0;
	switch (num)
	{
	case DBG_WRITE:
		if (_dbg_console)
			console_write(_dbg_console, (char*)param1, (int)param2);
		break;
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	return res;
}

