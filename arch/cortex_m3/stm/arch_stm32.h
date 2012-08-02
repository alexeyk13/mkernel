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

#ifndef ARCH_STM32_H
#define ARCH_STM32_H

//---------------------------------------------------------------------------- stm 32 F2 ----------------------------------------------------------------------------------------------------------
#if defined(STM32F205RB) || defined(STM32F205RC) || defined(STM32F205RE) || defined(STM32F205RF) || defined(STM32F205RG) || defined(STM32F205VG) || defined(STM32F205ZG) || defined(STM32F205VF) \
 || defined(STM32F205ZF) || defined(STM32F205VE) || defined(STM32F205ZE) || defined(STM32F205VC) || defined(STM32F205ZC) || defined(STM32F205VB)

#define STM32F205
#endif

#if defined(STM32F207VC) || defined(STM32F207VE) || defined(STM32F207VF) || defined(STM32F207VG) || defined(STM32F207ZC) || defined(STM32F207ZE) || defined(STM32F207ZF) || defined(STM32F207ZG) \
	|| defined(STM32F207IC) || defined(STM32F207IE) || defined(STM32F207IF) || defined(STM32F207IG)
#define STM32F207
#endif

#if defined(STM32F215RE) || defined(STM32F215RG) || defined(STM32F215VG) || defined(STM32F215ZG) || defined(STM32F215VE) || defined(STM32F215ZE)
#define STM32F215
#endif

#if defined(STM32F217VE) || defined(STM32F217VG) || defined(STM32F217ZE) || defined(STM32F217ZG) || defined(STM32F217IE) || defined(STM32F217IG)
#define STM32F217
#endif

#if defined(STM32F205) || defined(STM32F207) || defined(STM32F215) || defined(STM32F217)
#define STM32F2
#endif

#if defined(STM32F205RB) || defined(STM32F205VB)
#define FLASH_SIZE		0x20000
#endif //128K

#if defined(STM32F205RC) || defined(STM32F205VC) || defined(STM32F205ZC) || defined(STM32F207VC) || defined(STM32F207ZC) || defined(STM32F207IC)
#define FLASH_SIZE		0x40000
#endif //256K

#if defined(STM32F205RE) || defined(STM32F205VE) || defined(STM32F205ZE) || defined(STM32F215RE) || defined(STM32F215VE) || defined(STM32F215ZE) || defined(STM32F207VE) || defined(STM32F207ZE) \
	|| defined(STM32F207IE)  || defined(STM32F217VE) || defined(STM32F217ZE) || defined(STM32F207IE)
#define FLASH_SIZE		0x80000
#endif //512K

#if defined(STM32F205RF) || defined(STM32F205VF) || defined(STM32F205ZF) || defined(STM32F207VF) || defined(STM32F207ZF) || defined(STM32F207IF)
#define FLASH_SIZE		0xc0000
#endif //768K

#if defined(STM32F205RG) || defined(STM32F205VG) || defined(STM32F205ZG) || defined(STM32F215RG) || defined(STM32F215VG) || defined(STM32F215ZG) || defined(STM32F207VG) || defined(STM32F207ZG) \
	|| defined(STM32F207IG)  || defined(STM32F217VG) || defined(STM32F217ZG) || defined(STM32F207IG)
#define FLASH_SIZE		0x100000
#endif //1M

#if defined(STM32F215) || defined(STM32F207) || defined(STM32F217) || (FLASH_SIZE > 0x40000)
#define INT_RAM_SIZE		0x20000
#elif (FLASH_SIZE==0x20000)
#define INT_RAM_SIZE		0x10000
#elif (FLASH_SIZE==0x40000)
#define INT_RAM_SIZE		0x18000
#endif
//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if defined(STM32F1) || defined(STM32F2) || defined(STM32F4)
#define CORTEX_M3
#define STM

#ifndef __ASSEMBLER__

#if defined(STM32F1)
#define STM32F10x
#include "stm32f10x.h"
#elif defined(STM32F2)
#define STM32F2xx
#include "stm32f2xx.h"
#elif defined(STM32F4)
#define STM32F4xx
#include "stm32f4xx.h"
#endif

#define INT_RAM_BASE				SRAM_BASE

#else

#define FLASH_BASE				0x08000000
#define INT_RAM_BASE				0x20000000

#endif //__ASSEMBLER__

#endif //STM32F1 || STM32F2 || STM32F4

#endif //ARCH_STM32_H
