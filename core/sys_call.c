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

#include "sys_call.h"
#include "dbg.h"
#include "irq.h"
#include "sys_call.h"
#include "sys_calls.h"
#include "error.h"

#include "thread_private.h"
#include "mutex_private.h"
#include "event_private.h"
#include "sem_private.h"
#include "queue_private.h"
#include "sys_time_private.h"
#include "mem_private.h"
#include "dbg_console_private.h"

const unsigned short MIN_CONTEXT[] =			{SUPERVISOR_CONTEXT, SYSTEM_CONTEXT, USER_CONTEXT};

unsigned int sys_call(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3)
{
	if (get_context() < MIN_CONTEXT[num > 2 * CALL_CONTEXT ? 2 : num / CALL_CONTEXT])
		//raise context
		return do_sys_call(num, param1, param2, param3);
	else
		//enough context to call directly
		return sys_handler(num, param1, param2, param3);
}

unsigned int sys_handler(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3)
{
	unsigned int res = 0;
	switch (num & CALL_GROUP_MASK)
	{
	case SYS_CALL_THREAD:
		res = (unsigned int)svc_thread_handler(num, param1, param2);
		break;
	case SYS_CALL_MUTEX:
		res = (unsigned int)svc_mutex_handler(num, param1, param2);
		break;
	case SYS_CALL_EVENT:
		res = (unsigned int)svc_event_handler(num, param1, param2);
		break;
	case SYS_CALL_SEMAPHORE:
		res = (unsigned int)svc_semaphore_handler(num, param1, param2);
		break;
	case SYS_CALL_QUEUE:
		res = (unsigned int)svc_queue_handler(num, param1, param2, param3);
		break;
	case SYS_CALL_TIME:
		res = (unsigned int)svc_sys_time_handler(num, param1);
		break;
	case SYS_CALL_MEM:
		res = (unsigned int)svc_mem_handler(num, param1, param2);
		break;
	case SYS_CALL_DBG:
		res = (unsigned int)svc_dbg_handler(num, param1, param2);
		break;
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	return res;
}

