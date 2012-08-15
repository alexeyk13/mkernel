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

#include "event_private.h"
#include "sys_calls.h"
#include "time.h"
#include "mem.h"
#include "mem_private.h"
#include "irq.h"
#include "error.h"

const char *const EVENT_NAME =							"EVENT";

static inline EVENT* svc_event_create()
{
	EVENT* event = sys_alloc(sizeof(EVENT));
	if (event != NULL)
	{
		event->set = false;
		event->waiters = NULL;
		DO_MAGIC(event, MAGIC_EVENT);
	}
	else
		fatal_error(ERROR_MEM_OUT_OF_SYSTEM_MEMORY, EVENT_NAME);
	return event;
}

void svc_event_pulse(EVENT* event)
{
	CHECK_MAGIC(event, MAGIC_EVENT, EVENT_NAME);

	//release all waiters
	THREAD* thread;
	while (event->waiters)
	{
		thread = event->waiters;
		dlist_remove_head((DLIST**)&event->waiters);
		svc_thread_wakeup(thread);
	}
}

static inline void svc_event_set(EVENT* event)
{
	svc_event_pulse(event);
	event->set = true;
}

static inline bool svc_event_is_set(EVENT* event)
{
	return event->set;
}

static inline void svc_event_clear(EVENT* event)
{
	CHECK_MAGIC(event, MAGIC_EVENT, EVENT_NAME);
	event->set = false;
}

static inline bool svc_event_wait(EVENT* event, TIME* time)
{
	CHECK_MAGIC(event, MAGIC_EVENT, EVENT_NAME);

	THREAD* thread = svc_thread_get_current();
	if (!event->set)
	{
		//first - remove from active list
		//if called from IRQ context, thread_private.c will raise error
		svc_thread_sleep(time, THREAD_SYNC_EVENT, event);
		dlist_add_tail((DLIST**)&event->waiters, (DLIST*)thread);
	}
	return true;
}

void svc_event_lock_release(EVENT* event, THREAD* thread)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CHECK_MAGIC(event, MAGIC_EVENT, EVENT_NAME);
	dlist_remove((DLIST**)&event->waiters, (DLIST*)thread);
}

static inline void svc_event_destroy(EVENT* event)
{
	THREAD* thread;
	while (event->waiters)
	{
		thread = event->waiters;
		dlist_remove_head((DLIST**)&event->waiters);
		//patch return value
		thread_patch_context(thread, false);
		svc_thread_wakeup(thread);
	}
	sys_free(event);
}

unsigned int svc_event_handler(unsigned int num, unsigned int param1, unsigned int param2)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CRITICAL_ENTER;
	unsigned int res = 0;
	switch (num)
	{
	case EVENT_CREATE:
		res = (unsigned int)svc_event_create();
		break;
	case EVENT_PULSE:
		svc_event_pulse((EVENT*)param1);
		break;
	case EVENT_SET:
		svc_event_set((EVENT*)param1);
		break;
	case EVENT_IS_SET:
		res = svc_event_is_set((EVENT*)param1);
		break;
	case EVENT_CLEAR:
		svc_event_clear((EVENT*)param1);
		break;
	case EVENT_WAIT:
		res = (unsigned int)svc_event_wait((EVENT*)param1, (TIME*)param2);
		break;
	case EVENT_DESTROY:
		svc_event_destroy((EVENT*)param1);
		break;
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	CRITICAL_LEAVE;
	return res;
}
