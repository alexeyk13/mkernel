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

#include "gpio_stm32f2.h"
#include "arch.h"
#include "profile.h"
#include "error.h"

#define PORT(pin)																																(pin / 32)
#define PIN(pin)																																(pin & 31)


const GPIO_TypeDef_P GPIO[] =																												{GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI};
const uint32_t RCC_GPIO[] =																												{RCC_AHB1ENR_GPIOAEN, RCC_AHB1ENR_GPIOBEN, RCC_AHB1ENR_GPIOCEN,
																																					 RCC_AHB1ENR_GPIODEN, RCC_AHB1ENR_GPIOEEN, RCC_AHB1ENR_GPIOFEN,
																																					 RCC_AHB1ENR_GPIOGEN, RCC_AHB1ENR_GPIOHEN, RCC_AHB1ENR_GPIOIEN};
const IRQn_Type EXTI_SINGLE_VECTORS[] =																								{EXTI0_IRQn, EXTI1_IRQn, EXTI2_IRQn, EXTI3_IRQn, EXTI4_IRQn};

char _used_pins[9] __attribute__ ((section (".sys_bss"))) =																		{0};
char _syscfg_count __attribute__ ((section (".sys_bss"))) =																		0;
char _exti_5_9_active __attribute__ ((section (".sys_bss"))) =																	0;
char _exti_10_15_active __attribute__ ((section (".sys_bss")))	=																0;

static EXTI_HANDLER _exti_handlers[EXTI_LINES_COUNT] __attribute__ ((section (".sys_bss"))) =						{NULL};

//port is not decoded on STM32f2xx: only one line can be set for port
void EXTI0_IRQHandler (void)
{
	if (EXTI->PR & (1 << 0))
	{
		EXTI->PR |= (1 << 0);
		if (_exti_handlers[0] != NULL)
			_exti_handlers[0](GPIO_A0);
	}
}

void EXTI1_IRQHandler (void)
{
	if (EXTI->PR & (1 << 1))
	{
		EXTI->PR |= (1 << 1);
		if (_exti_handlers[1] != NULL)
			_exti_handlers[1](GPIO_A1);
	}
}

void EXTI2_IRQHandler (void)
{
	if (EXTI->PR & (1 << 2))
	{
		EXTI->PR |= (1 << 2);
		if (_exti_handlers[2] != NULL)
			_exti_handlers[2](GPIO_A2);
	}
}

void EXTI3_IRQHandler (void)
{
	if (EXTI->PR & (1 << 3))
	{
		EXTI->PR |= (1 << 3);
		if (_exti_handlers[3] != NULL)
			_exti_handlers[3](GPIO_A3);
	}
}

void EXTI4_IRQHandler (void)
{
	if (EXTI->PR & (1 << 4))
	{
		EXTI->PR |= (1 << 4);
		if (_exti_handlers[4] != NULL)
			_exti_handlers[4](GPIO_A4);
	}
}

void EXTI9_5_IRQHandler(void)
{
	int i;
	for (i = 5; i <= 9; ++i)
	{
		if (EXTI->PR & (1 << i))
		{
			EXTI->PR |= (1 << i);
			if (_exti_handlers[i] != NULL)
				_exti_handlers[i](GPIO_A0 + i);
		}
	}
}

void EXTI15_10_IRQHandler(void)
{
	int i;
	for (i = 10; i <= 15; ++i)
	{
		if (EXTI->PR & (1 << i))
		{
			EXTI->PR |= (1 << i);
			if (_exti_handlers[i] != NULL)
				_exti_handlers[i](GPIO_A0 + i);
		}
	}
}

void gpio_enable_pin_power(GPIO_CLASS pin)
{
	if (_used_pins[PORT(pin)]++ == 0)
		RCC->AHB1ENR |= RCC_GPIO[PORT(pin)];
}

