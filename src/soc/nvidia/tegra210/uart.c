/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Samsung Electronics
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
#include <boot/coreboot_tables.h>
#include <console/console.h>	/* for __console definition */
#include <console/uart.h>
#include <drivers/uart/uart8250reg.h>
#include <stdint.h>
#include <compiler.h>

struct tegra210_uart {
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
} __packed;


static struct tegra210_uart * const uart_ptr =
	(void *)CONFIG_CONSOLE_SERIAL_TEGRA210_UART_ADDRESS;

static void tegra210_uart_tx_flush(void);
static int tegra210_uart_tst_byte(void);

static void tegra210_uart_init(void)
{
	// Use a hardcoded divisor for now.
	const unsigned divisor = 221;
	const uint8_t line_config = UART8250_LCR_WLS_8; // 8n1

	tegra210_uart_tx_flush();

	// Disable interrupts.
	write8(&uart_ptr->ier, 0);
	// Force DTR and RTS to high.
	write8(&uart_ptr->mcr, UART8250_MCR_DTR | UART8250_MCR_RTS);
	// Set line configuration, access divisor latches.
	write8(&uart_ptr->lcr, UART8250_LCR_DLAB | line_config);
	// Set the divisor.
	write8(&uart_ptr->dll, divisor & 0xff);
	write8(&uart_ptr->dlm, (divisor >> 8) & 0xff);
	// Hide the divisor latches.
	write8(&uart_ptr->lcr, line_config);
	// Enable FIFOs, and clear receive and transmit.
	write8(&uart_ptr->fcr,
		UART8250_FCR_FIFO_EN |
		UART8250_FCR_CLEAR_RCVR |
		UART8250_FCR_CLEAR_XMIT);
}

static void tegra210_uart_tx_byte(unsigned char data)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_THRE));
	write8(&uart_ptr->thr, data);
}

static void tegra210_uart_tx_flush(void)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_TEMT));
}

static unsigned char tegra210_uart_rx_byte(void)
{
	if (!tegra210_uart_tst_byte())
		return 0;
	return read8(&uart_ptr->rbr);
}

static int tegra210_uart_tst_byte(void)
{
	return (read8(&uart_ptr->lsr) & UART8250_LSR_DR) == UART8250_LSR_DR;
}

void uart_init(int idx)
{
	tegra210_uart_init();
}

void uart_tx_byte(int idx, unsigned char data)
{
	tegra210_uart_tx_byte(data);
}

void uart_tx_flush(int idx)
{
	tegra210_uart_tx_flush();
}

unsigned char uart_rx_byte(int idx)
{
	return tegra210_uart_rx_byte();
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = CONFIG_CONSOLE_SERIAL_TEGRA210_UART_ADDRESS;
	serial.baud = get_uart_baudrate();
	serial.regwidth = 4;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
