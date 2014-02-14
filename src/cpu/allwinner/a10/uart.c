/*
 * Uart setup helpers for Allwinner SoCs
 *
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "uart.h"
#include <arch/io.h>
#include "drivers/uart/uart8250reg.h"

/**
 * \brief Configure line control settings for UART
 */
void a10_uart_configure(void *uart_base, u32 baud_rate, u8 data_bits,
			enum uart_parity parity, u8 stop_bits)
{
	u32 reg32;
	u16 div;
	struct a10_uart *uart = uart_base;

	/* Enable access to Divisor Latch register */
	write32(UART_LCR_DLAB, &uart->lcr);
	/* Set baudrate */
	/* FIXME: We assume clock is 24MHz, which may not be the case */
	div = 24000000 / 16 / baud_rate;
	write32((div >> 8) & 0xff, &uart->dlh);
	write32(div & 0xff, &uart->dll);
	/* Set line control */
	reg32 = (data_bits - 5) & UART_LCR_WLS_MSK;
	switch (parity) {
	case UART_PARITY_ODD:
		reg32 |= UART_LCR_PEN;
		break;
	case UART_PARITY_EVEN:
		reg32 |= UART_LCR_PEN;
		reg32 |= UART_LCR_EPS;
		break;
	case UART_PARITY_NONE:	/* Fall through */
	default:
		break;
	}
	write32(reg32, &uart->lcr);
}

void a10_uart_enable_fifos(void *uart_base)
{
	struct a10_uart *uart = uart_base;

	write32(UART_FCR_FIFO_EN, &uart->fcr);
}

static int tx_fifo_full(struct a10_uart *uart)
{
	/* This may be a misnomer, or a typo in the datasheet. THRE indicates
	 * that the TX register is empty, not that the FIFO is not full, but
	 * this may be due to a datasheet typo. Keep the current name to signal
	 * intent. */
	return !(read32(&uart->lsr) & UART_LSR_THRE);
}

static int rx_fifo_empty(struct a10_uart *uart)
{
	return !(read32(&uart->lsr) & UART_LSR_DR);
}

/**
 * \brief Read a single byte from the UART.
 *
 * Blocks until at least a byte is available.
 */
u8 a10_uart_rx_blocking(void *uart_base)
{
	struct a10_uart *uart = uart_base;

	while (rx_fifo_empty(uart)) ;

	return read32(&uart->rbr);
}

/**
 * \brief Write a single byte to the UART.
 *
 * Blocks until there is space in the FIFO.
 */
void a10_uart_tx_blocking(void *uart_base, u8 data)
{
	struct a10_uart *uart = uart_base;

	while (tx_fifo_full(uart)) ;

	return write32(data, &uart->thr);
}
