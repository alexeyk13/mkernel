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

#ifndef VECTORS_STM32_H
#define VECTORS_STM32_H

#ifdef STM32F2

/* External Interrupts */
.macro	external_vectors
	.word     WWDG_IRQHandler                   /* Window WatchDog              */
	.word     PVD_IRQHandler                    /* PVD through EXTI Line detection */
	.word     TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
	.word     RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
	.word     FLASH_IRQHandler                  /* FLASH                        */
	.word     RCC_IRQHandler                    /* RCC                          */
	.word     EXTI0_IRQHandler                  /* EXTI Line0                   */
	.word     EXTI1_IRQHandler                  /* EXTI Line1                   */
	.word     EXTI2_IRQHandler                  /* EXTI Line2                   */
	.word     EXTI3_IRQHandler                  /* EXTI Line3                   */
	.word     EXTI4_IRQHandler                  /* EXTI Line4                   */
	.word     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
	.word     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
	.word     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
	.word     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
	.word     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
	.word     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
	.word     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
	.word     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
	.word     CAN1_TX_IRQHandler                /* CAN1 TX                      */
	.word     CAN1_RX0_IRQHandler               /* CAN1 RX0                     */
	.word     CAN1_RX1_IRQHandler               /* CAN1 RX1                     */
	.word     CAN1_SCE_IRQHandler               /* CAN1 SCE                     */
	.word     EXTI9_5_IRQHandler                /* External Line[9:5]s          */
	.word     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9          */
	.word     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10        */
	.word     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
	.word     TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
	.word     TIM2_IRQHandler                   /* TIM2                         */
	.word     TIM3_IRQHandler                   /* TIM3                         */
	.word     TIM4_IRQHandler                   /* TIM4                         */
	.word     I2C1_EV_IRQHandler                /* I2C1 Event                   */
	.word     I2C1_ER_IRQHandler                /* I2C1 Error                   */
	.word     I2C2_EV_IRQHandler                /* I2C2 Event                   */
	.word     I2C2_ER_IRQHandler                /* I2C2 Error                   */
	.word     SPI1_IRQHandler                   /* SPI1                         */
	.word     SPI2_IRQHandler                   /* SPI2                         */
	.word     USART1_IRQHandler                 /* USART1                       */
	.word     USART2_IRQHandler                 /* USART2                       */
	.word     USART3_IRQHandler                 /* USART3                       */
	.word     EXTI15_10_IRQHandler              /* External Line[15:10]s        */
	.word     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
	.word     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */
	.word     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
	.word     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
	.word     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
	.word     TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
	.word     DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
	.word     FSMC_IRQHandler                   /* FSMC                         */
	.word     SDIO_IRQHandler                   /* SDIO                         */
	.word     TIM5_IRQHandler                   /* TIM5                         */
	.word     SPI3_IRQHandler                   /* SPI3                         */
	.word     UART4_IRQHandler                  /* UART4                        */
	.word     UART5_IRQHandler                  /* UART5                        */
	.word     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
	.word     TIM7_IRQHandler                   /* TIM7                         */
	.word     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
	.word     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
	.word     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
	.word     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
	.word     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
	.word     ETH_IRQHandler                    /* Ethernet                     */
	.word     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
	.word     CAN2_TX_IRQHandler                /* CAN2 TX                      */
	.word     CAN2_RX0_IRQHandler               /* CAN2 RX0                     */
	.word     CAN2_RX1_IRQHandler               /* CAN2 RX1                     */
	.word     CAN2_SCE_IRQHandler               /* CAN2 SCE                     */
	.word     OTG_FS_IRQHandler                 /* USB OTG FS                   */
	.word     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
	.word     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
	.word     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
	.word     USART6_IRQHandler                 /* USART6                       */
	.word     I2C3_EV_IRQHandler                /* I2C3 event                   */
	.word     I2C3_ER_IRQHandler                /* I2C3 error                   */
	.word     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
	.word     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
	.word     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
	.word     OTG_HS_IRQHandler                 /* USB OTG HS                   */
	.word     DCMI_IRQHandler                   /* DCMI                         */
	.word     CRYP_IRQHandler                   /* CRYP crypto                  */
	.word     HASH_RNG_IRQHandler               /* Hash and Rng                 */
.endm

