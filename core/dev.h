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

#ifndef DEV_H
#define DEV_H

/*! \mainpage Main index
 \tableofcontents
 M-Kernel documentation

	- \ref about
	- \ref gettingstarted
	- core functionality
		- \ref thread
		- \ref mutex
		- \ref event
		- \ref semaphore
		- \ref data_queue
		- \ref message_queue
	- \ref memory
	- library functions
		- \ref lib_time
		- \ref lib_printf
		- \ref lib_slist
		- \ref lib_dlist
		- \ref lib_rb
		- \ref lib_rb_block
	- debug and error handling
		- \ref error
		- \ref debug
		- \ref profiling
*/

/*
		dev.h:
		- device classes abstraction
		- device resource allocation abstraction
  */

typedef enum {
	DEV_SYS = 0,
	DEV_FLASH,
	DEV_TIMER,
	DEV_UART,
	DEV_SPI,
	DEV_GPIO,
	DEV_VIDEO,
	DEV_ADC,
	DEV_DAC,
	DEV_CAN,
	DEV_I2C,
	DEV_USB,
	DEV_ETH,
	DEV_SDIO,
	DEV_WDT,
	DEV_RTC
}DEVICE_CLASS;

typedef enum {
	DMA_1 = 0,
	DMA_2,
	DMA_3,
	DMA_4
}DMA_CLASS;

typedef enum {
	DMA_STREAM_0 = 0,
	DMA_STREAM_1,
	DMA_STREAM_2,
	DMA_STREAM_3,
	DMA_STREAM_4,
	DMA_STREAM_5,
	DMA_STREAM_6,
	DMA_STREAM_7,
	DMA_STREAM_8,
	DMA_STREAM_9,
	DMA_STREAM_10
}DMA_STREAM_CLASS;

typedef enum {
	SPI_1 = 0,
	SPI_2,
	SPI_3
}SPI_CLASS;

typedef enum {
	I2C_1 = 0,
	I2C_2,
	I2C_3
}I2C_CLASS;

typedef enum {
	USB_1 = 0,
	USB_2,
	USB_3,
	USB_4
}USB_CLASS;

typedef enum {
	SDIO_1 = 0,
	SDIO_2,
	SDIO_3
}SDIO_CLASS;

typedef enum {
	UART_1 = 0,
	UART_2,
	UART_3,
	UART_4,
	UART_5,
	UART_6,
	UART_7
}UART_CLASS;

typedef enum {
	ADC_1 = 0,
	ADC_2,
	ADC_3
}ADC_CLASS;

/*timer remap for stm32f10x:
	LD,MD,HD,CL		: no remap
	LD_VL	: 6->4, 7->5, 15->6, 16->7, 17->8
	MD_VL	: 6->5, 7->6, 15->7, 16->8, 17->9
	XL		: 8->5, 9->6, 10->7, 11->8, 12->9, 13->10, 14->11
	for stm32f10x: TIM1..TIM7		-> no remapping
						TIM12....TIM17 -> TIM8..TIM13
*/

typedef enum {
	TIM_1 = 0,
	TIM_2,
	TIM_3,
	TIM_4,
	TIM_5,
	TIM_6,
	TIM_7,
	TIM_8,
	TIM_9,
	TIM_10,
	TIM_11,
	TIM_12,
	TIM_13,
	TIM_14,
	TIM_15,
	TIM_16,
	TIM_17,
	TIM_18,
	TIM_19,
	TIM_20
}TIMER_CLASS;

typedef enum {
	WDT_0,
	WDT_1
}WDT_CLASS;

typedef enum {
	RTC_0,
	RTC_1
}RTC_CLASS;

#endif // DEV_H
