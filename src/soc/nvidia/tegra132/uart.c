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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <boot/coreboot_tables.h>
#include <console/console.h>	/* for __console definition */
#include <console/uart.h>
#include <drivers/uart/uart8250reg.h>
#include <stdint.h>

/*
 * TODO: Use DRIVERS_UART_8250MEM driver instead.
 * There is an issue in the IO call functions where x86 and ARM
 * ordering is reversed. This 8250MEM driver uses the x86 convention.
 * This driver can be replaced once the IO calls are sorted.
 */
struct tegra132_uart {
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

static void tegra132_uart_tx_flush(struct tegra132_uart *uart_ptr);
static int tegra132_uart_tst_byte(struct tegra132_uart *uart_ptr);

static void tegra132_uart_init(struct tegra132_uart *uart_ptr)
{
	const uint8_t line_config = UART8250_LCR_WLS_8; // 8n1

	uint16_t divisor = (u16) uart_baudrate_divisor(default_baudrate(),
		uart_platform_refclk(), 16);

	tegra132_uart_tx_flush(uart_ptr);

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
	write8(&uart_ptr->fcr, UART8250_FCR_FIFO_EN |
	       UART8250_FCR_CLEAR_RCVR | UART8250_FCR_CLEAR_XMIT);
}

static unsigned char tegra132_uart_rx_byte(struct tegra132_uart *uart_ptr)
{
	if (!tegra132_uart_tst_byte(uart_ptr))
		return 0;
	return read8(&uart_ptr->rbr);
}

static void tegra132_uart_tx_byte(struct tegra132_uart *uart_ptr, unsigned char data)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_THRE));
	write8(&uart_ptr->thr, data);
}

static void tegra132_uart_tx_flush(struct tegra132_uart *uart_ptr)
{
	while (!(read8(&uart_ptr->lsr) & UART8250_LSR_TEMT));
}

static int tegra132_uart_tst_byte(struct tegra132_uart *uart_ptr)
{
	return (read8(&uart_ptr->lsr) & UART8250_LSR_DR) == UART8250_LSR_DR;
}

/* FIXME: Add mainboard override */
unsigned int uart_platform_refclk(void)
{
	return 408000000;
}

uintptr_t uart_platform_base(int idx)
{
	/* Default to UART A */
	unsigned int base = 0x70006000;
	/* UARTs A - E are mapped as index 0 - 4 */
	if ((idx < 5) && (idx >= 0)) {
		if (idx != 1) { /* Not UART B */
			base += idx * 0x100;
		} else {
			base += 0x40;
		}
	}
	return base;
}

void uart_init(int idx)
{
	struct tegra132_uart *uart_ptr = uart_platform_baseptr(idx);
	tegra132_uart_init(uart_ptr);
}

unsigned char uart_rx_byte(int idx)
{
	struct tegra132_uart *uart_ptr = uart_platform_baseptr(idx);
	return tegra132_uart_rx_byte(uart_ptr);
}

void uart_tx_byte(int idx, unsigned char data)
{
	struct tegra132_uart *uart_ptr = uart_platform_baseptr(idx);
	tegra132_uart_tx_byte(uart_ptr, data);
}

void uart_tx_flush(int idx)
{
	struct tegra132_uart *uart_ptr = uart_platform_baseptr(idx);
	tegra132_uart_tx_flush(uart_ptr);
}

#if ENV_RAMSTAGE
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = uart_platform_base(CONFIG_UART_FOR_CONSOLE);
	serial.baud = default_baudrate();
	serial.regwidth = 1;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
