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

#include "thread.h"
#include "sys_call.h"
#include "sys_calls.h"

HANDLE thread_create(const char* name, int stack_size, unsigned int priority, THREAD_FUNCTION fn, void* param)
{
	THREAD_CALL tc;
	tc.name = name;
	tc.priority = priority;
	tc.stack_size = stack_size;
	tc.param = param;
	tc.fn = fn;
	return (HANDLE)sys_call(THREAD_CREATE, (unsigned int)&tc, 0, 0);
}

void thread_unfreeze(HANDLE thread)
{
	sys_call(THREAD_UNFREEZE, (unsigned int)thread, 0, 0);
}

void thread_freeze(HANDLE thread)
{
	sys_call(THREAD_FREEZE, (unsigned int)thread, 0, 0);
}

HANDLE thread_create_and_run(const char* name, int stack_size, unsigned int priority, THREAD_FUNCTION fn, void* param)
{
	HANDLE thread = thread_create(name, stack_size, priority, fn, param);
	thread_unfreeze(thread);
	return thread;
}

HANDLE thread_get_current()
{
	return (HANDLE)sys_call(THREAD_GET_CURRENT, 0, 0, 0);
}

void thread_set_priority(HANDLE thread, unsigned int priority)
{
	sys_call(THREAD_SET_PRIORITY, (unsigned int)thread, priority, 0);
}

void thread_set_current_priority(unsigned int priority)
{
	thread_set_priority(thread_get_current(), priority);
}

void thread_destroy(HANDLE thread)
{
	sys_call(THREAD_DESTROY, (unsigned int)thread, 0, 0);
}

void thread_exit()
{
	thread_destroy(thread_get_current());
}

void sleep(TIME* time)
{
	sys_call(THREAD_SLEEP, (unsigned int)time, 0, 0);
}

void sleep_ms(unsigned int ms)
{
	TIME time;
	ms_to_time(ms, &time);
	sys_call(THREAD_SLEEP, (unsigned int)&time, 0, 0);
}

void sleep_us(unsigned int us)
{
	TIME time;
	us_to_time(us, &time);
	sys_call(THREAD_SLEEP, (unsigned int)&time, 0, 0);
}

#if (KERNEL_PROFILING)
void thread_switch_test()
{
	sys_call(THREAD_SWITCH_TEST, 0, 0, 0);
}

void thread_stat()
{
	sys_call(THREAD_STAT, 0, 0, 0);
}

void stack_stat()
{
	sys_call(STACK_STAT, 0, 0, 0);
}
#endif
