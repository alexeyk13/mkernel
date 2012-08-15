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

#include "queue.h"
#include "sys_call.h"
#include "sys_calls.h"

HANDLE queue_create(unsigned int block_size, unsigned int blocks_count)
{
	return sys_call(QUEUE_CREATE, block_size, blocks_count, WORD_SIZE);
}

HANDLE queue_create_aligned(unsigned int block_size, unsigned int blocks_count, unsigned int align)
{
	return sys_call(QUEUE_CREATE, block_size, blocks_count, align);
}

void* queue_allocate_buffer(HANDLE queue, TIME* timeout)
{
	return (void*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)queue, (unsigned int)timeout, 0);
}

void* queue_allocate_buffer_ms(HANDLE queue, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return (void*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)queue, (unsigned int)&timeout, 0);
}

void* queue_allocate_buffer_us(HANDLE queue, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return (void*)sys_call(QUEUE_ALLOCATE_BUFFER, (unsigned int)queue, (unsigned int)&timeout, 0);
}

void queue_push(HANDLE queue, void* buf)
{
	sys_call(QUEUE_PUSH, (unsigned int)queue, (unsigned int)buf, 0);
}

void* queue_pull(HANDLE queue, TIME* timeout)
{
	return (void*)sys_call(QUEUE_PULL, (unsigned int)queue, (unsigned int)timeout, 0);
}

void* queue_pull_ms(HANDLE queue, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return (void*)sys_call(QUEUE_PULL, (unsigned int)queue, (unsigned int)&timeout, 0);
}

void* queue_pull_us(HANDLE queue, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return (void*)sys_call(QUEUE_PULL, (unsigned int)queue, (unsigned int)&timeout, 0);
}

void queue_release_buffer(HANDLE queue, void* buf)
{
	sys_call(QUEUE_RELEASE_BUFFER, (unsigned int)queue, (unsigned int)buf, 0);
}

bool queue_is_empty(HANDLE queue)
{
	return sys_call(QUEUE_IS_EMPTY, (unsigned int)queue, 0, 0);
}

bool queue_is_full(HANDLE queue)
{
	return sys_call(QUEUE_IS_FULL, (unsigned int)queue, 0, 0);
}

void queue_destroy(HANDLE queue)
{
	sys_call(QUEUE_DESTROY, (unsigned int)queue, 0, 0);
}

HANDLE messages_create(unsigned int messages_count)
{
	return sys_call(QUEUE_CREATE, sizeof(unsigned int), messages_count, WORD_SIZE);
}

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
