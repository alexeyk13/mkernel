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

//---------------------------------------------------------------------------- stm 32 F1 ----------------------------------------------------------------------------------------------------------
#if defined(STM32F100C4) || defined(STM32F100C6) || defined(STM32F100C8) || defined(STM32F100CB) \
 || defined(STM32F100R4) || defined(STM32F100R6) || defined(STM32F100R8) || defined(STM32F100RB) || defined(STM32F100RC) || defined(STM32F100RD) || defined(STM32F100RE) \
 || defined(STM32F100V8) || defined(STM32F100VB) || defined(STM32F100VC) || defined(STM32F100VD) || defined(STM32F100VE) \
 || defined(STM32F100ZC) || defined(STM32F100ZD) || defined(STM32F100ZE)
#define STM32F100
#endif

#if defined(STM32F101C4) || defined(STM32F101C6) || defined(STM32F101C8) || defined(STM32F101CB) \
 || defined(STM32F101R4) || defined(STM32F101R6) || defined(STM32F101R8) || defined(STM32F101RB) || defined(STM32F101RC) || defined(STM32F101RD) || defined(STM32F101RE) \
 || defined(STM32F101RF) || defined(STM32F101RG) \
 || defined(STM32F101T4) || defined(STM32F101T6) || defined(STM32F101T8) || defined(STM32F101TB) \
 || defined(STM32F101V8) || defined(STM32F101VB) || defined(STM32F101VC) || defined(STM32F101VD) || defined(STM32F101VE) || defined(STM32F101VF) || defined(STM32F101VG)  \
 || defined(STM32F101ZC) || defined(STM32F101ZD) || defined(STM32F101ZE) || defined(STM32F101ZG)
#define STM32F101
#endif

#if defined(STM32F102C4) || defined(STM32F102C6) || defined(STM32F102C8) || defined(STM32F102CB) \
 || defined(STM32F102R4) || defined(STM32F102R6) || defined(STM32F102R8) || defined(STM32F102RB)
#define STM32F102
#endif

#if defined(STM32F103C4) || defined(STM32F103C6) || defined(STM32F103C8) || defined(STM32F103CB) \
 || defined(STM32F103R4) || defined(STM32F103R6) || defined(STM32F103R8) || defined(STM32F103RB) || defined(STM32F103RC) || defined(STM32F103RD) || defined(STM32F103RE) \
 || defined(STM32F103RF) || defined(STM32F103RG) \
 || defined(STM32F103T4) || defined(STM32F103T6) || defined(STM32F103T8) || defined(STM32F103TB) \
 || defined(STM32F103V8) || defined(STM32F103VB) || defined(STM32F103VC) || defined(STM32F103VD) || defined(STM32F103VE) || defined(STM32F103VF) || defined(STM32F103VG)  \
 || defined(STM32F103ZC) || defined(STM32F103ZD) || defined(STM32F103ZE) || defined(STM32F103ZF) || defined(STM32F103ZG)
#define STM32F103
#endif

#if defined(STM32F105R8) || defined(STM32F105RB) || defined(STM32F105RC) \
 || defined(STM32F105V8) || defined(STM32F105VB) || defined(STM32F105VC)
#define STM32F105
#endif

#if defined(STM32F107RB) || defined(STM32F107RC) \
 || defined(STM32F107VB) || defined(STM32F107VC)
#define STM32F107
#endif

#if defined(STM32F100C4) || defined(STM32F100R4)
#define FLASH_SIZE		0x4000
#define STM32F10X_LD_VL
#endif //16k LD_VL

#if defined(STM32F101C4) || defined(STM32F101R4) || defined(STM32F101T4) || defined(STM32F102C4) || defined(STM32F102R4) || defined(STM32F103C4) || defined(STM32F103R4) || defined(STM32F103T4)
#define FLASH_SIZE		0x4000
#define STM32F10X_LD
#endif //16k LD

#if defined(STM32F100C6) || defined(STM32F100R6)
#define FLASH_SIZE		0x8000
#define STM32F10X_LD_VL
#endif //32k LD_VL

#if defined(STM32F101C6) || defined(STM32F101R6) || defined(STM32F101T6) || defined(STM32F102C6) || defined(STM32F102R6) || defined(STM32F103C6) || defined(STM32F103R6) || defined(STM32F103T6)
#define FLASH_SIZE		0x8000
#define STM32F10X_LD
#endif //32k LD

#if defined(STM32F100C8) || defined(STM32F100R8) || defined(STM32F100V8)
#define FLASH_SIZE		0x10000
#define STM32F10X_MD_VL
#endif //64k MD_VL

#if defined(STM32F101C8) || defined(STM32F101R8) || defined(STM32F101T8) || defined(STM32F101V8) || defined(STM32F102C8) || defined(STM32F102R8) || defined(STM32F103C8) || defined(STM32F103R8) \
 || defined(STM32F103T8) || defined(STM32F103V8)
#define FLASH_SIZE		0x10000
#define STM32F10X_MD
#endif //64k MD

