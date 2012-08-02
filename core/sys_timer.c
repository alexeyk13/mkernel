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

#include "sys_timer.h"
#include "kernel_config.h"
#include "timer.h"
#include "dbg.h"
#include "irq.h"
#include "string.h"
#if (SYS_TIMER_SOFT_RTC == 0)
#include "rtc.h"
#endif //SYS_TIMER_SOFT_RTC

#define TIMER_ONE_SECOND																					1000000
#define TIMER_FREE_RUN																						(TIMER_ONE_SECOND * 2)

static TIME _uptime __attribute__ ((section (".sys_bss"))) =							{0};

static TIMER* _timers[SYS_TIMER_CACHE_SIZE]__attribute__ ((section (".sys_bss"))) =		{NULL};
static TIMER* _timers_uncached __attribute__ ((section (".sys_bss"))) =						NULL;
static int _timers_count __attribute__ ((section (".sys_bss"))) =								0;
volatile bool _timer_inside_isr __attribute__ ((section (".sys_bss"))) =					false;
static unsigned int  _hpet_value __attribute__ ((section (".sys_bss"))) =					0;

static inline TIMER* pop_timer()
{
	TIMER* cur = _timers[0];
	memmove(_timers + 0, _timers + 1, (--_timers_count) * sizeof(void*));
	if (_timers_count >= SYS_TIMER_CACHE_SIZE)
	{
		_timers[SYS_TIMER_CACHE_SIZE - 1] = _timers_uncached;
		dlist_remove_head((DLIST**)&_timers_uncached);
	}
	return cur;
}

static inline void find_shoot_next()
{
	TIMER* to_shoot;

	do {
		to_shoot = NULL;
		CRITICAL_ENTER;
		if (_timers_count)
		{
			if (_timers[0]->time.sec < _uptime.sec || ((_timers[0]->time.sec == _uptime.sec) && (_timers[0]->time.usec <= _uptime.usec)))
				to_shoot = pop_timer();
			else if (_timers[0]->time.sec == _uptime.sec)
			{
				_uptime.usec += timer_elapsed(SYS_TIMER_HPET);
				timer_stop(SYS_TIMER_HPET);
				_hpet_value = _timers[0]->time.usec - _uptime.usec;
				timer_start(SYS_TIMER_HPET, _hpet_value);
			}
		}
		CRITICAL_LEAVE;
		if (to_shoot)
		{
			_timer_inside_isr = true;
			to_shoot->callback(to_shoot->param);
			_timer_inside_isr = false;
		}
	} while (to_shoot);
}

void hpet_on_isr(TIMER_CLASS timer)
{
	_uptime.usec += _hpet_value;
	_hpet_value = 0;
	timer_start(SYS_TIMER_HPET, TIMER_FREE_RUN);
	find_shoot_next();
}

#if (SYS_TIMER_SOFT_RTC)
void rtc_on_isr(TIMER_CLASS timer)
#else
void rtc_on_isr(RTC_CLASS rtc)
#endif //SYS_TIMER_SOFT_RTC
{
	_hpet_value = 0;
	timer_stop(SYS_TIMER_HPET);
	timer_start(SYS_TIMER_HPET, TIMER_FREE_RUN);
	++_uptime.sec;
	_uptime.usec = 0;

	find_shoot_next();
}

void sys_timer_init()
{
#if (SYS_TIMER_SOFT_RTC)
	timer_enable(SYS_TIMER_SOFT_RTC, rtc_on_isr, SYS_TIMER_PRIORITY, 0);
	timer_start(SYS_TIMER_SOFT_RTC, 1000000);
#else
	rtc_enable_second_tick(SYS_TIMER_RTC, rtc_on_isr, SYS_TIMER_PRIORITY);
#endif //SYS_TIMER_SOFT_RTC
	timer_enable(SYS_TIMER_HPET, hpet_on_isr, SYS_TIMER_PRIORITY, TIMER_FLAG_ONE_PULSE_MODE);
	timer_start(SYS_TIMER_HPET, TIMER_FREE_RUN);
}

