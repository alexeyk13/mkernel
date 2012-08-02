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

#include "mutex_private.h"
#include "sys_calls.h"
#include "mem.h"
#include "mem_private.h"
#include "error.h"
#include "thread_private.h"
#include "irq.h"

const char *const MUTEX_NAME =							"MUTEX";

static inline MUTEX* svc_mutex_create()
{
	MUTEX* mutex = sys_alloc(sizeof(MUTEX));
	if (mutex != NULL)
	{
		mutex->owner = NULL;
		mutex->waiters = NULL;
		DO_MAGIC(mutex, MAGIC_MUTEX);
	}
	else
		fatal_error(ERROR_MEM_OUT_OF_SYSTEM_MEMORY, MUTEX_NAME);

	return mutex;
}

unsigned int svc_mutex_calculate_owner_priority(THREAD* thread)
{
	unsigned int priority = thread->base_priority;
	DLIST_ENUM owned_mutexes, thread_waiters;
	MUTEX* current_mutex;
	THREAD* current_thread;
	dlist_enum_start(&thread->owned_mutexes, &owned_mutexes);
	while (dlist_enum(&owned_mutexes, (DLIST**)&current_mutex))
	{
		dlist_enum_start((DLIST**)&current_mutex->waiters, &thread_waiters);
		while (dlist_enum(&thread_waiters, (DLIST**)&current_thread))
			if (current_thread->current_priority < priority)
				priority = current_thread->current_priority;
	}
	return priority;
}

static inline bool svc_mutex_lock(MUTEX* mutex, TIME* time)
{
	CHECK_MAGIC(mutex, MAGIC_MUTEX, MUTEX_NAME);
	THREAD* thread = svc_thread_get_current();
	if (mutex->owner != NULL)
	{
		if (mutex->owner != thread)
		{
			//first - remove from active list
			svc_thread_sleep(time, THREAD_SYNC_MUTEX, mutex);
			//add to mutex watiers list
			dlist_add_tail((DLIST**)&mutex->waiters, (DLIST*)thread);
		}
		else
			error(ERROR_SYNC_ALREADY_OWNED, svc_thread_name(thread));
	}
	//we are first. just lock and add to owned
	else
	{
		mutex->owner = thread;
		dlist_add_tail((DLIST**)&mutex->owner->owned_mutexes, (DLIST*)mutex);
	}
	//in case of timeout, we will patch result in context by thread_private.c
	return true;
}

void svc_mutex_lock_release(MUTEX* mutex, THREAD* thread)
{
	//it's the only mutex call, that can be made in irq context - on sys_timer timeout call from thread_private.c
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CHECK_MAGIC(mutex, MAGIC_MUTEX, MUTEX_NAME);
	//release mutex owner
	if (thread == mutex->owner)
	{
		//thread now is not owning mutex, remove it from owned list and calculate new priority (he is still can own nested mutexes)
		dlist_remove((DLIST**)&thread->owned_mutexes, (DLIST*)mutex);
		svc_thread_set_current_priority(thread, svc_mutex_calculate_owner_priority(thread));

		mutex->owner = mutex->waiters;
		if (mutex->owner)
		{
			dlist_remove_head((DLIST**)&mutex->waiters);
			dlist_add_tail((DLIST**)&mutex->owner->owned_mutexes, (DLIST*)mutex);
			//owner can still depends on some waiters
			svc_thread_set_current_priority(mutex->owner, svc_mutex_calculate_owner_priority(mutex->owner));
			svc_thread_wakeup(mutex->owner);
		}
	}
	//remove item from waiters list
	else
	{
		dlist_remove((DLIST**)&mutex->waiters, (DLIST*)thread);
		//this can affect on owner priority
		svc_thread_set_current_priority(mutex->owner, svc_mutex_calculate_owner_priority(mutex->owner));
		//it's up to caller to decide, wake up thread (timeout, mutex destroy) or not (thread terminate) owned process
	}
}

static inline void svc_mutex_unlock(MUTEX* mutex)
{
	THREAD* thread = svc_thread_get_current();
	if (mutex->owner)
	{
		if (mutex->owner == thread)
			svc_mutex_lock_release(mutex, thread);
		else
			error(ERROR_SYNC_WRONG_UNLOCKER, svc_thread_name(thread));
	}
	else
		error(ERROR_SYNC_ALREADY_UNLOCKED, svc_thread_name(thread));
}

static inline void svc_mutex_destroy(MUTEX* mutex)
{
	THREAD* thread;
	while (mutex->waiters)
	{
		thread = mutex->waiters;
		svc_mutex_lock_release(mutex, mutex->waiters);
		//patch return value
		thread_patch_context(thread, false);
		svc_thread_wakeup(thread);
	}
	if (mutex->owner)
		svc_mutex_lock_release(mutex, mutex->owner);
	sys_free(mutex);
}

unsigned int svc_mutex_handler(unsigned int num, unsigned int param1, unsigned int param2)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT);
	CRITICAL_ENTER;
	unsigned int res = 0;
	switch (num)
	{
	case MUTEX_CREATE:
		res = (unsigned int)svc_mutex_create();
		break;
	case MUTEX_LOCK:
		res = (unsigned int)svc_mutex_lock((MUTEX*)param1, (TIME*)param2);
		break;
	case MUTEX_UNLOCK:
		svc_mutex_unlock((MUTEX*)param1);
		break;
	case MUTEX_DESTROY:
		svc_mutex_destroy((MUTEX*)param1);
		break;
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	CRITICAL_LEAVE;
	return res;
}
