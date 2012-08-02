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

#include "mem.h"
#include "sys_call.h"
#include "sys_calls.h"
#include "types.h"

void* malloc_aligned(int size, int align)
{
	return (void*)sys_call(MEM_ALLOCATE, (unsigned int)size, (unsigned int)align, 0);
}

void* malloc(int size)
{
	return (void*)sys_call(MEM_ALLOCATE, (unsigned int)size, (unsigned int)WORD_SIZE, 0);
}

void free(void* ptr)
{
	sys_call(MEM_FREE, (unsigned int)ptr, 0, 0);
}

HANDLE pool_allocate(char* name, int size)
{
	return (HANDLE)sys_call(POOL_ALLOCATE, (unsigned int)name, (unsigned int)size, 0);
}

void pool_free(HANDLE pool)
{
	sys_call(POOL_FREE, (unsigned int)pool, 0, 0);
}

void pool_select(HANDLE pool)
{
	sys_call(POOL_SELECT, (unsigned int)pool, 0, 0);
}

void pool_select_global()
{
	sys_call(POOL_SELECT_GLOBAL, 0, 0, 0);
}

#if (KERNEL_PROFILING)
void mem_stat()
{
	sys_call(MEM_STAT, 0, 0, 0);
}
#endif //KERNEL_PROFILING
