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

#include "rcc.h"
#include "arch.h"
#include "hw_config.h"
#include "profile.h"
#include "delay.h"

#define VCO_MIN										64000000l
#define VCO_MAX										432000000l

#define N_MIN											63l
#define N_MAX											432l
#define P_MIN											2l
#define P_MAX											8l
#define Q_MIN											2l
#define Q_MAX											15l

const unsigned int BUS_PRESCALLER[]				= {1, 2, 3, 4, 6, 7, 8, 9};

unsigned long _core_freq  __attribute__ ((section (".sys_bss"))) =		0;
unsigned long _core_cycles_us;
unsigned long _core_cycles_ms;
unsigned long _fs_freq  __attribute__ ((section (".sys_bss"))) =			0;
unsigned long _ahb_freq  __attribute__ ((section (".sys_bss"))) =			0;
unsigned long _apb1_freq  __attribute__ ((section (".sys_bss"))) =		0;
unsigned long _apb2_freq  __attribute__ ((section (".sys_bss"))) =		0;

static inline bool turn_hse_on()
{
	int i;
	if (HSE_VALUE)
	{
		RCC->CR |= RCC_CR_HSEON;

		for (i = 0; i < HSE_STARTUP_TIMEOUT; ++i)
			if (RCC->CR & RCC_CR_HSERDY)
				return true;
	}
	return false;
}

static inline unsigned long setup_pll(uint32_t pll_source, unsigned long desired_freq)
{
	// m_in = base_clock / m
	// m_in must be in range 1..2 mhz, recommended value is 2mhz.
	// vco = m_in * n, must be in range 64..432 mhz
	// core_clock = vco / p
	// usb_fs_clock = vco / q
	uint32_t m, m_in, n, p, vco, q, div, mul, freq_diff, cur_freq;
	m = (pll_source == RCC_PLLCFGR_PLLSRC_HSE ? HSE_VALUE : HSI_VALUE) / 1000000ul;
	vco = 0;
	//m_in must be in range
	if (m & 1)
		m_in = 1000000ul;
	else
	{
		m = m / 2;
		m_in = 2000000ul;
	}

	freq_diff = desired_freq;
	n = p = q = 0;
#ifndef PLL_Q_DONT_CARE
	unsigned int cur_p;
	for (div = Q_MIN; div <= Q_MAX && freq_diff; ++div)
	{
#ifdef PLL_Q_48MHZ
		vco = MAX_FS_FREQ * div;
#else //PLL_Q_25MHZ
		vco = 25000000ul * div;
#endif //PLL_Q_48MHZ
		mul = vco / m_in;
		vco = mul * m_in;
		if (vco < VCO_MIN)
			continue;
		if (vco > VCO_MAX)
			break;
		cur_p = vco / desired_freq;
		if (cur_p > P_MAX)
			cur_p = P_MAX;
		if (cur_p < P_MIN)
			cur_p = P_MIN;
		while ((cur_p > P_MIN) && (vco / cur_p > MAX_CORE_FREQ))
			--cur_p;
		cur_freq = (m_in * mul) / cur_p;
		if (vco / cur_p <= MAX_CORE_FREQ && desired_freq - cur_freq < freq_diff)
		{
			freq_diff = desired_freq - cur_freq;
			n = mul;
			q = div;
			p = cur_p;
		}
	}
#else //PLL_Q_DONT_CARE
	for (div = P_MIN; div <= P_MAX && freq_diff; div += 2)
	{
		vco = desired_freq * div;
		if (vco < VCO_MIN)
			continue;
		if (vco > VCO_MAX)
			break;
		mul = vco / m_in;
		cur_freq = (m_in * mul) / div;
		if (cur_freq <= desired_freq && desired_freq - cur_freq < freq_diff)
		{
			freq_diff = desired_freq - cur_freq;
			n = mul;
			p = div;
		}
	}
	q =  (m_in * n) / MAX_FS_FREQ;
	while ((m_in * n) / q > MAX_FS_FREQ)
		++q;
#endif //PLL_Q_DONT_CARE

	RCC->PLLCFGR = (m << 0) | (n << 6) | (((p >> 1) -1) << 16) | (pll_source) | (q << 24);
	//turn PLL on
	RCC->CR |= RCC_CR_PLLON;
	while (!(RCC->CR & RCC_CR_PLLRDY)) {}

	_fs_freq = (m_in * n) / q;

	return (m_in * n) / p;
}

static inline void switch_to_source (uint32_t source)
{
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= source;

	/* Wait till the main PLL is used as system clock source */
	while (((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) >> 2) != source) {}
}

