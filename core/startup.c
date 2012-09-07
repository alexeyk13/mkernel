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

#include "types.h"
#include <string.h>
#include "arch.h"
#include "memmap.h"
#include "mem_private.h"
#include "sys_time_private.h"
#include "sys_timer.h"
#include "rcc.h"
#include "thread_private.h"
#include "kernel_config.h"
#if (DBG_CONSOLE)
#include "dbg_console_private.h"
#endif //DBG_CONSOLE
#ifndef  NVIC_PRESENT
#include "irq.h"
#endif //NVIC_PRESENT
#include "rand.h"
#if (SW_TIMER_MODULE)
#include "sw_timer.h"
#endif //SW_TIMER_MODULE

extern void application_init();

void startup()
{
	set_core_freq(STARTUP_CORE_FREQ);

#ifndef NVIC_PRESENT
	irq_init();
#endif //NVIC_PRESENT

	//initialize system memory pools
	mem_init();
	//initialize thread subsystem, create idle task
	thread_init();
#if (DBG_CONSOLE)
	dbg_console_create();
#endif
#if (SW_TIMER_MODULE)
	sw_timer_init();
#endif
	//initialize RTC
	sys_time_init();
	//initialize system timers
	sys_timer_init();

	//user application initialize
	application_init();

	//initialize seed
	srand();
}
