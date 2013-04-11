/*
	M-Kernel - embedded real-time RTOS
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

#ifndef SDIO_H
#define SDIO_H

#include "types.h"
#include "dev.h"

typedef enum {
	SDIO_NO_RESPONSE,
	SDIO_RESPONSE_R1,
	SDIO_RESPONSE_R1B,
	SDIO_RESPONSE_R2,
	SDIO_RESPONSE_R3,
	SDIO_RESPONSE_R4,
	SDIO_RESPONSE_R4B,
	SDIO_RESPONSE_R5,
	SDIO_RESPONSE_R6,
	SDIO_RESPONSE_R7
}SDIO_RESPONSE_TYPE;

typedef enum {
	SDIO_RESULT_OK,
	SDIO_RESULT_TIMEOUT,
	SDIO_RESULT_CRC_FAIL
}SDIO_RESULT;

typedef enum {
	SDIO_BUS_WIDE_1B = 0,
	SDIO_BUS_WIDE_4B,
	SDIO_BUS_WIDE_8B
}SDIO_BUS_WIDE;

typedef enum {
	SDIO_ERROR_TIMEOUT,
	SDIO_ERROR_CRC,
	SDIO_ERROR_OVERRUN,
	SDIO_ERROR_UNDERRUN
}SDIO_ERROR;

typedef void (*SDIO_HANDLER)(SDIO_CLASS, void*);
typedef void (*SDIO_ERROR_HANDLER)(SDIO_CLASS, void*, SDIO_ERROR);

typedef struct {
	SDIO_HANDLER on_read_complete;
	SDIO_HANDLER on_write_complete;
	SDIO_ERROR_HANDLER on_error;
}SDIO_CB, *P_SDIO_CB;

void sdio_enable(SDIO_CLASS port, SDIO_CB *cb, void* param, int priority);
void sdio_disable(SDIO_CLASS port);
void sdio_power_on(SDIO_CLASS port);
void sdio_power_off(SDIO_CLASS port);
void sdio_setup_bus(SDIO_CLASS port, uint32_t bus_freq, SDIO_BUS_WIDE bus_wide);

//because sdio_cmd max timeout is only 64 ahb, this calls are blocking
SDIO_RESULT sdio_cmd(SDIO_CLASS port, uint8_t cmd, uint32_t* sdio_cmd_response, uint32_t arg, SDIO_RESPONSE_TYPE response_type);
//async. isr will call completer on data transferred/error
void sdio_read(SDIO_CLASS port, char* buf, uint16_t block_size, uint32_t blocks_count);
void sdio_write(SDIO_CLASS port, char* buf, uint16_t block_size, uint32_t blocks_count);

#endif // SDIO_H
