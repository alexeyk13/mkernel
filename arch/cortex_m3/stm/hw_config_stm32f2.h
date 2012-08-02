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

#ifndef HW_CONFIG_STM32_F2
#define HW_CONFIG_STM32_F2

//--------------------- RCC hw config -----------------------------------------
//choose one of for pll-q tuning
//for USB-FS
//#define PLL_Q_48MHZ
//for SDIO 25MHZ
//#define PLL_Q_25MHZ
#define PLL_Q_DONT_CARE

//if system stucks during flashing, turn this on, than back off for speed
//#define UNSTUCK_FLASH

//--------------------- USART hw config -----------------------------------------
//each bit for port
#define USART_RX_DISABLE_MASK_DEF				(1 << 0)
#define USART_TX_DISABLE_MASK_DEF				(0)
//USART1
/*
#define USART1_TX_PIN								GPIO_A9
#define USART1_RX_PIN								GPIO_A10
*/

#define USART1_TX_PIN								GPIO_B6
#define USART1_RX_PIN								GPIO_B7

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

//--------------------- I2C hw config -----------------------------------------
//I2C1
#define I2C1_SCL_PIN									GPIO_B6
#define I2C1_SDA_PIN									GPIO_B7

/*
#define I2C1_SCL_PIN									GPIO_B8
#define I2C1_SDA_PIN									GPIO_B9
*/

//I2C2
#define I2C2_SCL_PIN									GPIO_B10
#define I2C2_SDA_PIN									GPIO_B11

/*
#define I2C2_SCL_PIN									GPIO_F0
#define I2C2_SDA_PIN									GPIO_F1
*/

/*
#define I2C2_SCL_PIN									GPIO_H4
#define I2C2_SDA_PIN									GPIO_H5
*/

//I2C3
#define I2C3_SCL_PIN									GPIO_A8
#define I2C3_SDA_PIN									GPIO_C9

/*
#define I2C3_SCL_PIN									GPIO_H7
#define I2C3_SDA_PIN									GPIO_H8
*/
//--------------------- USB hw config -----------------------------------------
//pin remaps for ULPI

#define HS_NXT_PIN									GPIO_C3
#define HS_DIR_PIN									GPIO_C2

/*
#define HS_NXT_PIN									GPIO_H4
#define HS_DIR_PIN									GPIO_I11
*/

#define USB_DMA_ENABLED

//ST device-library specific
#define USB_ADDRESS_POST_PROCESSING				0

#ifdef USB_DMA_ENABLED
#define USB_DATA_ALIGN								8
#else
#define USB_DATA_ALIGN								4
#endif
//--------------------- SDIO hw config ---------------------------------------
#define SDIO_DMA_ENABLED
#define SDIO_DMA_STREAM								DMA_STREAM_3
//#define SDIO_DMA_STREAM								DMA_STREAM_6

#endif // HW_CONFIG_STM32_F2

