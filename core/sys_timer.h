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

#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include "time.h"
#include "dlist.h"
#include "sys_time.h"

typedef enum {
	//run callback in irq context. System use only.
	TIMER_ACTION_CALLBACK = 0,
	//wakeup thread
	TIMER_ACTION_WAKEUP,
	TIMER_ACTION_SET_EVENT
}TIMER_ACTION;

typedef void (*SYS_TIMER_HANDLER)(void*);

typedef struct {
	DLIST list;
	TIME time;
	TIMER_ACTION action;
	SYS_TIMER_HANDLER callback;
	void* param;
}TIMER;

void sys_timer_init();

void svc_sys_timer_create(TIMER* timer);
void svc_sys_timer_destroy(TIMER* timer);
TIME* svc_get_uptime(TIME* uptime);

#endif // SYS_TIMER_H
