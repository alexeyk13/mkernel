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

#ifndef KERNEL_CONFIG_H
#define KERNEL_CONFIG_H

//----------------------------------- kernel ------------------------------------------------------------------
//debug specific:
#define KERNEL_DEBUG								1
//marks objects with magic in headers
#define KERNEL_MARKS								1
//check range of dynamic objects and stacks. Only for kernel debug reasons
#define KERNEL_RANGE_CHECKING					1
//check context of system calls. Only for kernel debug reasons
#define KERNEL_CHECK_CONTEXT					1
//some kernel statistics
#define KERNEL_PROFILING						1

//sys_timer specific:
#define SYS_TIMER_RTC							RTC_0
#define SYS_TIMER_HPET							TIM_4
#define SYS_TIMER_PRIORITY						10
#define SYS_TIMER_CACHE_SIZE					16
#define SYS_TIMER_SOFT_RTC						1
#define SYS_TIMER_SOFT_RTC_TIMER				TIM_7

//thread specific
#define THREAD_CACHE_SIZE						16
#define THREAD_IDLE_STACK_SIZE				32

#define THREAD_STACK_SIZE						0x00002000
#define SVC_STACK_SIZE							256
#define SYSTEM_POOL_SIZE						(2 * 1024)
//--------------------------------- clock --------------------------------------------------------------------
//max core freq by default
#define STARTUP_CORE_FREQ						0

//------------------------- modules configure --------------------------------------------------------------
//think twice, before disabling
#define DBG_CONSOLE								1
#define WATCHDOG_MODULE							0

//------------------------------------ console --------------------------------------------------------------
#define DBG_CONSOLE_BAUD						115200
#define DBG_CONSOLE_UART						UART_1
//!13
#define DBG_CONSOLE_IRQ_PRIORITY				2
#define DBG_CONSOLE_TX_FIFO					32
//!!
#define DBG_CONSOLE_THREAD_PRIORITY			5
#define DBG_CONSOLE_THREAD_STACK_SIZE		64
//------------------------------------- I2C -----------------------------------------------------------------
#define I2C_DEBUG									1
//use with care. May halt transfers
#define I2C_DEBUG_TRANSFERS					0

//------------------------------------- USB -----------------------------------------------------------------
//change in case you have long string descriptors
#define USBD_CONTROL_BUF_SIZE					64

#define USB_DEBUG_SUCCESS_REQUESTS			0
#define USB_DEBUG_ERRORS						1
//only for hw driver debug
#define USB_DEBUG_FLOW							0
//#define USB_IRQ_PRIORITY						10
#define USB_IRQ_PRIORITY						0xff
//------------------------------------- USB MSC ------------------------------------------------------------
#define USB_MSC_THREAD_PRIORITY				10
#define USB_MSC_THREAD_STACK_SIZE			128

//must be at least 2 for double-buffering
#define USB_MSC_BUFFERS_IN_QUEUE				2
//more sectors in block, faster transfers
#define USB_MSC_SECTORS_IN_BLOCK				50

//only for hw driver debug
#define USB_MSC_DEBUG_FLOW						0
//---------------------------------- SCSI ------------------------------------------------------------------
#define SCSI_DEBUG_ERRORS						1
//debug all command completion
#define SCSI_DEBUG_FLOW							0
#define SCSI_DEBUG_READ_VERIFY				0
#define SCSI_DEBUG_WRITE						0
#define SCSI_DEBUG_UNSUPPORTED				0
//----------------------------------- SD_CARD ------------------------------------------------------------------
#define SD_CARD_CD_PIN							GPIO_A8
#define SD_CARD_DEBUG							0
#define SD_CARD_DEBUG_REGISTERS				0

#define SDIO_IRQ_PRIORITY						6

//----------------------------------- keyboard ----------------------------------------------------------------
#define KEYBOARD_DEBOUNCE_MS					10
#define KEYBOARD_POLL_MS						100
#define KEYBOARD_THREAD_PRIORITY				10

#endif // KERNEL_CONFIG_H
