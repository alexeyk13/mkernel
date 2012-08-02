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

#include "sem_private.h"
#include "sys_calls.h"
#include "time.h"
#include "mem.h"
#include "mem_private.h"
#include "irq.h"
#include "error.h"

const char *const SEMAPHORE_NAME =							"SEMAPHORE";

static inline SEMAPHORE* svc_semaphore_create()
{
	SEMAPHORE* sem = sys_alloc(sizeof(SEMAPHORE));
	if (sem != NULL)
	{
		sem->value = 0;
		sem->waiters = NULL;
		DO_MAGIC(sem, MAGIC_SEMAPHORE);
	}
	else
		fatal_error(ERROR_MEM_OUT_OF_SYSTEM_MEMORY, SEMAPHORE_NAME);

	return sem;
}

void svc_semaphore_signal(SEMAPHORE* sem)
{
	CHECK_MAGIC(sem, MAGIC_SEMAPHORE, SEMAPHORE_NAME);

	sem->value++;
	//release all waiters
	THREAD* thread;
	while (sem->value && sem->waiters)
	{
		thread = sem->waiters;
		dlist_remove_head((DLIST**)&sem->waiters);
		svc_thread_wakeup(thread);
		sem->value--;
	}
}

static inline bool svc_semaphore_wait(SEMAPHORE* sem, TIME* time)
{
	CHECK_MAGIC(sem, MAGIC_SEMAPHORE, SEMAPHORE_NAME);

	THREAD* thread = svc_thread_get_current();
	if (sem->value == 0)
	{
		//first - remove from active list
		//if called from IRQ context, thread_private.c will raise error
		svc_thread_sleep(time, THREAD_SYNC_SEMAPHORE, sem);
		dlist_add_tail((DLIST**)&sem->waiters, (DLIST*)thread);
	}
	return true;
}

void svc_semaphore_lock_release(SEMAPHORE* sem, THREAD* thread)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CHECK_MAGIC(sem, MAGIC_SEMAPHORE, SEMAPHORE_NAME);
	dlist_remove((DLIST**)&sem->waiters, (DLIST*)thread);
}

static inline void svc_semaphore_destroy(SEMAPHORE* sem)
{
	THREAD* thread;
	while (sem->waiters)
	{
		thread = sem->waiters;
		dlist_remove_head((DLIST**)&sem->waiters);
		//patch return value
		thread_patch_context(thread, false);
		svc_thread_wakeup(thread);
	}
	sys_free(sem);
}

unsigned int svc_semaphore_handler(unsigned int num, unsigned int param1, unsigned int param2)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CRITICAL_ENTER;
	unsigned int res = 0;
	switch (num)
	{
	case SEMAPHORE_CREATE:
		res = (unsigned int)svc_semaphore_create();
		break;
	case SEMAPHORE_SIGNAL:
		svc_semaphore_signal((SEMAPHORE*)param1);
		break;
	case SEMAPHORE_WAIT:
		res = (unsigned int)svc_semaphore_wait((SEMAPHORE*)param1, (TIME*)param2);
		break;
	case SEMAPHORE_DESTROY:
		svc_semaphore_destroy((SEMAPHORE*)param1);
		break;
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	CRITICAL_LEAVE;
	return res;
}