#if defined(STM32F105R8) || defined(STM32F105V8)
#define FLASH_SIZE		0x10000
#define STM32F10X_CL
#endif //64k CL

#if defined(STM32F100CB) || defined(STM32F100RB) || defined(STM32F100VB)
#define FLASH_SIZE		0x20000
#define STM32F10X_MD_VL
#endif //128k MD_VL

#if defined(STM32F101CB) || defined(STM32F101RB) || defined(STM32F101TB) || defined(STM32F101VB) || defined(STM32F102CB) || defined(STM32F102RB) || defined(STM32F103CB) || defined(STM32F103RB) \
 || defined(STM32F103TB) || defined(STM32F103VB)
#define FLASH_SIZE		0x20000
#define STM32F10X_MD
#endif //128k MD

#if defined(STM32F105RB) || defined(STM32F105VB) || defined(STM32F107RB) || defined(STM32F107VB)
#define FLASH_SIZE		0x20000
#define STM32F10X_CL
#endif //128k CL

#if defined(STM32F100RC) || defined(STM32F100VC) || defined(STM32F100ZC)
#define FLASH_SIZE		0x40000
#define STM32F10X_HD_VL
#endif //256k HD_VL

#if defined(STM32F101RC) || defined(STM32F101VC) || defined(STM32F101ZC) || defined(STM32F103RC) || defined(STM32F103VC) || defined(STM32F103ZC)
#define FLASH_SIZE		0x40000
#define STM32F10X_HD
#endif //256k HD

#if defined(STM32F105RC) || defined(STM32F105VC) || defined(STM32F107RC) || defined(STM32F107VC)
#define FLASH_SIZE		0x40000
#define STM32F10X_CL
#endif //256k CL

#if defined(STM32F100RD) || defined(STM32F100VD) || defined(STM32F100ZD)
#define FLASH_SIZE		0x60000
#define STM32F10X_HD_VL
#endif //384k HD_VL

#if defined(STM32F101RD) || defined(STM32F101VD) || defined(STM32F101ZD) || defined(STM32F103RD) || defined(STM32F103VD) || defined(STM32F103ZD)
#define FLASH_SIZE		0x60000
#define STM32F10X_HD
#endif //384k HD

#if defined(STM32F100RE) || defined(STM32F100VE) || defined(STM32F100ZE)
#define FLASH_SIZE		0x80000
#define STM32F10X_HD_VL
#endif //512k HD_VL

#if defined(STM32F101RE) || defined(STM32F101VE) || defined(STM32F101ZE) || defined(STM32F103RE) || defined(STM32F103VE) || defined(STM32F103ZE)
#define FLASH_SIZE		0x80000
#define STM32F10X_HD
#endif //512k HD

#if defined(STM32F101RF) || defined(STM32F101VF) || defined(STM32F103RF) || defined(STM32F103VF) || defined(STM32F103ZF)
#define FLASH_SIZE		0xC0000
#define STM32F10X_XL
#endif //768k XL

#if defined(STM32F101RG) || defined(STM32F101VG) || defined(STM32F101ZG) || defined(STM32F103RG) || defined(STM32F103VG) || defined(STM32F103ZG)
#define FLASH_SIZE		0x100000
#define STM32F10X_XL
#endif //1m XL

#if defined(STM32F100)
#if defined(STM32F10X_LD_VL)
#define INT_RAM_SIZE		0x1000
#elif defined(STM32F10X_MD_VL)
#define INT_RAM_SIZE		0x2000
#elif (FLASH_SIZE == 0x40000)
#define INT_RAM_SIZE		0x6000
#elif defined(STM32F10X_HD_VL)
#define INT_RAM_SIZE		0x8000
#endif //STM32F100

#elif defined(STM32F101) || defined(STM32F102)
#if (FLASH_SIZE == 0x4000)
#define INT_RAM_SIZE		0x1000
#elif (FLASH_SIZE == 0x8000)
#define INT_RAM_SIZE		0x1800
#elif (FLASH_SIZE == 0x10000)
#define INT_RAM_SIZE		0x2800
#elif (FLASH_SIZE == 0x20000)
#define INT_RAM_SIZE		0x4000
#elif (FLASH_SIZE == 0x40000)
#define INT_RAM_SIZE		0x8000
#elif defined(STM32F10X_HD)
#define INT_RAM_SIZE		0xC000
#elif defined(STM32F10X_XL)
#define INT_RAM_SIZE		0x14000
#endif //STM32F101 || STM32F102

#elif defined(STM32F103)
#if (FLASH_SIZE == 0x4000)
#define INT_RAM_SIZE		0x1800
#elif (FLASH_SIZE == 0x8000)
#define INT_RAM_SIZE		0x2800
#elif defined(STM32F10X_MD)
#define INT_RAM_SIZE		0x5000
#elif (FLASH_SIZE == 0x40000)
#define INT_RAM_SIZE		0xC000
#elif defined(STM32F10X_HD)
#define INT_RAM_SIZE		0x10000
#elif defined(STM32F10X_XL)
#define INT_RAM_SIZE		0x18000
#endif //STM32F103