void gpio_enable_pin(GPIO_CLASS pin, PIN_MODE mode)
{
	gpio_enable_pin_power(pin);

	//in/out
	GPIO[PORT(pin)]->MODER &= ~(3 << (PIN(pin) * 2));
	switch (mode)
	{
	case PIN_MODE_OUT:
	case PIN_MODE_OUT_OD:
		//speed 100mhz
		GPIO[PORT(pin)]->OSPEEDR |= (3 << (PIN(pin) * 2));
		GPIO[PORT(pin)]->MODER |= (1 << (PIN(pin) * 2));
		break;
	default:
		break;
	}

	//out pp/od
	switch (mode)
	{
	case PIN_MODE_OUT:
		GPIO[PORT(pin)]->OTYPER &= ~(1 << PIN(pin));
		break;
	case PIN_MODE_OUT_OD:
		GPIO[PORT(pin)]->OTYPER |= 1 << PIN(pin);
		break;
	default:
		break;
	}

	//pullup/pulldown
	GPIO[PORT(pin)]->PUPDR &= ~(3 << (PIN(pin) * 2));
	switch (mode)
	{
	case PIN_MODE_IN_PULLUP:
		GPIO[PORT(pin)]->PUPDR |= (1 << (PIN(pin) * 2));
		break;
	case PIN_MODE_IN_PULLDOWN:
		GPIO[PORT(pin)]->PUPDR |= (2 << (PIN(pin) * 2));
		break;
	default:
		break;
	}
}

void gpio_disable_pin(GPIO_CLASS pin)
{
	GPIO[PORT(pin)]->MODER &= ~(3 << (PIN(pin) * 2));
	GPIO[PORT(pin)]->PUPDR &= ~(3 << (PIN(pin) * 2));
	GPIO[PORT(pin)]->OSPEEDR |= (3 << (PIN(pin) * 2));

	if (--_used_pins[PORT(pin)] == 0)
		RCC->AHB1ENR &= ~RCC_GPIO[PORT(pin)];
}

void gpio_enable_afio(GPIO_CLASS pin, AFIO_MODE mode, AFIO_PUSH_MODE push_mode)
{
	gpio_enable_pin_power(pin);

	GPIO[PORT(pin)]->AFR[PIN(pin) >= 8 ? 1 : 0] &= ~(0xful << ((PIN(pin) & 0x7ul) * 4ul));
	GPIO[PORT(pin)]->AFR[PIN(pin) >= 8 ? 1 : 0] |= ((uint32_t)(mode & 0xf) << ((PIN(pin) & 0x7ul) * 4ul));

	GPIO[PORT(pin)]->MODER &= ~(3 << (PIN(pin) * 2));
	GPIO[PORT(pin)]->MODER |= (2 << (PIN(pin) * 2));
	GPIO[PORT(pin)]->OSPEEDR |= (3 << (PIN(pin) * 2));

	//out pp/od
	switch (push_mode)
	{
	case AFIO_PULL_UP:
	case AFIO_PULL_DOWN:
	case AFIO_NO_PULL:
		GPIO[PORT(pin)]->OTYPER &= ~(1 << PIN(pin));
		break;
	default:
		GPIO[PORT(pin)]->OTYPER |= 1 << PIN(pin);
	}

	//pull up/down
	GPIO[PORT(pin)]->PUPDR &= ~(3 << (PIN(pin) * 2));
	switch (push_mode)
	{
	case AFIO_PULL_UP:
	case AFIO_OD_UP:
		GPIO[PORT(pin)]->PUPDR |= (1 << (PIN(pin) * 2));
		break;
	case AFIO_PULL_DOWN:
	case AFIO_OD_DOWN:
		GPIO[PORT(pin)]->PUPDR |= (2 << (PIN(pin) * 2));
		break;
	default:
		break;
	}
}

void gpio_set_pin(GPIO_CLASS pin, bool set)
{
	if (set)
		GPIO[PORT(pin)]->BSRRL = 1 << PIN(pin);
	else
		GPIO[PORT(pin)]->BSRRH = 1 << PIN(pin);
}

