/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/uart.h>
#include <console/console.h>
#include <drivers/uart/uart8250reg.h>
#include <boot/coreboot_tables.h>
#include <stdint.h>
#include <assert.h>
#include <soc/common.h>
#include <soc/clock.h>

struct armada38x_uart {
	union {
		uint32_t thr; // Transmit holding register.
		uint32_t rbr; // Receive buffer register.
		uint32_t dll; // Divisor latch lsb.
	};
	union {
		uint32_t ier; // Interrupt enable register.
		uint32_t dlm; // Divisor latch msb.
	};
	union {
		uint32_t iir; // Interrupt identification register.
		uint32_t fcr; // FIFO control register.
	};
	uint32_t lcr; // Line control register.
	uint32_t mcr; // Modem control register.
	uint32_t lsr; // Line status register.
	uint32_t msr; // Modem status register.
} __attribute__ ((packed));

static void armada38x_uart_tx_flush(struct armada38x_uart *uart_ptr);
static int armada38x_uart_tst_byte(struct armada38x_uart *uart_ptr);

static void armada38x_uart_init(struct armada38x_uart *uart_ptr)
{
	const uint8_t line_config = UART8250_LCR_WLS_8;
	uint16_t divisor = (u16) uart_baudrate_divisor(default_baudrate(),
		uart_platform_refclk(), 16);

	armada38x_uart_tx_flush(uart_ptr);
	// Disable interrupts.
	write8(&uart_ptr->ier, 0);
	// Enable access to divisor latches.
	write8(&uart_ptr->lcr, UART8250_LCR_DLAB);
	// Set the divisor.
	write8(&uart_ptr->dll, divisor & 0xff);
	write8(&uart_ptr->dlm, (divisor >> 8) & 0xff);
	// Hide divisor latches and program line config.
	write8(&uart_ptr->lcr, line_config);
	// Enable FIFOs, and clear receive and transmit.
	write8(&uart_ptr->fcr, UART8250_FCR_FIFO_EN | UART8250_FCR_CLEAR_RCVR |
				   UART8250_FCR_CLEAR_XMIT);
}

static void armada38x_uart_tx_byte(struct armada38x_uart *uart_ptr,
					unsigned char data)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_THRE))
		;
	write8(&uart_ptr->thr, data);
}

static void armada38x_uart_tx_flush(struct armada38x_uart *uart_ptr)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_TEMT))
		;
}

static unsigned char armada38x_uart_rx_byte(struct armada38x_uart *uart_ptr)
{
	if (!armada38x_uart_tst_byte(uart_ptr))
		return 0;
	return read8(&uart_ptr->rbr);
}

static int armada38x_uart_tst_byte(struct armada38x_uart *uart_ptr)
{
	return (read8(&uart_ptr->lsr) & UART8250_LSR_DR) == UART8250_LSR_DR;
}

unsigned int uart_platform_refclk(void)
{
	return mv_tclk_get();
}

uintptr_t uart_platform_base(int idx)
{
	/* Default to UART 0 */
	unsigned int base = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

	assert((idx >= 0) && (idx < 2));
	base += idx * 0x100;
	return base;
}

void uart_init(int idx)
{
	struct armada38x_uart *uart_ptr = uart_platform_baseptr(idx);

	armada38x_uart_init(uart_ptr);
}

void uart_tx_byte(int idx, unsigned char data)
{
	struct armada38x_uart *uart_ptr = uart_platform_baseptr(idx);

	armada38x_uart_tx_byte(uart_ptr, data);
}

void uart_tx_flush(int idx)
{
	struct armada38x_uart *uart_ptr = uart_platform_baseptr(idx);

	armada38x_uart_tx_flush(uart_ptr);
}

unsigned char uart_rx_byte(int idx)
{
	struct armada38x_uart *uart_ptr = uart_platform_baseptr(idx);

	return armada38x_uart_rx_byte(uart_ptr);
}

#if ENV_RAMSTAGE
void uart_fill_lb(void *data)
{
	struct lb_serial serial;

	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial.baud = default_baudrate();
	serial.regwidth = 4;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
