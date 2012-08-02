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

#ifndef TASK_PRIVATE_H
#define TASK_PRIVATE_H

#include "sys_call.h"
#include "thread.h"
#include "sys_timer.h"
#include "kernel_config.h"
#include "dbg.h"
#include "arch.h"
#include "mem_pool.h"

typedef enum {
	THREAD_SYNC_TIMER_ONLY =(0x0 << 4),
	THREAD_SYNC_MUTEX =		(0x1 << 4),
	THREAD_SYNC_EVENT =		(0x2 << 4),
	THREAD_SYNC_SEMAPHORE =	(0x3 << 4),
	THREAD_SYNC_QUEUE =		(0x4 << 4)
}THREAD_SYNC_TYPE;

typedef struct {
	DLIST list;															//list of threads - active, frozen, or owned by sync object
	const char* name;
	unsigned int* sp_cur;											//current sp(if saved) !!TODO: move down
	MAGIC;
	unsigned long flags;
	unsigned int* sp_top;											//top of stack
	unsigned base_priority;											//base priority
	unsigned current_priority;										//priority, adjusted by mutex
	TIMER timer;														//timer for thread sleep and sync objects timeouts
	void* sync_object;												//sync object we are waiting for
	DLIST* owned_mutexes;											//owned mutexes list for nested mutex priority inheritance
	MEM_POOL* pool;													//allocate/free data in selected pool, if NULL - in global
#if (KERNEL_PROFILING)
	TIME uptime;
	TIME uptime_start;
	int stack_size;
#endif //KERNEL_PROFILING
}THREAD;

unsigned int svc_thread_handler(unsigned int num, unsigned int param1, unsigned int param2);
void thread_init();

const char* svc_thread_name(THREAD* thread);
//for sync-object internal calls (usually mutexes)
void svc_thread_set_current_priority(THREAD* thread, unsigned int priority);
void svc_thread_restore_current_priority(THREAD* thread);
//cannot be call from IRQ context. because this will freeze current system thread, not IRQ context
//also cannot be called from idle task, because idle task is only task running, while other tasks are waiting or frozen
//this function can be call indirectly from any sync object.
void svc_thread_sleep(TIME* time, THREAD_SYNC_TYPE sync_type, void* sync_object);
void svc_thread_wakeup(THREAD* thread);
THREAD* svc_thread_get_current();


//user idle_task
extern void idle_task(void);

//hw-related
extern void pend_switch_context(void);
extern void thread_setup_context(THREAD* thread, THREAD_FUNCTION fn, void* param);
extern void thread_patch_context(THREAD* thread, unsigned int res);


#endif // TASK_PRIVATE_H
