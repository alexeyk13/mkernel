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

#ifndef IRQ_H
#define IRQ_H

/*
	interrupt driver
  */

#include "dev.h"
#include "arch.h"

//context of current execution
typedef enum {
	USER_CONTEXT =											0x1,
	SYSTEM_CONTEXT =										0x2,
	SUPERVISOR_CONTEXT =									0x4,
	IRQ_CONTEXT =											0x8
} CONTEXT;

#define NON_REENTERABLE_ENTER(var)					disable_interrupts(); \
																if (!var) \
																{	var = true; \
																	enable_interrupts();

#define NON_REENTERABLE_EXIT(var)					disable_interrupts(); \
																	var = false;\
																}\
																enable_interrupts();

#define CRITICAL_ENTER									IRQ_STATE __state = interrupts_save_and_disable()
#define CRITICAL_ENTER_AGAIN							disable_interrupts()
#define CRITICAL_LEAVE									interrupts_restore(__state);

//all following is arch-specific
extern void disable_interrupts();
extern void enable_interrupts();
extern CONTEXT get_context();
extern void set_context(CONTEXT context);
extern IRQ_STATE interrupts_save_and_disable(void);
extern void interrupts_restore(IRQ_STATE state);

#endif // IRQ_H
