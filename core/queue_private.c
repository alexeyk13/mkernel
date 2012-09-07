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

#include "queue_private.h"
#include "sys_calls.h"
#include "time.h"
#include "mem.h"
#include "mem_private.h"
#include "irq.h"
#include "error.h"

const char *const QUEUE_NAME =							"QUEUE";

static inline QUEUE* svc_queue_create(unsigned int block_size, unsigned int blocks_count, unsigned int align)
{
	QUEUE* queue = NULL;
	int i;
	//first, try to allocate space for queue data. In thread's current mempool
	unsigned int align_offset = sizeof(DLIST);
	if (align > align_offset)
		align_offset = align;
	void* mem_block = malloc_aligned(blocks_count * (block_size + align_offset), align_offset);
	if (mem_block)
	{
		queue = sys_alloc(sizeof(QUEUE));
		if (queue != NULL)
		{
			queue->align_offset = align_offset;
			queue->mem_block = mem_block;
			queue->pull_waiters = NULL;
			queue->push_waiters = NULL;
			DO_MAGIC(queue, MAGIC_QUEUE);
			//set all as free
			queue->free_blocks = NULL;
			queue->filled_blocks = NULL;
			for (i = 0; i < blocks_count; ++i)
				dlist_add_tail(&queue->free_blocks, (DLIST*)((unsigned int)mem_block + i * (block_size + align_offset)));
		}
		else
		{
			free(mem_block);
			fatal_error(ERROR_MEM_OUT_OF_SYSTEM_MEMORY, QUEUE_NAME);
		}
	}
	else
		error(ERROR_MEM_OUT_OF_HEAP, svc_thread_name(svc_thread_get_current()));

	return queue;
}

static inline void* svc_queue_allocate_buffer(QUEUE* queue, TIME* time)
{
	CHECK_MAGIC(queue, MAGIC_QUEUE, QUEUE_NAME);

	void* res = NULL;
	THREAD* thread = svc_thread_get_current();
	if (queue->free_blocks)
	{
		res = (void*)((unsigned int)(queue->free_blocks) + queue->align_offset);
		dlist_remove_head(&queue->free_blocks);
	}
	else
	{
		//first - remove from active list
		//if called from IRQ context, thread_private.c will raise error
		svc_thread_sleep(time, THREAD_SYNC_QUEUE, queue);
		dlist_add_tail((DLIST**)&queue->push_waiters, (DLIST*)thread);
	}
	return res;
}

static inline void svc_queue_push(QUEUE* queue, void* buf)
{
	CHECK_MAGIC(queue, MAGIC_QUEUE, QUEUE_NAME);

	if (queue->pull_waiters)
	{
		THREAD* thread = queue->pull_waiters;
		dlist_remove_head((DLIST**)&queue->pull_waiters);
		//patch return value
		thread_patch_context(thread, (unsigned int)buf);
		svc_thread_wakeup(thread);
	}
	else
		dlist_add_tail(&queue->filled_blocks, (DLIST*)((unsigned int)buf - queue->align_offset));
}


static inline void* svc_queue_pull(QUEUE* queue, TIME* time)
{
	CHECK_MAGIC(queue, MAGIC_QUEUE, QUEUE_NAME);

	void* res = NULL;
	THREAD* thread = svc_thread_get_current();
	if (queue->filled_blocks)
	{
		res = (void*)((unsigned int)(queue->filled_blocks) + queue->align_offset);
		dlist_remove_head(&queue->filled_blocks);
	}
	else
	{
		//first - remove from active list
		//if called from IRQ context, thread_private.c will raise error
		svc_thread_sleep(time, THREAD_SYNC_QUEUE, queue);
		dlist_add_tail((DLIST**)&queue->pull_waiters, (DLIST*)thread);
	}
	return res;
}

static inline void svc_queue_release_buffer(QUEUE* queue, void* buf)
{
	CHECK_MAGIC(queue, MAGIC_QUEUE, QUEUE_NAME);

	if (queue->push_waiters)
	{
		THREAD* thread = queue->push_waiters;
		dlist_remove_head((DLIST**)&queue->push_waiters);
		//patch return value
		thread_patch_context(thread, (unsigned int)buf);
		svc_thread_wakeup(thread);
	}
	else
		dlist_add_tail(&queue->free_blocks, (DLIST*)((unsigned int)buf - queue->align_offset));
}

static inline bool svc_queue_is_empty(QUEUE* queue)
{
	CHECK_MAGIC(queue, MAGIC_QUEUE, QUEUE_NAME);
	return queue->filled_blocks == NULL ? true : false;
}

static inline bool svc_queue_is_full(QUEUE* queue)
{
	CHECK_MAGIC(queue, MAGIC_QUEUE, QUEUE_NAME);
	return queue->free_blocks == NULL ? true : false;
}

void svc_queue_lock_release(QUEUE* queue, THREAD* thread)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CHECK_MAGIC(queue, MAGIC_QUEUE, QUEUE_NAME);

	if (is_dlist_contains((DLIST**)&queue->push_waiters, (DLIST*)thread))
		dlist_remove((DLIST**)&queue->push_waiters, (DLIST*)thread);
	else if (is_dlist_contains((DLIST**)&queue->pull_waiters, (DLIST*)thread))
		dlist_remove((DLIST**)&queue->pull_waiters, (DLIST*)thread);
	else
	{
		ASSERT(false);
	}
}

static inline void svc_queue_destroy(QUEUE* queue)
{
	THREAD* thread;
	while (queue->push_waiters)
	{
		thread = queue->push_waiters;
		dlist_remove_head((DLIST**)&queue->push_waiters);
		//patch return value
		thread_patch_context(thread, NULL);
		svc_thread_wakeup(thread);
	}
	while (queue->pull_waiters)
	{
		thread = queue->pull_waiters;
		dlist_remove_head((DLIST**)&queue->pull_waiters);
		//patch return value
		thread_patch_context(thread, NULL);
		svc_thread_wakeup(thread);
	}
	//MUST be called from same thread, same mem pool
	free(queue->mem_block);
	sys_free(queue);
}

unsigned int svc_queue_handler(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CRITICAL_ENTER;
	unsigned int res = 0;
	switch (num)
	{
	case QUEUE_CREATE:
		res = (unsigned int)svc_queue_create(param1, param2, param3);
		break;
	case QUEUE_ALLOCATE_BUFFER:
		res = (unsigned int)svc_queue_allocate_buffer((QUEUE*)param1, (TIME*)param2);
		break;
	case QUEUE_PUSH:
		svc_queue_push((QUEUE*)param1, (void*)param2);
		break;
	case QUEUE_PULL:
		res = (unsigned int)svc_queue_pull((QUEUE*)param1, (TIME*)param2);
		break;
	case QUEUE_RELEASE_BUFFER:
		svc_queue_release_buffer((QUEUE*)param1, (void*)param2);
		break;
	case QUEUE_IS_EMPTY:
		res = (unsigned int)svc_queue_is_empty((QUEUE*)param1);
		break;
	case QUEUE_IS_FULL:
		res = (unsigned int)svc_queue_is_full((QUEUE*)param1);
		break;
	case QUEUE_DESTROY:
		svc_queue_destroy((QUEUE*)param1);
		break;
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	CRITICAL_LEAVE;
	return res;
}

