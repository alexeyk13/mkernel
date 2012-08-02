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

#include "uart.h"
#include "arch.h"
#include "hw_config.h"
#include "profile.h"
#include "gpio.h"
#include "rcc.h"
#include "irq.h"
#include "dbg.h"
#include "mem_private.h"
#include "error.h"

#if defined(STM32F1)
#include "rcc_stm32f1.h"
#include "gpio_stm32f1.h"
#elif defined(STM32F2)
#include "rcc_stm32f2.h"
#include "gpio_stm32f2.h"
#elif defined(STM32F4)
#include "rcc_stm32f4.h"
#include "gpio_stm32f4.h"
#endif

typedef USART_TypeDef* USART_TypeDef_P;

const USART_TypeDef_P USART[]							= {USART1, USART2, USART3, UART4, UART5, USART6};
const GPIO_CLASS UART_TX_PINS[]						= {USART1_TX_PIN, USART2_TX_PIN, USART3_TX_PIN, UART4_TX_PIN, GPIO_C12, USART6_TX_PIN};
const GPIO_CLASS UART_RX_PINS[]						= {USART1_RX_PIN, USART2_RX_PIN, USART3_RX_PIN, UART4_RX_PIN, GPIO_D2, USART6_RX_PIN};

#define UART_ERROR_MASK									0xf

#define DISABLE_RECEIVER()								USART[port]->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE)
#define ENABLE_RECEIVER()								USART[port]->CR1 |= (USART_CR1_RE | USART_CR1_RXNEIE)
#define ENABLE_TRANSMITTER()							USART[port]->CR1 |= USART_CR1_TE
#define DISABLE_TRANSMITTER()							USART[port]->CR1 &= ~(USART_CR1_TE)

const uint32_t RCC_UART[] =							{RCC_APB2ENR_USART1EN, RCC_APB1ENR_USART2EN, RCC_APB1ENR_USART3EN, RCC_APB1ENR_UART4EN, RCC_APB1ENR_UART5EN, RCC_APB2ENR_USART6EN};
const IRQn_Type UART_IRQ_VECTORS[] =				{USART1_IRQn, USART2_IRQn, USART3_IRQn, UART4_IRQn, UART5_IRQn, USART6_IRQn};
const int USART_RX_DISABLE_MASK =					USART_RX_DISABLE_MASK_DEF;
const int USART_TX_DISABLE_MASK =					USART_TX_DISABLE_MASK_DEF;

typedef struct {
	UART_CB* cb;
	char* read_buf;
	char* write_buf;
	unsigned int read_size, write_size;
	void* param;
	bool isr_active;
}UART_HW;

static UART_HW* _uart_handlers[UARTS_COUNT] __attribute__ ((section (".sys_bss"))) =			{NULL};

void uart_on_isr(UART_CLASS port)
{
	if (_uart_handlers[port])
	{
		_uart_handlers[port]->isr_active = true;
		uint16_t sr = USART[port]->SR;
		//slave: transmit more
		if ((sr & USART_SR_TXE) && _uart_handlers[port]->write_size)
		{
			USART[port]->DR = _uart_handlers[port]->write_buf[0];
			_uart_handlers[port]->write_buf++;
			_uart_handlers[port]->write_size--;
			if (_uart_handlers[port]->write_size == 0)
			{
				_uart_handlers[port]->write_buf = NULL;
				if (_uart_handlers[port]->cb->on_write_complete)
					_uart_handlers[port]->cb->on_write_complete(_uart_handlers[port]->param);
			}
			//no more
			if (_uart_handlers[port]->write_size == 0)
			{
				USART[port]->CR1 &= ~USART_CR1_TXEIE;
				USART[port]->CR1 |= USART_CR1_TCIE;
			}
		}
		//transmission completed and no more data
		else if ((sr & USART_SR_TC) && _uart_handlers[port]->write_size == 0)
			USART[port]->CR1 &= ~(USART_CR1_TE | USART_CR1_TCIE);
		//decode error, if any
		else if ((sr & UART_ERROR_MASK))
		{
			UART_ERROR error = UART_ERROR_OK;
			if (sr & USART_SR_ORE)
				error = UART_ERROR_OVERRUN;
			else
			{
				__REG_RC32(USART[port]->DR);
				if (sr & USART_SR_FE)
					error = UART_ERROR_FRAME;
				else if (sr & USART_SR_PE)
					error = UART_ERROR_PARITY;
				else if  (sr & USART_SR_NE)
					error = UART_ERROR_NOISE;
			}
			if (_uart_handlers[port]->cb->on_error)
				_uart_handlers[port]->cb->on_error(_uart_handlers[port]->param, error);
		}
		//slave: receive data
		else if (sr & USART_SR_RXNE && _uart_handlers[port]->read_size)
		{
			_uart_handlers[port]->read_buf[0] = USART[port]->DR;
			_uart_handlers[port]->read_buf++;
			if (--_uart_handlers[port]->read_size == 0)
			{
				_uart_handlers[port]->read_buf = NULL;
				if (_uart_handlers[port]->cb->on_read_complete)
					_uart_handlers[port]->cb->on_read_complete(_uart_handlers[port]->param);
			}
			//no more, disable receiver
			if (_uart_handlers[port]->read_size == 0)
				USART[port]->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE);
		}
		else if (sr & USART_SR_RXNE)
		{
			__REG_RC32(USART[port]->DR);
			USART[port]->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE);
		}
	}
}

