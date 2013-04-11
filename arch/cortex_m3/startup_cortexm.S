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

//if you've got error in this line, than this file is compiled wrong

#include "arch.h"
#include "memmap.h"
#include "hw_config.h"
#include "kernel_config.h"
#include "magic.h"
#include "vectors_cortex_m3.h"

/* Define constants used in low-level initialization.  */

	/*
		context:

		r4-r11
		r0-r3, r12
		lr
		pc
		psr
	  */
	.equ	SP_CUR_OFFSET,				0x0c
	.equ	R0_OFFSET_IN_CONTEXT,	(8 * 4)
	.equ	LR_OFFSET_IN_CONTEXT,	(13 * 4)
	.equ	PC_OFFSET_IN_CONTEXT,	(14 * 4)
	.equ	PSR_OFFSET_IN_CONTEXT,	(15 * 4)

	.equ	MINIMAL_CONTEXT_SIZE,	(8 * 4)
	.equ	LR_IN_MINIMAL_CONTEXT,	(5 * 4)
	.equ	PC_IN_MINIMAL_CONTEXT,	(6 * 4)
	.equ	PSR_IN_MINIMAL_CONTEXT,	(7 * 4)

	.equ	LR_TO_HANDLER,				0xfffffff1
	.equ	LR_TO_THREAD_PSP,			0xfffffffd
	.equ	INITIAL_PSP_VALUE,		0x01000000
	.equ	SVC_PSP_VALUE,				0x0100000b

	.equ	ICSR,							0xe000ed04
	.equ	CCR,							0xe000ed14
	.equ	SHPR1,						0xe000ed18
	.equ	SHPR2,						0xe000ed1c
	.equ	SHPR3,						0xe000ed20
	.equ	SHCSR,						0xe000ed24

	.equ	PEND_SV_BIT,				(1 << 28)
	.equ	SHPR1_VALUE,				((0x02 << 0) | (0x02 << 8) | (0x02 << 16))
	.equ	SHPR2_VALUE,				(0xff << 24)
	.equ	SHPR3_VALUE,				(0xff << 16)

/* imported global constants and functions */
	.extern _etext
	.extern _data
	.extern _edata
	.extern _bss
	.extern _bss_end
	.extern _etext_data
	.extern _sys_data
	.extern _esys_data
	.extern _sys_bss
	.extern _sys_bss_end

	.extern on_hard_fault
	.extern on_mem_manage
	.extern on_bus_fault
	.extern on_usage_fault
	.extern sys_handler
	.extern startup
	.extern main

	.extern _active_thread
	.extern _next_thread
	.extern abnormal_exit

#if (STARTUP_HARDWARE_INIT)
	.extern startup_hardware_init
#endif //STARTUP_HARDWARE_INIT

/* exported global constant and functions */
	.global _in_startup

	.global do_sys_call
	.global pend_switch_context
	.global thread_patch_context
	.global thread_setup_context

/* data segment */

	.section	.sys_data, "aw"

t_flag: .word 0

/* interrupt vectors */
	.section  .reset, "ax"

int_vectors:
	.word  SVC_STACK_END
	.word  Reset_Handler
	.word  NMI_Handler
	.word  HardFault_Handler
	.word  MemManage_Handler
	.word  BusFault_Handler
	.word  UsageFault_Handler
	.word  0
	.word  0
	.word  0
	.word  0
	.word  SVC_Handler
	.word  DebugMon_Handler
	.word  0
	.word  PendSV_Handler
	.word  SysTick_Handler

	@defined in macro
	external_vectors

/*********************** reset vector handler *********************/
	.syntax unified
	.cpu cortex-m3
	.thumb

	.thumb_func
Reset_Handler:
	cpsid	i

#if (STARTUP_HARDWARE_INIT)
	bl		startup_hardware_init
#endif //STARTUP_HARDWARE_INIT

	/* initialize memory */
	@ copy data from flash to ram
	ldr	r0, =_etext
	ldr	r1, =_data
	ldr	r2, =_edata
data_loop:
	cmp	r1, r2
	ittt	lo
	ldrlo	r3, [r0], #4
	strlo	r3, [r1], #4
	blo	data_loop

	@ zero bss area
	mov	r0, #0
	ldr	r1, =_bss
	ldr	r2, =_bss_end
bss_loop:
	cmp	r1, r2
	itt	lo
	strlo	r0, [r1], #4
	blo	bss_loop

	@ copy data from flash to system ram
	ldr	r0, =_etext_data
	ldr	r1, =_sys_data
	ldr	r2, =_esys_data
sys_data_loop:
	cmp	r1, r2
	ittt	lo
	ldrlo	r3, [r0], #4
	strlo	r3, [r1], #4
	blo	sys_data_loop

	@ zero system bss area
	mov	r0, #0
	ldr	r1, =_sys_bss
	ldr	r2, =_sys_bss_end
sys_bss_loop:
	cmp	r1, r2
	itt	lo
	strlo	r0, [r1], #4
	blo	sys_bss_loop

