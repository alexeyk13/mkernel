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

#include "sys_time.h"
#include "timer.h"
#include "sys_call.h"
#include "sys_calls.h"
#include "irq.h"
#include "dbg.h"

time_t get_sys_time()
{
	return (time_t)sys_call(TIME_GET_SYS_TIME, 0, 0, 0);
}

void set_sys_time(time_t time)
{
	sys_call(TIME_GET_SYS_TIME, (unsigned int)time, 0, 0);
}

TIME* get_uptime(TIME* uptime)
{
	sys_call(TIME_GET_UPTIME, (unsigned int)uptime, 0, 0);
	return uptime;
}

TIME* time_elapsed(TIME* from, TIME* res)
{
	TIME to;
	sys_call(TIME_GET_UPTIME, (unsigned int)&to, 0, 0);
	time_sub(from, &to, res);
	return res;
}

unsigned int time_elapsed_ms(TIME* from)
{
	TIME to;
	sys_call(TIME_GET_UPTIME, (unsigned int)&to, 0, 0);
	time_sub(from, &to, &to);
	return time_to_ms(&to);
}

unsigned int time_elapsed_us(TIME* from)
{
	TIME to;
	sys_call(TIME_GET_UPTIME, (unsigned int)&to, 0, 0);
	time_sub(from, &to, &to);
	return time_to_us(&to);
}
