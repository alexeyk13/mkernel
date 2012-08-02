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

#include "dma_stm32.h"
#include "profile.h"
#include "error.h"

const DMA_FULL_TypeDef_P DMA[] =					{DMA1_FULL, DMA2_FULL};
const uint32_t RCC_DMA[] =							{RCC_AHB1ENR_DMA1EN, RCC_AHB1ENR_DMA2EN};
const uint32_t DMA_SHIFT_MASK[] =				{0x0, 0x6, 0x10, 0x16};

unsigned long _dma_enabled[DMA_COUNT]  __attribute__ ((section (".sys_bss"))) =		{0};

void dma_enable(DMA_CLASS dma)
{
	if (dma < DMA_COUNT)
	{
		if (_dma_enabled[dma]++ == 0)
			RCC->AHB1ENR |= RCC_DMA[dma];
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_UART, dma);
}

void dma_disable(DMA_CLASS dma)
{
	if (dma < DMA_COUNT)
	{
		if (--_dma_enabled[dma] == 0)
			RCC->AHB1ENR &= ~RCC_DMA[dma];
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_UART, dma);
}

unsigned long dma_get_flags(DMA_CLASS dma, DMA_STREAM_CLASS stream)
{
	unsigned long res = 0;
	if (stream < DMA_STREAM_4)
		res = DMA[dma]->DMA.LISR;
	else
		res = DMA[dma]->DMA.HISR;
	res = (res >> DMA_SHIFT_MASK[(stream & 3)]) & 0x3f;
	return res;
}

void dma_clear_flags(DMA_CLASS dma, DMA_STREAM_CLASS stream, unsigned long flags)
{
	unsigned long res = (flags & 0x3f) << DMA_SHIFT_MASK[(stream & 3)];
	if (stream < DMA_STREAM_4)
		DMA[dma]->DMA.LIFCR = res;
	else
		DMA[dma]->DMA.HIFCR = res;
}
