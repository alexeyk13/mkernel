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

/** \addtogroup data_queue data queue
	data queue is a sync object. It's used, to send data objects
	from one thread to another.

	If there is no object to pull, thread, thrying to pull object
	will be put in waiting state, until other thread is not push

	sequence for sending:

	void* buf = queue_allocate_buffer*(queue, <time>);
	<fill buf object>
	queue_push(queue, buf);

	sequence for receiving:

	void* buf = queue_pull*(queue, <time>);
	<process buf object>
	queue_release_buffer(queue, buf);

	Plese mind, that space for data queue is allocated in current thread's
	memory pool.

	queue_pull* and queue_allocate_buffer* functions can be called from
	IRQ/SVC context only if they doesn't put current thread in waiting state. Check
	queue_is_empty or, corresponding, queue_is_full before calling, or exception will be raised.
	From SYSTEM/USER there are no restrictions on function call.
	\{
 */

#include "queue.h"
#include "sys_call.h"
#include "sys_calls.h"

/**
	\brief creates queue object.
	\details Mind, that data block will be allocated in current thread's memory pool.
	This memory pool can be destroyed only after queue destruction
	\param block_size: size of single memory block in bytes
	\param blocks_count: count of allocated blocks
	\retval queue HANDLE on success. On failure (out of memory), error will be raised
*/
HANDLE queue_create(unsigned int block_size, unsigned int blocks_count)
{
	return sys_call(QUEUE_CREATE, block_size, blocks_count, WORD_SIZE);
}

/**
	\brief creates queue object with block specific align.
	\details Mind, that data block will be allocated in current thread's memory pool.
	This memory pool can be destroyed only after queue destruction.
	\param block_size: size of single memory block in bytes
	\param blocks_count: count of allocated blocks
	\param align: block align. Must be multiples of	WORD_SIZE() and greater 0.
	\retval queue HANDLE on success. On failure (out of memory), error will be raised
*/
HANDLE queue_create_aligned(unsigned int block_size, unsigned int blocks_count, unsigned int align)
{
	return sys_call(QUEUE_CREATE, block_size, blocks_count, align);
}

/**
	\brief allocate buffer in queue.
	\param queue: data queue
	\param timeout: pointer to TIME structure
	\retval buffer on success, NULL on timeout
*/
void* queue_allocate_buffer(HANDLE queue, TIME* timeout)
{
	return (void*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)queue, (unsigned int)timeout, 0);
}

/**
	\brief allocate buffer in queue.
	\param queue: data queue
	\param timeout_ms: timeout in milliseconds
	\retval buffer on success, NULL on timeout
*/
void* queue_allocate_buffer_ms(HANDLE queue, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return (void*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)queue, (unsigned int)&timeout, 0);
}

/**
	\brief allocate buffer in queue.
	\param queue: data queue
	\param timeout_us: timeout in microseconds
	\retval buffer on success, NULL on timeout
*/
void* queue_allocate_buffer_us(HANDLE queue, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return (void*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)queue, (unsigned int)&timeout, 0);
}

/**
	\brief push buffer to queue
	\param queue: data queue
	\param buf: data buffer
	\retval none
*/
void queue_push(HANDLE queue, void* buf)
{
	sys_call(QUEUE_PUSH, (unsigned int)queue, (unsigned int)buf, 0);
}

/**
	\brief pull buffer from queue.
	\param queue: data queue
	\param timeout: pointer to TIME structure
	\retval buffer on success, NULL on timeout
*/
void* queue_pull(HANDLE queue, TIME* timeout)
{
	return (void*)sys_call(QUEUE_PULL, (unsigned int)queue, (unsigned int)timeout, 0);
}

/**
	\brief pull buffer from queue.
	\param queue: data queue
	\param timeout_ms: timeout in milliseconds
	\retval buffer on success, NULL on timeout
*/
void* queue_pull_ms(HANDLE queue, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return (void*)sys_call(QUEUE_PULL, (unsigned int)queue, (unsigned int)&timeout, 0);
}

/**
	\brief pull buffer from queue.
	\param queue: data queue
	\param timeout_us: timeout in microseconds
	\retval buffer on success, NULL on timeout
*/
void* queue_pull_us(HANDLE queue, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return (void*)sys_call(QUEUE_PULL, (unsigned int)queue, (unsigned int)&timeout, 0);
}

/**
	\brief release pulled buffer
	\param queue: data queue
	\param buf: data buffer
	\retval none
*/
void queue_release_buffer(HANDLE queue, void* buf)
{
	sys_call(QUEUE_RELEASE_BUFFER, (unsigned int)queue, (unsigned int)buf, 0);
}

/**
	\brief check if queue is empty
	\param queue: data queue
	\retval true if empty, false if not
*/
bool queue_is_empty(HANDLE queue)
{
	return sys_call(QUEUE_IS_EMPTY, (unsigned int)queue, 0, 0);
}

