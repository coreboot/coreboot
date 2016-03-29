/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

/*
 * TODO: Use DRIVERS_UART_8250MEM driver instead.
 * There is an issue in the IO call functions where x86 and ARM
 * ordering is reversed. This 8250MEM driver uses the x86 convention.
 * This driver can be replaced once the IO calls are sorted.
 */

struct rk_uart {
	union {
		uint32_t thr; /* Transmit holding register. */
		uint32_t rbr; /* Receive buffer register. */
		uint32_t dll; /* Divisor latch lsb. */
	};
	union {
		uint32_t ier; /* Interrupt enable register. */
		uint32_t dlm; /* Divisor latch msb. */
	};
	union {
		uint32_t iir; /* Interrupt identification register. */
		uint32_t fcr; /* FIFO control register. */
	};
	uint32_t lcr; /* Line control register. */
	uint32_t mcr; /* Modem control register. */
	uint32_t lsr; /* Line status register. */
	uint32_t msr; /* Modem status register. */
	uint32_t scr;
	uint32_t reserved1[(0x30 - 0x20) / 4];
	uint32_t srbr[(0x70 - 0x30) / 4];
	uint32_t far;
	uint32_t tfr;
	uint32_t rfw;
	uint32_t usr;
	uint32_t tfl;
	uint32_t rfl;
	uint32_t srr;
	uint32_t srts;
	uint32_t sbcr;
	uint32_t sdmam;
	uint32_t sfe;
	uint32_t srt;
	uint32_t stet;
	uint32_t htx;
	uint32_t dmasa;
	uint32_t reserver2[(0xf4 - 0xac) / 4];
	uint32_t cpr;
	uint32_t ucv;
	uint32_t ctr;
} __attribute__ ((packed));


static struct rk_uart * const uart_ptr =
	(void *)CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

static void rk_uart_tx_flush(void);
static int rk_uart_tst_byte(void);

static void rk_uart_init(void)
{
	/* FIXME: Use a hardcoded divisor for now.
	 * uint16_t divisor = (u16) uart_baudrate_divisor(default_baudrate(),
	 *	uart_platform_refclk(), 16)
	 */
	const unsigned divisor = 13;
	const uint8_t line_config = UART8250_LCR_WLS_8; // 8n1

	rk_uart_tx_flush();

	// Disable interrupts.
	write32(&uart_ptr->ier, 0);
	// Force DTR and RTS to high.
	write32(&uart_ptr->mcr, UART8250_MCR_DTR | UART8250_MCR_RTS);
	// Set line configuration, access divisor latches.
	write32(&uart_ptr->lcr, UART8250_LCR_DLAB | line_config);
	// Set the divisor.
	write32(&uart_ptr->dll, divisor & 0xff);
	write32(&uart_ptr->dlm, (divisor >> 8) & 0xff);
	// Hide the divisor latches.
	write32(&uart_ptr->lcr, line_config);
	// Enable FIFOs, and clear receive and transmit.
	write32(&uart_ptr->fcr, UART8250_FCR_FIFO_EN |
		UART8250_FCR_CLEAR_RCVR | UART8250_FCR_CLEAR_XMIT);
}

static void rk_uart_tx_byte(unsigned char data)
{
	while (!(read32(&uart_ptr->lsr) & UART8250_LSR_THRE));
	write32(&uart_ptr->thr, data);
}

static void rk_uart_tx_flush(void)
{
	while (!(read32(&uart_ptr->lsr) & UART8250_LSR_TEMT));
}

static unsigned char rk_uart_rx_byte(void)
{
	if (!rk_uart_tst_byte())
		return 0;
	return read32(&uart_ptr->rbr);
}

static int rk_uart_tst_byte(void)
{
	return (read32(&uart_ptr->lsr) & UART8250_LSR_DR) == UART8250_LSR_DR;
}



void uart_init(int idx)
{
	rk_uart_init();
}

unsigned char uart_rx_byte(int idx)
{
	return rk_uart_rx_byte();
}

void uart_tx_byte(int idx, unsigned char data)
{
	rk_uart_tx_byte(data);
}

void uart_tx_flush(int idx)
{
	rk_uart_tx_flush();
}

#ifndef __PRE_RAM__
void uart_fill_lb(void *data)
{
	struct lb_serial serial;
	serial.type = LB_SERIAL_TYPE_MEMORY_MAPPED;
	serial.baseaddr = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;
	serial.baud = default_baudrate();
	serial.regwidth = 4;
	lb_add_serial(&serial, data);

	lb_add_console(LB_TAG_CONSOLE_SERIAL8250MEM, data);
}
#endif
