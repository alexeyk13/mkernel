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

#include "thread_private.h"
#include "thread.h"
#include "kernel_config.h"
#include "irq.h"
#include "mem.h"
#include "mem_private.h"
#include "error.h"
#include <stddef.h>
#include "string.h"
#include "mutex_private.h"
#include "event_private.h"
#include "sem_private.h"
#include "queue_private.h"
#include "sys_calls.h"
#include "magic.h"
#if (KERNEL_PROFILING)
#include "arch.h"
#include "memmap.h"
#endif //KERNEL_PROFILING

#define THREAD_MODE_MASK																								0x3

#define THREAD_MODE_FROZEN																								(0 << 0)
#define THREAD_MODE_RUNNING																							(1 << 0)
#define THREAD_MODE_WAITING																							(2 << 0)
#define THREAD_MODE_WAITING_FROZEN																					(3 << 0)
#define THREAD_MODE_WAITING_SYNC_OBJECT																			(1 << 1)

#define THREAD_TIMER_ACTIVE																							(1 << 2)

#define THREAD_SYNC_MASK																								(0xf << 4)

#define THREAD_NAME_PRINT_SIZE																						16

#define UNNAMED_THREAD_DEF																								"UNNAMED_THREAD"
const char *const UNNAMED_THREAD =																					UNNAMED_THREAD_DEF;
const char *const IDLE_THREAD =																						"IDLE THREAD";

#if (KERNEL_PROFILING)
extern void svc_stack_stat();
#endif //KERNEL_PROFILING

#define IDLE_PRIORITY																									((unsigned int)-1)

#define THREAD_NAME(thread)																							thread->name ? thread->name : UNNAMED_THREAD

static THREAD* _active_threads[THREAD_CACHE_SIZE] __attribute__ ((section (".sys_bss"))) =		{NULL};
static THREAD* _threads_uncached __attribute__ ((section (".sys_bss"))) =								NULL;
static int _thread_list_size __attribute__ ((section (".sys_bss"))) =									0;

//Current thread. If there is no active tasks, idle_task will be run
static THREAD* _current_thread __attribute__ ((section (".sys_bss"))) =									NULL;
static THREAD* _idle_thread __attribute__ ((section (".sys_bss"))) =										NULL;

//for context-switching
//now running thread. (Active context)
volatile THREAD* _active_thread __attribute__ ((section (".sys_bss"))) =								NULL;
//next thread to run, after leave. For context switch. If NULL - no context switch is required
volatile THREAD* _next_thread __attribute__ ((section (".sys_bss"))) =									NULL;

//on thread abnormal exit
void abnormal_exit()
{
	register THREAD* thread;
	thread = (THREAD*)thread_get_current();
	error(ERROR_THREAD_OUT_OF_CONTEXT, thread->name ? thread->name : UNNAMED_THREAD_DEF);
}

void svc_thread_timeout(void* param)
{
	THREAD* thread = param;
	thread->flags &= ~THREAD_TIMER_ACTIVE;
	//say sync object to release us
	switch (thread->flags & THREAD_SYNC_MASK)
	{
	case THREAD_SYNC_TIMER_ONLY:
		break;
	case THREAD_SYNC_MUTEX:
		svc_mutex_lock_release((MUTEX*)thread->sync_object, thread);
		break;
	case THREAD_SYNC_EVENT:
		svc_event_lock_release((EVENT*)thread->sync_object, thread);
		break;
	case THREAD_SYNC_SEMAPHORE:
		svc_semaphore_lock_release((SEMAPHORE*)thread->sync_object, thread);
		break;
	case THREAD_SYNC_QUEUE:
		svc_queue_lock_release((QUEUE*)thread->sync_object, thread);
		break;
	default:
		ASSERT(false);
	}
	//patch return value
	thread_patch_context(thread, false);
	svc_thread_wakeup(thread);
}

const char* svc_thread_name(THREAD* thread)
{
	return THREAD_NAME(thread);
}