void USART1_IRQHandler(void)
{
	uart_on_isr(UART_1);
}

void USART2_IRQHandler(void)
{
	uart_on_isr(UART_2);
}

void USART3_IRQHandler(void)
{
	uart_on_isr(UART_3);
}

void UART4_IRQHandler(void)
{
	uart_on_isr(UART_4);
}

void UART5_IRQHandler(void)
{
	uart_on_isr(UART_5);
}

void USART6_IRQHandler(void)
{
	uart_on_isr(UART_6);
}

void uart_read(UART_CLASS port, char* buf, int size)
{
	//must be handled be upper layer
	ASSERT(_uart_handlers[port]->read_size == 0);

	_uart_handlers[port]->read_buf = buf;
	_uart_handlers[port]->read_size = size;

	USART[port]->CR1 |= (USART_CR1_RE | USART_CR1_RXNEIE);
}

void uart_read_cancel(UART_CLASS port)
{
	_uart_handlers[port]->read_size = 0;
	_uart_handlers[port]->read_buf = NULL;
	USART[port]->CR1 &= ~(USART_CR1_RE | USART_CR1_RXNEIE);
}

void uart_write_wait(UART_CLASS port)
{
	_uart_handlers[port]->isr_active = false;
	while (_uart_handlers[port]->write_size != 0)
	{
		while ((USART[port]->SR & USART_SR_TXE) == 0) {}
		CRITICAL_ENTER;
		//not handled in isr, do it manually
		if (!_uart_handlers[port]->isr_active)
		{
			USART[port]->DR = _uart_handlers[port]->write_buf[0];
			_uart_handlers[port]->write_buf++;
			_uart_handlers[port]->write_size--;
		}
		CRITICAL_LEAVE;
	}
	if (!_uart_handlers[port]->isr_active)
	{
		CRITICAL_ENTER;
		_uart_handlers[port]->write_buf = NULL;
		CRITICAL_LEAVE;
		if (_uart_handlers[port]->cb->on_write_complete)
			_uart_handlers[port]->cb->on_write_complete(_uart_handlers[port]->param);

		//no more data? wait for completion and turn TX off
		while (_uart_handlers[port]->write_size == 0)
		{
			while ((USART[port]->SR & USART_SR_TC) == 0) {}
			CRITICAL_ENTER_AGAIN;
			if ((USART[port]->SR & USART_SR_TC) && _uart_handlers[port]->write_size == 0)
				USART[port]->CR1 &= ~(USART_CR1_TE | USART_CR1_TXEIE);
			CRITICAL_LEAVE;
		}
	}
}

void uart_write(UART_CLASS port, char* buf, int size)
{
	//must be handled be upper layer
	ASSERT(_uart_handlers[port]->write_size == 0);

	_uart_handlers[port]->write_buf = buf;
	_uart_handlers[port]->write_size = size;

	USART[port]->CR1 &= ~USART_CR1_TCIE;
	USART[port]->CR1 |= (USART_CR1_TE | USART_CR1_TXEIE);
}

