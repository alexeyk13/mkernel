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

#include "mem_pool.h"
#include "dbg.h"
#include "magic.h"

/*
	to understand, how it's working, below is small description:

	header for every entry (used and free):
	- ptr next entry
	- ptr prior entry
#if (KERNEL_MARKS)
	- magic number, for consistency check
#endif //KERNEL_MARKS

	used entry (after header):
	<alignment words> 0..(align - 1) / WORD_SIZE
#if (KERNEL_RANGE_CHECKING)
	<magic words of range checking top> align / WORD_SIZE
#endif //KERNEL_RANGE_CHECKING
	<data>
	<align bytes> uninitialized. 0..WORD_SIZE - 1
#if (KERNEL_RANGE_CHECKING)
	<magic words of range checking bottom> align / WORD_SIZE
	<magic words of unitialized data> 0..(sizeof(entry header) + sizeof(free entry) - 1) / WORD_SIZE
#endif //KERNEL_RANGE_CHECKING
	uninitialized data is used only in kernel range checking, to calculate size of entry data.

	free entry (after header):
	- ptr next free entry

  */

//entry is empty
#define IS_EMPTY_FLAG						(1 << 0)

//align address to next align
#define ALIGN(addr, align)					(((addr) + (align - 1)) & ~(align - 1))
//check, if address is minimal aligned
#define CHECK_ALIGN(addr)					ASSERT(((unsigned int)(addr) % WORD_SIZE) == 0)

//size of mem pool entry
#define HEADER_SIZE							(sizeof(MEM_POOL_ENTRY))
//pointer to data based on header ptr
#define DATA_PTR(entry_ptr)				((unsigned int)(entry_ptr) + HEADER_SIZE)
//pointer to header based on unaligned data ptr
#define ENTRY_PTR(data_ptr)				((unsigned int)(data_ptr) - HEADER_SIZE)
//free entry
#define FREE_PTR(entry_ptr)				((SLIST*)(DATA_PTR(entry_ptr)))
//get entry by offset of free ptr
#define ENTRY_BY_FREE(free_ptr)			((MEM_POOL_ENTRY*)((unsigned int)(free_ptr) - HEADER_SIZE))
//minimal size of free block, where marking as free block has sense
#define MIN_DATA_SIZE						(sizeof(SLIST))
#define MIN_SIZE								(HEADER_SIZE + MIN_DATA_SIZE)
//entry is last/first?
#define IS_LAST(entry)						((unsigned int)((entry)->dlist.next) <= (unsigned int)(entry))
#define IS_FIRST(entry)						((unsigned int)((entry)->dlist.prev) >= (unsigned int)(entry))
//full size of entry, including header
#define ENTRY_SIZE(entry, pool)			(IS_LAST(entry) ? ((pool)->base + (pool)->size - (unsigned int)(entry)) : ((unsigned int)((entry)->dlist.next)- (unsigned int)(entry)))
#define ENTRY_DATA_SIZE(entry, pool)	(ENTRY_SIZE((entry), (pool)) - HEADER_SIZE)
//just to mind
#define RANGE_CHECK_SIZE					align

void mem_pool_init(MEM_POOL* pool)
{
	CHECK_ALIGN(pool->base);
	CHECK_ALIGN(pool->size);

	//mark all as freespace
	MEM_POOL_ENTRY* ee;
	ee = (MEM_POOL_ENTRY*)(pool->base);
	DO_MAGIC(ee, MAGIC_MEM_POOL_ENTRY);

	dlist_clear((DLIST**)&pool->blocks);
	dlist_add_tail((DLIST**)&pool->blocks, (DLIST*)ee);

	//empty list cache. used for fast allocate/free block
	FREE_PTR(ee)->next = NULL;
	pool->free_blocks = FREE_PTR(ee);
}