THREAD* svc_thread_create(THREAD_CALL* tc)
{
	THREAD* thread = sys_alloc(sizeof(THREAD));
	//allocate thread object
	if (thread != NULL)
	{
		thread->name = tc->name;
		thread->flags = 0;
		thread->base_priority = tc->priority;
		thread->current_priority = thread->base_priority;
		//allocate thread stack
		thread->sp_top = stack_alloc(tc->stack_size * sizeof(int));
		thread->sp_cur = thread->sp_top + tc->stack_size;
		if (thread->sp_top)
		{
#if (KERNEL_PROFILING)
			thread->uptime.sec = 0;
			thread->uptime.usec = 0;
			thread->uptime_start.sec = 0;
			thread->uptime_start.usec = 0;
			thread->stack_size = tc->stack_size;
			memset(thread->sp_top, MAGIC_UNINITIALIZED_BYTE, thread->stack_size * sizeof(unsigned int));
#endif //KERNEL_PROFILING

			thread_setup_context(thread, tc->fn, tc->param);
			thread->timer.callback = svc_thread_timeout;
			thread->timer.param = thread;
			thread->owned_mutexes = NULL;
			thread->sync_object = NULL;
			thread->pool = NULL;

			DO_MAGIC(thread, MAGIC_THREAD);
		}
		else
		{
			sys_free(thread);
			thread = NULL;
			fatal_error(ERROR_MEM_OUT_OF_STACK_MEMORY, THREAD_NAME(tc));
		}
	}
	else
		fatal_error(ERROR_MEM_OUT_OF_SYSTEM_MEMORY, THREAD_NAME(tc));

	return thread;
}

static inline void push_last_in_list()
{
	if (_thread_list_size == THREAD_CACHE_SIZE)
	{
		DLIST* cur;
		while (_active_threads[THREAD_CACHE_SIZE - 1] != NULL)
		{
			cur = _active_threads[THREAD_CACHE_SIZE - 1]->list.prev;
			dlist_remove_tail((DLIST**)&(_active_threads[THREAD_CACHE_SIZE - 1]));
			dlist_add_head((DLIST**)&_threads_uncached, cur);
		}
	}
	else
		++_thread_list_size;
}

static inline void pop_last_from_list()
{
	if (_threads_uncached != NULL)
	{
		_active_threads[THREAD_CACHE_SIZE - 1] = NULL;
		int priority = _threads_uncached->current_priority;
		THREAD* cur;
		while (_threads_uncached != NULL && _threads_uncached->current_priority == priority)
		{
			cur = _threads_uncached;
			dlist_remove_head((DLIST**)&_threads_uncached);
			dlist_add_tail((DLIST**)&(_active_threads[_thread_list_size]), (DLIST*)cur);
		}
	}
	else
	{
		--_thread_list_size;
	}
}

