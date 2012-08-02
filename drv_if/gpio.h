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

#ifndef GPIO_H
#define GPIO_H

#include "types.h"
#include "dev.h"
#include "arch.h"

#ifdef CUSTOM_GPIO
#include "gpio_custom.h"
#else
#include "gpio_default.h"
#endif //GPIO_CUSTOM

typedef enum {
	//output, max speed
	PIN_MODE_OUT = 0,
	//input float. external resistor must be provided
	PIN_MODE_IN,
	//out open drain
	PIN_MODE_OUT_OD,
	//input with integrated pull-up/pull-down
	PIN_MODE_IN_PULLUP,
	PIN_MODE_IN_PULLDOWN
} PIN_MODE;

typedef enum {
	EXTI_MODE_RISING = 1,
	EXTI_MODE_FALLING  = 2,
	EXTI_MODE_BOTH = 3
} EXTI_MODE;

typedef void (*EXTI_HANDLER)(EXTI_CLASS);

extern void gpio_enable_pin(GPIO_CLASS pin, PIN_MODE mode);
extern void gpio_disable_pin(GPIO_CLASS pin);
extern void gpio_set_pin(GPIO_CLASS pin, bool set);
extern bool gpio_get_pin(GPIO_CLASS pin);
extern bool gpio_get_out_pin(GPIO_CLASS pin);

extern void gpio_exti_enable(EXTI_CLASS exti, EXTI_MODE mode, EXTI_HANDLER callback, int priority);
extern void gpio_exti_disable(EXTI_CLASS exti);

#endif //GPIO_H
