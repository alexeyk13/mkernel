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

/** \addtogroup semaphore semaphore
	semaphore is a sync object. It's used, for signalling sync condition

	Semaphore is quite simple. semaphore_signal increments counter, any
	semaphore_wait*, decrements. If counter reached zero, thread will be putted
	in waiting state until next semaphore_signal.

	Because semaphore_wait, semaphore_wait_ms, semaphore_wait_us can put current
	thread in waiting state, this functions can be called only from
	SYSTEM/USER context. Other functions, including semaphore_signal
	can be called from any context
	\{
 */

#include "sem.h"
#include "sys_call.h"
#include "sys_calls.h"

/**
	\brief creates semaphore object.
	\retval semaphore HANDLE on success. On failure (out of memory), error will be raised
*/
HANDLE semaphore_create()
{
	return sys_call(SEMAPHORE_CREATE, 0, 0, 0);
}

/**
	\brief increments counter
	\param sem: semaphore handle
	\retval none
*/
void semaphore_signal(HANDLE sem)
{
	sys_call(SEMAPHORE_SIGNAL, (unsigned int)sem, 0, 0);
}

/**
	\brief wait for semaphore signal
	\param sem: semaphore handle
	\param timeout: pointer to TIME structure
	\retval true on success, false on timeout
*/
bool semaphore_wait(HANDLE sem, TIME* timeout)
{
	return sys_call(SEMAPHORE_WAIT, (unsigned int)sem, (unsigned int)timeout, 0);
}

/**
	\brief wait for semaphore signal
	\param sem: semaphore handle
	\param timeout_ms: timeout in milliseconds
	\retval true on success, false on timeout
*/
bool semaphore_wait_ms(HANDLE sem, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return sys_call(SEMAPHORE_WAIT, (unsigned int)sem, (unsigned int)&timeout, 0);
}

/**
	\brief wait for semaphore signal
	\param sem: semaphore handle
	\param timeout_us: timeout in microseconds
	\retval true on success, false on timeout
*/
bool semaphore_wait_us(HANDLE sem, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return sys_call(SEMAPHORE_WAIT, (unsigned int)sem, (unsigned int)&timeout, 0);
}

/**
	\brief destroys semaphore
	\param sem: semaphore handle
	\retval none
*/
void semaphore_destroy(HANDLE sem)
{
	sys_call(SEMAPHORE_DESTROY, (unsigned int)sem, 0, 0);
}

/** \} */ // end of semaphore group
