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
																 "Invalid state"};
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

void fatal_error(ERROR_CODE ec, const char* name)
{
#ifdef KERNEL_DEBUG
	if (name)
		printf("%s: %s\n\r", name, ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
	else
		printf("%s\n\r", ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
#else //KERNEL_DEBUG
	if (name)
		printf("%s: fatal error 0x%x\n\r", name, ec);
	else
		printf("fatal error 0x%x\n\r", ec);
#endif //KERNEL_DEBUG
	dump(SYSTEM_POOL_BASE, 0x100);
	reset();
}

void fatal_error_address(ERROR_CODE ec, unsigned int address)
{
#ifdef KERNEL_DEBUG
	printf("%s at 0x%08x\n\r", ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE], address);
#else //KERNEL_DEBUG
	printf("fatal error 0x%x at 0x%08x\n\r", ec, address);
#endif //KERNEL_DEBUG
	dump(SYSTEM_POOL_BASE, 0x100);
	reset();
}

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
}

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
	if (get_context() != IRQ_CONTEXT)
		thread_exit();
}

void error_thread(ERROR_CODE ec)
{
#ifdef KERNEL_DEBUG
	printf("%s: %s\n\r", svc_thread_name(svc_thread_get_current()), ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
#else //KERNEL_DEBUG
	printf("%s: error 0x%x\n\r", svc_thread_name(svc_thread_get_current()), ec);
#endif //KERNEL_DEBUG
	if (get_context() != IRQ_CONTEXT)
		thread_exit();
}

void error_address(ERROR_CODE ec, unsigned int address)
{
#ifdef KERNEL_DEBUG
	printf("%s at 0x%08x\n\r", ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE], address);
#else //KERNEL_DEBUG
	printf("error 0x%x at 0x%08x\n\r", ec, address);
#endif //KERNEL_DEBUG
	if (get_context() != IRQ_CONTEXT)
		thread_exit();
}

void error_value(ERROR_CODE ec, unsigned int value)
{
#ifdef KERNEL_DEBUG
	printf("%s %d\n\r", ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE], value);
#else //KERNEL_DEBUG
	printf("error 0x%x: %d\n\r", ec, value);
#endif //KERNEL_DEBUG
	thread_exit();
}

void error_dev(ERROR_CODE ec, DEVICE_CLASS dev, int idx)
{
#ifdef KERNEL_DEBUG
	printf("%s_%d: %s_\n\r", DEV_NAMES[dev], idx, ERRORS[ec / ERROR_GROUP_SIZE][ec % ERROR_GROUP_SIZE]);
#else //KERNEL_DEBUG
	printf("device %d,%d error 0x%x\n\r", dev, idx, ec);
#endif //KERNEL_DEBUG
	if (get_context() != IRQ_CONTEXT)
		thread_exit();
}
