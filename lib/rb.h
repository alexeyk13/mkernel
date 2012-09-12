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

/** \addtogroup lib_rb ring buffer
	ring buffer routines
	\{
	\}
 */

#include "types.h"
#include "cc_macro.h"

#define RB_ROUND(rb, pos)								(pos >= rb->header.size ? 0 : pos)

typedef struct {
	unsigned int head, tail, size;
}RB_HEADER;

typedef struct {
	RB_HEADER header;
	char data[((unsigned int)-1) >> 1];
}RB;

/** \addtogroup lib_rb ring buffer
	\{
 */

/**
	\brief initialize ring buffer structure
	\param rb: pointer to allocated \ref RB structure
	\param size: ring buffer size in bytes
	\retval none
*/
__STATIC_INLINE void rb_init(RB* rb, unsigned int size)
{
	rb->header.head = rb->header.tail = 0;
	rb->header.size = size;
}


/**
	\brief check, if ring buffer is empty
	\param rb: pointer to initialized \ref RB structure
	\retval \b true if empty
*/
__STATIC_INLINE bool rb_is_empty(RB* rb)
{
	return rb->header.head == rb->header.tail;
}

/**
	\brief check, if ring buffer is full
	\param rb: pointer to initialized \ref RB structure
	\retval \b true if full
*/
__STATIC_INLINE bool rb_is_full(RB* rb)
{
	return RB_ROUND(rb, rb->header.head + 1) == rb->header.tail;
}

/**
	\brief put item in ring buffer
	\param rb: pointer to initialized \ref RB structure
	\param c: item to put
	\retval none
*/
__STATIC_INLINE void rb_put(RB* rb, char c)
{
	rb->data[rb->header.head] = c;
	rb->header.head = RB_ROUND(rb, rb->header.head + 1);
}

/**
	\brief get item from ring buffer
	\param rb: pointer to initialized \ref RB structure
	\retval received item
*/
__STATIC_INLINE char rb_get(RB* rb)
{
	register char c = rb->data[rb->header.tail];
	rb->header.tail = RB_ROUND(rb, rb->header.tail + 1);
	return c;
}

/**
	\}
 */

#endif // RB_H