void* mem_pool_alloc(MEM_POOL* pool, unsigned int size, unsigned int align)
{
	CHECK_ALIGN(align);
	ASSERT(pool->blocks);
	unsigned int cur_size, align_space, i, aligned_data_ptr;
	unsigned int size_data = ALIGN(size, WORD_SIZE);
	//in other case we don't have mem to create free space
	if (size_data < MIN_DATA_SIZE)
		size_data = MIN_DATA_SIZE;
#if (KERNEL_RANGE_CHECKING)
	unsigned int size_full = size_data + 2 * RANGE_CHECK_SIZE;
#else
	unsigned int size_full = size_data;
#endif //KERNEL_RANGE_CHECKING

	//we will use first-fit algorithm
	MEM_POOL_ENTRY* cur;
	SLIST* prev_free = NULL;
	SLIST* cur_free = pool->free_blocks;
	void* res = NULL;
	while (cur_free)
	{
		cur = ENTRY_BY_FREE(cur_free);
		CHECK_MAGIC(cur, MAGIC_MEM_POOL_ENTRY, pool->name);

		//full size of current entry - excluding header, ignoring align and range checking marks
		cur_size = ENTRY_DATA_SIZE(cur, pool);
		//address of aligned data, but, ignoring range checking marks
		aligned_data_ptr = ALIGN(DATA_PTR(cur), align);
		//bytes needed for alignment
		align_space = aligned_data_ptr - DATA_PTR(cur);
		//entry has enough space, occupy it
		if (cur_size - align_space >= size_full)
		{
			if (prev_free)
				slist_remove_after(prev_free);
			else
				slist_remove_head(&pool->free_blocks);
#if (KERNEL_RANGE_CHECKING)
			res = (void*)(aligned_data_ptr + RANGE_CHECK_SIZE);
			for (i = aligned_data_ptr; i < aligned_data_ptr + RANGE_CHECK_SIZE; i += WORD_SIZE)
				*((unsigned int*)i) = MAGIC_RANGE_TOP;
			unsigned int data_end = aligned_data_ptr + RANGE_CHECK_SIZE + size_data;
			for (i = data_end; i < data_end + RANGE_CHECK_SIZE; i += WORD_SIZE)
				*((unsigned int*)i) = MAGIC_RANGE_BOTTOM;
#else
			res = (void*)aligned_data_ptr;
#endif //KERNEL_RANGE_CHECKING
			//fill alignment
			for (i = DATA_PTR(cur); i < aligned_data_ptr; i += WORD_SIZE)
				*((unsigned int*)i) = MAGIC_MEM_POOL_ALIGN_SPACE;
			cur_size -= (align_space + size_full);

			//we need to create free space entry?
			if (cur_size >= MIN_SIZE)
			{
				MEM_POOL_ENTRY* ee;
				ee = (MEM_POOL_ENTRY*)(DATA_PTR(cur) + align_space + size_full);
				DO_MAGIC(ee, MAGIC_MEM_POOL_ENTRY);
				dlist_add_after((DLIST**)&pool->blocks, (DLIST*)cur, (DLIST*)ee);
				if (prev_free)
					slist_add_after(prev_free, FREE_PTR(ee));
				else
					slist_add_head(&pool->free_blocks, FREE_PTR(ee));
			}
#if (KERNEL_RANGE_CHECKING)
			//fill unused space for range checking
			else
			{
				unsigned int entry_end = (unsigned int)cur + ENTRY_SIZE(cur, pool);
				for (i = data_end + RANGE_CHECK_SIZE; i < entry_end; i += WORD_SIZE)
					*((unsigned int*)i) = MAGIC_MEM_POOL_UNUSED;
			}
#endif //KERNEL_RANGE_CHECKING
			break;
		}
		prev_free = cur_free;
		cur_free = cur_free->next;
	}

	return res;
}

