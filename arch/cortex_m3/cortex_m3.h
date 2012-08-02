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

#ifndef CORTEX_M3_H
#define CORTEX_M3_H

//r0..r12, sp, lr, pc, psr
#define CONTEXT_SIZE				(16 * 4)

#ifndef __ASSEMBLER__

#include "cc_macro.h"

#define IRQ_STATE					unsigned int

#define reset						NVIC_SystemReset

#define disable_interrupts		__disable_irq
#define enable_interrupts		__enable_irq

__attribute__( ( always_inline ) ) __STATIC_INLINE IRQ_STATE interrupts_save_and_disable(void)
{
  IRQ_STATE result;
  __ASM volatile ("MRS %0, primask\n\t"
						"cpsid i" : "=r" (result) );
  return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void interrupts_restore(IRQ_STATE state)
{
  __ASM volatile ("MSR primask, %0" : : "r" (state) );
}

#endif //__ASSEMBLER__

#endif // CORTEX_M3_H
