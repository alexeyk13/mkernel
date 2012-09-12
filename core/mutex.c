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

/** \addtogroup mutex mutex
	mutex is a sync object. It's used for exclusive object access.

	First thread, requested mutex_lock() will get access for resource,
	while other threads, trying to call mutex_lock() will be putted in
	"waiting" state until owner thread release resource by mutex_unlock(),
	or, timeout condition is meet.

	caller, used timeout condition must check result of mutex_lock() call.

	in RTOS, if higher priority thread is trying to acquire mutex, owned lower
	priority thread, on mutex_lock(), mutex owner priority will be temporaily
	raised to caller's priority, to avoid deadlock. This condition is called
	mutex inheritance.

	Because thread can own many mutexes, effectivy priority is calculated by
	highest priority of all mutex-waiters of current thread. This condition is
	called nested mutex priority inheritance.

	After releasing mutex, thread priority is returned to base.

	Because mutex_lock can put current thread in waiting state, mutex
	locking/unlocking can be called only from SYSTEM/USER contex
	\{
 */

#include "mutex.h"
#include "sys_call.h"
#include "sys_calls.h"

/**
	\brief creates mutex object.
	\retval mutex HANDLE on success. On failure (out of memory), error will be raised
*/
HANDLE mutex_create()
{
	return sys_call(MUTEX_CREATE, 0, 0, 0);
}

/**
	\brief try to lock mutex.
	\details If mutex is already locked, exception is raised and current thread terminated
	\param mutex: mutex handle
	\param timeout: pointer to TIME structure
	\retval true on success, false on timeout
*/
bool mutex_lock(HANDLE mutex, TIME* timeout)
{
	return sys_call(MUTEX_LOCK, (unsigned int)mutex, (unsigned int)timeout, 0);
}

/**
	\brief try to lock mutex
	\details If mutex is already locked, exception is raised and current thread terminated
	\param mutex: mutex handle
	\param ms: time to try in milliseconds. Can be INFINITE
	\retval true on success, false on timeout
*/
bool mutex_lock_ms(HANDLE mutex, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return sys_call(MUTEX_LOCK, (unsigned int)mutex, (unsigned int)&timeout, 0);
}

/**
	\brief try to lock mutex
	\details If mutex is already locked, exception is raised and current thread terminated
	\param us: time to try in microseconds. Can be INFINITE
	\retval true on success, false on timeout
*/
bool mutex_lock_us(HANDLE mutex, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return sys_call(MUTEX_LOCK, (unsigned int)mutex, (unsigned int)&timeout, 0);
}

/**
	\brief unlock mutex
	\details If mutex is not locked, exception is raised and current thread terminated
	\param mutex: mutex handle
	\retval none
*/
void mutex_unlock(HANDLE mutex)
{
	sys_call(MUTEX_UNLOCK, (unsigned int)mutex, 0, 0);
}

/**
	\brief destroy mutex
	\param mutex: mutex handle
	\retval none
*/
void mutex_destroy(HANDLE mutex)
{
	sys_call(MUTEX_DESTROY, (unsigned int)mutex, 0, 0);
}

/** \} */ // end of mutex group
