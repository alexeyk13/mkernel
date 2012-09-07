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

#include "mem_private.h"
#include "sys_calls.h"
#include "mem_pool.h"
#include "memmap.h"
#include "irq.h"
#include "dbg.h"
#include "mutex.h"
#include "kernel_config.h"
#include "thread_private.h"
#include "error.h"
#if (KERNEL_PROFILING)
#include "string.h"
#endif //KERNEL_PROFILING

//dynamic memory pool in system RAM for fast memory objects
MEM_POOL _sys_pool __attribute__ ((section (".sys_bss")));
//dynamic memory pool for thread stacks
MEM_POOL _stack_pool __attribute__ ((section (".sys_bss")));
//generic dynamic memory pool
MEM_POOL _data_pool __attribute__ ((section (".sys_bss")));

void mem_init()
{
	_sys_pool.base = SYSTEM_POOL_BASE;
	_sys_pool.size = SYSTEM_POOL_SIZE;
	_sys_pool.name = "sys mem pool";
	mem_pool_init(&_sys_pool);

	_stack_pool.base = THREAD_STACK_BASE;
	_stack_pool.size = THREAD_STACK_SIZE;
	_stack_pool.name = "thread stack pool";
	mem_pool_init(&_stack_pool);

	_data_pool.base = DATA_POOL_BASE;
	_data_pool.size = DATA_POOL_SIZE;
	_data_pool.name = "data pool";
	mem_pool_init(&_data_pool);
}

void* sys_alloc_aligned(int size, int align)
{
	CHECK_CONTEXT(IRQ_CONTEXT | SUPERVISOR_CONTEXT | SYSTEM_CONTEXT);
	void* ptr = NULL;
	CRITICAL_ENTER;
	ptr = mem_pool_alloc(&_sys_pool, size, align);
	CRITICAL_LEAVE;
	return ptr;
}

void* sys_alloc(int size)
{
	CHECK_CONTEXT(IRQ_CONTEXT | SUPERVISOR_CONTEXT | SYSTEM_CONTEXT);
	void* ptr = NULL;
	CRITICAL_ENTER;
	ptr = mem_pool_alloc(&_sys_pool, size, WORD_SIZE);
	CRITICAL_LEAVE;
	return ptr;
}

void sys_free(void *ptr)
{
	CHECK_CONTEXT(IRQ_CONTEXT | SUPERVISOR_CONTEXT | SYSTEM_CONTEXT);
	CRITICAL_ENTER;
	mem_pool_free(&_sys_pool, ptr);
	CRITICAL_LEAVE;
}

void* stack_alloc(int size)
{
	CHECK_CONTEXT(IRQ_CONTEXT | SUPERVISOR_CONTEXT);
	return mem_pool_alloc(&_stack_pool, size, THREAD_STACK_ALIGN);
}

void stack_free(void* ptr)
{
	CHECK_CONTEXT(IRQ_CONTEXT | SUPERVISOR_CONTEXT);
	mem_pool_free(&_stack_pool, ptr);
}

static inline void* svc_malloc(int size, int align)
{
	void* ptr;
	CRITICAL_ENTER;
	if (svc_thread_get_current()->pool)
		ptr = mem_pool_alloc(svc_thread_get_current()->pool, size, align);
	else
		ptr = mem_pool_alloc(&_data_pool, size, align);
	CRITICAL_LEAVE;
	return ptr;
}

static inline void svc_free(void* ptr)
{
	CRITICAL_ENTER;
	if (svc_thread_get_current()->pool)
		mem_pool_free(svc_thread_get_current()->pool, ptr);
	else
		mem_pool_free(&_data_pool, ptr);
	CRITICAL_LEAVE;
}

static inline MEM_POOL* svc_allocate_pool(char* name, int size)
{
	MEM_POOL* pool = svc_malloc(sizeof(MEM_POOL) + size, WORD_SIZE);
	pool->name = name;
	pool->base = (unsigned int)pool + sizeof(MEM_POOL);
	pool->size = size;
	mem_pool_init(pool);
	return pool;
}

static inline void svc_free_pool(MEM_POOL* pool)
{
	svc_free(pool);
}

static inline void svc_select_pool(MEM_POOL* pool)
{
	svc_thread_get_current()->pool = pool;
}

static inline void svc_select_global_pool()
{
	svc_thread_get_current()->pool = NULL;
}

#if (KERNEL_PROFILING)
void print_value(unsigned int value)
{
	if (value < 1024)
		printf(" %3d", value);
	else if (value / 1024 < 1024)
		printf("%3dK", value / 1024);
	else
		printf("%3dM", value / 1024 / 1024);
}

void print_pool_stat(MEM_POOL* pool)
{
	int i;
	MEM_POOL_STAT stat;
	mem_pool_stat(pool, &stat);
	printf("%s ", pool->name);
	for (i = strlen(pool->name); i <= 16; ++i)
		printf(" ");
	print_value(pool->size);
	printf("   ");
	print_value(stat.total_used);
	printf("(%d)   ", stat.used_blocks_count);
	print_value(stat.total_free);
	printf("(%d)\n\r", stat.free_blocks_count);
}

static inline void svc_mem_stat()
{
	//just to see all text right
	CRITICAL_ENTER;
	printf("name              size     used     free  \n\r");
	printf("---------------------------------------------\n\r");
	print_pool_stat(&_sys_pool);
	print_pool_stat(&_stack_pool);
	print_pool_stat(&_data_pool);
	CRITICAL_LEAVE;
}

#endif //KERNEL_PROFILING

unsigned int svc_mem_handler(unsigned int num, unsigned int param1, unsigned int param2)
{
	unsigned int res = 0;
	switch (num)
	{
	case MEM_ALLOCATE:
		res = (unsigned int)svc_malloc((int)param1, (int)param2);
		break;
	case MEM_FREE:
		svc_free((void*)param1);
		break;
	case POOL_ALLOCATE:
		res = (unsigned int)svc_allocate_pool((char*)param1, (int)param2);
		break;
	case POOL_FREE:
		svc_free_pool((MEM_POOL*)param1);
		break;
	case POOL_SELECT:
		svc_select_pool((MEM_POOL*)param1);
		break;
	case POOL_SELECT_GLOBAL:
		svc_select_global_pool((MEM_POOL*)param1);
		break;
#if (KERNEL_PROFILING)
	case MEM_STAT:
		svc_mem_stat();
		break;
#endif //KERNEL_PROFILING
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	return res;
}
