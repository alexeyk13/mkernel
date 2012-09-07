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

#include "sw_timer.h"
#include "mem_private.h"
#include "event.h"
#include "thread.h"
#include "dlist.h"
#include "sys_timer.h"
#include "kernel_config.h"
#include "irq.h"
#include "dbg.h"
#include "error.h"

typedef struct {
	TIMER timer;
	SW_TIMER_HANDLER handler;
	void* param;
	bool active;
}SW_TIMER;

static HANDLE _event;
static SW_TIMER* _active_timers					= NULL;

void sw_timer_thread(void* param)
{
	for (;;)
	{
		event_wait_ms(_event, INFINITE);
		event_clear(_event);

		SW_TIMER_HANDLER handler;
		void* param = NULL;
		do {
			CRITICAL_ENTER;
			if (_active_timers)
			{
				handler = _active_timers->handler;
				param = _active_timers->param;
				_active_timers->active = false;
				dlist_remove_head((DLIST**)&_active_timers);
			}
			else
				handler = NULL;
			CRITICAL_LEAVE;
			if (handler)
				handler(param);
		} while (handler);
	}
}

void sw_timer_thread_wakeuper(void* param)
{
	SW_TIMER* sw_timer = (SW_TIMER*)param;
	CRITICAL_ENTER;
	dlist_add_tail((DLIST**)&_active_timers, (DLIST*)sw_timer);
	CRITICAL_LEAVE;
	event_set(_event);
}

HANDLE sw_timer_create(SW_TIMER_HANDLER handler, void *param)
{
	SW_TIMER* sw_timer = (SW_TIMER*)sys_alloc(sizeof(SW_TIMER));
	if (sw_timer)
	{
		sw_timer->active = false;
		sw_timer->handler = handler;
		sw_timer->param = param;
		sw_timer->timer.callback = sw_timer_thread_wakeuper;
		sw_timer->timer.param = sw_timer;
	}
	else
		error_thread(ERROR_MEM_OUT_OF_SYSTEM_MEMORY);
	return (HANDLE)sw_timer;
}

void sw_timer_destroy(HANDLE handle)
{
	sw_timer_stop(handle);
	SW_TIMER* sw_timer = (SW_TIMER*)handle;
	sys_free(sw_timer);
}

void sw_timer_start(HANDLE handle, TIME* timeout)
{
	SW_TIMER* sw_timer = (SW_TIMER*)handle;
	if (sw_timer->active)
		sw_timer_stop(handle);
	sw_timer->timer.time.sec = timeout->sec;
	sw_timer->timer.time.usec = timeout->usec;
	sw_timer->active = true;
	sys_timer_create(&sw_timer->timer);
}

void sw_timer_start_ms(HANDLE handle, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	sw_timer_start(handle, &timeout);
}

void sw_timer_start_us(HANDLE handle, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	sw_timer_start(handle, &timeout);
}

void sw_timer_stop(HANDLE handle)
{
	SW_TIMER* sw_timer = (SW_TIMER*)handle;
	bool need_destroy = false;
	if (sw_timer->active)
	{
		CRITICAL_ENTER;
		sw_timer->active = false;
		//worsest case, in active list
		if (is_dlist_contains((DLIST**)&_active_timers, (DLIST*)sw_timer))
			dlist_remove((DLIST**)&_active_timers, (DLIST*)sw_timer);
		else
			need_destroy = true;
		CRITICAL_LEAVE;
		if (need_destroy)
			sys_timer_destroy(&sw_timer->timer);
	}
}

void sw_timer_init()
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT | SYSTEM_CONTEXT);
	_event = event_create();
	thread_create_and_run("sw_timer", SW_TIMER_STACK_SIZE, SW_TIMER_PRIORITY, sw_timer_thread, NULL);
}
