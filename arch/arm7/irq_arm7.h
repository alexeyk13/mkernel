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

#ifndef IRQ_ARM7_H
#define IRQ_ARM7_H

#include "arch.h"

#define IRQ_LOWEST_PRIORITY					0xff
#define IRQ_NO_IRQ								0x100

#ifndef __ASSEMBLER__
typedef void (*ISR_VECTOR)(void);

void irq_init();
IRQn irq_get_current_vector();
//must be called, when interrupts are disabled
void irq_register_vector(IRQn irq, ISR_VECTOR vector);
void irq_set_priority(IRQn irq, unsigned char priority);
void fiq_register_vector(IRQn irq, ISR_VECTOR vector);
void irq_clear_pending(IRQn irq);

//must be declared by hw
void irq_mask(IRQn irq);
void irq_unmask(IRQn irq);
bool irq_is_masked(IRQn irq);
void fiq_mask();
void fiq_unmask();
bool fiq_is_masked();
#endif //__ASSEMBLER__

#endif // IRQ_ARM7_H
