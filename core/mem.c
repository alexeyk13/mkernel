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

/** \addtogroup memory dynamic memory management
	M-Kernel has embedded dynamic memory manager

	All data is allocated internally inside memory pools.
	Physical pools layout described in porting documentation	and freely configurable.

	If configured, pools are automatically checked for:
		- range checking - both up and bottom
		- consistency: every entry on debug supplied with "magic"

	Usage profiling for memory pools are also available (if enabled).

	Generally, there are 3 memory pools:
		- system pool: for system objects, like mutexes
		- data pool: for general use
		- stack pool: used internally by system for thread stack allocation.

	Inside data pool is also possible to create local memory pool for high-fragmented
	threads.

	Data pool is only pool, accessible from USER context. All memory calls are thread-safe.
	Any data is aligned by WORD_SIZE() by default. However, with *_aligned version of calls
	this can be changed: sometimes it's required by hardware. For example, some DMA calls must
	be aligned to 8 bytes on STM32.
	\{
 */

#include "mem.h"
#include "sys_call.h"
#include "sys_calls.h"
#include "types.h"

/**
	\brief allocate memory in current thread's pool with specific align
	\param size: data size in bytes
	\param align: align in bytes
	\retval pointer on success, NULL on out of memory conditiion
*/
void* malloc_aligned(int size, int align)
{
	return (void*)sys_call(MEM_ALLOCATE, (unsigned int)size, (unsigned int)align, 0);
}

/**
	\brief allocate memory in current thread's pool
	\param size: data size in bytes
	\retval pointer on success, NULL on out of memory conditiion
*/
void* malloc(int size)
{
	return (void*)sys_call(MEM_ALLOCATE, (unsigned int)size, (unsigned int)WORD_SIZE, 0);
}

/**
	\brief free memory in current thread's pool
	\details same memory pool as for malloc must be selected, or exception will be raised
	\param ptr: pointer to allocated data
	\retval none
*/
void free(void* ptr)
{
	sys_call(MEM_FREE, (unsigned int)ptr, 0, 0);
}

/**
	\brief allocate custom memory pool for current thread.
	\details Pool allocated insid current thread's pool -
	global data pool or local. After allocation, pool handle can be used
	by other thread. see \ref pool_select
	\param name: name of pool
	\param size: size of pool in bytes
	\retval pool handle
*/
HANDLE pool_allocate(char* name, int size)
{
	return (HANDLE)sys_call(POOL_ALLOCATE, (unsigned int)name, (unsigned int)size, 0);
}

/**
	\brief free allocated custom pool
	\details make sure, all threads, used this pool are selected global pool and all data
	in custom pool is freed
	\param pool: allocated pool handle
	\retval none
*/
void pool_free(HANDLE pool)
{
	sys_call(POOL_FREE, (unsigned int)pool, 0, 0);
}

/**
	\brief select custom pool
	\param pool: allocated pool handle
	\retval none
*/
void pool_select(HANDLE pool)
{
	sys_call(POOL_SELECT, (unsigned int)pool, 0, 0);
}

/**
	\brief back to global data pool
	\retval none
*/
void pool_select_global()
{
	sys_call(POOL_SELECT_GLOBAL, 0, 0, 0);
}

/** \} */ // end of memory group

#if (KERNEL_PROFILING)
/** \addtogroup profiling profiling
	\{
 */

/**
	\brief display memory statistics
	\details display info about all global memory pools
	and thread local memory pool (if selected): total size,
	used size, free size in bytes and objects count. Also
	displays free blocks fragmentation.
	\retval none
*/
void mem_stat()
{
	sys_call(MEM_STAT, 0, 0, 0);
}

/** \} */ // end of profiling group
#endif //KERNEL_PROFILING

