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

#ifndef ARM7_H
#define ARM7_H

#ifndef __ASSEMBLER__
#include "cc_macro.h"
#include "irq_arm7.h"
#endif //__ASSEMBLER__

#define SVC_MODE					0x13
#define IRQ_MODE					0x12
#define FIQ_MODE					0x11
#define ABORT_MODE				0x17
#define UNDEFINE_MODE			0x1b
#define SYS_MODE					0x1f
#define USER_MODE					0x10

#define I_BIT						0x80
#define F_BIT						0x40

//r0..r12, sp, lr, pc, cpsr
#define CONTEXT_SIZE				(16 * 4)

#define IRQ_STATE					unsigned int

#ifndef __ASSEMBLER__
__attribute__( ( always_inline ) ) __STATIC_INLINE void disable_interrupts(void)
{
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "orr r1, r1, #0xc0\n\t"
						 "msr cpsr_c, r1" : : : "r1", "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void enable_interrupts(void)
{
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "bic r1, r1, #0xc0\n\t"
						 "msr cpsr_c, r1" : : : "r1", "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE IRQ_STATE interrupts_save_and_disable(void)
{
	IRQ_STATE res;
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "mov	%0, r1\n\t"
						 "orr r1, r1, #0xc0\n\t"
						 "msr cpsr_c, r1" : "=r" (res) : : "r1", "cc");
	return res;
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void interrupts_restore(IRQ_STATE state)
{
	__ASM volatile ("msr cpsr_c, %0" : : "r" (state));
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __NOP(void)
{
	__ASM volatile ("nop");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __SWI(uint32_t num)
{
	__ASM volatile ("swi %0" : : "n" (num));
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MODE(void)
{
	uint32_t result;
	__ASM volatile ("mrs %0, cpsr" : "=r" (result) );
	return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_MODE(uint32_t mode)
{
	__ASM volatile ("msr cpsr_c, %0" : : "r" (mode));
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_SP(void)
{
	uint32_t result;
	__ASM volatile ("mov %0, sp" : "=r" (result) );
	return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_IRQ_SP(void)
{
	uint32_t result;
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "msr cpsr_c, #0xd2\n\t"
						 "mov %0, sp\n\t"
						 "msr cpsr_c, r1": "=r" (result) :: "r1");
	return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_FIQ_SP(void)
{
	uint32_t result;
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "msr cpsr_c, #0xd1\n\t"
						 "mov %0, sp\n\t"
						 "msr cpsr_c, r1": "=r" (result) :: "r1");
	return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_ABORT_SP(void)
{
	uint32_t result;
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "msr cpsr_c, #0xd7\n\t"
						 "mov %0, sp\n\t"
						 "msr cpsr_c, r1": "=r" (result) :: "r1");
	return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_UNDEFINE_SP(void)
{
	uint32_t result;
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "msr cpsr_c, #0xdb\n\t"
						 "mov %0, sp\n\t"
						 "msr cpsr_c, r1": "=r" (result) :: "r1");
	return(result);
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_SP(uint32_t sp)
{
	__ASM volatile ("mov sp, %0" : : "r" (sp) );
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_irq(void)
{
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "orr r1, r1, #0x80\n\t"
						 "msr cpsr_c, r1" : : : "r1", "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_fiq(void)
{
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "orr r1, r1, #0x40\n\t"
						 "msr cpsr_c, r1" : : : "r1", "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_irq(void)
{
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "bic r1, r1, #0x80\n\t"
						 "msr cpsr_c, r1" : : : "r1", "cc");
}

__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_fiq(void)
{
	__ASM volatile ("mrs r1, cpsr\n\t"
						 "bic r1, r1, #0x40\n\t"
						 "msr cpsr_c, r1" : : : "r1", "cc");
}
#endif //__ASSEMBLER__

#endif // ARM7_H