void thread_add_to_active_list(THREAD* thread)
{
	THREAD* thread_to_save = thread;
	//thread priority is less, than active, activate him
	if (thread->current_priority < _current_thread->current_priority)
	{
#if (KERNEL_PROFILING)
		svc_get_uptime(&thread->uptime_start);
		time_sub(&_current_thread->uptime_start, &thread->uptime_start, &_current_thread->uptime_start);
		time_add(&_current_thread->uptime_start, &_current_thread->uptime, &_current_thread->uptime);
#endif //KERNEL_PROFILING

		thread_to_save = _current_thread;
		_current_thread = thread;
		_next_thread = thread;
	}
	//first - look at cache
	int pos = 0;
	if (_thread_list_size)
	{
		int first = 0;
		int last = _thread_list_size - 1;
		int mid;
		while (first < last)
		{
			mid = (first + last) >> 1;
			if (_active_threads[mid]->current_priority < thread_to_save->current_priority)
				first = mid + 1;
			else
				last = mid;
		}
		pos = first;
		if (_active_threads[pos]->current_priority < thread_to_save->current_priority)
			++pos;
	}

	//we have space in cache?
	if (pos < THREAD_CACHE_SIZE)
	{
		//does we have active thread with same priority?
		if (!(_active_threads[pos] != NULL && _active_threads[pos]->current_priority == thread_to_save->current_priority))
		{
			//last list is going out ouf cache
			push_last_in_list();
			memmove(_active_threads + pos + 1, _active_threads + pos, (_thread_list_size - pos - 1) * sizeof(void*));
			_active_threads[pos] = NULL;
		}
		dlist_add_tail((DLIST**)&_active_threads[pos], (DLIST*)thread_to_save);
	}
	//find and allocate timer on uncached list
	else
	{
		//top
		if (_threads_uncached == NULL || thread_to_save->current_priority < _threads_uncached->current_priority)
			dlist_add_head((DLIST**)&_threads_uncached, (DLIST*)thread_to_save);
		//bottom
		else if (thread_to_save->current_priority >= ((THREAD*)_threads_uncached->list.prev)->current_priority)
			dlist_add_tail((DLIST**)&_threads_uncached, (DLIST*)thread_to_save);
		//in the middle
		else
		{
			DLIST_ENUM de;
			THREAD* cur;
			dlist_enum_start((DLIST**)&_threads_uncached, &de);
			while (dlist_enum(&de, (DLIST**)&cur))
				if (cur->current_priority < thread_to_save->current_priority)
				{
					dlist_add_before((DLIST**)&_threads_uncached, (DLIST*)cur, (DLIST*)thread_to_save);
					break;
				}
		}
	}
}

void thread_remove_from_active_list(THREAD* thread)
{
	int pos = 0;
	//freeze active task
	if (thread == _current_thread)
	{
#if (KERNEL_PROFILING)
		svc_get_uptime(&_active_threads[0]->uptime_start);
		time_sub(&_current_thread->uptime_start, &_active_threads[0]->uptime_start, &_current_thread->uptime_start);
		time_add(&_current_thread->uptime_start, &_current_thread->uptime, &_current_thread->uptime);
#endif //KERNEL_PROFILING
		_current_thread = _active_threads[0];
		_next_thread = _active_threads[0];
	}
	//try to search in cache
	else
	{
		int first = 0;
		int last = _thread_list_size - 1;
		int mid;
		while (first < last)
		{
			mid = (first + last) >> 1;
			if (_active_threads[mid]->current_priority < thread->current_priority)
				first = mid + 1;
			else
				last = mid;
		}
		pos = first;
		if (_active_threads[pos]->current_priority < thread->current_priority)
			++pos;
	}

	if (pos < THREAD_CACHE_SIZE)
	{
		dlist_remove_head((DLIST**)&(_active_threads[pos]));

		//removed all at current priority level
		if (_active_threads[pos] == NULL)
		{
			memmove(_active_threads + pos, _active_threads + pos + 1, (_thread_list_size - pos - 1) * sizeof(void*));
			//restore to cache from list
			pop_last_from_list();
		}
	}
	//remove from uncached
	else
		dlist_remove((DLIST**)&_threads_uncached, (DLIST*)thread);
}

static inline void svc_thread_unfreeze(THREAD* thread)
{
	CHECK_MAGIC(thread, MAGIC_THREAD, THREAD_NAME(thread));
	switch (thread->flags & THREAD_MODE_MASK)
	{
	case THREAD_MODE_FROZEN:
		thread_add_to_active_list(thread);
		if (_next_thread)
			pend_switch_context();
		thread->flags &= ~THREAD_MODE_MASK;
		thread->flags |= THREAD_MODE_RUNNING;
		break;
	case THREAD_MODE_WAITING_FROZEN:
		thread->flags &= ~THREAD_MODE_MASK;
		thread->flags |= THREAD_MODE_WAITING;
		break;
	}
}

