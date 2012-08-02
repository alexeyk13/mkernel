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

#include "sem.h"
#include "sys_call.h"
#include "sys_calls.h"

HANDLE semaphore_create()
{
	return sys_call(SEMAPHORE_CREATE, 0, 0, 0);
}

void semaphore_signal(HANDLE sem)
{
	sys_call(SEMAPHORE_SIGNAL, (unsigned int)sem, 0, 0);
}

bool semaphore_wait(HANDLE sem, TIME* timeout)
{
	return sys_call(SEMAPHORE_WAIT, (unsigned int)sem, (unsigned int)timeout, 0);
}

bool semaphore_wait_ms(HANDLE sem, unsigned int timeout_ms)
{
	TIME timeout;
	ms_to_time(timeout_ms, &timeout);
	return sys_call(SEMAPHORE_WAIT, (unsigned int)sem, (unsigned int)&timeout, 0);
}

bool semaphore_wait_us(HANDLE sem, unsigned int timeout_us)
{
	TIME timeout;
	us_to_time(timeout_us, &timeout);
	return sys_call(SEMAPHORE_WAIT, (unsigned int)sem, (unsigned int)&timeout, 0);
}

void semaphore_destroy(HANDLE sem)
{
	sys_call(SEMAPHORE_DESTROY, (unsigned int)sem, 0, 0);
}

