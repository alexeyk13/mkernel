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

#include "irq.h"
#include "arm7.h"
#include "irq_arm7.h"
#include "irq_arm7_private.h"
#include "dbg.h"
#include "string.h"
#include "memmap.h"
#include "error.h"

void default_irq_handler();
void default_fiq_handler();

//we are really in system mode, or in irq?
IRQn _current_irq __attribute__ ((section (".sys_data")))												= IRQ_NO_IRQ;

//list of pending interrupts
IRQn _irq_pending_list[IRQ_VECTORS_COUNT] __attribute__ ((section (".sys_bss")))					= {0};
int _irq_pending_list_size __attribute__ ((section (".sys_bss")))										= 0;
static uint32_t _irq_pending_mask __attribute__ ((section (".sys_bss")))							= 0;

//nested vector table
unsigned char _irq_priorities[IRQ_VECTORS_COUNT]__attribute__ ((section (".sys_bss")))			= {0};
ISR_VECTOR  _irq_vectors[IRQ_VECTORS_COUNT]__attribute__ ((section (".sys_data")))				= {default_irq_handler};

ISR_VECTOR _fiq_vector __attribute__ ((section (".sys_data")))											= default_fiq_handler;

extern void nested_call(IRQn irq, ISR_VECTOR vector);

void default_irq_handler()
{
#if (KERNEL_DEBUG)
	printf("Warning: irq vector 0x%02X without handler\n\r", irq_get_current_vector());
#endif
}

void default_fiq_handler()
{
#if (KERNEL_DEBUG)
	printf("Warning: unahandled FIQ vector\n\r");
#endif
}

__attribute__( ( always_inline ) ) CONTEXT get_context()
{
	CONTEXT res = SUPERVISOR_CONTEXT;
	switch (__get_MODE() & 0x1f)
	{
	case SYS_MODE:
			res = _current_irq >= IRQ_NO_IRQ ? SYSTEM_CONTEXT : IRQ_CONTEXT;
			break;
	case IRQ_MODE:
	case FIQ_MODE:
			res = IRQ_CONTEXT;
			break;
	case USER_MODE:
			res = USER_CONTEXT;
			break;
	default:
			break;
	}

	return res;
}

void irq_init()
{
	irq_hw_init();
}

IRQn irq_get_current_vector()
{
	return _current_irq;
}

void irq_push_pending(IRQn irq)
{
	int pos = 0;
	//don't pend already pending IRQ to avoid endless pending
	if ((_irq_pending_mask & irq) == 0)
	{
		_irq_pending_mask |= irq;
		if (_irq_pending_list_size)
		{
			//in most cases irq pending list is so short, so incremental search will work faster, than others;
			for(; pos < _irq_pending_list_size; ++ pos)
				if (_irq_priorities[_irq_pending_list[pos]] > _irq_priorities[irq])
					break;
			memmove(_irq_pending_list + pos + 1, _irq_pending_list + pos, (_irq_pending_list_size - pos) * sizeof(IRQn));
		}
		_irq_pending_list[pos] = irq;
		++_irq_pending_list_size;
	}
}

IRQn irq_pop_pending()
{
	IRQn res = _irq_pending_list[0];
	memmove(_irq_pending_list, _irq_pending_list + 1, (--_irq_pending_list_size) * sizeof(IRQn));
	_irq_pending_mask &= ~res;
	return res;
}

void irq_clear_pending(IRQn irq)
{
	disable_interrupts();
	int pos = 0;
	if (_irq_pending_mask & irq)
	{
		//in most cases irq pending list is so short, so incremental search will work faster, than others;
		for(; pos < _irq_pending_list_size; ++ pos)
			if (_irq_pending_list[pos] == irq)
			{
				memmove(_irq_pending_list + pos, _irq_pending_list + pos + 1, (_irq_pending_list_size - pos - 1) * sizeof(IRQn));
				--_irq_pending_list_size;
				break;
			}
		_irq_pending_mask &= ~irq;
	}
	enable_interrupts();
}

void undefined_entry_arm7(unsigned int address)
{
	fatal_error_address(ERROR_GENERAL_UNDEFINED_INSTRUCTION, address);
}

void prefetch_abort_entry_arm7(unsigned int address)
{
	fatal_error_address(ERROR_MEM_INSTRUCTION_ACCESS_VIOLATION, address);
}

void data_abort_entry_arm7(unsigned int address)
{
	fatal_error_address(ERROR_MEM_DATA_ACCESS_VIOLATION, address);
}

void irq_register_vector(IRQn irq, ISR_VECTOR vector)
{
	_irq_vectors[irq] = vector;
}

void irq_set_priority(IRQn irq, unsigned char priority)
{
	ASSERT(priority <= IRQ_LOWEST_PRIORITY);
	_irq_priorities[irq] =  priority;
}

void fiq_register_vector(IRQn irq, ISR_VECTOR vector)
{
	fiq_hw_set_source(irq);
	_fiq_vector = vector;
}
