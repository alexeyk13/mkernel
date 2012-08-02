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

#ifndef MEM_POOL_H
#define MEM_POOL_H

/*
		memory pool. All data is aligned on sizeof(int).
  */

#include "dlist.h"
#include "slist.h"
#include "kernel_config.h"

typedef struct {
	DLIST dlist;
#if (KERNEL_MARKS)
	uint32_t magic;
#endif
}MEM_POOL_ENTRY;

typedef struct {
	unsigned int base;
	unsigned int size;
	char const* name;

	MEM_POOL_ENTRY* blocks;
	SLIST* free_blocks;
}MEM_POOL;

#if (KERNEL_PROFILING)
typedef struct {
	unsigned int free_blocks_count;
	unsigned int used_blocks_count;
	unsigned int total_free;
	unsigned int largest_free;
	unsigned int total_used;
}MEM_POOL_STAT;

#endif //KERNEL_PROFILING

//creates mem pool. For perfomance reasons, base and size must be aligned to sizeof(int)
void mem_pool_init(MEM_POOL* pool);

void* mem_pool_alloc(MEM_POOL* pool, unsigned int size, unsigned int align);
void mem_pool_free(MEM_POOL* pool, void* ptr);

#if (KERNEL_PROFILING)
void mem_pool_stat(MEM_POOL* pool, MEM_POOL_STAT* stat);
#endif //KERNEL_PROFILING

#endif // MEM_POOL_H
