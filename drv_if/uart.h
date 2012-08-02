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

#ifndef UART_H
#define UART_H

/*
		UART driver. Hardware-independent part
  */

#include "types.h"
#include "dev.h"

//UART line status
typedef enum {
	UART_ERROR_OK = 0,
	UART_ERROR_OVERRUN,
	UART_ERROR_NOISE,
	UART_ERROR_FRAME,
	UART_ERROR_PARITY
}UART_ERROR;

#define UART_MODE_RX_ENABLE				(1 << 0)
#define UART_MODE_TX_ENABLE				(1 << 1)
#define UART_MODE_TX_COMPLETE_ENABLE	(1 << 2)

typedef struct {
	//baudrate
	uint32_t baud;
	//data bits: 7, 8
	uint8_t data_bits;
	//parity: 'N', 'O', 'E'
	char parity;
	//stop bits: 1, 2
	uint8_t stop_bits;
}UART_BAUD;

typedef struct {
	void (*on_read_complete)(void* param);
	void (*on_write_complete)(void* param);
	void (*on_error)(void* param, UART_ERROR error);
}UART_CB, *P_UART_CB;

extern void uart_enable(UART_CLASS port, UART_CB* cb, void* param, int priority);
extern void uart_disable(UART_CLASS port);
extern void uart_set_baudrate(UART_CLASS port, const UART_BAUD* config);

extern void uart_write(UART_CLASS port, char* buf, int size);
extern void uart_write_wait(UART_CLASS port);
extern void uart_read(UART_CLASS port, char* buf, int size);
extern void uart_read_cancel(UART_CLASS port);

#endif // UART_H
