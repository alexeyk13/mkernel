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

#ifndef CONSOLE_H
#define CONSOLE_H

#include "dev.h"
#include "event.h"
#include "mutex.h"
#include "rb.h"
#include "uart.h"

typedef struct {
	UART_CLASS port;
	unsigned int uart_mode;
	HANDLE rx_event;
	char* rx_buf;
	int size_max;
}CONSOLE_HEADER;

typedef struct {
	CONSOLE_HEADER h;
	RB tx_buf;
}CONSOLE;

CONSOLE* console_create(UART_CLASS port, int tx_size, int priority);
void console_destroy(CONSOLE* console);
void console_write(CONSOLE* console, char* buf, int size);
void console_writeln(CONSOLE* console, char* buf);
void console_putc(CONSOLE* console, char c);
void console_push(CONSOLE* console);
int console_read(CONSOLE* console, char* buf, int size_max);
int console_readln(CONSOLE* console, char* buf, int size_max);
char console_getc(CONSOLE* console);

#endif // CONSOLE_H
