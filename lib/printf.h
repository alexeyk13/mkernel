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

/*
The format tags follow this prototype:

%[flags][width][.precision][length]specifier

flags:
-			Left-justify within the given field width; Right justification is the default (see width sub-specifier).
+			Forces to precede the result with a plus or minus sign (+ or -) even for positive numbers. By default, only negative numbers are preceded with a - sign.
(space)	If no sign is going to be written, a blank space is inserted before the value.
#			Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X respectively for values different than zero.
0			Left-pads the number with zeroes (0) instead of spaces, where padding is specified (see width sub-specifier).

width:
(number)	Minimum number of characters to be printed. If the value to be printed is shorter than this number, the result is padded with blank spaces.
			The value is not truncated even if the result is larger.
*			The width is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.

.precision
.number	For integer specifiers (d, i, o, u, x, X): precision specifies the minimum number of digits to be written. If the value to be written is shorter than this number,
			the result is padded with leading zeros. The value is not truncated even if the result is longer. A precision of 0 means that no character is written for the value 0.
			For s: this is the maximum number of characters to be printed. By default all characters are printed until the ending null character is encountered.
			For c type: it has no effect.
			When no precision is specified, the default is 1. If the period is specified without an explicit value for precision, 0 is assumed.
.*			The precision is not specified in the format string, but as an additional integer value argument preceding the argument that has to be formatted.


length
h			The argument is interpreted as a short int or unsigned short int (only applies to integer specifiers: i, d, o, u, x and X).
l			The argument is interpreted as a long int or unsigned long int for integer specifiers (i, d, o, u, x and X)

specifier
c			Character
d or i	Signed decimal integer
o			Unsigned octal
s			String of characters	sample
u			Unsigned decimal integer
x			Unsigned hexadecimal integer
X			Unsigned hexadecimal integer (capital letters)
%			A % followed by another % character will write % to stdout.	%

*/

#include <stdarg.h>
#include "types.h"

//write handler prototype for custom printf implementation
typedef void (*WRITE_HANDLER)(void* param, const char *const buf, unsigned int size);

unsigned long atou(char* buf, int size);
int utoa(char* buf, unsigned long value, int radix, bool uppercase);

void format(WRITE_HANDLER write_handler, void* write_param, char *fmt, va_list va);

void printf(char *fmt, ...);
void sprintf(char* str, char *fmt, ...);

//before calling printf, printf_handler must be declared
extern void printf_handler(void* param, const char *const buf, unsigned int size);

#endif // PRINTF_H
