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

#ifndef THREAD_H
#define THREAD_H

#include "dlist.h"
#include "sys_time.h"
#include "kernel_config.h"

typedef void (*THREAD_FUNCTION)(void*);

typedef struct {
	const char* name;
	unsigned int stack_size;
	unsigned int priority;
	THREAD_FUNCTION fn;
	void* param;
}THREAD_CALL;

HANDLE thread_create(const char* name, int stack_size, unsigned int priority, THREAD_FUNCTION fn, void* param);
HANDLE thread_create_and_run(const char* name, int stack_size, unsigned int priority, THREAD_FUNCTION fn, void* param);
void thread_unfreeze(HANDLE thread);
void thread_freeze(HANDLE thread);
HANDLE thread_get_current();
void thread_set_priority(HANDLE thread, unsigned int priority);
void thread_set_current_priority(unsigned int priority);
void thread_destroy(HANDLE thread);
void thread_exit();
void sleep(TIME* time);
void sleep_ms(unsigned int ms);
void sleep_us(unsigned int us);

#if (KERNEL_PROFILING)
//this function freeze current thread, then unfrize it again and simulate context switch. main use - test switch context perfomance
void thread_switch_test();
void thread_stat();
void stack_stat();
#endif

#endif // THREAD_H