#if (KERNEL_PROFILING)
	ldr	r0, =MAGIC_UNINITIALIZED
	ldr	r1, =SVC_STACK_TOP
	ldr	r2, =SVC_STACK_END
svc_stack_fill:
	cmp	r1, r2
	itt	lo
	strlo	r0, [r1], #4
	blo	svc_stack_fill
#endif //KERNEL_PROFILING

	@setup system and fault handlers priority
	ldr	r1, =SHPR1
	ldr	r0, =SHPR1_VALUE
	str	r0, [r1]

	ldr	r1, =SHPR2
	ldr	r0, =SHPR2_VALUE
	str	r0, [r1]

	ldr	r1, =SHPR3
	ldr	r0, =SHPR3_VALUE
	str	r0, [r1]

	@enable detailed faults
	ldr	r1, =SHCSR
	mov	r2, #7
	lsl	r2, r2, #16
	ldr	r0, [r1]
	orr	r0, r0, r2
	str	r0, [r1]

	bl    startup			               @ to high-level initialization

	@make context and sp switch
	cpsie	i
	@never reach
	b		.

	/* code segment */

	.section  .text, "ax"
	.syntax unified
	.thumb

/*********************** exception vectors handlers *********************/
.macro exception_enter
	mov	r0, lr
	cmp	r0, 0xfffffffd
	bne	1f
	mrs	r1, psp
	b		2f
1:
	mrs	r1, msp
2:
.endm


	.thumb_func
HardFault_Handler:
	exception_enter
	b	on_hard_fault

	.thumb_func
MemManage_Handler:
	exception_enter
	b	on_mem_manage

	.thumb_func
BusFault_Handler:
	exception_enter
	b	on_bus_fault

	.thumb_func
UsageFault_Handler:
	exception_enter
	b	on_usage_fault

	.thumb_func
SVC_Handler:
	mrs	r12, psp
	ldmia	r12, {r0-r3}
	bl		sys_handler							@ call c handler

	@patch registers in minimal context
	mrs	r12, psp
	str	r0, [r12]

	//return to thread mode
	ldr	pc, =0xfffffffd

	.thumb_func
PendSV_Handler:
	/*save*/
	cpsid	i
	ldr	r2, =_next_thread
	ldr	r2, [r2]
	cmp	r2, #0								@late arrival of pendSV can cause double-calling
	beq	context_exit
	ldr	r3, =_active_thread				@_active_thread will be NULL on startup/task destroy
	ldr	r3, [r3]
	cmp	r3, #0
	beq	load_context

	mrs	r0, psp
	stmdb	r0!, {r4-r11}						@save other context on stack
	str	r0, [r3, #SP_CUR_OFFSET]		@save sp on thread sp_cur

load_context:
	ldr	r0, [r2, #SP_CUR_OFFSET]		@load sp from thread sp_cur
	ldmia	r0!, {r4-r11}						@load other context from stack
	msr	psp, r0

	ldr	r0, =_active_thread
	str	r2, [r0]
	ldr	r0, =_next_thread
	mov	r2, #0
	str	r2, [r0]

context_exit:
	cpsie i
	//return to thread mode
	ldr	pc, =0xfffffffd


	.thumb_func
default_handler:
	ldr	pc, =default_irq_handler
/************************** sys calls *************************/

/*
	extern unsigned int do_sys_call(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3);
 */

	.thumb_func
do_sys_call:
	svc	0x12
	bx		lr

/*********************** context specific *********************/
/*
	void pend_switch_context(void)
*/
	.thumb_func
pend_switch_context:
	ldr	r1, =ICSR
	ldr	r0, =PEND_SV_BIT
	str	r0, [r1]
	bx		lr

/*
	void thread_patch_context(THREAD* thread, unsigned int res);
 */
	.thumb_func
thread_patch_context:
	ldr	r2, [r0, #SP_CUR_OFFSET]
	str	r1, [r2, #R0_OFFSET_IN_CONTEXT]

	bx		lr
/*
	void thread_setup_context(THREAD* thread, THREAD_FUNCTION fn, void* param);
*/
	.thumb_func
thread_setup_context:
	ldr	r12, [r0, #SP_CUR_OFFSET]
	sub	r12,	r12, #CONTEXT_SIZE
	str	r2, [r12, #R0_OFFSET_IN_CONTEXT]				@param
	ldr	r3, =abnormal_exit
	str	r3, [r12, #LR_OFFSET_IN_CONTEXT]				@abnormal thread exit
	str	r1, [r12, #PC_OFFSET_IN_CONTEXT]				@entry point
	mov	r3, #INITIAL_PSP_VALUE							@T-Bit only
	str	r3, [r12, #PSR_OFFSET_IN_CONTEXT]			@run in system context, interrupts are enabled
	str	r12, [r0, #SP_CUR_OFFSET]

	bx		lr

/*********************** weak unhandled vectors *********************/
.macro weak_vector vector
	.weak	\vector
	.thumb_set \vector,default_handler
.endm

weak_vector	NMI_Handler
weak_vector	DebugMon_Handler
weak_vector	SysTick_Handler

external_weak_vectors