#elif defined(STM32F105) || defined(STM32F107)
#define INT_RAM_SIZE		0x10000

#endif  //INT_RAM_SIZE

#if defined(STM32F100) || defined(STM32F101) || defined(STM32F102) || defined(STM32F103) || defined(STM32F105) || defined(STM32F107)
#define STM32F1

//only gpio pin related
#define EXTI_LINES_COUNT					16

#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL) || defined(STM32F101T8) || defined(STM32F101TB) || defined(STM32F103T8) || defined(STM32F103TB)
#define UARTS_COUNT							2
#elif defined(STM32F10X_MD) || defined(STM32F10X_MD_VL)
#define UARTS_COUNT							3
#elif defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) || defined(STM32F10X_XL) || defined(STM32F10X_CL)
#define UARTS_COUNT							5
#endif //UARTS_COUNT

#if defined(STM32F10X_LD_VL)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3) || (1 << TIM_6) || (1 << TIM_7)  || (1 << TIM_15) || (1 << TIM_16) || (1 << TIM_17)
#define TIMERS_COUNT							8
#elif defined(STM32F10X_MD_VL)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)  || (1 << TIM_4) || (1 << TIM_6) || (1 << TIM_7)  || (1 << TIM_15) || (1 << TIM_16) || (1 << TIM_17)
#define TIMERS_COUNT							9
#elif defined(STM32F10X_HD_VL)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)  || (1 << TIM_4) || (1 << TIM_5) || (1 << TIM_6) || (1 << TIM_7) || (1 << TIM_12) || (1 << TIM_13) || (1 << TIM_14) \
						|| (1 << TIM_15) || (1 << TIM_16) || (1 << TIM_17)
#define TIMERS_COUNT							17
#elif defined(STM32F10X_LD)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)
#define TIMERS_COUNT							3
#elif defined(STM32F10X_MD)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)  || (1 << TIM_4)
#define TIMERS_COUNT							4
#elif defined(STM32F10X_HD)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)  || (1 << TIM_4) || (1 << TIM_5) || (1 << TIM_6) || (1 << TIM_7) || (1 << TIM_8)
#define TIMERS_COUNT							8
#elif defined(STM32F10X_CL)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)  || (1 << TIM_4) || (1 << TIM_5) || (1 << TIM_6) || (1 << TIM_7)
#define TIMERS_COUNT							7
#elif defined(STM32F10X_XL)
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)  || (1 << TIM_4) || (1 << TIM_5) || (1 << TIM_6) || (1 << TIM_7) || (1 << TIM_8) || (1 << TIM_9) || (1 << TIM_10) \
 || (1 << TIM_11) || (1 << TIM_12) || (1 << TIM_13) || (1 << TIM_14)
#define TIMERS_COUNT							14
#endif //TIMERS_MASK

#if defined(STM32F10X_LD) || defined(STM32F10X_LD_VL)
#define GPIO_PORTS_COUNT					4
#elif defined(STM32F10X_MD) || defined(STM32F10X_MD_VL) || defined(STM32F10X_CL_VL)
#define GPIO_PORTS_COUNT					5
#else
#define GPIO_PORTS_COUNT					7
#endif

#if defined(STM32F100)
#define MAX_CORE_FREQ						24000000
#define MAX_APB2_FREQ						24000000
#define MAX_APB1_FREQ						24000000

#elif defined(STM32F101)
#define MAX_CORE_FREQ						36000000
#define MAX_APB2_FREQ						36000000
#define MAX_APB1_FREQ						36000000

#elif defined(STM32F102)
#define MAX_CORE_FREQ						48000000
#define MAX_APB2_FREQ						48000000
#define MAX_APB1_FREQ						24000000

#elif defined(STM32F103) || defined(STM32F105) || defined(STM32F107)
#define MAX_CORE_FREQ						72000000
#define MAX_APB2_FREQ						72000000
#define MAX_APB1_FREQ						36000000
#endif//FREQ
#endif
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

#if defined(STM32F205) || defined(STM32F207) || defined(STM32F215) || defined(STM32F217)
#define STM32F2

#define GPIO_PORTS_COUNT					9

#define MAX_CORE_FREQ						120000000
#define MAX_APB2_FREQ						60000000
#define MAX_APB1_FREQ						30000000
#define MAX_FS_FREQ							48000000l

//only gpio pin related
#define EXTI_LINES_COUNT					16
#define TIMERS_MASK	(1 << TIM_1)  || (1 << TIM_2)  || (1 << TIM_3)  || (1 << TIM_4) || (1 << TIM_5) || (1 << TIM_6) || (1 << TIM_7) || (1 << TIM_8) || (1 << TIM_9) || (1 << TIM_10) \
 || (1 << TIM_11) || (1 << TIM_12) || (1 << TIM_13) || (1 << TIM_14)
#define TIMERS_COUNT							14
#define UARTS_COUNT							6
#define SDIO_COUNT							1
#define DMA_COUNT								2
#define USB_COUNT								1
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
