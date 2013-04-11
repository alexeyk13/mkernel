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

#ifndef GPIO_STM32_H
#define GPIO_STM32_H

/*
	stm32fxxx-specific hardware configuration
 */

#include "dev.h"
#include "arch.h"
#include "gpio.h"

typedef enum {
#if defined(STM32F1)
	AFIO_MODE_ANALOG = 0x0
	AFIO_MODE_PUSH_PULL = 0xb,
	AFIO_MODE_OD = 0xf
#elif defined(STM32F2)
	AFIO_MODE_SYS = 0,
	AFIO_MODE_TIM1_2,
	AFIO_MODE_TIM3_4_5,
	AFIO_MODE_TIM8_9_10_11,
	AFIO_MODE_I2C,
	AFIO_MODE_SPI1_2_I2S2,
	AFIO_MODE_SPI3_I2S3,
	AFIO_MODE_USART1_2_3,
	AFIO_MODE_UART_4_5_USART_6,
	AFIO_MODE_CAN1_2_TIM12_13_14,
	AFIO_MODE_OTG,
	AFIO_MODE_ETH,
	AFIO_MODE_FSMC_SDIO_OTG_FS,
	AFIO_MODE_DCMI,
	AFIO_MODE_14,
	AFIO_MODE_EVENTOUT,
	AFIO_MODE_ANALOG = 0x10,
	AFIO_MODE_FSMC_SDIO_OTG_FS_PULL_UP = 0x1c
#endif
}AFIO_MODE;

typedef GPIO_TypeDef* GPIO_TypeDef_P;
extern const GPIO_TypeDef_P GPIO[];

//stm32 specific, for internal use for drivers
void gpio_enable_afio(GPIO_CLASS pin, AFIO_MODE mode);
void gpio_disable_jtag();

#if defined(STM32F1)
void afio_remap();
void afio_unmap();
#endif

#endif // GPIO_STM32_H