bool gpio_get_pin(GPIO_CLASS pin)
{
	return GPIO[PORT(pin)]->IDR & (1 << PIN(pin)) ? true : false;
}

bool gpio_get_out_pin(GPIO_CLASS pin)
{
	return GPIO[PORT(pin)]->ODR & (1 << PIN(pin)) ? true : false;
}

void gpio_disable_jtag()
{
	gpio_enable_pin(GPIO_A13, PIN_MODE_IN);
	gpio_enable_pin(GPIO_A14, PIN_MODE_IN);
	gpio_enable_pin(GPIO_A15, PIN_MODE_IN);
	gpio_enable_pin(GPIO_B3, PIN_MODE_IN);
	gpio_enable_pin(GPIO_B4, PIN_MODE_IN);
}

void gpio_exti_enable(EXTI_CLASS exti, EXTI_MODE mode, EXTI_HANDLER callback, int priority)
{
	if (PIN(exti) < EXTI_LINES_COUNT)
	{
		_exti_handlers[PIN(exti)] = callback;
		if (_syscfg_count++ == 0)
			RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
		//connect to event
		SYSCFG->EXTICR[PIN(exti) / 4] &= ~(0xf << ((PIN(exti) & 3) * 4));
		SYSCFG->EXTICR[PIN(exti) / 4] |= PORT(exti) << ((PIN(exti) & 3) * 4);

		// Configure EXTI Line
		if (mode == EXTI_MODE_RISING || mode == EXTI_MODE_BOTH)
			EXTI->RTSR |= 1 << PIN(exti);
		else
			EXTI->RTSR &= ~(1 << PIN(exti));
		if (mode == EXTI_MODE_FALLING || mode == EXTI_MODE_BOTH)
			EXTI->FTSR |= 1 << PIN(exti);
		else
			EXTI->FTSR &= ~(1 << PIN(exti));

		EXTI->IMR |= 1 << PIN(exti);
		EXTI->EMR |= 1 << PIN(exti);

		//enable irq, if needed
		if (PIN(exti) <= 4)
		{
			NVIC_EnableIRQ(EXTI_SINGLE_VECTORS[PIN(exti)]);
			NVIC_SetPriority(EXTI_SINGLE_VECTORS[PIN(exti)], priority);
		}
		else if (PIN(exti) <= 10)
		{
			if (_exti_5_9_active++ == 0)
			{
				NVIC_EnableIRQ(EXTI9_5_IRQn);
				NVIC_SetPriority(EXTI9_5_IRQn, priority);
			}
		}
		else // if (PIN(exti) <= 15)
		{
			if (_exti_10_15_active++ == 0)
			{
				NVIC_EnableIRQ(EXTI15_10_IRQn);
				NVIC_SetPriority(EXTI15_10_IRQn, priority);
			}
		}
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_GPIO, 0);
}

void gpio_exti_disable(EXTI_CLASS exti)
{
	if (PIN(exti) < EXTI_LINES_COUNT)
	{
		//disable irq, if needed
		if (PIN(exti)  <= 4)
			NVIC_DisableIRQ(EXTI_SINGLE_VECTORS[PIN(exti)]);
		else if (PIN(exti) <= 10)
		{
			if (--_exti_5_9_active == 0)
				NVIC_DisableIRQ(EXTI9_5_IRQn);
		}
		else // if (PIN(exti) <= 15)
		{
			if (--_exti_10_15_active == 0)
				NVIC_DisableIRQ(EXTI15_10_IRQn);
		}

		if (--_syscfg_count == 0)
			RCC->APB2ENR &= ~RCC_APB2ENR_SYSCFGEN;

		EXTI->IMR &= ~(1 << PIN(exti));
		EXTI->EMR &= ~(1 << PIN(exti));
		_exti_handlers[PIN(exti)] = NULL;
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_GPIO, 0);
}