.macro external_weak_vectors
	weak_vector	WWDG_IRQHandler
	weak_vector	PVD_IRQHandler
	weak_vector	TAMP_STAMP_IRQHandler
	weak_vector	RTC_WKUP_IRQHandler
	weak_vector	FLASH_IRQHandler
	weak_vector	RCC_IRQHandler
	weak_vector	EXTI0_IRQHandler
	weak_vector	EXTI1_IRQHandler
	weak_vector	EXTI2_IRQHandler
	weak_vector	EXTI3_IRQHandler
	weak_vector	EXTI4_IRQHandler
	weak_vector	DMA1_Stream0_IRQHandler
	weak_vector	DMA1_Stream1_IRQHandler
	weak_vector	DMA1_Stream2_IRQHandler
	weak_vector	DMA1_Stream3_IRQHandler
	weak_vector	DMA1_Stream4_IRQHandler
	weak_vector	DMA1_Stream5_IRQHandler
	weak_vector	DMA1_Stream6_IRQHandler
	weak_vector	ADC_IRQHandler
	weak_vector	CAN1_TX_IRQHandler
	weak_vector	CAN1_RX0_IRQHandler
	weak_vector	CAN1_RX1_IRQHandler
	weak_vector	CAN1_SCE_IRQHandler
	weak_vector	EXTI9_5_IRQHandler
	weak_vector	TIM1_BRK_TIM9_IRQHandler
	weak_vector	TIM1_UP_TIM10_IRQHandler
	weak_vector	TIM1_TRG_COM_TIM11_IRQHandler
	weak_vector	TIM1_CC_IRQHandler
	weak_vector	TIM2_IRQHandler
	weak_vector	TIM3_IRQHandler
	weak_vector	TIM4_IRQHandler
	weak_vector	I2C1_EV_IRQHandler
	weak_vector	I2C1_ER_IRQHandler
	weak_vector	I2C2_EV_IRQHandler
	weak_vector	I2C2_ER_IRQHandler
	weak_vector	SPI1_IRQHandler
	weak_vector	SPI2_IRQHandler
	weak_vector	USART1_IRQHandler
	weak_vector	USART2_IRQHandler
	weak_vector	USART3_IRQHandler
	weak_vector	EXTI15_10_IRQHandler
	weak_vector	RTC_Alarm_IRQHandler
	weak_vector	OTG_FS_WKUP_IRQHandler
	weak_vector	TIM8_BRK_TIM12_IRQHandler
	weak_vector	TIM8_UP_TIM13_IRQHandler
	weak_vector	TIM8_TRG_COM_TIM14_IRQHandler
	weak_vector	TIM8_CC_IRQHandler
	weak_vector	DMA1_Stream7_IRQHandler
	weak_vector	FSMC_IRQHandler
	weak_vector	SDIO_IRQHandler
	weak_vector	TIM5_IRQHandler
	weak_vector	SPI3_IRQHandler
	weak_vector	UART4_IRQHandler
	weak_vector	UART5_IRQHandler
	weak_vector	TIM6_DAC_IRQHandler
	weak_vector	TIM7_IRQHandler
	weak_vector	DMA2_Stream0_IRQHandler
	weak_vector	DMA2_Stream1_IRQHandler
	weak_vector	DMA2_Stream2_IRQHandler
	weak_vector	DMA2_Stream3_IRQHandler
	weak_vector	DMA2_Stream4_IRQHandler
	weak_vector	ETH_IRQHandler
	weak_vector	ETH_WKUP_IRQHandler
	weak_vector	CAN2_TX_IRQHandler
	weak_vector	CAN2_RX0_IRQHandler
	weak_vector	CAN2_RX1_IRQHandler
	weak_vector	CAN2_SCE_IRQHandler
	weak_vector	OTG_FS_IRQHandler
	weak_vector	DMA2_Stream5_IRQHandler
	weak_vector	DMA2_Stream6_IRQHandler
	weak_vector	DMA2_Stream7_IRQHandler
	weak_vector	USART6_IRQHandler
	weak_vector	I2C3_EV_IRQHandler
	weak_vector	I2C3_ER_IRQHandler
	weak_vector	OTG_HS_EP1_OUT_IRQHandler
	weak_vector	OTG_HS_EP1_IN_IRQHandler
	weak_vector	OTG_HS_WKUP_IRQHandler
	weak_vector	OTG_HS_IRQHandler
	weak_vector	DCMI_IRQHandler
	weak_vector	CRYP_IRQHandler
	weak_vector	HASH_RNG_IRQHandler
.endm
#else
#error STM architecture unknown
#endif

#endif // VECTORS_STM32_H
