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

#include "error.h"
#include "dbg.h"
#include "irq.h"
#include "rcc.h"
#include "memmap.h"
#include "thread.h"
#include "thread_private.h"

#ifdef KERNEL_DEBUG
const char* const GENERAL_ERRORS[] =				{"Abstract general error",
																 "Wrong context",
																 "Undefined instruction",
																 "Invalid sys call",
																 "Object not found",
																 "Invalid magic",
																 "Vector table read fault",
																 "Hard fault",
																 "Stacking failed",
																 "Unstacking failed",
																 "Data bus error",
																 "Instruction bus error",
																 "Division by zero",
																 "Unaligned access",
																 "No coprocessor found",
																 "Invalid state",
																 "SYS call, while interrupts are disabled"};
const char* const MEM_ERRORS[] =						{"Abstract memory error",
																 "Pointer outside of memory pool",
																 "Range check failed",
																 "Out of system memory",
																 "Out of stack memory",
																 "Out of heap",
																 "Data access violation",
																 "Instruction access violation"};

const char* const DEV_ERRORS[] =						{"Abstract device error",
																 "Device index out of range",
																 "Device not present",
																 "Device failed to start",
																 "Device is not active"};

const char* const THREAD_ERRORS[] =					{"Abstract thread error",
																 "Thread out of context",
																 "Invalid call in IDLE thread"};

const char* const SYNC_ERRORS[] =					{"Abstract sync objec error",
																 "Wrong unlocker for sync object",
																 "Sync object already owned by caller",
																 "Sync object already unlocked"};

const char *const *const ERRORS[] =					{GENERAL_ERRORS, MEM_ERRORS, DEV_ERRORS, THREAD_ERRORS, SYNC_ERRORS};

const char* const DEV_NAMES[] =						{"SYS", "FLASH", "TIMER", "UART", "SPI", "GPIO", "VIDEO", "ADC", "DAC", "CAN", "I2C", "USB", "ETH", "SDIO", "WDT", "RTC"};

#endif //KERNEL_DEBUG

/** \addtogroup error error handling
	M-Kernel has embedded error management

	Generally, error handling is divided on 2 types:
		- non-fatal error handling
		- fatal error handling

	non-fatal is for USER/SYSTEM context, fatal: for IRQ/SVC context

	on non fatal error handler:
		- displays error message with current thread name
		- kill current running thread

	on fatal error:
		- display error message with FATAL prefix
		- made minidump of system memory pool
		- push debug info over console
		- if \ref KERNEL_HALT_ON_FATAL_ERROR is set, system will halt, otherwise,
		reset sequence is initiated

	in case of \ref KERNEL_DEBUG is set full error text is displayed, otherwise
	debug info is stripped and only error code is displayed. Anyway, error handling is
	working in both debug and release cases.

	list of actual error codes can be found in error.h
	\{
 */

