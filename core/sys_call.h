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

#ifndef SYS_CALL_H
#define SYS_CALL_H

unsigned int sys_call(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3);
unsigned int sys_handler(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3);

/** \addtogroup arch_porting architecture porting
	\{
 */
/**
	\brief arch-dependent context raiser
	\details If current contex is not enough during sys_call, context is raised, using
	this arch-specific function. After calling, return value is provided.

	For example, for cortex-m3 "svc 0x12" instruction is used.

	\param num: sys-call number
	\param param1: parameter 1. num-specific
	\param param2: parameter 2. num-specific
	\param param3: parameter 3. num-specific
	\retval result value. num-specific
*/
extern unsigned int do_sys_call(unsigned int num, unsigned int param1, unsigned int param2, unsigned int param3);
/** \} */ // end of arch_porting group

#endif // SYS_CALL_H
