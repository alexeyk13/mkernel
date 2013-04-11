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

#ifndef HW_CONFIG_STM32_F1
#define HW_CONFIG_STM32_F1

//--------------------- USART hw config -----------------------------------------
//each bit for port
//#define USART_RX_DISABLE_MASK_DEF				(1 << UART_1)
#define USART_RX_DISABLE_MASK_DEF				(0)
#define USART_TX_DISABLE_MASK_DEF				(0)

#define USART_REMAP_MASK							(0)
//#define USART_REMAP_MASK							(1 << UART_1)

//USART1
#define USART1_TX_PIN								GPIO_A9
#define USART1_RX_PIN								GPIO_A10

/*
#define USART1_TX_PIN								GPIO_B6
#define USART1_RX_PIN								GPIO_B7
*/

//USART2
#define USART2_TX_PIN								GPIO_A2
#define USART2_RX_PIN								GPIO_A3

/*
#define USART2_TX_PIN								GPIO_D5
#define USART2_RX_PIN								GPIO_D6
*/

//USART3
#define USART3_TX_PIN								GPIO_B10
#define USART3_RX_PIN								GPIO_B11

/*
#define USART3_TX_PIN								GPIO_C10
#define USART3_RX_PIN								GPIO_C11
*/

/*
#define USART3_TX_PIN								GPIO_D8
#define USART3_RX_PIN								GPIO_D9
*/

//UART4
#define UART4_TX_PIN									GPIO_A0
#define UART4_RX_PIN									GPIO_A1

/*
#define UART4_TX_PIN									GPIO_C10
#define UART4_RX_PIN									GPIO_C11
*/

//USART6
#define USART6_TX_PIN								GPIO_C6
#define USART6_RX_PIN								GPIO_C7

/*
#define USART6_TX_PIN								GPIO_G9
#define USART6_RX_PIN								GPIO_G14
*/


#endif // HW_CONFIG_STM32_F1

