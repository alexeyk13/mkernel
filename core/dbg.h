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

#ifndef DBG_H
#define DBG_H

/*
		dbg.h: debug-specific
  */

#include "kernel_config.h"
#include "types.h"
#include "printf.h"
#include "dbg_console.h"

#if (KERNEL_MARKS)
#include "magic.h"
#endif

#if (KERNEL_DEBUG)
#include "error.h"

#define HALT()											{dbg_push(); for (;;) {}}
#define ASSERT(cond)									if (!(cond))	{printf("ASSERT at %s, line %d\n\r", __FILE__, __LINE__);	HALT();}

#else

#define HALT()
#define ASSERT(cond)

#endif

#if (KERNEL_CHECK_CONTEXT)
#define CHECK_CONTEXT(value)						if ((get_context() & (value)) == 0) {printf("WRONG CONTEXT at %s, line %d\n\r", __FILE__, __LINE__);	HALT();}
#else
#define CHECK_CONTEXT(value)
#endif

#if (KERNEL_MARKS)
#define CHECK_MAGIC(obj, magic_value, name)	if (obj->magic != magic_value) fatal_error(ERROR_GENERAL_INVALID_MAGIC, name)
#define DO_MAGIC(obj, magic_value)				obj->magic = magic_value
#define MAGIC											unsigned int magic

#else
#define CHECK_MAGIC(obj, magic_vale, name)
#define DO_MAGIC(obj, magic_value)
#define MAGIC
#endif


#if (CONSOLE_MODULE)

void console_init();

#endif //CONSOLE_MODULE

#endif // DBG_H
