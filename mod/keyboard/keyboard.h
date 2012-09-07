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

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "gpio.h"

#define KEY									unsigned int
#define KEY_PRESSED(key)				((key) & (1 << 16))
#define KEY_SET_PRESSED(key)			((key) | (1 << 16))
#define KEY_VALID(key)					((key) & (1 << 17))
#define KEY_SET_VALID(key)				((key) | (1 << 17))
#define KEY_CODE(key)					((key) & 0xffff)
#define KEY_ROW(key)						((key) & 0xff)
#define KEY_LINE(key)					(((key) >> 8) & 0xff)

#define KEYBOARD_FLAGS_ACTIVE_LO		(1 << 0)
#define KEYBOARD_FLAGS_PULL			(1 << 1)

typedef struct {
	const GPIO_CLASS* keys;
	unsigned int keys_count;
	unsigned int queue_size;
	unsigned int flags;
}KEYBOARD_CREATE_PARAMS;

HANDLE keyboard_create(KEYBOARD_CREATE_PARAMS* params, unsigned int priority);
void keyboard_destroy(HANDLE handle);
bool keyboard_is_pressed(HANDLE handle, KEY key);
bool keyboard_has_messages(HANDLE handle);
KEY keyboard_read(HANDLE handle, unsigned int timeout_ms);
bool keyboard_wait_for_key(HANDLE handle, unsigned int timeout_ms);

#endif // KEYBOARD_H