/**
	\brief check if queue is full
	\param queue: data queue
	\retval true if full, false if not
*/
bool queue_is_full(HANDLE queue)
{
	return sys_call(QUEUE_IS_FULL, (unsigned int)queue, 0, 0);
}

/**
	\brief destroy queue
	\param queue: data queue
	\retval none
*/
void queue_destroy(HANDLE queue)
{
	sys_call(QUEUE_DESTROY, (unsigned int)queue, 0, 0);
}

/** \} */ // end of data_queue group

/** \addtogroup message_queue message queue
	message queue is a sync object. It's used, to send messages
	from one thread to another.

	Basically, message queue is \ref data_queue, with data object size
	of WORD_SIZE() and simplified interface. All \ref data_queue functions
	can be used for advanced functionality.

	Plese mind, that space for message queue is allocated in current thread's
	memory pool.

	message_post* and message_peek* functions can be called from
	IRQ/SVC context only if they doesn't put current thread in waiting state. Check
	queue_is_empty or, corresponding, queue_is_full before calling, or exception will be raised.
	From SYSTEM/USER there are no restrictions on function call.

	INVALID_HANDLE message (int == 0) is reserved for system use. Please, don't use 0 as message.
	\{
 */
/**
	\brief creates message queue object.
	\details Mind, that data block will be allocated in current thread's memory pool.
	This memory pool can be destroyed only after queue destruction
	\param messages_count: maximal messages count
	\retval message queue HANDLE on success. On failure (out of memory), error will be raised
*/
HANDLE messages_create(unsigned int messages_count)
{
	return sys_call(QUEUE_CREATE, sizeof(unsigned int), messages_count, WORD_SIZE);
}

/**
	\brief post message
	\param messages: message queue
	\param message: message to post
	\param timeout: pointer to TIME structure
	\retval true on success, false on timeout
*/
bool messages_post(HANDLE messages, unsigned int message, TIME* timeout)
{
	unsigned int* buf = (unsigned int*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)messages, (unsigned int)timeout, 0);
	if (buf)
	{
		*buf = message;
		sys_call(QUEUE_PUSH, (unsigned int)messages, (unsigned int)buf, 0);
	}
	return buf != NULL;
}

/**
	\brief post message
	\param messages: message queue
	\param message: message to post
	\param timeout_ms: timeout in milliseconds
	\retval true on success, false on timeout
*/
bool messages_post_ms(HANDLE messages, unsigned int message, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	unsigned int* buf = (unsigned int*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)messages, (unsigned int)&timeout, 0);
	if (buf)
	{
		*buf = message;
		sys_call(QUEUE_PUSH, (unsigned int)messages, (unsigned int)buf, 0);
	}
	return buf != NULL;
}

/**
	\brief post message
	\param messages: message queue
	\param message: message to post
	\param timeout_us: timeout in microseconds
	\retval true on success, false on timeout
*/
bool messages_post_us(HANDLE messages, unsigned int message, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	unsigned int* buf = (unsigned int*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)messages, (unsigned int)&timeout, 0);
	if (buf)
	{
		*buf = message;
		sys_call(QUEUE_PUSH, (unsigned int)messages, (unsigned int)buf, 0);
	}
	return buf != NULL;
}

/**
	\brief peek message
	\param messages: message queue
	\param timeout: pointer to TIME structure
	\retval message on success, INVALID_HANDLE on timeout
*/
unsigned int messages_peek(HANDLE messages, TIME* timeout)
{
	unsigned int* buf = (unsigned int*)sys_call(QUEUE_PULL, (unsigned int)messages, (unsigned int)timeout, 0);
	unsigned int message = 0;
	if (buf)
	{
		message = *buf;
		sys_call(QUEUE_RELEASE_BUFFER, (unsigned int)messages, (unsigned int)buf, 0);
	}
	return message;
}

/**
	\brief peek message
	\param messages: message queue
	\param timeout_ms: timeout in milliseconds
	\retval message on success, INVALID_HANDLE on timeout
*/
unsigned int messages_peek_ms(HANDLE messages, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	unsigned int* buf = (unsigned int*)sys_call(QUEUE_PULL, (unsigned int)messages, (unsigned int)&timeout, 0);
	unsigned int message = 0;
	if (buf)
	{
		message = *buf;
		sys_call(QUEUE_RELEASE_BUFFER, (unsigned int)messages, (unsigned int)buf, 0);
	}
	return message;
}

/**
	\brief peek message
	\param messages: message queue
	\param timeout_us: timeout in microseconds
	\retval message on success, INVALID_HANDLE on timeout
*/
unsigned int messages_peek_us(HANDLE messages, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	unsigned int* buf = (unsigned int*)sys_call(QUEUE_PULL, (unsigned int)messages, (unsigned int)&timeout, 0);
	unsigned int message = 0;
	if (buf)
	{
		message = *buf;
		sys_call(QUEUE_RELEASE_BUFFER, (unsigned int)messages, (unsigned int)buf, 0);
	}
	return message;
}

/** \} */ // end of message_queue group