static inline void svc_thread_freeze(THREAD* thread)
{
	CHECK_MAGIC(thread, MAGIC_THREAD, THREAD_NAME(thread));
	switch (thread->flags & THREAD_MODE_MASK)
	{
	case THREAD_MODE_RUNNING:
		thread_remove_from_active_list(thread);
		if (_next_thread)
			pend_switch_context();
		thread->flags &= ~THREAD_MODE_MASK;
		thread->flags |= THREAD_MODE_FROZEN;
		break;
	case THREAD_MODE_WAITING:
		thread->flags &= ~THREAD_MODE_MASK;
		thread->flags |= THREAD_MODE_WAITING_FROZEN;
		break;
	}
}

THREAD* svc_thread_get_current()
{
	return _current_thread;
}

void svc_thread_set_current_priority(THREAD* thread, unsigned int priority)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CHECK_MAGIC(thread, MAGIC_THREAD, THREAD_NAME(thread));
	if (thread->current_priority != priority)
	{
		switch (thread->flags & THREAD_MODE_MASK)
		{
		case THREAD_MODE_RUNNING:
			thread_remove_from_active_list(thread);
			thread->current_priority = priority;
			thread_add_to_active_list(thread);
			if (_next_thread == _active_thread)
				_next_thread = NULL;
			if (_next_thread)
				pend_switch_context();
			break;
		//if we are waiting for mutex, adjusting priority can affect on mutex owner
		case THREAD_MODE_WAITING:
		case THREAD_MODE_WAITING_FROZEN:
			thread->current_priority = priority;
			if ((thread->flags & THREAD_SYNC_MASK) == THREAD_SYNC_MUTEX)
				svc_thread_set_current_priority(((MUTEX*)thread->sync_object)->owner, svc_mutex_calculate_owner_priority(((MUTEX*)thread->sync_object)->owner));
			break;
		default:
			thread->current_priority = priority;
		}
	}
}

void svc_thread_restore_current_priority(THREAD* thread)
{
	svc_thread_set_current_priority(thread, thread->base_priority);
}

static inline void svc_thread_set_priority(THREAD* thread, unsigned int priority)
{
	thread->base_priority = priority;
	svc_thread_set_current_priority(thread, svc_mutex_calculate_owner_priority(thread));
}

static inline void svc_thread_destroy(THREAD* thread)
{
	CHECK_MAGIC(thread, MAGIC_THREAD, THREAD_NAME(thread));
	//we cannot destroy IDLE thread
	if (thread == _idle_thread)
		fatal_error(ERROR_THREAD_INVALID_CALL_IN_IDLE_THREAD, IDLE_THREAD);
	//if thread is running, freeze it first
	if ((thread->flags & THREAD_MODE_MASK)	== THREAD_MODE_RUNNING)
	{
		thread_remove_from_active_list(thread);
		//we don't need to save context on exit
		if (_active_thread == thread)
			_active_thread = NULL;
		if (_next_thread)
			pend_switch_context();
	}
	//if thread is owned by any sync object, release them
	if  (thread->flags & THREAD_MODE_WAITING_SYNC_OBJECT)
	{
		//if timer is still active, kill him
		if (thread->flags & THREAD_TIMER_ACTIVE)
			svc_sys_timer_destroy(&thread->timer);
		//say sync object to release us
		switch (thread->flags & THREAD_SYNC_MASK)
		{
		case THREAD_SYNC_TIMER_ONLY:
			break;
		case THREAD_SYNC_MUTEX:
			svc_mutex_lock_release((MUTEX*)thread->sync_object, thread);
			break;
		case THREAD_SYNC_EVENT:
			svc_event_lock_release((EVENT*)thread->sync_object, thread);
			break;
		case THREAD_SYNC_SEMAPHORE:
			svc_semaphore_lock_release((SEMAPHORE*)thread->sync_object, thread);
			break;
		case THREAD_SYNC_QUEUE:
			svc_queue_lock_release((QUEUE*)thread->sync_object, thread);
			break;
		default:
			ASSERT(false);
		}
	}
	//release memory, occupied by thread
	stack_free(thread->sp_top);
	sys_free(thread);
}