extern void uart_enable(UART_CLASS port, UART_CB *cb, void *param, int priority)
{
	if (port < UARTS_COUNT)
	{
		UART_HW* uart = (UART_HW*)sys_alloc(sizeof(UART_HW));
		if (uart)
		{
			_uart_handlers[port] = uart;
			uart->cb = cb;
			uart->param = param;
			uart->read_buf = NULL;
			uart->write_buf = NULL;
			uart->read_size = 0;
			uart->write_size = 0;
			//setup pins
			if ((USART_TX_DISABLE_MASK & (1 << port)) == 0)
				gpio_enable_afio(UART_TX_PINS[port], port < UART_4 ? AFIO_MODE_USART1_2_3 : AFIO_MODE_UART_4_5_USART_6, AFIO_PULL_UP);
			if ((USART_RX_DISABLE_MASK & (1 << port)) == 0)
				gpio_enable_afio(UART_RX_PINS[port], port < UART_4 ? AFIO_MODE_USART1_2_3 : AFIO_MODE_UART_4_5_USART_6, AFIO_PULL_UP);

			//power up
			if (port == UART_1 || port == UART_6)
				RCC->APB2ENR |= RCC_UART[port];
			else
				RCC->APB1ENR |= RCC_UART[port];

			//enable interrupts
			NVIC_EnableIRQ(UART_IRQ_VECTORS[port]);
			NVIC_SetPriority(UART_IRQ_VECTORS[port], priority);

			USART[port]->CR1 |= USART_CR1_UE;
		}
		else
			error_dev(ERROR_MEM_OUT_OF_SYSTEM_MEMORY, DEV_UART, port);
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_UART, port);
}

void uart_disable(UART_CLASS port)
{
	if (port < UARTS_COUNT)
	{
		//disable interrupts
		NVIC_DisableIRQ(UART_IRQ_VECTORS[port]);

		//disable core
		USART[port]->CR1 &= ~USART_CR1_UE;
		//power down
		if (port == UART_1 || port == UART_6)
			RCC->APB2ENR &= ~RCC_UART[port];
		else
			RCC->APB1ENR &= ~RCC_UART[port];

		//disable pins
		if ((USART_TX_DISABLE_MASK & (1 << port)) == 0)
			gpio_disable_pin(UART_TX_PINS[port]);
		if ((USART_RX_DISABLE_MASK & (1 << port)) == 0)
			gpio_disable_pin(UART_RX_PINS[port]);

		sys_free(_uart_handlers[port]);
		_uart_handlers[port] = NULL;
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_UART, port);
}

void uart_set_baudrate(UART_CLASS port, const UART_BAUD* config)
{
	if (port < UARTS_COUNT)
	{
		USART[port]->CR1 &= ~USART_CR1_UE;

		if (config->data_bits == 8 && config->parity != 'N')
			USART[port]->CR1 |= USART_CR1_M;
		else
			USART[port]->CR1 &= ~USART_CR1_M;

		if (config->parity != 'N')
		{
			USART[port]->CR1 |= USART_CR1_PCE;
			if (config->parity == 'O')
				USART[port]->CR1 |= USART_CR1_PS;
			else
				USART[port]->CR1 &= ~USART_CR1_PS;
		}
		else
			USART[port]->CR1 &= ~USART_CR1_PCE;

		USART[port]->CR2 = (config->stop_bits == 1 ? 0 : 2) << 12;
		USART[port]->CR3 = 0;

		unsigned int bus_freq;
		if (port == UART_1 || port == UART_6)
			bus_freq = _apb2_freq;
		else
			bus_freq = _apb1_freq;
		unsigned int mantissa, fraction;
		mantissa = (25 * bus_freq) / (4 * (config->baud));
		fraction = ((mantissa % 100) * 8 + 25)  / 50;
		mantissa = mantissa / 100;
		USART[port]->BRR = (mantissa << 4) | fraction;

		USART[port]->CR1 |= USART_CR1_UE | USART_CR1_PEIE;
		USART[port]->CR3 |= USART_CR3_EIE;

		USART[port]->CR1 |= USART_CR1_TE;
	}
	else
		error_dev(ERROR_DEVICE_INDEX_OUT_OF_RANGE, DEV_UART, port);
}
