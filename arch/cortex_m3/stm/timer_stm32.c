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

#include "timer.h"
#include "arch.h"
#include "profile.h"
#include "error.h"
#include "types.h"

#if defined(STM32F1)
#include "rcc_stm32f1.h"
#elif defined(STM32F2)
#include "rcc_stm32f2.h"
#elif defined(STM32F4)
#include "rcc_stm32f4.h"
#endif

typedef TIM_TypeDef* TIM_TypeDef_P;

const TIM_TypeDef_P TIMER[] =						{TIM1, TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM8, TIM9, TIM10, TIM11, TIM12, TIM13, TIM14};

//1 == 10, 8 == 13
const IRQn_Type TIMER_VECTORS[] =				{TIM1_UP_TIM10_IRQn, TIM2_IRQn, TIM3_IRQn, TIM4_IRQn, TIM5_IRQn, TIM6_DAC_IRQn, TIM7_IRQn, TIM8_UP_TIM13_IRQn,
															TIM1_BRK_TIM9_IRQn, TIM1_UP_TIM10_IRQn, TIM1_TRG_COM_TIM11_IRQn, TIM8_BRK_TIM12_IRQn, TIM8_UP_TIM13_IRQn,
															TIM8_TRG_COM_TIM14_IRQn};
const uint32_t	 RCC_TIMER[] =						{RCC_APB2ENR_TIM1EN, RCC_APB1ENR_TIM2EN, RCC_APB1ENR_TIM3EN, RCC_APB1ENR_TIM4EN, RCC_APB1ENR_TIM5EN, RCC_APB1ENR_TIM6EN, RCC_APB1ENR_TIM7EN,
															RCC_APB2ENR_TIM8EN, RCC_APB2ENR_TIM9EN, RCC_APB2ENR_TIM10EN, RCC_APB2ENR_TIM11EN, RCC_APB1ENR_TIM12EN, RCC_APB1ENR_TIM13EN, RCC_APB1ENR_TIM14EN};

static char _shared1  __attribute__ ((section (".sys_bss"))) =											0;
static char _shared8	 __attribute__ ((section (".sys_bss"))) =											0;
static TIMER_HANDLER _timer_handlers[TIMERS_COUNT] __attribute__ ((section (".sys_bss"))) =	{NULL};

void TIM1_UP_TIM10_IRQHandler(void)
{
	if (TIM1->SR & TIM_SR_UIF)
	{
		TIMER[TIM_1]->SR &= ~TIM_SR_UIF;
		if (_timer_handlers[TIM_1])
			_timer_handlers[TIM_1](TIM_1);
	}
	else
	{
		TIMER[TIM_10]->SR &= ~TIM_SR_UIF;
		if (_timer_handlers[TIM_10])
			_timer_handlers[TIM_10](TIM_10);
	}
}

void TIM2_IRQHandler(void)
{
	TIMER[TIM_2]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_2])
		_timer_handlers[TIM_2](TIM_2);
}

void TIM3_IRQHandler(void)
{
	TIMER[TIM_3]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_3])
		_timer_handlers[TIM_3](TIM_3);
}

void TIM4_IRQHandler(void)
{
	TIMER[TIM_4]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_4])
		_timer_handlers[TIM_4](TIM_4);
}

void TIM5_IRQHandler(void)
{
	TIMER[TIM_5]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_5])
		_timer_handlers[TIM_5](TIM_5);
}

void TIM6_DAC_IRQHandler(void)
{
	TIMER[TIM_6]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_6])
		_timer_handlers[TIM_6](TIM_6);
}

void TIM7_IRQHandler(void)
{
	TIMER[TIM_7]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_7])
		_timer_handlers[TIM_7](TIM_7);
}

void TIM8_UP_TIM13_IRQHandler(void)
{
	if (TIM8->SR & TIM_SR_UIF)
	{
		TIMER[TIM_8]->SR &= ~TIM_SR_UIF;
		if (_timer_handlers[TIM_8])
			_timer_handlers[TIM_8](TIM_8);
	}
	else
	{
		TIMER[TIM_13]->SR &= ~TIM_SR_UIF;
		if (_timer_handlers[TIM_13])
			_timer_handlers[TIM_13](TIM_13);
	}
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
	TIMER[TIM_9]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_9])
		_timer_handlers[TIM_9](TIM_9);
}

void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
	TIMER[TIM_11]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_11])
		_timer_handlers[TIM_11](TIM_11);
}

