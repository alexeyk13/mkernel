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

/*
	LD-script for ARM with external memory support.
 */
//if you've got error on this line, than this file is not preprocessed

#include "memmap.h"

MEMORY
{
	FLASH (rx) : ORIGIN = FLASH_BASE, LENGTH = FLASH_SIZE
	RAM (rw)   : ORIGIN = EXT_RAM_BASE, LENGTH = EXT_RAM_SIZE
	INT_RAM (rw)   : ORIGIN = INT_RAM_BASE, LENGTH = INT_RAM_SIZE
}

SECTIONS
{
	.text :
   {
		KEEP(*(.reset));							/* Startup code */
		*(.text .text.* .gnu.linkonce.t.*)
		*(.glue_7t) *(.glue_7)
		*(.rodata .rodata.* .gnu.linkonce.r.*)
		. = ALIGN(4);
		_etext = .;
	} > FLASH

   /* .data section which is used for initialized data */
	.data : AT (_etext)
	{
		_data = .;
		*(.data .data.*);
		. = ALIGN(4);
		_edata = . ;
	} > RAM
	_etext_data = _etext + SIZEOF(.data);

	/* .bss section which is used for uninitialized data */
	.bss (NOLOAD) :
	{
		_bss = . ;
		*(.bss .bss.*);
		*(COMMON)
		. = ALIGN(4);
		_bss_end = . ;
	} > RAM

  _end = . ;

	/* .sys_data section which is used for initialized data, stored int internal SRAM */
	.sys_data : AT (_etext_data)
	{
		_sys_data = .;
		*(.sys_data .sys_data.*);
		. = ALIGN(4);
		_esys_data = . ;
	} > INT_RAM
	_etext_sys_data = _etext_data + SIZEOF(.sys_data);

	/* .sys_bss section which is used for uninitialized critical data */
	.sys_bss (NOLOAD) :
	{
		_sys_bss = . ;
		*(.sys_bss .sys_bss.*);
		. = ALIGN(4);
		_sys_bss_end = . ;
	} > INT_RAM

	_sys_end = . ;
}

