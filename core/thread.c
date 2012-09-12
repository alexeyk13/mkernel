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

/** \addtogroup thread thread
	thread is the main object in kernel.

	thread can be in one of the following states:
	- active
	- waiting (for sync object)
	- frozen
	- waiting frozen

	After creation thread is in the frozen state

	Thread is also specified by:
	- it's name
	- priority
	- stack size
	\{
 */

#include "thread.h"
#include "sys_call.h"
#include "sys_calls.h"

/**
	\brief creates thread object. By default, thread is frozen after creation
	\param name: thread name
	\param stack_size: thread stack size in words. Must have enough space for context saving.
	Please, be carefull with this parameter: providing small stack may leave to overflows, providing
	large - to memory waste. Exact stack usage can be determined by turning on KERNEL_PROFILING.
	\param priority: thread priority. 0 - is highest priority, -idle- lowest. Only IDLE task can run on
	-idle- priority. Can be adjusted dynamically at any time.
	\param fn: thread function. Thread can't leave this function. In case of need of thread termination
	inside fn, thread_exit should be called.
	\param param: parameter for thread. Can be NULL

	\retval thread HANDLE on success, or INVALID_HANDLE on failure
*/
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

/**
	\brief unfreeze thread
	\param thread: handle of created thread
	\retval none
*/
void thread_unfreeze(HANDLE thread)
{
	sys_call(THREAD_UNFREEZE, (unsigned int)thread, 0, 0);
}

/**
	\brief freeze thread
	\param thread: handle of created thread
	\retval none
*/
void thread_freeze(HANDLE thread)
{
	sys_call(THREAD_FREEZE, (unsigned int)thread, 0, 0);
}

/**
	\brief creates thread object and run it. See \ref thread_create for details
	\retval thread HANDLE on success, or INVALID_HANDLE on failure
*/
HANDLE thread_create_and_run(const char* name, int stack_size, unsigned int priority, THREAD_FUNCTION fn, void* param)
{
	HANDLE thread = thread_create(name, stack_size, priority, fn, param);
	thread_unfreeze(thread);
	return thread;
}

/**
	\brief return handle to currently running thread
	\retval handle to current thread
*/
HANDLE thread_get_current()
{
	return (HANDLE)sys_call(THREAD_GET_CURRENT, 0, 0, 0);
}

/**
	\brief set priority
	\param thread: handle of created thread
	\param priority: new priority. 0 - highest, -idle- -1 - lowest. Cannot be call for IDLE thread.
	\retval none
*/
void thread_set_priority(HANDLE thread, unsigned int priority)
{
	sys_call(THREAD_SET_PRIORITY, (unsigned int)thread, priority, 0);
}

/**
	\brief set currently running thread priority
	\param priority: new priority. 0 - highest, -idle- -1 - lowest. Cannot be call for IDLE thread.
	\retval none
*/
void thread_set_current_priority(unsigned int priority)
{
	thread_set_priority(thread_get_current(), priority);
}

/**
	\brief destroys thread
	\param thread: previously created thread
	\retval none
*/
void thread_destroy(HANDLE thread)
{
	sys_call(THREAD_DESTROY, (unsigned int)thread, 0, 0);
}

/**
	\brief destroys current thread
	\details this function should be called instead of leaving thread function
	\retval none
*/
void thread_exit()
{
	thread_destroy(thread_get_current());
}

/**
	\brief put current thread in waiting state
	\param time: pointer to TIME structure
	\retval none
*/
void sleep(TIME* time)
{
	sys_call(THREAD_SLEEP, (unsigned int)time, 0, 0);
}

/**
	\brief put current thread in waiting state
	\param ms: time to sleep in milliseconds
	\retval none
*/
void sleep_ms(unsigned int ms)
{
	TIME time;
	ms_to_time(ms, &time);
	sys_call(THREAD_SLEEP, (unsigned int)&time, 0, 0);
}

/**
	\brief put current thread in waiting state
	\param us: time to sleep in microseconds
	\retval none
*/
void sleep_us(unsigned int us)
{
	TIME time;
	us_to_time(us, &time);
	sys_call(THREAD_SLEEP, (unsigned int)&time, 0, 0);
}

/** \} */ // end of thread group

#if (KERNEL_PROFILING)
/** \addtogroup profiling profiling
  \ref KERNEL_PROFILING option should be set to 1
	\{
 */

/**
	\brief thread switch test
	\details simulate thread switching. This function can be used for
	perfomance measurement
	\retval none
*/
void thread_switch_test()
{
	sys_call(THREAD_SWITCH_TEST, 0, 0, 0);
}

/**
	\brief thread statistics
	\details print statistics over debug console for all active threads:
	- names
	- priority, active priority (can be temporally raised for sync objects)
	- stack usage: current/max/defined
	\retval none
*/
void thread_stat()
{
	sys_call(THREAD_STAT, 0, 0, 0);
}

/**
	\brief system stacks statistics
	\details list of stacks depends on architecture.
	print statistics over debug console for all active threads:
	- names
	- stack usage: current/max/defined
	\retval none
*/
void stack_stat()
{
	sys_call(STACK_STAT, 0, 0, 0);
}

/** \} */ // end of profiling group
#endif
