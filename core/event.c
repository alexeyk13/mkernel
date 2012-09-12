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

/** \addtogroup event event
	event is a sync object. It's used, when thread(s) are waiting
	event from specific object.

	event can be in 2 states: active and inactive. When event becomes
	active, every event waiting functions returns immediatly. If event
	is inactive, event waiting put thread in waiting state.

	Because event_wait, event_wait_ms, event_wait_us can put current
	thread in waiting state, this functions can be called only from
	SYSTEM/USER context. Other functions, including event_set, event_pulse
	can be called from any context
	\{
 */

#include "event.h"
#include "sys_call.h"
#include "sys_calls.h"

/**
	\brief creates event object.
	\retval event HANDLE on success. On failure (out of memory), error will be raised
*/
HANDLE event_create()
{
	return sys_call(EVENT_CREATE, 0, 0, 0);
}

/**
	\brief make event active, release all waiters, go inactive state
	\param event: event handle
	\retval none
*/
void event_pulse(HANDLE event)
{
	sys_call(EVENT_PULSE, (unsigned int)event, 0, 0);
}

/**
	\brief make event active, release all waiters, stay in active state
	\param event: event handle
	\retval none
*/
void event_set(HANDLE event)
{
	sys_call(EVENT_SET, (unsigned int)event, 0, 0);
}

/**
	\brief check is event is active
	\param event: event handle
	\retval true if active, false if not
*/
bool event_is_set(HANDLE event)
{
	return sys_call(EVENT_IS_SET, (unsigned int)event, 0, 0);
}

/**
	\brief make event inactive
	\param event: event handle
	\retval none
*/
void event_clear(HANDLE event)
{
	sys_call(EVENT_CLEAR, (unsigned int)event, 0, 0);
}

/**
	\brief wait for event
	\param event: event handle
	\param timeout: pointer to TIME structure
	\retval true on success, false on timeout
*/
bool event_wait(HANDLE event, TIME* timeout)
{
	return sys_call(EVENT_WAIT, (unsigned int)event, (unsigned int)timeout, 0);
}

/**
	\brief wait for event
	\param event: event handle
	\param timeout_ms: timeout in milliseconds
	\retval true on success, false on timeout
*/
bool event_wait_ms(HANDLE event, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return sys_call(EVENT_WAIT, (unsigned int)event, (unsigned int)&timeout, 0);
}

/**
	\brief wait for event
	\param event: event handle
	\param timeout_ms: timeout in microseconds
	\retval true on success, false on timeout
*/
bool event_wait_us(HANDLE event, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return sys_call(EVENT_WAIT, (unsigned int)event, (unsigned int)&timeout, 0);
}

/**
	\brief destroy event
	\param event: event handle
	\retval none
*/
void event_destroy(HANDLE event)
{
	sys_call(EVENT_DESTROY, (unsigned int)event, 0, 0);
}

/** \} */ // end of event group