/**
	\brief fatal error with custom text
	\param ec: error code
	\param name: custom text to display
	\retval no return
*/
void fatal_error(ERROR_CODE ec, const char* name)
{
#ifdef KERNEL_DEBUG
	if (name)
		printf("FATAL: %s: %s\n\r", name, ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
	else
		printf("FATAL: %s\n\r", ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
#else //KERNEL_DEBUG
	if (name)
		printf("FATAL: %s: fatal error 0x%x\n\r", name, ec);
	else
		printf("FATAL: error 0x%x\n\r", ec);
#endif //KERNEL_DEBUG
	dump(SYSTEM_POOL_BASE, 0x100);
	dbg_push();
#if (KERNEL_HALT_ON_FATAL_ERROR)
	HALT();
#else
	reset();
#endif //KERNEL_HALT_ON_FATAL_ERROR
}

/**
	\brief fatal error on specific address
	\param ec: error code
	\param address: custom address to display
	\retval no return
*/
void fatal_error_address(ERROR_CODE ec, unsigned int address)
{
#ifdef KERNEL_DEBUG
	printf("FATAL: %s at 0x%08x\n\r", ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE], address);
#else //KERNEL_DEBUG
	printf("FATAL: error 0x%x at 0x%08x\n\r", ec, address);
#endif //KERNEL_DEBUG
	dump(SYSTEM_POOL_BASE, 0x100);
	dbg_push();
#if (KERNEL_HALT_ON_FATAL_ERROR)
	HALT();
#else
	reset();
#endif //KERNEL_HALT_ON_FATAL_ERROR
}

/** \} */ // end of error group

/** \addtogroup debug debug routines
	\{
 */

/**
	\brief minidump
	\param addr: starting address
	\param size: size in bytes
	\retval none
*/
void dump(unsigned int addr, unsigned int size)
{
	printf("memory dump 0x%08x-0x%08x\n\r", addr, addr + size);
	unsigned int i = 0;
	for (i = 0; i < size; ++i)
	{
		if ((i % 0x10) == 0)
			printf("0x%08x: ", addr + i);
		printf("%02X ", ((unsigned char*)addr)[i]);
		if ((i % 0x10) == 0xf)
			printf("\n\r");
	}
	if (size % 0x10)
		printf("\n\r");
}

/** \} */ // end of debug group

/** \addtogroup error error handling
	\{
 */

/**
	\brief error with custom text
	\param ec: error code
	\param name: custom text to display
	\retval none
*/
void error(ERROR_CODE ec, const char* name)
{
#ifdef KERNEL_DEBUG
	if (name)
		printf("%s: %s\n\r", name, ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
	else
		printf("%s\n\r", ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
#else //KERNEL_DEBUG
	if (name)
		printf("%s: error 0x%x\n\r", name, ec);
	else
		printf("error 0x%x\n\r", ec);
#endif //KERNEL_DEBUG
	if (get_context() & (IRQ_CONTEXT | SUPERVISOR_CONTEXT))
		svc_thread_destroy_current();
	else
		thread_exit();
}

/**
	\brief thread error
	\param ec: error code
	\retval none
*/
void error_thread(ERROR_CODE ec)
{
#ifdef KERNEL_DEBUG
	printf("%s: %s\n\r", svc_thread_name(svc_thread_get_current()), ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
#else //KERNEL_DEBUG
	printf("%s: error %#x\n\r", svc_thread_name(svc_thread_get_current()), ec);
#endif //KERNEL_DEBUG
	if (get_context() & (IRQ_CONTEXT | SUPERVISOR_CONTEXT))
		svc_thread_destroy_current();
	else
		thread_exit();
}

/**
	\brief thread error on custom address
	\param ec: error code
	\param address: custom address
	\retval none
*/
void error_address(ERROR_CODE ec, unsigned int address)
{
#ifdef KERNEL_DEBUG
	printf("%s: %s at %#.08x\n\r", svc_thread_name(svc_thread_get_current()), ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE], address);
#else //KERNEL_DEBUG
	printf("%s: error %#x at %#.08x\n\r", svc_thread_name(svc_thread_get_current()), ec, address);
#endif //KERNEL_DEBUG
	if (get_context() & (IRQ_CONTEXT | SUPERVISOR_CONTEXT))
		svc_thread_destroy_current();
	else
		thread_exit();
}

/**
	\brief thread error with custom error value
	\param ec: error code
	\param value: custom error value
	\retval none
*/
void error_value(ERROR_CODE ec, unsigned int value)
{
#ifdef KERNEL_DEBUG
	printf("%s: %s %d\n\r", svc_thread_name(svc_thread_get_current()), ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE], value);
#else //KERNEL_DEBUG
	printf("%s: error %#x %d\n\r", svc_thread_name(svc_thread_get_current()), ec, value);
#endif //KERNEL_DEBUG
	if (get_context() & (IRQ_CONTEXT | SUPERVISOR_CONTEXT))
		svc_thread_destroy_current();
	else
		thread_exit();
}

/**
	\brief device error
	\param ec: error code
	\param dev: device class. Name is only printed when \ref KERNEL_DEBUG is set
	\param idx: device index
	\retval none
*/
void error_dev(ERROR_CODE ec, DEVICE_CLASS dev, int idx)
{
#ifdef KERNEL_DEBUG
	printf("%s_%d: %s_\n\r", DEV_NAMES[dev], idx, ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
#else //KERNEL_DEBUG
	printf("device %d,%d error %#x\n\r", dev, idx, ec);
#endif //KERNEL_DEBUG
	if (get_context() & (IRQ_CONTEXT | SUPERVISOR_CONTEXT))
		svc_thread_destroy_current();
	else
		thread_exit();
}

/** \} */ // end of error group
