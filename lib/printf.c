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

#include "printf.h"
#include <string.h>
#include "dbg_console.h"
#include "dbg.h"

#define PRINTF_BUF_SIZE												10

#define FLAGS_PROCESSING											(1 << 0)

#define FLAGS_LEFT_JUSTIFY											(1 << 1)
#define FLAGS_FORCE_PLUS											(1 << 2)
#define FLAGS_SPACE_FOR_SIGN										(1 << 3)
#define FLAGS_RADIX_PREFIX											(1 << 5)
#define FLAGS_ZERO_PAD_NUMBERS									(1 << 6)
#define FLAGS_SHORT_INT												(1 << 7)

#define FLAGS_SIGN_MINUS											(1 << 8)

const char spaces[PRINTF_BUF_SIZE] =							"          ";
const char zeroes[PRINTF_BUF_SIZE] =							"0000000000";

void sprintf_handler(void* param, const char *const buf, unsigned int size)
{
	char** str = (char**)param;
	memcpy(*str, buf, size);
	*str += size;
}

unsigned long atou(char* buf, int size)
{
	int i;
	unsigned long res = 0;
	for (i = 0; i < size && buf[i] >= '0' && buf[i] <= '9'; ++i)
		res = res * 10 + buf[i] - '0';
	return res;
}

int utoa(char* buf, unsigned long value, int radix, bool uppercase)
{
	int size = 0;
	char c;
	while (value)
	{
		c = (char)(value % radix);
		if (c > 9)
			c += (uppercase ? 'A' : 'a') - 10;
		else
			c += '0';
		buf[PRINTF_BUF_SIZE - size++] = c;
		value /= radix;
	}
	memmove(buf, buf + PRINTF_BUF_SIZE - size + 1, size);
	return size;
}

void pad_spaces(WRITE_HANDLER write_handler, void *write_param, int count)
{
	while (count > PRINTF_BUF_SIZE)
	{
		write_handler(write_param, spaces, PRINTF_BUF_SIZE);
		count -= PRINTF_BUF_SIZE;
	}
	if (count)
		write_handler(write_param, spaces, count);
}

static inline void pad_zeroes(WRITE_HANDLER write_handler, void *write_param, int count)
{
	while (count > PRINTF_BUF_SIZE)
	{
		write_handler(write_param, zeroes, PRINTF_BUF_SIZE);
		count -= PRINTF_BUF_SIZE;
	}
	if (count)
		write_handler(write_param, zeroes, count);
}

