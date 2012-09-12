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

#ifndef RB_H
#define RB_H

/** \addtogroup lib_rb_block block based ring buffer
	block based ring buffer routines
	\{
	\}
 */

#include "types.h"
#include "cc_macro.h"

#define RB_BLOCK_ROUND(rb, pos)								(pos >= rb->header.blocks_count ? 0 : pos)

typedef struct {
	unsigned int head, tail, block_size, blocks_count;
}RB_BLOCK_HEADER;

typedef struct {
	RB_BLOCK_HEADER header;
	char data[((unsigned int)-1) >> 1];
}RB_BLOCK;

/** \addtogroup lib_rb_block block based ring buffer
	\{
 */

/**
	\brief initialize ring buffer structure
	\param rb: pointer to allocated \ref RB_BLOCK structure
	\param block_size: size of block in bytes
	\param blocks_count: count of blocks
	\retval none
*/
__STATIC_INLINE void rb_block_init(RB_BLOCK* rb, unsigned int block_size, unsigned int blocks_count)
{
	rb->header.head = rb->header.tail = 0;
	rb->header.block_size = block_size;
	rb->header.blocks_count = blocks_count;
}


/**
	\brief check, if ring buffer is empty
	\param rb: pointer to initialized \ref RB_BLOCK structure
	\retval \b true if empty
*/
__STATIC_INLINE bool rb_block_is_empty(RB_BLOCK* rb)
{
	return rb->header.head == rb->header.tail;
}

/**
	\brief check, if ring buffer is full
	\param rb: pointer to initialized \ref RB_BLOCK structure
	\retval \b true if full
*/
__STATIC_INLINE bool rb_block_is_full(RB_BLOCK* rb)
{
	return RB_BLOCK_ROUND(rb, rb->header.head + 1) == rb->header.tail;
}

/**
	\brief put item in ring buffer
	\param rb: pointer to initialized \ref RB_BLOCK structure
	\retval pointer to memory location, where caller need to put block
*/
__STATIC_INLINE void* rb_block_put(RB_BLOCK* rb)
{
	void* res = (void*)(rb->data + rb->header.head * rb->header.block_size);
	rb->header.head = RB_BLOCK_ROUND(rb, rb->header.head + 1);
	return res;
}

/**
	\brief get item from ring buffer
	\param rb: pointer to initialized \ref RB_BLOCK structure
	\retval pointer to memory location, from where caller can get block
*/
__STATIC_INLINE void* rb_block_get(RB_BLOCK* rb)
{
	void* res = (void*)(rb->data + rb->header.tail * rb->header.block_size);
	rb->header.tail = RB_BLOCK_ROUND(rb, rb->header.tail + 1);
	return res;
}

/**
	\}
 */

#endif // RB_H