void svc_thread_sleep(TIME* time, THREAD_SYNC_TYPE sync_type, void *sync_object)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT);
	THREAD* thread = _current_thread;
	CHECK_MAGIC(thread, MAGIC_THREAD, THREAD_NAME(thread));
	//idle thread cannot sleep or be locked by mutex
	if (thread == _idle_thread)
		fatal_error(ERROR_THREAD_INVALID_CALL_IN_IDLE_THREAD, IDLE_THREAD);
	thread_remove_from_active_list(thread);
	pend_switch_context();
	thread->flags &= ~(THREAD_MODE_MASK | THREAD_SYNC_MASK);
	thread->flags |= THREAD_MODE_WAITING | sync_type;
	thread->sync_object = sync_object;

	//adjust owner priority
	if (sync_type == THREAD_SYNC_MUTEX && ((MUTEX*)sync_object)->owner->current_priority > thread->current_priority)
		svc_thread_set_current_priority(((MUTEX*)sync_object)->owner, thread->current_priority);

	//create timer if not infinite
	if (time->sec || time->usec)
	{
		thread->flags |= THREAD_TIMER_ACTIVE;
		thread->timer.time.sec = time->sec;
		thread->timer.time.usec = time->usec;
		svc_sys_timer_create(&thread->timer);
	}
}

void svc_thread_wakeup(THREAD* thread)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CHECK_MAGIC(thread, MAGIC_THREAD, THREAD_NAME(thread));
	if  (thread->flags & THREAD_MODE_WAITING_SYNC_OBJECT)
	{
		//if timer is still active, kill him
		if (thread->flags & THREAD_TIMER_ACTIVE)
			svc_sys_timer_destroy(&thread->timer);
		thread->flags &= ~(THREAD_TIMER_ACTIVE | THREAD_SYNC_MASK);
		thread->sync_object = NULL;

		switch (thread->flags & THREAD_MODE_MASK)
		{
		case THREAD_MODE_WAITING_FROZEN:
			thread->flags &= ~THREAD_MODE_MASK;
			thread->flags |= THREAD_MODE_FROZEN;
			break;
		case THREAD_MODE_WAITING:
			thread_add_to_active_list(thread);
			if (_next_thread)
				pend_switch_context();
			thread->flags &= ~THREAD_MODE_MASK;
			thread->flags |= THREAD_MODE_RUNNING;
			break;
		}
	}
}

#if (KERNEL_PROFILING)
static inline void svc_thread_switch_test()
{
	THREAD* thread = _current_thread;
	thread_remove_from_active_list(thread);
	thread_add_to_active_list(thread);
	pend_switch_context();
	//next thread is now same as active thread, it will simulate context switching
}

static inline unsigned int stack_used_max(unsigned int top, unsigned int cur)
{
	unsigned int i;
	unsigned int last = cur;
	for (i = cur - sizeof(unsigned int); i >= top; i -= 4)
		if (*(unsigned int*)i != MAGIC_UNINITIALIZED)
			last = i;
	return last;
}

