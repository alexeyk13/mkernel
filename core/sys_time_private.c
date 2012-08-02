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

#include "sys_time_private.h"
#include "kernel_config.h"
#include "error.h"
#include "sys_timer.h"
#include "dbg.h"
#include "irq.h"
#include "sys_calls.h"
#if (SYS_TIMER_SOFT_RTC == 0)
#include "rtc.h"
#else
time_t _sys_diff __attribute__ ((section (".sys_bss"))) =								0;
#endif //SYS_TIMER_SOFT_RTC == 0

void sys_time_init()
{
#if (SYS_TIMER_SOFT_RTC == 0)
	rtc_enable(SYS_TIMER_RTC);
#endif //SYS_TIMER_SOFT_RTC == 0
}

static inline time_t svc_get_sys_time()
{
#if (SYS_TIMER_SOFT_RTC == 0)
	return rtc_get(SYS_TIMER_RTC);
#else
	TIME uptime;
	svc_get_uptime(&uptime);
	return uptime.sec + _sys_diff;
#endif //SYS_TIMER_SOFT_RTC == 0
}

static inline void svc_set_sys_time(time_t time)
{
#if (SYS_TIMER_SOFT_RTC == 0)
	rtc_set(SYS_TIMER_RTC, time);
#else
	TIME uptime;
	svc_get_uptime(&uptime);
	_sys_diff = time - uptime.sec;
#endif //SYS_TIMER_SOFT_RTC == 0
}

unsigned int svc_sys_time_handler(unsigned int num, unsigned int param1)
{
	CHECK_CONTEXT(SYSTEM_CONTEXT | SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	unsigned int res = 0;
	switch (num)
	{
	case TIME_GET_SYS_TIME:
		res = (unsigned int)svc_get_sys_time();
		break;
	case TIME_SET_SYS_TIME:
		svc_set_sys_time((time_t)param1);
		break;
	case TIME_GET_UPTIME:
		res = (unsigned int)svc_get_uptime((TIME*)param1);
		break;
	default:
		error_value(ERROR_GENERAL_INVALID_SYS_CALL, num);
	}
	return res;
}