static inline void setup_buses(uint32_t core_freq)
{
	//AHB. Can operates at maximum clock
	RCC->CFGR &= ~RCC_CFGR_HPRE;
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
	_ahb_freq = core_freq;

	//APB2
	//maximum frequency 60MHz
	RCC->CFGR &= ~RCC_CFGR_PPRE2;
	if (core_freq > MAX_APB2_FREQ)
	{
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;
		_apb2_freq = _ahb_freq / 2;
	}
	else
	{
		RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
		_apb2_freq = _ahb_freq;
	}

	//APB1
	RCC->CFGR &= ~RCC_CFGR_PPRE1;
	//maximum frequency 30MHz
	if (core_freq > MAX_APB1_FREQ)
	{
		if (core_freq / 2 > MAX_APB1_FREQ)
		{
			RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;
			_apb1_freq = _ahb_freq / 4;
		}
		else
		{
			RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
			_apb1_freq = _ahb_freq / 2;
		}

	}
	else
	{
		RCC->CFGR |= RCC_CFGR_PPRE1_DIV1;
		_apb1_freq = _ahb_freq;
	}
}

static inline void tune_flash_latency(uint32_t ahb_freq)
{
	uint32_t latency = 0;
#ifdef UNSTUCK_FLASH
	latency = FLASH_ACR_LATENCY_3WS;
#else
	if (ahb_freq > 90000000)
		latency = FLASH_ACR_LATENCY_3WS;
	else 	if (ahb_freq > 60000000)
		latency = FLASH_ACR_LATENCY_2WS;
	else 	if (ahb_freq > 30000000)
		latency = FLASH_ACR_LATENCY_1WS;
	else
		latency = FLASH_ACR_LATENCY_0WS;
#endif

	FLASH->ACR = FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN | latency;
}

RESET_REASON get_reset_reason()
{
	RESET_REASON res = RESET_REASON_UNKNOWN;
	if (RCC->CSR & RCC_CSR_LPWRRSTF)
		res = RESET_REASON_STANBY;
	else if (RCC->CSR & (RCC_CSR_WWDGRSTF | RCC_CSR_WDGRSTF))
		res = RESET_REASON_WATCHDOG;
	else if (RCC->CSR & RCC_CSR_SFTRSTF)
		res = RESET_REASON_SOFTWARE;
	else if (RCC->CSR & RCC_CSR_PORRSTF)
		res = RESET_REASON_POWERON;
	else if (RCC->CSR & RCC_CSR_PADRSTF)
		res = RESET_REASON_PIN_RST;
	return res;
}

uint32_t get_core_freq()
{
	uint32_t res = HSI_VALUE;
	unsigned int vco, m, n, p;
	switch (RCC->CFGR & RCC_CFGR_SWS)
	{
		case RCC_CFGR_SWS_HSE:
			res = HSE_VALUE;
			break;
		case RCC_CFGR_SWS_PLL:
			m = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
			n = (RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6;
			p = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1) * 2;
			if (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC_HSE)
				vco = (HSE_VALUE / m) * n;
			else
				vco = (HSI_VALUE / m) * n;
			res = vco / p;
			break;
	}
	return res;
}

unsigned long set_core_freq(unsigned long desired_freq)
{
	uint32_t ahb1, ahb2, ahb3, apb1, apb2;
	//disable all periphery
	apb1 = RCC->APB1ENR;
	RCC->APB1ENR = 0;
	apb2 = RCC->APB2ENR;
	RCC->APB2ENR = 0;
	ahb1 = RCC->AHB1ENR;
	RCC->AHB1ENR = 0;
	ahb2 = RCC->AHB2ENR;
	RCC->AHB2ENR = 0;
	ahb3 = RCC->AHB3ENR;
	RCC->AHB3ENR = 0;

	_core_freq = desired_freq == 0 || desired_freq > MAX_CORE_FREQ ? MAX_CORE_FREQ : desired_freq;

	//if HSE clock provided, trying using it
	uint32_t pll_source = turn_hse_on() ? RCC_PLLCFGR_PLLSRC_HSE : RCC_PLLCFGR_PLLSRC_HSI;

	switch_to_source(RCC_CFGR_SW_HSI);
	_core_freq = setup_pll(pll_source, _core_freq);
	setup_buses(_core_freq);
	tune_flash_latency(_core_freq);
	switch_to_source(RCC_CFGR_SW_PLL);

	//restore all periphery
	RCC->AHB1ENR = ahb1;
	RCC->AHB2ENR = ahb2;
	RCC->AHB3ENR = ahb3;
	RCC->APB1ENR = apb1;
	RCC->APB2ENR = apb2;

	_core_cycles_us = _core_freq / 1250000;
	_core_cycles_ms = _core_freq / 1250;

	return _core_freq;
}