void thread_print_stat(THREAD* thread)
{
	char thread_name[THREAD_NAME_PRINT_SIZE + 1];
	TIME thread_uptime;
	//format name
	int i;
	thread_name[THREAD_NAME_PRINT_SIZE] = 0;
	memset(thread_name, ' ', THREAD_NAME_PRINT_SIZE);
	strncpy(thread_name, THREAD_NAME(thread), THREAD_NAME_PRINT_SIZE);
	for (i = 0; i < THREAD_NAME_PRINT_SIZE; ++i)
		if (thread_name[i] == 0)
			thread_name[i] = ' ';
	printf("%s ", thread_name);

	//format priority
	if (thread->current_priority == IDLE_PRIORITY)
		printf("-idle-   ");
	else
		printf("%2d(%2d)   ", thread->current_priority, thread->base_priority);

	//stack size
	unsigned int current_stack, max_stack;
	current_stack = thread->stack_size - ((unsigned int)thread->sp_cur - (unsigned int)thread->sp_top) / sizeof(unsigned int);
	max_stack = thread->stack_size - (stack_used_max((unsigned int)thread->sp_top, (unsigned int)thread->sp_cur) - (unsigned int)thread->sp_top) / sizeof(unsigned int);
	printf("%3d/%3d/%3d   ", current_stack, max_stack, thread->stack_size);

	//uptime, including time for current thread
	if (thread == _current_thread)
	{
		get_uptime(&thread_uptime);
		time_sub(&thread->uptime_start, &thread_uptime, &thread_uptime);
		time_add(&thread_uptime, &thread->uptime, &thread_uptime);
	}
	else
	{
		thread_uptime.sec = thread->uptime.sec;
		thread_uptime.usec = thread->uptime.usec;
	}
	printf("%3d:%02d.%03d\n\r", thread_uptime.sec / 60, thread_uptime.sec % 60, thread->uptime.usec / 1000);
}

static inline void svc_thread_stat()
{
	int active_threads_count = 0;
	int i;
	DLIST_ENUM de;
	THREAD* cur;
	printf("    name        priority      stack        uptime\n\r");
	printf("----------------------------------------------------\n\r");
	//current
	thread_print_stat(_current_thread);
	++active_threads_count;
	//in cache
	for (i = 0; i < _thread_list_size; ++i)
	{
		dlist_enum_start((DLIST**)&_active_threads[i], &de);
		while (dlist_enum(&de, (DLIST**)&cur))
		{
			thread_print_stat(cur);
			++active_threads_count;
		}
	}
	//out of cache
	dlist_enum_start((DLIST**)&_threads_uncached, &de);
	while (dlist_enum(&de, (DLIST**)&cur))
	{
		thread_print_stat(cur);
		++active_threads_count;
	}
	printf("total %d threads active\n\r", active_threads_count);
}
#endif //KERNEL_PROFILING

unsigned int svc_thread_handler(unsigned int num, unsigned int param1, unsigned int param2)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CRITICAL_ENTER;
	unsigned int res = 0;
	switch (num)
	{
	case THREAD_CREATE:
		res = (unsigned int)svc_thread_create((THREAD_CALL*)param1);
		break;
	case THREAD_UNFREEZE:
		svc_thread_unfreeze((THREAD*)param1);
		break;
	case THREAD_FREEZE:
		svc_thread_unfreeze((THREAD*)param1);
		break;
	case THREAD_GET_CURRENT:
		res = (unsigned int)svc_thread_get_current();
		break;
	case THREAD_SET_PRIORITY:
		svc_thread_set_priority((THREAD*)param1, (unsigned int)param2);
		break;
	case THREAD_DESTROY:
		svc_thread_destroy((THREAD*)param1);
		break;
	case THREAD_SLEEP:
		svc_thread_sleep((TIME*)param1, THREAD_SYNC_TIMER_ONLY, NULL);
		break;
#if (KERNEL_PROFILING)
	case THREAD_SWITCH_TEST:
		svc_thread_switch_test();
		break;
	case THREAD_STAT:
		svc_thread_stat();
		break;
	case STACK_STAT:
		svc_stack_stat();
		break;
#endif //KERNEL_PROFILING
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	CRITICAL_LEAVE;
	return res;
}

void thread_init()
{
	THREAD_CALL tc;
	tc.name = IDLE_THREAD;
	tc.priority = IDLE_PRIORITY;
	tc.stack_size = THREAD_IDLE_STACK_SIZE;
	tc.fn = (THREAD_FUNCTION)idle_task;
	tc.param = NULL;
	_idle_thread = svc_thread_create(&tc);

	//activate idle_thread
	_idle_thread->flags = THREAD_MODE_RUNNING;
	_current_thread = _idle_thread;
	_next_thread = _idle_thread;
	pend_switch_context();
}
