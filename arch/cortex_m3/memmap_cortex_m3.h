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

#ifndef MEMMAP_CORTEXM_H
#define MEMMAP_CORTEXM_H

//logical memory layout

/*
	.data
	.bss
	.sys_data
	.sys_bss
	[system pool]

	/\
	[global pool]
	\/

	[thread stacks]
	SVC stack
 */

//external memory layout
#define SVC_STACK_TOP			(INT_RAM_BASE + INT_RAM_SIZE - SVC_STACK_SIZE * 4)
#define SVC_STACK_END			(SVC_STACK_TOP + SVC_STACK_SIZE * 4)

#define SYSTEM_POOL_BASE		((uint32_t)&_sys_end)

#define THREAD_STACK_BASE		(SVC_STACK_TOP - THREAD_STACK_SIZE)
#define THREAD_STACK_ALIGN		8

#define DATA_POOL_BASE			(SYSTEM_POOL_BASE + SYSTEM_POOL_SIZE)
#define DATA_POOL_SIZE			(THREAD_STACK_BASE - DATA_POOL_BASE)

#endif // MEMMAP_CORTEXM_H
