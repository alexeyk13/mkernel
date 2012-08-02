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

#include "arm7.h"
#include "kernel_config.h"
#include "memmap.h"
#include "dbg.h"

#if (KERNEL_PROFILING)
unsigned int stack_used_max(unsigned int top, unsigned int cur)
{
	unsigned int i;
	unsigned int last = cur;
	for (i = cur - sizeof(unsigned int); i >= top; i -= 4)
		if (*(unsigned int*)i != MAGIC_UNINITIALIZED)
			last = i;
	return last;
}

void svc_stack_stat()
{
	unsigned int sp, current_stack, max_stack;
	printf("    type       stack        top\n\r");
	printf("-----------------------------------\n\r");
	//assume we are in supervisor
	sp = (unsigned int)__get_SP();
	current_stack = (SVC_STACK_END - sp) / sizeof(unsigned int);
	max_stack = (SVC_STACK_END - stack_used_max(SVC_STACK_TOP, sp)) / sizeof(unsigned int);
	printf("Supervisor %3d/%3d/%3d   0x%08x\n\r", current_stack, max_stack, SVC_STACK_SIZE, SVC_STACK_TOP);
	sp = (unsigned int)__get_IRQ_SP();
	current_stack = (IRQ_STACK_END - sp) / sizeof(unsigned int);
	max_stack = (IRQ_STACK_END - stack_used_max(IRQ_STACK_TOP, sp)) / sizeof(unsigned int);
	printf("IRQ        %3d/%3d/%3d   0x%08x\n\r", current_stack, max_stack, IRQ_STACK_SIZE, IRQ_STACK_TOP);
	sp = (unsigned int)__get_FIQ_SP();
	current_stack = (FIQ_STACK_END - sp) / sizeof(unsigned int);
	max_stack = (FIQ_STACK_END - stack_used_max(FIQ_STACK_TOP, sp)) / sizeof(unsigned int);
	printf("FIQ        %3d/%3d/%3d   0x%08x\n\r", current_stack, max_stack, FIQ_STACK_SIZE, FIQ_STACK_TOP);
	sp = (unsigned int)__get_ABORT_SP();
	current_stack = (ABT_STACK_END - sp) / sizeof(unsigned int);
	max_stack = (ABT_STACK_END - stack_used_max(ABT_STACK_TOP, sp)) / sizeof(unsigned int);
	printf("Abort      %3d/%3d/%3d   0x%08x\n\r", current_stack, max_stack, ABT_STACK_SIZE, ABT_STACK_TOP);
	sp = (unsigned int)__get_UNDEFINE_SP();
	current_stack = (UND_STACK_END - sp) / sizeof(unsigned int);
	max_stack = (UND_STACK_END - stack_used_max(UND_STACK_TOP, sp)) / sizeof(unsigned int);
	printf("Undefine   %3d/%3d/%3d   0x%08x\n\r", current_stack, max_stack, UND_STACK_SIZE, UND_STACK_TOP);
}
#endif //KERNEL_PROFILING