void TIM8_BRK_TIM12_IRQHandler(void)
{
	TIMER[TIM_12]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_12])
		_timer_handlers[TIM_12](TIM_12);
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	TIMER[TIM_14]->SR &= ~TIM_SR_UIF;
	if (_timer_handlers[TIM_14])
		_timer_handlers[TIM_14](TIM_14);
}

void timer_enable(TIMER_CLASS timer, TIMER_HANDLER handler, int priority, unsigned int flags)
{
	if (timer < TIMERS_COUNT)
	{
		_timer_handlers[timer] = handler;
		//power up
		switch (timer)
		{
		case TIM_1:
		case TIM_8:
		case TIM_9:
		case TIM_10:
		case TIM_11:
			RCC->APB2ENR |= RCC_TIMER[timer];
			break;
		default:
			RCC->APB1ENR |= RCC_TIMER[timer];
		}

		//one-pulse mode
		TIMER[timer]->CR1 |= TIM_CR1_URS;
		if (flags & TIMER_FLAG_ONE_PULSE_MODE)
			TIMER[timer]->CR1 |= TIM_CR1_OPM;
		TIMER[timer]->DIER |= TIM_DIER_UIE;

		//enable IRQ
		if (timer == TIM_1 || timer == TIM_10)
		{
			if (_shared1++ == 0)
				NVIC_EnableIRQ(TIMER_VECTORS[timer]);
		}
		else if (timer == TIM_8 || timer == TIM_13)
		{
			if (_shared8++ == 0)
				NVIC_EnableIRQ(TIMER_VECTORS[timer]);
		}
		else
			NVIC_EnableIRQ(TIMER_VECTORS[timer]);
		NVIC_SetPriority(TIMER_VECTORS[timer], priority);
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_TIMER, timer);
}

void timer_disable(TIMER_CLASS timer)
{
	if (timer < TIMERS_COUNT)
	{
		TIMER[timer]->CR1 &= ~TIM_CR1_CEN;
		//disable IRQ
		if (timer == TIM_1 || timer == TIM_10)
		{
			if (--_shared1== 0)
				NVIC_DisableIRQ(TIMER_VECTORS[timer]);
		}
		else if (timer == TIM_8 || timer == TIM_13)
		{
			if (--_shared8 == 0)
				NVIC_DisableIRQ(TIMER_VECTORS[timer]);
		}
		else
			NVIC_DisableIRQ(TIMER_VECTORS[timer]);

		//power down
		switch (timer)
		{
		case TIM_1:
		case TIM_8:
		case TIM_9:
		case TIM_10:
		case TIM_11:
			RCC->APB2ENR &= ~RCC_TIMER[timer];
			break;
		default:
			RCC->APB1ENR &= ~RCC_TIMER[timer];
		}
		_timer_handlers[timer] = NULL;
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_TIMER, timer);
}

void timer_start(TIMER_CLASS timer, unsigned int time_us)
{
	uint32_t timer_freq;
	switch (timer)
	{
	case TIM_1:
	case TIM_8:
	case TIM_9:
	case TIM_10:
	case TIM_11:
		timer_freq = _apb2_freq;
		break;
	default:
		timer_freq = _apb1_freq;
	}
	if (timer_freq != _ahb_freq)
		timer_freq = timer_freq * 2;

	//find prescaller
	unsigned int psc = 1; //1us
	unsigned int count = time_us;
	while (count > 0xffff)
	{
		psc <<= 1;
		count >>= 1;
	}
	psc *= timer_freq / 1000000; //1us

	TIMER[timer]->PSC = psc - 1;
	TIMER[timer]->ARR = count - 1;
	TIMER[timer]->CNT = 0;

	TIMER[timer]->EGR = TIM_EGR_UG;
	TIMER[timer]->CR1 |= TIM_CR1_CEN;
}

void timer_stop(TIMER_CLASS timer)
{
	TIMER[timer]->CR1 &= ~TIM_CR1_CEN;
	TIMER[timer]->SR &= ~TIM_SR_UIF;
}

unsigned int timer_elapsed(TIMER_CLASS timer)
{
	uint32_t timer_freq;
	switch (timer)
	{
	case TIM_1:
	case TIM_8:
	case TIM_9:
	case TIM_10:
	case TIM_11:
		timer_freq = _apb2_freq;
		break;
	default:
		timer_freq = _apb1_freq;
	}
	if (timer_freq !=_ahb_freq)
		timer_freq = timer_freq * 2;
	return (((TIMER[timer]->PSC) + 1)/ (timer_freq / 1000000)) * ((TIMER[timer]->CNT) + 1);
}
