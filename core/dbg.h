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

/** \addtogroup debug debug routines
	\{
	debug routines
 */

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


/**
	\brief halts system macro
	\details only works, if \ref KERNEL_DEBUG is set
	\retval no return
*/
#define HALT()											{dbg_push(); for (;;) {}}
/**
	\brief debug assertion
	\details only works, if \ref KERNEL_DEBUG is set.

	prints over debug console file name and line, caused assertion
	\param cond: assertion made if \b cond is \b false
	\retval no return if not \b cond, else none
*/
#define ASSERT(cond)									if (!(cond))	{printf("ASSERT at %s, line %d\n\r", __FILE__, __LINE__);	HALT();}

#else

#define HALT()
#define ASSERT(cond)

#endif

#if (KERNEL_CHECK_CONTEXT)
/**
	\brief context assertion
	\details only works, if \ref KERNEL_DEBUG and \ref KERNEL_CHECK_CONTEXT are set.

	prints over debug console file name and line, caused assertion
	\param value: \ref CONTEXT to check
	\retval no return if wrong context, else none
*/
#define CHECK_CONTEXT(value)						if ((get_context() & (value)) == 0) {printf("WRONG CONTEXT at %s, line %d\n\r", __FILE__, __LINE__);	HALT();}
#else
#define CHECK_CONTEXT(value)
#endif

#if (KERNEL_MARKS)
/**
	\brief check, if object mark is right (object is valid)
	\details only works, if \ref KERNEL_DEBUG and \ref KERNEL_MARKS are set.
	\param obj: object to check
	\param magic_value: value to set. check \ref magic.h for details
	\param name: object text to display in case of wrong magic
	\retval no return if wrong magic, else none
*/
#define CHECK_MAGIC(obj, magic_value, name)	if (obj->magic != magic_value) fatal_error(ERROR_GENERAL_INVALID_MAGIC, name)
/**
	\brief apply object magic on object creation
	\details only works, if \ref KERNEL_DEBUG and \ref KERNEL_MARKS are set.
	\param obj: object to check
	\param magic_value: value to set. check \ref magic.h for details
	\retval none
*/
#define DO_MAGIC(obj, magic_value)				obj->magic = magic_value
/**
	\brief this macro must be put in object structure
*/
#define MAGIC											unsigned int magic

#else
#define CHECK_MAGIC(obj, magic_vale, name)
#define DO_MAGIC(obj, magic_value)
#define MAGIC
#endif

/** \} */ // end of debug group

#if (CONSOLE_MODULE)

void console_init();

#endif //CONSOLE_MODULE

#endif // DBG_H
