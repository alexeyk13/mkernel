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

#ifndef CC_MACRO_H
#define CC_MACRO_H

#if   defined ( __CC_ARM )
	#define __ASM            __asm                                      /*!< asm keyword for ARM Compiler          */
	#define __INLINE         __inline                                   /*!< inline keyword for ARM Compiler       */
	#define __STATIC_INLINE  static __inline
	#define __PACKED         __packed

#elif defined ( __ICCARM__ )
	#define __ASM            __asm                                      /*!< asm keyword for IAR Compiler          */
	#define __INLINE         inline                                     /*!< inline keyword for IAR Compiler. Only available in High optimization mode! */
	#define __STATIC_INLINE  static inline 
	#define __PACKED         __attribute__((packed))

#elif defined ( __GNUC__ )
	#define __ASM            __asm                                      /*!< asm keyword for GNU Compiler          */
	#define __INLINE         inline                                     /*!< inline keyword for GNU Compiler       */
	#define __STATIC_INLINE  static inline
	#define __PACKED         __attribute__((packed))
	//read R/C register
	#define __REG_RC16(reg)	 volatile uint16_t __attribute__ ((unused)) __reg = (reg)
	#define __REG_RC32(reg)	 volatile uint32_t __attribute__ ((unused)) __reg = (reg)

#elif defined ( __TASKING__ )
  #define __ASM            __asm                                      /*!< asm keyword for TASKING Compiler      */
  #define __INLINE         inline                                     /*!< inline keyword for TASKING Compiler   */
  #define __STATIC_INLINE  static inline
  #define __PACKED         __attribute__((packed))
#endif



#endif // CC_MACRO_H

