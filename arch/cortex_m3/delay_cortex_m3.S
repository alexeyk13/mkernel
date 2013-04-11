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

/* exported global constant and functions */
	.global delay_us
	.global delay_ms

/* imported global constant and functions */
	.extern _core_cycles_us
	.extern _core_cycles_ms

	.section	.text, "ax"
	.syntax unified
	.thumb

/*
	void delay_us(unsinged int us)
 */

	.thumb_func
delay_us:
	ldr	r1, =_core_cycles_us
	ldr	r1, [r1]
	mul	r1, r1, r0
	@2 cycles - calling and return, 4 instructions
	mov	r0, #6
us_loop:
	add	r1, r1, #3
	cmp	r1, r0
	blo	us_loop

	bx		lr

	.thumb_func
delay_ms:
	ldr	r2, =_core_cycles_ms
	ldr	r2, [r2]
	mov	r1, #7

ms_loop:
	nop
	add	r1, r1, #4
	cmp	r1, r2
	blo	ms_loop

	sub	r0, r0, #1
	mov	r1, #4
	cmp	r0, #0
	bne	ms_loop

	bx		lr
