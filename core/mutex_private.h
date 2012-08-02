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

#ifndef MUTEX_PRIVATE_H
#define MUTEX_PRIVATE_H

#include "dlist.h"
#include "thread_private.h"
#include "dbg.h"

typedef struct {
	DLIST list;
	MAGIC;
	//only one item
	THREAD* owner;
	//list
	THREAD* waiters;
}MUTEX;

//update owner priority according lowest priority of owner's all waiters of all mutexes
//can be called from thread_private.c on base priority update
//also called internally on mutex unlock
unsigned int svc_mutex_calculate_owner_priority(THREAD* thread);
//release lock, acquired by mutex. Called from thread_private to release
//locked object - by timeout or thread termination. also can be called on normal release
void svc_mutex_lock_release(MUTEX* mutex, THREAD* thread);

unsigned int svc_mutex_handler(unsigned int num, unsigned int param1, unsigned int param2);

#endif // MUTEX_PRIVATE_H
