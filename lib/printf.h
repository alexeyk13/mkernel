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

#ifndef PRINTF_H
#define PRINTF_H

/** \addtogroup lib_printf embedded stdio
	embedded stdio routines contains standart ANSI c printf/sprintf analogue, with only
	difference that it's required around 1.5k of code,	doesn't use dynamic memory allocation
	and minimize system calls by block processing instead of char.

	Please note, that usage of printf/sprintf is required around 32 words of stack memory, not
	including space for context saving. Recommended value of stack for threads, used printf/sprintf
	must be minimum 60 words.

	Following format of arguments are supported:

	%[flags][width][.precision][length]specifier

	flags:\n
	\b \-			Left-justify within the given field width; Right justification is the default (see width sub-specifier).\n
	\b +\r\l			Forces to precede the result with a plus or minus sign (+ or -) even for positive numbers. By default, only negative numbers are preceded with a - sign.\n
	\b (space)		If no sign is going to be written, a blank space is inserted before the value.\n
	\b \#				Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X respectively for values different than zero.\n
	\b 0				Left-pads the number with zeroes (0) instead of spaces, where padding is specified (see width sub-specifier).\n

	width:\n
	\b (number)	Minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces.\n
				The value is not truncated even if the result is larger.
	\b *			The width is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.\n

	.precision\n
	\b .number	For integer specifiers (d, i, o, u, x, X): precision specifies the minimum number of digits to be written. If the value to be written is shorter than this number,
				the result is padded with leading zeros. The value is not truncated even if the result is longer. A precision of 0 means that no character is written for the value 0.
				For s: this is the maximum number of characters to be printed. By default all characters are printed until the ending null character is encountered.
				For c type: it has no effect.
				When no precision is specified, the default is 1. If the period is specified without an explicit value for precision, 0 is assumed.\n
	\b .*		The precision is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.\n


	length\n
	\b h		The argument is interpreted as a short int or unsigned short int (only applies to integer specifiers: i, d, o, u, x and X).\n
	\b l		The argument is interpreted as a long int or unsigned long int for integer specifiers (i, d, o, u, x and X)\n

	specifier\n
	\b c			Character\n
	\b d or \b i	Signed decimal integer\n
	\b o			Unsigned octal\n
	\b s			String of characters	sample\n
	\b u			Unsigned decimal integer\n
	\b x			Unsigned hexadecimal integer\n
	\b X			Unsigned hexadecimal integer (capital letters)\n
	\b %			A \b % followed by another \b % character will write \b % to stdout.
	\{
 */

#include <stdarg.h>
#include "types.h"

/**
	\brief handler for custom \ref format handler
	\param param: param to send on callback
	\param buf: data buf
	\param size: data buf size in bytes
	\retval none
*/
//write handler prototype for custom printf implementation
typedef void (*WRITE_HANDLER)(void* param, const char *const buf, unsigned int size);
/** \} */ // end of lib_printf group

unsigned long atou(char* buf, int size);
int utoa(char* buf, unsigned long value, int radix, bool uppercase);

void format(WRITE_HANDLER write_handler, void* write_param, char *fmt, va_list va);

void printf(char *fmt, ...);
void sprintf(char* str, char *fmt, ...);

//before calling printf, printf_handler must be declared
extern void printf_handler(void* param, const char *const buf, unsigned int size);

#endif // PRINTF_H