void svc_sys_timer_create(TIMER* timer)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	TIME uptime;
	svc_get_uptime(&uptime);
	time_add(&uptime, &timer->time, &timer->time);
	CRITICAL_ENTER;
	//adjust time, according current uptime
	int list_size = _timers_count;
	if (list_size > SYS_TIMER_CACHE_SIZE)
		list_size = SYS_TIMER_CACHE_SIZE;
	//insert timer into queue
	int pos = 0;
	if (_timers_count)
	{
		int first = 0;
		int last = list_size - 1;
		int mid;
		while (first < last)
		{
			mid = (first + last) >> 1;
			if (time_compare(&_timers[mid]->time, &timer->time) >= 0)
				first = mid + 1;
			else
				last = mid;
		}
		pos = first;
		if (time_compare(&_timers[pos]->time, &timer->time) >= 0)
			++pos;
	}
	//we have space in cache?
	if (pos < SYS_TIMER_CACHE_SIZE)
	{
		//last is going out ouf cache
		if (_timers_count >= SYS_TIMER_CACHE_SIZE)
			dlist_add_head((DLIST**)&_timers_uncached, (DLIST*)timer);
		memmove(_timers + pos + 1, _timers + pos, (list_size - pos) * sizeof(void*));
		_timers[pos] = timer;

	}
	//find and allocate timer on uncached list
	else
	{
		//top
		if (_timers_uncached == NULL || time_compare(&_timers_uncached->time, &timer->time) < 0)
			dlist_add_head((DLIST**)&_timers_uncached, (DLIST*)timer);
		//bottom
		else if (time_compare(&((TIMER*)_timers_uncached->list.prev)->time, &timer->time) > 0)
			dlist_add_tail((DLIST**)&_timers_uncached, (DLIST*)timer);
		//in the middle
		else
		{
			DLIST_ENUM de;
			TIMER* cur;
			dlist_enum_start((DLIST**)&_timers_uncached, &de);
			while (dlist_enum(&de, (DLIST**)&cur))
				if (time_compare(&cur->time, &timer->time) < 0)
				{
					dlist_add_before((DLIST**)&_timers_uncached, (DLIST*)cur, (DLIST*)timer);
					break;
				}
		}
	}
	++_timers_count;
	CRITICAL_LEAVE;

	if (!_timer_inside_isr)
		find_shoot_next();
}

void svc_sys_timer_destroy(TIMER* timer)
{
	CHECK_CONTEXT(SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	CRITICAL_ENTER;
	int list_size = _timers_count;
	if (list_size > SYS_TIMER_CACHE_SIZE)
		list_size = SYS_TIMER_CACHE_SIZE;

	int pos = 0;
	int first = 0;
	int last = list_size - 1;
	int mid;
	while (first < last)
	{
		mid = (first + last) >> 1;
		if (time_compare(&_timers[mid]->time, &timer->time) > 0)
			first = mid + 1;
		else
			last = mid;
	}
	pos = first;
	if (time_compare(&_timers[pos]->time, &timer->time) > 0)
		++pos;

	//timer in cache?
	if (pos < SYS_TIMER_CACHE_SIZE)
	{
		memmove(_timers + pos, _timers + pos + 1, (list_size - pos - 1) * sizeof(void*));
		if (_timers_count >= SYS_TIMER_CACHE_SIZE)
		{
			_timers[SYS_TIMER_CACHE_SIZE - 1] = _timers_uncached;
			dlist_remove_head((DLIST**)&_timers_uncached);
		}
	}
	//timer in uncached area
	else
	{
		DLIST_ENUM de;
		TIMER* cur;
		dlist_enum_start((DLIST**)&_timers_uncached, &de);
		while (dlist_enum(&de, (DLIST**)&cur))
			if (cur == timer)
			{
				dlist_remove((DLIST**)&_timers_uncached, (DLIST*)&cur);
				break;
			}
	}
	--_timers_count;
	CRITICAL_LEAVE;
}

TIME* svc_get_uptime(TIME* uptime)
{
	CHECK_CONTEXT(SYSTEM_CONTEXT | SUPERVISOR_CONTEXT | IRQ_CONTEXT);
	uptime->sec = _uptime.sec;
	uptime->usec = _uptime.usec + timer_elapsed(SYS_TIMER_HPET);
	while (uptime->usec > TIMER_ONE_SECOND)
	{
		++uptime->sec;
		uptime->usec -= TIMER_ONE_SECOND;
	}
	return uptime;
}