void mem_pool_free(MEM_POOL* pool, void* ptr)
{
	CHECK_ALIGN(ptr);
	ASSERT(pool->blocks);

	MEM_POOL_ENTRY* cur;
	unsigned int data_unaligned = (unsigned int)ptr;
	//remove align
#if (KERNEL_RANGE_CHECKING)
	unsigned int align_top = 0;
	while (data_unaligned - WORD_SIZE >= pool->base && *((unsigned int*)(data_unaligned - WORD_SIZE)) == MAGIC_RANGE_TOP)
	{
		data_unaligned -= WORD_SIZE;
		align_top += WORD_SIZE;
	}
#endif //KERNEL_RANGE_CHECKING
	while (data_unaligned - WORD_SIZE >= pool->base && *((unsigned int*)(data_unaligned - WORD_SIZE)) == MAGIC_MEM_POOL_ALIGN_SPACE)
		data_unaligned -= WORD_SIZE;

	cur = (MEM_POOL_ENTRY*)ENTRY_PTR(data_unaligned);
	CHECK_MAGIC(cur, MAGIC_MEM_POOL_ENTRY, pool->name);

#if (KERNEL_RANGE_CHECKING)
	unsigned int align_bottom = 0;
	unsigned int entry_end = (unsigned int)cur + ENTRY_SIZE(cur, pool) - WORD_SIZE;
	//skip unused
	while (entry_end > pool->base && *((unsigned int*)entry_end) == MAGIC_MEM_POOL_UNUSED)
		entry_end -= WORD_SIZE;

	while (entry_end > pool->base && *((unsigned int*)entry_end) == MAGIC_RANGE_BOTTOM)
	{
		entry_end -= WORD_SIZE;
		align_bottom += WORD_SIZE;
	}
	if (align_top == 0 || align_bottom == 0 || align_bottom != align_top)
		error_address(ERROR_MEM_POOL_RANGE_CHECK_FAILED, (unsigned int)ptr);
#endif //KERNEL_RANGE_CHECKING
	//find in free list entry before current
	SLIST* prev_free = NULL;
	SLIST* next_free = pool->free_blocks;
	while (next_free && ((unsigned int)next_free < (unsigned int)cur))
	{
		prev_free = next_free;
		next_free = next_free->next;
	}

	if (prev_free)
		slist_add_after(prev_free, FREE_PTR(cur));
	else
		slist_add_head(&pool->free_blocks, FREE_PTR(cur));
	//join with entry before current, if it's empty
	if (FREE_PTR(cur->dlist.prev) == prev_free)
	{
		MEM_POOL_ENTRY* prev = (MEM_POOL_ENTRY*)(cur->dlist.prev);
		CHECK_MAGIC(prev, MAGIC_MEM_POOL_ENTRY, pool->name);
		dlist_remove((DLIST**)&pool->blocks, (DLIST*)cur);

		slist_remove_after(prev_free);
		//in case of next is also free
		cur = prev;
	}
	//join with entry after current
	if (!IS_LAST(cur) && FREE_PTR(cur)->next == FREE_PTR(cur->dlist.next))
	{
		MEM_POOL_ENTRY* next = (MEM_POOL_ENTRY*)(cur->dlist.next);
		CHECK_MAGIC(next, MAGIC_MEM_POOL_ENTRY, pool->name);
		dlist_remove((DLIST**)&pool->blocks, (DLIST*)next);

		slist_remove_after(FREE_PTR(cur));
	}
}

#if (KERNEL_PROFILING)
void mem_pool_stat(MEM_POOL* pool, MEM_POOL_STAT* stat)
{
	stat->free_blocks_count = 0;
	stat->used_blocks_count = 0;
	stat->largest_free = 0;
	stat->total_free = 0;
	stat->total_used = 0;

	SLIST* cur_free = pool->free_blocks;
	MEM_POOL_ENTRY* cur;
	DLIST_ENUM de;
	dlist_enum_start((DLIST**)&pool->blocks, &de);
	while (dlist_enum(&de, (DLIST**)&cur))
	{
		CHECK_MAGIC(cur, MAGIC_MEM_POOL_ENTRY, pool->name);
		//free
		if (FREE_PTR(cur) == cur_free)
		{
			cur_free = cur_free->next;
			stat->free_blocks_count++;
			stat->total_free += ENTRY_DATA_SIZE(cur, pool);
			if (ENTRY_DATA_SIZE(cur, pool) > stat->largest_free)
				stat->largest_free = ENTRY_DATA_SIZE(cur, pool);
		}
		//used
		else
		{
			stat->used_blocks_count++;
			stat->total_used += ENTRY_DATA_SIZE(cur, pool);
		}
	}
}

#endif //KERNEL_PROFILING