void format(WRITE_HANDLER write_handler, void *write_param, char *fmt, va_list va)
{
	char buf[PRINTF_BUF_SIZE];
	unsigned char flags;
	unsigned int start = 0;
	unsigned int cur = 0;
	unsigned int buf_size = 0;
	unsigned int width, precision;
	char* str = NULL;
	unsigned long u;
	long d;
	char c;
	while (fmt[cur])
	{
		if (fmt[cur] == '%')
		{
			//write plain block
			if (fmt[cur + 1] == '%')
			{
				++cur;
				write_handler(write_param, fmt + start, cur - start);
				++cur;
				start = cur;
			}
			else
			{
				if (cur > start)
					write_handler(write_param, fmt + start, cur - start);
				++cur;
				//1. decode flags
				flags = FLAGS_PROCESSING;
				while (fmt[cur] && (flags & FLAGS_PROCESSING))
				{
					switch (fmt[cur])
					{
					case '-':
						flags |= FLAGS_LEFT_JUSTIFY;
						++cur;
						break;
					case '+':
						flags |= FLAGS_FORCE_PLUS;
						++cur;
						break;
					case ' ':
						flags |= FLAGS_SPACE_FOR_SIGN;
						++cur;
						break;
					case '#':
						flags |= FLAGS_RADIX_PREFIX;
						++cur;
						break;
					case '0':
						flags |= FLAGS_ZERO_PAD_NUMBERS;
						++cur;
						break;
					default:
						flags &= ~FLAGS_PROCESSING;
					}
				}
				//2. width
				width = 0;
				if (fmt[cur] == '*')
				{
					width = va_arg(va, int);
					++cur;
				}
				else
				{
					start = cur;
					while (fmt[cur] >= '0' && fmt[cur] <= '9')
						++cur;
					if (cur > start)
						width = atou(fmt + start, cur - start);
				}
				//3. precision
				precision = 1;
				if (fmt[cur] == '.')
				{
					++cur;
					if (fmt[cur] == '*')
					{
						precision = va_arg(va, int);
						++cur;
					}
					else
					{
						start = cur;
						while (fmt[cur] >= '0' && fmt[cur] <= '9')
							++cur;
						if (cur > start)
							precision = atou(fmt + start, cur - start);
					}
				}
				//4. int length
				switch (fmt[cur])
				{
				case 'h':
					flags |= FLAGS_SHORT_INT;
					++cur;
					break;
				case 'l':
					++cur;
					break;
				}
				//5. specifier
				//a) calculate size and format
				d = 0;
				buf_size = 0;
				switch (fmt[cur])
				{
				case 'c':
					c = (char)va_arg(va, int);
					d = 1;
					break;
				case 's':
					str = va_arg(va, char*);
					d = strlen(str);
					if (precision > 1 && precision < d)
						d = precision;
					break;
				case 'i':
				case 'd':
					if ((flags & FLAGS_ZERO_PAD_NUMBERS) && (precision < width))
						precision = width;
					u = va_arg(va, unsigned long);
					if (flags & FLAGS_SHORT_INT)
						d = (short)u;
					else
						d = (long)u;
					if (d < 0)
					{
						buf_size = utoa(buf, -d, 10, false);
						d = buf_size + 1;
						flags |= FLAGS_SIGN_MINUS;
					}
					else if (d > 0)
					{
						buf_size = utoa(buf, d, 10, false);
						d = buf_size;
						if (flags & (FLAGS_FORCE_PLUS | FLAGS_SPACE_FOR_SIGN))
							++d;
					}
					if (buf_size < precision)
						d += precision - buf_size;
					break;
				case 'u':
					if ((flags & FLAGS_ZERO_PAD_NUMBERS) && (precision < width))
						precision = width;
					u = va_arg(va, unsigned long);
					if (flags & FLAGS_SHORT_INT)
						u = (unsigned short)u;
					if (u > 0)
					{
						buf_size = utoa(buf, u, 10, false);
						d = buf_size;
					}
					if (buf_size < precision)
						d += precision - buf_size;
					break;
				case 'x':
				case 'X':
					if ((flags & FLAGS_ZERO_PAD_NUMBERS) && (precision < width))
						precision = width;
					u = va_arg(va, unsigned long);
					if (flags & FLAGS_SHORT_INT)
						u = (unsigned short)u;
					if (u > 0)
					{
						buf_size = utoa(buf, u, 16, fmt[cur] == 'X');
						d = buf_size;
						if (flags & (FLAGS_RADIX_PREFIX))
							d += 2;
					}
					if (buf_size < precision)
						d += precision - buf_size;
					break;
				case 'o':
					if ((flags & FLAGS_ZERO_PAD_NUMBERS) && (precision < width))
						precision = width;
					u = va_arg(va, unsigned long);
					if (flags & FLAGS_SHORT_INT)
						u = (unsigned short)u;
					if (u > 0)
					{
						buf_size = utoa(buf, u, 8, false);
						d = buf_size;
						if (flags & (FLAGS_RADIX_PREFIX))
							d += 1;
					}
					if (buf_size < precision)
						d += precision - buf_size;
					break;
				}

				//right justify
				if ((flags & FLAGS_LEFT_JUSTIFY) == 0)
					pad_spaces(write_handler, write_param, width - d);

				//b) output
				switch (fmt[cur++])
				{
				case 'c':
					write_handler(write_param, &c, 1);
					break;
				case 's':
					write_handler(write_param, str, d);
					break;
				case 'i':
				case 'd':
				case 'u':
					//sign processing
					if (flags & FLAGS_SIGN_MINUS)
						write_handler(write_param, "-", 1);
					else if (buf_size)
					{
						if (flags & FLAGS_FORCE_PLUS)
							write_handler(write_param, "+", 1);
						else if (flags & FLAGS_SPACE_FOR_SIGN)
							write_handler(write_param, " ", 1);
					}
					//zero padding
					if (buf_size < precision)
						pad_zeroes(write_handler, write_param, precision - buf_size);
					//data
					write_handler(write_param, buf, buf_size);
					break;
				case 'x':
				case 'X':
					if (buf_size && (flags & FLAGS_RADIX_PREFIX))
						write_handler(write_param, "0x", 2);
					//zero padding
					if (buf_size < precision)
						pad_zeroes(write_handler, write_param, precision - buf_size);
					//data
					write_handler(write_param, buf, buf_size);
					break;
				case 'o':
					if (buf_size && (flags & FLAGS_RADIX_PREFIX))
						write_handler(write_param, "O", 1);
					//zero padding
					if (buf_size < precision)
						pad_zeroes(write_handler, write_param, precision - buf_size);
					//data
					write_handler(write_param, buf, buf_size);
					break;
				}

				//left justify
				if (flags & FLAGS_LEFT_JUSTIFY)
					pad_spaces(write_handler, write_param, width - d);

				start = cur;
			}
		}
		else
			++cur;
	}
	if (cur > start)
		write_handler(write_param, fmt + start, cur - start);
}

void printf(char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	format(printf_handler, NULL, fmt, va);
	va_end(va);
}

void sprintf(char* str, char *fmt, ...)
{
	va_list va;
	va_start(va, fmt);
	char* str_cur = str;
	format(sprintf_handler, &str_cur, fmt, va);
	*str_cur = 0;
	va_end(va);
}
